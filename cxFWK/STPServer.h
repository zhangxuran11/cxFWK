#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H
#include <vector>
#include <map>
#include "Json.h"
#include <boost/date_time.hpp>
#include <boost/foreach.hpp>

#include <boost/asio.hpp>
#include <boost/asio/impl/use_future.hpp>
#include <boost/asio/system_executor.hpp>
#include <boost/exception/all.hpp>

#include <string>
#include <iostream>
#include <memory>
#include <functional>

#include "STPClient.h"
namespace cxFWK{
    class STPServer{
        class Session
        : public std::enable_shared_from_this<Session>{
            public:
                Session(STPServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mSocket(std::move(socket)){
                    //std::cout<<"Session()"<<std::endl;
                }
                ~Session(){
                    //std::cout<<"~Session()"<<std::endl;
                }
                void run(){
                    doRead();
                }
            private:
                void doRead(){
                    auto self = shared_from_this();
                    mRecvBuffer.resize(4);
                    boost::asio::async_read(mSocket,boost::asio::buffer(mRecvBuffer, mRecvBuffer.size()),
                    [this, self](boost::system::error_code ec, std::size_t length)
                    {
                        if (!ec && length > 0)
                        {
                            std::uint32_t len = * reinterpret_cast<std::uint32_t*>(mRecvBuffer.data());
                            mRecvBuffer.resize(len);
                            auto self = shared_from_this();
                            boost::asio::async_read(mSocket,boost::asio::buffer(mRecvBuffer, mRecvBuffer.size()),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                                if (!ec && length > 0){
                                    Json json(std::string(mRecvBuffer.data(),mRecvBuffer.size()));
                                    std::string cmd;
                                    if(json.hasKey("topic")){
                                        cmd = json.get("topic","");
                                    }
                                    else {
                                        std::cout<<"no topic key"<<std::endl;
                                    }
                                    Json res;
                                    if(mServer.mProcesserSet.count(cmd) > 0){
                                        res.put("content",std::move(mServer.mProcesserSet[cmd](json.get("content",Json()))));
                                    }
                                    else{
                                        res.put("content",Json());
                                    }
                                    std::string res_str = res.toString();

                                    mSendBuffer.resize(res_str.size()+4);
                                    *reinterpret_cast<std::uint32_t*>(mSendBuffer.data()) = res_str.size();
                                    memcpy(mSendBuffer.data()+4,res_str.data(),res_str.size());
                                    boost::asio::async_write(mSocket,boost::asio::buffer(mSendBuffer.data(),mSendBuffer.size()),
                                        [this,self](boost::system::error_code ec, std::size_t ){
                                            if(!ec){
                                                doRead();
                                            }
                                            else{ //应答失败
                                            }
                                    });
                                }
                            });
                        }
                    });
                }
            private:
                STPServer& mServer;
                boost::asio::ip::tcp::socket mSocket;
                std::vector<char> mRecvBuffer;
                std::vector<char> mSendBuffer;
        };
        void doAccept(){
            mAcceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
            if (!ec)
            {
                std::make_shared<Session>(*this,socket)->run();
            }
            doAccept();
            });

        }

    public:
        STPServer(uint16_t port) :mAcceptor(mIoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
            mAcceptor.set_option(boost::asio::socket_base::reuse_address(true));
        }
        uint16_t port()const {
            return mAcceptor.local_endpoint().port();
        }
        void registerProcesser(const std::string& cmd,std::function<Json(const Json&)> processer){
            mProcesserSet[cmd] = processer;
        }
        void run(){
            doAccept();
            mIoContext.run();
        }
    private:
        boost::asio::io_context mIoContext;
        boost::asio::ip::tcp::acceptor mAcceptor;
        std::vector<char> mRecvBuffer;
        std::vector<char> mSendBuffer;
        std::map<std::string,std::function<Json (const Json&)> > mProcesserSet;
    };
}
#endif  //ECHO_SERVER_H

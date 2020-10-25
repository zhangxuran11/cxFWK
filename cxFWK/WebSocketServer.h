#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
namespace cxFWK{
    class WebSocketServer{
    public:
        class Session
        : public std::enable_shared_from_this<Session>{
            friend class WebSocketServer;
        public:
                Session(WebSocketServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mWs(std::move(socket)){
                    std::cout<<"Session()"<<std::endl;
                }

                ~Session(){
                    std::cout<<"~Session()"<<std::endl;
                }
                void write(std::string text){
                    auto self = shared_from_this();
                    auto boost::beast::bu
                    mWs.async_write(mRecvBuffer,
                        [self,this](boost::beast::error_code const& ec,std::size_t bytes_written){
                        if(!ec){
                            std::vector<uint8_t> buffer(bytes_written);
                            boost::asio::buffer_copy(boost::asio::mutable_buffer(buffer.data(),bytes_written),mRecvBuffer.data());
                            mServer.mOnread(*this,buffer.data(), bytes_written);
                            doRead();
                        }else{
                            mServer.mOnDisConnected(*this);
                        }
                    });

                }
        private:
                void run()
                {
                    // Accept the websocket handshake
                    auto self = shared_from_this();
                    mWs.async_accept([self,this](boost::beast::error_code const& ec){
                        if(!ec){
                            mServer.mOnConnected(*this);
                            doRead();
                        }

                    });
                }
                void doRead(){
                    auto self = shared_from_this();
                    mWs.async_read(mRecvBuffer,
                        [self,this](boost::beast::error_code const& ec,std::size_t bytes_written){
                        if(!ec){
                            std::vector<uint8_t> buffer(bytes_written);
                            boost::asio::buffer_copy(boost::asio::mutable_buffer(buffer.data(),bytes_written),mRecvBuffer.data());
                            mServer.mOnread(*this,buffer.data(), bytes_written);
                            doRead();
                        }else{
                            mServer.mOnDisConnected(*this);
                        }
                    });
                }
            private:
                WebSocketServer& mServer;
                
                boost::beast::websocket::stream<boost::asio::ip::tcp::socket> mWs;
                boost::beast::multi_buffer mRecvBuffer;
                std::vector<uint8_t> mSendBuffer;
        };


        WebSocketServer(uint16_t port):mAcceptor(mIoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),mOnread([](Session&,uint8_t* ,size_t  ){}),
                mOnConnected([](Session&  ){}),mOnDisConnected([](Session&){}){
                mAcceptor.set_option(boost::asio::socket_base::reuse_address(true));
            }
        void run(){
            doAccept();
            mIoContext.run();
        }
        void setOnRead(std::function<void(Session&,uint8_t* ,size_t  )> onRead){
            mOnread = std::move(onRead);
        }
        void setOnConnected(std::function<void(Session& )> onConnected){
            mOnConnected = std::move(onConnected);
        }
        void setOnDisConnected(std::function<void(Session& )> onDisConnected){
            mOnDisConnected = std::move(onDisConnected);
        }
    private:
        void doAccept(){
            mAcceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
            {
            if (!ec)
            {
                std::cout<<"00000"<<std::endl;
                std::make_shared<Session>(*this,socket)->run();
            }
            doAccept();
            });
        }
        boost::asio::io_context mIoContext;
        boost::asio::ip::tcp::acceptor mAcceptor;
        std::function<void(Session&,uint8_t* ,size_t  )> mOnread;
        std::function<void(Session& )> mOnConnected;
        std::function<void(Session& )> mOnDisConnected;

    };

}

#endif//WEBSOCKET_SERVER_H

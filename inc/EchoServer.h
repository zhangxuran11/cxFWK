#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
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

class EchoServer{
    boost::asio::ip::tcp::acceptor mAcceptor;
    boost::asio::io_context mIoContext;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ip::tcp::endpoint mSenderEndpoint;
    std::vector<char> mRecvBuffer;
    std::vector<char> mSendBuffer;
    std::map<std::string,std::function<void(const boost::property_tree::ptree&)> > mProcesserSet;
    void doReceive()
     {
       mSocket.async_receive_from(
           boost::asio::buffer(mRecvBuffer.data(), mRecvBuffer.size()-1), mSenderEndpoint,
           [this](boost::system::error_code ec, std::size_t bytes_recvd)
           {
             if (!ec && bytes_recvd > 0)
             {
                 boost::property_tree::ptree pt;
                 mRecvBuffer[bytes_recvd] = '\0';
                 std::stringstream sstream(mRecvBuffer.data());
                 boost::property_tree::json_parser::read_json(sstream, pt);
                 try{
                     std::string cmd = pt.get<std::string>("topic");
                     if(mProcesserSet.count(cmd) > 0){
                         mProcesserSet[cmd](pt);
                     }
                 }
                 catch (boost::exception& e){
                     std::cout<<boost::diagnostic_information(e)<<std::endl;
                 }
             }
             doReceive();
           });
     }

//     void do_send(std::size_t length)
//     {
//       socket_.async_send_to(
//           boost::asio::buffer(data_, length), sender_endpoint_,
//           [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
//           {
//             do_receive();

//           });
//     }

    void doReadHead(std::shared_ptr<boost::asio::ip::tcp::socket> shared_socket,std::shared_ptr<std::vector> shared_buff){
        shared_socket->async_receive(boost::asio::buffer(*shared_buff, shared_buff->size()),
            [this, shared_socket,shared_buff](boost::system::error_code ec, std::size_t length)
            {
                if (!ec && length > 0)
                {
                    std::uint32_t len = *static_cast<std::uint32_t*>(shared_buff->data());
                    shared_buff->resize(len);
                    shared_socket->async_receive(boost::asio::buffer(*shared_buff, shared_buff->size()),
                    [this,](shared_socket,shared_buff)(boost::system::error_code ec, std::size_t length)
                    {
                        if (!ec && length > 0){
                            boost::property_tree::ptree pt;
                            std::stringstream sstream(std::string(shared_buff->data(),shared_buff->size()));
                            boost::property_tree::json_parser::read_json(sstream, pt);

                        }

                    });
                    
                }
            });
        );
    }
    using boost::asio::ip::tcp;

class Session
  : public std::enable_shared_from_this<Session>{
      public:
        Session(boost::asio::ip::tcp::socket& socket):mSocket(std::move(socket)),mRecvBuffer(4){}
        void doRead(){
            mSocket.async_receive(boost::asio::buffer(mRecvBuffer, mRecvBuffer.size()),
            [this, auto self = std::shared_from_this()](boost::system::error_code ec, std::size_t length)
            {
                if (!ec && length > 0)
                {
                    std::uint32_t len = *static_cast<std::uint32_t*>(mRecvBuffer.data());
                    mRecvBuffer.resize(len);
                    mSocket.async_receive(boost::asio::buffer(mRecvBuffer, mRecvBuffer.size()),
                    [this,auto self = std::shared_from_this()](shared_socket,shared_buff)(boost::system::error_code ec, std::size_t length)
                    {
                        if (!ec && length > 0){
                            boost::property_tree::ptree pt;
                            std::stringstream sstream(std::string(mRecvBuffer.data(),mRecvBuffer.size()));
                            boost::property_tree::json_parser::read_json(sstream, pt);
                            try{
                                std::string cmd = pt.get<std::string>("topic");
                                if(mProcesserSet.count(cmd) > 0){
                                    mProcesserSet[cmd](pt);
                                }
                            }
                            catch (boost::exception& e){
                                std::cout<<boost::diagnostic_information(e)<<std::endl;
                            }
                        }

                    });
                    
                }
            });
        );

        }
      private:
        boost::asio::ip::tcp::socket mSocket;
        std::vector<char> mRecvBuffer;
  };
    void doAccept(){
        mAcceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
          if (!ec)
          {
              std::make_shared<Session>(socket)->doRead();
              //doReadHead(std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket)),std::make_shared<std::vecotor>(4));
          }

          doAccept();
        });

    }

public:
    EchoServer(uint16_t port) :mAcceptor(mIoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) {
        doAccept();
        //doReceive();
    }
    void registerProcesser(const std::string& cmd,std::function<void(const boost::property_tree::ptree&)> processer){

        mProcesserSet[cmd] = processer;

    }
    void sendResponse(const boost::property_tree::ptree& pt){
        std::stringstream ss;
        boost::property_tree::write_json(ss, pt);
        mSendBuffer.resize(ss.str().size()+1);
        strcpy(mSendBuffer.data(),ss.str().c_str());
        mSocket.send_to(boost::asio::buffer(mSendBuffer.data(), strlen(mSendBuffer.data())-1), mSenderEndpoint);

//        mSocket.async_send_to(
//                   boost::asio::buffer(mSendBuffer.data(), strlen(mSendBuffer.data())-1), mServerEndpoint,
//                   [this](boost::system::error_code ec, std::size_t bytes_sent)
//                   {
//                        std::cout<<ec.message()<<":"<<bytes_sent<<std::endl;
//                     //do_receive();

//                   });
    }
    void run(){
        mIoContext.run();
    }
};
#endif//ECHO_SERVER_H
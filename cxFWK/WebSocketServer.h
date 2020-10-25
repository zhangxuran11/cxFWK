#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <list>
namespace cxFWK{
    class WebSocketServer{
    public:
        class Session
        : public std::enable_shared_from_this<Session>{
            friend class WebSocketServer;
            enum MessageFormat{
                Text,
                Binary
            };
            struct Message{
                MessageFormat format;
                std::vector<uint8_t> buffer;
                Message(const std::string& text):format(Text),buffer(text.begin(),text.end()){
                }
                Message(const uint8_t* data,size_t len):format(Binary),buffer(len){
                    memcpy(buffer.data(),data,len);
                }
            };
            void write(void){
                Message* msg = mMessageQueue.front();
                auto self = shared_from_this();
                if(msg->format == Text){
                    mWs.text(true);
                }
                else{//binary
                    mWs.binary(true);
                }
                mWs.async_write(boost::asio::buffer(msg->buffer.data(),msg->buffer.size()),
                    [self,this](boost::beast::error_code const& ec,std::size_t ){
                    if(!ec){
                        Message* msg = mMessageQueue.front();
                        delete msg;
                        mMessageQueue.pop_front();
                        if(mMessageQueue.size() > 0)
                            write();

                    }else{
                        mServer.mOnDisConnected(*this);
                    }
                });

            }

        public:
                Session(WebSocketServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mWs(std::move(socket)){
                    std::cout<<"Session()"<<std::endl;
                }

                ~Session(){
                    std::cout<<"~Session()"<<std::endl;
                    while(auto msg = mMessageQueue.front()){
                        delete msg;
                        mMessageQueue.pop_front();
                    }
                }
                void write(const std::string& text){

                    mMessageQueue.push_back(new Message(text));
                    if(mMessageQueue.size() == 1){
                        write();
                    }
                }
                void write(const uint8_t* data,size_t len){
                    mMessageQueue.push_back(new Message(data,len));
                    if(mMessageQueue.size() == 1){
                        write();
                    }
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
                std::list<Message*> mMessageQueue;
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

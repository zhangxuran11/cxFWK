#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>

namespace cxFWK{
    class WebSocketServer{
    public:
        class Session
        : public std::enable_shared_from_this<Session>{
            friend class WebSocketServer;
        public:
                Session(WebSocketServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mWs(std::move(socket)){
                    //std::cout<<"Session()"<<std::endl;
                }

                ~Session(){
                    //std::cout<<"~Session()"<<std::endl;
                }
        private:
                void run()
                {
                    // Accept the websocket handshake
                    auto self = shared_from_this();
                    mWs.async_accept([self,this](boost::beast::error_code const& ec){
                        if(!ec)
                            doRead();
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
                        }
                    });
                }
            private:
                WebSocketServer& mServer;
                
                boost::beast::websocket::stream<boost::asio::ip::tcp::socket> mWs;
                boost::beast::multi_buffer mRecvBuffer;
                std::vector<char> mSendBuffer;
        };


        WebSocketServer(uint16_t port):mAcceptor(mIoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),mOnread([](Session&,uint8_t* ,size_t  ){}){
                mAcceptor.set_option(boost::asio::socket_base::reuse_address(true));
            }
        void run(){
            doAccept();
            mIoContext.run();
        }
        void setOnRead(std::function<void(Session&,uint8_t* ,size_t  )> onRead){
            mOnread = std::move(onRead);
        }
    private:
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
        boost::asio::io_context mIoContext;
        boost::asio::ip::tcp::acceptor mAcceptor;
        std::function<void(Session&,uint8_t* ,size_t  )> mOnread;

    };

}

#endif//WEBSOCKET_SERVER_H

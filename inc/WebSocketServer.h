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
        private:
                Session(WebSocketServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mWs(std::move(socket)){
                    //std::cout<<"Session()"<<std::endl;
                }

                ~Session(){
                    //std::cout<<"~Session()"<<std::endl;
                }
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
                            mRecvBuffer.
                            mOnread(*this,boost::beast:: mRecvBuffer);
                        }

                    });
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
                                    boost::property_tree::ptree pt;
                                    std::stringstream sstream(std::string(mRecvBuffer.data(),mRecvBuffer.size()));
                                    boost::property_tree::json_parser::read_json(sstream, pt);
                                    std::string cmd;
                                    try{
                                        cmd = pt.get<std::string>("topic");
                                    }
                                    catch (boost::exception& e){
                                        std::cout<<boost::diagnostic_information(e)<<std::endl;
                                    }
                                    boost::property_tree::ptree res;
                                    if(mServer.mProcesserSet.count(cmd) > 0){
                                        res.put_child("content",std::move(mServer.mProcesserSet[cmd](pt.get_child("content"))));
                                    }
                                    else{
                                        res.put_child("content",std::move(boost::property_tree::ptree()));
                                    }
                                    
                                    sstream.str("");
                                    std::stringstream ss;
                                    boost::property_tree::json_parser::write_json(sstream,res);
                                    mSendBuffer.resize(sstream.str().size()+4);
                                    *reinterpret_cast<std::uint32_t*>(mSendBuffer.data()) = sstream.str().size();
                                    memcpy(mSendBuffer.data()+4,sstream.str().data(),sstream.str().size());
                                    boost::asio::async_write(mSocket,boost::asio::buffer(mSendBuffer.data(),mSendBuffer.size()),
                                        [this,self](boost::system::error_code ec, std::size_t length){
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

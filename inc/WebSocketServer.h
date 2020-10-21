#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
namespace cxFWK{
    class WebSocketServer{
        boost::asio::io_context mIoContext;
        boost::asio::ip::tcp::acceptor mAcceptor;

        class Session
        : public std::enable_shared_from_this<Session>{
            
            public:
                Session(WebSocketServer &server,boost::asio::ip::tcp::socket& socket):mServer(server),mWs(std::move(socket)){
                    //std::cout<<"Session()"<<std::endl;
                }

                ~Session(){
                    //std::cout<<"~Session()"<<std::endl;
                }
                void run(){
                    //doRead();
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
                
                boost::beast::websocket mWs;
                std::vector<char> mRecvBuffer;
                std::vector<char> mSendBuffer;
        };

        public:
            WebSocketServer(uint16_t port):mAcceptor(mIoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)){
                mAcceptor.set_option(boost::asio::socket_base::reuse_address(true));
            }
        void run(){
            doAccept();
            mIoContext.run();
            
        }
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

    };

}
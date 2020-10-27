#ifndef CXFWK_STPCLIENT_H
#define CXFWK_STPCLIENT_H
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include "Json.h"
namespace cxFWK{
    class STPClient{
        public:
            STPClient():mSocket(mIoContext){
            }
            int connect(const char* serverIp,uint16_t port){
                boost::system::error_code ec;
                mSocket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(serverIp),port),ec);
                if( ec )
                    return -1;
                else
                    return 0;
            }
            int connect(const char* eurekaIp,uint16_t eurekaPort,const char* serviceName){
                mEurekaIp = eurekaIp;
                mEurekaPort = eurekaPort;
                mServiceName = serviceName;
                return connect();
            }
            
            ~STPClient(){
                mSocket.close();
            }
            Json request(const std::string& topic,const Json& json){
                std::stringstream ss;
                Json root;
                root.put("topic",topic);
                root.put("content",json);
                std::string root_str = root.toString();
                boost::system::error_code ec;
                mSendBuffer.resize(root_str.size()+4);
                *reinterpret_cast<uint32_t*>(mSendBuffer.data()) = root_str.size();
                memcpy(mSendBuffer.data()+4,root_str.data(),root_str.size());
                boost::asio::write(mSocket,boost::asio::buffer(mSendBuffer,mSendBuffer.size()),ec);
                if(ec)
                {
                    return Json();
                }
                mRecvBuffer.resize(4);
                boost::asio::read(mSocket,boost::asio::buffer(mRecvBuffer,mRecvBuffer.size()),ec);
                if(ec)
                {
                    return Json();
                }
                uint32_t len = *reinterpret_cast<uint32_t*>(mRecvBuffer.data());
                mRecvBuffer.resize(len);
                boost::asio::read(mSocket,boost::asio::buffer(mRecvBuffer,mRecvBuffer.size()),ec);
                if(ec)
                {
                    return Json();
                }
                ss.str("");
                ss<<std::string(mRecvBuffer.data(),mRecvBuffer.size());
                Json res(std::string(mRecvBuffer.data(),mRecvBuffer.size()));
                return res.get("content",Json());
            }
        private:
            int connect(){
                boost::system::error_code ec;
                mSocket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(mEurekaIp),mEurekaPort),ec);
                if( ec )
                    return -1;
                Json json;
                json.put("service name",mServiceName);
                json = request("query service",json);
                if(!json.hasKey("service address")){
                    return -1;
                }
                std::string serviceAddr = json.get("service address","");
                char ip[20];
                uint16_t port;
                sscanf(serviceAddr.c_str(),"%s:%hu",ip,&port);
                mSocket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip),port),ec);
                if( ec )
                    return -1;
                
                return 0;
            }
        private:
            std::string mEurekaIp;
            uint16_t mEurekaPort;
            std::string mServiceName;
            boost::asio::io_context mIoContext;
            boost::asio::ip::tcp::socket mSocket;
            std::vector<char> mRecvBuffer;
            std::vector<char> mSendBuffer;

    };
}

#endif//CXFWK_STPCLIENT_H

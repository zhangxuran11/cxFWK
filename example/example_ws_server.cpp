#include "WebSocketServer.h"
#include <iostream>
#include <string>
void fRead(cxFWK::WebSocketServer::Session& session,uint8_t* data,size_t  size){
    std::cout<<std::string((char*)data, size)<<std::endl;
    session.write("response1:"+std::string((char*)data, size));
    std::string str("response2:"+std::string((char*)data, size));
    session.write((uint8_t*)str.data(),str.length());
    session.write("response3:"+std::string((char*)data, size));
}
void fConn(cxFWK::WebSocketServer::Session& session){
    std::cout<<"new connect"<<std::endl;
    session.write("welcome");
}
void fDisConn(cxFWK::WebSocketServer::Session&){
    std::cout<<"disconnect"<<std::endl;
}
int main(){
    cxFWK::WebSocketServer wsServer(8800);
    wsServer.setOnRead(fRead);
    wsServer.setOnConnected(fConn);
    wsServer.setOnDisConnected(fDisConn);
    wsServer.run();
    return 0;
}

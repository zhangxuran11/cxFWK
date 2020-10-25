#include "WebSocketServer.h"
#include <iostream>
#include <string>
void fRead(cxFWK::WebSocketServer::Session&,uint8_t* data,size_t  size){
    std::cout<<std::string((char*)data, size)<<std::endl;
}
void fConn(cxFWK::WebSocketServer::Session&){
    std::cout<<"new connect"<<std::endl;
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

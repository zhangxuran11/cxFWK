#include "WebSocketServer.h"
#include <iostream>
#include <string>
void f(cxFWK::WebSocketServer::Session&,uint8_t* data,size_t  size){
    std::cout<<std::string((char*)data, size)<<std::endl;
}
int main(){
    cxFWK::WebSocketServer wsServer(8800) ;
    wsServer.setOnRead(f);
    wsServer.run();
    return 0;
}

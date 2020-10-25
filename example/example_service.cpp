#include <iostream>
#include <sstream>
#include "STPClient.h"
#include "Json.h"
#include "Util.h"
int main(){
    cxFWK::STPClient c;
    
    const char* ip = "127.0.0.1";
    uint16_t port = 8800;
    std::cout<<"ready connect "<<ip<<":"<<port<<std::endl;
    
    if(c.connect("127.0.0.1",8800) == 0)
        std::cout<<"connect ok"<<std::endl;
    else{
        std::cout<<"connect failured!"<<std::endl;
        return 0;
    }
    cxFWK::Json json;
    json.put("service name","example_service");
    json.put("service address","127.0.0.1:8800");
    std::cout<<"ready regsiter service"<<std::endl;
    std::cout<<"regsiter service res:"<<c.request("register service",json)<<std::endl;

    json.clear();
    std::cout<<"ready query services"<<std::endl;
    std::cout<<"query services res:"<<c.request("query services",json)<<std::endl;

    json.clear();
    json.put("service name","example_service");
    std::cout<<"ready query services"<<std::endl;
    std::cout<<"query service res:"<<c.request("query service",json)<<std::endl;

    return 0;
}

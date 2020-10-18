#include <iostream>
#include <sstream>
#include "EchoClient.h"
#include "Util.h"
int main(){
    EchoClient c;
    
    const char* ip = "127.0.0.1";
    uint16_t port = 8800;
    std::cout<<"ready connect "<<ip<<":"<<port<<std::endl;
    
    if(c.connect("127.0.0.1",8800) == 0)
        std::cout<<"connect ok"<<std::endl;
    else{
        std::cout<<"connect failured!"<<std::endl;
        return 0;
    }
    boost::property_tree::ptree pt;
    pt.put("service name","example_service");
    pt.put("service address","127.0.0.1:8800");
    std::cout<<"ready regsiter service"<<std::endl;
    std::cout<<"regsiter service res:"<<jsonToString(std::move(c.request("register service",pt)))<<std::endl;

    pt.clear();
    std::cout<<"ready query services"<<std::endl;
    std::cout<<"query services res:"<<jsonToString(std::move(c.request("query services",pt)))<<std::endl;

    pt.clear();
    pt.put("service name","example_service");
    std::cout<<"ready query services"<<std::endl;
    std::cout<<"query service res:"<<jsonToString(std::move(c.request("query service",pt)))<<std::endl;

    return 0;
}
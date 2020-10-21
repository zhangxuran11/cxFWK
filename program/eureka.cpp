#include <sstream>
#include "STPServer.h"
#include <string>
#include <map>
#include "Util.h"
static std::map<std::string,boost::property_tree::ptree> services;
static boost::property_tree::ptree registerService(const boost::property_tree::ptree& pt){
    std::cout<<cxFWK::Util::jsonToString(pt)<<std::endl;
    boost::property_tree::ptree res_content;
    try{
        std::string&& service_name = pt.get<std::string>("service name");
        services[service_name] = pt;
        res_content.put("ack","ok"); 
    }
    catch (...){
        res_content.put("ack","error");
    }
    return res_content;
}
static boost::property_tree::ptree queryServices(const boost::property_tree::ptree& pt){
    std::cout<<cxFWK::Util::jsonToString(pt)<<std::endl;
    boost::property_tree::ptree pt_servers;
    for(auto it = services.begin();it != services.end();it++)
    {
        pt_servers.push_back(std::make_pair("",it->second));
    }
    return pt_servers;
}
static boost::property_tree::ptree queryService(const boost::property_tree::ptree& pt){
    std::cout<<cxFWK::Util::jsonToString(pt)<<std::endl;
    try{
        std::string service_name = pt.get<std::string>("service name");
        if(services.count(service_name) > 0){
            return services[service_name];
        }
    }catch(...){
    }
    return boost::property_tree::ptree();
}
int main(){
    cxFWK::STPServer s(8800);
    s.registerProcesser("register service",registerService);
    s.registerProcesser("query services",queryServices);
    s.registerProcesser("query service",queryService);
    s.run();
    return 0;
}

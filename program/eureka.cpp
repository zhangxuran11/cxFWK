#include <sstream>
#include "STPServer.h"
#include <string>
#include <map>
#include "Util.h"
static std::map<std::string,cxFWK::Json> services;
static cxFWK::Json registerService(const cxFWK::Json& json){
    std::cout<<json<<std::endl;
    cxFWK::Json res_content;
    if(json.hasKey("service name")){
        std::string&& service_name = json.get("service name",std::string());
        services[service_name] = json;
        res_content.put("ack","ok"); 
    }
    else{
        res_content.put("ack","error");
    }
    return res_content;
}
static cxFWK::Json queryServices(const cxFWK::Json& json){
    std::cout<<json<<std::endl;
    cxFWK::Json json_servers;
    for(auto it = services.begin();it != services.end();it++)
    {
        json_servers.push(it->second);
    }
    return json_servers;
}
static cxFWK::Json queryService(const cxFWK::Json& json){
    std::cout<<json<<std::endl;
    if(json.hasKey("service name")){
        std::string service_name = json.get("service name","");
        if(services.count(service_name) > 0){
            return services[service_name];
        }
    }else{
    }
    return cxFWK::Json();
}
int main(){
    cxFWK::STPServer s(8800);
    s.registerProcesser("register service",registerService);
    s.registerProcesser("query services",queryServices);
    s.registerProcesser("query service",queryService);
    s.run();
    return 0;
}

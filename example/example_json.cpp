#include <iostream>
#include "Json.h"

int main(){
    std::string json_str = "{\"test1\":12345,\"test2\":1.2345,\"test3\":\"string\"}";

    std::cout<<"str json: "<<*std::make_shared<cxFWK::Json>(json_str)<<std::endl;
    std::string path = "./test.json";
    cxFWK::Json json = cxFWK::Json::read(path);
    std::cout<<"read "<<path<<": "<<json<<std::endl;
    json.clear();
    std::cout<<"after clear: "<<json<<std::endl;
    json.put("a",(int)123);
    json.put("b",1.23);
    json.put("c",std::string("ccc"));
    json.put("d",true);
    json.put("e",false);
    json.put("c",std::string("zzz"));

    cxFWK::Json child1;
    child1.put("c1",111);
    child1.put("c2","222");
    child1.put("c3",true);


    cxFWK::Json child2;
    child2.put("c1",111);
    child2.put("c3",true);
    child2.put("c2","222");



    std::cout<<"child1:"<<child1<<std::endl;
    std::cout<<"child2:"<<child2<<std::endl;

    std::cout<<"child1 == child2("<<(child1 == child2)<<")"<<std::endl;
    std::cout<<"child1 != child2("<<(child1 != child2)<<")"<<std::endl;

    json.put("child1",child1);
    json.put("child2",child2);

    std::cout<<"child1-c1: "<<child1.get("c1",-1)<<std::endl;
    std::cout<<"child1-c2: "<<child1.get("c2","000")<<std::endl;
    std::cout<<"child1-c3: "<<child1.get("c3",false)<<std::endl;

    std::cout<<"json-child1-c1: "<<json.get("child1",cxFWK::Json()).get("c1",-1)<<std::endl;
    std::cout<<"json-child1-c2: "<<json.get("child1",cxFWK::Json()).get("c2","000")<<std::endl;
    std::cout<<"json-child1-c3: "<<json.get("child1",cxFWK::Json()).get("c3",false)<<std::endl;

    std::cout<<json<<std::endl;
    json.write("./test.json");
    std::cout<<"click any key for quit..."<<std::endl;
    return 0;
}

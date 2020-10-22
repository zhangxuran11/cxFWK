#ifndef CXFWK_JSON_H
#define CXFWK_JSON_H
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include "cJSON.h"
namespace cxFWK {
    class Json{
        cJSON* mJson;
        Json(cJSON* json):mJson(json){
        }
    public:
        Json(){
            mJson = cJSON_CreateObject();
        }
        Json(const std::string& jsonStr){
            mJson = cJSON_Parse(jsonStr.c_str());
            if(mJson == nullptr)
                mJson = cJSON_CreateObject();
        }
        ~Json(){
            cJSON_Delete(mJson);
        }

        void put(const std::string& k,int v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddNumberToObject(mJson,k.c_str(),v);
        }
        void put(const std::string& k,double v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddNumberToObject(mJson,k.c_str(),v);
        }
        void put(const std::string& k,const std::string& v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddStringToObject(mJson,k.c_str(),v.c_str());
        }
        void put(const std::string& k,const char* v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddStringToObject(mJson,k.c_str(),v);
        }
        void put(const std::string& k,bool v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddBoolToObject(mJson,k.c_str(),v);
        }
        void put(const std::string& k,const Json& v){
            cJSON_Delete(cJSON_DetachItemFromObject(mJson,k.c_str()));
            cJSON_AddItemToObject(mJson,k.c_str(),cJSON_Duplicate( v.mJson,true));
        }

        int get(const std::string& k,int defaultVal){
            if(!cJSON_HasObjectItem(mJson,k.c_str()))
                return defaultVal;
            return (int)cJSON_GetNumberValue( cJSON_GetObjectItem(mJson,k.c_str()));
        }
        std::string get(const std::string& k,const std::string& defaultVal){
            if(!cJSON_HasObjectItem(mJson,k.c_str()))
                return defaultVal;
            return cJSON_GetStringValue( cJSON_GetObjectItem(mJson,k.c_str()));
        }
        std::string get(const std::string& k,const char* defaultVal){
            if(!cJSON_HasObjectItem(mJson,k.c_str()))
                return defaultVal;
            return cJSON_GetStringValue( cJSON_GetObjectItem(mJson,k.c_str()));
        }
        bool get(const std::string& k,bool defaultVal){
            if(!cJSON_HasObjectItem(mJson,k.c_str()))
                return defaultVal;
            return cJSON_IsTrue( cJSON_GetObjectItem(mJson,k.c_str())) == true ? true : false;
        }
        Json get(const std::string& k,const Json& defaultVal){
            if(!cJSON_HasObjectItem(mJson,k.c_str()))
                return defaultVal;
            return Json(cJSON_Duplicate(cJSON_GetObjectItem(mJson,k.c_str()),true));
        }
        bool operator==(const Json& json){
            return cJSON_Compare(mJson,json.mJson,true);
        }
        bool operator!=(const Json& json){
            return !(*this == json);
        }
        void clear(){
            cJSON_Delete( mJson);
            mJson = cJSON_CreateObject();
        }
        static Json read(std::istream& in){
            return Json(std::string(std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()));
        }
        static Json read(std::string& path){
            std::ifstream in(path);
            return read(in);
        }

        void write(const std::string& path){
            std::ofstream out(path);
            out << *this;
        }
        bool empty()
        {
            return cJSON_IsNull(mJson);
        }
        friend std::ostream& operator<<(std::ostream& out,Json& json);
    };
    std::ostream& operator<<(std::ostream& out,Json& json){
        out << std::shared_ptr<char>(cJSON_Print(json.mJson));
        return out;
    }

}

#endif // CXFWK_JSON_H

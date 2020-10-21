#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace cxFWK{
    class Util{
        Util(){}
        public:
            const std::string jsonToString(const boost::property_tree::ptree& pt){
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss,pt);
            return ss.str();
        }

    };
    
}
#endif//UTIL_H
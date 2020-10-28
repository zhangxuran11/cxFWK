#include<stdio.h>
#define __IMPL__
#include "cxFWK/Logger.h"
#include <unistd.h>


//void init() {
//    logging::add_file_log
//    (
//        keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
//        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
//        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
//        keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
//    );

//    logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::info);
//}

int main(int , char* []) {
    std::string src_file = __FILE__;
    int pos = src_file.find_last_of('/');
    src_file = src_file.substr(0,pos+1);
    cxFWK::Logger::Reconfigure(src_file+"my_log.conf");
    while(1){
        cxFWK::Logger::CheckConfFileChangedAndApply();

        cxFWK::Logger::Info()<<"hello1";
        cxFWK::Logger::Error()<<"hello2";
        cxFWK::Logger::Warn()<<"hello3";
        //cxFWK::Logger::Fatal()<<"hello4";
        cxFWK::Logger::Debug()<<"hello5";

        cxFWK::Logger::Info("print %v %v %v","print hello1",123,4.5);
        cxFWK::Logger::Error("print %v","print hello2");
        cxFWK::Logger::Warn("print %v","print hello3");
        //cxFWK::Logger::Fatal("print %v","print hello4");
        cxFWK::Logger::Debug("print %v","print hello5");

        sleep(1);

    }

    return 0;
}

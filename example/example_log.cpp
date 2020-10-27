#include<stdio.h>
#define __IMPL__
#include "cxFWK/Logger.h"



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
    return 0;
}

#ifndef CXFWK_LOGGER_H
#define CXFWK_LOGGER_H
#define ELPP_VARIADIC_TEMPLATES_SUPPORTED
#include "../third/easyloggingpp/easylogging++.h"
#include <string>
namespace cxFWK {
    class Logger{
        std::string mName;
        static void init(){

        }
        enum Level{
            Level_Info,
            Level_Warn,
            Level_Error,
            Level_Fatal,
            Level_Debug
        };
        class Stream{
            std::string mLoggerName;
            Level mLevel;
        public:
            Stream(const std::string& mloggerName,Level level):mLoggerName(mloggerName),mLevel(level){}
            template <typename T>
            inline Stream& operator<<(const T& log){
                switch (mLevel) {
                case Level_Info:
                    CLOG(INFO,mLoggerName.data()) << log;
                    break;
                case Level_Warn:
                    CLOG(WARNING,mLoggerName.data()) << log;
                    break;
                case Level_Error:
                    CLOG(ERROR,mLoggerName.data()) << log;
                    break;
                case Level_Fatal:
                    CLOG(FATAL,mLoggerName.data()) << log;
                    break;
                case Level_Debug:
                    CLOG(DEBUG,mLoggerName.data()) << log;
                    break;
                default:
                    break;
                }
                return *this;
            }

        };
        Stream mInfoStream;
        Stream mWarnStream;
        Stream mErrorStream;
        Stream mFatalStream;
        Stream mDebugStream;
        static Logger sDefaultLogger;
    public:
        Logger(const std::string& name):mName(name),mInfoStream(name,Level_Info),mWarnStream(name,Level_Warn),mErrorStream(name,Level_Error),mFatalStream(name,Level_Fatal),mDebugStream(name,Level_Debug){
            el::Configurations defaultConf;
            defaultConf.setToDefault();

            // 重新设置GLOBAL级别的配置项FORMAT的值
            defaultConf.setGlobally(
                el::ConfigurationType::Format, "%datetime %level %msg");
            /// 重新设置配置
            el::Loggers::reconfigureLogger(mName.data(), defaultConf);
        }

        Stream& info(){
            return mInfoStream;
        }
        Stream& warn(){
            return mWarnStream;
        }
        Stream& error(){
            return mErrorStream;
        }
        Stream& fatal(){
            return mFatalStream;
        }
        Stream& debug(){
            return mDebugStream;
        }
        static Stream& Info(){
            return sDefaultLogger.info();
        }
        static Stream& Warn(){
            return sDefaultLogger.warn();
        }
        static Stream& Error(){
            return sDefaultLogger.error();
        }
        static Stream& Fatal(){
            return sDefaultLogger.fatal();
        }
        static Stream& Debug(){
            return sDefaultLogger.debug();
        }

        template <typename T, typename... Args>
        inline void info(const char* s, const T& value, const Args&... args){
            el::Logger* defaultLogger = el::Loggers::getLogger(mName.data());
            defaultLogger->info(s, value, args...);
        }
        template <typename T, typename... Args>
        inline void warn(const char* s, const T& value, const Args&... args){
            el::Logger* defaultLogger = el::Loggers::getLogger(mName.data());
            defaultLogger->warn(s, value, args...);
        }
        template <typename T, typename... Args>
        inline void error(const char* s, const T& value, const Args&... args){
            el::Logger* defaultLogger = el::Loggers::getLogger(mName.data());
            defaultLogger->error(s, value, args...);
        }
        template <typename T, typename... Args>
        inline void fatal(const char* s, const T& value, const Args&... args){
            el::Logger* defaultLogger = el::Loggers::getLogger(mName.data());
            defaultLogger->fatal(s, value, args...);
        }
        template <typename T, typename... Args>
        inline void debug(const char* s, const T& value, const Args&... args){
            el::Logger* defaultLogger = el::Loggers::getLogger(mName.data());
            defaultLogger->debug(s, value, args...);
        }

        template <typename T, typename... Args>
        inline static void Info(const char* s, const T& value, const Args&... args){
            sDefaultLogger.info(s, value, args...);
        }
        template <typename T, typename... Args>
        inline static void Warn(const char* s, const T& value, const Args&... args){
            sDefaultLogger.warn(s, value, args...);
        }
        template <typename T, typename... Args>
        inline static void Error(const char* s, const T& value, const Args&... args){
            sDefaultLogger.error(s, value, args...);
        }
        template <typename T, typename... Args>
        inline static void Fatal(const char* s, const T& value, const Args&... args){
            sDefaultLogger.fatal(s, value, args...);
        }
        template <typename T, typename... Args>
        inline static void Debug(const char* s, const T& value, const Args&... args){
            sDefaultLogger.debug(s, value, args...);
        }
    };
}
#ifdef __IMPL__
#include "../third/easyloggingpp/easylogging++.cc"
INITIALIZE_EASYLOGGINGPP
cxFWK::Logger cxFWK::Logger::sDefaultLogger("default");
#endif//__IMPL__
#endif // CXFWK_LOGGER_H

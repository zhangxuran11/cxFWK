#ifndef CXFWK_LOGGER_H
#define CXFWK_LOGGER_H
#define ELPP_DEBUG_ASSERT_FAILURE
#define ELPP_VARIADIC_TEMPLATES_SUPPORTED

#define ELPP_THREAD_SAFE
#define ELPP_NO_LOG_TO_FILE
#define ELPP_NO_DEFAULT_LOG_FILE
#include "../third/easyloggingpp/easylogging++.h"
#include <string>
#include <sstream>
#include <map>
#include <sys/inotify.h>
#include <unistd.h>
namespace cxFWK {
    class Logger{
    public:
        enum Level{
            Level_Debug,
            Level_Info,
            Level_Warn,
            Level_Error,
            Level_Fatal
        } mLevel = Level_Info;
    private:
        std::string mName;

        static void init(){

        }

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
        std::string mConfFile;
        int mInotifyFd;
        static void preRollOutCallback(const char* path,size_t ){
            static std::map<std::string,int> rollCounts;
            if(rollCounts.count(path) < 1){
                rollCounts[path] = 0;
            }
            std::stringstream ss;
            ss<<"mv "<<path<<" "<<path<<"."<< ++rollCounts[path];
            system(ss.str().c_str());

        }
    public:
        Logger(const std::string& name):mName(name),mInfoStream(name,Level_Info),mWarnStream(name,Level_Warn),mErrorStream(name,Level_Error),mFatalStream(name,Level_Fatal),mDebugStream(name,Level_Debug),mInotifyFd(-1){
            if(!el::Loggers::hasFlag(el::LoggingFlag::StrictLogFileSizeCheck))
                el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
            el::Configurations defaultConf;
            defaultConf.setToDefault();
            defaultConf.setGlobally(el::ConfigurationType::Enabled, std::string("true"));
            //setGlobally(ConfigurationType::Filename, std::string(base::consts::kDefaultLogFile), true);
//          #if defined(ELPP_NO_LOG_TO_FILE)
//            setGlobally(ConfigurationType::ToFile, std::string("false"), true);
//          #else
//            setGlobally(ConfigurationType::ToFile, std::string("true"), true);
//          #endif // defined(ELPP_NO_LOG_TO_FILE)
            defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, std::string("true"));
            defaultConf.setGlobally(el::ConfigurationType::SubsecondPrecision, std::string("3"));
            defaultConf.setGlobally(el::ConfigurationType::PerformanceTracking, std::string("true"));
            defaultConf.setGlobally(el::ConfigurationType::MaxLogFileSize, std::string("0"));
            defaultConf.setGlobally(el::ConfigurationType::LogFlushThreshold, std::string("0"));

            defaultConf.setGlobally(el::ConfigurationType::Format, std::string("%datetime %level [%logger] %msg"));
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Format,
                std::string("%datetime %level [%logger] [%user@%host] [%func] [%loc] %msg"));
            // INFO and WARNING are set to default by Level::Global
            defaultConf.set(el::Level::Error, el::ConfigurationType::Format, std::string("%datetime %level [%logger] %msg"));
            defaultConf.set(el::Level::Fatal, el::ConfigurationType::Format, std::string("%datetime %level [%logger] %msg"));
            defaultConf.set(el::Level::Verbose, el::ConfigurationType::Format, std::string("%datetime %level-%vlevel [%logger] %msg"));
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Format, std::string("%datetime %level [%logger] [%func] [%loc] %msg"));

            el::Helpers::installPreRollOutCallback(preRollOutCallback);
            // 重新设置GLOBAL级别的配置项FORMAT的值
            defaultConf.setGlobally(
                el::ConfigurationType::Format, "%datetime %level %msg");
            /// 重新设置配置
            el::Loggers::reconfigureLogger(mName.data(), defaultConf);
        }
        ~Logger(){
            el::Helpers::uninstallPreRollOutCallback();
        }
        static void setDefaultLoggerLevel(Level lvl){
            sDefaultLogger.setLevel(lvl);
        }
        void setLevel(Level lvl){
            switch (lvl) {
            case Level_Debug:
                break;
            case Level_Info:
                break;
            case Level_Warn:
            {
                el::Configurations *configurations = el::Loggers::getLogger(mName)->configurations();
                configurations->set(el::Level::Debug,el::ConfigurationType::Enabled,"false");
                configurations->set(el::Level::Info,el::ConfigurationType::Enabled,"false");
                el::Loggers::reconfigureLogger(mName,*configurations);
                break;
            }
            default:
                break;
            }
        }
        void reconfigure(const std::string& fileName){
            el::Configurations conf(fileName);
            if(conf.empty()){
                return ;
            }
            el::Loggers::reconfigureLogger("default", conf);
            if(fileName != mConfFile){
                mConfFile = fileName;
                if(mInotifyFd > -1){
                    close(mInotifyFd);
                }
                mInotifyFd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
                if(inotify_add_watch(mInotifyFd, mConfFile.data(), IN_MODIFY | IN_DELETE_SELF) < 0){
                    close(mInotifyFd);
                    mInotifyFd = -1;
                }
            }
        }
        void checkConfFileChangedAndApply(){
            if(mInotifyFd == -1)
                return;
            uint8_t buf[1024] = {0};
            bool reconf = false;
            std::string confFile;
            int len = 0;
            while ( (len = read(mInotifyFd, buf, sizeof(buf) - 1)) > 0 )
            {
                int nread = 0;
                struct inotify_event *event;
                while ( len > 0 )
                {
                  event = (struct inotify_event *)&buf[nread];
                  if(event->mask & IN_MODIFY){
                      reconf = true;
                  }
                  if(event->mask & IN_DELETE_SELF){
                      reconf = true;
                      confFile = mConfFile;
                      mConfFile = "";
                      close(mInotifyFd);
                      mInotifyFd = -1;
                  }
                  nread = nread + sizeof(struct inotify_event) + event->len;
                  len   = len - sizeof(struct inotify_event) - event->len;
                }
            }
            if(reconf){
                reconfigure(confFile);
            }


        }
        static void Reconfigure(const std::string& fileName){
            return sDefaultLogger.reconfigure(fileName);
        }
        static void CheckConfFileChangedAndApply(){
            return sDefaultLogger.checkConfFileChangedAndApply();
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
        inline static void Warn(const char* s){
            Warn("%v%v",s,"");
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

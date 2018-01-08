#ifndef CWLOGGER_H
#define CWLOGGER_H
#include <stdarg.h>
#include <string>
#ifdef WIN32

#ifdef LOGMODULE_EXPORTS
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#endif

#else

#define LOG_API

#endif

/*
#logger config
#loggger level "trace < debug < information < notice < warnning < error < fatal"


const char* config =
                "logging.loggers.root.channel = c3\n"
                "logging.loggers.root.level = information\n"
                "logging.channels.c1.class = ColorConsoleChannel\n"
                "logging.channels.c1.formatter = f1\n"
                "logging.channels.c2.class = FileChannel\n"
                "logging.channels.c2.path = ./log/run.log\n"
                "logging.channels.c2.formatter = f1\n"
                "logging.channels.c2.rotation=1 days\n"
                "logging.channels.c2.archive=number\n"
                "logging.channels.c2.purgeCount=90\n"
                "logging.channels.c2.compress=true\n"
                "logging.channels.c2.flush=false\n"
                "logging.channels.c3.class = SplitterChannel\n"
                "logging.channels.c3.channel1=c1\n"
                "logging.channels.c3.channel2=c2\n"
                "logging.formatters.f1.class = PatternFormatter\n"
                "logging.formatters.f1.pattern = [%p] %L%Y-%n-%d %H:%M:%S %t  %U %u\n"
                "logging.formatters.f1.times = UTC\n";
*/
#define MAX_BUF_SIZE 8192
enum LOG_ERR_CODE
{
    LOG_ERR_OK=0,
    LOG_ERR_PARAM, //ÅäÖÃ²ÎÊý´íÎó
    LOG_ERR_BUFF_NULL, //´íÎóbufferÎªNULL
    LOG_ERR_BUFF_TOO_SMALL, //´íÎóbufferÌ«Ð¡.
    LOG_ERR_UNKOWN_ERR, //Î´Öª´íÎó.
};
enum{
        LOG_PRIO_TRACE =0,       /// A tracing message. This is the lowest priority.
        LOG_PRIO_DEBUG,       /// A debugging message.
        LOG_PRIO_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
        LOG_PRIO_NOTICE,      /// A notice, which is an information with just a higher priority.
        LOG_PRIO_WARNING,     /// A warning. An operation completed with an unexpected result.
        LOG_PRIO_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
//        LOG_PRIO_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
        LOG_PRIO_FATAL    /// A fatal error. The application will most likely terminate. This is the highest priority.
};


LOG_API void    log_print(char* buf,int size,const char* fmt,...);
LOG_API void    log_write(int level,const char* msg, const char* file, int line);
LOG_API int     log_init(const char* config,char* err_buffer, int err_buffer_size);
LOG_API int     log_setlevel(int level);
LOG_API int     log_getlevel();
LOG_API int     log_setchannel(std::string channel);
LOG_API int     log_getchannel(std::string &channel);
LOG_API int     log_read(std::string& msg);
#define log_printf(level,...) {\
    char __buf__[MAX_BUF_SIZE]; \
    log_print(__buf__,MAX_BUF_SIZE,__VA_ARGS__); \
    log_write(level,__buf__,__FUNCTION__,__LINE__);}
//#define CW_DEBUG_ENABLE 0
#ifdef CW_DEBUG_ENABLE
#define   cw_trace(...)  log_printf(LOG_PRIO_TRACE, __VA_ARGS__)
#define   cw_debug(...)  log_printf(LOG_PRIO_DEBUG,  __VA_ARGS__)
#define   cw_info(...)   log_printf(LOG_PRIO_INFORMATION,  __VA_ARGS__)
#define   cw_notice(...) log_printf(LOG_PRIO_NOTICE,  __VA_ARGS__)
#define   cw_warn(...)   log_printf(LOG_PRIO_WARNING,  __VA_ARGS__)
#define   cw_error(...)  log_printf(LOG_PRIO_ERROR,  __VA_ARGS__)
#define   cw_fatal(...)  log_printf(LOG_PRIO_FATAL,  __VA_ARGS__)
#else
#define   cw_trace(...) printf
#define   cw_debug(...) printf
#define   cw_info(...) printf
#define   cw_notice(...) printf
#define   cw_warn(...) printf
#define   cw_error(...) printf
#define   cw_fatal(...) printf
#endif

#define LOG_FUNC_ENTER cw_trace("%s %d",__FUNC__,__LINE__)
#endif


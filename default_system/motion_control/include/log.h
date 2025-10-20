/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */

//
// Created by master on 17. 3. 9.
//

#ifndef TEMP_LOG_H
#define TEMP_LOG_H
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <tuple>
#include <thread>
#include <string>
#include <memory>
#include <condition_variable>
#include <queue>
#include <Poco/SingletonHolder.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define LOG_MESSAGE_LEN_MAX  200

enum class LogLevel : int {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
};


struct LogMessage {
    int   length; // log message length
    int   level;
    double ts;  // tv_sec + tv_nsec
    pid_t pid;
    pid_t tid;
    const char* file;
    int   line;
    char  text[LOG_MESSAGE_LEN_MAX];
public:
    LogMessage(){
        static int cnt = 0; ++cnt;
        //printf("LOG %d created!\n", cnt);
    }
    ~LogMessage(){
        static int cnt = 0; ++cnt;
        //printf("LOG %d deleted!\n", cnt);
    }
    void vset(LogLevel lvl, const char* filename, int lineno, const char*fmt, va_list ap)
    {
        struct timespec now;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        now.tv_sec = mts.tv_sec;
        now.tv_nsec = mts.tv_nsec;

#else
        clock_gettime(CLOCK_REALTIME, &now);
#endif
        ts = now.tv_sec % 10000 + now.tv_nsec * 1e-9;
        pid = getpid();
        tid = syscall(SYS_gettid);
        level = static_cast<int>(lvl);
        file = filename;
        line = lineno;

        length = vsnprintf(text, LOG_MESSAGE_LEN_MAX-1, fmt, ap);

        // append new line
        if (text[length-1] != '\n') {
            text[length] = '\n';
            text[length+1] = 0;
            length += 1;
        }
    }

    void set(LogLevel lvl, const char* filename, int lineno, const char* fmt, ...)
    {
        // text setting
        va_list ap;
        va_start(ap, fmt);
        vset(lvl, filename, lineno, fmt, ap);
        va_end(ap);
    }

    void print()
    {
        printf("[%.5f]pid=%d, tid=%d, src=%s(%d), len = %d, text=%s", ts, pid, tid, file, line, length, text );
    }
};

class Connector;
class LogPool;
class Condition {
    public:
        Condition(std::mutex &lock): exit_(false), cv_m(lock){}
        void wait() {
            std::unique_lock<std::mutex> lock(cv_m);
            cv.wait(lock);
        }
        void wakeup() {
            cv.notify_one();
        }

        bool timeToExit() { return exit_; }
        bool exit() {
            exit_ = true;
            cv.notify_one();
        }
    private:
        std::condition_variable cv;
        std::mutex& cv_m;
        bool exit_;
};
// 210706 yjchoe
/*
class Condition_tmp {
public:
    Condition_tmp(std::recursive_mutex &lock): exit_(false), cv_m(lock){}
    void wait() {
        std::unique_lock<std::recursive_mutex> lock(cv_m);
        cv.wait(lock);
    }
    void wakeup() {
        cv.notify_one();
    }

    bool timeToExit() { return exit_; }
    bool exit() {
        exit_ = true;
        cv.notify_one();
    }
private:
    std::condition_variable_any cv;
    std::recursive_mutex& cv_m;
    bool exit_;
};
*/
using LogQueue = std::queue<LogMessage*>;
class LogSender {
public:
    LogSender();
    ~LogSender();

    LogMessage* getLogObject(void);
    void put(LogMessage *log);
private:
    int send(LogMessage* log);
    LogMessage* pop();

    Connector* con_;
    LogPool* pool_;
    LogQueue* queue_;
    std::thread* worker_;
    std::mutex qlock_;
    Condition* cond_;

    friend int logSender(LogSender*);
};

//using TraceQueue = std::queue<LogMessage*>;
//class TraceSender {
//public:
//    TraceSender();
//    ~TraceSender();
//
//    LogMessage* getLogObject(void);
//    void put(LogMessage *log);
//private:
//    int send(LogMessage* log);
//    LogMessage* pop();
//
//    Connector* con_;
//    LogPool* pool_;
//    TraceQueue* queue_;
//    std::thread* worker_;
//    std::mutex qlock_;
//    Condition* cond_;
//
//    friend int traceSender(TraceSender*);
//};

extern Poco::SingletonHolder<LogSender> theLogger;
//extern Poco::SingletonHolder<TraceSender> theTraceLogger;

#define __SEP_LOG(lvl, ...)  do{\
    LogMessage* log = theLogger.get()->getLogObject(); \
    log->set(lvl, __FILE__, __LINE__, __VA_ARGS__); \
    theLogger.get()->put(log); \
    }while(false)
//

#define SEP_LOGT(...) __SEP_LOG(LogLevel::TRACE, __VA_ARGS__)
#define SEP_LOGD(...) __SEP_LOG(LogLevel::DEBUG, __VA_ARGS__)
#define SEP_LOGI(...) __SEP_LOG(LogLevel::INFO, __VA_ARGS__)
#define SEP_LOGW(...) __SEP_LOG(LogLevel::WARN, __VA_ARGS__)
#define SEP_LOGE(...) __SEP_LOG(LogLevel::ERROR, __VA_ARGS__)


//#define __TRACE_LOG(lvl, ...)  do{\
//    LogMessage* log = theTraceLogger.get()->getLogObject(); \
//    log->set(lvl, __FILE__, __LINE__, __VA_ARGS__); \
//    theTraceLogger.get()->put(log); \
//    }while(false)
////
//
//#define TRACE_LOGT(...) __TRACE_LOG(LogLevel::TRACE, __VA_ARGS__)
//#define TRACE_LOGD(...) __TRACE_LOG(LogLevel::DEBUG, __VA_ARGS__)
//#define TRACE_LOGI(...) __TRACE_LOG(LogLevel::INFO, __VA_ARGS__)
//#define TRACE_LOGW(...) __TRACE_LOG(LogLevel::WARN, __VA_ARGS__)
//#define TRACE_LOGE(...) __TRACE_LOG(LogLevel::ERROR, __VA_ARGS__)

#define MACRO_ARG_COUNT(...)    std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value
#define __SEP_LOG_PREFIX(lvl, prefix, fmt, ...)  do{\
    LogMessage* log = theLogger.get()->getLogObject(); \
    log->set(lvl, __FILE__, __LINE__, "[%s]" fmt, prefix, __VA_ARGS__); \
    theLogger.get()->put(log); \
    }while(false)



#define SEP_LOGT_PREFIX(prefix, fmt, ...) __SEP_LOG_PREFIX(LogLevel::TRACE, (prefix), fmt, __VA_ARGS__)
#define SEP_LOGD_PREFIX(prefix, fmt, ...) __SEP_LOG_PREFIX(LogLevel::DEBUG, (prefix), fmt, __VA_ARGS__)
#define SEP_LOGI_PREFIX(prefix, fmt, ...) __SEP_LOG_PREFIX(LogLevel::INFO, (prefix), fmt, __VA_ARGS__)
#define SEP_LOGW_PREFIX(prefix, fmt, ...) __SEP_LOG_PREFIX(LogLevel::WARN, (prefix), fmt, __VA_ARGS__)
#define SEP_LOGE_PREFIX(prefix, fmt, ...) __SEP_LOG_PREFIX(LogLevel::ERROR, (prefix), fmt, __VA_ARGS__)


#define __SEP_TRACE(lvl, ...) do {printf("[%s:%d][%c]", __FILE__, __LINE__, lvl); printf(__VA_ARGS__);} while(false)
#define SEP_TRT(...) __SEP_TRACE('T', __VA_ARGS__)
#define SEP_TRD(...) __SEP_TRACE('D', __VA_ARGS__)
#define SEP_TRI(...) __SEP_TRACE('I', __VA_ARGS__)
#define SEP_TRW(...) __SEP_TRACE('W', __VA_ARGS__)
#define SEP_TRE(...) __SEP_TRACE('E', __VA_ARGS__)
#define SEP_TRACE SEP_TRT

#define SEP_REQUIRE_RET(cond,val) do{ if (!(cond)){SEP_TRACE(#cond "\n"); return (val);} } while(false)
#define SEP_REQUIRE(cond) do{ if (!(cond)){SEP_TRACE(#cond "\n"); return;} } while(false)
#define SEP_REQUIRE_THROW(cond) do{ if (!(cond)){throw std::runtime_error(#cond "\n"); } } while(false)



#if 0 //!__x86_64__ // 32bit machine

#include <memory.h>
#include "Poco/Net/StreamSocket.h"
class StreamSocket {
public:
    StreamSocket() {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    }
    ~StreamSocket() {
        if (sockfd_ >= 0) {
            close(sockfd_);
        }
    }
    int connect(const Poco::Net::SocketAddress& address) {
        SEP_REQUIRE_RET(sockfd_>=0, -1);
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
#ifdef __MACH__
        server_addr.sin_len = sizeof(server_addr);
#endif
        server_addr.sin_family = AF_INET;
        //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        //server_addr.sin_port = htons(1111);
        server_addr.sin_addr.s_addr = inet_addr(address.host().toString().c_str());
        server_addr.sin_port = htons(address.port());
        int connect_ret = ::connect(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr));
        SEP_REQUIRE_RET(connect_ret >= 0, -1);

        return  0;
    }
    int shutdownSend() {
        return 0;
    }
    int sendBytes(void* buff, size_t len, uint32_t flags) {
        return ::send(sockfd_, buff, len, flags);
    }

private:
    int sockfd_;
};
#endif

#endif //TEMP_LOG_H

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

#include "log.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include <list>
#include <queue>
#include <memory>
#include <algorithm>
#include <thread>
#include <mutex>
#include <memory.h>
#include <sys/timeb.h>

using Poco::Net::StreamSocket;
using Poco::Net::SocketStream;
using Poco::Net::SocketAddress;
using std::unique_ptr;


Poco::SingletonHolder<LogSender> theLogger;
//Poco::SingletonHolder<TraceSender> theTraceLogger;

// disable trace messages
#undef SEP_TRACE
#define SEP_TRACE



// static functions
class Connector
{
public:
    Connector(SocketAddress sa):
            //sa_(SocketAddress::UNIX_LOCAL, "/tmp/sock_log")
            sa_(sa)//"localhost", "1111")
    {
        try {
            sock_.connect(sa_);
            connected_ = true;
        }
        catch (...)
        {
            connected_ = false;
        }
    }
    ~Connector()
    {
        if (connected_) {
            sock_.shutdownSend();
        }
        //sock_.close();
    }
    int send(void *buff, int len)
    {
        try {
            if (connected_) {
                return sock_.sendBytes(buff, len, 0);
            } else {
                reconnect_to_logsvr();
                if (connected_) {
                    return sock_.sendBytes(buff, len, 0);
                } else {
                    // remove prefix & posfix %%<< %%>>
                    char *content = (char *) buff + 4;
                    content[len - 8] = 0;
                    return printf("%s", content);
                }
            }
        } catch (std::exception &e) {
            printf("logsvr error, please check it : %s\n", e.what());
            connected_ = false;
            sock_.close();
        }
    }

    void reconnect_to_logsvr()
    {
        try {
            sock_.connect(sa_);
            connected_ = true;
            usleep(1000000);
        }
        catch (...)
        {
            connected_ = false;
        }
    }
private:
    SocketAddress sa_;
    StreamSocket sock_;
    bool connected_;
};

class LogPool
{
public:
    LogPool(int size = 10)
    {
        for(int i = 0; i < size; i++) {
            list_.push_back(new LogMessage);
        }
    }
    ~LogPool()
    {
        for( auto& obj: list_) {
            delete obj;
        }
    }

    LogMessage* get(void)
    {
        LogMessage *ret = nullptr;
        std::lock_guard<std::mutex> lock(lock_);
        if (!list_.empty()) {
            ret = list_.front();
            list_.pop_front();
        } else { // create a new log if pool is empty
            ret = new LogMessage;
        }

        return ret;
    }

    void put(LogMessage *obj)
    {
        std::lock_guard<std::mutex> lock(lock_);
        list_.push_back(obj);
    }

private:
    std::mutex lock_;
    std::list<LogMessage*> list_;
};


void testfn()
{
    // log pool test
    LogPool pool(10);

    LogMessage *log = pool.get();
    pool.put(log);
}

int logSender(LogSender* logger)
{
    auto& ev = *logger->cond_;
    auto& logq = *logger->queue_;
    int cnt = 0;

    SEP_TRACE("log worker begin\n");
    while(!ev.timeToExit()) {
        while(!logq.empty()) {
            logger->send(logger->pop());
            usleep(0); // for scheduling
            SEP_TRACE("count = %d\n", ++cnt);
        }
        if (ev.timeToExit()) {
            break;
        }
        ev.wait();
    }
    // flush messages
    while(!logq.empty()) {
        logger->send(logger->pop());
    }
    SEP_TRACE("log worker end\n");
};

LogSender::LogSender()
{
    cond_ = new Condition(qlock_);
    pool_ = new LogPool(10);
    queue_ = new std::queue<LogMessage*>;
    con_ = new Connector(SocketAddress("localhost", "1111"));
    worker_ = new std::thread(logSender, this);
}

LogSender::~LogSender()
{
    cond_->exit();
    worker_->join();
    delete worker_;
    delete cond_;
//    delete con_;
    delete pool_;
    delete queue_;
}

LogMessage* LogSender::getLogObject(void)
{
    return pool_->get();
}

void LogSender::put(LogMessage *log)
{
    std::lock_guard<std::mutex> lock(qlock_);
    queue_->push(log);
    cond_->wakeup();
}

LogMessage *LogSender::pop()
{
    std::lock_guard<std::mutex> lock(qlock_);
    LogMessage *log = queue_->front();
    queue_->pop();
    return log;
}

int LogSender::send(LogMessage *log)
{
    char buff[2048];
    const char *loglevel = "TDIWE";
    const char *MAGIC1 = "%%<<";
    const char *MAGIC2 = "%%>>";

    struct timeb timer_msec;
    ftime(&timer_msec);
    struct tm* t = localtime(&timer_msec.time);

//    time_t timer;
//    struct tm* t;
//    timer = time(NULL);
//    t = localtime(&timer);

//    int length = snprintf(buff, sizeof(buff), "%s[%4.6f:%d:%d %p:%d][%c] %s%s", MAGIC1, log->ts, log->pid, log->tid,
//                          log->file, log->line, loglevel[log->level], log->text, MAGIC2);
    //char* file = log->file;
    int milli = timer_msec.millitm;
    const char* file = (log->file) ? strrchr(log->file, '/') : "(nofile)";
    std::string file_name = std::string(file).substr(1);
    int length = snprintf(buff, sizeof(buff), "%s[%04d%02d%02d %02d:%02d:%02d.%04d] [%c] [%s:%d] %s%s", MAGIC1, t->tm_year + 1900, t->tm_mon + 1,
        t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, milli*10, loglevel[log->level], (file) ? file_name.c_str() : log->file, log->line,  log->text, MAGIC2);
    pool_->put(log);
    return con_->send(buff, length);
}

//int traceSender(TraceSender* logger)
//{
//    auto& ev = *logger->cond_;
//    auto& logq = *logger->queue_;
//    int cnt = 0;
//
//    SEP_TRACE("log worker begin\n");
//    while(!ev.timeToExit()) {
//        while(!logq.empty()) {
//            logger->send(logger->pop());
//            usleep(0); // for scheduling
//            SEP_TRACE("count = %d\n", ++cnt);
//        }
//        if (ev.timeToExit()) {
//            break;
//        }
//        ev.wait();
//    }
//    // flush messages
//    while(!logq.empty()) {
//        logger->send(logger->pop());
//    }
//    SEP_TRACE("log worker end\n");
//};
//
//TraceSender::TraceSender()
//{
//    cond_ = new Condition(qlock_);
//    pool_ = new LogPool(10);
//    queue_ = new std::queue<LogMessage*>;
//    con_ = new Connector(SocketAddress("172.16.1.1", "1117"));
//    worker_ = new std::thread(traceSender, this);
//}
//
//TraceSender::~TraceSender()
//{
//    cond_->exit();
//    worker_->join();
//    delete worker_;
//    delete cond_;
////    delete con_;
//    delete pool_;
//    delete queue_;
//}
//
//LogMessage* TraceSender::getLogObject(void)
//{
//    return pool_->get();
//}
//
//void TraceSender::put(LogMessage *log)
//{
//    std::lock_guard<std::mutex> lock(qlock_);
//    queue_->push(log);
//    cond_->wakeup();
//}
//
//LogMessage *TraceSender::pop()
//{
//    std::lock_guard<std::mutex> lock(qlock_);
//    LogMessage *log = queue_->front();
//    queue_->pop();
//    return log;
//}
//
//int TraceSender::send(LogMessage *log)
//{
//    char buff[2048];
//    const char *loglevel = "TDIWE";
//    const char *MAGIC1 = "%%<<";
//    const char *MAGIC2 = "%%>>";
////    int length = snprintf(buff, sizeof(buff), "%s[%4.6f:%d:%d %p:%d][%c] %s%s", MAGIC1, log->ts, log->pid, log->tid,
////                          log->file, log->line, loglevel[log->level], log->text, MAGIC2);
//    //char* file = log->file;
//    const char* file = (log->file) ? strrchr(log->file, '/') : "(nofile)";
//    int length = snprintf(buff, sizeof(buff), "%s[%4.6f:%d:%d %s:%d][%c] %s%s", MAGIC1, log->ts, log->pid, log->tid,
//                          (file) ? file : log->file, log->line, loglevel[log->level], log->text, MAGIC2);
//    pool_->put(log);
//    return con_->send(buff, length);
//}
//

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
// Created by sy1135.jeong1696 on 2017-07-24.
//

#ifndef SEPMASTER_SIMPLE_CIRCULAR_QUEUE_H
#define SEPMASTER_SIMPLE_CIRCULAR_QUEUE_H
#include <cstdint>
#include <string.h>
#include <mutex>
namespace sephi
{

    struct NullMutex {
        void lock(){}
        void unlock(){}
    };

    template<int Q_SIZE = 256, typename MTX = NullMutex>
    class SimpleQueue {
    public:
        SimpleQueue():head_(0), tail_(0) {}
        void clear() {
            std::lock_guard<MTX> guard(lock_);
            head_ = tail_ = 0;
        }
        int push(const void *src, int len) {
            std::lock_guard<MTX> guard(lock_);
            if (capacity() - size() < len) return -1; // not enough free memory
            // [head_, end) 사이에 push 가능
            if (Q_SIZE - head_ >= len) {
                memcpy(data_+head_, src, len);
                head_ = (head_ + len) % Q_SIZE;
                return len;
            }
            // [head_, end), [begin, )사이에 나누어 넣어야 함
            auto copyed = len;
            memcpy(data_+head_, src, Q_SIZE - head_);
            len -= Q_SIZE - head_;
            memcpy(data_, (char*)src + (Q_SIZE - head_), len);
            head_ = len;
            return copyed;
        }

        int pop(void *dest, int len) {
            std::lock_guard<MTX> guard(lock_);
            if (len < 0) return -1; // invalid request

            auto copylen = (len <= size()) ? len : size();
            auto end = (head_ < tail_) ? Q_SIZE : head_;
            // [tail, end) 사이 값만 pop
            if (copylen <= end - tail_) {
                memcpy(dest, data_+tail_, copylen);
                tail_ = (tail_ + copylen) % Q_SIZE;
                return copylen;
            }
            // [tail, end), [begin, ) 나누어서 복사
            auto copyed = copylen;
            memcpy(dest, data_+tail_, end-tail_);
            copylen -= end-tail_;
            memcpy((char*)dest+end-tail_, data_, copylen);
            tail_ = copylen;
            return copyed;
        }

        int size() const
        {
            return  (head_ == tail_) ? 0 : (head_ + Q_SIZE - tail_) % Q_SIZE;
        }

        int capacity() const
        {
            return Q_SIZE - 1;
        }

        int remains() const ///< remained buffer size
        {
            return capacity() - size();
        }
    private:
        int head_;
        int tail_;
        int8_t data_[Q_SIZE];
        mutable MTX lock_;
    };
}
#endif //SEPMASTER_SIMPLE_CIRCULAR_QUEUE_H

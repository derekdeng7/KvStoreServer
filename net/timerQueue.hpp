#ifndef _KVSTORESERVER_NET_TIMERQUEUE_HPP_
#define _KVSTORESERVER_NET_TIMERQUEUE_HPP_

#include "declear.hpp"

#include <vector>
#include <set>

namespace KvStoreServer
{
    class TimerId
    {
    public:
        TimerId()
          : timer_(NULL),
            sequence_(0)
        {}

        TimerId(Timer* timer, int64_t seq)
          : timer_(timer),
            sequence_(seq)
        {}

    friend class TimerQueue;

    private:
        Timer* timer_;
        int64_t sequence_;
    };

    class TimerQueue
    {
    public:
        explicit TimerQueue(std::shared_ptr<EventLoop> loop);
        ~TimerQueue();

        TimerId AddTimer(TimerCallback cb, TimeStamp when, double interval);
        void CancelTimer(TimerId timerId);

    private:
        void AddTimerInLoop(Timer* timer);
        void CancelTimerInLoop(TimerId timerId);

        void HandleRead();
        void HandleWrite();

        std::vector<std::pair<TimeStamp, Timer*>> GetExpired(TimeStamp now);
        void Reset(const std::vector<std::pair<TimeStamp, Timer*>>& expired, TimeStamp now);
        bool Insert(Timer* pItem);

        const int timerfd_;
        std::shared_ptr<EventLoop> loop_;
        std::unique_ptr<Channel>  timerChannel_;    

        std::set<std::pair<TimeStamp, Timer*>> timerSet_;
    };
}

#endif  //_KVSTORESERVER_NET_TIMERQUEUE_HPP_
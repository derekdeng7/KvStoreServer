#include "channel.hpp"
#include "epoll.hpp"
#include "eventLoop.hpp"
#include "timerQueue.hpp"
#include "timer.hpp"
#include "timeStamp.hpp"
#include "unistd.h"

#include <sys/timerfd.h>
#include <inttypes.h>
#include <iostream>

namespace KvStoreServer
{
    int CreateTimerfd()
    {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                TFD_NONBLOCK | TFD_CLOEXEC);
        
        if(timerfd < 0)
        {
            std::cout << "failed in timerfd_create" << std::endl;
        }

        return timerfd;
    }

    struct timespec HowMuchTimeFromNow(TimeStamp when)
    {
        int64_t microseconds = when.MicroSecondsSinceEpoch()
            - TimeStamp::Now().MicroSecondsSinceEpoch();
        if (microseconds < 100)
        {
            microseconds = 100;
        }
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
                microseconds / TimeStamp::kMicroSecondsPerSecond_);
        ts.tv_nsec = static_cast<long>(
                (microseconds % TimeStamp::kMicroSecondsPerSecond_) * 1000);
        return ts;
    }

    void ReadTimerfd(int timerfd, TimeStamp now)
    {
        uint64_t howmany;
        ssize_t n = read(timerfd, &howmany, sizeof(howmany));
        if (n != sizeof(howmany))
        {
            std::cout << "Timer::readTimerfd() error " << std::endl;
        }
    }

    void ResetTimerfd(int timerfd, TimeStamp stamp)
    {
        struct itimerspec newValue;
        struct itimerspec oldValue;

        bzero(&newValue, sizeof(newValue));
        bzero(&oldValue, sizeof(oldValue));

        newValue.it_value = HowMuchTimeFromNow(stamp);
        int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
        if(ret)
        {
            std::cout << "timerfd_settime error" << std::endl;
        }
    }

    TimerQueue::TimerQueue(std::shared_ptr<EventLoop> loop)
        :timerfd_(CreateTimerfd()),
        loop_(loop),
        timerChannel_(new Channel(timerfd_, loop_))
    {
        timerChannel_->SetReadCallback(
            std::bind(&TimerQueue::HandleRead, this)
        );
        timerChannel_->SetWriteCallback(
            std::bind(&TimerQueue::HandleWrite, this)
        );
        timerChannel_->EnableReading();
        timerChannel_->AddChannel();
    }

    TimerQueue::~TimerQueue()
    {
        close(timerfd_);
    }

    TimerId TimerQueue::AddTimer(TimerCallback cb, TimeStamp when, double interval)
    {
        Timer* timer = new Timer(cb, when, interval); 
        loop_->QueueInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
       
        return TimerId(timer, timer->Sequence());
    }

    void TimerQueue::CancelTimer(TimerId timerId)
    {
        loop_->RunInLoop(std::bind(&TimerQueue::CancelTimerInLoop, this, timerId));
    }

    void TimerQueue::AddTimerInLoop(Timer* timer)
    {
        bool earliestChanged = Insert(timer);

        if (earliestChanged)
        {
            ResetTimerfd(timerfd_, timer->Expiration());
        }
    }

    void TimerQueue::CancelTimerInLoop(TimerId timerId)
    {
        for(auto it = timerSet_.begin(); it != timerSet_.end(); ++it)
        {
            if(it->second == timerId.timer_)
            {
                timerSet_.erase(it);
                return;
            }
        }
    }

    void TimerQueue::HandleRead()
    {
        TimeStamp now(TimeStamp::Now());
        ReadTimerfd(timerfd_, now);

        std::vector<std::pair<TimeStamp, Timer*>> expired = GetExpired(now);
        std::vector<std::pair<TimeStamp, Timer*>>::iterator it;
        for(it = expired.begin(); it != expired.end(); ++it)
        {
            it->second->Timeout();
        }

        Reset(expired, now);
    }

    void TimerQueue::HandleWrite()
    {}

    std::vector<std::pair<TimeStamp, Timer*>>  TimerQueue::GetExpired(TimeStamp now)
    {
        std::vector<std::pair<TimeStamp, Timer*>> expired;
        std::pair<TimeStamp, Timer*> sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
        auto end = timerSet_.lower_bound(sentry);
        copy(timerSet_.begin(), end, back_inserter(expired));
        timerSet_.erase(timerSet_.begin(), end);
        return expired;
    }

    void TimerQueue::Reset(const std::vector<std::pair<TimeStamp, Timer*>>& expired, TimeStamp now)
    {
        std::vector<std::pair<TimeStamp, Timer*>>::const_iterator it;
        for(it = expired.begin(); it != expired.end(); ++it)
        {
            if(it->second->IsRepeat())
            {
                it->second->MoveToNext();
                Insert(it->second);
            }
        }

        TimeStamp nextExpire;
        if(!timerSet_.empty())
        {
            nextExpire = timerSet_.begin()->second->Expiration();
        }
        if(nextExpire.Valid())
        {
            ResetTimerfd(timerfd_, nextExpire);
        }
    }

    bool TimerQueue::Insert(Timer* pTimer)
    {
        bool earliestChanged = false;
        TimeStamp when = pTimer->Expiration();
        std::set<std::pair<TimeStamp, Timer*>>::iterator it = timerSet_.begin();

        if(it == timerSet_.end() || when < it->first)
        {
            earliestChanged = true;
        }

        auto result = timerSet_.insert(std::pair<TimeStamp, Timer*>(when, pTimer));
        if(!(result.second))
        {
            std::cout << "timerSet_.insert() error " << std::endl;
        }

        return earliestChanged;
    }
}


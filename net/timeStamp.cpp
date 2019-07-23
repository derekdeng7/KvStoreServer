#include <inttypes.h>
#include <sys/time.h>
#include <stdio.h>

#include "timeStamp.hpp"

namespace KvStoreServer{

    TimeStamp::TimeStamp()
    :microSecondsSinceEpoch_(0)
    {}

    TimeStamp::TimeStamp(double microSeconds)
    :microSecondsSinceEpoch_(microSeconds)
    {}

    TimeStamp::~TimeStamp()
    {}

    void TimeStamp::Init()
    {
        microSecondsSinceEpoch_ = NowMicroSeconds();
    }

    bool TimeStamp::Valid()
    {
        return microSecondsSinceEpoch_ > 0;
    }

    int64_t TimeStamp::MicroSecondsSinceEpoch()
    {
        return microSecondsSinceEpoch_;
    }

    int64_t TimeStamp::MicroSecondsSinceInit()
    {
        return static_cast<int64_t>(TimeStamp::NowMicroSeconds()) - microSecondsSinceEpoch_;
    }

    std::string TimeStamp::ToString() const
    {
        int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond_;
        int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond_;
        std::string res = seconds + "." + microseconds;
        
        return std::move(res);
    }

    TimeStamp TimeStamp::Now()
    {
        return TimeStamp(TimeStamp::NowMicroSeconds());
    }

    TimeStamp TimeStamp::NowAfter(double seconds)
    {
        return TimeStamp(TimeStamp::NowMicroSeconds() + kMicroSecondsPerSecond_ * seconds);
    }

    double TimeStamp::NowMicroSeconds()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        return seconds * kMicroSecondsPerSecond_ + tv.tv_usec;
    }

}
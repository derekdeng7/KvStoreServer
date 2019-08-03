#ifndef _KVSTORESERVER_NET_TIMESTAMP_HPP_
#define _KVSTORESERVER_NET_TIMESTAMP_HPP_

#include "declear.hpp"

#include <sys/types.h>
#include <string>

namespace KvStoreServer{
    
    class TimeStamp
    {
    public:
        TimeStamp();
        explicit TimeStamp(double microSeconds);
        ~TimeStamp();

        void Init();
        bool Valid();
        int64_t MicroSecondsSinceEpoch();
        int64_t MicroSecondsSinceInit();
        std::string ToString() const;

        static TimeStamp Now();
        static TimeStamp NowAfter(double seconds);
        static double NowMicroSeconds();
       
        static const int kMicroSecondsPerSecond_ = 1000 * 1000;

    private:
        int64_t microSecondsSinceEpoch_;
    };

    static bool operator<(TimeStamp l, TimeStamp r)
    {
        return l.MicroSecondsSinceEpoch() < r.MicroSecondsSinceEpoch();
    }

    static bool operator==(TimeStamp l, TimeStamp r)
    {
        return l.MicroSecondsSinceEpoch() == r.MicroSecondsSinceEpoch();
    }

}

#endif //_KVSTORESERVER_NET_TIMESTAMP_HPP_
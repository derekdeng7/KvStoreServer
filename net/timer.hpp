#ifndef _KVSTORESERVER_TIMER_HPP_
#define _KVSTORESERVER_TIMER_HPP_

#include "../include/callback.hpp"
#include "timeStamp.hpp"

namespace KvStoreServer{

	static int64_t CreateSequence()
	{
		std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::system_clock::now().time_since_epoch());
        
		return static_cast<int64_t>(ms.count());
	}

	class Timer
	{
		public:
		Timer(TimerCallback timerCallback, TimeStamp when, double interval)
		   :timerCallback_(timerCallback),
            expiration_(when),
			interval_(interval),
			sequence_(CreateSequence())
    	{}

        TimeStamp Expiration()
        {
            return expiration_;
        }

        int64_t Sequence()
        {
            return sequence_;
        }

        void Timeout()
        {
            timerCallback_();
        }

        bool IsRepeat()
        {
            return interval_ > 0.0;
        }

        void MoveToNext()
        {
            expiration_ = TimeStamp::NowAfter(interval_);
        }

    private:
        TimerCallback timerCallback_; 
        TimeStamp expiration_;     
        const double interval_;
        const int64_t sequence_;
	};

}

#endif //_KVSTORESERVER_TIMER_HPP_
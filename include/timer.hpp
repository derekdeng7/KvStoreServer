#ifndef _KVSTORESERVER_TIMER_HPP_
#define _KVSTORESERVER_TIMER_HPP_

#include<chrono>

namespace KvStoreServer{

class Timer
{
public:
	Timer() : m_begin(std::chrono::high_resolution_clock::now())
     {}

	void reset() 
    { 
        m_begin = std::chrono::high_resolution_clock::now();
     }

	int64_t elapsed_milli() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	double elapsed_second() const
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_nano() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	int64_t elapsed_hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};

}

#endif //_KVSTORESERVER_TIMER_HPP_
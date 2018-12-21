#ifndef _KVSTORESERVER_THREADGUARD_HPP
#define _KVSTORESERVER_THREADGUARD_HPP

#include <iostream>
#include <thread>

namespace KvStoreServer {

	class ThreadGuard 
	{
	public:
		ThreadGuard(std::thread thd);
		~ThreadGuard();
		ThreadGuard(const ThreadGuard&) = delete;
		ThreadGuard& operator =(const ThreadGuard&) = delete;

	private:
		std::thread thd_;
	};
 
}

#endif //_KVSTORESERVER_THREADGUARD_HPP
#ifndef _KVSTORESERVER_SCOPEDTHREAD_HPP
#define _KVSTORESERVER_SCOPEDTHREAD_HPP

#include <iostream>
#include <thread>

namespace KvStoreServer {

	class ScopedThread 
	{
	public:
		ScopedThread(std::thread thd);
		~ScopedThread();
		ScopedThread(const ScopedThread&) = delete;
		ScopedThread& operator =(const ScopedThread&) = delete;

	private:
		std::thread thd_;
	};
 
}

#endif //_KVSTORESERVER_SCOPEDTHREAD_HPP
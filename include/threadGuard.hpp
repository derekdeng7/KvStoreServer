#ifndef _KVSTORESERVER_THREADGUARD_HPP
#define _KVSTORESERVER_THREADGUARD_HPP

#include <iostream>
#include <thread>

namespace KvStoreServer {

    class ThreadGuard 
    {
    public:
        ThreadGuard(std::thread thd) : thd_(std::move(thd))
        {
            if(!this->thd_.joinable())
            {
                throw std::logic_error("No thread!");
            }
            std::cout << "[i] thread id :" << std::this_thread::get_id() << " construct ThreadGuard..."  << std::endl;
        }

        ~ThreadGuard()
        {
            this->thd_.join();
            std::cout << "[i] thread id :" << std::this_thread::get_id() << " deconstruct ThreadGuard..." << std::endl;
        }
      
        ThreadGuard(const ThreadGuard&) = delete;
        ThreadGuard& operator =(const ThreadGuard&) = delete;

    private:
        std::thread thd_;
    };
 
}

#endif //_KVSTORESERVER_THREADGUARD_HPP

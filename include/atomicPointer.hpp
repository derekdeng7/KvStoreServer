#ifndef _KVSTORESERVER_ATOMICPOINTER_HPP_
#define _KVSTORESERVER_ATOMICPOINTER_HPP_

#include <atomic>

namespace KvStoreServer{

    class AtomicPointer
    {
    public:
        AtomicPointer(){}
        explicit AtomicPointer(void* v) : rep_(v) { }
        
        void* AcquireLoad() const 
        {
            return rep_.load(std::memory_order_acquire);
        }
        
        void ReleaseStore(void* v) 
        {
            rep_.store(v, std::memory_order_release);
        }
        
        void* NoBarrierLoad() const 
        {
            return rep_.load(std::memory_order_relaxed);
        }
        
        void NoBarrierStore(void* v) 
        {
            rep_.store(v, std::memory_order_relaxed);
        }

    private:
        std::atomic<void*> rep_;
    };
}

#endif //_KVSTORESERVER_ATOMICPOINTER_HPP_

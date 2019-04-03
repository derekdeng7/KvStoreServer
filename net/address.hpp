#ifndef _KVSTORESERVER_NET_ADDRESS_HPP_
#define _KVSTORESERVER_NET_ADDRESS_HPP_

#include "declear.hpp"

#include <string>
#include <cassert>
#include <arpa/inet.h>

namespace KvStoreServer{

    class Address
    {
    public:
        Address(uint16_t port, uint32_t ip) : port_(port), ip_(ip) 
        {}

        uint16_t Port() const 
        {
             return port_; 
        }

        uint32_t Ip() const 
        { 
            return ip_; 
        }

        std::string ToString() const
        {
            char buf[MaxLen];
            assert(inet_ntop(AF_INET, &ip_, buf, MaxLen));
            return std::string(buf) + " port " + std::to_string(port_);
        }

    private:
        static const uint32_t MaxLen = 16;
        uint16_t port_;
        uint32_t ip_;  
    };
}

#endif //_KVSTORESERVER_NET_ADDRESS_HPP_

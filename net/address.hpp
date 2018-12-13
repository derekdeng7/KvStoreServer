#ifndef _NETWORK_ADDRESS_HPP_
#define _NETWORK_ADDRESS_HPP_

#include <string>
#include <cassert>
#include <arpa/inet.h>

#include "declear.hpp"

namespace Network {

class Address
{
public:
        Address() : port_((u_int16_t)0), ip_((u_int32_t)0) {}

        Address(uint16_t port, uint32_t ip) : port_(port), ip_(ip) {}

        Address(uint16_t port, const char* ip) : port_(port)
        {
                assert(0 != inet_pton(AF_INET, ip, &ip_));
        }


        uint16_t Port() const { return port_; }

        uint32_t Ip() const { return ip_; }

        std::string ToString() const
        {
                char buf[MaxLen];
                assert(inet_ntop(AF_INET, &ip_, buf, MaxLen));
                return std::string(buf) + ":" + std::to_string(port_);
        }

private:
        static const uint32_t MaxLen = 16;
        uint16_t port_;
        uint32_t ip_;
};

} 

#endif // _NETWORK_ADDRESS_HPP_

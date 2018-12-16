#ifndef _NETWORK_CALLBACK_HPP
#define _NETWORK_CALLBACK_HPP

#include <functional>

namespace Network{

typedef std::function<void()> AcceptCallback;
typedef std::function<void()> ConnectCallback;
typedef std::function<void()> ReadCallback;
typedef std::function<void()> WriteCallback;

}

#endif // _NETWORK_CALLBACK_HPP
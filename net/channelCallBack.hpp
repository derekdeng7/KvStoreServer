#ifndef _NETWORK_CHANNELCALLBACK_HPP_
#define _NETWORK_CHANNELCALLBACK_HPP_

namespace Network{

class ChannelCallBack
{
public:
    void virtual HandleReading() = 0;
    void virtual HandleWriting() = 0;
};

}

#endif //_NETWORK_CHANNELCALLBACK_HPP_
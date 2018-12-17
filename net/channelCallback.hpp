#ifndef _KVSTORESERVER_CHANNELCallback_HPP_
#define _KVSTORESERVER_CHANNELCallback_HPP_

namespace KvStoreServer{

class ChannelCallback
{
public:
    void virtual HandleReading() = 0;
    void virtual HandleWriting() = 0;
};

}

#endif //_KVSTORESERVER_CHANNELCallback_HPP_
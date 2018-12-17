#ifndef _KVSTORESERVER_DEFINE_HPP_
#define _KVSTORESERVER_DEFINE_HPP_

//#define MAX_LINE 100
//#define MAX_EVENTS 500
//#define MAX_LISTENFD 5

namespace KvStoreServer{
    const int MAX_EVENTS = 1024;
    const int FD_SIZE = 1024;
    const int BUF_SIZE = 1024;
    const int MESSAGE_SIZE = 1024;
}

#endif //_KVSTORESERVER_DEFINE_HPP_
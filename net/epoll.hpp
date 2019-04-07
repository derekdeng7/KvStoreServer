#ifndef _KVSTORESERVER_NET_EPOLL_HPP_
#define _KVSTORESERVER_NET_EPOLL_HPP_

#include "channel.hpp"
#include "declear.hpp"
#include "../include/base.hpp"

#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>
#include <iostream>
#include <map>

namespace KvStoreServer{

  class Epoll
  {
  public:
      Epoll();
      ~Epoll();

      void DoEpoll(std::vector<Channel*>* pChannels);
      void AddChannel(Channel* pChannel);
      void RemoveChannel(Channel* pChannel);
      void UpdateChannel(Channel* pChannel);
   
  private:   
      int epollfd_;
      epoll_event events_[MAXEVENTNUM];
    
  };

}

#endif //_KVSTORESERVER_NET_EPOLL_HPP_

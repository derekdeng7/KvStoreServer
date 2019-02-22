#ifndef _KVSTORESERVER_EPOLL_HPP_
#define _KVSTORESERVER_EPOLL_HPP_

#include <sys/epoll.h>
#include <vector>
#include <fcntl.h>
#include <iostream>

#include <map>

#include "channel.hpp"
#include "declear.hpp"
#include "define.hpp"


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
      epoll_event events_[MAX_EVENTS];
    
  };

}

#endif //_KVSTORESERVER_EPOLL_HPP_

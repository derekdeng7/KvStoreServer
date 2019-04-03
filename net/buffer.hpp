#ifndef _KVSTORESERVER_NET_BUFFER_HPP_
#define _KVSTORESERVER_NET_BUFFER_HPP_

#include "declear.hpp"
#include "../include/base.hpp"

#include <string>

namespace KvStoreServer{

    class Buffer
    {
    public:
        Buffer(){}
        ~Buffer(){}

        const char* GetChar() const
        {
            return buf_.c_str();
        }

        size_t DataSize() const
        {
            return static_cast<size_t>(buf_.size());
        }

        void Retrieve(size_t len)
        {
            buf_ = buf_.substr(len, buf_.size());
        }

        void Append(const std::string& buf)
        {
            buf_.append(buf);
        }

        std::string RetriveAllAsString()
        {
            return RetriveAsString(DataSize());
        }
        
        std::string RetriveAsString(size_t len)
        {
            std::string result(GetChar(), len);
            Retrieve(len);
            return result;
        }

    private:
        std::string buf_;

    };

}

#endif //_KVSTORESERVER_NET_BUFFER_HPP_

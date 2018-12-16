#ifndef _NETWORK_BUFFER_HPP_
#define _NETWORK_BUFFER_HPP_

#include <string>

#include "declear.hpp"
#include "define.hpp"

namespace Network{

class Buffer
{
public:
    Buffer();
    ~Buffer();

    const char* GetChar() const;
    int DataSize() const;
    void Retrieve(size_t len);
    void Append(const std::string& buf);
    std::string RetriveAllAsString();
    std::string RetriveAsString(size_t len);

private:
    std::string buf_;

};

}

#endif //_NETWORK_BUFFER_HPP_
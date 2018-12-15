#include "buffer.hpp"

namespace Network{

    Buffer::Buffer()
    {}

    Buffer::~Buffer()
    {}

    const char* Buffer::GetChar() const
    {
        return buf_.c_str();
    }

    int Buffer::DataSize() const
    {
        return static_cast<int>(buf_.size());
    }

    void Buffer::Retrieve(size_t len)
    {
        buf_ = buf_.substr(len, buf_.size());
    }

    void Buffer::Append(const std::string& buf)
    {
        buf_.append(buf);
    }

    std::string Buffer::RetriveAllAsString()
    {
        return RetriveAsString(DataSize());
    }
    
    std::string Buffer::RetriveAsString(size_t len)
    {
        std::string result(GetChar(), len);
        Retrieve(len);
        return result;
    }
}
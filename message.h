#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <unistd.h>
#include <boost/noncopyable.hpp>

/*
 * 0 normal message
 */
class Message : public boost::noncopyable
{
public:
    Message(const char *str, size_t len = 0, unsigned short type = 0);

    ~Message() 
    { 
        if (data_) delete[] data_; 
        if (bytes_data_) delete[] bytes_data_;
    }

    char* getMsg() const { return data_; }

    char* getBytes() const { return bytes_data_; }

    size_t getBytesLen() const { return bytes_len_; }
    
    const unsigned short getType() const { return type_; }

private:
    void serialize();

    void unserialize();
private:
    unsigned short type_;
    char* data_;
    char* bytes_data_;
    size_t len_;
    size_t bytes_len_;
};

#endif

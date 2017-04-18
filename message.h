#include <unistd.h>

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/*
 * 0 normal message
 */
class Message
{
public:
    Message(const char *str = NULL, unsigned short type = 0);

    void serialize(char* &str, size_t &len);

    void unserialize(char* str, size_t len);

    ~Message() { if (data_) delete[] data_; }

    const char* getData() const { return data_; }
    
    const unsigned short getType() const { return type_; }

private:
    unsigned short type_;
    char* data_;
};

#endif

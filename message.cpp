#include "message.h"
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

Message::Message(const char *str, size_t len, unsigned short type) : type_(type), data_(NULL), bytes_data_(NULL), len_(0), bytes_len_(len)
{
    if (str)
    {
        if (!len) 
        {
            len_ = strlen(str)+1;
            data_ = new char[len_];
            memcpy(data_, str, len_);
            serialize();
        }
        else
        {
            bytes_data_ = new char[bytes_len_];
            memcpy(bytes_data_, str, len);
            unserialize();
        }
    }
}

void Message::serialize()
{
    if (!len_ || !data_)
        return;
    if (bytes_len_ && bytes_data_)
    {
        bytes_len_ = 0;
        delete[] bytes_data_;
    }
    unsigned short type = htons(type_);
    bytes_len_ = sizeof(type_) + len_;
    bytes_data_ = new char[bytes_len_];
    memcpy(bytes_data_, &type, sizeof(type));
    memcpy(bytes_data_ + sizeof(type), data_, len_);
}

void Message::unserialize()
{
    if (!bytes_len_ || !bytes_data_)
        return;
    if (len_ && data_)
    {
        len_ = 0;
        delete[] data_;
    }
    memcpy(&type_, bytes_data_, sizeof(type_));
    type_ = ntohs(type_);
    len_ = bytes_len_ - sizeof(type_);
    data_ = new char[len_];
    memcpy(data_, bytes_data_ + sizeof(type_), len_);
}


/*int main()
{
    Message msg("hello Worldgewagewg");
    const char* ptr = msg.getBytes();
    for (int i = 0; i < msg.getBytesLen(); i++)
    {
        printf("%c", *(ptr+i));
    }
    putchar(10);
    Message msg2(msg.getBytes(), msg.getBytesLen());
    printf("%s\n", msg2.getMsg());
    return 0;
}
*/

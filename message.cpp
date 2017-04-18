#include "message.h"
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

Message::Message(const char *str, unsigned short type) : type_(type)
{
    if (str)
    {
        data_ = new char[strlen(str)+1];
        memset(data_, 0, strlen(str)+1);
        strcpy(data_, str);
    }
    else
    {
        data_ = NULL; 
    }
}

void Message::serialize(char* &str, size_t &len)
{
    len = sizeof(type_) + strlen(data_) + 1;
    str = new char[len];
    memset(str, 0, len * sizeof(char));
    unsigned short type = htons(type_);
    char *tmp_ptr = (char *)&type;
    str[0] = *tmp_ptr;
    str[1] = *(tmp_ptr+1);
    strcat(str + 2, data_);
}

void Message::unserialize(char* str, size_t len)
{
    char *tmp_ptr = (char *)&type_;
    tmp_ptr[0] = str[0];
    tmp_ptr[1] = str[1];
    type_ = ntohs(type_);
    data_ = new char[len-2];
    strncpy(data_, str+2, len-2); 
}

/*
int main()
{
    Message msg("hello", 20);
    char* ptr = NULL;
    size_t len = 0;
    msg.serialize(ptr, len);
    Message msg2;
    msg2.unserialize(ptr, len);
    delete[] ptr;
    return 0;
}
*/

#include "common.h"
#include <list>

#ifndef _CHAT_SERVER_H_
#define _CHAT_SERVER_H_

class Server
{
private:
    fd_t server_;
    std::list<fd_t> clients_;
public:
    int start();
    Server();
    ~Server();
};

#endif

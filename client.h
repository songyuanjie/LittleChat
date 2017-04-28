#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "poller.h"

class Client : public PollHandler
{
private:
    fd_t client_;
    char *nickname_;
    bool connected_;
public:
    Client();

    virtual ~Client();

    int connectToServer(const char *ip, unsigned short port);

    int work();

    fd_t getClientFd();

    void setNickname(const char* name);

    void handleInput(IPoller *poller, fd_t fd);
};

#endif

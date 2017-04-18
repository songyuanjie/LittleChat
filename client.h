#include "common.h"

class Client
{
private:
    fd_t client_;
    char *nickname_;
    bool connected_;
public:
    Client();

    ~Client();

    int connectToServer(const char *ip, unsigned short port);

    void recvMessage();

    void sendMessage();

    fd_t getClientFd();

    void setNickname(const char* name);
};

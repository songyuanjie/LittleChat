#include "common.h"

class Client
{
private:
    fd_t client_;
public:
    Client();
    ~Client();
    int connectToServer(const char *ip, unsigned short port);
};

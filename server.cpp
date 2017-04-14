#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <memory.h>
#include "server.h"

Server::Server() : server_(-1) 
{
    printf("server cons\n");
}

int Server::start()
{
    int res;
    try {
        server_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_ == -1)
            throw SocketException();
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8888);
        server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        res = bind(server_, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (res == -1)
            throw SocketException();
        res = listen(server_, 10);
        if (res == -1)
            throw SocketException();
        while (true) {
            fd_t acceptor = accept(server_, NULL, NULL);
            if (acceptor == -1)
                throw SocketException();
            clients_.push_back(acceptor);
            res = 0;
            printf("a client connected!\n");
        }
    } catch(SocketException)
    {
        res = -1;
        if (server_)
            close(server_);
    }
    return res;
}

Server::~Server()
{
    if (server_ > 0)
        close(server_);
    std::list<fd_t>::iterator it = clients_.begin();
    while (it != clients_.end())
    {
        close(*it);
        ++it;
    }
    printf("server dest\n");
}

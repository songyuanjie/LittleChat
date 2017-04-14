#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <memory.h>
#include "thread.h"
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
        res = 0;
    } catch(SocketException)
    {
        res = -1;
        if (server_)
            close(server_);
    }
    return res;
}

void Server::acceptConnection()
{
    while (true) {
        fd_t acceptor = accept(server_, NULL, NULL);
        if (acceptor == -1)
            throw SocketException();
        clients_.push_back(acceptor);
        printf("Log: a client connected!\n");
    }
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

void* acceptFunc(void *arg)
{
    Server *server_ptr = (Server *)arg;
    server_ptr->acceptConnection();
    return NULL;
}

int main()
{
    Server server;
    int res = server.start();
    if (res != -1)
        printf("Server has started!\n");
    else
        printf("Server start failed.\n");

    Thread accept_thread(&acceptFunc, (void *)&server);
    accept_thread.start();

    char op;
    do
    {
        op = getchar();
        
    } while (op != 'q');
    return 0;
}



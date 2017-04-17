#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <memory.h>
#include "mutex.h"
#include "thread.h"
#include "server.h"

Server::Server() : server_(-1) 
{
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

void Server::sendMessage(fd_t client, char *message)
{
    send(client, message, strlen(message), 0);
}

void Server::sendMessageToAll(char *message)
{
    std::list<fd_t>::iterator it = clients_.begin();
    while (it != clients_.end())
    {
        sendMessage(*it, message);
        ++it;
    }
}

void Server::recvMessage()
{
    fd_set read_set; 
    char buf[BUFSIZ];
    struct timeval time_val;
    while (true)
    {
        time_val.tv_sec = 5;
        time_val.tv_usec = 0;
        FD_ZERO(&read_set);
        std::list<fd_t>::iterator it = clients_.begin();
        while (it != clients_.end())
        {
            FD_SET(*it, &read_set);
            ++it;
        }
        int res = select(FD_SETSIZE, &read_set, (fd_set *)NULL, (fd_set *)NULL, &time_val);
        int nread;
        if (res > 0)
        {
            it = clients_.begin();
            if (FD_ISSET(*it, &read_set))
            {
                ioctl(*it, FIONREAD, &nread);
                if (nread == 0)
                {
                    close(*it);
                    clients_.erase(it);
                }
                else
                {
                    recv(*it, buf, nread, 0);
                    printf("client: %s\n", buf);
                }
            }
        }
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
    printf("server destroy\n");
}

void* acceptFunc(void *arg)
{
    printf("acceptConn\n");
    Server *server_ptr = (Server *)arg;
    server_ptr->acceptConnection();
    return NULL;
}

void* recvFunc(void *arg)
{
    printf("recvMessage\n");
    Server *server_ptr = (Server *)arg;
    server_ptr->recvMessage();
    return NULL;
}

int main()
{
    Server server;
    int res = server.start();
    if (res == -1)
    {
        printf("Server start failed.\n");
        perror("server start");
        return 1;
    }

    printf("Server has started!\n");

    Thread accept_thread(&acceptFunc, (void *)&server);
    accept_thread.start();

    Thread recv_thread(&recvFunc, (void *)&server);
    recv_thread.start();

    char op;
    char buf[BUFSIZ];
    do
    {
        op = getchar();
        if (op == '\n')
            continue;
        else if (op == 's')
        {
            //scanf("%s", buf);
            fgets(buf, BUFSIZ, stdin);
            size_t len = strlen(buf);
            if (buf[len-1] == '\n')
                buf[len-1] = '\0';
            server.sendMessageToAll(buf);
        }
        
    } while (op != 'q');
    return 0;
}



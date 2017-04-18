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
        printf("a client connected, id: %d!\n", acceptor);
    }
}

void Server::sendMessage(fd_t client, Message *message)
{
    char *ptr = NULL;
    size_t len = 0;
    message->serialize(ptr, len);
    send(client, ptr, len, 0); 
    delete[] ptr;
}

void Server::sendMessageToAll(Message *message, int except)
{
    //printf("before server send:%s",message);
    std::list<fd_t>::iterator it = clients_.begin();
    while (it != clients_.end())
    {
        if (except != *it)
            sendMessage(*it, message);
        ++it;
    }
}

void Server::recvMessage()
{
    fd_set read_set; 
    char buf[BUFSIZ+1];
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
            while (it != clients_.end())
            {
                if (FD_ISSET(*it, &read_set))
                {
                    nread = recv(*it, buf, BUFSIZ, 0);
                    if (nread <= 0)
                    {
                        printf("a client disconnected, id: %d!\n", *it);
                        close(*it);
                        it = clients_.erase(it);
                    }
                    else
                    {
                        buf[nread] = '\0';
                        Message *msg = new Message;
                        msg->unserialize(buf, nread);
                        sendMessageToAll(msg, *it);
                        delete msg;
                        ++it;
                    }
                }
                else
                    ++it;
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
    Server *server_ptr = (Server *)arg;
    server_ptr->acceptConnection();
    return NULL;
}

void* recvFunc(void *arg)
{
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
        perror("Server start");
        return 1;
    }

    printf("Server has started!\n");

    Thread accept_thread(&acceptFunc, (void *)&server);
    accept_thread.start();

    Thread recv_thread(&recvFunc, (void *)&server);
    recv_thread.start();

    char op[2];
    char buf[BUFSIZ];
    do
    {
        fgets(op, 2, stdin);
        if (op[0] == 's')
        {
            //scanf("%s", buf);
            memset(buf, 0, sizeof(buf));
            fgets(buf, BUFSIZ, stdin);
            size_t len = strlen(buf);
            if (buf[len-1] == '\n')
                buf[len-1] = '\0';
            Message *msg = new Message(buf);
            server.sendMessageToAll(msg);
        }
        
    } while (op[0] != 'q');
    return 0;
}



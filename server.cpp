#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <memory.h>
#include <string>
#include "message.h"
#include "mutex.h"
#include "thread.h"
#include "server.h"

Server::Server() : server_(0), start_(false)
{
    int ret = pipe(pipefd_);
    if (ret == -1)
        throw std::string("pipe error");
}

int Server::start()
{
    int ret;
    try {
        server_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_ == -1)
            throw std::string("Server::start() : create socket failed");
        int reuse = 1;
        setsockopt(server_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8888);
        server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        ret = bind(server_, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret == -1)
            throw std::string("Server::start() : bind failed");
        ret = listen(server_, 10);
        if (ret == -1)
            throw std::string("Server::start() : listen failed");
        ret = 0;
        start_ = true;
    } catch(std::string)
    {
        ret = -1;
        if (server_ > 0)
            close(server_);
    }
    return ret;
}

Server::~Server()
{
    if (server_ > 0)
        close(server_);
    for (ClientInfoIter it = clients_.begin(); it != clients_.end(); ++it)
    {
        close(it->first);
    }
}

void Server::stop()
{
    start_ = false; 
    if (server_ > 0)
        close(server_);
    server_ = 0;
    char ch = 0;
    write(pipefd_[1], &ch, 1);
}

void Server::work()
{
    boost::shared_ptr<IPoller> poller(new EPoller);
    poller->addFd(server_, IPoller::POLL_READ);
    poller->addFd(pipefd_[0], IPoller::POLL_READ);
    poller->setHandler(this);
    while (start_) {
        /*printf("poll clients:\n");
        }*/
        poller->poll();
    }
}

void Server::handleInput(IPoller *poller, fd_t fd)
{
    if (fd == server_)
    {
        ClientInfoPtr client_p(new ClientInfo); 
        struct sockaddr_in *client_addr_p = &(client_p->addr_);
        socklen_t addr_len = sizeof(*client_addr_p);
        int acceptor = accept(server_, (struct sockaddr *)client_addr_p, &addr_len);
        if (acceptor == -1)
            return;
        client_p->fd_ = acceptor;
        memset(client_p->nickname_, 0, sizeof(client_p->nickname_));
        sprintf(client_p->nickname_, "default%d", acceptor);
        int ret =  poller->addFd(acceptor, IPoller::POLL_READ);
        if (ret == -1)
        {
            close(acceptor);
            return;
        }
        clients_[acceptor] = client_p;
        printf("a client connected!id:%d\n", acceptor);
    }
    else if (fd == pipefd_[0])
    {

    }
    else
    {
        char buf[BUF_SIZE_MAX];
        memset(buf, 0, sizeof(buf));
        int ret = recv(fd, buf, BUF_SIZE_MAX, 0); 
        if (ret <= 0)
        {
            close(fd);
            clients_.erase(fd);
            printf("a client disconnected! id : %d\n", fd);
            return;
        }
        Message msg(buf, ret);
        if (msg.getType() == 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%s: %s", clients_[fd]->nickname_, msg.getMsg());
            printf("%s", buf);
            Message nickmsg(buf);
            for (ClientInfoIter it = clients_.begin(); it != clients_.end(); ++it)
            {
                if (it->first != fd)
                    send(it->first, nickmsg.getBytes(), nickmsg.getBytesLen(), 0); 
            }
        }
        else if (msg.getType() == 2)
        {
            strcpy(clients_[fd]->nickname_, msg.getMsg());
        }
    }
}

void Server::handleError(IPoller *poller, fd_t fd)
{
    printf("handler error");
    close(fd);
    clients_.erase(fd);
    printf("a client disconnected! id : %d\n", fd);
}

void* thread_func(void *arg)
{
    Server *server_p = (Server *)arg;
    server_p->work();
    return NULL;
}

int main()
{
    Server server;
    int ret = server.start();
    if (ret == -1)
    {
        perror("Server start");
        return 1;
    }
    printf("Server has started!\n");
    Thread thread(&thread_func, &server);
    thread.start();
    char op[10];
    while(true)
    {
        scanf("%s", op);
        if (!strncmp(op, "quit", 4))
        {
            server.stop();
            break;
        }
        else
            printf("invalid operation!\n");
    }
    thread.join();
    return 0;
}



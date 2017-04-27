#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <boost/shared_ptr.hpp>
#include "client.h"
#include "message.h"

Client::Client() : client_(0), nickname_(NULL), connected_(false)
{

}

Client::~Client() 
{
    if (client_ > 0)
        close(client_);
    if (nickname_)
        delete[] nickname_;
    printf("client destroy\n");
}

void Client::setNickname(const char* name)
{
    size_t len = strlen(name) + 1;
    if (nickname_)
        delete[] nickname_;
    nickname_ = new char[len];
    strcpy(nickname_, name);
    Message msg(name, 0, 2); 
    send(client_, msg.getBytes(), msg.getBytesLen(), 0);
}

int Client::connectToServer(const char *ip, unsigned short port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    client_ = socket(AF_INET, SOCK_STREAM, 0);
    int res = connect(client_, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (res == -1)
    {
        if (client_ > 0)
            close(client_);
        return res;
    }
    connected_ = true;
    return 0;
}

void Client::handleInput(IPoller *poller, fd_t fd)
{
    char buf[BUF_SIZE_MAX];
    if (fd == 0)
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, BUFSIZ, stdin);
        if (!strncmp(buf, "quit", 4))
        {
            connected_ = false;
            return; 
        }
        Message msg(buf);
        send(client_, msg.getBytes(), msg.getBytesLen(), 0);
    }
    else if (fd == client_)
    {
        memset(buf, 0, sizeof(buf));
        int ret = recv(client_, buf, BUFSIZ-1, 0);
        if (ret <= 0)
        {
            close(client_);
            connected_ = false;
            printf("connection lost!\n");
            return;
        }
        Message msg(buf, ret);
        printf("%s", msg.getMsg());
    }
}

int Client::work()
{
    boost::shared_ptr<IPoller> poller(new EPoller);
    poller->addFd(client_, IPoller::POLL_READ);
    poller->addFd(0, IPoller::POLL_READ);
    poller->setHandler(this);
    while(connected_)
    {
        poller->poll();
    } 
    return 1;
}

int main(int argc, char* argv[])
{
    int port = 8888;
    const char *ip = "127.0.0.1";
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }
    Client client;
    char nickname[NICKNAME_LEN_MAX];
    printf("Please input your nickname: ");
    fgets(nickname, NICKNAME_LEN_MAX, stdin);
    nickname[strlen(nickname)-1] = '\0';
    int res = client.connectToServer(ip, port);
    if (res == -1)
    {
        printf("connect failed!\n");
        return 1;
    }
    printf("connect success!\n");
    client.setNickname(nickname);
    client.work();
    return 0;
}

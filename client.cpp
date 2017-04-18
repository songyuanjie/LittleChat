#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#include "thread.h"
#include "message.h"
#include "mutex.h"

Client::Client() : client_(-1), nickname_(NULL), connected_(false)
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
}

void Client::recvMessage()
{
    char buf[BUFSIZ+103];
    memset(buf, 0, sizeof(buf));
    while (connected_)
    {
        int nread = recv(client_, buf, BUFSIZ, 0);
        if (nread <= 0)
        {
            connected_ = false;
            break;
        }
        buf[nread] = '\0';
        Message msg;
        msg.unserialize(buf, nread);
        if (msg.getType() == 0)
        {
            printf("%s\n", msg.getData());
        }
    }
}

void Client::sendMessage()
{
    char write_buf[BUFSIZ+103];
    memset(write_buf, 0, sizeof(write_buf));
    strcpy(write_buf, nickname_);
    strcat(write_buf, ": ");
    size_t offset = strlen(nickname_) + 2;
    while (connected_)
    {
        fgets(write_buf+offset, BUFSIZ, stdin);
        size_t len = strlen(write_buf);
        if (write_buf[len-1] == '\n')
            write_buf[len-1] = '\0';
        if (!strncmp(write_buf+offset, "bye", 3))  
            break;  
        Message msg(write_buf);
        char *ptr = NULL;
        msg.serialize(ptr, len);
        send(client_, ptr, len, 0);
        delete[] ptr;
    }
    if (!connected_)
    {
        printf("Notice: Connection lost, client closed.\n");
    }
}

int Client::connectToServer(const char *ip, unsigned short port)
{
    client_ = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_family = AF_INET;
    //printf("%d %d %d", sizeof(server_addr.sin_port), sizeof(server_addr.sin_addr), sizeof(server_addr.sin_family));
    socklen_t server_len = sizeof(server_addr);
    int res = connect(client_, (struct sockaddr *)&server_addr, server_len);
    if (res == -1)
    {
        close(client_);
        return res;
    }
    connected_ = true;
    return 0;
}

void* recvData(void *arg)
{
    printf("thread start\n");
    Client *client_ptr = (Client *)arg; 
    client_ptr->recvMessage();
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    Client client;
    char nickname[100];
    printf("Please input your nickname: ");
    fgets(nickname, 100, stdin);
    nickname[strlen(nickname)-1] = '\0';
    client.setNickname(nickname);
    int res = client.connectToServer("127.0.0.1", 8888);
    if (res == -1)
    {
        printf("connect failed!\n");
        return 1;
    }
    printf("connect success!\n");
    Thread recvThread(&recvData, (void *)&client);
    recvThread.start();
    client.sendMessage();
    return 0;
}

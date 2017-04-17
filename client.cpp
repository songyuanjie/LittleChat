#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#include "thread.h"
#include "mutex.h"

Client::Client() : client_(-1)
{

}

Client::~Client() 
{
    if (client_ > 0)
        close(client_);
    printf("client destroy\n");
}

void Client::recvMessage()
{
    char buf[BUFSIZ+1];
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
        fprintf(stdout, "%s\n", buf);
    }
}

void Client::sendMessage()
{
    char write_buf[BUFSIZ+1];
    while (connected_)
    {
        fgets(write_buf, BUFSIZ, stdin);
        size_t len = strlen(write_buf);
        if (write_buf[len-1] == '\n')
            write_buf[len-1] = '\0';
        if (!strncmp(write_buf, "bye", 3))  
            break;
        //printf("before client send:%s",write_buf);
        send(client_, write_buf, len, 0);
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

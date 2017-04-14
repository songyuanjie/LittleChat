#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

Client::Client() : client_(-1)
{

}

Client::~Client() 
{
    if (client_ > 0)
        close(client_);
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
    return 0;
}

int main()
{
    Client client;
    int res = client.connectToServer("127.0.0.1", 8888);

    if (res == -1)
        printf("connect failed!\n");
    else
        printf("connect success!\n");

    return 0;
}

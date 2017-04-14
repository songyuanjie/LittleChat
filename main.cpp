#include "server.h"

int main()
{
    Server server;
    server.start();
    char op;
    do
    {
        op = getchar();
        if (op == '\n')
            continue;
    } while (op != 'q');
    
    return 0;
}

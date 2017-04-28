#ifndef _SERVER_H_
#define _SERVER_H_

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "common.h"
#include "poller.h"

class Server : public PollHandler
{
private:
    struct ClientInfo
    {
        fd_t fd_;
        char nickname_[NICKNAME_LEN_MAX];
        struct sockaddr_in addr_;
    };
    typedef boost::shared_ptr<ClientInfo> ClientInfoPtr;
    typedef boost::unordered_map<fd_t, ClientInfoPtr> ClientContainer;
    typedef ClientContainer::iterator ClientInfoIter;

    fd_t server_;
    bool start_;
    ClientContainer clients_;
    int pipefd_[2];
public:
    int start();

    void work();

    Server();

    virtual ~Server();

    virtual void handleInput(IPoller *poller, fd_t fd);

    virtual void handleError(IPoller *poller, fd_t fd);

    void stop();
};

#endif

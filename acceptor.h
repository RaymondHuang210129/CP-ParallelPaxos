#include "node.h"

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

class Acceptor {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
    public:
        Acceptor(int port);
        void run(void* arg);
        void terminate();
};

#endif
#include "node.h"

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
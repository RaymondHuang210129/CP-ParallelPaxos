#include "node.h"

#ifndef LEADER_H
#define LEADER_H

class Leader {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
        int port;
        int numberOfCommander;
		std::vector<std::thread> commanderThreads;

    public:
        Leader(int port, int numberOfCommander);
		~Leader();
        void run(void* arg);
        void terminate();
};

#endif

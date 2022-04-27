#include "node.h"

#ifndef LEADER_H
#define LEADER_H

class Leader {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
        int numberOfCommander;
        int threadID;
        int port;
        Entry myEntry;
		std::vector<std::thread> commanderThreads;

    public:
        Leader(Entry myEntry, int numberOfCommander, int threadID, std::vector<Entry>& replicas, std::vector<Entry>& acceptors);
		~Leader();
        void run(void* arg);
        void terminate();
};

#endif

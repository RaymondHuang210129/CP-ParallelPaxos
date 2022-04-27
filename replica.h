#include <set>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include "node.h"
#include "message.h"

#ifndef REPLICA_H
#define REPLICA_H

class Replica {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
        std::set<Command> requests;
        std::map<int, Command> decisions;
        std::map<int, Command> proposals;
        std::vector<std::pair<std::string, int> > leaders;
        int slotIn;
        int slotOut;
        int numThreads;
        int threadId;
        int leaderLoadBalanceIdx; // used in serial replica parallel leader
        int numLeaderThreads; // used in serial replica parallel leader
        int numLeaderInstances; // used in serial replica parallel leader
        void proposeParallel();
        void executeParallel(Command command);
    public:
        Replica(int port, std::vector<Entry> leaders, int numThreads);
        Replica(int port, std::vector<Entry> leaders, int numThreads, int threadId);
        void runParallel(void* arg);
        void runExecuter(void* arg);
        void terminate();
        ~Replica();
};

#endif
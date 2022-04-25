#include <unordered_set>
#include <unordered_map>
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
        std::unordered_set<Command> requests;
        std::unordered_map<int, Command> decisions;
        std::unordered_map<int, Command> proposals;
        std::vector<std::pair<std::string, int> > leaders;
        int slotIn;
        int slotOut;
        void propose();
        void perform(Command);
    public:
        Replica(int port, std::vector<std::pair<std::string, int>> leaders);
        void run(void* arg);
        void terminate();
};

#endif
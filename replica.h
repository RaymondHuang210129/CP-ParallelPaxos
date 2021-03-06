#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "message.h"
#include "node.h"

#ifndef REPLICA_H
#define REPLICA_H

class Replica {
   private:
    Node* node;
    Node* resendNode;
    struct sockaddr_in recvfrom;
    bool shouldTerminate;
    std::set<Command> requests;
    std::map<int, Command> decisions;
    std::map<int, Command> proposals;
    std::vector<std::pair<std::string, int> > leaders;
    std::mutex proposalMutex;
    std::vector<Command>* logsOfThread;
    int slotIn;
    int slotOut;
    int numThreads;
    int threadId;
    int leaderLoadBalanceIdx;  // used in serial replica parallel leader
    int numLeaderThreads;      // used in serial replica parallel leader
    int numLeaderInstances;    // used in serial replica parallel leader
    void proposeParallel();
    void executeParallel(Command command);
    void resendProposal();

   public:
    Replica(int port, std::vector<Entry> leaders, int numThreads);
    Replica(int port, std::vector<Entry> leaders, int numThreads, int threadId);
    void runParallel(void* arg);
    void runExecuter(void* arg);
    void terminate();
    ~Replica();
};

#endif
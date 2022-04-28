#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "node.h"

#ifndef COMMANDER_H
#define COMMANDER_H

class Commander {
   private:
    Node* node;
    struct sockaddr_in recvfrom;
    bool shouldTerminate;
    std::unordered_map<std::string, std::unordered_set<std::string>> waitFor;
    std::vector<std::pair<std::string, int>> acceptors;
    std::vector<std::pair<std::string, int>> replicas;

   public:
    Commander(int port, int leaderThreaID, std::vector<Entry> acceptors,
              std::vector<Entry> replicas);
    ~Commander();
    void run(void* arg);
    void terminate();
};

#endif
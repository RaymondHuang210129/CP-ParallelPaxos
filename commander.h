#include "node.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

class Commander {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
        std::unordered_map<std::string, std::unordered_set<std::string>> waitFor;
        std::vector<std::pair<std::string, int>> acceptors;
        std::vector<std::pair<std::string, int>> replicas;

    public:
        Commander(int port, std::vector<std::pair<std::string, int>> acceptors, 
                            std::vector<std::pair<std::string, int>> replicas);
		~Commander();
        void run(void* arg);
        void terminate();
};

#include "commander.h"

#include <pthread.h>

#include "message.h"
#include "node.h"

Commander::Commander(int port, int leaderThreadID, std::vector<Entry> replicas,
                     std::vector<Entry> acceptors) {
    node = new Node(port);
    for (int i = 0; i < replicas.size(); i++) {
        this->replicas.push_back(std::make_pair(
            replicas[i].address, replicas[i].threadStartPort + leaderThreadID));
    }
    for (int i = 0; i < acceptors.size(); i++) {
        this->acceptors.push_back(
            std::make_pair(acceptors[i].address,
                           acceptors[i].threadStartPort + leaderThreadID));
    }
    memset(&recvfrom, 0, sizeof(recvfrom));
    shouldTerminate = false;
};

Commander::~Commander() { delete node; };

void Commander::run(void *arg) {
    std::thread timerThread([this] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "thread " << this->node->getPort() << ": "
                      << this->waitFor.size() << std::endl;
        }
    });

    while (!shouldTerminate) {
        Message *m = Message::deserialize(
            node->receive_data((struct sockaddr_in *)&recvfrom));
        if (dynamic_cast<Assign *>(m) != nullptr) {
            Assign *assign = dynamic_cast<Assign *>(m);
            waitFor[assign->serialize()] = std::unordered_set<std::string>{};
            Accept accept(assign->getSlot(), assign->getCommand());
            node->broadcast_data(acceptors, accept.serialize());
        } else if (dynamic_cast<Accepted *>(m) != nullptr) {
            Accepted *accepted = dynamic_cast<Accepted *>(m);
            Assign tmpAssign(accepted->getSlot(), accepted->getCommand());
            if (waitFor.count(tmpAssign.serialize()) != 0) {
                std::string ip = inet_ntoa(recvfrom.sin_addr);
                std::string port = std::to_string(htons(recvfrom.sin_port));
                // now get it back and print it
                waitFor[tmpAssign.serialize()].insert(ip + port);
                if (waitFor[tmpAssign.serialize()].size() >=
                    ((acceptors.size() / 2) + 1)) {
                    Decision decision(accepted->getSlot(),
                                      accepted->getCommand());
                    node->broadcast_data(replicas, decision.serialize());
                    waitFor.erase(tmpAssign.serialize());
                }
            }
        }

        delete m;
    }
    return;
}

void Commander::terminate() {
    shouldTerminate = true;
    return;
}
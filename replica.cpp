#include <string>
#include <unordered_map>
#include <iostream>
#include "message.h"
#include "replica.h"
#include <functional>
#include <thread>
#include <cassert>
#include "semaphore.h"

static std::vector<Semaphore*> semaphores;
static std::vector<Command> logs;

/* This constructor is used to create a executer thread of replica. */
Replica::Replica(int port, std::vector<Entry> leaders) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    std::vector<std::pair<std::string, int> > targetLeaders;
    if (numThreads == 1 && leaders.size() > 0 && leaders[0].numThreads > 1) {
        // serial replica parallel leader
        leaderLoadBalanceIdx = 0;
        numLeaderInstances = leaders.size();
        numLeaderThreads = leaders[0].numThreads;
        for (int j = 0; j < leaders[0].numThreads; j++) {
            for (int i = 0; i < leaders.size(); i++) {
                targetLeaders.push_back(std::make_pair(leaders[i].address, leaders[i].threadStartPort + j));
            }
        }
    } else {
        for (int i = 0; i < leaders.size(); i++) {
            targetLeaders.push_back(std::make_pair(leaders[i].address, leaders[i].threadStartPort + threadId));
        }
        this->leaders = targetLeaders;
    }
    shouldTerminate = false;
}

/* This constructor is used to create a handler thread of replica */
Replica::Replica(int port, std::vector<Entry> leaders, int numThreads, int threadId) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    std::vector<std::pair<std::string, int> > targetLeaders;
    for (int i = 0; i < leaders.size(); i++) {
        targetLeaders.push_back(std::make_pair(leaders[i].address, leaders[i].threadStartPort + threadId));
    }
    this->leaders = targetLeaders;
    this->numThreads = numThreads;
    this->threadId = threadId;
    shouldTerminate = false;
    slotIn = threadId;
    slotOut = threadId;
}

Replica::~Replica() {
    delete node;
}

/* This method should be called by handler thread */
void Replica::runParallel(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        Request* request = dynamic_cast<Request*>(m);
        Decision* decision = dynamic_cast<Decision*>(m);
        if (request != nullptr) {
            if (std::hash<std::string>{}(request->serialize()) % numThreads != threadId) {
                return;
            }
            std::cout << "Replica received Request message " << request->serialize() << std::endl;
            requests.insert(request->getCommand());
            proposeParallel();
        } else if (decision != nullptr) {
            if (numThreads != 1 && decision->getSlot() % numThreads != threadId) {
                return;
            }
            std::cout << "Replica received Decision message " << decision->serialize() << std::endl;
            decisions[decision->getSlot()] = decision->getCommand();
            while (decisions.find(slotOut) != decisions.end()) {
                Command decidedCommand = decisions[slotOut];
                if (proposals.find(slotOut) != proposals.end()) {
                    Command proposedCommand = proposals[slotOut];
                    proposals.erase(slotOut);
                    if (proposedCommand.serialize() != decidedCommand.serialize()) {
                        requests.insert(proposedCommand);
                    }
                }
                logs[slotOut] = decidedCommand;
                semaphores[threadId]->notify();
            }
            proposeParallel();
        } 
        delete m;
    }
}

/* This method should be called by executer thread */
void Replica::runExecuter(void* arg) {
    for (int slot = 0;; slot++) {
        semaphores[slot % numThreads]->wait();
        Command command = logs[slot];
        executeParallel(command);
    }
}

void Replica::executeParallel(Command command) {
    Result result(command.getContent(), command.getAddress(), command.getPort());
    Response response(result);
    node->send_data(result.getAddress(), result.getPort(), response.serialize());
}

void Replica::proposeParallel() {
    std::cout << "Replica proposing" << std::endl;
    while (requests.size() > 0) {
        Command command = *requests.begin();
        if (decisions.find(slotIn) == decisions.end()) {
            requests.erase(command);
            proposals[slotIn] = command;
            Propose propose(slotIn, command);
            if (numThreads == 1 && leaders.size() > 0 && numLeaderThreads > 1) {
                std::vector<std::pair<std::string, int> > 
                        targetLeaders(leaders.begin() + leaderLoadBalanceIdx, 
                                      leaders.begin() + leaderLoadBalanceIdx + numLeaderInstances);
                leaderLoadBalanceIdx = (leaderLoadBalanceIdx + numLeaderInstances) % leaders.size(); 
                node->broadcast_data(targetLeaders, propose.serialize());
            } else {
                node->broadcast_data(leaders, propose.serialize());
            }
        }
        slotIn += numThreads;
    }

}

void Replica::terminate() {
    shouldTerminate = true;
    return;
}

int main(int argc, char* argv[]) {
    // 0      1     
    // server [address] [port]

    if(argc != 3) {
        std::cout << "Invalid arguments count. Should enter replica [address] [port]" << std::endl;
        exit(1);
    }

    std::string myAddress = argv[1];
    int myPort = atoi(argv[2]);
    logs.reserve(10000000);

    std::vector<Entry> replicas;
    std::vector<Entry> leaders;
    std::vector<Entry> acceptors;
    read_config(replicas, leaders, acceptors);
    Entry myEntry = getMyEntry(replicas, myAddress, myPort);

    std::vector<std::thread> threads;
    std::vector<std::pair<std::string, int> > localAddresses;
    
    for (int threadId = 0; threadId < myEntry.numThreads; threadId++) {
        if (threadId != myEntry.numThreads - 1) {
            // create threads for handler threads
            threads.emplace_back([&localAddresses, threadId, &myEntry, &leaders]() {
                Replica replica(myEntry.threadStartPort + threadId, leaders, myEntry.numThreads, threadId);
                replica.runParallel(nullptr);
            });
            semaphores.push_back(new Semaphore(0));
        } else {
            // create thread for executer
            threads.emplace_back([&localAddresses, threadId, &myEntry, &leaders]() {
                Replica replica(localAddresses[threadId].second, leaders);
                replica.runExecuter(nullptr);
            });
        }
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    return 0;
}



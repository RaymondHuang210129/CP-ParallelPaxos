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
Replica::Replica(int port, std::vector<std::pair<std::string, int> > leaders) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    this->leaders = leaders;
    shouldTerminate = false;
}

/* This constructor is used to create a handler thread of replica */
Replica::Replica(int port, std::vector<std::pair<std::string, int> > leaders, int numThreads, int threadId) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    this->leaders = leaders;
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
void Replica::run(void* arg) {
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
            propose();
        } else if (decision != nullptr) {
            if (decision->getSlot() % numThreads != threadId) {
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
            propose();
        } 
        delete m;
    }
}

/* This method should be called by executer thread */
void Replica::runExecuter(void* arg) {
    for (int slot = 0;; slot++) {
        semaphores[slot % numThreads]->wait();
        Command command = logs[slot];
        execute(command);
    }
}

void Replica::execute(Command command) {
    Result result(command.getContent(), command.getAddress(), command.getPort());
    Response response(result);
    node->send_data(result.getAddress(), result.getPort(), response.serialize());
}

void Replica::propose() {
    std::cout << "Replica proposing" << std::endl;
    while (requests.size() > 0) {
        Command command = *requests.begin();
        if (decisions.find(slotIn) == decisions.end()) {
            requests.erase(command);
            proposals[slotIn] = command;
            Propose propose(slotIn, command);
            node->broadcast_data(leaders, propose.serialize());
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
    // server [hostname]

    if(argc != 2) {
        std::cout << "Invalid arguments count. Should enter replica [hostname]" << std::endl;
        exit(1);
    }

    std::string hostName = argv[1];
    logs.reserve(10000000);

    std::vector<std::pair<std::string, int> > replicaAddresses;
    std::vector<std::pair<std::string, int> > leaderAddresses;
    std::vector<std::pair<std::string, int> > acceptorAddresses;
    read_config(replicaAddresses, leaderAddresses, acceptorAddresses);

    std::vector<std::thread> threads;
    std::vector<std::pair<std::string, int> > localAddresses;
    
    // check how many threads to create
    int threadCount = 0;
    for (int i = 0; i < replicaAddresses.size(); i++) {
        if (replicaAddresses[i].first == hostName) {
            threadCount++;
            localAddresses.push_back(replicaAddresses[i]);
        }
    }

    for (int threadId = 0; threadId < threadCount; threadId++) {
        if (threadId != threadCount - 1) {
            // create threads for handler threads
            threads.emplace_back([&localAddresses, threadId, &leaderAddresses, &threadCount]() {
                Replica replica(localAddresses[threadId].second, leaderAddresses, threadCount - 1, threadId);
                replica.run(nullptr);
            });
            semaphores.push_back(new Semaphore(0));
        } else {
            // create thread for executer
            threads.emplace_back([&localAddresses, &leaderAddresses, &threadId]() {
                Replica replica(localAddresses[threadId].second, leaderAddresses, 0, 0);
                replica.runExecuter(nullptr);
            });
        }
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    return 0;
}



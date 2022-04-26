#include <string>
#include <iostream>
#include "message.h"
#include "replica.h"
#include <functional>
#include <thread>
#include <cassert>

Replica::Replica(int port, std::vector<std::pair<std::string, int> > leaders) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    this->leaders = leaders;
    shouldTerminate = false;
    slotIn = 0;
    slotOut = 0;
}

Replica::~Replica() {
    delete node;
}

void Replica::run(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        Request* request = dynamic_cast<Request*>(m);
        Decision* decision = dynamic_cast<Decision*>(m);
        if (request != nullptr) {
            std::cout << "Replica received Request message " << request->serialize() << std::endl;
            requests.insert(request->getCommand());
            propose();
        } else if (decision != nullptr) {
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
                perform(decidedCommand);
            }
            propose();
        } 
        delete m;
    }
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
        slotIn++;
    }

}

void Replica::perform(Command command) {
    std::cout << "Replica performing " << command.serialize() << std::endl;
    // dummy result
    slotOut++;
    Result result(command.getContent(), command.getAddress(), command.getPort());
    Response response(result);
    node->send_data(result.getAddress(), result.getPort(), response.serialize());

}


void Replica::terminate() {
    shouldTerminate = true;
    return;
}

int replica_test() {
    std::vector<std::pair<std::string, int> > leaders;
    leaders.push_back(std::make_pair("127.0.0.1", 8000)); // leader's address and port, aka main thread's node
    Replica tmpReplica(8001, leaders);
    std::thread replicaThread([&tmpReplica]() {
        tmpReplica.run(nullptr);
        return nullptr;
    });
    Node node(8000);
    std::cout << "sending request to replica" << std::endl;
    Request request(Command("com1", "127.0.0.1", 8000));
    node.send_data("127.0.0.1", 8001, request.serialize());
    struct sockaddr_in recvfrom;
    std::cout << "expected propose from replica" << std::endl;
    Message* m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
    Propose* p = dynamic_cast<Propose*>(m);
    assert(p != nullptr);
    std::cout << p->serialize() << std::endl;
    assert(p->getSlot() == 0);
    assert(p->getCommand().serialize().compare("com1|127.0.0.1|8000") == 0);
    std::cout << "sending decision to replica" << std::endl;
    Decision decision(0, Command("com2", "127.0.0.1", 8000));
    node.send_data("127.0.0.1", 8001, decision.serialize());
    std::cout << "expected response and proposal from replica" << std::endl;
    Response* r = nullptr;
    p = nullptr;
    m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
    if (dynamic_cast<Response*>(m) != nullptr) {
        r = dynamic_cast<Response*>(m);
    } else if (dynamic_cast<Propose*>(m) != nullptr) {
        p = dynamic_cast<Propose*>(m);
    }
    m = nullptr;
    m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
    if (dynamic_cast<Response*>(m) != nullptr) {
        r = dynamic_cast<Response*>(m);
    } else if (dynamic_cast<Propose*>(m) != nullptr) {
        p = dynamic_cast<Propose*>(m);
    }
    assert(r != nullptr);
    assert(p != nullptr);
    assert(r->getResult().serialize().compare("com2|127.0.0.1|8000") == 0);
    assert(p->getSlot() == 1);
    assert(p->getCommand().serialize().compare("com1|127.0.0.1|8000") == 0);
    std::cout << "test passed" << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    // 0      1         2
    // server [address] [port]
    if (argc == 1) {
        std::cout << "Run test" << std::endl;
        replica_test();
    }
    if(argc != 3) {
        std::cout << "Invalid arguments count. Should enter server [address] [port]" << std::endl;
        exit(1);
    }
    std::vector<Entry> replicas;
    std::vector<Entry> leaders;
    std::vector<Entry> acceptors;
    std::string myAddress = argv[1];
    int myPort = atoi(argv[2]);
    read_config(replicas, leaders, acceptors);
    Entry myEntry = getMyEntry(replicas, myAddress, myPort);
    std::vector<std::pair<std::string, int>> leaderAddresses;
    for (Entry leader : leaders) {
        leaderAddresses.push_back(std::make_pair(leader.address, leader.threadStartPort));
    }

    std::thread replicaThread([&replicas, &argv, &leaderAddresses, &myEntry]() {
        Replica replica(myEntry.threadStartPort, leaderAddresses);
        replica.run(nullptr);
        return nullptr;
    });

    replicaThread.join();
    return 0;
}



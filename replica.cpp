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
    leaders.push_back(std::make_pair("127.0.0.1", 8010));
    Replica tmpReplica(8001, leaders);
    std::thread replicaThread([&tmpReplica]() {
        tmpReplica.run(nullptr);
        return nullptr;
    });
    Node node(8011);
    Request request(Command("com1", "127.0.0,1", 8010));
    node.send_data("127.0.0.1", 8010, request.serialize());
    struct sockaddr_in recvfrom;
    Message* m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
    Propose* p = dynamic_cast<Propose*>(m);
    assert(p != nullptr);
    std::cout << p->serialize() << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    // 0      1      
    // server [port]
    if(argc != 2) {
        std::cout << "Invalid arguments count. Should enter server [port]" << std::endl;
        exit(1);
    }
    std::vector<std::pair<std::string, int> > replicas;
    std::vector<std::pair<std::string, int> > leaders;
    std::vector<std::pair<std::string, int> > acceptors;
    read_config(replicas, leaders, acceptors);

    std::thread replicaThread([&replicas, &argv, &leaders]() {
        Replica replica(atoi(argv[1]), leaders);
        replica.run(nullptr);
        return nullptr;
    });

    return 0;
}



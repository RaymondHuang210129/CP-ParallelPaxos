#include <string>
#include <iostream>
#include "node.h"
#include "message.h"
#include "commander.h"
#include <pthread.h>
#include <functional>
#include <thread>
#include <cassert>

#include <fstream>
#include <utility>


Commander::Commander(int port, std::vector<std::pair<std::string, int> > replicas, std::vector<std::pair<std::string, int> > acceptors){
    node = new Node(port);
    this->acceptors = acceptors;
    this->replicas = replicas;
    memset(&recvfrom, 0, sizeof(recvfrom));
};

Commander::~Commander(){
    delete node;
};

void Commander::run(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        if(dynamic_cast<Assign*>(m) != nullptr) {
            Assign *assign = dynamic_cast<Assign*>(m);
            waitFor[assign->serialize()] = std::unordered_set<std::string>{};
            Accept accept(assign->getSlot(), assign->getCommand());
            node->broadcast_data(acceptors, accept.serialize());
        } else if (dynamic_cast<Accepted*>(m) != nullptr) {
            Accepted *accepted = dynamic_cast<Accepted*>(m);
            Assign tmpAssign(accepted->getSlot(), accepted->getCommand());
            if(waitFor.count(tmpAssign.serialize()) != 0){
                std::string ip = inet_ntoa(recvfrom.sin_addr);
                // now get it back and print it
                waitFor[tmpAssign.serialize()].insert(ip);
                if(waitFor[tmpAssign.serialize()].size() > ((acceptors.size()/2)+1)){
                    Decision decision(accepted->getSlot(), accepted->getCommand());
                    node->broadcast_data(replicas, decision.serialize());
                    waitFor.erase(tmpAssign.serialize());
                }
            }
        }
    
        delete m;
    }
    return;
}

void Commander::terminate(){
    shouldTerminate = true;
    return;
}
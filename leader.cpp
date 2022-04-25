#include <string>
#include <iostream>
#include "node.h"
#include "message.h"
#include "leader.h"
#include <pthread.h>
#include <functional>
#include <thread>
#include <cassert>
#include <vector> // STL vector


Leader::Leader(int port){
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
};

Leader::~Leader(){
    delete node;
};

void Leader::run(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        Accept* accept = dynamic_cast<Accept*>(m);
        if (accept != nullptr) {
            std::cout << "Acceptor received Accept message " << accept->serialize() << std::endl;
            Accepted accepted(accept->getSlot(), accept->getCommand());
            node->send_data((struct sockaddr_in *)&recvfrom, accepted.serialize());
        }
    }
    return;
}

void Leader::terminate(){
    shouldTerminate = true;
    return;
}

int main(int argc, char *argv[]) {
    // 0      1     2
    // server [port]  [number of commander]
    if(argc != 3){
        std::cout << "Invalid arguments count. Should enter server [port] []\n " << std::endl;
        exit(1);
    }

    int numOfCommander = atoi(argv[2]);
    for(int i = 0; i<numOfCommander; ++i){
        std::thread acceptorThread([&tmpAcceptor]() {
            tmpAcceptor.run(nullptr);
            return nullptr;
        });
    }

    Leader leader(atoi(argv[1]));
    std::thread acceptorThread([&leader]() {
        leader.run(nullptr);
        return nullptr;
    });

    return 0;
}

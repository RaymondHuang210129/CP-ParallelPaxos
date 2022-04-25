#include <string>
#include <iostream>
#include "message.h"
#include "acceptor.h"
#include <functional>
#include <thread>
#include <cassert>


Acceptor::Acceptor(int port) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    shouldTerminate = false;
};

void Acceptor::run(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        Accept* accept = dynamic_cast<Accept*>(m);
        if (accept != nullptr) {
            std::cout << "Acceptor received Accept message " << accept->serialize() << std::endl;
            Accepted accepted(accept->getSlot(), accept->getCommand());
            node->send_data((struct sockaddr_in *)&recvfrom, accepted.serialize());
        }
        delete m;
    }
    return;
}

void Acceptor::terminate() {
    shouldTerminate = true;
    return;
}

int main(int argc, char *argv[]) {
    // 0      1     
    // server [port]
    if(argc != 2) {
        std::cout << "Invalid arguments count. Should enter server [port] \n " << std::endl;
        exit(1);
    }
    Acceptor tmpAcceptor(atoi(argv[1]));
    std::thread acceptorThread([&tmpAcceptor]() {
        tmpAcceptor.run(nullptr);
        return nullptr;
    });

    acceptorThread.join();
    return 0;
}
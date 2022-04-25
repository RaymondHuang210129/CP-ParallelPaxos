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

int acceptor_test() {
    Acceptor tmpAcceptor(8000);
    std::thread acceptorThread([&tmpAcceptor]() {
        tmpAcceptor.run(nullptr);
        return nullptr;
    });
    Node node(8001);
    Accept accept(1, Command("com", "address", 123));
    node.send_data("127.0.0.1", 8000, accept.serialize());
    struct sockaddr_in recvfrom;
    Message* m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
    Accepted* accepted = dynamic_cast<Accepted*>(m);
    assert(accepted != nullptr);
    assert(accepted->getSlot() == accept.getSlot());
    assert((int)(accepted->getCommand().serialize() == accept.getCommand().serialize()));
    tmpAcceptor.terminate();
    node.send_data("127.0.0.1", 8000, accept.serialize());
    acceptorThread.join();
    return 0;
}
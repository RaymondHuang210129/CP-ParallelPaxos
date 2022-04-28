#include "acceptor.h"

#include "message.h"

Acceptor::Acceptor(int port) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    shouldTerminate = false;
};

void Acceptor::run(void* arg) {
    while (!shouldTerminate) {
        Message* m = Message::deserialize(
            node->receive_data((struct sockaddr_in*)&recvfrom));
        Accept* accept = dynamic_cast<Accept*>(m);
        if (accept != nullptr) {
            // std::cout << "Acceptor received Accept message " <<
            // accept->serialize() << std::endl;
            Accepted accepted(accept->getSlot(), accept->getCommand());
            node->send_data((struct sockaddr_in*)&recvfrom,
                            accepted.serialize());
        }
        delete m;
    }
    return;
}

void Acceptor::terminate() {
    shouldTerminate = true;
    return;
}

int main(int argc, char* argv[]) {
    // 0      1          2
    // server [address]  [port]
    if (argc != 3) {
        std::cout << "Invalid arguments count. Should enter server [port] \n "
                  << std::endl;
        exit(1);
    }
    std::vector<Entry> replicas;
    std::vector<Entry> leaders;
    std::vector<Entry> acceptors;
    read_config(replicas, leaders, acceptors);
    std::string myAddress = argv[1];
    int myPort = atoi(argv[2]);
    Entry myEntry = getMyEntry(acceptors, myAddress, myPort);

    std::vector<std::thread> acceptorsThreads;
    for (int i = 0; i < myEntry.numThreads; i++) {
        int acceptorPort = myEntry.threadStartPort + i;
        acceptorsThreads.emplace_back([&leaders, i, &myEntry, acceptorPort]() {
            Acceptor tmpAcceptor(acceptorPort);
            tmpAcceptor.run(nullptr);
            return nullptr;
        });
    }

    for (int i = 0; i < acceptorsThreads.size(); ++i) {
        acceptorsThreads.at(i).join();
    }
    return 0;
}
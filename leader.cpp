#include <string>
#include <iostream>
#include <pthread.h>
#include <functional>
#include <thread>
#include <cassert>
#include "node.h"
#include "message.h"
#include "leader.h"
#include "commander.h"


Leader::Leader(int port, int numberOfCommander){
    node = new Node(port);
    this->port = port;
    this->numberOfCommander = numberOfCommander;
    memset(&recvfrom, 0, sizeof(recvfrom));
	shouldTerminate = false;
	
    for(int i = 1; i<=this->numberOfCommander; ++i){
        int commander_port = i+port;
		
        commanderThreads.emplace_back([commander_port]() {
			std::vector<std::pair<std::string, int> > replicas;
			std::vector<std::pair<std::string, int> > leaders;
			std::vector<std::pair<std::string, int> > acceptors;
			read_config(replicas, leaders, acceptors);
            Commander commander(commander_port, replicas, acceptors);
            commander.run(nullptr);
            return nullptr;
        });

        // std::thread commanderThread([port]() {
        //     std::vector<std::pair<std::string, int> > replicas;
        //     std::vector<std::pair<std::string, int> > leaders;
        //     std::vector<std::pair<std::string, int> > acceptors;
        //     read_config(replicas, leaders, acceptors);
        //     Commander commander(port, replicas, acceptors);
        //     commander.run(nullptr);
        //     return nullptr;
        // });
        //commanderThreads.push_back(commanderThread);
    }
};

Leader::~Leader(){
    delete node;
};

void Leader::run(void* arg) {
    int assignCommander = 0;
    while (!shouldTerminate) {
        Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
        Propose* propose = dynamic_cast<Propose*>(m);
        if (propose != nullptr) {
            std::cout << "Leader Receive Propose message " << propose->serialize() << std::endl;
            Assign assign(propose->getSlot(), propose->getCommand());
            node->send_data("127.0.0.1", port+(assignCommander%numberOfCommander)+1, assign.serialize());
        }
        delete m;
        assignCommander+=1;
    }
    return;
}

void Leader::terminate(){
    shouldTerminate = true;
	
	for(int i = 0; i < this->numberOfCommander; ++i){
        commanderThreads.at(i).join();
    }
	
    return;
}

int main(int argc, char *argv[]) {
    // 0      1      2
    // server [port] [number of commander]
    if(argc != 3) {
        std::cout << "Invalid arguments count. Should enter server [port] [number of commander]\n " << std::endl;
        exit(1);
    }
    int numOfCommander = atoi(argv[2]);

    std::thread leaderThread([&]() {
		Leader leader(atoi(argv[1]), atoi(argv[2]));
        leader.run(nullptr);
        return nullptr;
    });

    leaderThread.join();
    return 0;
}

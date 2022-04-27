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


Leader::Leader(Entry myEntry, int numberOfCommander, int threadID, std::vector<Entry>& replicas, std::vector<Entry>& acceptors){
    this->myEntry = myEntry;
    this->numberOfCommander = numberOfCommander;
    this->threadID = threadID;
    this->port = myEntry.threadStartPort + threadID;
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
	shouldTerminate = false;
	int commanderStartPort = myEntry.threadStartPort + myEntry.numThreads + threadID * numberOfCommander;
    for(int i = 0; i < this->numberOfCommander; ++i){
        int commanderPort = commanderStartPort+i;
        commanderThreads.emplace_back([commanderPort, &threadID, &replicas, &acceptors]() {
            Commander commander(commanderPort, threadID, replicas, acceptors);
            commander.run(nullptr);
            return nullptr;
        });
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
    // 0      1           2             3
    // server [leader ip] [leader port] [number of commander]
    if(argc != 4) {
        std::cout << "Invalid arguments count. Should enter server [leader ip] [leader port] [number of commander]\n " << std::endl;
        exit(1);
    }
	
	// std::vector<std::pair<std::string, int> > replicas;
	// std::vector<std::pair<std::string, int> > leaders;
	// std::vector<std::pair<std::string, int> > acceptors;
	// read_config(replicas, leaders, acceptors);
    

    std::vector<Entry> replicas;
    std::vector<Entry> leaders;
    std::vector<Entry> acceptors;
    read_config(replicas, leaders, acceptors);
    std::string myAddress = argv[1];
    int leaderPort = atoi(argv[2]);
    Entry myEntry = getMyEntry(leaders, myAddress, leaderPort);

	std::vector<std::thread> leaderThreads;
    int numberOfCommander = atoi(argv[3]);
    for (int i = 0; i < myEntry.numThreads; i++){
        leaderThreads.emplace_back([&leaders, &numberOfCommander, i, &myEntry, &replicas, &acceptors]() {
            Leader leader(myEntry, numberOfCommander, i, replicas, acceptors);
            leader.run(nullptr);
            return nullptr;
        });
	}
    
	for(int i = 0; i < leaderThreads.size(); ++i){
        leaderThreads.at(i).join();
    }
	
    return 0;
}

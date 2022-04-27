#include <string>
#include <iostream>
#include <thread>
#include "message.h"
#include "client.h"
#define CLIENT_RECV_MAX 100
Client::Client(int port, std::string ip){
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
	recv_count = 0;
	this->ip = ip;

    std::vector<Entry> leaders_entry;
    std::vector<Entry> acceptors_entry;
	std::vector<Entry> replicas_entry;
	read_config(replicas_entry, leaders_entry, acceptors_entry);

	for (int i = 0; i < replicas_entry.size(); i++) {
		replicas.push_back(std::make_pair(replicas_entry[i].address, replicas_entry[i].threadStartPort + i));
	}
};

Client::~Client(){
    delete node;
};

void Client::send(std::string req) {
    node->broadcast_data(replicas, req);
    return;
};

Result Client::recv(){
    Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
	Response* r = dynamic_cast<Response*>(m);
    if (r != nullptr) {
		Result rspd = r->getResult();
		delete m;
		return rspd;
    }
	else {
		delete m;
        std::cerr << "Not receiving Response message" << std::endl;
		return Result("Not receiving Response message", "null", 0);
    }
};

void Client::run(){
	while(!needTerminate()){
		std::string cmd = "cmd" + std::to_string(recv_count) + "@" + std::to_string(node->getPort());
		Request request = Request(Command(cmd, ip, node->getPort()));
		send(request.serialize());

		Result recv_result = recv();
		while(request.getCommand().serialize() != recv_result.serialize()){
			std::cout << "Mismatch command" << std::endl;
			recv_result = recv();
		}
		std::cout << "Got result: "<<recv_result.serialize() << std::endl;
	}
};

bool Client::needTerminate() {
	++recv_count;
	return (recv_count == CLIENT_RECV_MAX);
}

int main(int argc, char *argv[]) {
    if(argc != 4){
        std::cout << "Invalid arguments count. Should enter [client-port] [client-thread-number] [client-ip]\n " << std::endl;
        exit(1);
    }

	int numOfClient = atoi(argv[2]);
	std::vector<std::thread> clientThreads;
	for(int i = 0; i<numOfClient; ++i){
		int port = i+atoi(argv[1]);
		std::string ip = argv[3];
		clientThreads.emplace_back([port, ip]() {
            Client tmpClient(port, ip);
            tmpClient.run();
            return nullptr;
        });
	}

	for(int i = 0; i < numOfClient; ++i){
        clientThreads.at(i).join();
    }
    return 0;
}

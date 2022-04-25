#include <string>
#include <iostream>
#include <thread>
#include "message.h"
#include "client.h"
#define CLIENT_RECV_MAX 100
Client::Client(int port){
    node = new Node(port);
	request = new Request(Command("com", "address", 123));
    memset(&recvfrom, 0, sizeof(recvfrom));
	recv_count = 0;
	
    std::vector<std::pair<std::string, int> > leaders;
    std::vector<std::pair<std::string, int> > acceptors;
	read_config(replicas, leaders, acceptors);
};

Client::~Client(){
    delete node;
	delete request;
};

void Client::send(std::string req){
    node->broadcast_data(replicas, req);
    return;
};

Result Client::recv(){
    Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
	Response* r = dynamic_cast<Response*>(m);
    if (r != nullptr) {
		Result rspd = r->getResult();
		delete m;
        std::cout << rspd.serialize() << std::endl;
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
		send(request->serialize());
		while(request->getCommand().serialize() != recv().serialize()){
			std::cout << "Mismatch command" << std::endl;
		}
	}
};

bool Client::needTerminate() {
	++recv_count;
	return (recv_count == CLIENT_RECV_MAX);
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        std::cout << "Invalid arguments count. Should enter [cleint-port] \n " << std::endl;
        exit(1);
    }
    Client tmpClient(atoi(argv[1]));
	std::thread clientThread([&tmpClient]() {
		tmpClient.run();
		return nullptr;
	});

	clientThread.join();
    return 0;
}
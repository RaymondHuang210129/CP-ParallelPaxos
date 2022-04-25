#include <string>
#include <iostream>
#include <thread>
#include "message.h"
#include "client.h"

Client::Client(int port){
    node = new Node(port);
	request = new Request(Command("com"));
    memset(&recvfrom, 0, sizeof(recvfrom));
	shouldTerminate = false;
};

Client::~Client(){
    delete node;
	delete request;
};

void Client::send(std::string address, int port, std::string req){
    node->send_data(address, port, req);
    return;
};

void Client::recv(){
    Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
    if (dynamic_cast<Response*>(m) != nullptr) {
        std::cout << m->serialize() << std::endl;
    } else{
        std::cerr << "Not receiving Response message" << std::endl;
    }
	delete m;
    return;
};

void Client::run(char *dest_ip, int port){
	int i = 0;
	while(!shouldTerminate){
		send(dest_ip, port, request->serialize());
        recv();
		
		++i;
	}
};

void Client::terminate() {
    shouldTerminate = true;
    return;
}

int client_test(int argc, char *argv[]) {
    // 0      1         2
    // server server-ip port
    if(argc != 3){
        std::cout << "Invalid arguments count. Should enter server [server-ip] [port] \n " << std::endl;
        exit(1);
    }
	
	///////////////////////////////////////////////
    Client tmpClient(8000);
	std::thread clientThread([&tmpClient, &argv]() {
		tmpClient.run(argv[1], atoi(argv[2]));
		return nullptr;
	});
	
	char c;
	Response resp(Result("com"));
	Node node(atoi(argv[2]));
	while(1){
		struct sockaddr_in recvfrom;
		Message* m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
		node.send_data(&recvfrom, resp.serialize());
	}
	
	tmpClient.terminate();
	clientThread.join();
    return 0;
}
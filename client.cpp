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
};

Client::~Client(){
    delete node;
	delete request;
};

void Client::send(std::string address, int port, std::string req){
    node->send_data(address, port, req);
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

void Client::run(char *dest_ip, int port){
	while(isTerminate()){
		send(dest_ip, port, request->serialize());
		
		while(request->getCommand().serialize() != recv().serialize()){
			std::cout << "Mismatch rspd" << std::endl;
		}
	}
};

bool Client::isTerminate() {
	++recv_count;
	return (recv_count == CLIENT_RECV_MAX);
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
	Response resp(Result("com", "address", 123));
	Node node(atoi(argv[2]));
	while(1){
		struct sockaddr_in recvfrom;
		Message* m = Message::deserialize(node.receive_data((struct sockaddr_in *)&recvfrom));
		node.send_data(&recvfrom, resp.serialize());
	}
	
	//tmpClient.terminate();
	clientThread.join();
    return 0;
}

int main(int argc, char *argv[]) {
    // 0      1         2
    // server server-ip port
    if(argc != 3){
        std::cout << "Invalid arguments count. Should enter server [server-ip] [port] \n " << std::endl;
        exit(1);
    }

    Client tmpClient(8000);
	std::thread clientThread([&tmpClient, &argv]() {
		tmpClient.run(argv[1], atoi(argv[2]));
		return nullptr;
	});

	clientThread.join();
    return 0;
}
#include <string>
#include <iostream>
#include "message.h"
#include "client.h"


Client::Client(int port){
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
};

Client::~Client(){
    delete node;
};

void Client::send(std::string address, int port, std::string req){
    Request request(req);
    node->send_data(address, port, request.serialize());
    return;
};

 void Client::recv(){
    Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
    if (dynamic_cast<Result*>(m) != nullptr) {
        std::cout << m->serialize() << std::endl;
    } else{
        std::cerr << "Not receiving Result message" << std::endl;
    }
	delete m;
    return;
};


int client_test(int argc, char *argv[]) {
    // 0      1         2
    // server server-ip port
    if(argc != 3){
        std::cout << "Invalid arguments count. Should enter server [server-ip] [port] \n " << std::endl;
        exit(1);
    }
    std::string request;
    Client tmpClient(atoi(argv[2]));
    while(1){
        getline (std::cin, request);
        tmpClient.send(argv[1], 8000, request);
        tmpClient.recv();
    }
    return 0;
}

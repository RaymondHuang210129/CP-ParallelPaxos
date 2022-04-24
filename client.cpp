#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include "node.h"
#include "message.h"

using namespace std;

class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;

    public:
        Client(int port);
        void send(string address, int port, string req);
        void recv();
};

Client::Client(int port){
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
};

void Client::send(string address, int port, string req){
    Request request(req);
    node->send_data(address, port, request.serialize());
    return;
};

 void Client::recv(){
    Message* m = Message::deserialize(node->receive_data((struct sockaddr_in *)&recvfrom));
    if (dynamic_cast<Result*>(m) != nullptr) {
        cout << m->serialize() << endl;
    } else{
        cerr << "Not receiving Result message" << endl;
    }
    return;
};


int main(int argc, char *argv[]) {
    // 0      1         2
    // server server-ip port
    if(argc != 3){
        printf("Invalid arguments count. Should enter server [server-ip] [port] \n ");
        exit(1);
    }
    string request;
    Client tmpClient(atoi(argv[2]));
    while(1){
        getline (cin, request);
        tmpClient.send(argv[1], 8000, request);
        tmpClient.recv();
    }
    return 0;
}

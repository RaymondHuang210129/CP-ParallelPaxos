#include "node.h"

#ifndef CLIENT_H
#define CLIENT_H
class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
		Request* request;
		int recv_count;
    public:
        Client(int port);
		~Client();
        void send(std::string address, int port, std::string req);
        Result recv();
		void run(char*, int);
		bool isTerminate();
};
#endif
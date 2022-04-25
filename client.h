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
		std::vector<std::pair<std::string, int> > replicas;
    public:
        Client(int port);
		~Client();
        void send(std::string req);
        Result recv();
		void run();
		bool isTerminate();
};
#endif
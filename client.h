#include "node.h"

#ifndef CLIENT_H
#define CLIENT_H
class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
		int recvCount;
		int threadId;
		int clientRecvMax;
		std::vector<std::pair<std::string, int> > replicas;
        std::string ip;
        void send(std::string req);
        Result recv();
        bool needTerminate();
    public:
        Client(int port, std::string ip, int threadId, int clientRecvMax);
		~Client();
		void run();
        int getReceiveCount();
};
#endif
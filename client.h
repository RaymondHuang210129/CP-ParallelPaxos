#include "node.h"

class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
		Request* request;
    public:
        Client(int port);
		~Client();
        void send(std::string address, int port, std::string req);
        void recv();
		void run(char*, int);
		void terminate();
};
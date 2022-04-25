#include "node.h"

class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;

    public:
        Client(int port);
		~Client();
        void send(std::string address, int port, std::string req);
        void recv();
};
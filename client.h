#include "node.h"

class Client {
    private:
        Node* node;
        struct sockaddr_in recvfrom;

    public:
        Client(int port);
        void send(string address, int port, string req);
        void recv();
};
#include "node.h"

class Leader {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;
        int port;
        int numberOfCommander;

    public:
        Leader(int port, int numberOfCommander);
		~Leader();
        void run(void* arg);
        void terminate();
};
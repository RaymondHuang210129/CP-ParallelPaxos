#include "node.h"

class Leader {
    private:
        Node* node;
        struct sockaddr_in recvfrom;
        bool shouldTerminate;

    public:
        Leader(int port);
		~Leader();
        void run(void* arg);
        void terminate();
};

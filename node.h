#include <iostream> //cout
#include <stdio.h> //printf
#include <string.h> //strlen
#include <string> //string
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#define MAX_BUF_SIZE 100
using namespace std;

/*
    UDP Client class
*/
class Node
{
    private:
        int sock;
    public:
        Node(int);
        struct sockaddr_in setDest(string, int);
        bool send_data(sockaddr_in*, string);
		bool send_data(string, int, string);
        string receive_data(sockaddr_in*);
};

#include <iostream> //cout
#include <stdio.h> //printf
#include <string.h> //strlen
#include <string> //string
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <vector> // STL vector
#define MAX_BUF_SIZE 100

/*
    UDP Client class
*/
#ifndef NODE_H
#define NODE_H
class Node
{
    private:
        int sock;
    public:
        Node(int);
        struct sockaddr_in setDest(std::string, int);
        bool send_data(sockaddr_in*, std::string);
		bool send_data(std::string, int, std::string);
		bool broadcast_data(std::vector<std::pair<std::string, int>>&, std::string);
        std::string receive_data(sockaddr_in*);
};
#endif
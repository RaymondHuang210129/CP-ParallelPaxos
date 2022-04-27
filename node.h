#include <iostream> //cout
#include <stdio.h> //printf
#include <string.h> //strlen
#include <string> //string
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <vector> // STL vector
#include <utility> // STL pair
#include <fstream> // ifstream
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
		int port;
    public:
        Node(int);
        struct sockaddr_in setDest(std::string, int);
        bool send_data(sockaddr_in*, std::string);
		bool send_data(std::string, int, std::string);
		bool broadcast_data(std::vector<std::pair<std::string, int> >&, std::string);
        std::string receive_data(sockaddr_in*);
		int getPort();
};

struct Entry{
    std::string address;
    int hostPort;
    int threadStartPort;
    int numThreads;
    Entry(std::string address, int hostPort, int threadStartPort, int numThreads){
        this->address = address;
        this->hostPort = hostPort;
        this->threadStartPort = threadStartPort;
        this->numThreads = numThreads;
    }
    Entry() {}
};

void read_config(std::vector<Entry> &replicas, std::vector<Entry> &leaders,
                std::vector<Entry> &acceptors);

Entry getMyEntry(std::vector<Entry> entries, std::string address, int port);

#endif
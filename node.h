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
class Node
{
    private:
        int sock;
    public:
        Node(int);
        struct sockaddr_in setDest(std::string, int);
        bool send_data(sockaddr_in*, std::string);
		bool send_data(std::string, int, std::string);
		bool broadcast_data(std::vector<std::string>&, int, std::string);
        std::string receive_data(sockaddr_in*);
};

void read_config(std::vector<std::pair<std::string, int> > &replicas, std::vector<std::pair<std::string, int> > &leaders, 
                std::vector<std::pair<std::string, int> > &acceptors){
    std::ifstream config;
    config.open("config.txt");
    std::string tmp;
    while (getline(config,tmp) && tmp != "----"){ // process replicas
        std::string delimiter = " ";
        int pos = tmp.find(delimiter);
        std::string address = tmp.substr(0, pos);
        std::string port = tmp.substr(pos+1, tmp.length());
        replicas.push_back(std::make_pair(address, stoi(port)));
    }
    while (getline(config,tmp) && tmp != "----"){// process leaders
        std::string delimiter = " ";
        int pos = tmp.find(delimiter);
        std::string address = tmp.substr(0, pos);
        std::string port = tmp.substr(pos+1, tmp.length());
        replicas.push_back(std::make_pair(address, stoi(port)));
    }
    while (getline(config,tmp)){ // process acceptors
        std::string delimiter = " ";
        int pos = tmp.find(delimiter);
        std::string address = tmp.substr(0, pos);
        std::string port = tmp.substr(pos+1, tmp.length());
        replicas.push_back(std::make_pair(address, stoi(port)));
    }
    config.close();
}

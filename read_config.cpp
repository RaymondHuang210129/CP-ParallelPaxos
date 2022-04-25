#include "node.h"

void read_config(std::vector<std::pair<std::string, int> > &replicas, std::vector<std::pair<std::string, int> > &leaders,
                std::vector<std::pair<std::string, int> > &acceptors){
    std::ifstream config;
    config.open("config.conf");
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

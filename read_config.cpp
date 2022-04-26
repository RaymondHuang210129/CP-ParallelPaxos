#include "node.h"

void read_config(std::vector<Entry> &replicas, std::vector<Entry> &leaders,
                std::vector<Entry> &acceptors){
    std::ifstream config;
    config.open("config.conf");
    std::string line;
    // lines before section 1
    while (std::getline(config, line)) {
        if (line == "") continue;
        line = line.substr(0, line.find("//"));
        if (line.substr(0, 4) == "----") {
            break;
        }
    }

    // lines in section 1
    while (std::getline(config, line)) {
        if (line == "") continue;
        line = line.substr(0, line.find("//"));
        if (line.substr(0, 4) == "----") {
            break;
        }
        if (line == "") continue;
        int pos = line.find(" ");
        std::string host = line.substr(0, pos);
        line = line.substr(pos+1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos+1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":")+1));
        int threadStartPort = stoi(startPort);
        replicas.push_back(Entry(hostAddress, hostPort, threadStartPort, numThreads));
    }

    // lines in section 2
    while (std::getline(config, line)) {
        if (line == "") continue;
        line = line.substr(0, line.find("//"));
        if (line.substr(0, 4) == "----") {
            break;
        }
        if (line == "") continue;
        int pos = line.find(" ");
        std::string host = line.substr(0, pos);
        line = line.substr(pos+1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos+1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":")+1));
        int threadStartPort = stoi(startPort);
        leaders.push_back(Entry(hostAddress, hostPort, threadStartPort, numThreads));
    }

    // lines in section 3
    while (std::getline(config, line)) {
        
        line = line.substr(0, line.find("//"));
        if (line.substr(0, 4) == "----") {
            break;
        }
        if (line == "") continue;
        int pos = line.find(" ");
        std::string host = line.substr(0, pos);
        line = line.substr(pos+1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos+1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":")+1));
        int threadStartPort = stoi(startPort);
        leaders.push_back(Entry(hostAddress, hostPort, threadStartPort, numThreads));
    }
}
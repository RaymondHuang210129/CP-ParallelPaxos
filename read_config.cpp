#include "node.h"

void read_config(std::vector<Entry> &replicas, std::vector<Entry> &leaders,
                 std::vector<Entry> &acceptors) {
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
        line = line.substr(pos + 1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos + 1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":") + 1));
        int threadStartPort = stoi(startPort);
        replicas.push_back(
            Entry(hostAddress, hostPort, threadStartPort, numThreads));
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
        line = line.substr(pos + 1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos + 1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":") + 1));
        int threadStartPort = stoi(startPort);
        leaders.push_back(
            Entry(hostAddress, hostPort, threadStartPort, numThreads));
    }

    // lines in section 3
    while (std::getline(config, line)) {
        if (line == "") continue;
        line = line.substr(0, line.find("//"));
        if (line.substr(0, 4) == "----") {
            break;
        }
        if (line == "") continue;
        int pos = line.find(" ");
        std::string host = line.substr(0, pos);
        line = line.substr(pos + 1, line.length());
        pos = line.find(" ");
        std::string startPort = line.substr(0, pos);
        int numThreads = stoi(line.substr(pos + 1, line.length()));
        std::string hostAddress = host.substr(0, host.find(":"));
        int hostPort = stoi(host.substr(host.find(":") + 1));
        int threadStartPort = stoi(startPort);
        acceptors.push_back(
            Entry(hostAddress, hostPort, threadStartPort, numThreads));
    }
}

Entry getMyEntry(std::vector<Entry> entries, std::string address, int port) {
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i].address == address && entries[i].hostPort == port) {
            return entries[i];
        }
    }
    std::cout << "unable to find my entry" << std::endl;
    assert(false);
}

int read_test() {
    std::vector<Entry> replicas;
    std::vector<Entry> leaders;
    std::vector<Entry> acceptors;
    read_config(replicas, leaders, acceptors);
    for (int i = 0; i < replicas.size(); i++) {
        std::cout << replicas[i].address << " " << replicas[i].hostPort << " "
                  << replicas[i].threadStartPort << " "
                  << replicas[i].numThreads << std::endl;
    }
    std::cout << "--" << std::endl;
    for (int i = 0; i < leaders.size(); i++) {
        std::cout << leaders[i].address << " " << leaders[i].hostPort << " "
                  << leaders[i].threadStartPort << " " << leaders[i].numThreads
                  << std::endl;
    }
    std::cout << "--" << std::endl;
    for (int i = 0; i < acceptors.size(); i++) {
        std::cout << acceptors[i].address << " " << acceptors[i].hostPort << " "
                  << acceptors[i].threadStartPort << " "
                  << acceptors[i].numThreads << std::endl;
    }
    return 0;
}

#include "client.h"

#include <sys/time.h>

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "message.h"
#define CLIENT_RECV_MAX 100

Client::Client(int port, std::string ip, int threadId, int clientRecvMax) {
    node = new Node(port);
    memset(&recvfrom, 0, sizeof(recvfrom));
    recvCount = -1;
    this->ip = ip;
    this->threadId = threadId;
    this->clientRecvMax = clientRecvMax;

    std::vector<Entry> leaders_entry;
    std::vector<Entry> acceptors_entry;
    std::vector<Entry> replicas_entry;
    read_config(replicas_entry, leaders_entry, acceptors_entry);

    for (int i = 0; i < replicas_entry.size(); i++) {
        for (int j = 0; j < replicas_entry[i].numThreads; j++) {
            replicas.push_back(
                std::make_pair(replicas_entry[i].address,
                               replicas_entry[i].threadStartPort + j));
        }
    }
};

Client::~Client() { delete node; };

void Client::send(std::string req) {
    node->broadcast_data(replicas, req);
    return;
};

Result Client::recv() {
    Message* m = Message::deserialize(
        node->receive_data((struct sockaddr_in*)&recvfrom));
    Response* r = dynamic_cast<Response*>(m);
    if (r != nullptr) {
        Result rspd = r->getResult();
        delete m;
        return rspd;
    } else {
        delete m;
        std::cerr << "Not receiving Response message" << std::endl;
        return Result("Not receiving Response message", "null", 0);
    }
};

void Client::run() {
    while (!needTerminate()) {
        std::string cmd = std::to_string(
            recvCount + threadId);  //"cmd" + std::to_string(recvCount) + "@" +
                                    //std::to_string(node->getPort());
        Request request = Request(Command(cmd, ip, node->getPort()));
        send(request.serialize());

        Result recv_result = recv();
        while (request.getCommand().serialize() != recv_result.serialize()) {
            std::cout << "Mismatch command" << std::endl;
            recv_result = recv();
        }
        // std::cout << "Got result: "<<recv_result.serialize() << std::endl;
    }
};

bool Client::needTerminate() {
    ++recvCount;
    return (recvCount == clientRecvMax);
}

int Client::getReceiveCount() { return recvCount; }

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cout << "Invalid arguments count. Should enter [ip] [port] "
                     "[thread number] [recv max]\n "
                  << std::endl;
        exit(1);
    }

    int numOfClient = atoi(argv[3]);
    int clientRecvMax = atoi(argv[4]);
    std::vector<std::thread> clientThreads;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    std::vector<Client*> clients;

    for (int i = 0; i < numOfClient; ++i) {
        int port = i + atoi(argv[2]);
        std::string ip = argv[1];
        clients.emplace_back(new Client(port, ip, i, clientRecvMax));
    }

    for (int i = 0; i < numOfClient; ++i) {
        clientThreads.emplace_back([&clients, i]() {
            clients.at(i)->run();
            return nullptr;
        });
    }

    int totalReceiveCount = 0;
    for (int sec = 1; sec < 20; sec++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        int receiveCount = 0;
        for (int i = 0; i < numOfClient; i++) {
            receiveCount += clients[i]->getReceiveCount();
        }
        std::cout << sec
                  << " seconds expired: " << receiveCount - totalReceiveCount
                  << std::endl;
        totalReceiveCount = receiveCount;
    }

    return 0;

    for (int i = 0; i < numOfClient; ++i) {
        clientThreads.at(i).join();
    }

    gettimeofday(&end_time, NULL);

    float delta_ns = (end_time.tv_sec - start_time.tv_sec) * 1.0f +
                     (end_time.tv_usec - start_time.tv_usec) * 0.000001f;
    printf("Throughput = %f commands\ns",
           (CLIENT_RECV_MAX * numOfClient) / delta_ns);

    return 0;
}

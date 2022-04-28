#include "node.h"
/*
    constructor
*/
Node::Node(int port_in) {
    port = port_in;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
    }
    // std::cout<<"Socket created on: "<<port<<"\n";

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_in);
    if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) <
        (unsigned long)0) {
        perror("bind");
    }
}

int Node::getPort() { return port; }

/*
    Set receiver
*/
struct sockaddr_in Node::setDest(std::string address, int port) {
    // setup address structure
    /*if(inet_addr(address.c_str()) == -1){
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ((he = gethostbyname(address.c_str())) == NULL){
            //gethostbyname failed
            perror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        // Cast the h_addr_list to in_addr, since h_addr_list also has the ip
    address in long format only addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++){
            //strcpy(ip, inet_ntoa(*addr_list[i]) );
            receiver.sin_addr = *addr_list[i];
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
            break;
        }
    }
    else{ //plain ip address
        receiver.sin_addr.s_addr = inet_addr(address.c_str());
    }*/
    struct sockaddr_in receiver;
    receiver.sin_addr.s_addr = inet_addr(address.c_str());
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(port);

    return receiver;
}

/*
    Send data to a receiver
*/
bool Node::send_data(sockaddr_in* receiver, std::string data) {
    char tmp[20];
    inet_ntop(AF_INET, &(receiver->sin_addr), tmp, INET_ADDRSTRLEN);
    // std::cout<<"Sending " << data << " to
    // "<<tmp<<":"<<htons(receiver->sin_port)<<"...\n";

    // Send some data
    if (sendto(sock, data.c_str(), strlen(data.c_str()), 0,
               (struct sockaddr*)receiver,
               (socklen_t)sizeof(struct sockaddr)) < 0) {
        perror("Send failed : ");
        return false;
    }

    return true;
}

/*
    Send data to a ip & port
*/
bool Node::send_data(std::string address, int port, std::string data) {
    struct sockaddr_in receiver;
    receiver = setDest(address, port);

    return send_data(&receiver, data);
}

/*
    Send data to list of ip(s) & port
*/
bool Node::broadcast_data(std::vector<std::pair<std::string, int>>& addresses,
                          std::string data) {
    for (int i = 0; i < addresses.size(); i++) {
        struct sockaddr_in receiver;
        receiver = setDest(addresses[i].first, addresses[i].second);

        if (!send_data(&receiver, data)) return false;
    }
    return true;
}

/*
    Receive data from the connected host
*/
std::string Node::receive_data(sockaddr_in* sender) {
    char buffer[MAX_BUF_SIZE];
    std::string reply;

    memset(&buffer, 0, sizeof(buffer));

    // Receive a reply from the receiver
    socklen_t sender_length = (socklen_t)sizeof(struct sockaddr);
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)sender,
                 &sender_length) < 0) {
        perror("Receive failed : ");
        return NULL;
    }

    reply = buffer;

    return reply;
}

int node_test(int argc, char* argv[]) {
    Node c(51651);
    std::string host;

    std::cout << "Enter hostname : ";
    std::cin >> host;

    // send some data
    c.send_data(host, 80, "GET / HTTP/1.1\r\n\r\n");

    // receive and echo reply
    struct sockaddr_in sender;
    std::cout << "----------------------------\n\n";
    std::cout << c.receive_data(&sender);
    std::cout << "\n\n----------------------------\n\n";

    // done
    return 0;
}
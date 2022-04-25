#include <iostream>
#include <string>
#include <memory>
#include "message.h"
#include <cassert>

Command::Command() {};
Command::Command(std::string str, std::string address, int port) {
    this->content = str;
    source = std::make_pair(address, port);
};
bool Command::operator < (const Command& other) const {
    return content < other.content;
};
std::string Command::serialize() {
    return content + "|" + source.first + "|" + std::to_string(source.second);
};
Command* Command::deserialize(std::string serialized) {
    std::string str = serialized.substr(0, serialized.find("|"));
    std::string remainStr = serialized.substr(serialized.find("|") + 1);
    std::string address = remainStr.substr(0, remainStr.find("|"));
    int port = std::stoi(remainStr.substr(remainStr.find("|") + 1));
    return new Command(str, address, port);
};
std::string Command::getContent() {
    return content;
};
std::pair<std::string, int> Command::getSource() {
    return source;
};
std::string Command::getAddress() {
    return source.first;
};
int Command::getPort() {
    return source.second;
};

Result::Result(std::string str, std::string address, int port){
    source = std::make_pair(address, port);
    this->content = str;
}
std::string Result::serialize() {
    return content + "|" + source.first + "|" + std::to_string(source.second);
};
Result* Result::deserialize(std::string serialized) {
    std::string str = serialized.substr(0, serialized.find("|"));
    std::string remainStr = serialized.substr(serialized.find("|") + 1);
    std::string address = remainStr.substr(0, remainStr.find("|"));
    int port = std::stoi(remainStr.substr(remainStr.find("|") + 1));
    return new Result(str, address, port);
};
std::string Result::getContent() {
    return content;
};
std::pair<std::string, int> Result::getSource() {
    return source;
};
std::string Result::getAddress() {
    return source.first;
};
int Result::getPort() {
    return source.second;
};

Message::Message() {};
std::string Message::serialize() {return "";};
Message::~Message() {};
Message* Message::deserialize(std::string serialized) {
    if (serialized.substr(0, 8) == "Request(") {
        std::string content = serialized.substr(8, serialized.length() - 9);
        Command* command = Command::deserialize(content);
        Request* request = new Request(*command);
        delete command;
        return request;
    } else if (serialized.substr(0, 9) == "Response(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        Result* result = Result::deserialize(content);
        Response* response = new Response(*result);
        delete result;
        return response;
    } else if (serialized.substr(0, 8) == "Propose(") {
        std::string content = serialized.substr(8, serialized.length() - 9);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        Command* command = Command::deserialize(content.substr(commaPosition + 1, content.length() - commaPosition - 1));
        Propose* propose = new Propose(slot, *command);
        delete command;
        return propose;
    } else if (serialized.substr(0, 7) == "Accept(") {
        std::string content = serialized.substr(7, serialized.length() - 8);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        Command* command = Command::deserialize(content.substr(commaPosition + 1, content.length() - commaPosition - 1));
        Accept* accept = new Accept(slot, *command);
        delete command;
        return accept;
    } else if (serialized.substr(0, 9) == "Accepted(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        Command* command = Command::deserialize(content.substr(commaPosition + 1, content.length() - commaPosition - 1));
        Accepted* accepted = new Accepted(slot, *command);
        delete command;
        return accepted;
    } else if (serialized.substr(0, 9) == "Decision(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        Command* command = Command::deserialize(content.substr(commaPosition + 1, content.length() - commaPosition - 1));
        Decision* decision = new Decision(slot, *command);
        delete command;
        return decision;
    } else if (serialized.substr(0, 7) == "Assign(") {
        std::string content = serialized.substr(7, serialized.length() - 8);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        Command* command = Command::deserialize(content.substr(commaPosition + 1, content.length() - commaPosition - 1));
        Assign* assign = new Assign(slot, *command);
        delete command;
        return assign;
    } else {
        std::cout << "unrecognized serialized" << std::endl;
        return new Message();
    }
};

Request::Request(Command command) : command(command) {};
std::string Request::serialize() {
    return "Request(" + command.serialize() + ")";
};
Command Request::getCommand() {
    return command;
};

Response::Response(Result result) : result(result) {};
std::string Response::serialize() {
    return "Response(" + result.serialize() + ")";
};
Result Response::getResult() {
    return result;
};

Propose::Propose(int slot, Command command) : slot(slot), command(command) {};
std::string Propose::serialize() {
    return "Propose(" + std::to_string(slot) + "," + command.serialize() + ")";
};
int Propose::getSlot() {
    return slot;
};
Command Propose::getCommand() {
    return command;
};

Accept::Accept(int slot, Command command) : slot(slot), command(command) {};
std::string Accept::serialize() {
    return "Accept(" + std::to_string(slot) + "," + command.serialize() + ")";
};
int Accept::getSlot() {
    return slot;
};
Command Accept::getCommand() {
    return command;
};


Accepted::Accepted(int slot, Command command) : slot(slot), command(command) {};
std::string Accepted::serialize() {
    return "Accepted(" + std::to_string(slot) + "," + command.serialize() + ")";
}
int Accepted::getSlot() {
    return slot;
};
Command Accepted::getCommand() {
    return command;
};

Decision::Decision(int slot, Command command) : slot(slot), command(command) {};
std::string Decision::serialize() {
    return "Decision(" + std::to_string(slot) + "," + command.serialize() + ")";
}
int Decision::getSlot() {
    return slot;
};
Command Decision::getCommand() {
    return command;
};

Assign::Assign(int slot, Command command) : slot(slot), command(command) {};
std::string Assign::serialize() {
    return "Assign(" + std::to_string(slot) + "," + command.serialize() + ")";
}
int Assign::getSlot() {
    return slot;
};
Command Assign::getCommand() {
    return command;
};



int message_test() {

    int slot1 = 1;
    std::string str1 = "com";
    Command command1(str1, "address", 123);
    Result result1(str1, "address", 123);
    Request request(command1);
    Response response(result1);
    Propose propose(slot1, command1);
    Accept accept(slot1, command1);
    Accepted accepted(slot1, command1);
    Decision decision(slot1, command1);
    Assign assign(slot1, command1);
    std::cout << request.serialize() << std::endl;
    std::cout << response.serialize() << std::endl;
    std::cout << propose.serialize() << std::endl;
    std::cout << accept.serialize() << std::endl;
    std::cout << accepted.serialize() << std::endl;
    std::cout << decision.serialize() << std::endl;
    std::cout << assign.serialize() << std::endl;

    Message* message = Message::deserialize(request.serialize());
    assert(message != nullptr);
    Request* request2 = dynamic_cast<Request*>(message);
    assert(request2 != nullptr);
    message = nullptr;

    message = Message::deserialize(response.serialize());
    assert(message != nullptr);
    Response* response2 = dynamic_cast<Response*>(message);
    assert(response2 != nullptr);
    message = nullptr;

    message = Message::deserialize(propose.serialize());
    assert(message != nullptr);
    Propose* propose2 = dynamic_cast<Propose*>(message);
    assert(propose2 != nullptr);
    message = nullptr;

    message = Message::deserialize(accept.serialize());
    assert(message != nullptr);
    Accept* accept2 = dynamic_cast<Accept*>(message);
    assert(accept2 != nullptr);
    message = nullptr;

    message = Message::deserialize(accepted.serialize());
    assert(message != nullptr);
    Accepted* accepted2 = dynamic_cast<Accepted*>(message);
    assert(accepted2 != nullptr);
    message = nullptr;

    message = Message::deserialize(decision.serialize());
    assert(message != nullptr);
    Decision* decision2 = dynamic_cast<Decision*>(message);
    assert(decision2 != nullptr);

    message = Message::deserialize(assign.serialize());
    assert(message != nullptr);
    Assign* assign2 = dynamic_cast<Assign*>(message);
    assert(assign2 != nullptr);

    std::cout << request2->serialize() << std::endl;
    std::cout << response2->serialize() << std::endl;
    std::cout << propose2->serialize() << std::endl;
    std::cout << accept2->serialize() << std::endl;
    std::cout << accepted2->serialize() << std::endl;
    std::cout << decision2->serialize() << std::endl;
    std::cout << assign2->serialize() << std::endl;

    assert(request2->serialize() == request.serialize());
    assert(response2->serialize() == response.serialize());
    assert(propose2->serialize() == propose.serialize());
    assert(accept2->serialize() == accept.serialize());
    assert(accepted2->serialize() == accepted.serialize());
    assert(decision2->serialize() == decision.serialize());
    assert(assign2->serialize() == assign.serialize());

    return 0;
}


#include <iostream>
#include <string>
#include <memory>
#include "message.h"

Command::Command(std::string str) : str(str) {}
std::string Command::serialize() {
    return str;
};

Result::Result(std::string str) : str(str) {}
std::string Result::serialize() {
    return str;
};

Message::Message() {};
std::string Message::serialize() {return "";};
Message::~Message() {};
Message* Message::deserialize(std::string serialized) {
    if (serialized.substr(0, 8) == "Request(") {
        std::string content = serialized.substr(8, serialized.length() - 9);
        Command command(content);
        return new Request(command);
    } else if (serialized.substr(0, 9) == "Response(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        Result result(content);
        return new Response(result);
    } else if (serialized.substr(0, 8) == "Propose(") {
        std::string content = serialized.substr(8, serialized.length() - 9);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        std::string command = content.substr(commaPosition + 1, content.length() - commaPosition - 1);
        return new Propose(slot, command);
    } else if (serialized.substr(0, 7) == "Accept(") {
        std::string content = serialized.substr(7, serialized.length() - 8);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        std::string command = content.substr(commaPosition + 1, content.length() - commaPosition - 1);
        return new Accept(slot, command);
    } else if (serialized.substr(0, 9) == "Accepted(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        std::string command = content.substr(commaPosition + 1, content.length() - commaPosition - 1);
        return new Accepted(slot, command);
    } else if (serialized.substr(0, 9) == "Decision(") {
        std::string content = serialized.substr(9, serialized.length() - 10);
        int commaPosition = content.find(",");
        int slot = std::stoi(content.substr(0, commaPosition));
        std::string command = content.substr(commaPosition + 1, content.length() - commaPosition - 1);
        return new Decision(slot, command);
    } else {
        return new Message();
    }
};

Request::Request(Command command) : command(command) {};
std::string Request::serialize() {
    return "Request(" + command.serialize() + ")";
};

Response::Response(Result result) : result(result) {};
std::string Response::serialize() {
    return "Response(" + result.serialize() + ")";
};

Propose::Propose(int slot, Command command) : slot(slot), command(command) {};
std::string Propose::serialize() {
    return "Propose(" + std::to_string(slot) + "," + command.serialize() + ")";
};

Accept::Accept(int slot, Command command) : slot(slot), command(command) {};
std::string Accept::serialize() {
    return "Accept(" + std::to_string(slot) + "," + command.serialize() + ")";
};


Accepted::Accepted(int slot, Command command) : slot(slot), command(command) {};
std::string Accepted::serialize() {
    return "Accepted(" + std::to_string(slot) + "," + command.serialize() + ")";
}

Decision::Decision(int slot, Command command) : slot(slot), command(command) {};
std::string Decision::serialize() {
    return "Decision(" + std::to_string(slot) + "," + command.serialize() + ")";
}



int test() {

    int slot1 = 1;
    std::string str1 = "com";
    Command command1(str1);
    Result result1(str1);
    Request request(command1);
    Response response(result1);
    Propose propose(slot1, command1);
    Accept accept(slot1, command1);
    Accepted accepted(slot1, command1);
    Decision decision(slot1, command1);
    std::cout << request.serialize() << std::endl;
    std::cout << response.serialize() << std::endl;
    std::cout << propose.serialize() << std::endl;
    std::cout << accept.serialize() << std::endl;
    std::cout << accepted.serialize() << std::endl;
    std::cout << decision.serialize() << std::endl;

    Message* message = Message::deserialize(request.serialize());
    assert(message != nullptr);
    Request* request2 = dynamic_cast<Request*>(message);
    assert(request2 != nullptr);
    message = nullptr;

    std::cout << "." << std::endl;

    message = Message::deserialize(response.serialize());
    assert(message != nullptr);
    Response* response2 = dynamic_cast<Response*>(message);
    assert(response2 != nullptr);
    message = nullptr;

    std::cout << "." << std::endl;

    message = Message::deserialize(propose.serialize());
    assert(message != nullptr);
    Propose* propose2 = dynamic_cast<Propose*>(message);
    assert(propose2 != nullptr);
    message = nullptr;

    std::cout << "." << std::endl;

    message = Message::deserialize(accept.serialize());
    assert(message != nullptr);
    Accept* accept2 = dynamic_cast<Accept*>(message);
    assert(accept2 != nullptr);
    message = nullptr;

    std::cout << "." << std::endl;

    message = Message::deserialize(accepted.serialize());
    assert(message != nullptr);
    Accepted* accepted2 = dynamic_cast<Accepted*>(message);
    assert(accepted2 != nullptr);
    message = nullptr;

    std::cout << "." << std::endl;

    message = Message::deserialize(decision.serialize());
    assert(message != nullptr);
    Decision* decision2 = dynamic_cast<Decision*>(message);
    assert(decision2 != nullptr);

    std::cout << "." << std::endl;

    std::cout << request2->serialize() << std::endl;
    std::cout << response2->serialize() << std::endl;
    std::cout << propose2->serialize() << std::endl;
    std::cout << accept2->serialize() << std::endl;
    std::cout << accepted2->serialize() << std::endl;
    std::cout << decision2->serialize() << std::endl;

    return 0;
}


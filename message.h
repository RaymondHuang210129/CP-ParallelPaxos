#include <string>

#ifndef MESSAGE_H
#define MESSAGE_H

class Command {
private:
    std::string content;
    std::pair<std::string, int> source;
public:
    Command();
    Command(std::string str, std::string address, int port);
    bool operator < (const Command& other) const; 
    std::string serialize();
    static Command* deserialize(std::string serialized);
    std::string getContent();
    std::pair<std::string, int> getSource();
    std::string getAddress();
    int getPort();
};

class Result {
private:
    std::string content;
    std::pair<std::string, int> source;
public:
    Result(std::string str, std::string address, int port);
    std::string serialize();
    static Result* deserialize(std::string serialized);
    std::string getContent();
    std::pair<std::string, int> getSource();
    std::string getAddress();
    int getPort();
};

class Message {
private:
public:
    Message();
    std::string serialize();
    virtual ~Message();
    static Message* deserialize(std::string serialized);
};

class Request : public Message {
private:
    Command command;
public:
    Request(Command command);
    std::string serialize();
    Command getCommand();
};

class Response : public Message {
private:
    Result result;
public:
    Response(Result result);
    std::string serialize();
    Result getResult();
};

class Propose : public Message {
private:
    int slot;
    Command command;
public:
    Propose(int slot, Command command);
    std::string serialize();
    int getSlot();
    Command getCommand();
};

class Accept : public Message {
private:
    int slot;
    Command command;
public:
    Accept(int slot, Command command);
    std::string serialize();
    int getSlot();
    Command getCommand();
};

class Accepted : public Message {
private:
    int slot;
    Command command;
public:
    Accepted(int slot, Command command);
    std::string serialize();
    int getSlot();
    Command getCommand();
};

class Decision : public Message {
private:
    int slot;
    Command command;
public:
    Decision(int slot, Command command);
    std::string serialize();
    int getSlot();
    Command getCommand();
};

class Assign : public Message {
private:    
    int slot;
    Command command;
public:
    Assign(int slot, Command command);
    std::string serialize();
    int getSlot();
    Command getCommand();
};

#endif
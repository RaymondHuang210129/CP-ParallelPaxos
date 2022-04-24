#include <string>

class Command {
private:
    std::string str;
public:
    Command(std::string str);
    std::string serialize();
};

class Result {
private:
    std::string str;
public:
    Result(std::string str);
    std::string serialize();
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
};

class Response : public Message {
private:
    Result result;
public:
    Response(Result result);
    std::string serialize();
};

class Propose : public Message {
private:
    int slot;
    Command command;
public:
    Propose(int slot, Command command);
    std::string serialize();
};

class Accept : public Message {
private:
    int slot;
    Command command;
public:
    Accept(int slot, Command command);
    std::string serialize();
};

class Accepted : public Message {
private:
    int slot;
    Command command;
public:
    Accepted(int slot, Command command);
    std::string serialize();
};

class Decision : public Message {
private:
    int slot;
    Command command;
public:
    Decision(int slot, Command command);
    std::string serialize();
};
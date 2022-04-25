CC=g++
all: client.cpp message.cpp node.cpp
	$(CC) client.cpp message.cpp node.cpp acceptor.cpp -o main -std=c++17 -pthread
.PHONY: clean
clean:
	rm -f main

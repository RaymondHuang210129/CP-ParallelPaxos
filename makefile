CC=g++
all: client.cpp message.cpp node.cpp
	$(CC) client.cpp message.cpp node.cpp reconfig.cpp -o client -std=c++17 -pthread
	$(CC) replica.cpp message.cpp node.cpp reconfig.cpp -o replica -std=c++17 -pthread
	$(CC) leader.cpp message.cpp node.cpp commander.cpp reconfig.cpp -o leader -std=c++17 -pthread
	$(CC) acceptor.cpp message.cpp node.cpp reconfig.cpp -o acceptor -std=c++17 -pthread
.PHONY: clean
clean:
	rm -f main

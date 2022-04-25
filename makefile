CC=g++
all: client.cpp message.cpp node.cpp
	$(CC) client.cpp message.cpp node.cpp read_config.cpp -o client -std=c++17 -pthread
	$(CC) replica.cpp message.cpp node.cpp read_config.cpp -o replica -std=c++17 -pthread
	$(CC) leader.cpp message.cpp node.cpp commander.cpp read_config.cpp -o leader -std=c++17 -pthread
	$(CC) acceptor.cpp message.cpp node.cpp read_config.cpp -o acceptor -std=c++17 -pthread
.PHONY: clean
clean:
	rm -f main

CC=g++
all: client.cpp message.cpp node.cpp
	$(CC) client.cpp message.cpp node.cpp -o main
.PHONY: clean
clean:
	rm -f main

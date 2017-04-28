CC = g++
FLAGS = -Wall -ansi -g 

all: server	client 
server: server.h server.cpp common 
	$(CC) $(FLAGS) -o server server.cpp -lpthread message.o common.o thread.o poller.o
client: client.cpp client.h common 
	$(CC) $(FLAGS) -o client client.cpp	-lpthread message.o common.o thread.o poller.o
common: thread.h thread.cpp message.cpp message.h common.h poller.h poller.cpp
	$(CC) $(FLAGS) -c thread.cpp message.cpp poller.cpp
	



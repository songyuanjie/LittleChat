CC = g++
FLAGS = -Wall -ansi -g 

all: server	client
server: server.h server.cpp message thread
	$(CC) $(FLAGS) -o server server.cpp thread.o -lpthread message.o
client: client.cpp client.h message thread
	$(CC) $(FLAGS) -o client client.cpp	thread.o -lpthread message.o
thread: thread.h thread.cpp
	$(CC) $(FLAGS) -c -o thread.o thread.cpp 
message: message.h message.cpp
	$(CC) $(FLAGS) -c message.cpp 
	



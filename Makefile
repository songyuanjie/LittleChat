CC = g++
FLAGS = -Wall -ansi -g


all: server	client
server: server.h server.cpp mutex.h
	$(CC) $(FLAGS) -o server server.cpp thread.o -lpthread	
client: client.cpp client.h mutex.h
	$(CC) $(FLAGS) -o client client.cpp	thread.o -lpthread
thread: thread.h thread.cpp
	$(CC) $(FLAGS) -c -o thread.o thread.cpp 



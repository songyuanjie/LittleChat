CC = g++
FLAGS = -Wall -ansi -g


all: main	client
main: server main.cpp thread
	$(CC) $(FLAGS) -o main main.cpp server.o thread.o -lpthread
server: server.h server.cpp
	$(CC) $(FLAGS) -c -o server.o server.cpp	
client: client.cpp client.h
	$(CC) $(FLAGS) -o client client.cpp	
thread: thread.h thread.cpp
	$(CC) $(FLAGS) -c -o thread.o thread.cpp -lpthread


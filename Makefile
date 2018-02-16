# Compiler options
CC = g++
CFLAGS = -c -Wall

default: board

board: main.o channel.o board.o List.o Node.o
	$(CC) main.o channel.o board.o List.o Node.o -o board

main.o : main.cpp board.h channel.h List.h
	$(CC) $(CFLAGS) main.cpp board.cpp channel.cpp List.cpp

board.o : board.cpp board.h
	$(CC) $(CFLAGS) board.cpp

channel.o : channel.cpp channel.h
	$(CC) $(CFLAGS) channel.cpp

List.o : List.cpp List.h Node.h
	$(CC) $(CFLAGS) List.cpp

Node.o: Node.cpp Node.h
	$(CC) $(CFLAGS) Node.cpp 

# Removing executable,old .o object files AND backup files
clean:
	rm *.o board

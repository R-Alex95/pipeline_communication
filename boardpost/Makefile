# Compiler options
CC = g++
CFLAGS = -c -Wall

default: boardpost

boardpost: main.o 
	$(CC) main.o -o boardpost

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp 

# Removing executable,old .o object files AND backup files
clean:
	rm *.o boardpost

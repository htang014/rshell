CFLAGS = -ansi -pedantic -Wall -Werror

all: rshell
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell
	

rshell:
	mkdir -p bin
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell

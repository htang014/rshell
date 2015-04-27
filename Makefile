CFLAGS = -ansi -pedantic -Wall -Werror

all: rshell cp
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell
	

rshell:
	mkdir -p bin
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell

cp:
	g++ $(CFLAGS) src/cp.cpp -o bin/cp

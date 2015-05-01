CFLAGS = -ansi -pedantic -Wall -Werror

all: rshell cp ls
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell
	

rshell:
	mkdir -p bin
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell

cp:
	g++ src/cp.cpp -o bin/cp

ls:
	g++ $(CFLAGS) src/ls.cpp -o bin/ls -Wno-write-strings

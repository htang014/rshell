CFLAGS = -ansi -pedantic -Wall -Werror

all:
	g++ $(CFLAGS) src\main.cpp

rshell:
	g++ $(CFLAGS) src\main.cpp

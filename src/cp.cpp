#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include "Timer.h"
using namespace std;

void getput(char *p[]) {
	char* input = p[1];
	char* output = p[2];

	ifstream inFS(input);
	ofstream outFS(output);
	
	char temp;
	while (inFS.get(temp)) {
		outFS.put(temp);
	}

}


void readchar(char* p[]){
	int fdnew;
	int fdold;
	fdnew = open(p[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	fdold = open(p[1], O_RDONLY);

	int size;
	char c[sizeof(char)];
	size = read(fdold, c, sizeof(c));

	while (size > 0){
		write(fdnew, c, size);
		size = read(fdold, c, sizeof(c));
	}
}


void readbuf(char* p[]){
	int fdnew;
	int fdold;
	fdnew = open(p[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	fdold = open(p[1], O_RDONLY);

	int size;
	char c[BUFSIZ];
	size = read(fdold, c, sizeof(c));

	while (size > 0){
		write(fdnew, c, size);
		size = read(fdold, c, sizeof(c));
	}
}

int main (int argc, char* argv[]){
	bool yes = 0;
	char* output = argv[2];

	if (argc == 4) yes = 1;

	struct stat s;
	if (stat(output, &s) == 0 && S_ISREG(s.st_mode)){
		cout << "ERROR" << endl;
	}

	if (yes){
		Timer a;
		double aUserTime, aWallclockTime, aSystemTime;
		a.start();
		getput(argv);
		a.elapsedUserTime(aUserTime);
		a.elapsedWallclockTime(aWallclockTime);
		a.elapsedSystemTime(aSystemTime);
		cout << aUserTime << ' ' << aWallclockTime << ' ' << aSystemTime << endl;

		Timer b;
		double bUserTime, bWallclockTime, bSystemTime;
		b.start();
		readchar(argv);
		b.elapsedUserTime(aUserTime);
		b.elapsedWallclockTime(aWallclockTime);
		b.elapsedSystemTime(aSystemTime);
		cout << bUserTime << ' ' << bWallclockTime << ' ' << bSystemTime << endl;
	}

	Timer c;
	double cUserTime, cWallclockTime, cSystemTime;
	c.start();
	readbuf(argv);
	c.elapsedUserTime(cUserTime);
	c.elapsedWallclockTime(cWallclockTime);
	c.elapsedSystemTime(cSystemTime);
	if (yes)
		cout << cUserTime << ' ' << cWallclockTime << ' ' << cSystemTime << endl;

	return 0;
}




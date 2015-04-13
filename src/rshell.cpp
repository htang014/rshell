#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

int main () {
	
	string input;
	string argList;
	string arg;
	vector<string> args;

	istringstream inSS(argList);

	while (input != "exit"){
		cout << "$ ";
		cin >> input;

		getline(cin,argList);

		while (!inSS.eof() ){
			inSS >> arg;
			args.push_back(arg);
		}

		

	}
		
	return 0;

}

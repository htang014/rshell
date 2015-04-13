#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;

/*void doExec(char* cmd){
	int pid = fork();

	if (pid == -1){
		perror("There was an error with fork()");
		exit(1);
	}
	else if (pid == 0){
		if (-1 == execvp(cmd, argv)){
			perror("There was an error with execvp()");
			exit(1);
		}
	}
	else {
		if (-1 == wait(0)){
			perror("There was an error with wait()");
			exit(1);
		}
	}
}*/

char* convert(const string& str){
	char* p = new char[str.size()+1];
	strcpy(p,str.c_str());
	return p;
}

vector<char*> str_parse(string str){
	string arg;
	vector<string> argList;
	vector<char*> argListC;
	istringstream inSS(arg);

	while(!inSS.eof()){
		inSS >> str;
		argList.push_back(arg);
	}
	
	transform(argList.begin(), argList.end(), back_inserter(argListC), convert);

	return argListC;
}


int main () {
	string cmd;

	while (true){
		cout << "$ ";
		getline(cin, cmd);
		
		//check for exit command
		if (cmd == "exit") exit(0);

		str_parse(cmd);	
	}
		
	return 0;

}

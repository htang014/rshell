#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

void doExec(vector<char*> instr){
	int pid = fork();

	if (pid == -1){
		perror("There was an error with fork()");
		exit(1);
	}
	else if (pid == 0){
			if (-1 == execvp(instr[0],&instr[0] )){
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
}

char* convert(const string& str){
	char* p = new char[str.size()+1];
	strcpy(p,str.c_str());
	return p;
}

vector<char*> str_parse(string str){
	string arg;
	vector<string> argList;
	vector<char*> argListC;
	istringstream inSS(str);

	//parse from command string
	while(!inSS.eof()){
		inSS >> arg;
		argList.push_back(arg);
	}

	//convert c++ string vector into cstring array
	transform(argList.begin(), argList.end(), back_inserter(argListC), convert);
	argListC.push_back(NULL);

	return argListC;
}


int main () {
	string cmd;

	while (true){
		cout << "$ ";
		getline(cin, cmd);
		
		//check for exit command
		if (cmd == "exit") exit(0);

		vector<char*> v = str_parse(cmd);
	
		doExec(str_parse(cmd));	
	}

	return 0;

}

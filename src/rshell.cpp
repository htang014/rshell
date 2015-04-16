
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

void commentOut (string& s){
	size_t found = s.find('#');
	if (found != string::npos){
		s.erase(s.begin()+found, s.end());
	}
}


//------------------------------------------------------------
int doExec(vector<char*> instr){
	int pid = fork();
	int childStat;

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
		int waitPid = 0;

		if (-1 == waitpid(waitPid, &childStat, 0)){
			perror("There was an error with wait()");
		}

		childStat = WEXITSTATUS(childStat);
	}
	return childStat;
}
//------------------------------------------------------------


void doLogic (vector<char*> a){
	vector<char*> cmd = a;
	vector<char*> segment;
	int i = 0;

	char OR[3] = "||";
	char AND[3] = "&&";
	char END[2] = ";";

	while (cmd.size() > 1){
		if (strcmp(cmd.at(0),OR) == 0){
			int status = doExec(segment);
			segment.clear();
			if (status != -1) return;	
		}
		else if (strcmp(cmd.at(0), AND) == 0){
			int status = doExec(segment);
			segment.clear();
			if (status != 0) return;
		}
		else if (strcmp(cmd.at(0), END) == 0){
			doExec(segment);
			segment.clear();
		}
		else {
			segment.push_back(cmd.at(0));
		}
		cmd.erase(cmd.begin());
	}
	doExec(segment);
}
//------------------------------------------------------------

char* convert(const string& str){
	char* p = new char[str.size()+1];
	strcpy(p,str.c_str());
	return p;
}
//------------------------------------------------------------
//parse the user input into char* array
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

//------------------------------------------------------------

int main () {
	string cmd;

	while (true){
		cout << "$ ";
		getline(cin, cmd);
		
		commentOut(cmd);
		
		//resolve space after command scenario
		if (cmd.at(cmd.size() - 1) == ' ') 
			cmd.erase(cmd.begin() + cmd.size() - 1 );

		//check for exit command
		if (cmd == "exit") exit(0);

		vector<char*> v = str_parse(cmd);
	
		doLogic(str_parse(cmd));	
	}

	return 0;

}

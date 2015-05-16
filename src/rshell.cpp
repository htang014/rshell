#include <unistd.h>
#include <fcntl.h>
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

//-------------------------------------------------------------------------

char* convert(const string& str){
	char* p = new char[str.size()+1];
	strcpy(p,str.c_str());
	return p;
}

//------------------------------------------------------------

int doRedirOut1(string outfile, int & savestdout){
	int fd;
	if (-1 == (fd = open(outfile.c_str(), O_CREAT|O_WRONLY, 0666)))
		perror("there was an error with open()");

	if (-1 == (savestdout = dup(1))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, 1))
		perror("There was an error with dup2()");

	return fd;

}

int doRedirOut2(string outfile, int & savestdout){
	int fd;
	if (-1 == (fd = open(outfile.c_str(), O_CREAT|O_APPEND|O_WRONLY, 0666)))
		perror("there was an error with open()");

	if (-1 == (savestdout = dup(1))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, 1))
		perror("There was an error with dup2()");

	return fd;

}

int doRedirIn(string infile, int & savestdin){
	int fd = 0;
	if (-1 == (fd = open(infile.c_str(), O_RDONLY)))
		perror("there was an error with open()");

	if (-1 == (savestdin = dup(0))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, 0))
		perror("There was an error with dup2()");


	return fd;

}

int doPipe (vector<string> & pipeline) {
	int status = 0;

	vector<string> argLeft;
	vector<string> argRight;
	vector<char*> argLeftC;
	vector<char*> argRightC;
	bool argSwitch = 0;

	//---DEBUGGING CODE---
	/*for (unsigned i = 0; i < pipeline.size(); i++) cout << pipeline.at(i) << " ";
	cout << endl;*/

	for (unsigned i = 0; i < pipeline.size(); i++){
		if (pipeline.at(i) == "|" && argSwitch){
			break;
		}
		else if (pipeline.at(i) == "|"){
			argSwitch = 1;
		}
		else if (argSwitch){
			argRight.push_back(pipeline.at(i));
		}
		else{
			argLeft.push_back(pipeline.at(i));
		}
	}

	while (pipeline.front() != "|" && !pipeline.empty())
		pipeline.erase(pipeline.begin());
	if (!pipeline.empty())
		pipeline.erase(pipeline.begin());


	transform(argLeft.begin(), argLeft.end(), back_inserter(argLeftC), convert);
	argLeftC.push_back(NULL);

	transform(argRight.begin(), argRight.end(), back_inserter(argRightC), convert);
	argRightC.push_back(NULL);

	//---DEBUGGING CODE---
	/*for (unsigned i = 0; argLeftC.at(i) != NULL; i++) cout << argLeftC.at(i) << " ";
	cout << endl;
	for (unsigned i = 0; argRightC.at(i) != NULL; i++) cout << argRightC.at(i) << " ";
	cout << endl;*/


	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	int fd[2];


	if (pipe(fd) == -1){
		perror("There was an error with pipe()");
	}

	int pid = fork();
	if (pid == -1){
		perror("There was an error with fork()");
		return -1;
	}
	else if (pid == 0){

		if (-1 == dup2(fd[PIPE_WRITE], 1))
			perror("There was an error with dup2()");
		if (-1 == close(fd[PIPE_READ]))
			perror("There was an error with close()");

		if (-1 == execvp(argLeftC[0], &argLeftC[0]))
			perror("There was an error with execvp()");

		exit(1);
	}
	else if (pid > 0){
		int savestdin;
		if (-1 == (savestdin = dup(0)))
			perror("There was an error with dup()");
		if (-1 == wait(0))
			perror("There was an error with wait()");
		
		int pid2 = fork();
		if (pid2 == -1){
			perror("There was an error with fork()");
			return -1;
		}	
		else if (pid2 == 0){

			if (-1 == dup2(fd[PIPE_READ],0))
				perror("There was an error with dup2");
			if (-1 == close(fd[PIPE_WRITE]))
				perror("There was an error with close()");

		
			for (unsigned k = 0; k < pipeline.size(); k++){
				if (pipeline.at(k) == "|"){
					status = doPipe (pipeline);
					break;
				}
			}

			if (-1 == execvp(argRightC[0], &argRightC[0]))
				perror("There was an error with execvp()");

			exit(1);
		}
		else if (pid2 > 0){
			if (-1 == close(fd[PIPE_WRITE]))
				perror("There was an error with close()");
			if (-1 == wait(0))
				perror("There was an error with wait()");
		}


		if (-1 == dup2(savestdin, 0))
			perror("There was an error with dup2()");

	}


	return status;
}

//------------------------------------------------------------
int doExec(vector<char*> instr){

	string IN = "<";
	string OUT1 = ">";
	string OUT2 = ">>";
	int fd = -1, savestdout = -1, savestdin = -1;

	vector<string> instrCopy; //backup instr to a mutable vector<string>
	for (unsigned k = 0; k < instr.size(); k++){
		string s = instr.at(k);
		instrCopy.push_back(s);
	}

	for (unsigned i = 0; i < instrCopy.size(); i++){
		if (instrCopy.at(i) == IN || instrCopy.at(i) == OUT1 || instrCopy.at(i) == OUT2){
			instr.resize(i);
			break;
		}
	}

	//INPUT REDIRECTION
	bool alreadyInput = 0;
	for (unsigned i = 0; i < instrCopy.size(); i++){
		if (instrCopy.at(i) == IN && alreadyInput){
			cerr << "Error: too many input redirects\n";
			return -1;
		}
		else if (instrCopy.at(i) == IN){
			fd = doRedirIn(instrCopy.at(i+1),savestdin);

			while (i != instrCopy.size() && instrCopy.at(i) != OUT1 && instrCopy.at(i) != OUT2 && instrCopy.at(i) != "|"){
				instrCopy.erase(instrCopy.begin()+i);
			}
		}
	}

	int counter = 0;
	int pos = -1;

	for (unsigned i = 0; i < instrCopy.size(); i++){
		if (instrCopy.at(i) == OUT1 || instrCopy.at(i) == OUT2){
			counter++;
		}
	}



	//BEGIN EXEC
	int childStat;
	do {
		counter--;

		for (unsigned i = 0; i < instrCopy.size(); i++){
			if (instrCopy.at(i) == OUT1 || instrCopy.at(i) == OUT2){

				pos = (int)i;
			}
		}

		//OUTPUT REDIRECTION
		if (pos != -1){

			if (instrCopy.at(pos) == OUT1){
				fd = doRedirOut1(instrCopy.at(pos+1),savestdout);
			}
			else if (instrCopy.at(pos) == OUT2){
				fd = doRedirOut2(instrCopy.at(pos+1),savestdout);
			} 

			instrCopy.resize(pos);

		}

		vector<string> pipeline;
		bool noPipe = 1;
		for (unsigned l = 0; l < instrCopy.size();l++){
			if (!noPipe && (instrCopy.at(l) == IN || instrCopy.at(l) == OUT1 || instrCopy.at(l) == OUT2)){
				break;
			}
			if (!noPipe){
				pipeline.push_back(instrCopy.at(l));
			}
			if (instrCopy.at(l) == "|" && noPipe){
				noPipe = 0;
				while (l > 0){
					if (instrCopy.at(l) == IN && instrCopy.at(l) == OUT1 && instrCopy.at(l) == OUT2){
						l++;
						break;
					}
					l--;
				}
				l--;
			}
		}

		//EXECUTION
		if (!pipeline.empty()){
			if (-1 == doPipe(pipeline))
				cerr << "Piping failed\n";
		}
		else {
			int pid = fork();

			instr.push_back(NULL);

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
		}


		if (fd != -1 && -1 == close(fd))
			perror("There was an error with close()");

		if (savestdin != -1 && -1 == dup2(savestdin,0))
			perror("There was an error with dup2()");

		if (savestdout != -1 && -1 == dup2(savestdout, 1))
			perror("There was an error with dup2()");


	} while (counter > 0);

	return childStat;
}
//------------------------------------------------------------



void doLogic (vector<char*> a){
	vector<char*> cmd = a;
	vector<char*> segment;

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
	char name[25];

	gethostname(name,sizeof name);

	for (int i = 0; i < 25; i++){
		if ( name[i] == '.'){ 
			name[i] = '\0';
			break;
		}

	}

	while (true){
		cout << get_current_dir_name() << endl;
		cout << getlogin() << '@' << name  << " $ ";
		getline(cin, cmd);
		
		commentOut(cmd);


		for (unsigned i = 0; i < cmd.length(); i++){
			if (i < cmd.length()-1 && ((cmd.at(i) == '&' && cmd.at(i+1) == '&') || (cmd.at(i) == '|' && cmd.at(i+1) == '|') || (cmd.at(i) == '>' && cmd.at(i+1) == '>'))){
				cmd.insert(i+2, " ");
				cmd.insert(i, " ");
				i += 2;
			}
			else if (cmd.at(i) == ';' || cmd.at(i) == '<' || cmd.at(i) == '>' || cmd.at(i) == '|'){
				cmd.insert(i+1, " ");
				cmd.insert(i, " ");
				i++;
			}
		}

		
		//resolve space after command scenario
		if (cmd.at(cmd.size() - 1) == ' ') 
			cmd.erase(cmd.begin() + cmd.size() - 1 );

		//check for exit command
		if (cmd == "exit") exit(0);
	
		doLogic(str_parse(cmd));	

		cout << endl;
	}

	return 0;

}

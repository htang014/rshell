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
#include <signal.h>
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

int doRedirOut1(string outfile, int & savestdout,int replaceFD){
	int fd;
	if (-1 == (fd = open(outfile.c_str(), O_CREAT|O_WRONLY, 0666))){
		perror("there was an error with open()");
		return -1;
	}

	if (-1 == (savestdout = dup(replaceFD))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, replaceFD))
		perror("There was an error with dup2()");

	return fd;

}

int doRedirOut2(string outfile, int & savestdout,int replaceFD){
	int fd;
	if (-1 == (fd = open(outfile.c_str(), O_CREAT|O_APPEND|O_WRONLY, 0666))){
		perror("there was an error with open()");
		return -1;
	}

	if (-1 == (savestdout = dup(replaceFD))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, replaceFD))
		perror("There was an error with dup2()");

	return fd;

}

int doRedirIn(string infile, int & savestdin){
	int fd = 0;
	if (-1 == (fd = open(infile.c_str(), O_RDONLY))){
		perror("there was an error with open()");
		return -1;
	}

	if (-1 == (savestdin = dup(0))){
		perror("There was an error with dup()");
	}

	if (-1 == dup2(fd, 0))
		perror("There was an error with dup2()");


	return fd;

}

int doRedirInStr(string instring, int & savestdin){
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	int fd[2];


	if (pipe(fd) == -1){
		perror("There was an error with pipe()");
		return -1;
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

		cout << instring;

		exit(1);
	}
	else if (pid > 0){
		sigignore(SIGINT);
		if (-1 == (savestdin = dup(0)))
			perror("There was an error with dup()");
		if (-1 == wait(0))
			perror("There was an error with wait()");

		if (-1 == dup2(fd[PIPE_READ], 0))
			perror("There was an error with dup2()");
		if (-1 == close(fd[PIPE_WRITE]))
			perror("There was an error with close()");

	}

	return fd[0];

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
		sigignore(SIGINT);
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

	for (unsigned j = 0; j < argLeftC.size(); j++){
		if (argLeftC.at(j) != NULL)
			delete[] argLeftC.at(j);
	}
	for (unsigned k = 0; k < argRightC.size(); k++){
		if (argRightC.at(k) != NULL)
			delete[] argRightC.at(k);
	}

	return status;
}
//------------------------------------------------------------

void changeDir (string a){
	char *dirName, *cwd, *pwd, *home, *oldpwd;
	string fullPath;

	//syscall variables
	if (NULL == (cwd = getcwd(NULL,0))){
		perror("There was an error with getcwd()");
	}
	if (NULL == (pwd = getenv("PWD"))){
		perror("There was an error with getenv()");
	}
	if (NULL == (home = getenv("HOME"))){
		perror("There was an error with getenv()");
	}
	if (NULL == (oldpwd = getenv("OLDPWD"))){
		perror("There was an error with getenv()");
	}
	///////////
	//
	if (NULL == (dirName = get_current_dir_name())){
		perror("There was an error with get_current_dir_name()");
	}

	fullPath.append(dirName);
	fullPath.append("/");
	fullPath.append(a);

	if (a.empty()){
		fullPath = home;
	}
	if (a == "-"){
		fullPath = oldpwd;
	}

	if (-1 == chdir(fullPath.c_str())){
		perror("There was an error with chdir()");
		free(dirName);
		free(cwd);
		return;
	}

	//update value of oldpwd env var
	if (-1 == setenv("OLDPWD", pwd, 1)){
		perror("There was an error with setenv()");
	}

	//update value of cwd
	free(cwd);
	if (NULL == (cwd = getcwd(NULL,0))){
		perror("There was an error with getcwd()");
	}

	//update value of pwd env var
	if (-1 == setenv("PWD", cwd, 1)){
		perror("There was an error with setenv()");
	}

	free(dirName);
	free(cwd);

}
//------------------------------------------------------------
int doExec(vector<char*> instr){
	string firstStr = instr.at(0);
	if (firstStr == "exit")
		return -10;

	if (instr.empty())
		return 0;

	string IN = "<";
	string IN2 = "<<<";
	string OUT1 = ">";
	string OUT2 = ">>";
	int fd = -1, savestdout = -1, savestdin = -1;

	vector<string> instrCopy; //backup instr to a mutable vector<string>
	for (unsigned k = 0; k < instr.size(); k++){
		string s = instr.at(k);
		instrCopy.push_back(s);
	}

	#define ISFDOUT1(x) (instrCopy.at(x).size() == 2 && isdigit(instrCopy.at(x).at(0)) && instrCopy.at(x).at(1) == '>')
	#define ISFDOUT2(x) (instrCopy.at(x).size() == 2 && isdigit(instrCopy.at(x).at(0)) && instrCopy.at(x).at(1) == '>' && instrCopy.at(x).at(2) == '>')


	if (instrCopy.back() == "<" || instrCopy.back() == "<<<"
															|| instrCopy.back() == ">" 
															|| instrCopy.back() == ">>" 
															|| instrCopy.back() == "|" 
															|| ISFDOUT1(instrCopy.size()-1) 
															|| ISFDOUT2(instrCopy.size()-1)){
		cerr << "Invalid redirection at back()\n";
		return -1;
	}
	if (instrCopy.at(0) == "<" || instrCopy.at(0) == "<<<" 
															|| instrCopy.at(0) == ">" || instrCopy.back() == ">>" 
															||  instrCopy.at(0) == "|" 
															|| ISFDOUT1(0) 
															|| ISFDOUT2(0)){
		cerr << "Invalid redirection at front()\n";
		return -1;
	}


	for (unsigned i = 0; i < instrCopy.size(); i++){
		if (instrCopy.at(i) == IN || instrCopy.at(i) == IN2 
															|| instrCopy.at(i) == OUT1 
															|| instrCopy.at(i) == OUT2 
															|| ISFDOUT1(i) 
															|| ISFDOUT2(i)){
			instr.resize(i);
			break;
		}
	}

	//INPUT REDIRECTION
	bool alreadyInput = 0;
	for (unsigned i = 0; i < instrCopy.size(); i++){
		if ((instrCopy.at(i) == IN || instrCopy.at(i) == IN2) && alreadyInput){
			cerr << "Error: too many input redirects\n";
			return -1;
		}
		else if (instrCopy.at(i) == IN2){
			alreadyInput = 1;
			string str;
			for (unsigned j = i+1; j < instrCopy.size(); j++){
				if (instrCopy.at(j) == "<" || instrCopy.at(j) == ">" 
																		|| instrCopy.at(j) == ">>" 
																		|| instrCopy.at(j) == "|" 
																		|| ISFDOUT1(j) 
																		|| ISFDOUT2(j))
					break;
				
				str.append(instrCopy.at(j));
				str.append(" ");
			}

			if (str.at(0) != '\"' || str.at(str.size()-2) != '\"'){
				cerr << "Redirected to invalid input string";
				return -1;
			}
			else {
				str.erase(str.begin());
				str.erase(str.end() - 2);
				str.erase(str.end() - 1);
			}

			if (-1 == (fd = doRedirInStr(str,savestdin)))
				return -1;


			while (i != instrCopy.size() && instrCopy.at(i) != OUT1 && instrCopy.at(i) != OUT2 && instrCopy.at(i) != "|" && !ISFDOUT1(i) && !ISFDOUT2(i)){
				instrCopy.erase(instrCopy.begin()+i);
			}
		}
		else if (instrCopy.at(i) == IN){
			alreadyInput = 1;
			if (-1 == (fd = doRedirIn(instrCopy.at(i+1),savestdin)))
					return -1;

			while (i != instrCopy.size() && instrCopy.at(i) != OUT1 && instrCopy.at(i) != OUT2 && instrCopy.at(i) != "|" && !ISFDOUT1(i) && !ISFDOUT2(i)){
				instrCopy.erase(instrCopy.begin()+i);
			}
		}
	}

	//COUNT NUMBER OF OUTPUTS; INITIALIZE POS
	int counter = 0;
	int pos = -1;

	for (unsigned i = 0; i < instrCopy.size(); i++){
		if (instrCopy.at(i) == OUT1 || instrCopy.at(i) == OUT2 || ISFDOUT1(i) || ISFDOUT2(i)){
			counter++;
		}
	}

	//BEGIN EXEC
	int childStat;
	do {
		counter--;

		for (unsigned i = 0; i < instrCopy.size(); i++){
			if (instrCopy.at(i) == OUT1 || instrCopy.at(i) == OUT2 || ISFDOUT1(i) || ISFDOUT2(i)){

				pos = (int)i;
			}
		}


		//OUTPUT REDIRECTION
		if (pos != -1){

			if (ISFDOUT1(pos)){
				if (-1 == (fd = doRedirOut1(instrCopy.at(pos+1),savestdout, instrCopy.at(pos).at(0) - '0')))
					return -1;
			}
			else if (ISFDOUT2(pos)){
				if (-1 == (fd = doRedirOut2(instrCopy.at(pos+1),savestdout, instrCopy.at(pos).at(0) - '0')))
					return -1;
			}
			else if (instrCopy.at(pos) == OUT1){
				if ( -1 == (fd = doRedirOut1(instrCopy.at(pos+1),savestdout, 1)))
					return -1;
			}
			else if (instrCopy.at(pos) == OUT2){
				if ( -1 == (fd = doRedirOut2(instrCopy.at(pos+1),savestdout, 1))){
					return -1;
				}
			
			} 

			instrCopy.resize(pos);

		}


		vector<string> pipeline;
		bool noPipe = 1;
		for (unsigned l = 0; l < instrCopy.size();l++){
			if (!noPipe && (instrCopy.at(l) == IN || instrCopy.at(l) == OUT1 
																						|| instrCopy.at(l) == OUT2 
																						|| ISFDOUT1(l) 
																						|| ISFDOUT2(l) 
																						|| instrCopy.at(l) == IN2)){
				break;
			}
			if (!noPipe){
				pipeline.push_back(instrCopy.at(l));
			}
			if (instrCopy.at(l) == "|" && noPipe){
				noPipe = 0;
				while (l > 0){
					if (instrCopy.at(l) == IN || instrCopy.at(l) == OUT1 
																		|| instrCopy.at(l) == OUT2 
																		|| instrCopy.at(l) == IN2 
																		|| ISFDOUT1(l) 
																		|| ISFDOUT2(l)){
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
		else if (instrCopy.at(0) == "cd"){
			if (instrCopy.size() > 1)
				changeDir(instrCopy.at(1));
			else
				changeDir("");
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
				sigignore(SIGINT);
				int waitPid = 0;

				if (-1 == waitpid(waitPid, &childStat, 0)){
					perror("There was an error with wait()");
				}

				childStat = WEXITSTATUS(childStat);
			}
		}


		if (fd != -1 && -1 == close(fd))
			perror("There was an error with close()");

		if (counter <= 0){
			if (savestdin != -1 && -1 == dup2(savestdin,0))
				perror("There was an error with dup2()");
		}
		else 
			rewind (stdin);

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
			if (status == -10){

				for (unsigned i = 0; i< a.size(); i++){
					if (a.at(i) != NULL)
						delete[] a.at(i);
				}
			 	exit(0);	

			}
			segment.clear();
			if (status == 0){

				for (unsigned i = 0; i< a.size(); i++){
					if (a.at(i) != NULL)
						delete[] a.at(i);
				}
			 	return;	
			}
		}
		else if (strcmp(cmd.at(0), AND) == 0){
			int status = doExec(segment);
			if (status == -10){

				for (unsigned i = 0; i< a.size(); i++){
					if (a.at(i) != NULL)
						delete[] a.at(i);
				}
			 	exit(0);	

			}

			segment.clear();
			if (status != 0){

				for (unsigned i = 0; i< a.size(); i++){
					if (a.at(i) != NULL)
						delete[] a.at(i);
				}
			 	return;	

			}
		}
		else if (strcmp(cmd.at(0), END) == 0){
			int status = doExec(segment);
			if (status == -10){

				for (unsigned i = 0; i< a.size(); i++){
					if (a.at(i) != NULL)
						delete[] a.at(i);
				}
			 	exit(0);	

			}

			segment.clear();
		}
		else {
			segment.push_back(cmd.at(0));
		}
		cmd.erase(cmd.begin());
	}
	int finalStatus = doExec(segment);
	if (finalStatus == -10){

		for (unsigned i = 0; i< a.size(); i++){
			if (a.at(i) != NULL)
				delete[] a.at(i);
			}
		 	exit(0);	

	}


	for (unsigned i = 0; i< a.size(); i++){
		if (a.at(i) != NULL)
			delete[] a.at(i);
	}
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
		if (arg.empty()) break;	
		argList.push_back(arg);
	}

	//convert c++ string vector into cstring array
	transform(argList.begin(), argList.end(), back_inserter(argListC), convert);
	argListC.push_back(NULL);

	return argListC;
}

//-----------------------------------------------------------

void handler(int signum){
	switch(signum){
		case SIGINT:
			cout << endl;
			break;

		default:
			break;
	}
}

//------------------------------------------------------------

struct sigaction curr, prev;

int main () {
	curr.sa_handler = handler;
	if (prev.sa_handler != SIG_IGN)
		sigaction(SIGINT, &curr, &prev);

	string cmd;
	char name[26];


	if (-1 == gethostname(name,25)){
		perror("There was an error with gethostname()");
	}

	for (int i = 0; i < 25; i++){
		if ( name[i] == '.'){ 
			name[i] = '\0';
			break;
		}

	}

	while (true){
		char * loginID, * dirName, * home;
		string homeStr, dirNameStr;

		if (NULL == (loginID = getlogin())){
			perror("There was an error with getlogin()");
		}
		if (NULL == (dirName = get_current_dir_name())){
			perror("There was an error with get_current_dir_name()");
		}
		if (NULL == (home = getenv("HOME"))){
			perror("There was an error with getenv()");
		}
		homeStr = home;
		dirNameStr = dirName;

		dirNameStr.replace(dirNameStr.find(homeStr),homeStr.size(),"~");

		cout << dirNameStr << endl;
		cout << loginID << '@' << name  << " $ ";

		free(dirName);

		cin.clear();
		getline(cin, cmd);

		commentOut(cmd);

		if (cmd.empty())
			continue;	

		//check for connectors; if found place spaces before and after for better parsing
		for (unsigned i = 0; i < cmd.length(); i++){
			if (i < cmd.length()-1 && isdigit(cmd.at(i)) && cmd.at(i+1) == '>'){ //"#>" connector
				cmd.insert(i+2, " ");
				cmd.insert(i, " ");
				i += 2;
			}
			else if (i < cmd.length()-2 && isdigit(cmd.at(i)) && cmd.at(i+1) == '>' && cmd.at(i+2) == '>'){ //"#>>" connector
				cmd.insert(i+3, " ");
				cmd.insert(i, " ");
				i += 3;
			}
			else if (i < cmd.length()-2 && (cmd.at(i) == '<' && cmd.at(i+1) == '<' && cmd.at(i+2) == '<')){ //"<<<" connector
				cmd.insert(i+3, " ");
				cmd.insert(i, " ");
				i += 3;
			}
			else if (i < cmd.length()-1 && ((cmd.at(i) == '&' && cmd.at(i+1) == '&') ||
					 											(cmd.at(i) == '|' && cmd.at(i+1) == '|') ||
																(cmd.at(i) == '>' && cmd.at(i+1) == '>'))){
				cmd.insert(i+2, " ");
				cmd.insert(i, " ");
				i += 2;
			}
			else if (cmd.at(i) == ';' || cmd.at(i) == '<' 
						|| cmd.at(i) == '>' || cmd.at(i) == '|'){
				cmd.insert(i+1, " ");
				cmd.insert(i, " ");
				i++;
			}
		}


		
		//resolve space after command scenario
		if (cmd.at(cmd.size() - 1) == ' ') 
			cmd.erase(cmd.begin() + cmd.size() - 1 );


		extern void __libc_freeres (void);

		//check for exit command
		if (cmd == "exit") exit(0);

		/*if (cmd.substr(0,2) == "cd"){
			if (cmd.size() > 3)
				changeDir(cmd.substr(3));
			else
				changeDir("");
		}
		else{*/
			doLogic(str_parse(cmd));
		//}	

		cout << endl;
	}

	return 0;

}

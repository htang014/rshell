#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <pwd.h>
#include <algorithm>

using namespace std;

//FLAGS
void not_a_flag (vector<char*> & files){
		
	for (vector<char*>::iterator i = files.begin(); i != files.end();){
		char c = **i;

		if (c == '.')
			files.erase(i);
		else
			i++;
	}

}

void l_flag (vector<char*> & files, char & whitespace) {
	whitespace = '\n';
	vector<char*> filesCopy = files;
	files.clear();

	for (vector<char*>::iterator i = filesCopy.begin(); i != filesCopy.end(); ++i) {

		struct stat buf;
		string s = "./";
		s.append(*i);


		const char* path = s.c_str();
		if (-1 == stat (path, &buf)) {
			perror("There was a problem with stat()");
			exit(1);
		}

		struct passwd* ownerInfo;
		errno = 0;

		ownerInfo = getpwuid(buf.st_uid);
	
		if (errno != 0) {
			perror("There was an error with getpwuid()");
			exit(1);
		}

		char* userCopy = (char*)malloc(strlen(ownerInfo->pw_name) + 1);
		strcpy(userCopy, ownerInfo->pw_name);

		files.push_back(userCopy);
		strcat(files.back(), " ");
		strcat(files.back(), *i);


	}

}



//MAIN
int main(int argc, char* argv[]){
	bool a = 0, l = 0, R = 0;
	char whitespace = ' ';


	vector<char*> filenames;


	DIR* dirp;
	if (NULL == (dirp = opendir(".")) ){
		perror("There was an error with opendir()");
		exit(1);
	}

	struct dirent *filespecs;
	errno = 0;
	while(NULL != (filespecs = readdir(dirp))){
		filenames.push_back(filespecs->d_name);
	}
	if (errno != 0){
		perror("There was an error with readdir()");
		exit(1);
	}

	//FIND FLAGS ATTACHED TO EXECUTION
	for (int i = 1; i < argc; i++) {
		if (*argv[i] == '-'){
			if (strchr(argv[i], 'a') != NULL) a = 1;
			if (strchr(argv[i], 'l') != NULL) l = 1;
			if (strchr(argv[i], 'R') != NULL) R = 1;
		}

	}

	//CONDITIONAL ADJUSTMENTS
	if (!a) not_a_flag(filenames); //no -a
	
	if (l) l_flag(filenames, whitespace);


	//PRINTING CONTENTS OF FILENAME VECTOR
	for (vector<char*>::iterator it = filenames.begin();
	it != filenames.end(); ++it){
		cout << *it << whitespace;
	}
	if (whitespace != '\n')
		cout << endl;

	if (-1 == closedir(dirp)){
		perror("There was an error with closedir()");
		exit(1);
	}

	return 0;
}

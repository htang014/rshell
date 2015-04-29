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
#include <grp.h>
#include <sstream>

using namespace std;

struct permissions {
	permissions() : TYP("-"),
									O_R("-"),
									O_W("-"),
									O_X("-"),
									G_R("-"),
									G_W("-"),
									G_X("-"),
									A_R("-"),
									A_W("-"),
									A_X("-") {}

	char* TYP;
	char* O_R;
	char* O_W;
	char* O_X;
	char* G_R;
	char* G_W;
	char* G_X;
	char* A_R;
	char* A_W;
	char* A_X;
	
};

bool charCompare(const char * c1, const char * c2){
	return strcasecmp(c1, c2) < 0;
}

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

		errno = 0;

		struct passwd* ownerInfo;
		struct group* groupInfo;

		ownerInfo = getpwuid(buf.st_uid);
		groupInfo = getgrgid(buf.st_gid);

	
		if (errno != 0) {
			perror("There was an error with getpwuid()");
			exit(1);
		}


		permissions p;
		cout << buf.st_ino << endl;
		if (buf.st_mode & S_IRUSR) p.O_R = "r";
		if (buf.st_mode & S_IWUSR) p.O_W = "w";
		if (buf.st_mode & S_IXUSR) p.O_X = "x";
		if (buf.st_mode & S_IRGRP) p.G_R = "r";
		if (buf.st_mode & S_IWGRP) p.G_W = "w";
		if (buf.st_mode & S_IXGRP) p.G_X = "x";
		if (buf.st_mode & S_IROTH) p.A_R = "r";
		if (buf.st_mode & S_IWOTH) p.A_W = "w";
		if (buf.st_mode & S_IXOTH) p.A_X = "x";

		char* permlist = (char*)malloc(255);

		strcpy(permlist, p.TYP);
		strcat(permlist, p.O_R);
		strcat(permlist, p.O_W);
		strcat(permlist, p.O_X);
		strcat(permlist, p.G_R);
		strcat(permlist, p.G_W);
		strcat(permlist, p.G_X);
		strcat(permlist, p.A_R);
		strcat(permlist, p.A_W);
		strcat(permlist, p.A_X);

		char* userCopy = (char*)malloc(strlen(ownerInfo->pw_name) + 1); 
		strcpy(userCopy, ownerInfo->pw_name);

		char* grpCopy = (char*)malloc(strlen(groupInfo->gr_name) + 1);
		strcpy(grpCopy, groupInfo->gr_name);

		stringstream strs;
		strs << buf.st_size;
		string fsize = strs.str();
		char* fsizeC = (char*) fsize.c_str();

		struct tm * tm;
		char datetime[200];
		time_t temp = buf.st_mtime;
		tm = localtime(&temp);
		strftime(datetime, sizeof(datetime), "%b %d %H:%M", tm);

		files.push_back(permlist);

		strcat(files.back(), "\t");
		strcat(files.back(), userCopy);

		strcat(files.back(), "\t");
		strcat(files.back(), grpCopy);

		strcat(files.back(), "\t");
		strcat(files.back(), fsizeC);

		strcat(files.back(), "\t");
		strcat(files.back(), datetime);

		strcat(files.back(), "\t");
		strcat(files.back(), *i);



	}

}



//MAIN
int main(int argc, char* argv[]){
	bool a = 0, l = 0, R = 0;
	if (R); //temporary
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


	sort(filenames.begin(), filenames.end(), charCompare);
	
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

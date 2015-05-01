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

bool charCompare(const string c1, const string c2){
	return strcasecmp(c1.c_str(), c2.c_str()) < 0;
}

//FLAGS
//EXECUTES IF -a IS NOT PRESENT
void not_a_flag (vector<string> & files){
		
	for (vector<string>::iterator i = files.begin(); i != files.end();){
		char c = i->at(0);

		if (c == '.')
			files.erase(i);
		else
			i++;
	}

}

//EXECUTES IF -l IS PRESENT
void l_flag (vector<string> & files, char & whitespace, string dir) {
	whitespace = '\n';
	vector<string> filesCopy = files;
	files.clear();

	struct stat buf0;
	string s0 = dir;

	const char* path0 = s0.c_str();
	if (-1 == stat (path0, &buf0)) {
		perror("There was a problem with stat()");
		exit(1);
	}

	cout << "total " << buf0.st_size << endl; 

	
	for (vector<string>::iterator i = filesCopy.begin(); i != filesCopy.end(); ++i) {

		struct stat buf;
		string s = dir;
		s.append("/");
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

		cout << "tot " << buf.st_size << endl;


		//SETTING PERMISSIONS STRING
		permissions p;

		//SET SPECIAL FILE TYPE
		if (S_ISDIR(buf.st_mode)) p.TYP = "d";
		if (S_ISBLK(buf.st_mode)) p.TYP = "b";
		if (S_ISCHR(buf.st_mode)) p.TYP = "c";
		if (S_ISFIFO(buf.st_mode)) p.TYP = "p";
		if (S_ISLNK(buf.st_mode)) p.TYP = "l";
		if (S_ISSOCK(buf.st_mode)) p.TYP = "s";

		//SET RWX PERMISSIONS
		if (buf.st_mode & S_IRUSR) p.O_R = "r";
		if (buf.st_mode & S_IWUSR) p.O_W = "w";
		if (buf.st_mode & S_IXUSR) p.O_X = "x";
		if (buf.st_mode & S_IRGRP) p.G_R = "r";
		if (buf.st_mode & S_IWGRP) p.G_W = "w";
		if (buf.st_mode & S_IXGRP) p.G_X = "x";
		if (buf.st_mode & S_IROTH) p.A_R = "r";
		if (buf.st_mode & S_IWOTH) p.A_W = "w";
		if (buf.st_mode & S_IXOTH) p.A_X = "x";

		//ADD TO STRING
		string permlist = p.TYP;
		permlist.append(p.O_R);
		permlist.append(p.O_W);
		permlist.append(p.O_X);
		permlist.append(p.G_R);
		permlist.append(p.G_W);
		permlist.append(p.G_X);
		permlist.append(p.A_R);
		permlist.append(p.A_W);
		permlist.append(p.A_X);

		ostringstream oss;
		oss << buf.st_nlink;
		string linkNum = oss.str();

		string userCopy = ownerInfo->pw_name; 

		string grpCopy = groupInfo->gr_name;

		stringstream strs;
		strs << buf.st_size;
		string fsize = strs.str();

		struct tm * tm;
		char datetime[200];
		time_t temp = buf.st_mtime;
		tm = localtime(&temp);
		strftime(datetime, sizeof(datetime), "%b %d %H:%M", tm);

		files.push_back(permlist);

		files.back().append( "\t");
		files.back().append( linkNum);

		files.back().append( "\t");
		files.back().append( userCopy);

		files.back().append( "\t");
		files.back().append( grpCopy);

		files.back().append( "\t");
		files.back().append( fsize);

		files.back().append( "\t");
		files.back().append( datetime);

		files.back().append( "\t");
		files.back().append( *i);



	}

}

//DOES EVERYTHING; CAPABLE OF RECURSION
void pull_and_print(string dir, bool a, bool l, bool R){
	
	char whitespace = ' ';
	vector<string> filenames;

	DIR* dirp;
	if (NULL == (dirp = opendir(dir.c_str())) ){
		perror("There was an error with opendir()");
		exit(1);
	}

	struct dirent *filespecs;
	errno = 0;
	while(NULL != (filespecs = readdir(dirp))){
		string dName = filespecs->d_name;
		filenames.push_back(dName);
	}
	if (errno != 0){
		perror("There was an error with readdir()");
		exit(1);
	}

	//CONDITIONAL ADJUSTMENTS
	if (!a) not_a_flag(filenames); //no -a

	sort(filenames.begin(), filenames.end(), charCompare); //sort alphabetically
	
	vector<string> filesCopy = filenames;
	if (l) l_flag(filenames, whitespace, dir);

	if (R) cout << dir << ':' << endl;

	//PRINTING CONTENTS OF FILENAME VECTOR
	for (vector<string>::iterator it = filenames.begin();
	it != filenames.end(); ++it){
		cout << *it << whitespace;
	}
	if (whitespace != '\n')
		cout << endl;

	if (-1 == closedir(dirp)){
		perror("There was an error with closedir()");
		exit(1);
	}

	//PERFORM RECURSION IF -R
	if (R) {

		cout << endl;	

		for (vector<string>::iterator i = filesCopy.begin(); i != filesCopy.end(); ++i) {
			string new_dir = dir;

			if (*i != "." && *i != ".." && *i != ""){

				new_dir.append("/");
				new_dir.append(*i);
				struct stat buf;

				if (-1 == stat (new_dir.c_str(), &buf)) {
					perror("There was a problem with stat()");
					exit(1);
				}

				if (S_ISDIR(buf.st_mode)) {
					pull_and_print(new_dir, a, l, R);
				}
		
			}	

		}

	}

}



//MAIN
int main(int argc, char* argv[]){
	bool a = 0, l = 0, R = 0;
	if (R); //temporary

	//FIND FLAGS
	unsigned n = argc;
	for (unsigned i = 1; i < n; i++) {
		if (*argv[i] == '-'){

			string thisArg = argv[i];

			for (unsigned j = 1; j < thisArg.size(); j++){
				if (thisArg.at(j) == 'a') a = 1;
				else if (thisArg.at(j) == 'l') l = 1;
				else if (thisArg.at(j) == 'R') R = 1;
				else cout << "One or more flags not recognized.\n";
			}

		}

	}

	pull_and_print(".", a, l, R);

	return 0;
}

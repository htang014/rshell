#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
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

	bool b = strcasecmp(c1.c_str(), c2.c_str()) < 0;

	if (c1.at(0) == '.' && c2.at(0) != '.') b = 1;
	if (c2.at(0) == '.' && c1.at(0) != '.') b = 0;

	return b;
}

//FLAGS
//EXECUTES IF -a IS NOT PRESENT
void not_a_flag (vector<string> & files){

	vector<string>::iterator i = files.begin();
		
	while ( i != files.end()){

		if (i->at(0) == '.')
			i =	files.erase(i);
		else
			++i;
	}

}

//EXECUTES IF -l IS PRESENT
void l_flag (vector<string> & files, char & whitespace, string dir) {
	whitespace = '\n';
	vector<string> filesCopy = files;
	files.clear();

	int total = 0;
	
	for (vector<string>::iterator i = filesCopy.begin(); i != filesCopy.end(); ++i) {

		struct stat buf;
		string s = dir;
		s.append("/");
		s.append(*i);

		const char* path = s.c_str();

		int success = stat(path, &buf);
		if (-1 == success) {
			perror("There was a problem with stat()");
			//exit(1);
		}

		errno = 0;
		if  (success != -1) {
		struct passwd* ownerInfo;
		struct group* groupInfo;

		ownerInfo = getpwuid(buf.st_uid);
		groupInfo = getgrgid(buf.st_gid);

	
		if (errno != 0) {
			perror("There was an error with getpwuid()");
			exit(1);
		}

		total = total +  buf.st_blocks;

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

	total = total / 2;
	cout << "total " << total << endl;
	cout << setw(10);

}

bool narrow_files (vector<string> & fileParam, vector<string> & filenames){
	if (fileParam.empty()) return 0;

	filenames = fileParam;
	return 1;
}

//DOES EVERYTHING; CAPABLE OF RECURSION
void pull_and_print(string dir, bool a, bool l, bool R, vector<string> fileParam){
	
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
	bool fileParamExist = narrow_files (fileParam, filenames);

	if (!a) not_a_flag(filenames); //no -a

	sort(filenames.begin(), filenames.end(), charCompare); //sort alphabetically
	
	vector<string> filesCopy = filenames;
	
	if (fileParamExist) {
		vector<string>::iterator j = filenames.begin();
		while (j != filenames.end()){
			struct stat buf;

			if (-1 == stat(j->c_str(), &buf)){
				string errMsg = "Could not open ";
				errMsg.append(*j);
				perror(errMsg.c_str());
				j = filenames.erase(j);
			}
			else if (S_ISDIR(buf.st_mode)) {

				if (!R) {
					cout << *j << ':' << endl;
				}

				vector<string> empty;
				pull_and_print(*j, a, l, R, empty);
				filenames.erase(j);

				if (!R) 	cout << endl;
			}
			else {
				++j;
			}
			
	
		}
	}

	if (R) cout << dir << ':' << endl;

	if (l) l_flag(filenames, whitespace, dir);

	//PRINTING CONTENTS OF FILENAME VECTOR
	if (l) {
		for (unsigned i = 0; i < filenames.size(); i++){
			string buf;
			istringstream in(filenames.at(i));

			in >> buf;
			cout << buf;

			in >> buf;
			cout << setw(5) << buf;

			for (unsigned j = 0; j < 3; j++){
				in >> buf;
				cout << setw(12) << buf;
			}
			
			in >> buf;
			cout << setw(12) << buf << " ";


			for (unsigned k = 0; k < 2; k++){
				in >> buf;
				cout << buf << " ";
			}

			in >> buf;
			cout << setw(11) << buf;

			cout << endl;
		}
	}
	else if ( filenames.size() > 10) { //directory contains more than 10 items - create nice columns
		unsigned maxSize = 0;
		for (unsigned i = 0; i < filenames.size(); i++){
			if (filenames.at(i).size() > maxSize) maxSize = filenames.at(i).size();
		}

		unsigned numRows = filenames.size()/4;
	
		for (unsigned k = 0; k < numRows; k++) {
			for (unsigned i = k; i < filenames.size(); i = i + numRows){
				cout << left << setw(maxSize + 1) << filenames.at(i);
			}
			cout << endl;
		}
	}
	else {
		for (vector<string>::iterator it = filenames.begin();
		it != filenames.end(); ++it){
			cout << *it << whitespace;
		}
		if (whitespace != '\n')
			cout << endl;
	}

	
	//CLOSE DIRECTORY
	if (-1 == closedir(dirp)){
		perror("There was an error with closedir()");
		exit(1);
	}

	//PERFORM RECURSION IF -R
	if (R && !fileParamExist) {

		cout << endl;	

		for (vector<string>::iterator i = filesCopy.begin(); i != filesCopy.end(); ++i) {
			string new_dir = dir;

			if (*i != "." && *i != ".." && *i != ""){

				new_dir.append("/");
				new_dir.append(*i);
				struct stat buf;

				if (-1 == stat (new_dir.c_str(), &buf)) {
					perror("There was a problem with stat()");
					//exit(1);
				}
				else if (S_ISDIR(buf.st_mode)) {				//	cout << "TEST" << endl;
					pull_and_print(new_dir, a, l, R, fileParam);

				}
		
			}	

		}

	}

}



//MAIN
int main(int argc, char* argv[]){
	bool a = 0, l = 0, R = 0;
	vector<string> fileParam; 

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
		else {
			string s = argv[i];
			if (s.substr(0, 2) == "./"){
				s = s.substr(2, s.length());
			}
			fileParam.push_back(s);
		}

	}

	pull_and_print(".", a, l, R, fileParam);

	return 0;
}

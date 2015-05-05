#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <string.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <boost/tokenizer.hpp>
#include <boost/token_iterator.hpp>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <set>
#include <pwd.h>
#include <grp.h>
#include <time.h>


using namespace boost;
using namespace std;

int main(int argc, char ** argv){
	if(argc<3){
		cerr << "mv: missing destination file operand" << endl;
		return -1;
	}

	vector<string> holder;
	for(unsigned int i = 1; i<argc; i++){
		holder.push_back(string(argv[i]));
	}
	
	DIR *dirp;
	struct stat s;
	if(stat(holder[0].c_str(), &s) == -1){
		perror("stat");
		exit(1);
	}

	
	if(stat(holder[1].c_str(), &s)!=-1){ // if exists		
		if(S_IFDIR & s.st_mode){ //is a dir
			if( NULL == (dirp = opendir(holder[1].c_str()))){ 
				cerr << holder[1] << "does not exist" << endl;
				exit(1);
			}	
			if(-1 == link(holder[0].c_str(), (holder[1] + '/' + holder[0]).c_str())){
				perror("link");
				exit(1);
			}
			if(-1 == unlink(holder[0].c_str())){
				perror("unlink");
				exit(1);
			}
		}
	}
		
	else{ //not exist
		if(-1 == link(holder[0].c_str(), holder[1].c_str())){
			perror("link");
			exit(1);
		}
		if(-1 == unlink(holder[0].c_str())){
			perror("unlink");
			exit(1);
		}
	}

	return 0;
}

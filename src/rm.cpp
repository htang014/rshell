#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

bool hasr = false;

void recursive_check(int argc, vector<string> argv)
{
	struct stat s;
	for(int i = 0; i < argc; ++i)
	{
		if(((stat(argv.at(i).c_str(), &s)) == -1))
		{
			perror("stat");
			exit(1);
		}
		if(!(s.st_mode & S_IFDIR))		//if it's a file, unlink it
		{
			if(-1 == (unlink(argv.at(i).c_str())))		//if it doesnt exist, output error
			{
				perror("Does not exist");
			}
		}
		else
		{
			DIR *dirp;
			if(NULL == (dirp = opendir(argv.at(i).c_str())))
			{
				perror("opendir");
			}
			else if(hasr)
			{
				struct dirent *file;
				errno = 0;
				vector<string> files;
				while(NULL != (file = readdir(dirp)))
				{
					if(string(file->d_name) !="." && string(file->d_name) != ".."){
						files.push_back(argv[i] + '/' + string(file->d_name));
					}
				}
				if(errno != 0)
				{
					perror("readdir");
					exit(1);
				}
				if(-1 == closedir(dirp))
				{
					perror("closedir");
					exit(1);
				}
				if(files.empty())
				{
					if(-1 == (rmdir(argv.at(i).c_str())))
					{
						perror("rmdir");
					}
				}
				else 
				{
					recursive_check(files.size(), files);
					if( -1 == (rmdir(argv.at(i).c_str()))){
						perror("rmdir");
					}
				}
				//
				//files.clear();
			}
			else{
				cerr << "Directory is not empty!" << endl;
			}
		}
	}
}

int main(int argc, char **argv)
{
	vector<string> argvholder;
	for(int i = 1; i < argc; ++i)
	{
		if((argv[i][0] == '-') && (argv[i][1] == 'r') && (argv[i][2] == '\0')) 
		{
						hasr = true;
		}
		else{
			argvholder.push_back(string(argv[i]));
		}
	}
	recursive_check(argvholder.size(), argvholder);
	return 0;
}

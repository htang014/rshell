#include <stdio.h>
#include <string.h>
using namespace std;

/*void doExec(char* cmd){
	int pid = fork();

	if (pid == -1){
		perror("There was an error with fork()");
		exit(1);
	}
	else if (pid == 0){
		if (-1 == execvp(cmd, argv)){
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
}*/

void str_parse(){
	

}


int main () {
		
	char a;

	printf("$ ");

	while (a != EOF) {
		char temp[100];
		a = getchar();

		if (a == '\n') {
			str_parse();
			printf(temp);
			printf("\n");
			printf("$ ");
		}
		else {
			strncat(temp, &a, 1);	
		}
		
	}
		
	return 0;

}

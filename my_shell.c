#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens*/
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for(i =0; i < strlen(line); i++){
		char readChar = line[i];
		if (readChar == ' ' || readChar == '\n' || readChar == '\t' || readChar == '&'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		} 
		else {
			token[tokenIndex++] = readChar;
		}
	}
	free(token);
	tokens[tokenNo] = NULL ;
	return tokens;
}


//HANDLER FOR SIGINT(CTRL+C): KILLING ONLY FOREGROUND PROCESS
pid_t fid = 0, first = 0;
static void killfg() {
	if(fid!=0)
		kill(fid, SIGTERM);
	printf("\n");
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i, count = 0;

	signal(SIGINT, killfg);

	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		pid_t pid = 0;
		int status;	

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		if(tokens[0]==NULL){
			pid=waitpid(-1, &status, WNOHANG);
			if(pid>0){
				count--;
				printf("Shell: Background process finished\n");
			}
			continue;
		}

		//EXIT COMMAND
		if(strcmp(tokens[0],"exit")==0){
			free(tokens);
			killpg(first, SIGTERM);
			break;
		}

		//CD COMMAND
		if(strcmp(tokens[0],"cd")==0){
			if(tokens[1]!=NULL){
				if(chdir(tokens[1])!=0)
					printf("ERROR: Wrong Path\n");
			}
			continue;
		}
	
		//FLAG FOR BACKGROUND PROCESS
		int background = 0;
		if(line[strlen(line)-2] == '&')
			background = 1;

		//CREATING CHILD PROCESS
		pid = fork();
		if(pid==0){
			if(execvp(tokens[0], tokens) < 0){
				printf("ERROR: exec failed\n");
                exit(1);
			}
		}
		else if(pid>0){
			if(!background){
				//Foreground child process
				fid = pid;
				pid = waitpid(pid, &status, 0);   
			}
			else{
				//Background child process
				if(count==0){
					first = pid;
				}
				count++;
				setpgid(pid, first);	//Putting background child processes in a group
			}
		}
		else{
			perror("In fork():");
		}

		//REAPING ZOMBIE BACKGROUND PROCESSES
		pid=waitpid(-1, &status, WNOHANG);
		if(pid>0){
			count--;
			printf("Shell: Background process finished\n");
		}			
	}

	return 0;
}
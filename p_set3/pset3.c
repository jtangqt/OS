#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>
#include <wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <libgen.h>

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!file_val){
		if(!error_val)
			fprintf(stderr, "%s\n", message);
		else
			fprintf(stderr, "%s Error value: %s\n", message, error_val);
	}
	else
		fprintf(stderr, "%s Error value: %s. Name: %s\n", message, error_val, file_val);
	exit(1);
}

void aesthetics(){
	char cwd[1024];
	if(!getcwd(cwd, 1024))
		error_message("ERROR: could not open directory", strerror(errno), cwd);
	fprintf(stderr, "%s$ ", basename(cwd));
}

void redirect(int f, int fd, const char *filename){
	//fprintf(stderr, "redirecting, %s", filename);
	int tmp; 
	if((tmp = open(filename, fd, 0666)) > 0){
		//fprintf(stderr, "opened file");
		if(dup2(tmp, f) < 0){
			switch(f){
				case 0:
					fprintf(stderr, "ERROR: could not redirect from stdin. Error message: %s. File name: %s.\n", strerror(errno), filename);
					exit(1);
				case 1:
					fprintf(stderr, "ERROR: could not redirect to stdout. Error message: %s. File name: %s.\n", strerror(errno), filename);
					exit(1);
				case 2: 
					fprintf(stderr, "ERROR: could not redirect to stderr. Error message: %s. File name: %s.\n", strerror(errno), filename);
					exit(1); 
				default: 
					error_message("You should not be here.", 0, 0);
			}
		}
		//fprintf(stderr, "closed");
		if(close(tmp) < 0){
			error_message("ERROR: could not close file.", strerror(errno), filename); 
			exit(1); 
		}
	}
	else{
		error_message("ERROR: could not open file.", strerror(errno), filename); 
		exit(1); 
	}
}

int processing(char *input){
	//parse to tokens
	char line[strlen(input)];
	strncpy(line, input, strlen(input));
	line[strlen(input) -1] = 0; 
	
	char *argv_new[1024];
	int new = 0; 
	char *in = NULL; 
	char *a_out = NULL;
	char *t_out = NULL; 
	char *a_err = NULL; 
	char *t_err = NULL; 

	char *token = strtok(line, " \t\n\r");

	while(token){
		if(token[0] == '<') //< 
			in = token + 1; 
		else if(token[0] == '>'){ //>
			if(strlen(token) > 1 && token[1] == '>') //>>
				a_out = token + 2; 
			else
				t_out = token + 1; 
		}
		else if(strlen(token) > 1 && token[0] == '2' && token[1] == '>'){ //2>
			if(token[2] == '>') //2>>
				a_err = token + 3; 
			else
				t_err = token + 2; 
		}	
		else{
			if(new < 1023){
				argv_new[new++] = token; 
			}else
				fprintf(stderr, "Too many arguments, truncating to 1023 arguments");
		}

		token = strtok(NULL, " \t\n\r"); 
	}

	argv_new[new] = NULL; 

	if(argv_new[0] != NULL && !strcmp(argv_new[0], "cd")){ //directory
		if(new == 1){
			fprintf(stderr, "ERROR: no directory path was specified.");
			return 1; 
		}
		char *new_dir = argv_new[1]; 
		if(chdir(new_dir)){
			fprintf(stderr, "ERROR: could not open directory (perhaps try relative pathname). Error message: %s. Path name: %s.\n", strerror(errno), new_dir);
			return 1; 
		}
		return 0; 
	}	
	
	pid_t pid = fork(); 
	if(pid == -1){
		error_message("ERROR: could not fork to create a new process.", strerror(errno), NULL); 
	}
	else if(!pid){
		int tmp; 
		
		if(in != NULL){
			fprintf(stderr, "in in\n");
			redirect(0, O_RDONLY, in);
		}
		//fprintf(stderr, "debug1"); 
		if(a_out != NULL){
			fprintf(stderr, "in a_out\n");
			redirect(1, O_RDWR| O_APPEND| O_CREAT, a_out); 
		}
		//fprintf(stderr, "debug2");
		else if(t_out != NULL){
			fprintf(stderr, "in t_out\n");
			redirect(1, O_RDWR| O_TRUNC| O_CREAT, t_out); 
		}
		//fprintf(stderr, "debug3"); 
		if(a_err != NULL){
			fprintf(stderr, "in a_err\n");
			redirect(2, O_RDWR| O_APPEND| O_CREAT, a_err); 
		}
	//	fprintf(stderr, "debug4"); 
		else if(t_err != NULL){
			fprintf(stderr, "in t_err\n");
			redirect(2, O_RDWR| O_TRUNC| O_CREAT, t_err); 
		}
		if(execvp(argv_new[0], argv_new) < 0){
			error_message("ERROR: execvp() failed.", strerror(errno), argv_new[0]); 	
		}
	}

	return 0;

}

int main(int argc, char **argv){
	
	FILE *input;
	int val; 
	char cin[1024]; //this gave a segfault when I did char * -> pointer 

	//see if we're taking stdin or file (for all commands)
	if(argc == 1)
		input = stdin; 
	else if (!(input = fopen(argv[1], "r")))
		error_message("ERROR: could not open file", strerror(errno), argv[1]);

	//making sure first time "enter" is pressed is also aesthetically pleasing
	if(input == stdin) 
		aesthetics(); 

	//see if it's getting information from the command line
	while(fgets(cin, 1024, input) != NULL){
		//if it needs to be processed - not a hashtag and not a new line
		if(cin[0] != '#' && cin[0] != '\n'){
			//understand the data
			val += processing(cin);
		}
		//just for aesthetics - happens after every line
		if(input == stdin)
			aesthetics(); 
	}

	return val; 

	
}
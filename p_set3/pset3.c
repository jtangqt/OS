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

void redirect(int f, int ta, const char *filename){
	int tmp; 
	if((tmp = open(filename, O_RDONLY| ta| O_CREAT, 0666)) > 0){
		if(dup2(tmp, f) < 0){
			error_message("ERROR: could not redirect input.", strerror(errno), filename);
			exit(1); 
		}
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
		if(new == 1)
			error_message("ERROR: no directory path was specified.", 0, 0);
		char *new_dir = argv_new[1]; 
		fprintf(stderr, "Changing to the specified directory %s...\n", new_dir);
		if(chdir(new_dir))
			error_message("ERROR: could not open directory (perhaps try relative pathname)", strerror(errno), new_dir);
	}	
	
	pid_t pid = fork(); 
	if(pid == -1){
		error_message("ERROR: could not fork to create a new process.", strerror(errno), NULL); 
	}
	else if(!pid){
		int tmp; 
		
		if(in != NULL){
			if((tmp = open(in, O_RDONLY, 0666)) > 0){
				if(dup2(tmp, 0) < 0){
					error_message("ERROR: could not redirect input.", strerror(errno), in); 
					exit(1);
				}
			}
			else{
				error_message("ERROR: could not open file.", strerror(errno), in); 
				exit(1);
			}
		}
		fprintf(stderr, "debug1"); 
		if(a_out != NULL){
			redirect(1, O_APPEND, a_out); 
		}
		//fprintf(stderr, "debug2");
		else if(t_out != NULL){
			redirect(1, O_TRUNC, t_out); 
		}
		fprintf(stderr, "debug3"); 
		if(a_err != NULL){
			redirect(2, O_APPEND, a_err); 
		}
	//	fprintf(stderr, "debug4"); 
		else if(t_err != NULL){
			redirect(2, O_TRUNC, t_err); 
		}
		if(execvp(argv_new[0], argv_new) < 0){
			error_message("ERROR: execvp() is not working", strerror(errno), argv_new[0]); 	
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

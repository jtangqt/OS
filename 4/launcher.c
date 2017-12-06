#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


int error_message(const char *message, const char *error_val, const char *file_val){
	if(!file_val){
		if(!error_val)
			fprintf(stderr, "%s\n", message);
		else
			fprintf(stderr, "%s Error value: %s\n", message, error_val);
	}
	else
		fprintf(stderr, "%s Error value: %s. Name: %s\n", message, error_val, file_val);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
	pid_t pid_1, pid_2, pid_3;
	int wstatus; 
	int pipe1[2], pipe2[2];

	if(argc > 2){
		error_message("ERROR: Please follow this format\n./launcher [number]", strerror(errno), 0); 
	}
	if(pipe(pipe1) == -1)
		error_message("ERROR: Pipe1", strerror(errno), 0);

	//first fork
	pid_1 = fork(); 
	if(pid_1 == -1)
		error_message("ERROR: could not fork to create a new process.", strerror(errno), 0);
	else if (!pid_1)
	else{
		if(pipe(pipe2) == -1)
			error_message("ERROR: Pipe2", strerror(errno), 0);

		//second fork
		pid_2 = fork();
		if(pid_2 == -1)
		else if (!pid_2)
		else{

			//third fork
			pid_3 = fork(); 
			if(pid_3 == -1)
			else if (!pid_3)
			else{

			}
		}
	}
}
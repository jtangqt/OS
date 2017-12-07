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

void close_fd(int closing){
	char buffer[50];
	sprintf(buffer, "ERROR: Could not close file descripter %d.", closing);
	if(close(closing) < 0)
		error_message(buffer, strerror(errno), 0);
}

int redirect(int old, int new){
	char buffer[10];
	sprintf(buffer, "%i", new);
	if(dup2(old, new) < 0)
		error_message("ERROR: dup2 encountered an error.", strerror(errno), buffer);
	close_fd(old);
	return 0; 
}

int main(int argc, char **argv){
	pid_t pid_1, pid_2, pid_3;
	int wstatus; 
	int pipe_fd1[2], pipe_fd2[2];

	if(argc > 2)
		error_message("ERROR: Please follow this format\n./launcher [number].", strerror(errno), 0); 

	if(pipe(pipe_fd1) == -1)
		error_message("ERROR: Pipe_fd1 could not be piped.", strerror(errno), 0);

	//first fork
	pid_1 = fork(); 
	if(pid_1 == -1)
		error_message("ERROR: could not fork to create a new process for wordgen.", strerror(errno), 0);

	else if (!pid_1){
		close_fd(pipe_fd1[0]);
		redirect(pipe_fd1[1], STDOUT_FILENO);
		char *argv_new[3] = {};
		argv_new[0] = "./wordgen";

		if(argc > 1)
			argv_new[1] = argv[1];

		if(execvp(argv_new[0], argv_new) < 0)
			error_message("ERROR: execvp() failed.", strerror(errno), argv_new[0]);
	}

	else{
		if(pipe(pipe_fd2) == -1)
			error_message("ERROR: Pipe_fd2 could not be piped.", strerror(errno), 0);

		//second fork
		pid_2 = fork();
		if(pid_2 == -1)
			error_message("ERROR: could not fork to create a new process for wordsearch.", strerror(errno), 0);
		
		else if (!pid_2){
			close_fd(pipe_fd2[0]);
			close_fd(pipe_fd1[1]);
			redirect(pipe_fd2[1], STDOUT_FILENO);
			redirect(pipe_fd1[0], STDIN_FILENO);
			char *argv_new[3] = {};
			argv_new[0] = "./wordsearch";
			argv_new[1] = "words.txt";

			if(execvp(argv_new[0], argv_new) < 0)
				error_message("ERROR: execvp() failed.", strerror(errno), argv_new[0]);
		}

		else{

			//third fork
			pid_3 = fork(); 
			if(pid_3 == -1)
				error_message("ERROR: could not fork to create a new process for pager.", strerror(errno), 0);
			else if (!pid_3){
				close_fd(pipe_fd1[0]);
				close_fd(pipe_fd1[1]);
				close_fd(pipe_fd2[1]);
				redirect(pipe_fd2[0], STDIN_FILENO);
				char *argv_new[2] = {};
				argv_new[0] = "./pager";

				if(execvp(argv_new[0], argv_new) < 0)
					error_message("ERROR: execvp() failed.", strerror(errno), argv_new[0]);
			}
			else{

			}
		}
	}
}
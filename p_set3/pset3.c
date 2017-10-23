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
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s. Name: %s\n", message, error_val, file_val);
	exit(-1);
}

void aesthetics(){
	char cwd[1024];
	if(!getcwd(cwd, 1024))
		error_message("ERROR: could not open directory", strerror(errno), cwd);
	fprintf(stderr, "%s$ ", basename(cwd));
}

int processing(char *input){
	//TODO
	return 0; 
}

int main(int argc, char **argv){
	
	FILE *input;
	int val; 
	char cin[1024]; //this gave a segfault when I did char * -> pointer 

	//see if we're taking stdin or file
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
		if(cin[0] != '#' && cin[0] != '\n') 
			//understand the data
			val += processing(cin);
			
		//just for aesthetics - happens after every line
		if(input == stdin)
			aesthetics(); 
	}

	return val; 

	//parse to tokens
	//do what the instructions say to do
	//implement cd 
	
}
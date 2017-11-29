#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "uthash/include/uthash.h"

int main(int argc, char **argv){
	
	char *line = NULL;
    size_t len = 0;

	if(argc < 2) exit(1); 

	while(getline()) // getline from stdin and then search through each word in the wordlist and if there's a match then print out

	FILE *wordlist = fopen(argv[1], "r"); 
	while(getline(&line, &len, wordlist)){
		int i = 0; 
		while(line[i]){
			i++; 
		}
		cout << line << endl; 
	}
}
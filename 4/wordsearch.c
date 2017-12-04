#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "uthash/include/uthash.h"

struct my_struct{
	char id[30];
	UT_hash_handle hh; 
};

int main(int argc, char **argv){
	
	struct my_struct *dict = NULL; 
	char *line = NULL;
    size_t len = 0;

	if(argc < 2) exit(1); 

	FILE *wordlist = fopen(argv[1], "r"); 
	int r = 0, a = 0; 

	while(getline(&line, &len, wordlist) != EOF){
		int i = 0; 
		struct my_struct *t; 
		int rejected = 0; 

		while(line[i]){
			line[i] = toupper(line[i]);
			if(!isalpha(line[i]) && line[i] != '\n'){
				rejected = 1; 
			}
			i++; 
		}

		if(rejected){
			r++; 
			continue; 
		}

		a++;
		HASH_FIND_STR(dict, line, t);
		if(!t){
			t = malloc(sizeof(struct my_struct));
			strncpy(t -> id, line, 30);
			HASH_ADD_STR(dict, id, t);
		}
	}

	fclose(wordlist);
	fprintf(stderr, "Accepted: %i and rejected: %i\n", a, r);

	char cin[30];
	int matched = 0; 

	while(fgets(cin, 1024, stdin) != NULL){ // getline from stdin and then search through each word in the wordlist and if there's a match then print out
		struct my_struct *t; 
		int i = 0; 

		while(cin[i]){
			cin[i] = toupper(cin[i]);
			i++;
		}

		HASH_FIND_STR(dict, cin, t);
		if(t){
			fprintf(stdout, "%s", cin);
			matched++; 
		}
	}
	fprintf(stderr, "Matched: %i Words\n", matched); 
}
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv){

	char *bit_val = NULL;
	char *out_file = NULL; 
	int c; 

	opterr = 0; 

	while((c = getopt(argc, argv, "bo:")) != -1)	
		switch(c){
			case 'b':
				bit_val = optarg; 
				break; 
			case 'o':
				out_file = optarg; 
				break; 	
			case '?':
				if (optopt == 'b' || optopt == 'o')
					fprintf(stderr, "Option -%c requires an argument", optopt);
			default: 
				abort(); 
		}
		printf("%s%s", bit_val, out_file);

}
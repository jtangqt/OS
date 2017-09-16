#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv){

	char *bit_val = 0;
	char *out_file = NULL; 
	char *in_file = NULL;
	int c; 

	opterr = 0; 

	while((c = getopt(argc, argv, "b:o:")) != -1)	
		switch(c){
			case 'b':
				bit_val = optarg; 
				//check if this a number
				// if atoi(bitval) != 1 then error message
					// if bitval is too large/too small, then error message
				break; 
			case 'o':
				out_file = optarg; 
				break; 	
			case '?':
				if (optopt == 'b' || optopt == 'o')
					fprintf(stderr, "Option -%c requires an argument", optopt);
				else
					fprintf(stderr, "%c", optopt);
					//in_file = optopt;
					//if in_file does not exist, error message
					return 1; 
			default: 
				abort(); 
		}

		printf("%s%s", bit_val, out_file);
		// if(out_file ! = NULL){
		// 	//print the input in the out_file
		// }
		// else{
		// 	//print the input in std. output
		// }

		// return 0; 
}

//the operation that was being attempted
//the object being operated on (the file name)
//other relevant parameters
//the error returned (strerror(errno))
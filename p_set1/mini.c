#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value:%s File Name:%s\n", message, error_val, file_val);
	exit(-1);
}

void file_manipulation(const char *in_file, const char *out_file, int fin, int fout, char * data, long * bit_val){
	char * read_data, write_data; 
}

int main(int argc, char **argv){

	long bit_val = -1;
	const char *out_file = NULL, *in_file = NULL;
	int oCount = 0, bCount = 0, fout = -1, fin = -1; 
	int c;

	while((c = getopt(argc, argv, "b:o:")) != -1)	
		switch(c){
			case 'b':
				//1. make sure that the -b isnt repeated
				if(bCount++ > 0 || !strcmp(optarg, "-b"))
					error_message("ERROR: multiple -b flags found!", 0, 0); 
				//2. make sure that there is a value behind b
				else if (!strcmp(optarg, "-") || !strcmp(optarg, "-o"))
					error_message("ERROR: no buffer size specified!", 0, 0);
				//3. check if optarg is a number that doesnt fit
				else if ((bit_val = strtol(optarg, NULL, 0)) <= 0)
					error_message("ERROR: invalid buffer size specified!", 0, 0);
				break; 
			case 'o':
				//1. make sure that -o isnt repeated
				if(oCount++ > 0 || !strcmp(optarg, "-o"))
					error_message("ERROR: multiple -o flags found!", 0, 0); 
				//2. make sure that there is a value behind o
				else if (!strcmp(optarg, "-"))
					error_message("ERROR: Invalid output file, - is used for standard input!", 0, 0);
				//3. make sure that there is a value behind o
				else if (!strcmp(optarg, "-b"))
					error_message("ERROR: no output file specified!", 0, 0);
				out_file = optarg; 
				break; 	
			default: 
				error_message("ERROR: Please follow this format\nminicat [-b ###] [-o outfile] infile1 [...infile2...]\nOR\nminicat [-b ###] [-o outfile] infile1 [...infile2...]\n", 0, 0); 
		}

	if(!out_file) {
		out_file = "stdout";
		fout = STDOUT_FILENO;
	}
	else if((fout = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
		error_message("ERROR: Could not open for writing!", strerror(errno), out_file);

	if(bit_val < 0) 
		bit_val = 4096; 
	char data[bit_val];

	for(; optind < argc; ++optind){
		if(!strcmp("-", argv[optind])){
			fin = STDIN_FILENO;
			in_file = "stdin";
		}
		else if((fin = open(argv[optind], O_RDONLY) < 0)){
			error_message("ERROR: Could not open for reading!", strerror(errno), in_file);
		}
		else 
			in_file = argv[optind];
	}

	if(fin < 0) {
		fin = STDIN_FILENO; 
		in_file = "stdin";
	}

	file_manipulation(in_file, out_file, fin, fout, data, &bit_val);

	if(fin == -1 && close(fin) < 0)
		error_message("ERROR: Could not close input file!", strerror(errno), in_file);
	else if(fout == -1 && close(fout) < 0)
		error_message("ERROR: Could not close output file!", strerror(errno), out_file);

	return 0; 
}

//the operation that was being attempted
//the object being operated on (the file name)
//other relevant parameters
//the error returned (strerroror(errno))
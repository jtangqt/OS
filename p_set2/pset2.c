#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>


char * create_dir() {
	char * dir; 
	return dir; 
}

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s. File Name: %s\n", message, error_val, file_val);
	exit(-1);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void search_file(int arg_no, const char *file_location){	//recursively search down the paths (go through the directory first, so argv[2])
	const char *location, *next_location; 
	//parse the string for the first location (to go to that location)
	if(next_location != NULL){
		if(arg_no == 1 && !is_regular_file(location))
			//check that this is a file, error message
		if(arg_no == 2 && is_regular_file(location))
			//check that it is a directory, error message
	}

	//here, the next location exists, therefore this location must not exist

	//check to make sure that we can go to the next location (the next location is a valid location)
	//if that checks out then print out the information
	//if this is not the end, search_file(int arg_no, string file_location-file_location - 1)
}

int main(int argc, char **argv){
	int c; 
	const char *open_file, *o_dir; 
	if(argc > 2){
		//TODO!! too many arguments
		error_message("ERROR! Too many input arguments. Please follow this format\nfinddir [direction to input file] [starting directory]\n", 0, 0); 
	}

	open_file = argv[1];

	if (argv[2] != NULL){
		o_dir = argv[2]; //original directory
		search_file(2, o_dir);
	} //TODO!! can we assume where we are now is a directory? 

	search_file(1, open_file);

}

#include <sys/stat.h> 
#include <string.h> 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>       
#include <sys/types.h>
#include <dirent.h>

void search_tree(const char*);


//take all elements in the directory and check if they are identical to the file given
//if they are identical, then print out all of its information 
//pathname, linkcount > 1 (hardlink, symlink, distinct -> nlink), and read by users? 
int main(int argc, char **argv){
	struct stat file_st; 
	lstat(argv[1], &file_st);
	search_tree(argv[1], &file_st, argv[2]);
	
}



void search_tree(const char * file_path, struct stat *file_st, const char *start_path){
	
	// going through each layer
	
	// if file
	
	
	//if directory
	DIR *dir;
	struct dirent *de; 
	dir = opendir(start_path); 
	struct stat st; 
	while(de = readdir(dir)){
		char *next_path = (char *)malloc(strlen(start_path) + strlen(de -> d_name) + 2); 
		sprintf(next_path, "%s/%s", start_path, de -> d_name); 
		lstat(next_path, &st);
		if((st.st_mode & S_IFMT) != S_IFDIR && (st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK)
			continue; 
		if((st.st_mode & S_IFMT) == S_IFDIR)
			search_tree(file_path, next_path); 
		else if((st.st_mode & S_IFMT) == S_IFREG){
			//TODO!! MEMCPY SHIT
			if(st.st_size != file_st -> st_size)
				continue; 
			//compare actual contents
			file1 = fopen(file_path);
			file2 = fopen(next_path); 
			char a, b; 
			do {
				a = fgetc(file1);
				b = fgetc(file2);
				if (a != b)
					break;
			} while(a != EOF && b != EOF)
			if (a != b)
				continue; 
		}
		else
			//TODO!! SYMLINK 
		
		fprintf(stderr, "%s\n", de -> d_name); 
	}
}



#include <sys/stat.h> 
#include <string.h> 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>       
#include <sys/types.h>
#include <dirent.h>

void search_tree(const char*);


//take all elements in the directory and check if they are identical to the file given
//if they are identical, then print out all of its information 
//pathname, linkcount > 1 (hardlink, symlink, distinct -> nlink), and read by users? 
int main(int argc, char **argv){
	struct stat file_st; 
	lstat(argv[1], &file_st);
	search_tree(argv[1], &file_st, argv[2]);
	
}



void search_tree(const char * file_path, struct stat *file_st, const char *start_path){
	
	// going through each layer
	
	// if file
	
	
	//if directory
	DIR *dir;
	struct dirent *de; 
	dir = opendir(start_path); 
	struct stat st; 
	while(de = readdir(dir)){
		char *next_path = (char *)malloc(strlen(start_path) + strlen(de -> d_name) + 2); 
		sprintf(next_path, "%s/%s", start_path, de -> d_name); 
		lstat(next_path, &st);
		if((st.st_mode & S_IFMT) != S_IFDIR && (st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK)
			continue; 
		if((st.st_mode & S_IFMT) == S_IFDIR)
			search_tree(file_path, next_path); 
		else if((st.st_mode & S_IFMT) == S_IFREG){
			//TODO!! MEMCPY SHIT
			if(st.st_size != file_st -> st_size)
				continue; 
			//compare actual contents
			file1 = fopen(file_path);
			file2 = fopen(next_path); 
			char a, b; 
			do {
				a = fgetc(file1);
				b = fgetc(file2);
				if (a != b)
					break;
			} while(a != EOF && b != EOF)
			if (a != b)
				continue; 
		}
		else
			//TODO!! SYMLINK 
		
		fprintf(stderr, "%s\n", de -> d_name); 
	}
}
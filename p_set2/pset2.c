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

void search_tree(const char*, const char*);

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s. File Name: %s\n", message, error_val, file_val);
	exit(-1);
}

void message(int n_link, char *link, char *read, char *duplicate){
	if(!n_link){
		if(!duplicate)
			fprintf(stderr, "%s to target", link); 
		else  
			fprintf(stderr, "%s to duplicate at %s", link, duplicate); 
	}
	else{
		fprintf(stderr, "duplicate to target"); 
		if(n_link > 1)	
			fprintf(stderr, " (n_link = %i)", n_link); 
	}
	if(!read)	
		fprintf(stderr, ", %s to read by other\n", read); 

}

//take all elements in the directory and check if they are identical to the file given
//if they are identical, then print out all of its information 
//pathname, linkcount > 1 (hardlink, symlink, distinct -> nlink), and read by users? 
int main(int argc, char **argv){
	int c; 

	if(argc > 2){
		error_message("ERROR! Too many input arguments. Please follow this format\nfinddir [direction to input file] [starting directory]\n", 0, 0); 
	}

	search_tree(argv[1], argv[2]);
}

void search_tree(const char * file_path, const char *start_path){
	struct stat file_st; 
	DIR *dir;
	struct dirent *de; 
	struct stat st;
	char a, b;
	FILE *file1, *file2; 
	lstat(file_path, &file_st);
	
	// going through each layer	
	dir = opendir(start_path); 
	while(de = readdir(dir)){
		char *next_path = (char *)malloc(strlen(start_path) + strlen(de -> d_name) + 2); 
		sprintf(next_path, "%s/%s", start_path, de -> d_name); 
		lstat(next_path, &st);
		if((st.st_mode & S_IFMT) != S_IFDIR && (st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK)
			continue; 
		//if directory
		if((st.st_mode & S_IFMT) == S_IFDIR)
			search_tree(file_path, next_path);
		//if regular file 
		else if((st.st_mode & S_IFMT) == S_IFREG){
			if(st.st_size != file_st.st_size) //check to see if the sizes match
				continue; 
			//compare actual contents
			file1 = fopen(file_path, "r");
			file2 = fopen(next_path, "r"); 
			do {
				a = fgetc(file1);
				b = fgetc(file2);
				if (a != b)
					break;
			} while(a != EOF && b != EOF); 
			if (a != b)
				continue;
			fprintf(stderr, "%s ", next_path); 
			int n_link = 0;
			char *read = NULL, *duplicate = NULL;  
			//TODO!! change the info for message
			//the pathname of the matching file; if the target count has a link count > 1 and hard link; otherwise distinct inode, output nlink value of matching file; can it be read by others
			message(n_link, "hard link", read, duplicate);
			fclose(file1);
			fclose(file2); 
		}
		else{
			char *duplicate_sym = NULL; 
			//TODO!! change the info for symlink
			message(0, "sym link", 0, duplicate_sym); 
		}
	}
}

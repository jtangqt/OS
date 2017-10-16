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

void search_tree(char*, char*);

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s. Name: %s\n", message, error_val, file_val);
	exit(-1);
}

void message(char *next_path, int n_link, char *link, char *read, char *duplicate){
	if(!n_link){
		if(!link)
			fprintf(stderr, "%s\tduplicate to target", next_path); 
		else  
			fprintf(stderr, "%s\t%s to duplicate at %s", next_path, link, duplicate); 
	}
	else{
		fprintf(stderr, "%s\t%s to target", next_path, link); 
		if(n_link > 1)	
			fprintf(stderr, " (n_link = %i)", n_link); 
	}
	if(read)	
		fprintf(stderr, ", %s to read by other\n", read); 
	else
		fprintf(stderr, "\n");
}

//take all elements in the directory and check if they are identical to the file given
//if they are identical, then print out all of its information 
//pathname, linkcount > 1 (hardlink, symlink, distinct -> nlink), and read by users? 
int main(int argc, char **argv){
	int c; 	
	char cwd[1024];

	if(argc > 3){
		error_message("ERROR! Too many input arguments. Please follow this format\nfinddir [direction to input file] [starting directory]\n", 0, 0); 
	}

	search_tree(argv[1], argv[2]);

}

void search_tree(char * file_path, char *s_path){
	struct stat file_st, compare; 
	DIR *dir;
	struct dirent *de; 
	struct stat st;
	char a, b;
	FILE *file1, *file2; 
	lstat(file_path, &file_st);

	// going through each layer	
	if(!(dir = opendir(s_path))) //check for error in opening a directory
		error_message("ERROR: Could not open.", strerror(errno), s_path);
	while(de = readdir(dir)){
		char * buf; 
		int n_link = 0;

		if(!strcmp(de -> d_name, "..") || !strcmp(de -> d_name, "."))
			continue;	

		char *next_path = (char *)malloc(strlen(s_path) + strlen(de -> d_name) + 2); 
		sprintf(next_path, "%s/%s", s_path, de -> d_name); 
		lstat(next_path, &st);

		if((st.st_mode & S_IFMT) != S_IFDIR && (st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK)
			continue; 
		
		if((st.st_mode & S_IFMT) == S_IFDIR) //if directory
			search_tree(file_path, next_path);
		
		else if((st.st_mode & S_IFMT) == S_IFREG){ //if regular file 
			char *duplicate = "duplicate", *read = NULL, *link = NULL;
			
			if(st.st_mode & S_IROTH) //if it's readable by others
				read = "able";
			else
				read = "not able";
			
			//case 1: inodes match
			if(st.st_ino == file_st.st_ino){
				if(st.st_nlink == 1){
					link = "original";
					n_link = st.st_nlink;
				}
				else if (st.st_nlink > 1){
					link = "hard link";
					n_link = st.st_nlink; 
				}
				continue; 
			}

			//case 2: inodes don't match
			if(st.st_size != file_st.st_size)
				continue;
			
			file1 = fopen(file_path, "r"); //now compare actual contents
			file2 = fopen(next_path, "r");			
			do {
				a = fgetc(file1);
				b = fgetc(file2);
				if (a != b)
					break;
			} while(a != EOF && b != EOF); 
			if (a != b)
				continue;	
			
			message(next_path, n_link, link, read, duplicate);
			fclose(file1);
			fclose(file2); 
		}
		
		else{ //if symlink
			char *duplicate = NULL; 
		
			if(file_st.st_ino == st.st_ino)
				message(next_path, 0, "sym link", 0, duplicate); 

			else{
				if(st.st_size != file_st.st_size)
					continue;

				file1 = fopen(file_path, "r"); //now compare actual contents
				file2 = fopen(next_path, "r");			
				do {
					a = fgetc(file1);
					b = fgetc(file2);
					if (a != b)
						break;
				} while(a != EOF && b != EOF); 
				if (a != b)
					continue;	
			
				fclose(file1);
				fclose(file2); 

				buf = (char*)malloc(st.st_size+1); 
				readlink(next_path, buf, sizeof(buf)+1);
			}

			message(next_path, 0, "sym link", 0, buf); 
			
			free(buf); 
		}
	}
	closedir(dir);
}

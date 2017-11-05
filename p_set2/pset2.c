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

void search_tree(char *, char*, char*);

void error_message(const char *message, const char *error_val, const char *file_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s. Name: %s\n", message, error_val, file_val);
	exit(1); 
}

//take all elements in the directory and check if they are identical to the file given
//if they are identical, then print out all of its information 
//pathname, linkcount > 1 (hardlink, symlink, distinct -> nlink), and read by users? 
int main(int argc, char **argv){
	int c; 	
	char cwd[1024], o_path[1024]; 

	if(argc > 3){
		error_message("ERROR! Too many input arguments. Please follow this format\n./hunt [direction to input file] [starting directory]", 0, 0); 
	}
	else if (argc < 2){
		error_message("ERROR! Not enough input arguments. Please follow this format\n./hunt [direction to input file] [starting directory]", 0, 0); 
	}

	getcwd(o_path, sizeof(o_path));
	char *file_path = (char *)malloc(strlen(argv[1]) + strlen(o_path) + 2);
	sprintf(file_path, "%s/%s", o_path, argv[1]);
	chdir(argv[2]);
	getcwd(cwd, sizeof(cwd));
	search_tree(o_path, file_path, cwd);

}

void search_tree(char * o_path, char * file_path, char *s_path){
	struct stat file_st, buf_st; 
	DIR *dir;
	struct dirent *de; 
	struct stat st;
	char a, b;
	FILE *file1, *file2; 
	lstat(file_path, &file_st);

	if(lstat(file_path, &file_st) == -1){
		error_message("ERROR: Could not lstat.", strerror(errno), file_path); 
	}

	// going through each layer	
	if(!(dir = opendir(s_path))) //check for error in opening a directory
		error_message("ERROR: Could not open.", strerror(errno), s_path);	
	
	while(de = readdir(dir)){
		char * buf; 
		int n_link = 0;

		char *next_path = (char *)malloc(strlen(s_path) + strlen(de -> d_name) + 2); 
		sprintf(next_path, "%s/%s", s_path, de -> d_name); 

		if(!strcmp(de -> d_name, "..") || !strcmp(de -> d_name, "."))
			continue;	

		if(lstat(next_path, &st) == -1){
			error_message("ERROR: Could not lstat.", strerror(errno), next_path);
			continue; 
		}

		if((st.st_mode & S_IFMT) != S_IFDIR && (st.st_mode & S_IFMT) != S_IFREG && (st.st_mode & S_IFMT) != S_IFLNK)
			continue; 
		
		if((st.st_mode & S_IFMT) == S_IFDIR) //if directory		
			search_tree(o_path, file_path, next_path);

		else if((st.st_mode & S_IFMT) == S_IFREG){ //if regular file 
			char *read = NULL; 
			
			if(st.st_mode & S_IROTH) //if it's readable by others
				read = "able";
			else
				read = "not able";
			
			//case 1: inodes match
			if(st.st_ino == file_st.st_ino){
				fprintf(stderr, "%s\thardlink to target found, %s to read by others\n", next_path, read);
				continue; 
			}

			//case 2: inodes don't match - compare size of file first 
			if(st.st_size != file_st.st_size)
				continue;

			//now compare actual contents
			file1 = fopen(file_path, "r"); 
			file2 = fopen(next_path, "r");			
			do {
				a = fgetc(file1);
				b = fgetc(file2);
				if (a != b){
					fclose(file1);
					fclose(file2); 
					break;
				}
			} while(a != EOF && b != EOF); 
			if (a != b){
				fclose(file1);
				fclose(file2); 
				continue;	
			}

			fprintf(stderr, "%s\tduplicate to target found (n_link = %lu), %s to read by others\n", next_path, st.st_nlink, read);
			
			fclose(file1);
			fclose(file2); 
		}
		
		else{ //if symlink
			char *duplicate = NULL; 

			buf = (char*)malloc(st.st_size+1); 
			readlink(next_path, buf, st.st_size+1);
			buf[st.st_size] = '\0';
			char *buf_path = (char *)malloc(strlen(buf) + strlen(o_path) + 2);
			sprintf(buf_path, "%s/%s", o_path, buf);

			if(!strcmp(buf_path, file_path)){
				fprintf(stderr, "%s\tsymlink to target found\n", next_path);
			}
			else{
				//compare size of file
				lstat(buf_path, &buf_st);

				if(buf_st.st_size != file_st.st_size)
					continue;

				//now compare actual contents	
				file1 = fopen(file_path, "r"); 
				file2 = fopen(buf_path, "r");			
				do {
					a = fgetc(file1);
					b = fgetc(file2);
					if (a != b){
						fclose(file1);
						fclose(file2); 
						break;
					}
				} while(a != EOF && b != EOF); 
				if (a != b){
					fclose(file1);
					fclose(file2); 
					continue;	
				}

				fclose(file1);
				fclose(file2); 

				fprintf(stderr, "%s\tsymlink to duplicate found (%s)\n", next_path, buf);

			}
			
			free(buf); 

		}
		free(next_path);
	}
	closedir(dir);
}
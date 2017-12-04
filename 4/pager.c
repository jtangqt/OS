#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

void press_q(const char *line, int f_in, char *buffer){
	if(!strcmp(line, "q") || !strcmp(line, "Q")){
		fprintf(stdout, "\n\n		*** Pager terminated by Q command ***\n");
		free(buffer);
		if(f_in)
			close(f_in);
		exit(0);
	}
}

int main(){

	char *line = NULL;
    size_t len = 0;
    int f_in = 0, cflag = 1, count = 0, buffersize = 100, r_in = 0, val_get;
    char *buffer = (char *)calloc(buffersize,sizeof(char*));

	if(!(f_in = open("/dev/tty", O_RDWR, S_IRUSR | S_IWUSR))){
		//error message
		fprintf(stderr, "ERROR: Could not stat /dev/tty. Error value: %s\n", strerror(errno));
		exit(-1);
	}
	for(;;){
		while(cflag){
			val_get = getline(&line, &len, stdin);
			if(val_get == EOF){
				free(buffer);
				if(f_in)
					close(f_in);
				return 0; 
			}
			else if(val_get != -1){
				//press_q(line, f_in, buffer);
				fprintf(stdout, "%s", line);
				count++; 
				cflag = count % 24; //page size + 1 
			}
			else{
				//error message
				fprintf(stderr, "ERROR: Getline returned error. Error value: %s\n", strerror(errno));
				exit(-1);
			}
		}
		if(write(f_in, "\n\n\t\t---Press RETURN for more--- \n\n", 34) == -1){
			//error message
			fprintf(stderr, "ERROR: Could not write to /dev/tty. Error value: %s\n", strerror(errno));
			exit(-1);
		}
		if((r_in = read(f_in, buffer, buffersize)) == -1){
			//error message
			fprintf(stderr, "ERROR: Could not read in /dev/tty. Error value: %s\n", strerror(errno));
			exit(-1);
		}
		
		if(!strcmp(buffer, "\r") || !strcmp(buffer, "\n"))
			cflag = (++count) % 24;
		
		buffer[1] = 0;
		press_q(buffer, f_in, buffer);

	}

	return 0; 

}
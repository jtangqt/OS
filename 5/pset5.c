#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <time.h> 
#include <signal.h>

void sig_handler(int sig){
	fprintf(stderr, "Signal %s received!\n", strsignal(sig));
	exit(sig);
}

void error_message(const char *message, const char *error_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s.\n", message, error_val);
}

int create_fd(int length){
	int fd; 
	char buf[1];


	if((fd = open("file.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0){
		error_message("ERROR: could not open properly", strerror(errno));
		exit(255);
	}
	srand(time(NULL));

	for(int i = 0; i < length; i++){
		if((rand() % 36) <= 10){
			sprintf(buf, "%i", rand()%10);
			if(write(fd, buf, 1) < 0){
				error_message("ERROR: could not write properly", strerror(errno));
				exit(255);
			}
		}
		else{
			buf[0] = rand()%26 + 'A'; 
			if(write(fd, buf, 1) < 0){
				error_message("ERROR: could not write properly", strerror(errno));
				exit(255);
			}	
		}
	}

	return fd; 
}

void map_close_error(int length, int fd, char * map){
	if(munmap(map, length) < 0){
		close(fd);
		error_message("ERROR: could not munmap file from memory.", strerror(errno));
		exit(255);
	}
	if(close(fd) < 0){
		error_message("ERROR: could not close file descriptor.", strerror(errno));
		exit(255);
	}
}

int test_1(){
	char *map;
	size_t length = 4096; 

	printf("Executing Test #1 (write to r/o mmap): \n");
	int fd = create_fd(length);	

	if((map = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	printf("map[3] = \'%c\'\nwriting a \'%c\'\n", map[3], map[4]);
	map[3] = map[4];
	printf("After write, map[3] returned \'%c\'\n", map[3]);
	map_close_error(length, fd, map);

	exit(0);
}

int test_23(int flag){

	char *map;
	size_t length = 4096; 
	char buf[1];
	char *out_flag;
	int private, offset = 6;  

	if(flag == MAP_SHARED){
		printf("Executing Test #2 (writing to MAP_SHARED): \n");
		out_flag = "MAP_SHARED";
	}
	else if(flag == MAP_PRIVATE){
		printf("Executing Test #3 (writing to MAP_PRIVATE): \n");
		out_flag = "MAP_PRIVATE";
	}
	else
		error_message("ERROR: invalid input for 2 or 3.", strerror(errno));

	int fd = create_fd(length);
	if((map = mmap(NULL, length, PROT_WRITE, flag, fd, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	printf("map[%d] = \'%c\'\nwriting a new value: J\n", offset, map[offset]);
	map[offset] = 'J';

	if(lseek(fd, offset, SEEK_SET) < 0)
		error_message("ERROR: could not lseek properly", strerror(errno));
	if(read(fd, buf, 1) < 0)
		error_message("ERROR: could not read properly", strerror(errno));

	if(buf[0] == 'J')
		private = 0;
	else
		private = 1;

	printf("Updated map[%d] with \'%c\' for %s. Map %s private.\n", offset, map[offset], out_flag, private ? "is": "is not");

	map_close_error(length, fd, map);

	private ? exit(0) : exit(1);

}

int test_4(){
	char * map;
	size_t length = 6009;
	struct stat sb; 

	printf("Executing Test #4 (writing beyond the edge): \n");

	int fd = create_fd(length);
	
	if(fstat(fd, &sb) < 0){
		error_message("ERROR: could not fstat properly.", strerror(errno));
		exit(255);
	}
	
	int size_1 = sb.st_size; 
	
	if((map = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	printf("map[%d]'s original size is %d, writing a new value at the next byte: D\n", map[length-1], size_1);
	map[length] = 'D';

	if(fstat(fd, &sb) < 0){
		error_message("ERROR: could not fstat properly.", strerror(errno));
		exit(255);
	}

	int size_2 = sb.st_size;
	printf("map's new size after writing a new value is %d, the value at map[%zu] is %d\n", size_2, length, map[length]);

	if(size_1 == size_2){
		map_close_error(length, fd, map);
		printf("The sizes are the same. \n");
		exit(1);
	}
	else{
		map_close_error(length, fd, map);
		printf("The sizes are different.\n");
		exit(0);
	}

}

int test_5(){

	char *map;
	size_t length = 6009;
	char buf[1];
	struct stat sb; 
	int fd, offset;

	fd = create_fd(length);

	printf("Executing Test #5 (writing into a hole): \n");

	if((map = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	map[length] = 'X';
	buf[0] = 'A';

	printf("map's original size is %zu, writing a new value one byte beyond the last byte: X\n", length);

	if(lseek(fd, length + 16, SEEK_SET) < 0){
		error_message("ERROR: could not lseek properly", strerror(errno));
		exit(255);
	}
	if(write(fd, buf, 1) < 0){
		error_message("ERROR: could not write properly", strerror(errno));
		exit(255);
	}
	if(lseek(fd, length, SEEK_SET) < 0){
		error_message("ERROR: could not lseek properly", strerror(errno));
		exit(255);
	}
	if(read(fd, buf, 1) < 0){
		error_message("ERROR: could not read properly", strerror(errno));
		exit(255);
	}

	printf("Read from \'one byte beyond last byte\': %c\n", buf[0]);

	if(buf[0] == 'X'){
		map_close_error(length, fd, map);
		printf("Writing into hole is visible. \n");
		exit(0);
	}else{
		map_close_error(length, fd, map);
		printf("Writing to hole is not visible. \n");
		exit(1);
	}

}

int test_6(){
	int fd, length = 2000;
	char *map; 

	printf("Executing Test #6 (reading beyond the edges): \n");

	if((map = mmap(NULL, 8192, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	printf("map[3000]: \'%d\'\n", map[3000]);
	printf("map[6000]: \'%d\'\n", map[6000]);

	map_close_error(length, fd, map);

	exit(0);

}

int main(int argc, char **argv){

	for(int i = 0; i < 32; i++){
		signal(i, sig_handler);
	}

	if(argc > 2 || argc == 1){
		error_message("ERROR: not enough input arguments/too many input arguments", 0);
		return 1; 
	}
	else{
		switch(atoi(argv[1])){
			case 1:
				test_1();
				break; 
			case 2:
				test_23(MAP_SHARED);
				break;
			case 3:
				test_23(MAP_PRIVATE);
				break; 
			case 4:
				test_4();
				break; 
			case 5:
				test_5();
				break;
			case 6:
				test_6(); 
				break; 
			default:
				error_message("ERROR: unidentified input test value, choose a value from 1-6.", strerror(errno));
				return 1; 
		}
	}
}
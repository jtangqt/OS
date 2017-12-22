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

int create_fd(int length){
	int fd; 
	char buf[1];


	if((fd = open("file.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0){
		fprintf(stderr, "ERROR MESSAGE: ");
		exit(255);
	}
	srand(time(NULL));

	for(int i = 0; i < length; i++){
		if((rand() % 36) <= 10){
			sprintf(buf, "%i", rand()%10);
			if(write(fd, buf, 1) < 0){
				fprintf(stderr, "ERROR MESSAGE: ");
				exit(255);
			}
		}
		else{
			buf[0] = rand()%26 + 'A'; 
			if(write(fd, buf, 1) < 0){
				fprintf(stderr, "ERROR MESSAGE: ");
				exit(255);
			}	
		}
	}

	return fd; 
}

void map_close_error(int length, int fd, char * map){
	if(munmap(map, length) < 0){
		close(fd);
		fprintf(stderr, "ERROR MESSAGE could not munmap, %s\n", strerror(errno));
		exit(255);
	}
	if(close(fd) < 0){
		fprintf(stderr, "ERROR MESSAGE could not close, %s\n", strerror(errno));
		exit(255);
	}
}

int test_1(){
	char *map;
	size_t length = 4096; 

	printf("Executing Test #1 (write to r/o mmap): \n");
	int fd = create_fd(length);	

	if((map = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
		fprintf(stderr, "ERROR MESSAGE  could not map, %s\n", strerror(errno));
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
		fprintf(stderr, "Invalid command for test 2 or 3");

	int fd = create_fd(length);
		if((map = mmap(NULL, length, PROT_WRITE, flag, fd, 0)) == MAP_FAILED){
		fprintf(stderr, "ERROR MESSAGE  could not map, %s\n", strerror(errno));
		exit(255);
	}

	printf("map[%d] = \'%c\'\nwriting a new value: J\n", offset, map[offset]);
	map[offset] = 'J';

	if(lseek(fd, offset, SEEK_SET) < 0)
		fprintf(stderr, "ERROR MESSAGE LSEEK:");
	if(read(fd, buf, 1) < 0)
		fprintf(stderr, "ERROR MESSAGE READ: ");

	if(buf[0] == 'J')
		private = 1;
	else
		private = 0;

	printf("Updated map[%d] with \'%c\' for %s. Map %s private.\n", offset, map[offset], out_flag, private ? "is not": "is");

	map_close_error(length, fd, map);

	private ? exit(1) : exit(0);

}

int test_4(){
	printf("Executing Test #4 (writing beyond the edge): ");
}

int test_5(){
	printf("Executing Test #5 (writing into a hole): ");
}

int test_6(){
	printf("Executing Test #6 (reading beyond the edges): ");
}

int main(int argc, char **argv){

	for(int i = 0; i < 32; i++){
		signal(i, sig_handler);
	}

	if(argc > 2 || argc == 1){
		fprintf(stderr, "ERROR MESSAGE");
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
				fprintf(stderr, "ERROR MESSAGE");
				return 1; 
		}
	}
}
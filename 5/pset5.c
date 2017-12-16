#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int test1(){}
int test2(){}
int test3(){}
int test4(){}
int test5(){}
int test6(){}

int main(int argc, char **argv){
	if(argc > 2)
		fprintf(stderr, "ERROR MESSAGE");
	else
		switch(atoi(argv[1])){
			case 1:
				test1();
			case 2:
				test2();
			case 3:
				test3();
			case 4:
				test4();
			case 5:
				test5();
			case 6:
				test6(); 
			default:
				fprintf(stderr, "ERROR MESSAGE");
		}
}
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

void emptyfunction(){;}

void shortfunction(){
	int c = getpid(); 
}

int main(int argc, char **argv){

	uint64_t diff; 
	struct timespec start, end;
	uint64_t start_time = clock_gettime(CLOCK_MONOTONIC, &start);

	for(int i = 0; i < 1000000000; i++){

		//part 1
		//part 2
		//emptyfunction();
		//part 3
		shortfunction(); 
	}

	uint64_t end_time = clock_gettime(CLOCK_MONOTONIC, &end);
	diff = 10000000000*(end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec; 
	fprintf(stderr, "Time: %llu nanoseconds.", (long long unsigned int) diff); 
	return 0; 

}

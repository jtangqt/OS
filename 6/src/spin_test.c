#include "spinlock.h"

int main(int argc, char **argv){
	
	int *map; 

	if(argc != 3)
		fprintf(stderr, "ERROR MESSAGE; non proper usage");

	long long int c_num = atoll(argv[1]);
	long long int iter_num = atoll(argv[2]);

	spinlock * lock;
	
	if((map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	map[0] = 0; 
	lock = (spinlock*)(map + sizeof(spinlock));
	lock -> p_lock = map[1];
	pid_t pid[c_num];

	for(int i = 0; i < c_num; i++){
		if((pid[i] = fork()) < 0){
			fprintf(stderr, "ERROR MESSAGE");
			return 255; 
		}
		if(!pid[i]){
			spin_lock(lock);
			for(int j = 0; j < iter_num; j++)
				map[0]++;
			spin_unlock(lock);
			exit(0);
		}
	}
	for(int i = 0; i < c_num; i++)
		wait(0);
	fprintf(stderr, "%d\n", map[0]);

}
#include "fifo.h"


int cv_test(int n_child){
	
	spinlock mutex;
	cv* tmp;
	int pid;

	if((tmp = (cv *)mmap(NULL, sizeof(cv), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}
	
	cv_init(tmp);

	for(int i = 0; i < n_child; i++){
		if((pid = fork()) < 0) {
			fprintf(stderr, "ERORR MESSAGE");
			return 255;
		}
		if(!pid){
			cv_wait(tmp, &mutex);
			exit(0);
		}
	}

	sleep(2);
	int up_count = cv_broadcast(tmp);
	
	for (int i = 0; i < n_child; i++)
		wait(0);
	
	fprintf(stderr, "children awoken = %d\n", up_count);
	
	return 0;
}

int main(int argc, char **argv){
	/*
	if(argc != 2){
		fprintf(stderr, "ERROR MESSAGE");
	}
	cv_test(atoi(argv[1])); 
	*/
	if(argc != 3)
		fprintf(stderr, "ERROR MESSAGE");

	fifo *tmp; 
	if((tmp = (fifo*)mmap(NULL, sizeof(fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	fifo_init(tmp);
	int n_writers = atoi(argv[1]);
	int n_items = atoi(argv[2]);
	pid_t pid[n_writers];

	fprintf(stderr, "Beginning test with %d writers, %d each\n", n_writers, n_items);

	for(int i = 0; i < n_writers; i++){
		//fprintf(stderr, "before all forks");
		if((pid[i] = fork() < 0)){
			fprintf(stderr, "ERROR MESSAGE");
			return 255; 
		}

		if(!pid[i]){
			unsigned long buf[n_items];
			for(int j = 0; j < n_items; j++){
				//fprintf(stderr, "%d %d\n", i, j);
				//if(c = getpid() )
				buf[j] = j + getpid()*10000;
				fifo_wr(tmp, buf[j]);
				//fprintf(stderr, "writer %d wrote %d and %lu\n", i, j, buf[j]);
			}
			fprintf(stderr, "Writer %d completed\n", n_writers);
			exit(EXIT_SUCCESS);
		}
		fprintf(stderr, "IM HERE 4");
	}

	fprintf(stderr, "in lonely reader");

	int lonelyreader = fork(); 
	if(lonelyreader < 0){
		fprintf(stderr, "ERROR MESSAGE");
		return 255; 
	}
	if(!lonelyreader){
		unsigned r_buf[n_writers*n_items];
		for(int i = 0; i < n_writers*n_items; i++){
			r_buf[i] = fifo_rd(tmp);
			fprintf(stderr, "%d\t", r_buf[i]);
		}
		//fprintf(stderr, "Reader stream %d completed", r_buf[i]);
		exit(0);
	}
	for(int i = 0; i < n_writers + 1; i ++){
		wait(0);
	}
	fprintf(stderr, "byebye\n");
	return 0; 
}
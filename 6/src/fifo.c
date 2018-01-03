#include "fifo.h"

void fifo_init(struct fifo *f){
	int *tmp1, *tmp2;

	f -> next_read = 0; 
	f -> next_write = 0; 	
	f -> item_count = 0; 
	f -> mutex.p_lock = 0; 
	
	if((tmp1 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED || (tmp2 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	f -> full = *(cv *)tmp1;
	f -> empty = *(cv *)tmp2; 
	cv_init(&f -> full);
	cv_init(&f -> empty);

}

void fifo_wr(struct fifo *f, unsigned long d){

	spin_lock(&f -> mutex);
	
	//fprintf(stderr, "HERE");

	while(f -> item_count >= BSIZE)
		cv_wait(&f -> full, &f -> mutex);

	/*if(!(f -> item_count % 1024))
		fprintf(stderr, "%d ", f -> item_count);
*/
	fprintf(stderr, "IM HERE %d", f->item_count%1024);

	f -> buf[f -> next_write++] = d;
	f -> next_write %= BSIZE;
	f -> item_count++; 
	//fprintf(stderr, "IM HERE 2");

	cv_signal(&f -> empty); 
	//fprintf(stderr, "IM HERE 3");
	spin_unlock(&f -> mutex);

}

unsigned long fifo_rd(struct fifo *f){
	unsigned long tmp;
	spin_lock(&f -> mutex);
	while(f -> item_count < 1)
		cv_wait(&f -> empty, &f -> mutex);
	tmp = f -> buf[f -> next_read++];
	f -> next_read %= BSIZE;
	f -> item_count--;
}
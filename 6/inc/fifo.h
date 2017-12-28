#ifndef FIFO_H
#define FIFO_H


#include "cv.h"

#define BSIZE 1024

typedef struct fifo{
	unsigned long buf[BSIZE];
	int next_write, next_read;
	int item_count; 
	cv full, empty; 
	spinlock mutex;
} fifo; 

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
/* Initialize the shared memory FIFO *f including any required underlying
*initializations (such as calling cv_init). The FIFO will have a static
*fifo length of MYFIFO_BUFSIZ elements. #define this in fifo.h.
*A value of 1K is reasonable.
*/

void fifo_wr(struct fifo *f, unsigned long d){

	spin_lock(&f -> mutex);
	
	fprintf(stderr, "HERE");

	while(f -> item_count >= BSIZE)
		cv_wait(&f -> full, &f -> mutex);

	fprintf(stderr, "%d", f -> item_count);

	//fprintf(stderr, "IM HERE");

	f -> buf[f -> next_write++] = d;
	f -> next_write %= BSIZE;
	f -> item_count++; 
	//fprintf(stderr, "IM HERE 2");

	cv_signal(&f -> empty); 
	//	fprintf(stderr, "IM HERE 3");
	spin_unlock(&f -> mutex);

}
/* Enqueue the data word d into the FIFO, blocking unless and until the
*FIFO has room to accept it. (i.e. block until !full)
*/

unsigned long fifo_rd(struct fifo *f){
	unsigned long tmp;
	spin_lock(&f -> mutex);
	while(f -> item_count < 1)
		cv_wait(&f -> empty, &f -> mutex);
	tmp = f -> buf[f -> next_read++];
	f -> next_read %= BSIZE;
	f -> item_count--;
}
/* Dequeue the next data word from the FIFO and return it. Block unless
*and until there are available words. (i.e. block until !empty)
*/


#endif
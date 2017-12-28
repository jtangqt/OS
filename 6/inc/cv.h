#ifndef CV_H
#define CV_H

#include "spinlock.h"
#define CV_MAXPROC 64

typedef struct cv{
	int count; 
	pid_t pid[CV_MAXPROC];
	spinlock lock;
	sigset_t sigmask; 
} cv; 

void sig_handler(int sig){;}

void cv_init(struct cv *cv){
	
	int * map;

	if((map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED){
		error_message("ERROR: could not mmap file from memory.", strerror(errno));
		exit(255);
	}

	cv -> lock = *(spinlock*)(map + sizeof(spinlock)); 
	cv -> count = 0; 
	for(int i = 0; i < CV_MAXPROC; i++)
		cv -> pid[i] = 0; 

	signal(SIGUSR1, sig_handler);
	sigfillset(&cv -> sigmask);
	sigdelset(&cv -> sigmask, SIGUSR1);

}
/* Initialize any internal data structures in cv so that it is ready for
*use. The initial condition is that nobody is waiting for this cv.
*You can probably arrange your struct cv so that all-0 bytes is
*the initialization condition.
*/

void cv_wait(struct cv *cv, struct spinlock *mutex){
	if(cv -> count >= CV_MAXPROC){
		fprintf(stderr, "ERROR MESSAGE");
		exit(255);
	}

	fprintf(stderr, " HERE1 ");

	spin_lock(&cv -> lock);
	cv -> pid[cv -> count] = getpid();
	cv -> count++; 
	spin_unlock(&cv -> lock);
	spin_unlock(mutex);

	fprintf(stderr, "HERE2 ");

	sigprocmask(SIG_BLOCK, &cv -> sigmask, NULL);
	sigsuspend(&cv -> sigmask);

	fprintf(stderr, "HERE3 ");

	if(cv -> count > 0){
		spin_lock(&cv -> lock);
		cv -> count --; 
		cv -> pid[cv -> count] = 0; 
		spin_unlock(&cv -> lock);
		spin_lock(mutex);
		return; 
	}

	fprintf(stderr, "HERE4 ");

	sigprocmask(SIG_UNBLOCK, &cv -> sigmask, NULL);
	spin_lock(mutex);

}
/* This will be called with the spinlock mutex held by the caller (otherwise
*results will be undefined). Atomically record within the internals
* of cv that the caller is going to sleep, release the mutex, and
* go to sleep (see text below). After waking up, re-acquire the mutex
*before returning to the caller
*/

int cv_broadcast(struct cv *cv){
	spin_lock(&cv -> lock);
	int up_count = 0; 
	if(!cv -> count){
		spin_unlock(&cv -> lock);
		return 0; 
	}
	for(int i = 0; i < CV_MAXPROC; i++){
		if(cv -> pid[i]){
			kill(cv -> pid[i], SIGUSR1);
			up_count++;
		}
	}
	spin_unlock(&cv -> lock);
	return up_count;
}
/* Wake up any and all waiters (sleepers) on this cv. If there are no waiters
*the call has no effect and is not "remembered" for the next time that
*someone calls cv_wait. cv_broadcast should be called with the same mutex
*held that protects cv_wait, as discussed in lecture notes under "Lost
*Wakup", but note that cv_broadcast does not take a mutex as a parameter.
*Return value: the number of sleepers that were awoken.
*/

int cv_signal(struct cv *cv){
	spin_lock(&cv -> lock);
	int up_count = 0; 

	if(!cv -> count){
		spin_unlock(&cv -> lock);
		return 0; 
	}

	kill(cv -> pid[--(cv -> count)], SIGUSR1);
	up_count++;
	spin_unlock(&cv -> lock);
	
	return up_count;
}
/* Exactly the same as cv_broadcast except at most one sleeper is awoken.
*Your choice how to pick which one if more than one candidate
*/

#endif
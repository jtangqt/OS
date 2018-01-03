#include "cv.h"

void sig_handler(int sig){
	usr_interrupt = 1;
}

void cv_init(struct cv *cv){
	
	spin_unlock(&cv -> lock);
	cv -> count = 0; 
	for(int i = 0; i < CV_MAXPROC; i++)
		cv -> pid[i] = 0; 

	signal(SIGUSR1, sig_handler);
	sigfillset(&cv -> sigmask);
	sigdelset(&cv -> sigmask, SIGUSR1);

}

void cv_wait(struct cv *cv, struct spinlock *mutex){
	if(cv -> count >= CV_MAXPROC){
		fprintf(stderr, "ERROR MESSAGE");
		exit(255);
	}

	//fprintf(stderr, " HERE1 ");


	spin_lock(&cv -> lock);
	usr_interrupt = 0; 
	cv -> pid[cv -> count] = getpid();
	cv -> count++; 
	/*

	spin_unlock(mutex);
	spin_unlock(&cv -> lock);
	sigprocmask(SIG_BLOCK, &cv -> sigmask, NULL);

	fprintf(stderr, "HERE2 ");

	//sigsuspend(&cv -> sigmask);

	fprintf(stderr, "HERE3 %d\n", cv -> count);

	if(cv -> count > 0){
		spin_lock(&cv -> lock);
		fprintf(stderr,"cv_wait: process[sigsuspended] woke up!\n");
		cv -> pid[cv -> count-1] = 0; 
		cv -> count--; 
		spin_unlock(&cv -> lock);
		spin_lock(mutex);
		return; 
	}

	fprintf(stderr, "HERE4 ");

	sigprocmask(SIG_UNBLOCK, &cv -> sigmask, NULL);
	spin_lock(mutex);
	
	*/


	sigprocmask(SIG_BLOCK, &cv -> sigmask, &oldmask); // block all signals in sigmask 
	spin_unlock(mutex);
	spin_unlock(&cv->lock);

	fprintf(stderr, "%d ", cv -> count);

	while(!usr_interrupt)
		sigsuspend(&oldmask);

	spin_lock(mutex);
	sigprocmask(SIG_UNBLOCK, &cv -> sigmask, NULL);

	fprintf(stderr, "HEHE XD");
	
}

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
	cv -> count = 0; 
	spin_unlock(&cv -> lock);
	return up_count;
}


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

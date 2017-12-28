#include "spinlock.h"

void error_message(const char *message, const char *error_val){
	if(!error_val)
		fprintf(stderr, "%s\n", message);
	else
		fprintf(stderr, "%s Error value: %s.\n", message, error_val);
}

void spin_lock(struct spinlock *l){
	while(tas(&(l -> p_lock)));
}

void spin_unlock(struct spinlock *l){
	l -> p_lock = 0; 
}
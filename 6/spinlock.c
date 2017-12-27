#include "spinlock.h"

void spin_lock(struct spinlock *l){
	while(tas(&(l -> p_lock)));
}

void spin_unlock(struct spinlock *l){
	l -> p_lock = 0; 
}
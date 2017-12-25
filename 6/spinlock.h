#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

typedef struct spinlock{
	volatile char p_lock;
}spinlock;

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);
int tas(volatile char *l);
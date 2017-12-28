#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <time.h> 
#include <signal.h>
#include <sys/wait.h>

typedef struct spinlock{
	volatile char p_lock;
} spinlock;

void error_message(const char *message, const char *error_val);
void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);
int tas(volatile char *l);
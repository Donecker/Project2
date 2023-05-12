#include "private.h"
#include "queue.h"
#include "sem.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// @count: amount of resources
// @blocked: queue of blocked threads for this semaphore
struct semaphore {
	int count;
	queue_t blocked;
};

sem_t sem_create(size_t count){

	// Allocate memory for semaphore
	sem_t sem = (sem_t)malloc(sizeof(sem_t));

	// Check if memory was successfully allocated
	if (sem == NULL) {
		return NULL;
	}

	// Set member variables to default
	sem->count = count;
	sem->blocked = queue_create();

	return sem;
}

int sem_destroy(sem_t sem){

	// Check if sem is NULL
	if (sem == NULL) {
		return -1;
	}

	// Check if sem isn't empty
	if (queue_length(sem->blocked) > 0) {
		return -1;
	}
	
	// Free up the memory allocated to sem
	free(sem);

	return 0;
}

int sem_down(sem_t sem){

	// Check if sem is NULL
	if (sem == NULL) {
		return -1;
	}

	// Not a regular spinlock since the item is put in the blocked queue.
	// This prevents the thread from accessing the resource if it happens to gets blocked again
	while (sem->count == 0) {
		queue_enqueue(sem->blocked, uthread_current());
		uthread_block();
	}

	// Once it exits, successfully take the lock
	sem->count--;

	return 0;
}

int sem_up(sem_t sem){

	// Check if sem is NULL
	if (sem == NULL) {
		return -1;
	}

	// Check if there are no blocked threads
	if (queue_length(sem->blocked) == 0) {
		// Free up lock
		sem->count++;
		return 0;
	}

	// Holder variable for the blocked thread
	struct uthread_tcb* blocked_thread;

	// Dequeue the thread from the queue of blocked threads
	queue_dequeue(sem->blocked, (void**)&blocked_thread);

	// Put the blocked thread back into ready queue
	uthread_unblock(blocked_thread);

	// Free up lock
	sem->count++;

	return 0;
}


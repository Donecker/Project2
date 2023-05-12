#include "uthread.h"
#include "private.h"
#include "queue.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * The thread state represents all the possible states of a thread.
 */
enum thread_state {INIT, READY, RUNNING, WAITING, FINISHED};

/*
 * Thread control block containing the thread state, context, and stack
 * references.
 */
struct uthread_tcb {
	enum thread_state state;
	uthread_ctx_t* context;
	void* stack;
};

/* The ready queue holds thread control blocks representing threads that are
 * ready to be run by the scheduler.
 */
queue_t ready_queue;

/*
 * The finished queue holds thread control blocks representing the threads that
 * have exited but are awaiting destruction.
 */
queue_t finished_queue;

/*
 * The currently running thread control block.
 */
struct uthread_tcb* current_thread;

int uthread_run(bool preempt, uthread_func_t func, void *arg) {

	// Initialize global variables
	ready_queue = queue_create();
	finished_queue = queue_create();
	current_thread = NULL;

	// Build main thread control block
	struct uthread_tcb* main_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (main_thread == NULL) {
		return -1;
	}
	main_thread->state = RUNNING;
	main_thread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (main_thread->context == NULL) {
		return -1;
	}
	getcontext(main_thread->context);
	main_thread->stack = NULL;

	// Main thread is the current running thread
	current_thread = main_thread;

	// Create a new thread to populate the ready queue
	if (uthread_create(func, arg) == -1) {
		return -1;
	}

	preempt_start(preempt);

	while (queue_length(ready_queue) > 0) {

		uthread_yield();

		while (queue_length(finished_queue) > 0) {

			// Dequeue finished thread
			struct uthread_tcb* finished_thread;
			queue_dequeue(finished_queue, (void**)&finished_thread);

			// Destroy finished thread
			uthread_ctx_destroy_stack(finished_thread->stack);
			free(finished_thread->context);
			free(finished_thread);
		}
	}

	if (preempt) {
		preempt_stop();
	}

	// Destroy queues
	queue_destroy(ready_queue);
	queue_destroy(finished_queue);

	// Destroy main thread context
	free(main_thread->context);

	return 0;
}

int uthread_create(uthread_func_t func, void *arg) {

	// Create new thread control block
	struct uthread_tcb* new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (new_thread == NULL) {
		return -1;
	}

	// Initialize new thread control block
	new_thread->state = INIT;
	new_thread->stack = uthread_ctx_alloc_stack();
	new_thread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (new_thread->context == NULL) {
		return -1;
	}
	if (uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) == -1) {
		return -1;
	}

	// Critical code begin
	preempt_disable();

	// Put the new thread into the ready queue
	queue_enqueue(ready_queue, new_thread);

	// Critical code end
	preempt_enable();

	// Update thread state
	new_thread->state = READY;

	return 0;
}

void uthread_yield(void) {

	// Critical code begin
	preempt_disable();

	// Store the current thread
	struct uthread_tcb *previous_thread = current_thread;

	// Get the next thread to run
	queue_dequeue(ready_queue, (void **) &current_thread);

	// Put the previous thread into the ready queue
	queue_enqueue(ready_queue, previous_thread);

	// Critical code end
	preempt_enable();

	// Update thread states
	previous_thread->state = READY;
	current_thread->state = RUNNING;

	// Switch contexts between the previous thread and the current thread
	uthread_ctx_switch(previous_thread->context, current_thread->context);
}

void uthread_exit(void) {

	// Critical code begin
	preempt_disable();

	// Store the current thread
	struct uthread_tcb* previous_thread = current_thread;

	// Get the next thread to run
	queue_dequeue(ready_queue, (void**)&current_thread);

	// Put the previous thread into the finished queue
	queue_enqueue(finished_queue, previous_thread);

	// Critical code end
	preempt_enable();

	// Update thread states
	previous_thread->state = FINISHED;
	current_thread->state = RUNNING;

	// Switch contexts between the previous thread and the current thread
	uthread_ctx_switch(previous_thread->context, current_thread->context);
}

struct uthread_tcb* uthread_current(void) {
	return current_thread;
}

void uthread_block(void) {

	// Critical code begin
	preempt_disable();

	// Store the current thread
	struct uthread_tcb* blocked_thread = current_thread;

	// Get the next thread to run
	queue_dequeue(ready_queue, (void**)&current_thread);

	// Critical code end
	preempt_enable();

	// Update thread states
	blocked_thread->state = WAITING;
	current_thread->state = RUNNING;

	// Switch contexts between the blocked thread and the current thread
	uthread_ctx_switch(blocked_thread->context, current_thread->context);
}

void uthread_unblock(struct uthread_tcb* uthread) {

	// Put the unblocked thread into the ready queue
	preempt_disable();
	queue_enqueue(ready_queue, uthread);
	preempt_enable();

	// Update thread state
	uthread->state = READY;
}
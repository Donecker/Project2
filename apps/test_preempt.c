/*
 * Test to verify preemption functionality. The output should fluctuate between the following outputs.
 *
 * Thread A --------------------
 * Thread A --------------------
 * ...
 * Thread A --------------------
 * Thread B ----------
 * Thread B ----------
 * ...
 * Thread B ----------
 * ...
 */

#include <stdbool.h>
#include <stdio.h>

#include <uthread.h>

void thread_b(void *arg) {
	(void) arg;
	while (true) {
		printf("Thread B ----------\n");
	}
}

void thread_a(void *arg) {
	(void)arg;
	uthread_create(thread_b, NULL);
	while (true) {
		printf("Thread A --------------------\n");
	}
}

int main(void)
{
	uthread_run(true, thread_a, NULL);
	return 0;
}

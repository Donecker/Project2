#include "private.h"
#include "uthread.h"

#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* Preemption interval (usec) */
const int PREEMPTION_INTERVAL = 10000; // usec

/* Should preempt flag */
bool should_preempt;

/* The signal set */
sigset_t* sigset;

/* The old signal action */
struct sigaction* old_sigaction;

/* The new signal action */
struct sigaction* new_sigaction;

/* The old timer values */
struct itimerval* old_itimerval;

/* The new timer values */
struct itimerval* new_itimerval;

/*
 * The timer interrupt handler
 */
void timer_interrupt_handler(int signum) {

	if (signum == SIGVTALRM) {

		// Preemptively yield to the next thread
		uthread_yield();
	}
}

void preempt_disable(void) {

	if (!should_preempt) {
		return;
	}

	// Mask the alarm signal to disable preemption
	if (sigset != NULL) {
		sigprocmask(SIG_BLOCK, sigset, NULL);
	}
}

void preempt_enable(void) {

	if (!should_preempt) {
		return;
	}

	// Initialize signal set
	if (sigset == NULL) {
		sigset = (sigset_t *) malloc(sizeof(sigset_t));
		sigemptyset(sigset);
		sigaddset(sigset, SIGVTALRM);
	}

	// Unmask the alarm signal to enable preemption
	if (sigset != NULL) {
		sigprocmask(SIG_UNBLOCK, sigset, NULL);
	}
}

void preempt_start(bool preempt) {

	should_preempt = preempt;
	if (!should_preempt) {
		return;
	}

	// Initialize new signal action
	if (new_sigaction == NULL) {
		new_sigaction = (struct sigaction*)malloc(sizeof(struct sigaction));
		new_sigaction->sa_handler = timer_interrupt_handler;
		sigemptyset(&new_sigaction->sa_mask);
		new_sigaction->sa_flags = 0;
	}

	// Set new signal action
	if (old_sigaction == NULL) {
		sigaction(SIGVTALRM, new_sigaction, old_sigaction);
	} else {
		sigaction(SIGVTALRM, new_sigaction, NULL);
	}

	// Initialize new timer values
	if (new_itimerval == NULL) {
		new_itimerval = (struct itimerval*)malloc(sizeof(struct itimerval));

		// Time until next timer expiration
		new_itimerval->it_value.tv_sec = 0;
		new_itimerval->it_value.tv_usec = PREEMPTION_INTERVAL;

		// Interval for periodic timer
		new_itimerval->it_interval.tv_sec = 0;
		new_itimerval->it_interval.tv_usec = PREEMPTION_INTERVAL;
	}

	// Set new timer values
	if (old_itimerval == NULL) {
		setitimer(ITIMER_VIRTUAL, new_itimerval, old_itimerval);
	} else {
		setitimer(ITIMER_VIRTUAL, new_itimerval, NULL);
	}
}

void preempt_stop(void) {

	if (!should_preempt) {
		return;
	}

	// Restore old signal action
	if (old_sigaction != NULL) {
		sigaction(SIGVTALRM, old_sigaction, NULL);
	}

	// Restore old timer values
	if (old_itimerval != NULL) {
		setitimer(ITIMER_VIRTUAL, old_itimerval, NULL);
	}

	// Destroy structs
	if (sigset != NULL) {
		free(sigset);
	}
	if (new_sigaction != NULL) {
		free(new_sigaction);
	}
	if (new_itimerval != NULL) {
		free(new_itimerval);
	}
}
/*
 Watchdog service that monitors unintended resets of the SX1301.
 */

#include <stdint.h>					/* C99 types */
#include <stdbool.h>				/* bool type */
#include <stdio.h>					/* printf, fprintf, snprintf, fopen, fputs */

#include <signal.h>					/* sigaction */
#include <time.h>					/* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>				/* timeval */
#include <stdlib.h>					/* atoi, exit */
#include <errno.h>					/* error messages */

#include "loragw_hal.h"

#define WATCHDOG_INTERVAL 5			/* default watchdog time interval in seconds */

/* Signal handling variables */
volatile bool exit_sig = false;		/* 1 -> application terminates cleanly (shut down hardware, close open files, etc) */
volatile bool quit_sig = false;		/* 1 -> application terminates without shutting down the hardware */

/* Function declarations */
static void sig_handler(int sigio);

/* Function definitions */
static void sig_handler(int sigio) {
	if (sigio == SIGQUIT) {
		quit_sig = true;;
	} else if ((sigio == SIGINT) || (sigio == SIGTERM)) {
		exit_sig = true;
	}
	return;
}


int main(void)
{
	struct sigaction sigact; /* SIGQUIT&SIGINT&SIGTERM signal handling */

	/* configure signal handling */
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigact.sa_handler = sig_handler;
	sigaction(SIGQUIT, &sigact, NULL); /* Ctrl-\ */
	sigaction(SIGINT, &sigact, NULL); /* Ctrl-C */
	sigaction(SIGTERM, &sigact, NULL); /* default "kill" command */

	printf("INFO: Starting SX1301 watchdog service\n");

	while (!exit_sig && !quit_sig) {
		wait_ms(1000 * WATCHDOG_INTERVAL);

		if (exit_sig) {
			printf("INFO: stopping watchdog service\n");
		}

	}

	printf("INFO: exiting watchdog service\n");
	exit(EXIT_SUCCESS);
}

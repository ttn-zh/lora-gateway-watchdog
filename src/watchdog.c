/*
 Watchdog service that monitors unintended resets of the SX1301.
 */

/* Dependencies */

/* Fix an issue between POSIX and C99 */
#ifdef __MACH__
#elif __STDC_VERSION__ >= 199901L
	#define _XOPEN_SOURCE 600
#else
	#define _XOPEN_SOURCE 500
#endif

#include <stdint.h>					/* C99 types */
#include <stdbool.h>				/* bool type */
#include <stdio.h>					/* printf, fprintf, snprintf, fopen, fputs */
#include <signal.h>					/* sigaction */
#include <time.h>					/* time, clock_gettime, strftime, gmtime */
#include <sys/time.h>				/* timeval */
#include <stdlib.h>					/* atoi, exit */
#include <errno.h>					/* error messages */

#include "loragw_hal.h"
#include "loragw_aux.h"

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
	int check_count = 0;

	while (!exit_sig && !quit_sig) {
		wait_ms(1000 * WATCHDOG_INTERVAL);

		uint32_t trig_cnt_us;
		if (lgw_get_trigcnt(&trig_cnt_us) == LGW_HAL_SUCCESS && trig_cnt_us == 0x7E000000) {
			printf("WARN: unintended reset detected\n");
			exit(EXIT_FAILURE);
		} else {
			// Simulate failure exit to test restart
			if (check_count++ > 1) {
				printf("INFO: SX1301 status is valid\n");
				check_count = 0;
				exit(EXIT_FAILURE);
			}
		}

		if (exit_sig) {
			printf("INFO: stopping watchdog service\n");
		}

	}

	printf("INFO: exiting watchdog service\n");
	exit(EXIT_SUCCESS);
}

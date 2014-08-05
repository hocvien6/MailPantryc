#include <libmilter/mfapi.h>

#include "../include/pantryc-signal.h"

/* #Pantryc_signal_handler */
void pantryc_signal__sigterm(int signal);
/* ... TODO more handlers */

int pantryc_signal__register(signals)
	int signals; {
	int result = 1;

	if (!!(signals & PANTRYC_SIGNAL__SIGTERM)) {
		if (signal(SIGTERM, pantryc_signal__sigterm) == SIG_ERR) {
			result &= 0;
		}
	}

	/* TODO add more code to register other #Pantryc_signal_handler here
	 * ...
	 */

	return result;
}

/* Private functions */
#include <stdio.h> // TESTING
void pantryc_signal__sigterm(signal)
	int signal; {

	puts("CATCHED!!!"); // TESTING
	smfi_stop();
}

/* ... TODO more handlers */

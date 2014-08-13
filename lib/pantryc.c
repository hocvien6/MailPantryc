#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <libmilter/mfapi.h>

#include "../include/pantryc.h"

bool setport = FALSE;

pantrycMilter milter = { "PantrycMilter", /* filter name */
SMFI_VERSION, /* version code -- do not change */
SMFIF_ADDHDRS | SMFIF_ADDRCPT,
/* flags */
pantryc_milter__xxfi_connect, /* connection info filter */
pantryc_milter__xxfi_helo, /* SMTP HELO command filter */
pantryc_milter__xxfi_envfrom, /* envelope sender filter */
pantryc_milter__xxfi_envrcpt, /* envelope recipient filter */
pantryc_milter__xxfi_header, /* header filter */
pantryc_milter__xxfi_eoh, /* end of header */
pantryc_milter__xxfi_body, /* body block filter */
pantryc_milter__xxfi_eom, /* end of message */
pantryc_milter__xxfi_abort, /* message aborted */
pantryc_milter__xxfi_close, /* connection cleanup */
pantryc_milter__xxfi_unknown, /* unknown SMTP commands */
pantryc_milter__xxfi_data, /* DATA command */
pantryc_milter__xxfi_negotiate /* Once, at the start of each SMTP connection */
};

bool pantryc__set_port(port)
	char *port; {

	if (smfi_setconn(port) == MI_FAILURE) {
		setport = FALSE;
	} else {
		/*
		 * If we're using a local socket, make sure it
		 * doesn't already exist.  Don't ever run this
		 * code as root!!
		 */
		if (strncasecmp(port, "unix:", 5) == 0)
			unlink(port + 5);
		else if (strncasecmp(port, "local:", 6) == 0)
			unlink(port + 6);
		setport = TRUE;
	}
	return setport;
}

bool pantryc__set_timeout(timeout)
	char *timeout; {
	if (smfi_settimeout(atoi(timeout)) == MI_FAILURE) {
		return FALSE;
	}
	return TRUE;
}

int pantryc__run(argc, argv)
	int argc;char **argv; {
	if (!setport) {
		exit(EX_USAGE);
	}
	if (smfi_register(milter) == MI_FAILURE) {
		exit(EX_UNAVAILABLE);
	}

	return smfi_main();
}

int pantryc__quit() {
	return pantryc_sqlite__close();
}
/* eof */

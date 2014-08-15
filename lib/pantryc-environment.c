#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <libmilter/mfapi.h>

#include "../include/pantryc-environment.h"

static pBOOL pantryc_environment__setport = pFALSE;

pantrycMilter pantryc_environment__milter = { "PantrycMilter", /* filter name */
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

pBOOL pantryc_environment__set_port(port)
	char *port; {

	if (smfi_setconn(port) == MI_FAILURE) {
		pantryc_environment__setport = pFALSE;
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
		pantryc_environment__setport = pTRUE;
	}
	return pantryc_environment__setport;
}

pBOOL pantryc_environment__set_timeout(timeout)
	char *timeout; {
	if (smfi_settimeout(atoi(timeout)) == MI_FAILURE) {
		return pFALSE;
	}
	return pTRUE;
}

int pantryc_environment__run(argc, argv)
	int argc;char **argv; {
	if (!pantryc_environment__setport) {
		exit(EX_USAGE);
	}
	if (smfi_register(pantryc_environment__milter) == MI_FAILURE) {
		exit(EX_UNAVAILABLE);
	}

	return smfi_main();
}

pBOOL pantryc_environment__quit() {
	return sqlite3_close(pantryc_sqlite__database)
			&& fclose(pantryc_global__log_file);
}
/* eof */

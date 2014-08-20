#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <libmilter/mfapi.h>

#include "../include/pantryc-milter.h"
#include "../include/pantryc-environment.h"

#define PANTRYC_ENVIRONMENT__LOG_FILE	"log.txt"

typedef struct smfiDesc pantrycMilter; /* structure mail filter Describe */

static pBOOL pantryc_environment__setport = pFALSE;
static pantrycMilter pantryc_environment__milter = { "PantrycMilter", /* filter name */
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

void pantryc_global__change_working_directory(directory)
	const char *directory; {
	if (directory != NULL) {
		if (pantryc_environment__working_directory != NULL) {
			free(pantryc_environment__working_directory);
		}
		pantryc_environment__working_directory = (char*) malloc(
				(strlen(directory) + 1) * sizeof(char));
		strcpy(pantryc_environment__working_directory, directory);
	}
}

void pantryc_global__change_attachment_permission(permission)
	const int permission; {
	pantryc_environment__attachment_permission = permission;
}

void pantryc_global__create_log_file() {
	// TESTING TODO rename log file, #filename; change log file mode
	char *filename = (char*) malloc(
			sizeof(char)
					* (strlen(pantryc_environment__working_directory)
							+ strlen(PANTRYC_ENVIRONMENT__LOG_FILE) + 1));
	strcpy(filename, pantryc_environment__working_directory);
	strcat(filename, PANTRYC_ENVIRONMENT__LOG_FILE);
	pantryc_environment__log_file = fopen(filename, "w+");
}

pBOOL pantryc_global__close_log_file() {
	if (pantryc_environment__log_file != NULL
			&& fclose(pantryc_environment__log_file) == EOF) {
		return pFALSE;
	}
	return pTRUE;
}

pBOOL pantryc_environment__set_port(port)
	char *port; {
	if (port != NULL) {
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
	} else
		return pFALSE;
}

pBOOL pantryc_environment__set_timeout(timeout)
	char *timeout; {
	if (timeout != NULL) {
		if (smfi_settimeout(atoi(timeout)) == MI_FAILURE) {
			return pFALSE;
		}
		return pTRUE;
	} else
		return pFALSE;
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
/* eof */

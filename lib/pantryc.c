#include "../include/pantryc.h"

pBOOL pantryc__born(directory, permission, port, timeout)
	const char *directory;const int permission;char *port;char *timeout; {
	pantryc_global__change_working_directory(directory);
	pantryc_global__change_attachment_permission(permission);
	pBOOL setport = pantryc_environment__set_port(port);
	pBOOL settimeout = pantryc_environment__set_timeout(timeout);
	return setport && settimeout;
}

void pantryc__learn_add_rejected_receipt_address(address)
	char * address; {
	pantryc_sqlite__add_rejected_receipt_address(address);
}

int pantryc__work(argc, argv)
	int argc;char **argv; {
	pantryc_global__create_log_file();
	if (pantryc_sqlite__open_database() == pFALSE)
		return pFALSE;
	else
		return pantryc_environment__run(argc, argv);
}

pBOOL pantryc__die() {
	return pantryc_environment__quit();
}

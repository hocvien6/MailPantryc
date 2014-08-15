#include "../include/pantryc.h"
#include "../include/pantryc-environment.h"

int pantryc__born(directory, permission, port, timeout)
	const char *directory;const int permission;char *port;char *timeout; {
	pantryc_global__change_working_directory(directory);
	pantryc_global__change_attachment_permission(permission);
	pBOOL setport = pantryc_environment__set_port(port);
	pBOOL settimeout = pantryc_environment__set_timeout(timeout);

	pantryc_global__create_log_file();
	pBOOL opendatabase = pantryc_sqlite__open_database();
	return setport && settimeout && opendatabase;
}

void pantryc__learn_add_rejected_receipt_address(address)
	char * address; {
	pantryc_sqlite__add_rejected_receipt_address(address);
}

int pantryc__work(argc, argv)
	int argc;char **argv; {
	return pantryc_environment__run(argc, argv);
}

int pantryc__die() {
	pBOOL closelogfile = pantryc_global__close_log_file();
	pBOOL closedatabase = pantryc_sqlite__close_database();
	return closelogfile && closedatabase;
}

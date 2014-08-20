#include "../include/pantryc.h"
#include "../include/pantryc-environment.h"
#include "../include/pantryc-sqlite.h"

int pantryc__born(directory, permission, score, port, timeout)
	const char *directory;const int permission;const int score;char *port;char *timeout; {
	pantryc_environment__change_working_directory(directory);
	pantryc_environment__change_attachment_permission(permission);
	pantryc_environment__change_required_score(score);
	pBOOL setport = pantryc_environment__set_port(port);
	pBOOL settimeout = pantryc_environment__set_timeout(timeout);

	pantryc_environment__create_log_file();
	pBOOL opendatabase = pantryc_sqlite__open_database();
	return setport && settimeout && opendatabase;
}

void pantryc__learn_insert_rejected_receipt_address(address)
	char * address; {
	pantryc_sqlite__insert_rejected_receipt_address(address);
}

void pantryc__learn_delete_rejected_receipt_address(address)
	char* address; {
	pantryc_sqlite__delete_rejected_receipt_address(address);
}

void pantryc__learn_insert_bad_word(word, score)
	char *word;int score; {
	pantryc_sqlite__insert_bad_word(word, score);
}

void pantryc__learn_delete_bad_word(word)
	char *word; {
	pantryc_sqlite__delete_bad_word(word);
}

int pantryc__work(argc, argv)
	int argc;char **argv; {
	return pantryc_environment__run(argc, argv);
}

int pantryc__die() {
	pBOOL closelogfile = pantryc_environment__close_log_file();
	pBOOL closedatabase = pantryc_sqlite__close_database();
	return closelogfile && closedatabase;
}

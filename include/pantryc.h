#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-milter.h>

typedef struct smfiDesc pantrycMilter; /* structure mail filter Describe */

/**
 * Add an address to rejected recipients list
 * @address:		rejected address to add
 */
void pantryc__add_rejected_receipt_address(char *address);

/**
 * Add an address to rejected recipients list
 * @address:		rejected address to add
 */
void pantryc__remove_rejected_receipt_address(char *address);

/**
 * Set port for pantryc to connect
 * @port:			port to set (ex: f1.sock...)
 * return			set port successful or not
 */
bool pantryc__set_port(char *port);

/**
 * Set time out
 * @timeout:		time out
 * return			set time out successful or not
 */
bool pantryc__set_timeout(char *timeout);

/**
 * Pantryc milter start
 * @argc:
 * @argv:
 */
int pantryc__run(int argc, char **argv);

/**
 * Pantryc milter quit
 * return
 */
int pantryc__quit();

#endif /* PANTRYC_H_ */

#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-environment.h>

/**
 * Born
 * @directory:		where extract attachments and store log file for each message
 * @permission:		mode of permission (ex: 0644, 0755...)
 * @port:			port to set (ex: f1.sock...)
 * @timeout:		time out
 */
pBOOL pantryc__born(const char *directory, const int permission, char *port, char *timeout);

/**
 * Learn how to add rejected receipt address
 * @address:		rejected address to add
 */
void pantryc__learn_add_rejected_receipt_address(char *address);

/**
 * Work
 * @argc
 * @argv
 */
int pantryc__work(int argc, char **argv);

/**
 * Die
 */
pBOOL pantryc__die();

#endif /* PANTRYC_H_ */

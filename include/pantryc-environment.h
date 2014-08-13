#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-milter.h>

typedef struct smfiDesc pantrycMilter; /* structure mail filter Describe */

/**
 * Set port for pantryc to connect
 * @port:			port to set (ex: f1.sock...)
 * return			set port successful or not
 */
bool pantryc_environment__set_port(char *port);

/**
 * Set time out
 * @timeout:		time out
 * return			set time out successful or not
 */
bool pantryc_environment__set_timeout(char *timeout);

/**
 * Pantryc milter start
 * @argc:
 * @argv:
 */
int pantryc_environment__run(int argc, char **argv);

/**
 * Pantryc milter quit
 * return
 */
int pantryc_environment__quit();

#endif /* PANTRYC_H_ */
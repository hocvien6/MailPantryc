#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-milter.h>

typedef struct smfiDesc pantrycMilter; /* structure mail filter Describe */

/**
 * Change working directory of pantryc milter
 * @directory		where extract attachments and store log file for each message
 */
void pantryc__change_working_directory(const char *directory);

/**
 * Change permission of extracted attachment
 * @permission:		mode of permission (ex: 0644, 0755...)
 */
void pantryc__change_attachment_permission(const int permission);

/**
 * Set port for pantryc to connect
 * @port			port to set (ex: f1.sock...)
 * return			set port successful or not
 */
bool pantryc__set_port(char *port);

/**
 * Set time out
 * @timeout			time out
 * return			set time out successful or not
 */
bool pantryc__set_time_out(char *timeout);

/**
 * Pantryc milter start
 * @argc
 * @argv
 */
int pantryc__run(int argc, char **argv);

#endif /* PANTRYC_H_ */

#ifndef PANTRYC_ENVIRONMENT_H_
#define PANTRYC_ENVIRONMENT_H_

#include <stdio.h>

#include <pantryc-util.h>

char *pantryc_environment__working_directory;
FILE *pantryc_environment__log_file;
int pantryc_environment__attachment_permission;

/**
 * Change working directory of pantryc milter
 * @directory:		where extract attachments and store log file for each message
 */
void pantryc_global__change_working_directory(const char *directory);

/**
 * Change permission of extracted attachment
 * @permission:		mode of permission (ex: 0644, 0755...)
 */
void pantryc_global__change_attachment_permission(const int permission);

/**
 * Create log file
 */
void pantryc_global__create_log_file();

/**
 * Close log file
 */
pBOOL pantryc_global__close_log_file();

/**
 * Set port for pantryc to connect
 * @port:			port to set (ex: f1.sock...)
 * return			set port successful or not
 */
pBOOL pantryc_environment__set_port(char *port);

/**
 * Set time out
 * @timeout:		time out
 * return			set time out successful or not
 */
pBOOL pantryc_environment__set_timeout(char *timeout);

/**
 * Pantryc milter start
 * @argc:
 * @argv:
 */
int pantryc_environment__run(int argc, char **argv);

#endif /* PANTRYC_ENVIRONMENT_H_ */

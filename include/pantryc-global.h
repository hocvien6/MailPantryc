#ifndef PANTRYC_GLOBAL_H_
#define PANTRYC_GLOBAL_H_

#include <stdio.h>

#define PANTRYC_GLOBAL__LOG_FILE	"log.txt"

// TODO rename macro
#define pFALSE		(0)
#define pTRUE		!pFALSE
typedef int pBOOL;

char *pantryc_global__working_directory;
FILE *pantryc_global__log_file;
int pantryc_global__attachment_permission;

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

#endif /* PANTRYC_GLOBAL_H_ */

#ifndef PANTRYC_GLOBAL_H_
#define PANTRYC_GLOBAL_H_

#include <pantryc-list.h>

char *pantryc_global__working_directory;
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

#endif /* PANTRYC_GLOBAL_H_ */

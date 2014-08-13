#ifndef PANTRYC_GLOBAL_H_
#define PANTRYC_GLOBAL_H_

char *pantryc_milter__working_directory;
int pantryc_milter__attachment_permission;

/**
 * Change working directory of pantryc milter
 * @directory:		where extract attachments and store log file for each message
 */
void pantryc__change_working_directory(const char *directory);

/**
 * Change permission of extracted attachment
 * @permission:		mode of permission (ex: 0644, 0755...)
 */
void pantryc__change_attachment_permission(const int permission);

#endif /* PANTRYC_GLOBAL_H_ */

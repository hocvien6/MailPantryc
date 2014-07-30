#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-scanner.h>
#include <pantryc-base64.h>

char *pantryc_working_directory;
/**
 * Pantryc milter start
 */
int pantryc_run(int argc, char **argv);
void pantryc_change_working_directory(const char *new_directory);

#endif /* PANTRYC_H_ */

#ifndef PANTRYC_H_
#define PANTRYC_H_

#include <pantryc-milter.h>

void pantryc__change_working_directory(const char *directory);
void pantryc__change_attachment_permission(const int permission);
bool pantryc__set_port(char *port);
bool pantryc__set_time_out(char *timeout);
/**
 * Pantryc milter start
 */
int pantryc__run(int argc, char **argv);

#endif /* PANTRYC_H_ */

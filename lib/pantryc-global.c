#include <stdlib.h>

#include "../include/pantryc-global.h"

void pantryc__change_working_directory(directory)
	const char *directory; {
	if (pantryc_milter__working_directory != NULL) {
		free(pantryc_milter__working_directory);
	}
	pantryc_milter__working_directory = (char*) malloc(
			(strlen(directory) + 1) * sizeof(char));
	strcpy(pantryc_milter__working_directory, directory);
}

void pantryc__change_attachment_permission(permission)
	const int permission; {
	pantryc_milter__attachment_permission = permission;
}

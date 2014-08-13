#include <stdlib.h>
#include <string.h>

#include "../include/pantryc-global.h"

void pantryc_global__change_working_directory(directory)
	const char *directory; {
	if (pantryc_global__working_directory != NULL) {
		free(pantryc_global__working_directory);
	}
	pantryc_global__working_directory = (char*) malloc(
			(strlen(directory) + 1) * sizeof(char));
	strcpy(pantryc_global__working_directory, directory);
}

void pantryc_global__change_attachment_permission(permission)
	const int permission; {
	pantryc_global__attachment_permission = permission;
}

#include <stdlib.h>
#include <string.h>

#include "../include/pantryc-global.h"

void pantryc_global__change_working_directory(directory)
	const char *directory; {
	if (directory != NULL) {
		if (pantryc_global__working_directory != NULL) {
			free(pantryc_global__working_directory);
		}
		pantryc_global__working_directory = (char*) malloc(
				(strlen(directory) + 1) * sizeof(char));
		strcpy(pantryc_global__working_directory, directory);
	}
}

void pantryc_global__change_attachment_permission(permission)
	const int permission; {
	pantryc_global__attachment_permission = permission;
}

void pantryc_global__create_log_file() {
	// TESTING TODO rename log file, #filename; change log file mode
	char *filename = (char*) malloc(
			sizeof(char)
					* (strlen(pantryc_global__working_directory)
							+ strlen(PANTRYC_GLOBAL__LOG_FILE) + 1));
	strcpy(filename, pantryc_global__working_directory);
	strcat(filename, PANTRYC_GLOBAL__LOG_FILE);
	pantryc_global__log_file = fopen(filename, "w+");
}

pBOOL pantryc_global__close_log_file() {
	if (pantryc_global__log_file != NULL
			&& fclose(pantryc_global__log_file) == EOF) {
		return pFALSE;
	}
	return pTRUE;
}

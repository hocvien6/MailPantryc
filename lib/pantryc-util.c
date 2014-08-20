#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../include/pantryc-util.h"

char* pantryc_util__to_lower(text)
	char*text; {
	int length = strlen(text);
	char *result = (char*) malloc(sizeof(char) * (length + 1));
	strcpy(result, text);
	result[length] = '\0';
	int i = -1;
	while (result[++i] != '\0') {
		result[i] = tolower(result[i]);
	}
	return result;
}

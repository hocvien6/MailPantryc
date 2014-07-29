#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "../include/pantryc-base64.h"

FILE * pantryc_base64_decode_gzip(text, filename)
	const char *text;char *filename; {

	gsize length;
	guchar *res = g_base64_decode(text, &length);

	guchar *data;
	data = (guchar*) malloc(length * sizeof(guchar));
	memcpy(data, res, length);

	FILE *file = fopen(filename, "wb");
	fwrite(data, sizeof(guchar), length, file);

	return file;
}


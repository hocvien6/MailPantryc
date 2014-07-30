#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "../include/pantryc-scanner.h"

gchar* pantryc_scanner_get_recip(message, type)
	GMimeMessage *message;GMimeRecipientType type; {
	char *recep;
	InternetAddressList *receps;

	receps = g_mime_message_get_recipients(message, type);
	recep = (char*) internet_address_list_to_string(receps, FALSE);

	if (!recep || !*recep) {
		g_free(recep);
		return NULL;
	}

	return recep;
}

gchar* pantryc_scanner_print_date(message)
	GMimeMessage *message; {
	time_t t;
	int tz;
	char buf[64];
	size_t len;
	struct tm *t_m;

	g_mime_message_get_date(message, &t, &tz);
	t_m = localtime(&t);

	len = strftime(buf, sizeof(buf) - 1, "%c", t_m);

	if (len > 0) {
		gchar *date = (gchar*) malloc(72 * sizeof(gchar));
		sprintf(date, "%s (%s%04d)\n", buf, tz < 0 ? "-" : "+", tz);
		return date;
	}
	return NULL;
}

gchar* pantryc_scanner_get_refs_str(message)
	GMimeMessage *message; {
	const gchar *str;
	const GMimeReferences *cur;
	GMimeReferences *mime_refs;
	gchar *rv;

	str = g_mime_object_get_header(GMIME_OBJECT(message), "References");
	if (!str)
		return NULL;

	mime_refs = g_mime_references_decode(str);
	for (rv = NULL, cur = mime_refs; cur;
			cur = g_mime_references_get_next(cur)) {

		const char* messageid;
		messageid = g_mime_references_get_message_id(cur);
		rv = g_strdup_printf("%s%s%s", rv ? rv : "", rv ? "," : "", messageid);
	}
	g_mime_references_free(mime_refs);

	return rv;
}

GMimeStream* pantryc_scanner_extract_attachment(message, index, permission,
		filename)
	GMimeMessage *message;int index;const char* permission;char *filename; {

	GMimeMultipart *parts = (GMimeMultipart*) message->mime_part;
	GMimeObject *object = g_mime_multipart_get_part(parts, index);

	if (!GMIME_IS_CONTENT_DISPOSITION(
			g_mime_object_get_content_disposition(object))
			|| strcmp(
					g_mime_content_disposition_get_disposition(
							g_mime_object_get_content_disposition(object)),
					(char*) GMIME_DISPOSITION_ATTACHMENT) != 0)
		return NULL;

	GMimePart *part = (GMimePart*) object;
	strcat(filename, g_mime_part_get_filename(part));
	printf("%s\n",filename); // TESTING
	int fd = open(filename, O_CREAT | O_WRONLY | O_EXCL, permission);

	GMimeStream *stream = g_mime_stream_fs_new(fd);
	GMimeDataWrapper *data = g_mime_part_get_content_object(part);
	g_mime_data_wrapper_write_to_stream(data, stream);

	return stream;
}

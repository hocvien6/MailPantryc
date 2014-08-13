#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "../include/pantryc-scanner.h"

gchar* pantryc_scanner__get_recip(message, type)
	GMimeMessage *message;GMimeRecipientType type; {
	char *recip;
	InternetAddressList *receps;

	receps = g_mime_message_get_recipients(message, type);
	recip = (char*) internet_address_list_to_string(receps, FALSE);

	if (!recip || !*recip) {
		g_free(recip);
		return NULL;
	}

	return recip;
}

gchar* pantryc_scanner__get_date(message)
	GMimeMessage *message; {
	time_t timer;
	int timezone;
	char buf[64];
	size_t length;
	struct tm *time;

	g_mime_message_get_date(message, &timer, &timezone);
	time = localtime(&timer);

	length = strftime(buf, sizeof(buf) - 1, "%c", time);

	if (length > 0) {
		gchar *date = (gchar*) malloc(72 * sizeof(gchar));
		sprintf(date, "%s (%s%04d)", buf, timezone < 0 ? "-" : "+", timezone);
		return date;
	}
	return NULL;
}

gchar* pantryc_scanner__get_references(message)
	GMimeMessage *message; {
	const gchar *header;
	const GMimeReferences *cur;
	GMimeReferences *mime_refs;
	gchar *references;

	header = g_mime_object_get_header(GMIME_OBJECT(message), "References");
	if (!header)
		return NULL;

	mime_refs = g_mime_references_decode(header);
	for (references = NULL, cur = mime_refs; cur; cur =
			g_mime_references_get_next(cur)) {

		const char* id;
		id = g_mime_references_get_message_id(cur);
		references = g_strdup_printf("%s%s%s", references ? references : "",
				references ? "," : "", id);
	}
	g_mime_references_free(mime_refs);

	return references;
}

gchar* pantryc_scanner__get_content(message, index)
	GMimeMessage *message;int index; {
	GMimeMultipart *parts = (GMimeMultipart*) message->mime_part;
	GMimeObject *object = g_mime_multipart_get_part(parts, index);
	GMimePart *part = (GMimePart*) object;
	GMimeStream *stream = g_mime_data_wrapper_get_stream(
			g_mime_part_get_content_object(part));
	gint64 length = g_mime_stream_length(stream);
	gchar *content = (gchar*) malloc(sizeof(gchar) * (length + 1));
	g_mime_stream_read(stream, content, length);

	// TODO cut off the tail, modify appropriate index of the last character
	content[length - 2] = '\0';

	return content;
}

GMimeStream* pantryc_scanner__extract_attachment(message, index, permission,
		filepath)
	GMimeMessage *message;int index;const int permission;const char *filepath; {
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
	const char *filename = g_mime_part_get_filename(part);
	char *filefullname;
	filefullname = (char*) malloc(
			(strlen(filepath) + strlen(filename) + 1) * sizeof(char));
	strcpy(filefullname, filepath);
	strcat(filefullname, filename);
	int fd = open(filefullname, O_CREAT | O_WRONLY | O_EXCL, permission);

	GMimeStream *stream = g_mime_stream_fs_new(fd);
	GMimeDataWrapper *data = g_mime_part_get_content_object(part);
	g_mime_data_wrapper_write_to_stream(data, stream);

	return stream;
}

FILE* pantryc_scanner__read_attachment(message, index)
	GMimeMessage *message;int index; {
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
	GMimeStream *stream = g_mime_data_wrapper_get_stream(
			g_mime_part_get_content_object(part));
	gint64 length = g_mime_stream_length(stream);
	char *content = (char*) malloc(sizeof(char) * length);
	g_mime_stream_read(stream, content, length);

	char *buffer;
	size_t size;
	FILE *file = open_memstream(&buffer, &size);

	// TODO: write content to file stream

	return file;
}

#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <gmime/gmime.h>

#include "../include/pantryc-milter.h"

typedef struct {
	char *buffer;
	size_t size;
	char *connectfrom;
	char *helofrom;
	FILE *mail;
	FILE *log;
	int permission; /* attachment permission */
} PantrycData;

#define PANTRYC_MILTER__GET_PRIVATE_DATA			((PantrycData*) smfi_getpriv(ctx))

sfsistat pantryc_milter__cleanup(SMFICTX *ctx, bool ok);
void pantryc_milter__free_pantrycData(PantrycData *data);
void pantryc_milter__write_message_to_log(PantrycData *data,
		GMimeMessage *message);

sfsistat pantryc_milter__xxfi_connect(ctx, hostname, hostaddr)
	SMFICTX *ctx;char *hostname;_SOCK_ADDR *hostaddr; {
	PantrycData *data;
	char *ident;
	/* allocate some private memory */
	data = malloc(sizeof *data);
	if (data == NULL) {
		/* can't accept this message right now */
		return SMFIS_TEMPFAIL;
	}
	memset(data, '\0', sizeof *data);
	/* save the private data */
	smfi_setpriv(ctx, data);
	ident = smfi_getsymval(ctx, "_");
	if (ident == NULL)
		ident = "???";
	if ((data->connectfrom = strdup(ident)) == NULL) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_helo(ctx, helohost)
	SMFICTX *ctx;char *helohost; {
	size_t len;
	char *tls;
	char *buf;
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	tls = smfi_getsymval(ctx, "{tls_version}");
	if (tls == NULL)
		tls = "No TLS";
	if (helohost == NULL)
		helohost = "???";
	len = strlen(tls) + strlen(helohost) + 3;
	if ((buf = (char*) malloc(len)) == NULL) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	snprintf(buf, len, "%s, %s", helohost, tls);
	if (data->helofrom != NULL)
		free(data->helofrom);
	data->helofrom = buf;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envfrom(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;

#ifdef WIN32
	char szFilename[MAX_PATH]= {0};
#endif /* WIN32 */
	/* open a file to store this message */
#ifdef WIN32
	if (GetTempPath(MAX_PATH, szFilename) == 0)
	{
		/* This shouldn't fail */
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	if ((data->pantryc_fname = strdup(_tempnam(szFilename, "msgPantryc."))) == NULL)
	{
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
#else /* WIN32 */

#endif /* WIN32 */
	// TESTING, TODO rename log file, #logfullname; change log file mod
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	char *logfullname;
	logfullname = (char*) malloc(100 * sizeof(char));
	strcpy(logfullname, pantryc_milter__working_directory);
	strcat(logfullname, "log.txt");
	if ((data->log = fopen(logfullname, "w+")) == NULL) {
		(void) fclose(data->log);
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	if ((data->mail = open_memstream(&data->buffer, &data->size)) == NULL) {
		(void) fclose(data->mail);
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	////
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envrcpt(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	// TESTING
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	char *rcptaddr = smfi_getsymval(ctx, "{rcpt_addr}");
	/* log this recipient */
	if (!PANTRYC_LIST__IS_NULL(
			pantryc_milter__rejected_receipt_addresses) && rcptaddr != NULL) {
		PantrycList *seeker;
		PANTRYC_LIST__TRAVERSE(seeker, pantryc_milter__rejected_receipt_addresses)
		{
			if (strcasecmp(rcptaddr, (char*) pantryc_list__get_value(seeker))
					== 0) {
				if (fprintf(data->log, "RCPT %s -- REJECTED\n", rcptaddr) == EOF) {
					(void) pantryc_milter__cleanup(ctx, FALSE);
					return SMFIS_TEMPFAIL;
				}
				return SMFIS_REJECT;
			}
		}
	}
	////
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_header(ctx, name, value)
	SMFICTX *ctx;char *name;char *value; {
	/* write the header to the mail file */
	if (fprintf(PANTRYC_MILTER__GET_PRIVATE_DATA->mail, "%s: %s\n", name,
			value) == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eoh(ctx)
	SMFICTX *ctx; {
	/* output the blank line between the header and the body */
	if (fprintf(PANTRYC_MILTER__GET_PRIVATE_DATA->mail, "\n") == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_body(ctx, bodyp, bodylen)
	SMFICTX *ctx;unsigned char *bodyp;size_t bodylen; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	/* output body block to mail file */
	if (fwrite(bodyp, bodylen, 1, data->mail) != 1) {
		/* write failed */
		fprintf(data->log, "Couldn't write file (error: %s)\n",
				strerror(errno));
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eom(ctx)
	SMFICTX *ctx; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	rewind(data->mail);

	g_mime_init(GMIME_ENABLE_RFC2047_WORKAROUNDS);
	GMimeStream *stream;
	stream = g_mime_stream_file_new(data->mail);
	if (!stream) {
		g_warning("Cannot open stream");
	}

	GMimeParser *parser;
	GMimeMessage *message;
	parser = NULL;
	message = NULL;
	parser = g_mime_parser_new_with_stream(stream);
	if (!parser) {
		g_warning("failed to create parser");
	}
	message = g_mime_parser_construct_message(parser);
	if (!message) {
		g_warning("failed to construct message");
	}
	pantryc_milter__write_message_to_log(data, message);

	if (GMIME_IS_MULTIPART(message->mime_part)) {
		int number_of_parts = g_mime_multipart_get_count(
				(GMimeMultipart*) message->mime_part);
		data->permission = pantryc_milter__attachment_permission;
		int i;
		for (i = 0; i <= number_of_parts - 1; i++) {
			pantryc_scanner__extract_attachment(message, i, data->permission,
					pantryc_milter__working_directory);
		}
	}
	return pantryc_milter__cleanup(ctx, TRUE);
}

sfsistat pantryc_milter__xxfi_abort(ctx)
	SMFICTX *ctx; {
	return pantryc_milter__cleanup(ctx, FALSE);
}

sfsistat pantryc_milter__xxfi_close(ctx)
	SMFICTX *ctx; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;

	if (data == NULL)
		return SMFIS_CONTINUE;
	else
		pantryc_milter__free_pantrycData(data);
	smfi_setpriv(ctx, NULL);

	g_mime_shutdown();
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_unknown(ctx, cmd)
	SMFICTX *ctx;const char *cmd; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_data(ctx)
	SMFICTX *ctx; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_negotiate(ctx, f0, f1, f2, f3, pf0, pf1, pf2, pf3)
	SMFICTX *ctx;unsigned long f0;unsigned long f1;unsigned long f2;unsigned long f3;unsigned long *pf0;unsigned long *pf1;unsigned long *pf2;unsigned long *pf3; {
	return SMFIS_ALL_OPTS;
}

/* Private functions */

sfsistat pantryc_milter__cleanup(ctx, ok)
	SMFICTX *ctx;bool ok; {
	sfsistat status = SMFIS_CONTINUE;
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	char host[512];
	if (data == NULL)
		return status;
	if (ok) {
		// add a header to the message announcing our presence
		if (gethostname(host, sizeof host) < 0)
			snprintf(host, sizeof host, "localhost");
	} else {
		// message was aborted -- delete the archive file
		fprintf(data->log, "Message aborted. Removing file\n");
		status = SMFIS_TEMPFAIL;
	}

	if (data->log != NULL && fclose(data->log) == EOF) {
		fprintf(data->log, "Couldn't close log file (error: %s)\n",
				strerror(errno));
		status = SMFIS_TEMPFAIL;
	}
	/* close the archive file */
	if (data->mail != NULL && fclose(data->mail) == EOF) {
		/* failed; we have to wait until later */
		fprintf(data->log, "Couldn't close archive file (error: %s)\n",
				strerror(errno));
		status = SMFIS_TEMPFAIL;
	}
	/* return status */
	return status;
}

void pantryc_milter__free_pantrycData(data)
	PantrycData *data; {
	if (data->buffer != NULL)
		free(data->buffer);
	if (data->connectfrom != NULL)
		free(data->connectfrom);
	if (data->helofrom != NULL)
		free(data->helofrom);
	free(data);
}

void pantryc_milter__write_message_to_log(data, message)
	PantrycData *data;GMimeMessage *message; {
	gchar *val;
	const gchar *str;
	fprintf(data->log, "From   : %s\n", g_mime_message_get_sender(message));

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_TO);
	fprintf(data->log, "To     : %s\n", val ? val : "<none>");
	g_free(val);

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_CC);
	fprintf(data->log, "Cc     : %s\n", val ? val : "<none>");
	g_free(val);

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_BCC);
	fprintf(data->log, "Bcc    : %s\n", val ? val : "<none>");
	g_free(val);

	str = g_mime_message_get_subject(message);
	fprintf(data->log, "Subject: %s\n", str ? str : "<none>");

	val = pantryc_scanner__get_date(message);
	fprintf(data->log, "Date   : %s\n", val);

	str = g_mime_message_get_message_id(message);
	fprintf(data->log, "Msg-id : %s\n", str ? str : "<none>");

	gchar *refsstr;
	refsstr = pantryc_scanner__get_references(message);
	fprintf(data->log, "Refs   : %s\n", refsstr ? refsstr : "<none>");
	g_free(refsstr);
}

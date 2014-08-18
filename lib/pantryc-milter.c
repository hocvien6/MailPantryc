#include <ctype.h>
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
} PantrycData;

#define PANTRYC_MILTER__GET_PRIVATE_DATA			((PantrycData*) smfi_getpriv(context))

static sfsistat pantryc_milter__cleanup(SMFICTX *context, pBOOL ok);
static void pantryc_milter__free_pantrycData(PantrycData *data);
static void pantryc_milter__write_message_to_log(PantrycData *data,
		GMimeMessage *message);
static int pantryc_milter__calculate_score_content(gchar *content);

sfsistat pantryc_milter__xxfi_connect(context, hostname, hostaddr)
	SMFICTX *context;char *hostname;_SOCK_ADDR *hostaddr; {
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
	smfi_setpriv(context, data);
	ident = smfi_getsymval(context, "_");
	if (ident == NULL)
		ident = "???";
	if ((data->connectfrom = strdup(ident)) == NULL) {
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_helo(context, helohost)
	SMFICTX *context;char *helohost; {
	size_t len;
	char *tls;
	char *buf;
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	tls = smfi_getsymval(context, "{tls_version}");
	if (tls == NULL)
		tls = "No TLS";
	if (helohost == NULL)
		helohost = "???";
	len = strlen(tls) + strlen(helohost) + 3;
	if ((buf = (char*) malloc(len)) == NULL) {
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	snprintf(buf, len, "%s, %s", helohost, tls);
	if (data->helofrom != NULL)
		free(data->helofrom);
	data->helofrom = buf;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envfrom(context, argv)
	SMFICTX *context;char **argv; {
	int argc = 0;

#ifdef WIN32
	char szFilename[MAX_PATH]= {0};
#endif /* WIN32 */
	/* open a file to store this message */
#ifdef WIN32
	if (GetTempPath(MAX_PATH, szFilename) == 0)
	{
		/* This shouldn't fail */
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	if ((data->pantryc_fname = strdup(_tempnam(szFilename, "msgPantryc."))) == NULL)
	{
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
#else /* WIN32 */

#endif /* WIN32 */

	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	if ((data->mail = open_memstream(&data->buffer, &data->size)) == NULL) {
		(void) fclose(data->mail);
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}

	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envrcpt(context, argv)
	SMFICTX *context;char **argv; {
	int argc = 0;
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	char *rcptaddr = smfi_getsymval(context, "{rcpt_addr}");
	/* log this recipient */
	if (pantryc_sqlite__check_rejected_receipt_address_list(rcptaddr)) {
		if (fprintf(pantryc_global__log_file, "RCPT %s -- REJECTED\n",
				rcptaddr) == EOF) {
			(void) pantryc_milter__cleanup(context, pFALSE);
			return SMFIS_TEMPFAIL;
		}
		return SMFIS_REJECT;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_header(context, name, value)
	SMFICTX *context;char *name;char *value; {
	/* write the header to the mail file */
	if (fprintf(PANTRYC_MILTER__GET_PRIVATE_DATA->mail, "%s: %s\n", name,
			value) == EOF) {
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eoh(context)
	SMFICTX *context; {
	/* output the blank line between the header and the body */
	if (fprintf(PANTRYC_MILTER__GET_PRIVATE_DATA->mail, "\n") == EOF) {
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_body(context, bodyp, bodylen)
	SMFICTX *context;unsigned char *bodyp;size_t bodylen; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	/* output body block to mail file */
	if (fwrite(bodyp, bodylen, 1, data->mail) != 1) {
		/* write failed */
		fprintf(pantryc_global__log_file, "Couldn't write file (error: %s)\n",
				strerror(errno));
		(void) pantryc_milter__cleanup(context, pFALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eom(context)
	SMFICTX *context; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;
	rewind(data->mail);

	g_mime_init(GMIME_ENABLE_RFC2047_WORKAROUNDS);
	GMimeStream *stream;
	stream = g_mime_stream_file_new(data->mail);
	if (!stream) {
		g_warning("Cannot open stream");
	}

	/* Parse the message only once at this time */
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
		int i;
		for (i = 0; i <= number_of_parts - 1; i++) {
			pantryc_scanner__extract_attachment(message, i,
					pantryc_global__attachment_permission,
					pantryc_global__working_directory);
		}
	}

	return pantryc_milter__cleanup(context, pTRUE);
}

sfsistat pantryc_milter__xxfi_abort(context)
	SMFICTX *context; {
	return pantryc_milter__cleanup(context, pFALSE);
}

sfsistat pantryc_milter__xxfi_close(context)
	SMFICTX *context; {
	PantrycData *data = PANTRYC_MILTER__GET_PRIVATE_DATA;

	if (data == NULL)
		return SMFIS_CONTINUE;
	else
		pantryc_milter__free_pantrycData(data);
	smfi_setpriv(context, NULL);
	g_mime_shutdown();
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_unknown(context, command)
	SMFICTX *context;const char *command; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_data(context)
	SMFICTX *context; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_negotiate(context, f0, f1, f2, f3, pf0, pf1, pf2,
		pf3)
	SMFICTX *context;unsigned long f0;unsigned long f1;unsigned long f2;unsigned long f3;unsigned long *pf0;unsigned long *pf1;unsigned long *pf2;unsigned long *pf3; {
	return SMFIS_ALL_OPTS;
}

/* Private functions */

static sfsistat pantryc_milter__cleanup(context, ok)
	SMFICTX *context;pBOOL ok; {
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
		fprintf(pantryc_global__log_file, "Message aborted. Removing file\n");
		status = SMFIS_TEMPFAIL;
	}

	/* close the archive file */
	if (data->mail != NULL && fclose(data->mail) == EOF) {
		/* failed; we have to wait until later */
		fprintf(pantryc_global__log_file,
				"Couldn't close archive file (error: %s)\n", strerror(errno));
		status = SMFIS_TEMPFAIL;
	}
	/* return status */
	return status;
}

static void pantryc_milter__free_pantrycData(data)
	PantrycData *data; {
	if (data->buffer != NULL)
		free(data->buffer);
	if (data->connectfrom != NULL)
		free(data->connectfrom);
	if (data->helofrom != NULL)
		free(data->helofrom);
	free(data);
}

static void pantryc_milter__write_message_to_log(data, message)
	PantrycData *data;GMimeMessage *message; {
	const gchar *value;
	fprintf(pantryc_global__log_file, "From:\t\t%s\n",
			g_mime_message_get_sender(message));

	value = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_TO);
	fprintf(pantryc_global__log_file, "To:\t\t\t%s\n",
			value ? value : "<none>");

	value = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_CC);
	fprintf(pantryc_global__log_file, "Cc:\t\t\t%s\n",
			value ? value : "<none>");

	value = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_BCC);
	fprintf(pantryc_global__log_file, "Bcc:\t\t%s\n", value ? value : "<none>");

	value = g_mime_message_get_subject(message);
	fprintf(pantryc_global__log_file, "Subject:\t%s\n",
			value ? value : "<none>");

	value = pantryc_scanner__get_date(message);
	fprintf(pantryc_global__log_file, "Date:\t\t%s\n", value);

	value = pantryc_scanner__get_content(message, 0);
	fprintf(pantryc_global__log_file, "Message:\t\"%s\"\n", value);

	fprintf(pantryc_global__log_file, "Score:\t\t%d\n",
			pantryc_milter__calculate_score_content((gchar*) value));

	value = g_mime_message_get_message_id(message);
	fprintf(pantryc_global__log_file, "Msg-id:\t\t%s\n",
			value ? value : "<none>");

	value = pantryc_scanner__get_references(message);
	fprintf(pantryc_global__log_file, "Refs:\t\t%s\n",
			value ? value : "<none>");
}

static int pantryc_milter__calculate_score_content(gchar *content) {
	int score = 0;
	int i;
	char lastchar;
	char *previous;
	char *seeker = content;

	previous = seeker;
	lastchar = ' ';
	i = 0;
	do {
		if (isspace(seeker[i])) {
			lastchar = seeker[i];
			seeker[i] = '\0';
			score += pantryc_sqlite__get_score_bad_word(previous);
		} else {
			if (isspace(lastchar))
				previous = seeker + i;
			lastchar = seeker[i];
		}
		i++;
		if (seeker[i] == '\0') { // last word
			score += pantryc_sqlite__get_score_bad_word(previous);
			break;
		}
	} while (1);

	return score;
}

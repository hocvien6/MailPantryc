#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <gmime/gmime.h>

#include "../include/pantryc-milter.h"

#define GET_PANTRYC_PRIVATE_DATA			((pantrycData *) smfi_getpriv(ctx))

sfsistat pantryc_milter__cleanup(ctx, ok)
	SMFICTX *ctx;bool ok; {
	sfsistat rstat = SMFIS_CONTINUE;
	pantrycData *data = GET_PANTRYC_PRIVATE_DATA;
	char host[512];
	if (data == NULL)
		return rstat;
	// close the archive file
	if (ok) {
		// add a header to the message announcing our presence
		if (gethostname(host, sizeof host) < 0)
			snprintf(host, sizeof host, "localhost");
	} else {
		// message was aborted -- delete the archive file
		fprintf(data->log, "Message aborted. Removing file\n");
		rstat = SMFIS_TEMPFAIL;
	}

	/* return status */
	return rstat;
}

sfsistat pantryc_milter__xxfi_connect(ctx, hostname, hostaddr)
	SMFICTX *ctx;char *hostname;_SOCK_ADDR *hostaddr; {
	pantrycData *data;
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

	// TODO rename log file
	char *logfullname;
	logfullname = (char*) malloc(100 * sizeof(char));
	strcpy(logfullname, pantryc__working_directory);
	strcat(logfullname, "log.txt");
	data->log = fopen(logfullname, "w+");
	if (data->log == NULL)
		exit(EX_NOINPUT);
	////

	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_helo(ctx, helohost)
	SMFICTX *ctx;char *helohost; {
	size_t len;
	char *tls;
	char *buf;
	pantrycData *data = GET_PANTRYC_PRIVATE_DATA;
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
	printf("%s\n",data->helofrom); // TESTING
	if ((data->mail = open_memstream(&data->buffer, &data->size)) == NULL) {
		(void) fclose(data->mail);
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
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
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;

	// TESTING
	while (*argv++ != NULL)
		printf("%s\n", *argv);
	////

	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envrcpt(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_header(ctx, headerf, headerv)
	SMFICTX *ctx;char *headerf;char *headerv; {
	/* write the header to the mail file */
	if (fprintf(GET_PANTRYC_PRIVATE_DATA->mail, "%s: %s\n", headerf,
			headerv) == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eoh(ctx)
	SMFICTX *ctx; {
	/* output the blank line between the header and the body */
	if (fprintf(GET_PANTRYC_PRIVATE_DATA->mail, "\n") == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_body(ctx, bodyp, bodylen)
	SMFICTX *ctx;unsigned char *bodyp;size_t bodylen; {
	pantrycData *data = GET_PANTRYC_PRIVATE_DATA;
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
	bool ok = TRUE;
	return pantryc_milter__cleanup(ctx, ok);
}

sfsistat pantryc_milter__xxfi_abort(ctx)
	SMFICTX *ctx; {
	return pantryc_milter__cleanup(ctx, FALSE);
}

sfsistat pantryc_milter__xxfi_close(ctx)
	SMFICTX *ctx; {
	pantrycData *data = GET_PANTRYC_PRIVATE_DATA;

	rewind(data->mail);

	g_mime_init(GMIME_ENABLE_RFC2047_WORKAROUNDS);
	GMimeStream *stream;
	stream = g_mime_stream_file_new(data->mail);
	//stream = g_mime_stream_file_new(file);
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
	refsstr = pantryc_scanner__get_refs_str(message);
	fprintf(data->log, "Refs   : %s\n", refsstr ? refsstr : "<none>");
	g_free(refsstr);

	int number_of_parts = g_mime_multipart_get_count(
			(GMimeMultipart*) message->mime_part);
	data->permission = pantryc__attachment_permission;
	int i;
	for (i = 0; i <= number_of_parts - 1; i++) {
		pantryc_scanner__extract_attachment(message, i, data->permission,
				pantryc__working_directory);
	}
	g_mime_shutdown();

	if (data == NULL)
		return SMFIS_CONTINUE;
	if (data->connectfrom != NULL)
		free(data->connectfrom);
	if (data->helofrom != NULL)
		free(data->helofrom);
	free(data);
	smfi_setpriv(ctx, NULL);

	if (data->log == NULL || fclose(data->log) == EOF) {
		fprintf(data->log, "Couldn't close log file (error: %s)\n",
				strerror(errno));
	}

	// TESTING
	char c;
	while ((c = fgetc(data->mail)) != EOF)
		printf("%c", c);
	////

	/* close the archive file */
	if (data->mail != NULL && fclose(data->mail) == EOF) {
		/* failed; we have to wait until later */
		fprintf(data->log, "Couldn't close archive file (error: %s)\n",
				strerror(errno));
		return SMFIS_TEMPFAIL;
	}
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

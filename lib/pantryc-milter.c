#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <gmime/gmime.h>

#include "../include/pantryc-milter.h"

#define MLFIPRIV			((pantrycPriv *) smfi_getpriv(ctx))

sfsistat pantryc_milter__cleanup(ctx, ok)
	SMFICTX *ctx;bool ok; {
	sfsistat rstat = SMFIS_CONTINUE;
	pantrycPriv *priv = MLFIPRIV;
	char host[512];
	if (priv == NULL)
		return rstat;
	// close the archive file
	if (ok) {
		// add a header to the message announcing our presence
		if (gethostname(host, sizeof host) < 0)
			snprintf(host, sizeof host, "localhost");
	} else {
		// message was aborted -- delete the archive file
		fprintf(stderr, "Message aborted. Removing file\n");
		rstat = SMFIS_TEMPFAIL;
	}

	/* return status */
	return rstat;
}

sfsistat pantryc_milter__xxfi_connect(ctx, hostname, hostaddr)
	SMFICTX *ctx;char *hostname;_SOCK_ADDR *hostaddr; {
	pantrycPriv *priv;
	char *ident;
	/* allocate some private memory */
	priv = malloc(sizeof *priv);
	if (priv == NULL) {
		/* can't accept this message right now */
		return SMFIS_TEMPFAIL;
	}
	memset(priv, '\0', sizeof *priv);
	/* save the private data */
	smfi_setpriv(ctx, priv);
	ident = smfi_getsymval(ctx, "_");
	if (ident == NULL)
		ident = "???";
	if ((priv->connectfrom = strdup(ident)) == NULL) {
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
	pantrycPriv *priv = MLFIPRIV;
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
	if (priv->helofrom != NULL)
		free(priv->helofrom);
	priv->helofrom = buf;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_envfrom(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	pantrycPriv *priv = MLFIPRIV;

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
	if ((priv->pantryc_fname = strdup(_tempnam(szFilename, "msgPantryc."))) == NULL)
	{
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
#else /* WIN32 */

#endif /* WIN32 */
	if ((priv->file = open_memstream(&priv->buffer, &priv->size)) == NULL) {
		(void) fclose(priv->file);
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
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
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_header(ctx, headerf, headerv)
	SMFICTX *ctx;char *headerf;char *headerv; {
	/* write the header to the log file */
	if (fprintf(MLFIPRIV->file, "%s: %s\n", headerf, headerv) == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_eoh(ctx)
	SMFICTX *ctx; {
	/* output the blank line between the header and the body */
	if (fprintf(MLFIPRIV->file, "\n") == EOF) {
		(void) pantryc_milter__cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_milter__xxfi_body(ctx, bodyp, bodylen)
	SMFICTX *ctx;unsigned char *bodyp;size_t bodylen; {
	pantrycPriv *priv = MLFIPRIV;
	/* output body block to log file */
	if (fwrite(bodyp, bodylen, 1, priv->file) != 1) {
		/* write failed */
		fprintf(stderr, "Couldn't write file (error: %s)\n", strerror(errno));
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
	pantrycPriv *priv = MLFIPRIV;

	rewind(priv->file);

	g_mime_init(GMIME_ENABLE_RFC2047_WORKAROUNDS);
	GMimeStream *stream;
	stream = g_mime_stream_file_new(priv->file);
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
	g_print("From   : %s\n", g_mime_message_get_sender(message));

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_TO);
	g_print("To     : %s\n", val ? val : "<none>");
	g_free(val);

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_CC);
	g_print("Cc     : %s\n", val ? val : "<none>");
	g_free(val);

	val = pantryc_scanner__get_recip(message, GMIME_RECIPIENT_TYPE_BCC);
	g_print("Bcc    : %s\n", val ? val : "<none>");
	g_free(val);

	str = g_mime_message_get_subject(message);
	g_print("Subject: %s\n", str ? str : "<none>");

	val = pantryc_scanner__print_date(message);
	g_print("Date   : %s\n", val);

	str = g_mime_message_get_message_id(message);
	g_print("Msg-id : %s\n", str ? str : "<none>");

	{
		gchar *refsstr;
		refsstr = pantryc_scanner__get_refs_str(message);
		g_print("Refs   : %s\n", refsstr ? refsstr : "<none>");
		g_free(refsstr);
	}

	// TESTING
	int number_of_parts = g_mime_multipart_get_count(
			(GMimeMultipart*) message->mime_part);

	int i;
	for (i = 0; i <= number_of_parts - 1; i++) {
		pantryc_scanner__extract_attachment(message, i,
				pantryc__attachment_permission, pantryc__working_directory);
	}
	////

	g_mime_shutdown();

	if (priv == NULL)
		return SMFIS_CONTINUE;
	if (priv->connectfrom != NULL)
		free(priv->connectfrom);
	if (priv->helofrom != NULL)
		free(priv->helofrom);
	free(priv);
	smfi_setpriv(ctx, NULL);

	/* close the archive file */
	if (priv->file != NULL && fclose(priv->file) == EOF) {
		/* failed; we have to wait until later */
		fprintf(stderr, "Couldn't close archive file (error: %s)\n",
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

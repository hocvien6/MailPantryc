#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <libmilter/mfapi.h>
#include <gmime/gmime.h>
#include <gmime/gmime-multipart.h>
#include <glib.h>

struct mlfiPriv {
	char *buffer;
	size_t size;
	char *pantryc_connectfrom;
	char *pantryc_helofrom;
	FILE *pantryc_fp;
};

#define MLFIPRIV        ((struct mlfiPriv *) smfi_getpriv(ctx))

extern sfsistat pantryc_cleanup(SMFICTX *, bool);
/* recipients to add and reject (set with -a and -r options) */
char *add = NULL;
char *reject = NULL;

sfsistat pantryc_connect(ctx, hostname, hostaddr)
	SMFICTX *ctx;char *hostname;_SOCK_ADDR *hostaddr; {
	struct mlfiPriv *priv;
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
	if ((priv->pantryc_connectfrom = strdup(ident)) == NULL) {
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_helo(ctx, helohost)
	SMFICTX *ctx;char *helohost; {
	size_t len;
	char *tls;
	char *buf;
	struct mlfiPriv *priv = MLFIPRIV;
	tls = smfi_getsymval(ctx, "{tls_version}");
	if (tls == NULL)
		tls = "No TLS";
	if (helohost == NULL)
		helohost = "???";
	len = strlen(tls) + strlen(helohost) + 3;
	if ((buf = (char*) malloc(len)) == NULL) {
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	snprintf(buf, len, "%s, %s", helohost, tls);
	if (priv->pantryc_helofrom != NULL)
		free(priv->pantryc_helofrom);
	priv->pantryc_helofrom = buf;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_envfrom(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	struct mlfiPriv *priv = MLFIPRIV;

#ifdef WIN32
	char szFilename[MAX_PATH]= {0};
#endif /* WIN32 */
	/* open a file to store this message */
#ifdef WIN32
	if (GetTempPath(MAX_PATH, szFilename) == 0)
	{
		/* This shouldn't fail */
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	if ((priv->pantryc_fname = strdup(_tempnam(szFilename, "msgPantryc."))) == NULL)
	{
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
#else /* WIN32 */

#endif /* WIN32 */
	if ((priv->pantryc_fp = open_memstream(&priv->buffer, &priv->size)) == NULL) {
		(void) fclose(priv->pantryc_fp);
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_envrcpt(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_header(ctx, headerf, headerv)
	SMFICTX *ctx;char *headerf;unsigned char *headerv; {
	/* write the header to the log file */
	if (fprintf(MLFIPRIV->pantryc_fp, "%s: %s\n", headerf, headerv) == EOF) {
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_eoh(ctx)
	SMFICTX *ctx; {
	/* output the blank line between the header and the body */
	if (fprintf(MLFIPRIV->pantryc_fp, "\n") == EOF) {
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_body(ctx, bodyp, bodylen)
	SMFICTX *ctx;unsigned char *bodyp;size_t bodylen; {
	struct mlfiPriv *priv = MLFIPRIV;
	/* output body block to log file */
	if (fwrite(bodyp, bodylen, 1, priv->pantryc_fp) != 1) {
		/* write failed */
		fprintf(stderr, "Couldn't write file (error: %s)\n", strerror(errno));
		(void) pantryc_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat pantryc_eom(ctx)
	SMFICTX *ctx; {
	bool ok = TRUE;
	/* change recipients, if requested */
	if (add != NULL)
		ok = (smfi_addrcpt(ctx, add) == MI_SUCCESS);
	return pantryc_cleanup(ctx, ok);
}

sfsistat pantryc_abort(ctx)
	SMFICTX *ctx; {
	return pantryc_cleanup(ctx, FALSE);
}

sfsistat pantryc_cleanup(ctx, ok)
	SMFICTX *ctx;
	bool ok; {
	sfsistat rstat = SMFIS_CONTINUE;
	struct mlfiPriv *priv = MLFIPRIV;
	char host[512];
	if (priv == NULL)
		return rstat;
	/* close the archive file */
	if (priv->pantryc_fp != NULL && fclose(priv->pantryc_fp) == EOF) {
		/* failed; we have to wait until later */
		fprintf(stderr, "Couldn't close archive file (error: %s)\n",
				strerror(errno));
		rstat = SMFIS_TEMPFAIL;
	} else if (ok) {
		/* add a header to the message announcing our presence */
		if (gethostname(host, sizeof host) < 0)
			snprintf(host, sizeof host, "localhost");
	} else {
		/* message was aborted -- delete the archive file */
		fprintf(stderr, "Message aborted. Removing file\n");
		rstat = SMFIS_TEMPFAIL;
	}
	/* return status */
	return rstat;
}

sfsistat pantryc_close(ctx)
	SMFICTX *ctx; {
	struct mlfiPriv *priv = MLFIPRIV;

	// DEBUG
	//char c;
	//while((c = fgetc(priv->pantryc_fp))!=EOF) printf("%c",c);
	rewind(priv->pantryc_fp);

	g_mime_init(GMIME_ENABLE_RFC2047_WORKAROUNDS);
	GMimeStream *stream;
	stream = g_mime_stream_file_new(priv->pantryc_fp);
	if (!stream) {
		g_warning("Cannot open stream");
	}

	GMimeParser *parser;
	GMimeMessage *msg;
	parser = NULL;
	msg = NULL;
	parser = g_mime_parser_new_with_stream(stream);
	if (!parser) {
		g_warning("failed to create parser");
	}
	msg = g_mime_parser_construct_message(parser);
	if (!msg) {
		g_warning("failed to construct message");
	}

	//gchar *val;
	//const gchar *str;

	g_print("From   : %s\n", g_mime_message_get_sender(msg));
/*
	val = get_recip(msg, GMIME_RECIPIENT_TYPE_TO);
	g_print("To     : %s\n", val ? val : "<none>");
	g_free(val);

	val = get_recip(msg, GMIME_RECIPIENT_TYPE_CC);
	g_print("Cc     : %s\n", val ? val : "<none>");
	g_free(val);

	val = get_recip(msg, GMIME_RECIPIENT_TYPE_BCC);
	g_print("Bcc    : %s\n", val ? val : "<none>");
	g_free(val);

	str = g_mime_message_get_subject(msg);
	g_print("Subject: %s\n", str ? str : "<none>");

	print_date(msg);

	str = g_mime_message_get_message_id(msg);
	g_print("Msg-id : %s\n", str ? str : "<none>");

	{
		gchar *refsstr;
		refsstr = get_refs_str(msg);
		g_print("Refs   : %s\n", refsstr ? refsstr : "<none>");
		g_free(refsstr);
	}
*/
	GMimeMultipart *m = (GMimeMultipart*)msg->mime_part;
	printf("Part: %d\n",g_mime_multipart_get_count(m));
	GMimeObject *o = g_mime_multipart_get_part(m,0);
	printf("Part: %s\n",g_mime_content_disposition_get_disposition(g_mime_object_get_content_disposition(o)));

	g_mime_shutdown();
	////

	if (priv == NULL)
		return SMFIS_CONTINUE;
	if (priv->pantryc_connectfrom != NULL)
		free(priv->pantryc_connectfrom);
	if (priv->pantryc_helofrom != NULL)
		free(priv->pantryc_helofrom);
	free(priv);
	smfi_setpriv(ctx, NULL);
	return SMFIS_CONTINUE;
}

sfsistat pantryc_unknown(ctx, cmd)
	SMFICTX *ctx;char *cmd; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_data(ctx)
	SMFICTX *ctx; {
	return SMFIS_CONTINUE;
}

sfsistat pantryc_negotiate(ctx, f0, f1, f2, f3, pf0, pf1, pf2, pf3)
	SMFICTX *ctx;unsigned long f0;unsigned long f1;unsigned long f2;unsigned long f3;unsigned long *pf0;unsigned long *pf1;unsigned long *pf2;unsigned long *pf3; {
	return SMFIS_ALL_OPTS;
}

struct smfiDesc smfilter = { "PantrycMilter", /* filter name */
SMFI_VERSION, /* version code -- do not change */
SMFIF_ADDHDRS | SMFIF_ADDRCPT,
/* flags */
pantryc_connect, /* connection info filter */
pantryc_helo, /* SMTP HELO command filter */
pantryc_envfrom, /* envelope sender filter */
pantryc_envrcpt, /* envelope recipient filter */
pantryc_header, /* header filter */
pantryc_eoh, /* end of header */
pantryc_body, /* body block filter */
pantryc_eom, /* end of message */
pantryc_abort, /* message aborted */
pantryc_close, /* connection cleanup */
pantryc_unknown, /* unknown SMTP commands */
pantryc_data, /* DATA command */
pantryc_negotiate /* Once, at the start of each SMTP connection */
};

static void usage(prog)
	char *prog; {
	fprintf(stderr,
			"Usage: %s -p socket-addr [-t timeout] [-r reject-addr] [-a add-addr]\n",
			prog);
}

int main(argc, argv)
	int argc;char **argv; {
	bool setconn = FALSE;
	int c;
	const char *args = "p:t:r:a:h";
	extern char *optarg;
	/* Process command line options */
	while ((c = getopt(argc, argv, args)) != -1) {
		switch (c) {
		case 'p':
			if (optarg == NULL || *optarg == '\0') {
				(void) fprintf(stderr, "Illegal conn: %s\n", optarg);
				exit(EX_USAGE);
			}
			if (smfi_setconn(optarg) == MI_FAILURE) {
				(void) fprintf(stderr, "smfi_setconn failed\n");
				exit(EX_SOFTWARE);
			}
			/*
			 **  If we're using a local socket, make sure it
			 **  doesn't already exist.  Don't ever run this
			 **  code as root!!
			 */
			if (strncasecmp(optarg, "unix:", 5) == 0)
				unlink(optarg + 5);
			else if (strncasecmp(optarg, "local:", 6) == 0)
				unlink(optarg + 6);
			setconn = TRUE;
			break;
		case 't':
			if (optarg == NULL || *optarg == '\0') {
				(void) fprintf(stderr, "Illegal timeout: %s\n", optarg);
				exit(EX_USAGE);
			}
			if (smfi_settimeout(atoi(optarg)) == MI_FAILURE) {
				(void) fprintf(stderr, "smfi_settimeout failed\n");
				exit(EX_SOFTWARE);
			}
			break;
		case 'r':
			if (optarg == NULL) {
				(void) fprintf(stderr, "Illegal reject rcpt: %s\n", optarg);
				exit(EX_USAGE);
			}
			reject = optarg;
			break;
		case 'a':
			if (optarg == NULL) {
				(void) fprintf(stderr, "Illegal add rcpt: %s\n", optarg);
				exit(EX_USAGE);
			}
			add = optarg;
			smfilter.xxfi_flags |= SMFIF_ADDRCPT;
			break;
		case 'h':
		default:
			usage(argv[0]);
			exit(EX_USAGE);
		}
	}
	if (!setconn) {
		fprintf(stderr, "%s: Missing required -p argument\n", argv[0]);
		usage(argv[0]);
		exit(EX_USAGE);
	}
	if (smfi_register(smfilter) == MI_FAILURE) {

		fprintf(stderr, "smfi_register failed\n");
		exit(EX_UNAVAILABLE);
	}
	return smfi_main();
}

/* eof */

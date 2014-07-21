#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <libmilter/mfapi.h>

#ifndef bool
# define bool   int
# define TRUE   1
# define FALSE  0
#endif /* ! bool */

struct mlfiPriv {
	char *xxfi_fname;
	char *xxfi_connectfrom;
	char *xxfi_helofrom;
	FILE *xxfi_fp;
};

#define MLFIPRIV        ((struct mlfiPriv *) smfi_getpriv(ctx))

extern sfsistat xxfi_cleanup(SMFICTX *, bool);
/* recipients to add and reject (set with -a and -r options) */
char *add = NULL;
char *reject = NULL;

sfsistat xxfi_connect(ctx, hostname, hostaddr)
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
	if ((priv->xxfi_connectfrom = strdup(ident)) == NULL) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_helo(ctx, helohost)
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
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	snprintf(buf, len, "%s, %s", helohost, tls);
	if (priv->xxfi_helofrom != NULL)
		free(priv->xxfi_helofrom);
	priv->xxfi_helofrom = buf;
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_envfrom(ctx, argv)
	SMFICTX *ctx;char **argv; {
	int argc = 0;
	struct mlfiPriv *priv = MLFIPRIV;
	char *mailaddr = smfi_getsymval(ctx, "{mail_addr}");

#ifdef WIN32
	char szFilename[MAX_PATH]= {0};
#endif /* WIN32 */
	/* open a file to store this message */
#ifdef WIN32
	if (GetTempPath(MAX_PATH, szFilename) == 0)
	{
		/* This shouldn't fail */
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	if ((priv->xxfi_fname = strdup(_tempnam(szFilename, "msgPantryc."))) == NULL)
	{
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
#else /* WIN32 */
	if ((priv->xxfi_fname = strdup("/tmp/msgPantryc.txt")) == NULL) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}

#endif /* WIN32 */
	if ((priv->xxfi_fp = fopen(priv->xxfi_fname, "w+")) == NULL) {
		(void) fclose(priv->xxfi_fp);
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* log the connection information we stored earlier: */
	if (fprintf(priv->xxfi_fp, "Connect from %s (%s)\n\n", priv->xxfi_helofrom,
			priv->xxfi_connectfrom) == EOF) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* log the sender */
	if (fprintf(priv->xxfi_fp, "FROM %s (%d argument%s)\n",
			mailaddr ? mailaddr : "???", argc, (argc == 1) ? "" : "s") == EOF) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_envrcpt(ctx, argv)
	SMFICTX *ctx;char **argv; {
	struct mlfiPriv *priv = MLFIPRIV;
	char *rcptaddr = smfi_getsymval(ctx, "{rcpt_addr}");
	int argc = 0;
	/* count the arguments */
	while (*argv++ != NULL)
		++argc;
	/* log this recipient */
	if (reject != NULL && rcptaddr != NULL
			&& (strcasecmp(rcptaddr, reject) == 0)) {
		if (fprintf(priv->xxfi_fp, "RCPT %s -- REJECTED\n", rcptaddr) == EOF) {
			(void) xxfi_cleanup(ctx, FALSE);
			return SMFIS_TEMPFAIL;
		}
		return SMFIS_REJECT;
	}

	if (fprintf(priv->xxfi_fp, "RCPT %s (%d argument%s)\n",
			rcptaddr ? rcptaddr : "???", argc, (argc == 1) ? "" : "s") == EOF) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_header(ctx, headerf, headerv)
	SMFICTX *ctx;char *headerf;unsigned char *headerv; {
	/* write the header to the log file */
	if (fprintf(MLFIPRIV->xxfi_fp, "%s: %s\n", headerf, headerv) == EOF) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_eoh(ctx)
	SMFICTX *ctx; {
	/* output the blank line between the header and the body */
	if (fprintf(MLFIPRIV->xxfi_fp, "\n") == EOF) {
		(void) xxfi_cleanup(ctx, FALSE);
		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_body(ctx, bodyp, bodylen)
	SMFICTX *ctx;unsigned char *bodyp;size_t bodylen; {
	struct mlfiPriv *priv = MLFIPRIV;
	/* output body block to log file */
	printf("%s\n",bodyp); //DEBUG
	if (fwrite(bodyp, bodylen, 1, priv->xxfi_fp) != 1) {
		/* write failed */
		fprintf(stderr, "Couldn't write file %s: %s\n", priv->xxfi_fname,
				strerror(errno));
		(void) xxfi_cleanup(ctx, FALSE);

		return SMFIS_TEMPFAIL;
	}
	/* continue processing */
	return SMFIS_CONTINUE;
}

sfsistat xxfi_eom(ctx)
	SMFICTX *ctx; {
	bool ok = TRUE;
	/* change recipients, if requested */
	if (add != NULL)
		ok = (smfi_addrcpt(ctx, add) == MI_SUCCESS);
	return xxfi_cleanup(ctx, ok);
}

sfsistat xxfi_abort(ctx)
	SMFICTX *ctx; {
	return xxfi_cleanup(ctx, FALSE);
}

sfsistat xxfi_cleanup(ctx, ok)
	SMFICTX *ctx;
	bool ok; {
	sfsistat rstat = SMFIS_CONTINUE;
	struct mlfiPriv *priv = MLFIPRIV;
	char *p;
	char host[512];
	char hbuf[1024];
	if (priv == NULL)
		return rstat;
	/* close the archive file */
	if (priv->xxfi_fp != NULL && fclose(priv->xxfi_fp) == EOF) {
		/* failed; we have to wait until later */
		fprintf(stderr, "Couldn't close archive file %s: %s\n",
				priv->xxfi_fname, strerror(errno));
		rstat = SMFIS_TEMPFAIL;
		(void) unlink(priv->xxfi_fname);
	} else if (ok) {
		/* add a header to the message announcing our presence */
		if (gethostname(host, sizeof host) < 0)
			snprintf(host, sizeof host, "localhost");
		p = strrchr(priv->xxfi_fname, '/');
		if (p == NULL)
			p = priv->xxfi_fname;
		else
			p++;
		snprintf(hbuf, sizeof hbuf, "%s@%s", p, host);
		if (smfi_addheader(ctx, "X-Archived", hbuf) != MI_SUCCESS) {
			/* failed; we have to wait until later */
			fprintf(stderr, "Couldn't add header: X-Archived: %s\n", hbuf);
			ok = FALSE;
			rstat = SMFIS_TEMPFAIL;
			(void) unlink(priv->xxfi_fname);
		}
	} else {
		/* message was aborted -- delete the archive file */
		fprintf(stderr, "Message aborted.  Removing %s\n", priv->xxfi_fname);
		rstat = SMFIS_TEMPFAIL;
		(void) unlink(priv->xxfi_fname);
	}
	/* release private memory */
	if (priv->xxfi_fname != NULL)
		free(priv->xxfi_fname);
	/* return status */
	return rstat;
}

sfsistat xxfi_close(ctx)
	SMFICTX *ctx; {
	struct mlfiPriv *priv = MLFIPRIV;
	if (priv == NULL)
		return SMFIS_CONTINUE;
	if (priv->xxfi_connectfrom != NULL)
		free(priv->xxfi_connectfrom);
	if (priv->xxfi_helofrom != NULL)
		free(priv->xxfi_helofrom);
	free(priv);
	smfi_setpriv(ctx, NULL);
	return SMFIS_CONTINUE;
}

sfsistat xxfi_unknown(ctx, cmd)
	SMFICTX *ctx;char *cmd; {
	return SMFIS_CONTINUE;
}

sfsistat xxfi_data(ctx)
	SMFICTX *ctx; {
	return SMFIS_CONTINUE;
}

sfsistat xxfi_negotiate(ctx, f0, f1, f2, f3, pf0, pf1, pf2, pf3)
	SMFICTX *ctx;unsigned long f0;unsigned long f1;unsigned long f2;unsigned long f3;unsigned long *pf0;unsigned long *pf1;unsigned long *pf2;unsigned long *pf3; {
	return SMFIS_ALL_OPTS;
}

struct smfiDesc smfilter = { "SampleFilter", /* filter name */
SMFI_VERSION, /* version code -- do not change */
SMFIF_ADDHDRS | SMFIF_ADDRCPT,
/* flags */
xxfi_connect, /* connection info filter */
xxfi_helo, /* SMTP HELO command filter */
xxfi_envfrom, /* envelope sender filter */
xxfi_envrcpt, /* envelope recipient filter */
xxfi_header, /* header filter */
xxfi_eoh, /* end of header */
xxfi_body, /* body block filter */
xxfi_eom, /* end of message */
xxfi_abort, /* message aborted */
xxfi_close, /* connection cleanup */
xxfi_unknown, /* unknown SMTP commands */
xxfi_data, /* DATA command */
xxfi_negotiate /* Once, at the start of each SMTP connection */
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

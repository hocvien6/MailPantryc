#ifndef PANTRYC_MILTER_H_
#define PANTRYC_MILTER_H_

#include <libmilter/mfapi.h>
#include <pantryc-scanner.h>

char *pantryc__working_directory;
int pantryc__attachment_permission;

typedef struct pantrycData {
	char *buffer;
	size_t size;
	char *connectfrom;
	char *helofrom;
	FILE *mail;
	FILE *log;
	int permission; /* attachment permission */
} pantrycData;

/**
 * #Function_in_pantryc_milter_structure
 * Connection info filter
 */
sfsistat pantryc_milter__xxfi_connect(SMFICTX *ctx, char *hostname,
_SOCK_ADDR *hostaddr);

/**
 * #Function_in_pantryc_milter_structure
 * SMTP HELO command filter
 */
sfsistat pantryc_milter__xxfi_helo(SMFICTX *ctx, char *helohost);

/**
 * #Function_in_pantryc_milter_structure
 * Envelope sender filter
 */
sfsistat pantryc_milter__xxfi_envfrom(SMFICTX *ctx, char **argv);

/**
 * #Function_in_pantryc_milter_structure
 * Envelope recipient filter
 */
sfsistat pantryc_milter__xxfi_envrcpt(SMFICTX *ctx, char **argv);

/**
 * #Function_in_pantryc_milter_structure
 * Header filter
 */
sfsistat pantryc_milter__xxfi_header(SMFICTX *ctx, char *name, char *value);

/**
 * #Function_in_pantryc_milter_structure
 * End of header
 */
sfsistat pantryc_milter__xxfi_eoh(SMFICTX *ctxx);

/**
 * #Function_in_pantryc_milter_structure
 * Body block
 */
sfsistat pantryc_milter__xxfi_body(SMFICTX *ctx, unsigned char *bodyp,
		size_t bodylen);

/**
 * #Function_in_pantryc_milter_structure
 * End of message
 */
sfsistat pantryc_milter__xxfi_eom(SMFICTX *ctx);

/**
 * #Function_in_pantryc_milter_structure
 * Message abort
 */
sfsistat pantryc_milter__xxfi_abort(SMFICTX *ctx);

/**
 * #Function_in_pantryc_milter_structure
 * Connection cleanup
 */
sfsistat pantryc_milter__xxfi_close(SMFICTX *ctx);

/**
 * #Function_in_pantryc_milter_structure
 * Any unrecognized or unimplemented command filter
 */
sfsistat pantryc_milter__xxfi_unknown(SMFICTX *ctx, const char *cmd);

/**
 * #Function_in_pantryc_milter_structure
 * SMTP DATA command filter
 */
sfsistat pantryc_milter__xxfi_data(SMFICTX *ctx);

/**
 * #Function_in_pantryc_milter_structure
 * Negotiation callback
 */
sfsistat pantryc_milter__xxfi_negotiate(SMFICTX *ctx, unsigned long f0,
		unsigned long f1, unsigned long f2, unsigned long f3,
		unsigned long *pf0, unsigned long *pf1, unsigned long *pf2,
		unsigned long *pf3);

#endif /* PANTRYC_MILTER_H_ */

#ifndef PANTRYC_MILTER_H_
#define PANTRYC_MILTER_H_

#include <libmilter/mfapi.h>

#include <pantryc-util.h>

/**
 * Connection info filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_connect(SMFICTX *context, char *hostname,
_SOCK_ADDR *hostaddr);

/**
 * SMTP HELO command filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_helo(SMFICTX *context, char *helohost);

/**
 * Envelope sender filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_envfrom(SMFICTX *context, char **argv);

/**
 * Envelope recipient filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_envrcpt(SMFICTX *context, char **argv);

/**
 * Header filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_header(SMFICTX *context, char *name, char *value);

/**
 * End of header
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_eoh(SMFICTX *context);

/**
 * Body block
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_body(SMFICTX *context, unsigned char *bodyp,
		size_t bodylen);

/**
 * End of message
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_eom(SMFICTX *context);

/**
 * Message abort
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_abort(SMFICTX *context);

/**
 * Connection cleanup
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_close(SMFICTX *context);

/**
 * Any unrecognized or unimplemented command filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_unknown(SMFICTX *context, const char *command);

/**
 * SMTP DATA command filter
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_data(SMFICTX *context);

/**
 * Negotiation callback
 */
$pTag_milter
sfsistat pantryc_milter__xxfi_negotiate(SMFICTX *context, unsigned long f0,
		unsigned long f1, unsigned long f2, unsigned long f3,
		unsigned long *pf0, unsigned long *pf1, unsigned long *pf2,
		unsigned long *pf3);

#endif /* PANTRYC_MILTER_H_ */

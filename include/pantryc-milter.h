#ifndef PANTRYC_MILTER_H_
#define PANTRYC_MILTER_H_

#include <libmilter/mfapi.h>

#include <pantryc-scanner.h>
#include <pantryc-sqlite.h>

/**
 * #Pantryc_milter_structure_function
 * Connection info filter
 */
sfsistat pantryc_milter__xxfi_connect(SMFICTX *context, char *hostname,
_SOCK_ADDR *hostaddr);

/**
 * #Pantryc_milter_structure_function
 * SMTP HELO command filter
 */
sfsistat pantryc_milter__xxfi_helo(SMFICTX *context, char *helohost);

/**
 * #Pantryc_milter_structure_function
 * Envelope sender filter
 */
sfsistat pantryc_milter__xxfi_envfrom(SMFICTX *context, char **argv);

/**
 * #Pantryc_milter_structure_function
 * Envelope recipient filter
 */
sfsistat pantryc_milter__xxfi_envrcpt(SMFICTX *context, char **argv);

/**
 * #Pantryc_milter_structure_function
 * Header filter
 */
sfsistat pantryc_milter__xxfi_header(SMFICTX *context, char *name, char *value);

/**
 * #Pantryc_milter_structure_function
 * End of header
 */
sfsistat pantryc_milter__xxfi_eoh(SMFICTX *context);

/**
 * #Pantryc_milter_structure_function
 * Body block
 */
sfsistat pantryc_milter__xxfi_body(SMFICTX *context, unsigned char *bodyp,
		size_t bodylen);

/**
 * #Pantryc_milter_structure_function
 * End of message
 */
sfsistat pantryc_milter__xxfi_eom(SMFICTX *context);

/**
 * #Pantryc_milter_structure_function
 * Message abort
 */
sfsistat pantryc_milter__xxfi_abort(SMFICTX *context);

/**
 * #Pantryc_milter_structure_function
 * Connection cleanup
 */
sfsistat pantryc_milter__xxfi_close(SMFICTX *context);

/**
 * #Pantryc_milter_structure_function
 * Any unrecognized or unimplemented command filter
 */
sfsistat pantryc_milter__xxfi_unknown(SMFICTX *context, const char *command);

/**
 * #Pantryc_milter_structure_function
 * SMTP DATA command filter
 */
sfsistat pantryc_milter__xxfi_data(SMFICTX *context);

/**
 * #Pantryc_milter_structure_function
 * Negotiation callback
 */
sfsistat pantryc_milter__xxfi_negotiate(SMFICTX *context, unsigned long f0,
		unsigned long f1, unsigned long f2, unsigned long f3,
		unsigned long *pf0, unsigned long *pf1, unsigned long *pf2,
		unsigned long *pf3);

#endif /* PANTRYC_MILTER_H_ */

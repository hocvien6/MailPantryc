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

sfsistat pantryc_milter__cleanup(SMFICTX *ctx, bool ok);

sfsistat pantryc_milter__xxfi_connect(SMFICTX *ctx, char *hostname,
_SOCK_ADDR *hostaddr);
sfsistat pantryc_milter__xxfi_helo(SMFICTX *ctx, char *helohost);
sfsistat pantryc_milter__xxfi_envfrom(SMFICTX *ctx, char **argv);
sfsistat pantryc_milter__xxfi_envrcpt(SMFICTX *ctx, char **argv);
sfsistat pantryc_milter__xxfi_header(SMFICTX *ctx, char *headerf, char *headerv);
sfsistat pantryc_milter__xxfi_eoh(SMFICTX *ctxx);
sfsistat pantryc_milter__xxfi_body(SMFICTX *ctx, unsigned char *bodyp,
		size_t bodylen);
sfsistat pantryc_milter__xxfi_eom(SMFICTX *ctx);
sfsistat pantryc_milter__xxfi_abort(SMFICTX *ctx);
sfsistat pantryc_milter__xxfi_close(SMFICTX *ctx);
sfsistat pantryc_milter__xxfi_unknown(SMFICTX *ctx, const char *cmd);
sfsistat pantryc_milter__xxfi_data(SMFICTX *ctx);
sfsistat pantryc_milter__xxfi_negotiate(SMFICTX *ctx, unsigned long f0,
		unsigned long f1, unsigned long f2, unsigned long f3,
		unsigned long *pf0, unsigned long *pf1, unsigned long *pf2,
		unsigned long *pf3);

#endif /* PANTRYC_MILTER_H_ */

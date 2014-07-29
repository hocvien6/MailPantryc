#ifndef PANTRYC_SCANNER_H_
#define PANTRYC_SCANNER_H_
#include <gmime/gmime.h>

gchar* pantryc_scanner_get_recip(GMimeMessage *msg, GMimeRecipientType rtype);
void pantryc_scanner_print_date(GMimeMessage *msg);
gchar* pantryc_scanner_get_refs_str(GMimeMessage *msg);

#endif /* PANTRYC_SCANNER_H_ */

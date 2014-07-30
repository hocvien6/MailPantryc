#ifndef PANTRYC_SCANNER_H_
#define PANTRYC_SCANNER_H_
#include <gmime/gmime.h>

gchar* pantryc_scanner_get_recip(GMimeMessage *message, GMimeRecipientType type);
gchar* pantryc_scanner_print_date(GMimeMessage *message);
gchar* pantryc_scanner_get_refs_str(GMimeMessage *message);

/**
 * Extract attached file from message
 * @message:		GMimeMessage message want to extract file from
 * @index:			index of MIME part contain file to extract
 * @permission:		specify permission mode to create file (Ex: 644, 755...)
 * @filename:		should be appropriate directory to extract file to, after function call
 * 					name of extracted file would be appended to @filename
 */
GMimeStream* pantryc_scanner_extract_attachment(GMimeMessage *message,
		int index, const char *permission, char *filename);

#endif /* PANTRYC_SCANNER_H_ */

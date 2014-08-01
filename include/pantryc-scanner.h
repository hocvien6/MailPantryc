#ifndef PANTRYC_SCANNER_H_
#define PANTRYC_SCANNER_H_
#include <gmime/gmime.h>

gchar* pantryc_scanner__get_recip(GMimeMessage *message, GMimeRecipientType type);
gchar* pantryc_scanner__get_date(GMimeMessage *message);
gchar* pantryc_scanner__get_references(GMimeMessage *message);

/**
 * Extract attached file from message
 * @message:		GMimeMessage message want to extract file from
 * @index:			index of MIME part contain file to extract
 * @permission:		specify permission mode to create file (Ex: 644, 755...)
 * @filepath:		should be appropriate directory to extract file to, after function call
 * 					name of extracted file would be appended to @filepath
 */
GMimeStream* pantryc_scanner__extract_attachment(GMimeMessage *message,
		int index, const int permission, const char *filepath);

#endif /* PANTRYC_SCANNER_H_ */

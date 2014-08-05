#ifndef PANTRYC_SCANNER_H_
#define PANTRYC_SCANNER_H_
#include <gmime/gmime.h>

/**
 * Get recipients of message
 * @message:		GMimeMessage message to get recipients
 * @type:			type of header: to, cc, or bcc...
 */
gchar* pantryc_scanner__get_recip(GMimeMessage *message, GMimeRecipientType type);

/**
 * Get date of message
 * @message:		GMimeMessage message to get date
 */
gchar* pantryc_scanner__get_date(GMimeMessage *message);

/**
 * Get references of message
 * @message:		GMimeMessage message to get references
 */
gchar* pantryc_scanner__get_references(GMimeMessage *message);

/**
 * Extract attached file from message
 * @message:		GMimeMessage message that want to extract file from
 * @index:			index of MIME part contain file to extract
 * @permission:		specify permission mode to create file (Ex: 0644, 0755...)
 * @filepath:		should be appropriate directory to extract file to, after function call
 * 					name of extracted file would be appended to @filepath
 */
GMimeStream* pantryc_scanner__extract_attachment(GMimeMessage *message,
		int index, const int permission, const char *filepath);

#endif /* PANTRYC_SCANNER_H_ */

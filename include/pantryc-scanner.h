#ifndef PANTRYC_SCANNER_H_
#define PANTRYC_SCANNER_H_
#include <gmime/gmime.h>

/**
 * Get recipients of message
 * @message:		GMimeMessage message to get recipients
 * @type:			type of header: to, cc, or bcc...
 * return			recipients
 */
gchar* pantryc_scanner__get_recipient(GMimeMessage *message,
		GMimeRecipientType type);

/**
 * Get date of message
 * @message:		GMimeMessage message to get date
 * return			date
 */
gchar* pantryc_scanner__get_date(GMimeMessage *message);

/**
 * Get references of message
 * @message:		GMimeMessage message to get references
 * return			references
 */
gchar* pantryc_scanner__get_references(GMimeMessage *message);

/**
 * Get message content of message
 * @message:		GMimeMessage message to read message content
 * @index:			index of MIME part to get content
 * return			message content
 */
gchar* pantryc_scanner__get_content(GMimeMessage *message, int index);

/**
 * Extract attached file from a MIME part
 * @message:		GMimeMessage message that want to extract file from
 * @index:			index of MIME part contain file to extract
 * @permission:		specify permission mode to create file (Ex: 0644, 0755...)
 * @filepath:		should be appropriate directory to extract file to, after function call
 * 					name of extracted file would be appended to @filepath
 * return			file stream
 */
GMimeStream* pantryc_scanner__extract_attachment(GMimeMessage *message,
		int index, const int permission, const char *filepath);

/**
 * Read data from certain MIME part and write to file stream
 * @message:		GMimeMessage message that want to get file stream data
 * @index:			index of MIME part contain file to get
 * return			file stream get from MIME part
 */
FILE* pantryc_scanner__read_attachment(GMimeMessage *message, int index);

#endif /* PANTRYC_SCANNER_H_ */

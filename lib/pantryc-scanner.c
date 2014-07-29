#include "../include/pantryc-scanner.h"

gchar* pantryc_scanner_get_recip(msg, rtype)
	GMimeMessage *msg;GMimeRecipientType rtype; {
	char *recep;
	InternetAddressList *receps;

	receps = g_mime_message_get_recipients(msg, rtype);
	recep = (char*) internet_address_list_to_string(receps, FALSE);

	if (!recep || !*recep) {
		g_free(recep);
		return NULL;
	}

	return recep;
}
void pantryc_scanner_print_date(msg)
	GMimeMessage *msg; {
	time_t t;
	int tz;
	char buf[64];
	size_t len;
	struct tm *t_m;

	g_mime_message_get_date(msg, &t, &tz);
	t_m = localtime(&t);

	len = strftime(buf, sizeof(buf) - 1, "%c", t_m);

	if (len > 0)
		g_print("Date   : %s (%s%04d)\n", buf, tz < 0 ? "-" : "+", tz);
}
gchar* pantryc_scanner_get_refs_str(msg)
	GMimeMessage *msg; {
	const gchar *str;
	const GMimeReferences *cur;
	GMimeReferences *mime_refs;
	gchar *rv;

	str = g_mime_object_get_header(GMIME_OBJECT(msg), "References");
	if (!str)
		return NULL;

	mime_refs = g_mime_references_decode(str);
	for (rv = NULL, cur = mime_refs; cur;
			cur = g_mime_references_get_next(cur)) {

		const char* msgid;
		msgid = g_mime_references_get_message_id(cur);
		rv = g_strdup_printf("%s%s%s", rv ? rv : "", rv ? "," : "", msgid);
	}
	g_mime_references_free(mime_refs);

	return rv;
}

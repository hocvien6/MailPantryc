#ifndef PANTRYC_UTIL_H_
#define PANTRYC_UTIL_H_

// TODO rename macro
#define pFALSE		(0)
#define pTRUE		!pFALSE
typedef int pBOOL;

/**
 * Convert text to lower string
 * @text			text to convert
 * return			lower string
 */
char* pantryc_util__to_lower(char *text);

#endif /* PANTRYC_UTIL_H_ */

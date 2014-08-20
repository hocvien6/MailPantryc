#ifndef PANTRYC_UTIL_H_
#define PANTRYC_UTIL_H_

/* TODO rename macro */
#define pSUCCESS		"**SUCCESS**\t"
#define pNOTICE			"**NOTICE**\t"
#define pERROR			"**ERROR**\t"

#define pFALSE			(0)
#define pTRUE			!pFALSE
typedef int pBOOL;

/* Pantryc Tags */
/* Pantryc environment log file, define at pantryc-environment.h */
#define	$pTag_log
/* Pantryc milter function, used to declare a smfiDesc structure
 * and register then plug into MTA */
#define	$pTag_milter

/**
 * Convert text to lower string
 * @text			text to convert
 * return			lower string
 */
char* pantryc_util__to_lower(char *text);

#endif /* PANTRYC_UTIL_H_ */

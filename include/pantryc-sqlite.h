#ifndef PANTRYC_SQLITE_H_
#define PANTRYC_SQLITE_H_

#include <sqlite3.h>

#include <pantryc-util.h>

sqlite3 *pantryc_sqlite__database;

/**
 * Open Pantryc Database
 * return			open successfully or not
 */
pBOOL pantryc_sqlite__open_database();

/**
 * Insert an address to rejected recipients list
 * @address:		rejected address to add
 */
void pantryc_sqlite__insert_rejected_receipt_address(char *address);

/**
 * Delete an address from rejected recipients list
 * @address:		rejected address to remove
 */
void pantryc_sqlite__delete_rejected_receipt_address(char *address);

/**
 * Get rejected receipt address list
 * @address			address to check
 * return			address is rejected or not
 */
pBOOL pantryc_sqlite__check_rejected_receipt_address_list(char *address);

/**
 * Insert a word with score to bad word list
 * @word:			word to insert
 * @score:			score
 */
void pantryc_sqlite__insert_bad_word(char *word, int score);

/**
 * Delete a word with score from bad word list
 * @word:			word to delete
 */
void pantryc_sqlite__delete_bad_word(char *word);

/**
 * Get rejected receipt address list
 * @word			word to get score
 * return			score
 */
int pantryc_sqlite__get_score_bad_word(char *word);

/**
 * Close Pantryc Database
 * return			open successfully or not
 */
pBOOL pantryc_sqlite__close_database();

#endif /* PANTRYC_SQLITE_H_ */

#ifndef PANTRYC_H_
#define PANTRYC_H_

/**
 * Born
 * @directory:		where extract attachments and store log file for each message
 * @permission:		mode of permission (ex: 0644, 0755...)
 * @score:			required score
 * @port:			port to set (ex: f1.sock...)
 * @timeout:		time out
 */
int pantryc__born(const char *directory, const int permission, const int score,
		char *port, char *timeout);

/**
 * Learn how to add a rejected receipt address
 * @address:		rejected address to add
 */
void pantryc__learn_insert_rejected_receipt_address(char *address);

/**
 * Learn how to remove a rejected receipt address
 * @address:		rejected address to remove
 */
void pantryc__learn_delete_rejected_receipt_address(char *address);

/**
 * Learn how to add a bad word
 * @word:			word to add
 * @score:			score
 */
void pantryc__learn_insert_bad_word(char *word, int score);

/**
 * Learn how to remove a bad word
 * @word:			word to remove
 */
void pantryc__learn_delete_bad_word(char *word);

/**
 * Work
 * @argc
 * @argv
 */
int pantryc__work(int argc, char **argv);

/**
 * Die
 */
int pantryc__die();

#endif /* PANTRYC_H_ */

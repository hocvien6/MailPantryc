#include <string.h>
#include <stdlib.h>

#include "../include/pantryc-sqlite.h"
#include "../include/pantryc-environment.h"

/* Pantryc Database default name */
#define PANTRYC_SQLITE__DATABASE								"PantrycDatabase.db"
/* Table "Address" with column "RejectedReceipt" */
#define PANTRYC_SQLITE__TABLE_ADDRESS							"Address"
#define PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT			"RejectedReceipt"
/* Table "Word" with 2 columns "Bad" and "Score" */
#define PANTRYC_SQLITE__TABLE_WORD								"Word"
#define PANTRYC_SQLITE__COLUMN_WORD_BAD							"Bad"
#define PANTRYC_SQLITE__COLUMN_WORD_SCORE						"Score"

#define PANTRYC_SQLITE__TEXT_LENGTH								"50"

#define PANTRYC_SQLITE__SQL_LENGTH								200
#define PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(message)			\
	char *message = (char*) malloc(sizeof(char) * PANTRYC_SQLITE__SQL_LENGTH)

static void pantryc_sqlite__initialize(char *table, char *column);
static int pantryc_sqlite__execute(const char *sql,
		int (*callback)(void*, int, char**, char**), void *data, char *success,
		char *failure, char **error);

pBOOL pantryc_sqlite__open_database() {
	int result;
	char *filename = (char*) malloc(
			sizeof(char)
					* (strlen(pantryc_environment__working_directory)
							+ strlen(PANTRYC_SQLITE__DATABASE) + 1));
	sprintf(filename, "%s" PANTRYC_SQLITE__DATABASE,
			pantryc_environment__working_directory);
	result = sqlite3_open(filename, &pantryc_sqlite__database);

	if (result) {
		fprintf(pantryc_environment__log_file,
				"**ERROR**\tCannot open database " PANTRYC_SQLITE__DATABASE "\n");
		pantryc_sqlite__database = NULL;
		return pFALSE;
	}
	return pTRUE;
}

void pantryc_sqlite__insert_rejected_receipt_address(address)
	char *address; {
	if (address != NULL) {
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_ADDRESS,
		PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT);

		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(success);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
		char *error;
		sprintf(sql, "insert into " PANTRYC_SQLITE__TABLE_ADDRESS
		" (" PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT ")"
		" values ('%s')", address);
		sprintf(success,
				"Add address '%s' to column \"" PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT
				"\" on table \"" PANTRYC_SQLITE__TABLE_ADDRESS "\"", address);
		sprintf(failure, "Cannot add rejected receipt address '%s'", address);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}
}

void pantryc_sqlite__delete_rejected_receipt_address(address)
	char *address; {
	if (address != NULL) {
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_ADDRESS,
		PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT);

		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(success);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
		char *error;
		sprintf(sql, "delete from " PANTRYC_SQLITE__TABLE_ADDRESS
		" where " PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT " = '%s';",
				address);
		sprintf(success,
				"Remove address '%s' from column \"" PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT
				"\" on table \"" PANTRYC_SQLITE__TABLE_ADDRESS "\"", address);
		sprintf(failure, "Cannot remove rejected receipt address '%s'",
				address);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}
}

pBOOL pantryc_sqlite__check_rejected_receipt_address_list(address)
	char *address; {
	pantryc_sqlite__initialize(
	PANTRYC_SQLITE__TABLE_ADDRESS,
	PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT);

	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
	char *error;
	sprintf(sql, "select " PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT
	" from " PANTRYC_SQLITE__TABLE_ADDRESS
	" where " PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT " = '%s';",
			address);
	sprintf(failure, "Cannot get rejected receipt address '%s'", address);
	int data = 0;

	int result =
			pantryc_sqlite__execute(sql,
					( {	int $(void *data, int size, char **value, char **column) {
									*((int*) data) = size;
									return 0;
								}$;
							}), &data,
					NULL, failure, &error);
	if (result != SQLITE_OK)
		return pFALSE;
	data = data > 0 ? data : 0;
	return data;
}

void pantryc_sqlite__insert_bad_word(word, score)
	char*word;int score; {
	if (word != NULL) {
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_WORD,
		PANTRYC_SQLITE__COLUMN_WORD_BAD);
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_WORD,
		PANTRYC_SQLITE__COLUMN_WORD_SCORE);

		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(success);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
		char *error;

		word = pantryc_util__to_lower(word);
		sprintf(sql,
				"insert into " PANTRYC_SQLITE__TABLE_WORD
				" (" PANTRYC_SQLITE__COLUMN_WORD_BAD ", " PANTRYC_SQLITE__COLUMN_WORD_SCORE ")"
				" values ('%s', %d);", word, score);
		sprintf(success,
				"Add word '%s' with score %d to column \"" PANTRYC_SQLITE__COLUMN_WORD_BAD
				"\" on table \"" PANTRYC_SQLITE__TABLE_WORD "\"", word, score);
		sprintf(failure, "Cannot add word '%s'", word);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}
}

void pantryc_sqlite__delete_bad_word(word)
	char*word; {
	if (word != NULL) {
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_WORD,
		PANTRYC_SQLITE__COLUMN_WORD_BAD);
		pantryc_sqlite__initialize(
		PANTRYC_SQLITE__TABLE_WORD,
		PANTRYC_SQLITE__COLUMN_WORD_SCORE);

		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(success);
		PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
		char *error;
		sprintf(sql, "delete from " PANTRYC_SQLITE__TABLE_WORD
		" where " PANTRYC_SQLITE__COLUMN_WORD_BAD " = '%s';", word);
		sprintf(success,
				"Remove word '%s' from column \"" PANTRYC_SQLITE__COLUMN_WORD_BAD
				"\" on table \"" PANTRYC_SQLITE__TABLE_WORD "\"", word);
		sprintf(failure, "Cannot remove word '%s'", word);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}

}

int pantryc_sqlite__get_score_bad_word(word)
	char*word; {
	pantryc_sqlite__initialize(
	PANTRYC_SQLITE__TABLE_WORD,
	PANTRYC_SQLITE__COLUMN_WORD_BAD);

	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
	char *error;

	sprintf(sql, "select " PANTRYC_SQLITE__COLUMN_WORD_SCORE
	" from " PANTRYC_SQLITE__TABLE_WORD
	" where " PANTRYC_SQLITE__COLUMN_WORD_BAD " = '%s' collate nocase;", word);
	sprintf(failure, "Cannot get score of bad word '%s'", word);
	int data = 0;

	int result =
			pantryc_sqlite__execute(sql,
					( {	int $(void *data, int size, char **value, char **column) {
									*((int*) data) = atoi(value[0]);
									return 0;
								}$;
							}), &data,
					NULL, failure, &error);
	if (result != SQLITE_OK)
		return pFALSE;
	data = data > 0 ? data : 0;

	return data;
}

pBOOL pantryc_sqlite__close_database() {
	return sqlite3_close(pantryc_sqlite__database);
}

/* Private function */
static void pantryc_sqlite__initialize(table, column)
	char *table;char *column; {
	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(sql);
	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(success);
	PANTRYC_SQLITE__INITIALZE_QUERY_MESSAGE(failure);
	char *error;

	/* Check table existing */
	sprintf(sql, "select * from %s;", table);
	int result = sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL,
			&error);

	/* No such table */
	if (result != SQLITE_OK) {
		fprintf(pantryc_environment__log_file,
				"**NOTICE**\tNo such table \"%s\"\n", table);
		if (strcmp(table, PANTRYC_SQLITE__TABLE_ADDRESS) == 0) {
			/* Create table "Address" */
			sprintf(sql, "create table %s ("
			PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT
			" char(" PANTRYC_SQLITE__TEXT_LENGTH ") unique);", table);
		} else if (strcmp(table, PANTRYC_SQLITE__TABLE_WORD) == 0) {
			/* Create table "Word" */
			sprintf(sql, "create table %s ("
			PANTRYC_SQLITE__COLUMN_WORD_BAD
			" char(" PANTRYC_SQLITE__TEXT_LENGTH ") unique, "
			PANTRYC_SQLITE__COLUMN_WORD_SCORE
			" int);", table);
		} else
			return;

		sprintf(success, "Create table \"%s\"", table);
		sprintf(failure, "Cannot create table \"%s\"", table);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}

	/* Check column existing */
	sprintf(sql, "select %s from %s;", column, table);
	result = sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL, &error);
	/* No such column */
	if (result != SQLITE_OK) {
		fprintf(pantryc_environment__log_file,
				"**NOTICE**\tNo such column \"%s\"\n", column);
		if (strcmp(column, PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT)
				== 0) {
			/* Add column "RejectedReceipt" table "Address" */
			sprintf(sql, "alter table " PANTRYC_SQLITE__TABLE_ADDRESS
			" add column %s char(" PANTRYC_SQLITE__TEXT_LENGTH ");", column); // still not unique
		} else if (strcmp(column,
		PANTRYC_SQLITE__COLUMN_WORD_BAD) == 0) {
			/* Add column "Bad" table "Word" */
			sprintf(sql, "alter table " PANTRYC_SQLITE__TABLE_WORD
			" add column %s char(" PANTRYC_SQLITE__TEXT_LENGTH ");", column); // still not unique
		} else if (strcmp(column,
		PANTRYC_SQLITE__COLUMN_WORD_SCORE) == 0) {
			/* Add column "Score" table "Word" */
			sprintf(sql, "alter table " PANTRYC_SQLITE__TABLE_WORD
			" add column %s int;", column); // still not unique
		} else
			return; // do not forget `else return;' statement here!!!

		sprintf(success, "Add column \"%s\""
				" on table \"%s\"", column, table);
		sprintf(failure, "Cannot add column \"%s\"", column);
		pantryc_sqlite__execute(sql, NULL, NULL, success, failure, &error);
	}
	return;
}

static int pantryc_sqlite__execute(sql, callback, data, success, failure, error)
	const char *sql;int (*callback)(void*, int, char**, char**);void *data;char *success;char *failure;char **error; {
	int result = sqlite3_exec(pantryc_sqlite__database, sql, callback, data,
			error);
	if (result == SQLITE_OK) {
		if (success != NULL)
			fprintf(pantryc_environment__log_file, "**SUCCESS**\t%s\n",
					success);
	} else {
		if (failure != NULL)
			fprintf(pantryc_environment__log_file, "**ERROR**\t%s, %s\n",
					failure, *error);
		sqlite3_free(*error);
	}
	return result;
}

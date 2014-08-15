#include <string.h>
#include <stdlib.h>

#include "../include/pantryc-sqlite.h"

#define PANTRYC_SQLITE__SQL_LENGTH 200
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
					* (strlen(pantryc_global__working_directory)
							+ strlen(PANTRYC_SQLITE__DATABASE) + 1));
	sprintf(filename, "%s" PANTRYC_SQLITE__DATABASE,
			pantryc_global__working_directory);
	result = sqlite3_open(filename, &pantryc_sqlite__database);

	if (result) {
		fprintf(pantryc_global__log_file,
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
	sprintf(failure, "Cannot get rejected receipt address list");
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

void pantryc_sqlite__insert_bad_word(word)
	char*word; {

}

void pantryc_sqlite__delete_bad_word(word)
	char*word; {

}

int pantryc_sqlite__get_score_bad_word(word)
	char*word; {
	return 1;
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
		fprintf(pantryc_global__log_file, "**NOTICE**\tNo such table \"%s\"\n",
				table);
		/* Create table "Address" */
		if (strcmp(table, PANTRYC_SQLITE__TABLE_ADDRESS) == 0) {
			sprintf(sql,/*
			 "create table %s (" PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT
			 " char(" PANTRYC_SQLITE__TEXT_LENGTH ") unique);",*/
			"create table %s (tmp int);", table);
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
		fprintf(pantryc_global__log_file, "**NOTICE**\tNo such column \"%s\"\n",
				column);
		/* Add column "RejectedReceipt" */
		if (strcmp(column, PANTRYC_SQLITE__COLUMN_ADDRESS_REJECTED_RECEIPT)
				== 0) {
			sprintf(sql, "alter table " PANTRYC_SQLITE__TABLE_ADDRESS
			" add column %s char(" PANTRYC_SQLITE__TEXT_LENGTH ");", column); // still not unique
		} else
			return;

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
			fprintf(pantryc_global__log_file, "**SUCCESS**\t%s\n", success);
	} else {
		if (failure != NULL)
			fprintf(pantryc_global__log_file, "**ERROR**\t%s, %s\n", failure,
					*error);
		sqlite3_free(*error);
	}
	return result;
}

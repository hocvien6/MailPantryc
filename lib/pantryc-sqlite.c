#include <string.h>

#include "../include/pantryc-sqlite.h"

static void pantryc_sqlite__initialize(char *column);

void pantryc__open_database() {
	char *message = 0;
	int result;
	result = sqlite3_open(PANTRYC_SQLITE__DATABASE, &pantryc_sqlite__database);

	if (result) {
		pantryc_sqlite__database = NULL;
	}
}

void pantryc_sqlite__add_rejected_receipt_address(address)
	char *address; {
	pantryc_sqlite__initialize( PANTRYC_SQLITE__COLUMN_ADDRESS);
	char *sql;
	sql = "insert into " PANTRYC_SQLITE__TABLE_REJECTED_RECEIPT_ADDRESS
	" (" PANTRYC_SQLITE__COLUMN_ADDRESS ")"
	" values (";
	strcat(sql, address);
	strcat(sql, ")");

	char *message;
	sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL, &message);
}

int pantryc_sqlite__close() {
	return sqlite3_close(pantryc_sqlite__database);
}

/* Private function */
static void pantryc_sqlite__initialize(column)
	char *column; {

	/* Create SQL statement */
	char* sql;
	char *message;

	/* Execute SQL statement */
	sql = "select * from " PANTRYC_SQLITE__TABLE_REJECTED_RECEIPT_ADDRESS;
	int result = sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL,
			&message);
	if (result != SQLITE_OK) {
		// no such table
		sql = "create table " PANTRYC_SQLITE__TABLE_REJECTED_RECEIPT_ADDRESS
		" (" PANTRYC_SQLITE__COLUMN_ADDRESS " char(" PANTRYC_SQLITE__TEXT_LENGTH
		") primary key not null );";
		sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL, &message);
		sqlite3_free(message);
	} else {
		sql = "select ";
		strcat(sql, column);
		strcat(sql, " from " PANTRYC_SQLITE__TABLE_REJECTED_RECEIPT_ADDRESS);
		int result = sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL,
				&message);
		if (result != SQLITE_OK) {
			// no such column
			sql = "alter table " PANTRYC_SQLITE__TABLE_REJECTED_RECEIPT_ADDRESS
			" add column ";
			strcat(sql, column);
			strcat(sql, " char(" PANTRYC_SQLITE__TEXT_LENGTH
			") primary key not null );");
			sqlite3_exec(pantryc_sqlite__database, sql, NULL, NULL, &message);
		}
	}
}

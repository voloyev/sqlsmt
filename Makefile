CC  = gcc

sqlsmt_make: sqlsmt.c
	$(CC) sqlsmt.c -o bin/sqlsmt

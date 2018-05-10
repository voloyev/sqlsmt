CC  = gcc

sqlsmt_make: sqlsmt.c
	mkdir -p bin
	$(CC) sqlsmt.c -o bin/sqlsmt

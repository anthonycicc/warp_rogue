/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Roguelike Database Library
 *
 * Provides basic functions for parsing and writing
 * the roguelike database format i.e. *.rdb files
 *
 * NAMESPACE: RDB_* / rdb_*
 *
 */


/*
 * RDB_NEXT_TOKEN can be passed to the X_data_token() functions
 * to iterate through data tokens
 */
#define		RDB_NEXT_TOKEN		-1


/*
 * RDB file modes
 */
typedef enum {
	
	RDB_NIL = -1,
	
	RDB_READ,
	RDB_WRITE,
	
	RDB_OUT_OF_BOUNDS
	
} RDB_FILE_MODE;



/*
 * function prototypes
 */
int		rdb_open(const char *, RDB_FILE_MODE);
void		rdb_close(void);

void		rdb_write_field_name(const char *);
void		rdb_write_integer(int);
void		rdb_write_string(const char *);

int		rdb_next_field(void);
const char *	rdb_field_name(void);
const char *	rdb_data_token(int);
const char *	rdb_optional_data_token(int);

void		rdb_set_error_handler(void (*)(const char *, ...));




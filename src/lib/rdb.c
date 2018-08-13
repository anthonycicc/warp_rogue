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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "rdb.h"


/*
 * the following defines determine which newline style
 * is used when writing RDB files
 *
 * UNIX style newlines are the default, because
 * most modern text editors understand them
 *
 * notice that choosing Windows style newlines
 * will make the RDB files larger!
 *
 */
#define RDB_WRITE_UNIX_NEWLINES
/* #define RDB_WRITE_WINDOWS_NEWLINES */
/* #define RDB_WRITE_MAC_NEWLINES */


/*
 * RDB symbols
 */
#define RDB_TOKEN_DELIMITER		':'
#define RDB_OVERRIDE			'\\'
#define RDB_COMMENT			'#'


/*
 * various limits
 */
#define RDB_LINE_BUFFER_SIZE		1024
#define RDB_MAX_TOKENS			64
#define RDB_TOKEN_SIZE			128

#define RDB_ERROR_MESSAGE_BUFFER_SIZE	256


/*
 * ASCII codes
 */
#define RDB_LF				0x0A
#define RDB_CR				0x0D
#define RDB_TAB				0x09
#define RDB_SPACE			0x20


/*
 * this macro tells us which characters must be prefixed with RDB_OVERRIDE
 */
#define rdb_requires_override(c)	((c) == RDB_TOKEN_DELIMITER || \
	(c) == RDB_OVERRIDE)


/*
 * prototypes for internal functions
 */
static void		rdb_write_newline(void);

static char *		rdb_read_line(char *, int, FILE *);

static int		rdb_irrelevant_line(const char *);

static int		rdb_tokenize(const char *);

static void		rdb_error(const char *, ...);


/*
 * internal variables
 */
static FILE *		RDB_File;
static const char *	RDB_FilePath;

static int		RDB_LineNumber;

static char		RDB_Token[RDB_MAX_TOKENS][RDB_TOKEN_SIZE];
static char		RDB_TokenValid[RDB_MAX_TOKENS];

static int		RDB_DataTokenIterator;

static int		RDB_FirstFieldWritten;

static void		(*RDB_Error)(const char *, ...) = rdb_error;




/*
 * opens a RDB file
 * returns 0 if the attempt fails
 */
int rdb_open(const char *file_path, RDB_FILE_MODE file_mode)
{
	RDB_LineNumber = 0;
	RDB_FilePath = file_path;

	if (file_mode == RDB_READ) {

		RDB_File = fopen(RDB_FilePath, "rb");

	} else {

		RDB_File = fopen(RDB_FilePath, "wb");

		RDB_FirstFieldWritten = 0;
	}

	if (RDB_File == NULL) {

		(*RDB_Error)("*** RDB ERROR *** cannot open file (%s)",
			RDB_FilePath
		);

		return 0;
	}

	return 1;
}



/*
 * closes the RDB file
 */
void rdb_close(void)
{

	fclose(RDB_File);
}



/*
 * writes a new field name
 */
void rdb_write_field_name(const char *field_name)
{
	const char *p;

	if (RDB_FirstFieldWritten) {

		rdb_write_newline();

	} else {

		RDB_FirstFieldWritten = 1;
	}

	p = field_name;

	if (*p == RDB_COMMENT) {

		fputc(RDB_OVERRIDE, RDB_File);
	}

	while (*p != '\0') {

		if (rdb_requires_override(*p)) {

			fputc(RDB_OVERRIDE, RDB_File);
		}

		fputc(*p, RDB_File);

		++p;
	}

	fputc(RDB_TOKEN_DELIMITER, RDB_File);
}



/*
 * writes an integer data token
 */
void rdb_write_integer(int data)
{

	fprintf(RDB_File, "%d:", data);
}



/*
 * writes a string data token
 */
void rdb_write_string(const char *data)
{
	const char *p;

	p = data;

	while (*p != '\0') {

		if (rdb_requires_override(*p)) {

			fputc(RDB_OVERRIDE, RDB_File);
		}

		fputc(*p, RDB_File);

		++p;
	}

	fputc(RDB_TOKEN_DELIMITER, RDB_File);
}



/*
 * reads the next field
 * returns 0 if there is no next field
 */
int rdb_next_field(void)
{
        char line[RDB_LINE_BUFFER_SIZE];

        do {

		if (rdb_read_line(line, RDB_LINE_BUFFER_SIZE,
			RDB_File) == NULL) {

			return 0;
		}

	} while (rdb_irrelevant_line(line));

	if (!rdb_tokenize(line)) {

		return 0;
	}

	return 1;
}



/*
 * returns the name of the current field
 */
const char * rdb_field_name(void)
{

	return RDB_Token[0];
}



/*
 * returns the requested data token
 * if the requested token does not exist, the function
 * sends an error message and returns NULL
 */
const char * rdb_data_token(int token_index)
{

	if (token_index == RDB_NEXT_TOKEN) {

		token_index = RDB_DataTokenIterator;
		++RDB_DataTokenIterator;
	}

	if (!RDB_TokenValid[token_index + 1]) {

		(*RDB_Error)("*** RDB ERROR *** missing token (%s:%d)",
			RDB_FilePath, RDB_LineNumber
		);

		return NULL;
	}

	return RDB_Token[token_index + 1];
}



/*
 * returns the requested data token or NULL if
 * the requested token does not exist
 */
const char * rdb_optional_data_token(int token_index)
{

	if (token_index == RDB_NEXT_TOKEN) {

		token_index = RDB_DataTokenIterator;
		++RDB_DataTokenIterator;
	}

	if (!RDB_TokenValid[token_index + 1]) {

		return NULL;
	}

	return RDB_Token[token_index + 1];
}



/*
 * sets the error handling function
 */
void rdb_set_error_handler(void (*error_function)(const char *, ...))
{

	RDB_Error = error_function;
}



/*
 * writes the newline character(s)
 */
static void rdb_write_newline(void)
{

#if defined(RDB_WRITE_UNIX_NEWLINES)
	fputc(RDB_LF, RDB_File);
#elif defined(RDB_WRITE_WINDOWS_NEWLINES)
	fputc(RDB_CR, RDB_File);
	fputc(RDB_LF, RDB_File);
#elif defined(RDB_WRITE_MAC_NEWLINES)
	fputc(RDB_CR, RDB_File);
#else
#	error	*** RDB ERROR *** no newline style defined!
#endif

}



/*
 * reads a line of ASCII text from a binary file stream
 * - supports DOS, Unix, and Mac style new line characters
 * - the new line characters aren't written to the output buffer
 * returns NULL when EOF is reached
 */
static char * rdb_read_line(char *buffer, int n, FILE *stream)
{
	int c;
	int i;

	i = 0;
	while (i < n - 1) {

		c = getc(stream);

		if (c == EOF) {

			if (i > 0) break;

			return NULL;
		}

		if (c == RDB_CR) {

			c = getc(stream);

			if (c != RDB_LF) ungetc(c, stream);

			break;

		} else if (c == RDB_LF) {

			break;

		} else {

			buffer[i] = (char)c;
			++i;
		}

	}

	buffer[i] = '\0';

	++RDB_LineNumber;

	return buffer;
}



/*
 * return 1 if the passed line is irrelevant (must not be parsed)
 * or 0 if it is not
 */
static int rdb_irrelevant_line(const char *line)
{
	const char *p;

	p = line;

	if (*p == RDB_COMMENT) return 1;

	while (*p != '\0') {

		if (!isspace(*p)) {

			return 0;
		}

		++p;
	}

	return 1;
}



/*
 * tokenizes a line
 * returns 0 if the tokenization fails
 */
static int rdb_tokenize(const char *line)
{
	const char *p;
	int i, token_i;
	int n_started_tokens, n_closed_tokens;

	RDB_DataTokenIterator = 0;

	for (i = 0; i < RDB_MAX_TOKENS; i++) RDB_TokenValid[i] = 0;

	for (i = token_i = n_closed_tokens = 0,
		p = line, n_started_tokens = 1;
		*p != '\0';
		++p, ++i) {

		if (*p == RDB_OVERRIDE) {

			++p;

		} else if (*p == RDB_TOKEN_DELIMITER) {

			++n_closed_tokens;
			RDB_Token[token_i][i] = '\0';
			RDB_TokenValid[token_i] = 1;
			++token_i;

			if (token_i >= RDB_MAX_TOKENS) {

				(*RDB_Error)("*** RDB ERROR *** " \
					"too many tokens (%s:%d)",
					RDB_FilePath, RDB_LineNumber
				);

				return 0;
			}

			i = -1;

			continue;
		}

		if (n_started_tokens == n_closed_tokens &&
			!isspace(*p)) {

			++n_started_tokens;
		}

		if (i >= RDB_TOKEN_SIZE - 1) {

			(*RDB_Error)("*** RDB ERROR *** token too long (%s:%d)",
				RDB_FilePath, RDB_LineNumber
			);

			return 0;
		}

		RDB_Token[token_i][i] = *p;
	}

	if (n_started_tokens > n_closed_tokens) {

		(*RDB_Error)("*** RDB ERROR *** unclosed token (%s:%d)",
			RDB_FilePath, RDB_LineNumber
		);

		return 0;
	}

	if (token_i == 1) {

		(*RDB_Error)("*** RDB ERROR *** field without data token (%s:%d)",
			RDB_FilePath, RDB_LineNumber
		);

		return 0;
	}

	return 1;
}



/*
 * default error handler: outputs to stderr and aborts
 */
static void rdb_error(const char *fmt, ...)
{
	va_list vl;
	static char buffer[RDB_ERROR_MESSAGE_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);

	fprintf(stderr, "%s\n", buffer);

	exit(EXIT_FAILURE);
}


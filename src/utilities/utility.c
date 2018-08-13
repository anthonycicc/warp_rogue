/*
 * Copyright (C) 2002-2008 The Warp Rogue Team
 * Part of the Warp Rogue Project
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Module: Utility
 */

#include "wrogue.h"




static char *			read_line(char *, int, FILE *);



/*
 * direction modifiers
 */
static const COORD_MODIFIER	DirectionModifier[MAX_REAL_DIRECTIONS][2] = {

	{-1,	0},
	{+1,	0},
	{0,	-1},
	{0,	+1},
	{-1,	-1},
	{-1,	+1},
	{+1,	-1},
	{+1,	+1}

};


/*
 * direction names
 */
static const char *		DirectionName[MAX_DIRECTIONS] = {

	"Up",
	"Down",
	"Left",
	"Right",
	
	"Up left",
	"Up righ",
	"Down left",
	"Down right",
	
	"Elevator",
	
	"Way up",
	"Way down"

};






/*
 * returns the coordinate modifier for the passed coord type / direction
 * combo
 */
COORD_MODIFIER direction_modifier(COORD_TYPE type, DIRECTION direction)
{

	return DirectionModifier[direction][type];
}



/*
 * name -> direction
 */
DIRECTION name_to_direction(const char *name)
{
	DIRECTION i;

	for (i = 0; i < MAX_DIRECTIONS; i++) {

		if (strings_equal(name, DirectionName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid direction: %s", name);

	/* NEVER REACHED */
	return DIRECTION_NIL;
}



/*
 * returns an array which contains each direction exactly once
 * in randomised order
 */
DIRECTION * randomised_directions(DIRECTION *direction)
{
#define RD_MAX_SWAPS		32

	int i;

	for (i = 0; i < MAX_REAL_DIRECTIONS; i++) {

		direction[i] = i;
	}

	for (i = 0; i < RD_MAX_SWAPS; i++) {
		DIRECTION temp;
		int p;

		p = random_int(0, MAX_REAL_DIRECTIONS - 2);

		temp = direction[p];
		direction[p] = direction[p + 1];
		direction[p + 1] = temp;
	}

	return direction;

#undef RD_MAX_SWAPS
}



/*
 * allocate memory or die tryin'
 */
void * checked_malloc(size_t m_size)
{
	void *p;

	p = malloc(m_size);

	if (p == NULL) {

		die("*** CORE ERROR *** memory allocation failed");
	}

	return p;
}



/*
 * realloc memory or die tryin'
 */
void * checked_realloc(void *m, size_t m_size)
{
	void *p;

	p = realloc(m, m_size);

	if (p == NULL) {
		
		die("*** CORE ERROR *** memory reallocation failed");
	}

	return p;
}



/*
 * returns a divided by d always rounding up
 */
int divide_and_round_up(int a, int d)
{
	int r;

	r = a / d;

	if (a % d != 0) {
		
		++r;
	}

	return r;
}





/*
 * hex byte -> decimal (assumes ASCII)
 */
int hex_byte_to_decimal(const char *hex_byte)
{
	int d = 0;
	int m = 16;
	const char *c_p = hex_byte;

	for ( ; m >= 1; m -= 15) {
		int v;
		char c;

		c = *c_p++;

		if (c >= '0' && c <= '9') {
			v = c - '0';
		} else if (c >= 'A' && c <= 'F') {
			v = (c - 'A') + 10;
		} else {
			
			die("*** CORE ERROR *** invalid hex code (%s)", 
				hex_byte
			);

			break;
		}

		d += v * m;
	}

	return d;
}



/*
 * open a file or die tryin'
 */
FILE * open_file(const char *fn, const char *mode)
{
	FILE *f_ptr;

	f_ptr = fopen(fn, mode);

	if (f_ptr == NULL) {

		die("*** CORE ERROR *** cannot open file (%s)", fn);
	}

	return f_ptr;
}



/*
 * closes a file
 */
void close_file(FILE *f)
{

	fclose(f);
}



/*
 * checks whether a certain file exists on disk or not
 */
bool file_exists(const char *fn)
{
	FILE *fp;

	fp = fopen(fn, "rb");

	if (fp == NULL) {
		
		return false;
	}

	fclose(fp);

	return true;
}



/*
 * returns the length of a file (in bytes)
 */
long int file_length(FILE *f)
{
	long int pos;
	long int end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}



/*
 * reads a text file
 */
char * read_text_file(char *text_buffer, PROGRAM_DIRECTORY directory, 
	const char *file_name
)
{
	char line[LINE_BUFFER_SIZE];
	char newline[2];
	FILE *in_file;

	set_data_path(directory, file_name);

	clear_string(text_buffer);

	newline[0] = ASCII_LF;
	newline[1] = '\0';

	in_file = open_file(data_path(), "rb");

	while (read_line(line, LINE_BUFFER_SIZE, in_file)
		!= NULL) {

		strcat(text_buffer, line);
		strcat(text_buffer, newline);
	}

	close_file(in_file);

	return text_buffer;
}



/*
 * reads a line of ASCII text from a binary file stream
 * - supports DOS, Unix, and Mac style new line characters
 * - the new line characters aren't written to the output buffer
 * returns NULL when EOF is reached
 */
static char * read_line(char *buffer, int n, FILE *stream)
{
	int c;
	int i;

	i = 0;
	while (i < n - 1) {

		c = getc(stream);

		if (c == EOF) {

			if (i > 0) {

				break;
			}

			return NULL;
		}

		if (c == ASCII_CR) {

			c = getc(stream);

			if (c != ASCII_LF) {

				ungetc(c, stream);
			}

			break;

		} else if (c == ASCII_LF) {

			break;

		} else {

			buffer[i] = (char)c;
			++i;
		}

	}

	buffer[i] = '\0';

	return buffer;
}


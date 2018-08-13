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
 * Shake
 *
 * NAMESPACE: SHAKE_* / shake_*
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "shake.h"



/*
 * ASCII codes
 */
#define SHAKE_LF			0x0A
#define SHAKE_CR			0x0D



/*
 * limits
 */
#define SHAKE_STRING_SIZE		32
#define SHAKE_MAX_SETS			8



/*
 * buffer size
 */
#define SHAKE_BUFFER_SIZE		256


typedef int				SHAKE_STRING_INDEX;
typedef int				SHAKE_N_STRINGS;

typedef struct {
	
	char	string[SHAKE_STRING_SIZE];

	bool	used;

} SHAKE_STRING;


typedef struct {
	
	SHAKE_STRING *	string;

	int		n_strings;
	
	bool		no_duplicates;

} SHAKE_SET;



static int		shake_default_random_int(int, int);

static char *		shake_read_line(char *, int, FILE *);

static SHAKE_STRING *	shake_set_string(SHAKE_SET *, SHAKE_STRING_INDEX);

static bool		shake_set_finalize(SHAKE_SET *, SHAKE_N_STRINGS);




static int		(*shake_random_int)(int, int) = 
				shake_default_random_int;


static SHAKE_SET	SHAKE_Set[SHAKE_MAX_SETS] = {

	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false},
	{NULL, 0, false}

};



/*
 * loads a string set
 */
bool shake_load_set(SHAKE_SET_ID set_id, const char *file_path)
{
	FILE *in_file;
	SHAKE_SET *set;
	SHAKE_STRING_INDEX string_index;

	in_file = fopen(file_path, "rb");

	if (in_file == NULL) {
		
		return false;
	}
	
	set = &SHAKE_Set[set_id];
	
	if (set->string != NULL) {

		shake_unload_set(set_id);
	}

	for (string_index = 0; ; string_index++) {
		SHAKE_STRING *string;
		
		string = shake_set_string(set, string_index);
		
		if (string == NULL) {
			
			return false;
		}
		
		if (shake_read_line(string->string, 
			SHAKE_STRING_SIZE, in_file) == NULL) {
		
			break;
		}
		
		string->used = false;
	}
	
	shake_set_finalize(set, string_index);

	fclose(in_file);
	
	return true;
}



/*
 * unloads a string set
 */
void shake_unload_set(SHAKE_SET_ID set_id)
{
	SHAKE_SET *set;

	set = &SHAKE_Set[set_id];

	if (set->string != NULL) {

		free(set->string);
	}
	
	set->n_strings = 0;

	set->no_duplicates = false;
}



/*
 * sets the RNG
 */
void shake_set_rng(int (*random_int_function)(int, int))
{

	shake_random_int = random_int_function;
}



/*
 * sets "no duplicates" to true/false
 */
void shake_no_duplicates(SHAKE_SET_ID set_id, bool new_state)
{

	SHAKE_Set[set_id].no_duplicates = new_state;	
}



/*
 * returns a randomly selected string
 */
const char * shake_get_string(SHAKE_SET_ID set_id)
{
	int string_index, i;
	SHAKE_STRING *string;
	SHAKE_SET *set;

	set = &SHAKE_Set[set_id];

	string_index = (*shake_random_int)(0, set->n_strings - 1);
	
	string = &set->string[string_index];

	if (!set->no_duplicates || !string->used) {

		string->used = true;

		return string->string;
	}

	for (i = string_index + 1; i != string_index; i++) {
	
		if (i == set->n_strings) {

			i = 0;
		}
		
		string = &set->string[i];

		if (!string->used) {
		
			string->used = true;

			return string->string;
		}
	}

	return NULL;
}



/*
 * shouldn't be used: supply a quality random number function instead.
 */
static int shake_default_random_int(int min, int max)
{
	static bool initialized = false;
	int r;

	if (!initialized) {

		srand(time(NULL));

		initialized = true;
	}

	r = rand() / (RAND_MAX / (max + 1 - min));

	r += min;

	return r;
}



/*
 * reads a line of ASCII text from a binary file stream
 * - supports DOS, Unix, and Mac style new line characters
 * - the new line characters aren't written to the output buffer
 * returns NULL when EOF is reached
 */
static char * shake_read_line(char *buffer, int n, FILE *stream)
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

		if (c == SHAKE_CR) {

			c = getc(stream);

			if (c != SHAKE_LF) {
				
				ungetc(c, stream);
			}

			break;

		} else if (c == SHAKE_LF) {

			break;

		} else {

			buffer[i] = (char)c;

			++i;
		}

	}

	buffer[i] = '\0';

	return buffer;
}



/*
 * returns a pointer to the string located at the passed index 
 * in the passed set. expands the set if necessary.
 */
static SHAKE_STRING * shake_set_string(SHAKE_SET *set, 
	SHAKE_STRING_INDEX string_index
)
{
	SHAKE_STRING *string;

	if (string_index >= set->n_strings) {

		do {
		
			set->n_strings += SHAKE_BUFFER_SIZE;

			set->string = realloc(set->string, 
				set->n_strings * sizeof *(set->string)
			);
		
			if (set->string == NULL) {

				return NULL;
			}
	
		} while (string_index >= set->n_strings);
	}

	string = &set->string[string_index];

	return string;
}



/*
 * finalizes the passed shake set
 */
static bool shake_set_finalize(SHAKE_SET *set, SHAKE_N_STRINGS n_strings)
{

	set->string = realloc(set->string, n_strings * 
		sizeof *(set->string)
	);

	if (set->string == NULL) {

		return false;
	}

	set->n_strings = n_strings;
	
	return true;
}



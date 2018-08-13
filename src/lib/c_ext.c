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
 * C Extendsion Library
 */
 
/*
 * This is a small, portable library which extends the C standard library.
 * It adds some new functions and a little syntax sugar.
 */

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "c_ext.h"


#define STRING_BUFFER_SIZE	512



/*
 * like strcat, but with format support
 */
char * string_cat(char *string, const char *fmt, ...)
{
	char to_add[STRING_BUFFER_SIZE];
	va_list vl;

	va_start(vl, fmt);
	vsprintf(to_add, fmt, vl);
	va_end(vl);

	strcat(string, to_add);

	return string;
}



/*
 * makes all characters in a string lowercase
 */
char * string_to_lowercase(char *string)
{
	char *p;

	p = string;

	while (*p != '\0') {
		
		if (!islower(*p)) {

			*p = (char)tolower(*p);
		}
		
		++p;
	}

	return string;
}




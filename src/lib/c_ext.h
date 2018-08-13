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
 * string functions
 */
#define is_empty_string(s)		((s)[0] == '\0')
#define clear_string(s)			((s)[0] = '\0')
#define strings_equal(s1,s2)		(strcmp((s1),(s2)) == 0)
char *		string_cat(char *, const char *, ...);
char *		string_to_lowercase(char *);


/*
 * math functions
 */
#define sgn(x)				((x) == 0 ? 0 :((x) > 0 ? 1 : -1))
#define percent(b,p)			(((b) * (p)) / 100)


/* 
 * this macro is used to suppress some pointless compiler warnings
 */
#ifdef __LCC__
#	define NOT_USED(p)		((p) = (p))
#else
#	define NOT_USED(p)		((void)(p))
#endif




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
 * Module: UI Keys
 */

#include "wrogue.h"




#define special_key_name(n)	(strlen(n) > 1)




/*
 * key name return buffer
 */
static char KeyName[2];



/*
 * special keys
 */
static const SPECIAL_KEY	SpecialKey[N_SPECIAL_KEYS] = {

	{"Enter",	KEY_ENTER},
	{"Esc",		KEY_ESC},
	{"Up",		KEY_UP},
	{"Down",	KEY_DOWN},
	{"Left",	KEY_LEFT},

	{"Right",	KEY_RIGHT},
	{"Page Up",	KEY_PAGE_UP},
	{"Page Down",	KEY_PAGE_DOWN},
	{"F1",		KEY_F1},
	{"F2",		KEY_F2},

	{"F3",		KEY_F3},
	{"F4",		KEY_F4},
	{"F5",		KEY_F5},
	{"F6",		KEY_F6},
	{"F7",		KEY_F7},

	{"F8",		KEY_F8},
	{"F9",		KEY_F9},
	{"F10",		KEY_F10},
	{"F11",		KEY_F11},
	{"F12",		KEY_F12}

};







/*
 * name -> key code
 */
KEY_CODE name_to_key_code(const char *name)
{
	SPECIAL_KEY_INDEX i;

	if (!special_key_name(name)) {

		return (KEY_CODE)(name[0]);
	}

	for (i = 0; i < N_SPECIAL_KEYS; i++) {

		if (strings_equal(name, SpecialKey[i].name)) {

			return SpecialKey[i].key_code;

		}
	}

	die("*** CORE ERROR *** unrecognized key name (%s)", name);

	/* NEVER REACHED */
	return KEY_NIL;
}



/*
 * returns the name of a key
 */
const char * key_name(KEY_CODE key_code)
{
	SPECIAL_KEY_INDEX i;

	for (i = 0; i < N_SPECIAL_KEYS; i++) {

		if (key_code == SpecialKey[i].key_code) {

			return SpecialKey[i].name;
		}
	}

	KeyName[0] = (char)key_code;
	KeyName[1] = '\0';

	return KeyName;
}

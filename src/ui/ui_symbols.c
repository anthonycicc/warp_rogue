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
 * Module: UI Symbols
 */

#include "wrogue.h"





/*
 * custom symbol names
 */
static const char CustomSymbolName[MAX_CUSTOM_SYMBOLS][SYMBOL_NAME_SIZE] = {

	"Floor",
	"Wall",
	"UpDown",
	"Fluid",
	"Floor2",
	"Mushroom",
	"Hive city",
	"Ruins",
	"Mountain",
	"Road",
	"Entrance",
	"Corpse",
	"Wire",
	"Fortress"

};




/*
 * symbol name -> symbol
 */
SYMBOL name_to_symbol(const char *symbol_name)
{
	SYMBOL i;

	for (i = 0; i < MAX_CUSTOM_SYMBOLS; i++) {

		if (strings_equal(symbol_name, 
			CustomSymbolName[i])) {

			return CUSTOM_SYMBOL_OFFSET + i;
		}
	}

	die("*** CORE ERROR *** invalid symbol: %s", symbol_name);

	return SYM_NIL;
}


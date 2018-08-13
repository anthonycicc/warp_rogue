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
 * Module: Character Attributes
 */

#include "wrogue.h"




/*
 * character attributes data
 */
static CHARACTER_ATTRIBUTE_DATA CharacterAttribute[MAX_CHARACTER_ATTRIBUTES] = {

	{"Broken",		C_NIL},
	{"Bull",		C_NIL},
	{"Busy",		C_NIL},
	{"Cannot speak",	C_NIL},
	{"Daemonic",		C_NIL},
	{"Diseased",		C_NIL},
	{"Flash",		C_NIL},
	{"Focus",		C_NIL},
	{"Machine",		C_NIL},
	{"No unarmed attack",	C_NIL},
	{"Nomat",		C_NIL},
	{"Plant",		C_NIL},
	{"Poisoned",		C_NIL},
	{"Psychic shield",	C_NIL},
	{"Stoic",		C_NIL},
	{"Stunned",		C_NIL},
	{"Unique",		C_NIL},
	{"Unnoticed",		C_NIL},
	{"Warp strength",	C_NIL}

};



/*
 * returns the description of a character attribute
 */
char * character_attribute_description(char *description,
	CHARACTER_ATTRIBUTE attribute
)
{

	return data_file_character_attribute_description(description,
		attribute
	);
}



/*
 * the character attribute screen
 */
void character_attribute_screen(CHARACTER_ATTRIBUTE attribute)
{

	command_bar_set(1, CM_EXIT);

	render_character_attribute_screen(attribute);

	update_screen();

	command_bar_get_command();
}


/*
 * returns the name of a character attribute
 */
const char * character_attribute_name(CHARACTER_ATTRIBUTE attribute)
{

	return CharacterAttribute[attribute].name;
}



/*
 * returns the colour of a character attribute
 */
COLOUR character_attribute_colour(CHARACTER_ATTRIBUTE attribute)
{

	return CharacterAttribute[attribute].colour;
}



/*
 * sets the colour of a character attribute
 */
void character_attribute_set_colour(CHARACTER_ATTRIBUTE attribute, 
	COLOUR colour
)
{

	CharacterAttribute[attribute].colour = colour;
}



/*
 * name -> character attribute
 */
CHARACTER_ATTRIBUTE name_to_character_attribute(const char *name)
{
	CHARACTER_ATTRIBUTE i;

	for (i = 0; i < MAX_CHARACTER_ATTRIBUTES; i++) {

		if (strings_equal(name, CharacterAttribute[i].name)) {

			return i;
		}

	}

	die("*** CORE ERROR *** invalid character attribute: %s", name);

	/* NEVER REACHED */
	return 0;
}


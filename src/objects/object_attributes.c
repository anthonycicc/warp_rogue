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
 * Module: Object Attributes
 */

#include "wrogue.h"
 

 
/*
 * object attribute names
 */
static const char	ObjectAttributeName[MAX_OBJECT_ATTRIBUTES]
				[OBJECT_ATTRIBUTE_NAME_SIZE] = {

	"Alien",
	"Automatic recharge",
	"Blocks line of sight",
	"Cannot be parried",
	"Cannot parry",
	"Daemonic",
	"Dangerous",
	"Disease",
	"Environmental",
	"Force",
	"Force channel",
	"Force rune",
	"Hinders movement",
	"Ignore armour",
	"Impassable",
	"Inherent",
	"Jammed",
	"Laser",
	"Noisy",
	"Pain",
	"Partial reload",
	"Plasma",
	"Poison",
	"Power",
	"Projectile",
	"Reliable",
	"Shock",
	"Silent",
	"Strength boost",
	"Strength boost II",
	"Unique"

};




/*
 * the object attribute screen
 */
void object_attribute_screen(OBJECT_ATTRIBUTE attribute)
{

	command_bar_set(1, CM_EXIT);

	render_object_attribute_screen(attribute);

	update_screen();

	command_bar_get_command();
}



/*
 * returns the description of an object attribute
 */
char * object_attribute_description(char *description,
	OBJECT_ATTRIBUTE attribute
)
{

	return data_file_object_attribute_description(description,
		attribute
	);
}



/*
 * returns the name of an object attribute
 */
const char * object_attribute_name(OBJECT_ATTRIBUTE object_attribute)
{

	return ObjectAttributeName[object_attribute];
}



/*
 * name -> object attribute
 */
OBJECT_ATTRIBUTE name_to_object_attribute(const char *name)
{
	OBJECT_ATTRIBUTE i;

	for (i = 0; i < MAX_OBJECT_ATTRIBUTES; i++) {

		if (strings_equal(name, ObjectAttributeName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid object attribute: %s", name);

	/* NEVER REACHED */
	return OA_NIL;
}


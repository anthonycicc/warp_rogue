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
 * Module: Terrain Attributes
 */

#include "wrogue.h"





/*
 * terrain attribute names
 */
static const char	TerrainAttributeName[MAX_TERRAIN_ATTRIBUTES]
				[TERRAIN_ATTRIBUTE_NAME_SIZE] = {

	"Air",
	"Blocks line of sight",
	"Causes disease",
	"Dangerous",
	"Impassable"

};





/*
 * the terrain attribute screen
 */
void terrain_attribute_screen(TERRAIN_ATTRIBUTE attribute)
{

	command_bar_set(1, CM_EXIT);

	render_terrain_attribute_screen(attribute);

	update_screen();

	command_bar_get_command();
}



/*
 * returns the name of a terrain attribute
 */
const char * terrain_attribute_name(TERRAIN_ATTRIBUTE attribute)
{

	return TerrainAttributeName[attribute];
}



/*
 * returns the description of a terrain attribute
 */
char * terrain_attribute_description(char *description,
	TERRAIN_ATTRIBUTE attribute
)
{

	return data_file_terrain_attribute_description(description,
		attribute
	);
}



/*
 * name -> terrain attribute
 */
TERRAIN_ATTRIBUTE name_to_terrain_attribute(const char *name)
{
	TERRAIN_ATTRIBUTE i;

	for (i = 0; i < MAX_TERRAIN_ATTRIBUTES; i++) {

		if (strings_equal(name, TerrainAttributeName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid terrain attribute: %s", name);

	/* NEVER REACHED */
	return TA_NIL;
}



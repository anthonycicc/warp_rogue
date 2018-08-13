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
 * Module: Terrain
 */

#include "wrogue.h"



static void *		terrain_new_full(void);
static void		terrain_free_full(void *);

static TERRAIN_INDEX	name_to_terrain_index(const char *);




/*
 * the terrain box
 */
static BOX *		TerrainBox = NULL;

/*
 * direct link to terrain data for fast accesss
 */
static void **		Terrain;



/*
 * terrain template (default values)
 */
static const TERRAIN	TerrainTemplate = {
	
	/* NAME */ NULL, /* SYMBOL */ '.', 
	/* COLOUR */ {C_NIL, C_NIL, C_NIL, 0},
	/* GORE_LEVEL */ GORE_LEVEL_ZERO, /* DESCRIPTION */ NULL,
	/* ATTRIBUTES */ {false, false, false, false, false},
	/* SN_DATA */ SERIAL_NIL, /* SN_SAVE */ SERIAL_NIL

};






/*
 * Terrain module init
 */
void terrain_init(void)
{

	TerrainBox = box_new(terrain_new_full, terrain_free_full,
		TERRAIN_BOX_BUFFER_SIZE
	);
}



/*
 * Terrain module clean up
 */
void terrain_clean_up(void)
{

	if (TerrainBox != NULL) {

		box_free(TerrainBox);
	}
}



/*
 * adds a terrain to the terrain box
 */
TERRAIN * terrain_box_new_terrain(void)
{

	return box_new_item(TerrainBox);
}



/*
 * optimises the terrain box
 */
void terrain_box_optimise(void)
{

	box_optimise(TerrainBox);

	Terrain = TerrainBox->item;
}



/*
 * allocs a terrain
 */
void * terrain_new(void)
{
	TERRAIN *terrain;

	terrain = checked_malloc(sizeof *terrain);
	
	*terrain = TerrainTemplate;
	
	return terrain;
}



/*
 * frees a terrain
 */
void terrain_free(void *p)
{

	free(p);
}



/*
 * creates a terrain (name based)
 */
TERRAIN * terrain_create(const char *terrain_name)
{
	TERRAIN_INDEX terrain_index;
	TERRAIN *terrain;

	terrain_index = name_to_terrain_index(terrain_name);

	terrain = terrain_create_i(terrain_index);

	return terrain;
}



/*
 * creates a terrain (index based)
 */
TERRAIN * terrain_create_i(TERRAIN_INDEX terrain_index)
{
	TERRAIN *terrain = terrain_clone(Terrain[terrain_index]);

	terrain->sn_data = terrain_index;

	determine_colour(&terrain->colour);

	return terrain;
}



/*
 * destroys a terrain
 * i.e. free + handling the complex data management mess
 */
void terrain_destroy(void *p)
{
	TERRAIN *terrain;
	LIST *terrain_events;

	terrain = (TERRAIN *)p;

	terrain_events = list_new();

	remove_terrain(terrain, terrain_events);

	list_free_with(terrain_events, event_destroy);	

	terrain_free(terrain);
}



/*
 * produces an identical copy of the passed terrain
 */
TERRAIN * terrain_clone(const TERRAIN *terrain)
{
	TERRAIN *clone;

	clone = terrain_new();

	*clone = *terrain;

	return clone;
}



/*
 * the terrain screen
 */
void terrain_screen(const TERRAIN *terrain)
{

	command_bar_set(1, CM_EXIT);

	render_terrain_screen(terrain);

	update_screen();

	command_bar_get_command();
}



/*
 * disease causing terrain effect
 */
void terrain_cause_disease(CHARACTER *character)
{

	if (character->armour != NULL) {

		if (object_has_attribute(character->armour,
			OA_ENVIRONMENTAL)) {

			return;
		}
	}

	if (disease_resisted(character)) {

		if (character->party == PARTY_PLAYER) {

			dynamic_message(MSG_RESIST, character,
				NULL, MOT_NIL
			);
		}

	} else {

		effect_activate(character, ET_DISEASED, TIMER_UNDETERMINED);
	}
}



/*
 * returns true if the passed terrain is dangerous for the passed character
 */
bool terrain_dangerous_for(const CHARACTER *character, const TERRAIN *terrain)
{

	if (terrain_has_attribute(terrain, TA_AIR)) {

		return true;
	}

	if (!terrain_has_attribute(terrain, TA_CAUSES_DISEASE)) {

		return false;
	}

	if (character_has_perk(character, PK_IMMUNITY_TO_DISEASE)) {

		return false;
	}

	if (character->armour != NULL) {

		if (object_has_attribute(character->armour,
			OA_ENVIRONMENTAL)) {

			return false;
		}
	}

	return true;
}



/*
 * allocs a terrain (full version)
 */
static void * terrain_new_full(void)
{
	TERRAIN *terrain;

	terrain = checked_malloc(sizeof *terrain);
	
	*terrain = TerrainTemplate;
	
	terrain->name = checked_malloc(TERRAIN_NAME_SIZE);
	terrain->description = checked_malloc(DESCRIPTION_SIZE);

	clear_string(terrain->name);
	clear_string(terrain->description);
	
	return terrain;
}



/*
 * frees a terrain (full version)
 */
static void terrain_free_full(void *p)
{
	TERRAIN *terrain = p;

	free(terrain->description);
	free(terrain->name);
	free(terrain);
}



/*
 * name -> terrain attribute
 */
static TERRAIN_INDEX name_to_terrain_index(const char *name)
{
	TERRAIN_INDEX i;

	for (i = 0; i < TerrainBox->current_size; i++) {
		const TERRAIN *terrain;

		terrain = Terrain[i];

		if (strings_equal(name, terrain->name)) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid terrain: %s", name);

	return TERRAIN_INDEX_NIL;
}



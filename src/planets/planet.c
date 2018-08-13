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
 * Module: Planet
 */

#include "wrogue.h"




static void *			planet_tile_new_full(void);
static void			planet_tile_free_full(void *);

static bool			enter_area(const PLANET_POINT *, DIRECTION);





/*
 * planet tile template (default values)
 */
static const PLANET_TILE		PlanetTileTemplate = {

	/* NAME */ NULL, /* SCRIPT */ NULL,
	/* SYMBOL */ 'E', /* SCREEN_SYMBOL */ 'E',
	/* COLOUR */ {C_NIL, C_NIL, C_NIL, 0},
	/* IMPASSABLE */ false, /* N_LEVELS */ 0,

};



/*
 * the planet
 */
static PLANET			Planet;



/*
 * the planet tile box
 */
static BOX *			PlanetTileBox = NULL;







/*
 * Planet module init
 */
void planet_init(void)
{

	clear_string(Planet.description);

	PlanetTileBox = box_new(planet_tile_new_full, planet_tile_free_full,
		PLANET_TILE_BOX_BUFFER_SIZE
	);
}



/*
 * Planet module clean up
 */
void planet_clean_up(void)
{

	if (PlanetTileBox != NULL) {

		box_free(PlanetTileBox);
	}
}



/*
 * returns the planet
 */
PLANET * planet(void)
{

	return &Planet;
}



/*
 * returns a planet tile pointer
 */
PLANET_TILE * planet_tile(const PLANET_POINT *p)
{

	return &Planet.map[p->y][p->x];
}



/*
 * adds a tile to the planet tile box
 */
PLANET_TILE * planet_tile_box_new_tile(void)
{

	return box_new_item(PlanetTileBox);
}



/*
 * optimises the tile box
 */
void planet_tile_box_optimise(void)
{

	box_optimise(PlanetTileBox);
}





/*
 * returns true if the coordinates of both points are equal
 */
bool planet_points_equal(const PLANET_POINT *p1, const PLANET_POINT *p2)
{

	if (p1->z == p2->z && p1->y == p2->y &&
		p1->x == p2->x) {

		return true;
	}

	return false;
}



/*
 * moves a planet point one step in the passed direction
 */
void move_planet_point(PLANET_POINT *planet_point, DIRECTION direction)
{

	planet_point->x += direction_modifier(COORD_X, direction);
	planet_point->y += direction_modifier(COORD_Y, direction);
}



/*
 * returns true if the passed point is out of bounds
 */
bool out_of_planet_bounds(const PLANET_POINT *p)
{

	if (p->y < 0 || p->x < 0 ||
		p->y >= PLANET_HEIGHT || p->x >= PLANET_WIDTH) {

		return true;
	}

	return false;
}



/*
 * planet map screen
 */
void planet_map_screen(CHARACTER *character)
{
	COMMAND entered_command;
	DIRECTION direction;
	PLANET_POINT position;

	position = active_area()->location;

	command_bar_clear();
	command_bar_add_move_commands();

	while (true) {
		PLANET_POINT previous_position;

		render_planet_map_screen(&position, character);

		update_screen();

		entered_command = command_bar_get_command();

		move_command_to_direction(&direction, entered_command);

		previous_position = position;

		move_planet_point(&position, direction);

		if (out_of_planet_bounds(&position) ||
			planet_tile(&position)->impassable) {

			position = previous_position;
			continue;
		}

		if (area_exists(&position)) {

			if (enter_area(&position, direction)) break;

			position = previous_position;
		}
	}
}



/*
 * symbol -> planet tile
 */
const PLANET_TILE * symbol_to_planet_tile(SYMBOL symbol)
{
	BOX_ITEM_INDEX i;

	for (i = 0; i < PlanetTileBox->current_size; i++) {
		const PLANET_TILE *tile = PlanetTileBox->item[i];

		if (tile->symbol == symbol) return tile;
	}

	die("*** CORE ERROR *** invalid planet map symbol: %c", symbol);

	/* NEVER REACHED */
	return NULL;
}



/*
 * allocs a planet tile (full version)
 */
static void * planet_tile_new_full(void)
{
	PLANET_TILE *tile;

	tile = checked_malloc(sizeof *tile);

	*tile = PlanetTileTemplate;

	tile->name = checked_malloc(AREA_NAME_SIZE);
	tile->script = checked_malloc(FILE_NAME_SIZE);

	clear_string(tile->name);
	clear_string(tile->script);

	return tile;
}



/*
 * frees a planet tile (full version)
 */
static void planet_tile_free_full(void *data)
{
	PLANET_TILE *tile = data;

	free(tile->name);
	free(tile->script);

	free(tile);
}



/*
 * ask the player whether or not he wants to enter an area
 * if he confirms, his party is transfered to the given area
 * returns true if the party has been transfered
 */
static bool enter_area(const PLANET_POINT *position, DIRECTION direction)
{
	CONFIRMATION_DIALOGUE dialogue;

	/* ask for confirmation */
	sprintf(dialogue.prompt, "Enter %s?",
		Planet.map[position->y][position->x].name
	);
	enter_area_dialogue(&dialogue);
	if (!dialogue.confirmed) return false;

	/* move party */
	move_party_to_area(position, direction, false);
	update_perception_data(player_controlled_character()); /* hack */

	return true;
}




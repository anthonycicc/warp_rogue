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
 * Module: Area Generation
 */

#include "wrogue.h"


static void	add_connection(OBJECT *);





/*
 * adds connections
 */
void add_connections(const AREA_SECTION *bounds,
	const char *way_up, const char *way_down
)
{
	OBJECT *connection;
	AREA *area;

	area = active_area();

	sector_set_bounds(bounds);

	if (area->location.z > PLANET_SURFACE_Z) {

		connection = object_create(way_down);
		add_connection(connection);
	}

	if (area->location.z <
		planet_tile(&area->location)->n_levels - 1) {

		connection = object_create(way_up);
		add_connection(connection);
	}

	sector_reset_bounds();
}



/*
 * adds the environment
 */
void add_environment(const char *terrain_name, const char *object_name)
{
	AREA_POINT p;
	const AREA_SECTION *bounds;

	bounds = area_bounds();

	for (p.y = bounds->top; p.y <= bounds->bottom; p.y++) {
	for (p.x = bounds->left; p.x <= bounds->right; p.x++) {
		TERRAIN *terrain;

		if (sector_at(&p)->terrain != NULL) {

			continue;
		}

		terrain = terrain_create(terrain_name);
		place_terrain(terrain, &p);

		if (!is_empty_string(object_name)) {
			OBJECT *object;

			object = object_create(object_name);
			place_object(object, &p);
		}
	}
	}
}



/*
 * builds a gate
 */
void build_gate(AREA_POINT *c, AREA_COORD height, AREA_COORD side_ex)
{
	AREA_COORD y_mod;
	bool connection_reached;

	y_mod = 0;
	connection_reached = false;

	do {
		AREA_COORD x_mod;

		for (x_mod = -(side_ex);
			x_mod <= side_ex;
			x_mod++) {
			OBJECT *object;
			AREA_POINT p;

			p.y = c->y + y_mod;
			p.x = c->x + x_mod;

			object = object_at(&p);

			if (object == NULL) {

				connection_reached = true;

			} else {

				object_destroy(object);

				sector_at(&p)->object = NULL;
			}
		}

		--y_mod;

	} while (!connection_reached ||
		y_mod > -(height)
	);
}



/*
 * spawns a character in the currently active area
 */
bool spawn_character(const char *character_name)
{
	CHARACTER *character;
	const AREA_POINT *spawn_point;

	if (is_empty_string(character_name)) return false;

	spawn_point = random_sector(SC_MOVE_TARGET_SAFE);
	if (spawn_point == NULL) return false;

	character = character_create(character_name);

	place_character(character, spawn_point);

	return true;
}



/*
 * spawns an object in the currently active area
 *
 * do NOT use for objects which can block a path
 */
bool spawn_object(const char *object_name)
{
	const AREA_POINT *spawn_point;
	OBJECT *object;

	if (is_empty_string(object_name)) {

		die("*** CORE ERROR *** invalid spawn_object call");
		return false; /* never reached */
	}

	/* choose random suitable sector */
	spawn_point = random_sector(SC_FREE_OBJECT_LOCATION);
	
	/* no suitable sector - return false */
	if (spawn_point == NULL) return false;

	/* create object */
	object = object_create(object_name);

	/* place object */
	place_object(object, spawn_point);

	return true;
}



/*
 * adds a connection
 */
static void add_connection(OBJECT *connection)
{
	const AREA_POINT *location;
	SECTOR_CLASS wanted_class = SC_FREE_OBJECT_LOCATION;

	if (object_has_attribute(connection, OA_IMPASSABLE)) {

		wanted_class = SC_FREE_OBJECT_LOCATION_ANTIBLOCK;
	}

	location = random_sector(wanted_class);
	if (location == NULL) {

		/* this should never happen ... */
		die("*** CORE ERROR *** could not place connection object");
	}

	place_object(connection, location);
}


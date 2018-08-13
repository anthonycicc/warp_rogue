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
 * Module: Area Cave
 */

#include "wrogue.h"




static void		cave_generate_sector(const AREA_POINT *,
				CCD_TILE
			);

static void		cave_build_gate(void);

static void		cave_add_environment(const DUNGEON_ENVIRONMENT *);


/*
 * cave generator configuartion
 */
static CAVE_CONFIG		CaveConfig;



/*
 * cave generator configuration template (default values)
 */
static const CAVE_CONFIG	CaveConfigTemplate = {

	/* TYPE */ CCD_DT_CAVE,
	/* WALL */ "", /* FLOOR */ "",
	/* WAY_UP */ "", /* WAY_DOWN */ "",
	/* GATE_LEVEL */ PLANET_COORD_NIL,
	/* ENVIRONMENT */ {"", "", "", "", "", "", "", ""}

};




/*
 * the area section occupied by the cave
 */
static AREA_SECTION	CaveArea = {

	CAVE_Y_MIN, CAVE_Y_MAX,
	CAVE_X_MIN, CAVE_X_MAX

};



/*
 * generates a cave area
 */
void cave_generate(void)
{
	AREA *area;
	CCD_DUNGEON *cave;
	AREA_POINT p;

	area = active_area();

	cave = ccd_build_dungeon(CAVE_HEIGHT, CAVE_WIDTH,
		CaveConfig.type
	);

	for (p.y = CaveArea.top; p.y <= CaveArea.bottom; p.y++) {
	for (p.x = CaveArea.left; p.x <= CaveArea.right; p.x++) {
		AREA_COORD y, x;

		y = p.y - CAVE_Y_MIN;
		x = p.x - CAVE_X_MIN;

		cave_generate_sector(&p, cave->map[y][x]);
	}
	}

	ccd_free_dungeon(cave);

	if (area->location.z == CaveConfig.gate_level) {

		cave_build_gate();
	}

	cave_add_environment(&CaveConfig.environment);

	add_connections(&CaveArea, CaveConfig.way_up,
		CaveConfig.way_down
	);
}



/*
 * resets the cave generator
 */
void cave_reset(void)
{

	CaveConfig = CaveConfigTemplate;
}



/*
 * sets the cave type
 */
void cave_set_type(const char *type_name)
{
	
	if (strings_equal(type_name, "Tunnels")) {
	
		CaveConfig.type = CCD_DT_TUNNELS;	

	} else if (strings_equal(type_name, "Standard")) {
	
		CaveConfig.type = CCD_DT_CAVE;
		
	} else {

		die("*** CORE ERROR *** invalid cave type: %s", type_name);
	}
}



/*
 * sets the wall object
 */
void cave_set_wall(const char *object)
{

	strcpy(CaveConfig.wall, object);
}



/*
 * sets the floor terrain
 */
void cave_set_floor(const char *terrain)
{

	strcpy(CaveConfig.floor, terrain);
}



/*
 * sets the connection objects
 */
void cave_set_connections(const char *way_up, const char *way_down)
{

	strcpy(CaveConfig.way_up, way_up);
	strcpy(CaveConfig.way_down, way_down);
}



/*
 * sets the gate level
 */
void cave_set_gate_level(PLANET_COORD level)
{

	CaveConfig.gate_level = level;
}



/*
 * sets the environment
 */
void cave_set_environment(const DUNGEON_ENVIRONMENT *environment)
{

	CaveConfig.environment = *environment;
}



/*
 * generates a cave sector
 */
static void cave_generate_sector(const AREA_POINT *p,
	CCD_TILE dungeon_tile
)
{
	TERRAIN *terrain;
	OBJECT *object;

	object = NULL;

	terrain = terrain_create(CaveConfig.floor);
	place_terrain(terrain, p);

	if (dungeon_tile == CCD_TILE_FLOOR) return;

	if (dungeon_tile == CCD_TILE_WALL) {

		object = object_create(CaveConfig.wall);
	}

	if (object != NULL) {

		place_object(object, p);
	}
}



/*
 * builds the cave gate
 */
static void cave_build_gate(void)
{
	AREA_POINT c;

	c.y = CaveArea.bottom;
	c.x = (CAVE_WIDTH / 2) + CAVE_X_MIN;

	build_gate(&c, CAVE_GATE_HEIGHT, CAVE_GATE_SIDE_EX);
}



/*
 * adds the environment
 */
static void cave_add_environment(const DUNGEON_ENVIRONMENT *environment)
{
	AREA_POINT point;
	const AREA_SECTION *bounds;

	bounds = area_bounds();

	for (point.y = bounds->top; point.y <= bounds->bottom; point.y++) {
	for (point.x = bounds->left; point.x <= bounds->right; point.x++) {
		TERRAIN *terrain;
		const char *terrain_name;
		const char *object_name;

		if (sector_at(&point)->terrain != NULL) {

			continue;
		}

		if (point.y < CaveArea.top) {
			terrain_name = environment->terrain_top;
			object_name = environment->object_top;
		} else if (point.y > CaveArea.bottom) {
			terrain_name = environment->terrain_bottom;
			object_name = environment->object_bottom;
		} else if (point.x < CaveArea.left) {
			terrain_name = environment->terrain_left;
			object_name = environment->object_left;
		} else {
			terrain_name = environment->terrain_right;
			object_name = environment->object_right;
		}

		terrain = terrain_create(terrain_name);
		place_terrain(terrain, &point);

		if (!is_empty_string(object_name)) {
			OBJECT *object = object_create(object_name);
			place_object(object, &point);
		}
	}
	}
}









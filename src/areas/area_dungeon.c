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
 * Module: Area Dungeon
 */

#include "wrogue.h"

#include "cellpnt.h"




static void		dungeon_generate_sector(const AREA_POINT *,
				CPN_TILE
			);

static void		dungeon_build_gate(void);

static void		add_pools(void);
static void		add_pool(void);



/*
 * dungeon generator configuartion
 */
static DUNGEON_CONFIG		DungeonConfig;



/*
 * dungeon generator configuration template (default values)
 */
static const DUNGEON_CONFIG	DungeonConfigTemplate = {

	/* WALL */ "", /* FLOOR */ "",
	/* WAY_UP */ "", /* WAY_DOWN */ "",
	/* OBSTACLE */ "", /* OBSTACLE2 */ "",
	/* POOL */ "", /* GATE_LEVEL */ PLANET_COORD_NIL,
	/* ENV_TERRAIN */ "", /* EVN_OBJECT */ ""

};



/*
 * the area section occupied by the dungeon
 */
static const AREA_SECTION	DungeonArea = {

	DUNGEON_Y_MIN, DUNGEON_Y_MAX,
	DUNGEON_X_MIN, DUNGEON_X_MAX

};




/*
 * generates a dungeon level
 */
void dungeon_generate(void)
{
	AREA *area;
	CPN_DUNGEON *dungeon_map;
	AREA_COORD y, x;

	area = active_area();

	dungeon_map = cpn_build_dungeon(5, 5, NULL);

	if (dungeon_map == NULL) {

		die(cpn_get_error());
	}

	for (y = 0; y < dungeon_map->height; y++) {
	for (x = 0; x < dungeon_map->width; x++) {
		AREA_POINT p;

		p.y = y + DUNGEON_Y_MIN;
		p.x = x + DUNGEON_X_MIN;

		dungeon_generate_sector(&p, dungeon_map->map[y][x]);
	}
	}

	cpn_free_dungeon(dungeon_map);

	if (area->location.z == DungeonConfig.gate_level) {

		dungeon_build_gate();
	}

	add_environment(DungeonConfig.env_terrain,
		DungeonConfig.env_object
	);

	add_connections(&DungeonArea, DungeonConfig.way_up,
		DungeonConfig.way_down
	);

	/* add pools if requested */
	if (!is_empty_string(DungeonConfig.pool)) add_pools();
}



/*
 * resets the dungeon generator
 */
void dungeon_reset(void)
{

	DungeonConfig = DungeonConfigTemplate;
}



/*
 * sets the wall object
 */
void dungeon_set_wall(const char *object)
{

	strcpy(DungeonConfig.wall, object);
}



/*
 * sets the floor terrain
 */
void dungeon_set_floor(const char *terrain)
{

	strcpy(DungeonConfig.floor, terrain);
}



/*
 * sets the connection objects
 */
void dungeon_set_connections(const char *way_up, const char *way_down)
{

	strcpy(DungeonConfig.way_up, way_up);
	strcpy(DungeonConfig.way_down, way_down);
}



/*
 * sets the obstacle objects
 */
void dungeon_set_obstacles(const char *obstacle, const char *obstacle2)
{

	strcpy(DungeonConfig.obstacle, obstacle);
	strcpy(DungeonConfig.obstacle2, obstacle2);
}



/*
 * sets the pool terrain
 */
void dungeon_set_pools(const char *terrain)
{

	strcpy(DungeonConfig.pool, terrain);
}



/*
 * sets the gate level
 */
void dungeon_set_gate_level(PLANET_COORD level)
{

	DungeonConfig.gate_level = level;
}



/*
 * sets the environment
 */
void dungeon_set_environment(const char *terrain, const char *object)
{

	strcpy(DungeonConfig.env_terrain, terrain);

	clear_string(DungeonConfig.env_object);

	if (object != NULL) {

		strcpy(DungeonConfig.env_object, object);
	}
}



/*
 * generates a hive city sector
 */
static void dungeon_generate_sector(const AREA_POINT *p,
	CPN_TILE dungeon_tile
)
{
	TERRAIN *terrain;
	OBJECT *object = NULL;

	terrain = terrain_create(DungeonConfig.floor);
	place_terrain(terrain, p);

	if (dungeon_tile == CPN_FLOOR) return;

	if (dungeon_tile == CPN_WALL) {

		object = object_create(DungeonConfig.wall);

		place_object(object, p);

		return;
	}

	/* no obstacles set, do not place them */
	if (is_empty_string(DungeonConfig.obstacle)) return;

	if (dungeon_tile == CPN_DESTRUCTABLE_OBSTACLE) {

		object = object_create(DungeonConfig.obstacle);

	} else if (dungeon_tile == CPN_DESTRUCTABLE_OBSTACLE_2) {

		/* if obstacle2 is not set, use obstacle instead */
		if (is_empty_string(DungeonConfig.obstacle2)) {

			object = object_create(DungeonConfig.obstacle);

		} else {

			object = object_create(DungeonConfig.obstacle2);
		}
	}

	place_object(object, p);
}



/*
 * adds pools
 */
static void add_pools(void)
{
	int i;

	for (i = random_int(MIN_POOLS, MAX_POOLS);
		i > 0;
		--i) {

		add_pool();
	}
}



/*
 * adds a pool
 */
static void add_pool(void)
{
	TERRAIN *terrain;
	const AREA_POINT *pool_centre;
	DIRECTION direction[MAX_REAL_DIRECTIONS];
	int i, j;

	pool_centre = random_sector(SC_FREE);
	if (pool_centre == NULL) return;

	terrain_destroy(terrain_at(pool_centre));
	sector_at(pool_centre)->terrain = NULL;

	terrain = terrain_create(DungeonConfig.pool);
	place_terrain(terrain, pool_centre);

	randomised_directions(direction);

	for (i = random_int(MIN_POOL_EXTENSION,
		MAX_POOL_EXTENSION), j = 0;
		i > 0;
		--i, ++j) {
		AREA_POINT pool_extension;

		pool_extension = *pool_centre;
		move_area_point(&pool_extension, direction[j]);

		if (out_of_area_bounds(&pool_extension)) {

			continue;
		}

		if (sector_is_class(&pool_extension, SC_FREE)) {

			terrain_destroy(terrain_at(&pool_extension));
			sector_at(&pool_extension)->terrain = NULL;

			terrain = terrain_create(DungeonConfig.pool);
			place_terrain(terrain, &pool_extension);
		}
	}
}



/*
 * builds the dungeon gate
 */
static void dungeon_build_gate(void)
{
	AREA_POINT c;

	c.y = DungeonArea.bottom;
	c.x = DungeonArea.left +
		((DungeonArea.right - DUNGEON_X_MIN) / 2);

	build_gate(&c, DUNGEON_GATE_HEIGHT, DUNGEON_GATE_SIDE_EX);
}




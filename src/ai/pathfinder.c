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
 * Module: Pathfinder
 */

#include "wrogue.h"

#include "path.h"



static int			obstacle_at(int, int);



/*
 * the current (i.e. most recently calculated) path
 */
static PATH_NODE *		Path = NULL;
static AREA_DISTANCE		PathLength;



/*
 * the current configuration of the pathfinder 
 */
static PATHFINDER_CONFIG	Config;




/*
 * Pathfinder module init
 */
void pathfinder_init(void)
{
	const AREA_SECTION *bounds;
	
	Path = checked_malloc(PATHFINDER_MAX_PATH * sizeof *Path);

	bounds = area_bounds();

	ptf_setup(bounds->bottom + 1, bounds->right + 1, 
		PATHFINDER_MAX_PATH, obstacle_at
	);
}



/*
 * Pathfinder module clean up
 */
void pathfinder_clean_up(void)
{

	ptf_clean_up();

	if (Path != NULL) free(Path);
}



/*
 * configurates the pathfinder
 */
void pathfinder_config(const PATHFINDER_CONFIG *config)
{

	Config = *config;
}



/*
 * tries to find a path (uses the PTF pathfinding lib)
 */
const PATH_NODE * pathfinder_find_path(void)
{
	const PTF_PATH_NODE *path = ptf_find_path(
		Config.start->y, Config.start->x, 
		Config.target->y, Config.target->x
	);
	const PTF_PATH_NODE *node;
	int i;	

	if (path == NULL) return NULL;
	
	for (node = path, i = 0;
		node != NULL && i < PATHFINDER_MAX_PATH;
		node = node->parent, i++) {

		Path[i].p.y = node->y;
		Path[i].p.x = node->x;

		if (i < PATHFINDER_MAX_PATH - 1) {
			
			Path[i].parent = &Path[i + 1];
		}
	}

	Path[i - 1].parent = NULL;
	PathLength = i;

	return Path;
}



/*
 * returns the current path
 */
const PATH_NODE * pathfinder_path(void)
{

	return Path;
}



/*
 * returns the length of the current path
 */
AREA_DISTANCE pathfinder_path_length(void)
{
		
	return PathLength;
}



/*
 * returns true if there is a destructable obstacle at the passed
 * point
 *
 * this is an AI specific function; it only returns true if the
 * AI considers the obstacle destructable i.e. if its condition 
 * is below a certain limit
 *
 */
bool destructable_obstacle_at(const AREA_POINT *point)
{
	const OBJECT *object = object_at(point);

	if (object != NULL && 
		object_has_attribute(object, OA_IMPASSABLE) &&
		object->condition != CONDITION_INDESTRUCTABLE &&
		object->condition <= AI_OBSTACLE_DESTRUCTION_MAX) {

		return true;
	}

	return false;
}



/*
 * returns 1 if there is an obstacle at the passed point
 * or 0 if there is no obstacle
 */
static int obstacle_at(int y, int x)
{
	AREA_POINT point;
	SECTOR *sector;
	TERRAIN *terrain;
	OBJECT *object;
	CHARACTER *character;

	point.y = y;
	point.x = x;
	
	if (area_points_equal(&point, Config.start)) {

		return false;
	}
	
	sector = sector_at(&point);
	
	character = sector->character;
	if (!Config.ignore_characters && character != NULL) {

		return true;
	}

	object = sector->object;
	if (object != NULL && object_has_attribute(object, OA_IMPASSABLE)) {
		
		if (!Config.ignore_destructable || 
			!destructable_obstacle_at(&point)) {

			return true;
		}
	}

	terrain = sector->terrain;
	if (terrain != NULL) {

		if (terrain_has_attribute(terrain, TA_IMPASSABLE)) {

			return true;
		}

		if (!Config.ignore_dangerous &&
			terrain_has_attribute(terrain, TA_DANGEROUS)) {

			return terrain_dangerous_for(Config.subject,
				terrain
			);
		}
	}

	return false;
}



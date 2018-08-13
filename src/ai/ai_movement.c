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
 * Module: AI Movement
 */

#include "wrogue.h"



static bool			ai_character_blocks_path(AI_CONTROL_DATA *);

static const PATH_NODE *	find_ai_path_safe(AI_CONTROL_DATA *);
static const PATH_NODE *	find_ai_path_any(AI_CONTROL_DATA *);
static const PATH_NODE *	find_ai_path_bypass_characters(
					AI_CONTROL_DATA *
				);
static const PATH_NODE *	find_ai_path_combat(AI_CONTROL_DATA *);
static const PATH_NODE *	find_ai_path_follow(AI_CONTROL_DATA *);




/*
 * tries to find a path leading to the target of the AI
 *
 * if a path is found, a pointer to the path is stored in ai->path
 * otherwise ai->path will be a NULL pointer
 */
const PATH_NODE * ai_find_path(AI_CONTROL_DATA *ai)
{

	/* default value for target_reachable is false */
	ai->target_reachable = false;
	
	/* find path - rules dependent on state */
	if (ai->state == AI_STATE_PATROL) {
	
		ai->path = find_ai_path_safe(ai);

	} else if (ai->state == AI_STATE_COMBAT || 
		ai->state == AI_STATE_SEARCH) {

		ai->path = find_ai_path_combat(ai);

	} else if (ai->state == AI_STATE_FOLLOW) {
		
		ai->path = find_ai_path_follow(ai);

	} else {

		ai->path = NULL;
	}

	/* no path found - return */
	if (ai->path == NULL) return NULL;

	/* target seems to be reachable .. */
	ai->target_reachable = true;

	/* the path is blocked by a character! */
	if (ai_character_blocks_path(ai)) {
		
		/* try to find a path which bypasses characters */
		const PATH_NODE *path = find_ai_path_bypass_characters(ai);

		/* no path found - target not reachable */
		if (path == NULL) {

			ai->target_reachable = false;
	
		/* path found - use this path */
		} else {

			ai->path = path;
		}		
	}

	return ai->path;
}




/*
 * makes the AI attempt to run towards its target
 * returns true if the attempt succeeded
 */
bool ai_run_towards_target(AI_CONTROL_DATA *ai)
{

	/* no path - run false */
	if (ai->path == NULL) return false;

	/* path blocked by a destructable obstacle */
	if (destructable_obstacle_at(path_first_step(ai->path))) {

		/* destroy obstacle */
		ai_set_target(ai, path_first_step(ai->path));
		ai_attack(ai);
		return true;
	}

	/* attempt to run .. */
	if (action_run(ai->self, ai->path, run_factor(ai->self))) {
	
		/* run succeeded .. return true */
		return true;
	}

	/* run failed .. return false */
	return false;
}



/*
 * changes the coordinates of area point 'a' by moving it
 * one step away from area point 'b'
 */
void ai_flee(AREA_POINT *a, const AREA_POINT *b)
{
	
	if (a->y > b->y) {

		++a->y;

	} else if (a->y < b->y) {

		--a->y;
	}

	if (a->x > b->x) {

		++a->x;

	} else if (a->x < b->x) {

		--a->x;
	}
}



/*
 * returns true if a character blocks the path of the AI
 */
static bool ai_character_blocks_path(AI_CONTROL_DATA *ai)
{
	const AREA_POINT *first_step = path_first_step(ai->path);
	const SECTOR *sector = sector_at(first_step);

	if (sector->character == NULL ||
		can_push_past(ai->self, sector->character)) {
		
		return false;
	}

	return true;
}



/*
 * tries to find a safe path for the AI
 */
static const PATH_NODE * find_ai_path_safe(AI_CONTROL_DATA *ai)
{
	PATHFINDER_CONFIG config;
	
	config.subject = ai->self;
	config.start = &ai->self->location;
	config.target = &ai->target_point;
	config.ignore_dangerous = false;
	config.ignore_destructable = false;
	config.ignore_characters = true;

	pathfinder_config(&config);

	return pathfinder_find_path();
}



/*
 * tries to find a path for the AI, ignores dangerous terrain
 * and destructable obstacles
 */
static const PATH_NODE * find_ai_path_any(AI_CONTROL_DATA *ai)
{
	PATHFINDER_CONFIG config;
	
	config.subject = ai->self;
	config.start = &ai->self->location;
	config.target = &ai->target_point;
	config.ignore_dangerous = true;
	config.ignore_destructable = true;
	config.ignore_characters = true;

	pathfinder_config(&config);

	return pathfinder_find_path();
}



/*
 * tries to find a safe path for the AI which bypasses characters
 */
static const PATH_NODE * find_ai_path_bypass_characters(
	AI_CONTROL_DATA *ai
)
{
	PATHFINDER_CONFIG config;

	config.subject = ai->self;
	config.start = &ai->self->location;
	config.target = &ai->target_point;
	config.ignore_dangerous = false;
	config.ignore_destructable = false;
	config.ignore_characters = false;
	
	pathfinder_config(&config);

	return pathfinder_find_path();
}



/*
 * tries to find a path for the AI
 *
 * this version may return dangerous paths if no safe path is available
 */
static const PATH_NODE * find_ai_path_combat(AI_CONTROL_DATA *ai)
{	

	if (find_ai_path_safe(ai) != NULL &&
		pathfinder_path_length() <= PATHFINDER_MAX_SHORT_PATH) {

		return pathfinder_path();
	}
	
	if (find_ai_path_any(ai) != NULL) {
			
		return pathfinder_path();
	}

	return NULL;
}



/*
 * tries to find a path for the AI
 *
 * this version may return dangerous paths if no safe path is available,
 * or if the safe path is long while the dangerous path is short
 */
static const PATH_NODE * find_ai_path_follow(AI_CONTROL_DATA *ai)
{
	AREA_DISTANCE safe_path_length;

	if (find_ai_path_safe(ai) == NULL) return find_ai_path_any(ai);
		
	safe_path_length = pathfinder_path_length();
	
	if (safe_path_length <= PATHFINDER_MAX_SHORT_PATH) {
		
		return pathfinder_path();
	}
	
	if (find_ai_path_any(ai) != NULL &&
		pathfinder_path_length() <= PATHFINDER_MAX_SHORT_PATH) {
			
		return pathfinder_path();
	}

	return find_ai_path_safe(ai);
}


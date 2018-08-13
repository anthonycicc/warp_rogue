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
 * Module: AI Target
 */

#include "wrogue.h"




/*
 * sets the target of the AI, and updates the related AI data
 */
void ai_set_target(AI_CONTROL_DATA *ai, const AREA_POINT *target_point)
{

	/* set target point */
	ai->target_point = *target_point;
	
	/* determine target distance */
	ai->target_distance = area_distance(
		&ai->self->location, 
		&ai->target_point
	);

	/* try to find a path leading to the target */
	ai_find_path(ai);
}



/*
 * returns true if the AI has a target
 */
bool ai_has_target(AI_CONTROL_DATA *ai)
{

	if (AREA_POINT_NOT_DEFINED(&ai->target_point)) {

		return false;
	}

	return true;
}



/*
 * sets the target of the AI
 */
void ai_erase_target(AI_CONTROL_DATA *ai)
{

	ai->target_point = *area_point_nil();
}



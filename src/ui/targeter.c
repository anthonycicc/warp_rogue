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
 * Module: Targeter
 */

#include "wrogue.h"



static bool 	advanced_target_mode(const CHARACTER *, TARGET_MODE,
			AREA_POINT *
		);

static bool 	direction_based_targeter(const CHARACTER *, TARGET_MODE, 
			int, DIRECTION, AREA_POINT *
		);

static bool	targeter_target_at(const AREA_POINT *, TARGET_MODE);

static bool	targeter_obstacle_at(const AREA_POINT *, TARGET_MODE);




/*
 * lets a character pick a target
 */
bool pick_target(const char *prompt, const CHARACTER *character, 
	TARGET_MODE target_mode, int max_range, AREA_POINT *target_point
)
{
	COMMAND entered_command;

	if (max_range > 1) {

		if (target_mode == TMODE_SHOOT) {

			command_bar_set(3, CM_ADV_TARGET_MODE,
				CM_NEAREST_ENEMY, CM_EXIT
			);

		} else if (target_mode == TMODE_PSY_POWER) {

			command_bar_set(4, CM_ADV_TARGET_MODE,
				CM_NEAREST_ENEMY, CM_CURRENT_TILE, CM_EXIT
			);

		} else {

			command_bar_set(2, CM_ADV_TARGET_MODE, CM_EXIT);
		}

	} else {

		if (target_mode == TMODE_TRIGGER) {

			command_bar_set(2, CM_CURRENT_TILE, CM_EXIT);

		} else {

			command_bar_set(1, CM_EXIT);
		}
	}
	command_bar_add_move_commands();

	if (!macro_is_being_executed()) {

		render_pick_target_screen(prompt);
		
		update_screen();
	}

	entered_command = command_bar_get_command();

	if (is_move_command(entered_command)) {
		DIRECTION direction;

		move_command_to_direction(&direction, entered_command);

		return direction_based_targeter(character, target_mode,
			max_range, direction, target_point
		);

	} else if (entered_command == CM_NEAREST_ENEMY) {
		CHARACTER *nearest_enemy;

		nearest_enemy = nearest_noticed_enemy(character);

		if (nearest_enemy == NULL) {
			
			message(SID_GAME, 
				"No target in line of sight."
			);
			
			return false;
		}

		*target_point = nearest_enemy->location;
		
		return true;

	} else if (entered_command == CM_ADV_TARGET_MODE) {

		return advanced_target_mode(character, target_mode,
			target_point
		);

	} else if (entered_command == CM_CURRENT_TILE) {

		*target_point = character->location;

		return true;
	}

	return false;
}



/*
 * the advanced target mode
 */
static bool advanced_target_mode(const CHARACTER *character, 
	TARGET_MODE target_mode, AREA_POINT *target_point
)
{
	COMMAND entered_command;
	AREA_POINT area_point;

	command_bar_set(2, CM_OK, CM_EXIT);
	command_bar_add_move_commands();

	area_point = character->location;

	do {

		render_advanced_target_mode_screen(character, &area_point, 
			target_mode
		);
		update_screen();

		entered_command = command_bar_get_command();

		if (is_move_command(entered_command)) {
			DIRECTION direction;
			AREA_POINT previous_point;

			move_command_to_direction(&direction, 
				entered_command
			);

			previous_point = area_point;

			move_area_point(&area_point, direction);

			if (out_of_area_bounds(&area_point)) {

				area_point = previous_point;
			}
		}

	} while (entered_command != CM_OK && entered_command != CM_EXIT);

	if (entered_command == CM_EXIT) return false;

	if (area_points_equal(&character->location, &area_point)) {

		if (target_mode == TMODE_SHOOT) {

			message(SID_GAME, "Suicide?!");

			return false;

		} else if (target_mode == TMODE_PSY_POWER) {

			*target_point = area_point;
			return true;
		}
	}

	if ((target_mode == TMODE_SHOOT || target_mode == TMODE_PSY_POWER)
		&& !los(&character->location, &area_point)) {

		message(SID_GAME, "No line of sight.");
		return false;
	}

	*target_point = area_point;
	return true;
}



/*
 * the direction based targeter
 */
static bool direction_based_targeter(const CHARACTER *character,
	TARGET_MODE target_mode, int max_range, DIRECTION direction,
	AREA_POINT *target_point
)
{
	bool target_found;
	AREA_POINT area_point;
	AREA_POINT previous_point;
	int i;

	target_found = false;
	area_point = character->location;

	for (i = 1; i <= max_range && !target_found; i++) {

		previous_point = area_point;

		move_area_point(&area_point, direction);

		if (out_of_area_bounds(&area_point)) break;

		target_found = targeter_target_at(&area_point,
			target_mode
		);

		if (i != max_range &&
			(target_mode == TMODE_RUN ||
			target_mode == TMODE_JUMP)) {

			target_found = false;
		}

		if (target_found) break;

		if (targeter_obstacle_at(&area_point, target_mode)) {

			area_point = previous_point;

			target_found = targeter_target_at(
				&area_point, target_mode
			);

			break;
		}
	}

	if (!target_found) {

		message(SID_GAME, "No target.");
		return false;
	}

	*target_point = area_point;

	return true;
}



/*
 * returns true if there is a target at the passed point
 * what constitutes a 'target' depends on the passed target mode
 */
static bool targeter_target_at(const AREA_POINT *area_point, 
	TARGET_MODE target_mode
)
{

	if (target_mode == TMODE_TRIGGER ||
		target_mode == TMODE_STRIKE ||
		target_mode == TMODE_DISARM) {

		return true;
	}

	if (target_mode == TMODE_SHOOT || target_mode == TMODE_PSY_POWER) {
		const SECTOR *sector;

		sector = sector_at(area_point);

		if (sector->character != NULL) {

			return true;
		}

		if (sector->object != NULL &&
			object_has_attribute(sector->object, OA_IMPASSABLE)
		) {

			return true;
		}

	} else if (target_mode == TMODE_RUN || target_mode == TMODE_JUMP) {

		if (sector_is_class(area_point, SC_MOVE_TARGET)) {

			return true;
		}
	}

	return false;
}



/*
 * returns true if there is an obstacle at the passed point
 * what constitutes an 'obstacle' depends on the passed target mode
 */
static bool targeter_obstacle_at(const AREA_POINT *area_point, 
	TARGET_MODE target_mode
)
{

	if (target_mode == TMODE_RUN) {

		if (!sector_is_class(area_point, SC_MOVE_TARGET)) {

			return true;
		}
	}

	if (target_mode == TMODE_SHOOT) {
		const SECTOR *sector;

		sector = sector_at(area_point);

		if (terrain_has_attribute(sector->terrain,
			TA_BLOCKS_LINE_OF_SIGHT)) {

			return true;
		}
	}

	return false;
}


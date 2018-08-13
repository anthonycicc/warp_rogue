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
 * Module: Character Perception
 */

#include "wrogue.h"




static bool	silent_move_noticed(const CHARACTER *);
static bool	silent_action_noticed(const CHARACTER *);

static bool	los_blocked(const AREA_POINT *);





/*
 * view screen (for look and mind view)
 */
void view_screen(const CHARACTER *character, const char *name,
	COLOUR cursor_colour
)
{
	COMMAND entered_command;
	AREA_POINT cursor;

	cursor = character->location;

	do {
		SECTOR *sector;

		sector = sector_at(&cursor);

		command_bar_set(2, CM_INFO, CM_EXIT);
		command_bar_add_move_commands();

		render_view_screen(name, &cursor, cursor_colour);
		update_screen();

		entered_command = command_bar_get_command();

		if (is_move_command(entered_command)) {
			DIRECTION direction;
			AREA_POINT previous_location;

			previous_location = cursor;

			move_command_to_direction(&direction, entered_command);

			move_area_point(&cursor, direction);

			if (out_of_area_bounds(&cursor)) {

				cursor = previous_location;
			}

		} else if (entered_command == CM_INFO) {

			if (sector->character != NULL) {

				character_screen(sector->character);

			} else if (sector->object != NULL) {

				object_screen(sector->object);

			} else {

				terrain_screen(sector->terrain);
			}
		}


	} while (entered_command != CM_EXIT);

	update_view_character(character);
}



/*
 * updates the passed character's perception data
 */
void update_perception_data(CHARACTER *character)
{
	CHARACTER *target;
	LIST_NODE *node;
	LIST *noticed_enemies;

	noticed_enemies = character->noticed_enemies;

	list_clear(noticed_enemies);

	for (node = area_character_list()->head;
		node != NULL;
		node = node->next) {

		target = (CHARACTER *)node->data;

		if (!hostility_between(character, target)) {

			continue;
		}

		if (!los(&character->location, &target->location)) {

			continue;
		}

		if (character_unnoticed(target)) {

			notice_check(target, true, false);
		}

		if (!character_unnoticed(target)) {

			list_add(noticed_enemies, target);

			continue;
		}
	}
}



/*
 * gives the unnoticed status to a character
 */
void activate_stealth(CHARACTER *character)
{

	character_set_attribute(character, CA_UNNOTICED);
}



/*
 * removes the unnoticed status from a character
 */
void cancel_stealth(CHARACTER *character, const CHARACTER *noticed_by)
{

	character_remove_attribute(character, CA_UNNOTICED);

	if (noticed_by != NULL) {

		dynamic_message(MSG_NOTICE, noticed_by, character,
			MOT_CHARACTER
		);
	}
}



/*
 * returns true if the passed character has the unnoticed status
 */
bool character_unnoticed(const CHARACTER *character)
{

	return character_has_attribute(character, CA_UNNOTICED);
}




/*
 * checks whether or not of a character has been noticed,
 * and cancels the unnoticed status of the character if he
 * has been noticed
 */
void notice_check(CHARACTER *character, bool silent, bool action)
{
	LIST_NODE *node;

	if (character_killed(character)) {

		return;
	}

	for (node = area_character_list()->head;
		node != NULL;
		node = node->next) {
		CHARACTER *observer;

		observer = (CHARACTER *)node->data;

		if (!hostility_between(observer, character) ||
			!los(&observer->location, &character->location)) {

			continue;
		}

		if (!silent) {

			cancel_stealth(character, observer);

			break;
		}

		if (action) {

			if (silent_action_noticed(observer)) {

				cancel_stealth(character, observer);

				break;
			}

			continue;
		}

		if (silent_move_noticed(observer)) {

			cancel_stealth(character, observer);

			break;
		}
	}
}



/*
 * returns true if the player's party can see the passed character
 */
bool party_can_see(const CHARACTER *character)
{
	LIST_NODE *node;

	if (character->party == PARTY_PLAYER) {

		return true;
	}

	if (character_unnoticed(character)) {

		return false;
	}

	for (node = party_player()->head;
		node != NULL;
		node = node->next) {
		const CHARACTER *party_member;

		party_member = (const CHARACTER *)node->data;

		if (los(&party_member->location, &character->location)) {

			return true;
		}
	}

	return false;
}



/*
 * returns true if the character is being watched by enemies
 */
bool enemies_are_watching(const CHARACTER *character)
{
	LIST_NODE *node;

	for (node = area_character_list()->head;
		node != NULL;
		node = node->next) {
		const CHARACTER *tested_character;

		tested_character = (const CHARACTER *)node->data;

		if (!hostility_between(tested_character, character)) {

			continue;
		}

		if (los(&tested_character->location,
			&character->location)) {

			return true;
		}
	}

	return false;
}



/*
 * returns the nearest enemy the passed character is aware of
 */
CHARACTER * nearest_noticed_enemy(const CHARACTER *character)
{
	CHARACTER *nearest_enemy;
	AREA_DISTANCE nearest_enemy_distance;
	LIST_NODE *node;

	nearest_enemy_distance = AREA_DISTANCE_MAX;
	nearest_enemy = NULL;

	for (node = character->noticed_enemies->head;
		node != NULL;
		node = node->next) {
		CHARACTER *tested_character;
		AREA_DISTANCE tested_character_distance;

		tested_character = (CHARACTER *)node->data;

		if (!los(&tested_character->location,
			&character->location)) {

			continue;
		}

		if (character_unnoticed(tested_character)) {

			continue;
		}

		tested_character_distance = area_distance(
			&tested_character->location, &character->location
		);

		if (tested_character_distance < nearest_enemy_distance) {

			nearest_enemy = tested_character;
			nearest_enemy_distance = tested_character_distance;
		}
	}

	return nearest_enemy;
}



/*
 * returns true if there is an unnoticed enemy at the passed location
 */
bool unnoticed_enemy_at(const CHARACTER *character, const AREA_POINT *location)
{
	const CHARACTER *character_at_location;

	character_at_location = character_at(location);

	if (character_at_location == NULL) {

		return false;
	}

	if (character_unnoticed(character_at_location) &&
		hostility_between(character, character_at_location)) {

		return true;
	}

	return false;
}



/*
 * returns true if there is a line of sight between the two passed points
 */
bool line_of_sight(const AREA_POINT *source, const AREA_POINT *target)
{
	int t, ax, ay, sx, sy, dx, dy;
	AREA_POINT point;

 	dx = target->x - source->x;
	dy = target->y - source->y;

	ax = abs(dx)<<1;
	ay = abs(dy)<<1;

	sx = sgn(dx);
	sy = sgn(dy);

	point = *source;

	if(ax > ay) {

		t = ay - (ax >> 1);

		do {

			if(t >= 0) {

           			point.y += sy;
				t -= ax;
			}

			point.x += sx;
			t += ay;

			if (area_points_equal(&point, target)) {

				return true;
			}

		} while (!los_blocked(&point));

		return false;

	} else {

		t = ax - (ay >> 1);

		do {

			if(t >= 0) {

				point.x += sx;
				t -= ay;
			}

			point.y += sy;
			t += ax;

			if(area_points_equal(&point, target)) {

				return true;
			}

		} while(!los_blocked(&point));

		return false;
	}
}



/*
 * checks whether or not of a silent move has been noticed
 */
static bool silent_move_noticed(const CHARACTER *observer)
{

	if (character_has_perk(observer, PK_ALERTNESS)) {

		return true;
	}

	return false;
}



/*
 * checks whether or not of a silent action has been noticed
 */
static bool silent_action_noticed(const CHARACTER *observer)
{

	if (character_has_perk(observer, PK_ALERTNESS) ||
		d100_test_passed(50)) {

		return true;
	}

	return false;
}



/*
 * returns true if there is something that blocks LOS at the passed location
 */
static bool los_blocked(const AREA_POINT *location)
{
	SECTOR *sector;

	sector = sector_at(location);

	if (sector->object != NULL) {

		if (object_has_attribute(sector->object,
			OA_BLOCKS_LINE_OF_SIGHT)) {

			return true;
		}
	}

	if (sector->terrain != NULL) {

		if (terrain_has_attribute(sector->terrain,
			TA_BLOCKS_LINE_OF_SIGHT)) {

			return true;
		}
	}

	return false;
}



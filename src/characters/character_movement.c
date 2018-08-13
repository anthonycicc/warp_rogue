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
 * Module: Character Movement
 */

#include "wrogue.h"




static void		enter_sector_effect(CHARACTER *, SECTOR *);




/*
 * returns true if the passed character can move
 */
bool character_can_move(const CHARACTER *character)
{

	if (character->stat[S_AG].current == 0) return false;

	return true;
}



/*
 * moves a character to a new location
 */
bool move_character(CHARACTER *character, const AREA_POINT *target_point)
{
	SECTOR *sector;
	
	if (out_of_area_bounds(target_point)) {

		return false;
	}

	sector = sector_at(target_point);
	
	if (sector->character != NULL) {
		
		if (can_push_past(character, sector->character)) {

			push_past(character, sector->character);
			
			return true;
		}
		
		if (unnoticed_enemy_at(character, target_point)) {

			cancel_stealth(sector->character, character);
		}

		return false;
	}
	
	if (!sector_is_class(target_point, SC_MOVE_TARGET)) {

		return false;
	} 

	sector_at(&character->location)->character = NULL;
	
	sector->character = character;

	character->location = *target_point;

	if (is_player_controlled_character(character)) {

		update_view_character(character);
	}

	enter_sector_effect(character, sector);

	return true;
}



/*
 * returns the maximal number of tiles a character can pass
 * in one action while running
 */
AREA_DISTANCE run_factor(const CHARACTER *character)
{
	AREA_DISTANCE factor;

	if (character->armour != NULL) {

		if (object_has_attribute(character->armour,
			OA_HINDERS_MOVEMENT)) {

			factor = 1;

			return factor;
		}
	}

	factor = divide_and_round_up(character->stat[S_AG].current, 
		RUN_FACTOR_DIVISOR
	);

	return factor;
}



/*
 * returns the passed character's maximal movement speed 
 */
AREA_DISTANCE movement_speed_max(const CHARACTER *character)
{

	if (!character_can_move(character)) return 0;

	return run_factor(character);
}



/*
 * executes a run action
 */
bool run(CHARACTER *character, const PATH_NODE *path, AREA_DISTANCE n_steps)
{
	const PATH_NODE *step;
	AREA_DISTANCE steps_taken;

	for (steps_taken = 0, step = path;
       		step != NULL && steps_taken < n_steps;
		steps_taken++, step = step->parent) {

		if (!move_character(character, &step->p)) {

			break;
		}
	}

	if (steps_taken > 0) {
		
		return true;
	}

	return false;
}



/*
 * returns true if character c1 can push past character c2
 */
bool can_push_past(const CHARACTER *c1, const CHARACTER *c2)
{

	if (is_player_controlled_character(c2) ||
		hostility_between(c1, c2)) {

		return false;
	}

	if (is_player_controlled_character(c1) ||
		(c1->power_level > c2->power_level) ||
		(c1->party == PARTY_PLAYER && c2->party != PARTY_PLAYER)) {

		return true;
	}

	return false;
}



/*
 * makes character c1 push past character c2
 */
void push_past(CHARACTER *c1, CHARACTER *c2)
{
	AREA_POINT temp;

	sector_at(&c1->location)->character = c2;
	sector_at(&c2->location)->character = c1;

	temp = c1->location;
	c1->location = c2->location;
	c2->location = temp;

	enter_sector_effect(c1, sector_at(&c1->location));
	enter_sector_effect(c2, sector_at(&c2->location));
}



/*
 * executes the effects that happen when a character enters a sector
 */
static void enter_sector_effect(CHARACTER *character, SECTOR *sector)
{
	TERRAIN *terrain = sector->terrain;

	if (terrain_has_attribute(terrain, TA_CAUSES_DISEASE)) {

		terrain_cause_disease(character);
	}

	if (terrain_has_attribute(terrain, TA_AIR)) {

		dynamic_message(MSG_FALL_DOWN, character, NULL, MOT_NIL);

		character->injury = injury_max(character) * 2;

		handle_destruction(&character->location, NULL, DT_FALL);
	}

	if (terrain->gore_level == GORE_LEVEL_BODY_PART ||
		terrain->gore_level == GORE_LEVEL_CORPSE) {

		character->bloody_feet = BLOODY_FEET_FACTOR;

		return;
	}

	if (character->bloody_feet == 0) {

		return;
	}

	if (terrain->gore_level < GORE_LEVEL_BLOOD &&
		!terrain_has_attribute(terrain, TA_DANGEROUS)) {
		
		terrain->gore_level = GORE_LEVEL_BLOOD;
	}

	character->bloody_feet -= 1;
}






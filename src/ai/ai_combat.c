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
 * Module: AI Combat
 */

#include "wrogue.h"





/*
 * AI: attack
 */
void ai_attack(AI_CONTROL_DATA *ai)
{

	/* choose a combat option and execute it */
	ai_execute_option(ai, ai_choose_option(ai));
}



/*
 * makes the AI run towards its target if the distance between
 * the AI and its target is too large to attack
 */
void ai_reach_attack_distance(AI_CONTROL_DATA *ai)
{
	const OBJECT *weapon = ai->self->weapon;
	bool must_run = false;

	/* unarmed attack or close combat weapon .. */
	if (weapon == NULL ||
		weapon->type == OTYPE_CLOSE_COMBAT_WEAPON) {

		/* target distance larger than 1 .. */
		if (ai->target_distance > 1) {

			/* run towards target */
			must_run = true;
		}

	/* ranged combat weapon */
	} else if (weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {

		/* target distance larger than MAX_LONG_RANGE AND
			no marksman perk AND
			not a heavy weapon */
		if (ai->target_distance > MAX_LONG_RANGE &&
			!character_has_perk(ai->self, PK_MARKSMAN) &&
			weapon->subtype != OSTYPE_HEAVY) {

			/* run towards target */
			must_run = true;
		}
	}
	
	/* run towards target required .. */
	if (must_run) {
		
		/* we cannot move - so we can do nothing but wait */
		if (movement_speed_max(ai->self) == 0) {
		
			action_do_nothing(ai->self);
			return;
		}

		/* run succeeded .. return */
		if (ai_run_towards_target(ai)) return;

		/* run failed .. wait */
		action_do_nothing(ai->self);
	}
}






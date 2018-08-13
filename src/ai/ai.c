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
 * Module: AI
 */

#include "wrogue.h"




static void	ai_update_data(AI_CONTROL_DATA *);



/*
 * lets the AI control the passed character
 */
void ai_control(CHARACTER *character)
{
	AI_CONTROL_DATA *ai = &character->ai;

	ai_update_data(ai);

	ai_fix_self(ai);

	while (!ai->self->action_spent) {

		ai_state_execute(ai);
	}
}







/*
 * AI: update data
 */
static void ai_update_data(AI_CONTROL_DATA *ai)
{

	ai->follow_failed = false;

	/* the AI has a target .. */
	if (ai_has_target(ai)) {

		/* .. update target distance */
		ai->target_distance = area_distance(
			&ai->self->location,
			&ai->target_point
		);

		/* .. update path */
		ai_find_path(ai);
	}
}



/*
 * AI: fix self
 *
 * the AI attempts to "fix" itself
 *
 */
void ai_fix_self(AI_CONTROL_DATA *ai)
{

	ai_use_drugs_to_fix_self(ai);

	ai_use_stealth(ai);
}



/*
 * AI: use drugs to fix self
 *
 * the AI uses drugs to remove the status attributes "Poisoned" and "Broken"
 * if the needed drugs are available
 *
 */
void ai_use_drugs_to_fix_self(AI_CONTROL_DATA *ai)
{
	OBJECT *drug;

	if (character_has_attribute(ai->self, CA_POISONED)) {

		drug = inventory_find_object(ai->self, "Antidote");

		if (drug != NULL) {

			action_use_drug(ai->self, drug);
		}
	}

	if (character_has_attribute(ai->self, CA_BROKEN)) {

		drug = inventory_find_object(ai->self, "Stoic");

		if (drug != NULL) {

			action_use_drug(ai->self, drug);
		}
	}
}



/*
 * AI: use stealth
 *
 * the AI uses the "Stealth" perk if its available
 *
 */
void ai_use_stealth(AI_CONTROL_DATA *ai)
{

	if (!character_has_perk(ai->self, PK_STEALTH) ||
		character_unnoticed(ai->self)) {

		return;
	}

	action_stealth(ai->self);
}



/*
 * AI: get ready
 *
 * the AI will ready itself for combat
 * if there is nothing left to do it will just do nothing
 */
void ai_get_ready(AI_CONTROL_DATA *ai)
{

	/* character carries a weapon .. */
	if (ai->self->weapon != NULL) {
		
		/* fix weapon if necessary */
		ai_fix_weapon(ai, false);
	}

	/* character must recover .. */
	if (character_must_recover(ai->self)) {

		/* recover */
		action_recover(ai->self);
		return;
	}

	/* action not spent yet i.e. nothing to do .. */
	if (!ai->self->action_spent) {

		/* do nothing */
		action_do_nothing(ai->self);
		return;
	}
}





/*
 * AI: is close to PCC
 *
 * returns true if the passed character is close to the player
 * controlled character
 *
 */
bool ai_is_close_to_pcc(AI_CONTROL_DATA *ai)
{
	const AREA_POINT *pcc_location;

	pcc_location = &(player_controlled_character()->location);

	if (area_distance(&ai->self->location, pcc_location) <=
		AI_MAX_FOLLOW_DISTANCE) {

		return true;
	}

	return false;
}






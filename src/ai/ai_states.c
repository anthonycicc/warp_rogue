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
 * Module: AI States
 */

#include "wrogue.h"






static void		ai_state_wait(AI_CONTROL_DATA *);
static void		ai_state_guard(AI_CONTROL_DATA *);
static void		ai_state_patrol(AI_CONTROL_DATA *);

static void		ai_state_follow(AI_CONTROL_DATA *);
static void		ai_state_flee(AI_CONTROL_DATA *);
static void		ai_state_search(AI_CONTROL_DATA *);

static void		ai_state_combat(AI_CONTROL_DATA *);




/*
 * AI state functions
 */
static void (* const AiState[MAX_AI_STATES])(AI_CONTROL_DATA *) = {

	ai_state_wait,
	ai_state_guard,
	ai_state_patrol,

	ai_state_follow,
	ai_state_flee,
	ai_state_search,

	ai_state_combat

};





/*
 * executes the current AI state
 */
void ai_state_execute(AI_CONTROL_DATA *ai)
{

	(*AiState[ai->state])(ai);
}




/*
 * sets the state of the AI
 */
void ai_set_state(AI_CONTROL_DATA *ai, AI_STATE new_state)
{

	if (ai->state == AI_STATE_COMBAT &&
		new_state == AI_STATE_SEARCH) {

		/* DO NOTHING */

	} else {

		ai_erase_target(ai);
	}

	ai->state = new_state;
}



/*
 * sets the default state of the AI
 */
void ai_set_default_state(AI_CONTROL_DATA *ai, AI_STATE new_state)
{

	ai->default_state = new_state;
}




/*
 * AI state: wait
 */
static void ai_state_wait(AI_CONTROL_DATA *ai)
{

	if (enemies_noticed(ai->self)) {

		ai_set_state(ai, AI_STATE_COMBAT);

		return;
	}

	ai_get_ready(ai);
}



/*
 * AI state: guard
 */
static void ai_state_guard(AI_CONTROL_DATA *ai)
{

	if (enemies_noticed(ai->self)) {

		ai_set_state(ai, AI_STATE_COMBAT);

		return;
	}

	if (ai->self->party != PARTY_PLAYER &&
		random_choice(AI_SWITCH_TO_PATROL_PROBABILITY)) {

		ai_set_state(ai, AI_STATE_PATROL);
		ai_set_default_state(ai, AI_STATE_PATROL);

		return;
	}

	ai_get_ready(ai);
}



/*
 * AI state: patrol
 *
 */
static void ai_state_patrol(AI_CONTROL_DATA *ai)
{

	if (enemies_noticed(ai->self)) {

		ai_set_state(ai, AI_STATE_COMBAT);
		return;
	}

	if (random_choice(AI_SWITCH_TO_GUARD_PROBABILITY)) {

		ai_set_state(ai, AI_STATE_GUARD);
		ai_set_default_state(ai, AI_STATE_GUARD);
		return;
	}
	
	if (movement_speed_max(ai->self) == 0) {
		
		action_do_nothing(ai->self);
		return;
	}

	/*
	 * non-hostile characters patrol slowly, this makes
	 * talking to them easier
	 */
	if (faction_relationship(ai->self->faction, FACTION_PLAYER) 
		!= FR_HOSTILE && random_choice(50)) {

		ai_get_ready(ai);
		return;
	}

	if (ai->target_distance == 0) {

		ai_erase_target(ai);
	}

	if (!ai_has_target(ai)) {
		const AREA_POINT * target_point =
			random_sector(SC_MOVE_TARGET_SAFE);

		if (target_point == NULL) {

			ai_get_ready(ai);
			return;
		}

		ai_set_target(ai, target_point);
	}
	
	if (ai->target_reachable) {
	
		action_move(ai->self, path_first_step(ai->path));

	} else {

		ai_get_ready(ai);
	}
}



/*
 * AI state: follow
 */
static void ai_state_follow(AI_CONTROL_DATA *ai)
{

	if (enemies_noticed(ai->self)) {

		ai_set_state(ai, AI_STATE_COMBAT);

		return;
	}

	if (!ai_is_close_to_pcc(ai)) {
		const AREA_POINT *pcc_location;

		pcc_location = &(player_controlled_character()->location);

		ai_set_target(ai, pcc_location);

		if (ai_run_towards_target(ai)) {

			return;
		}

		ai->follow_failed = true;
	}

	ai_get_ready(ai);
}



/*
 * AI state: flee
 */
static void ai_state_flee(AI_CONTROL_DATA *ai)
{
	CHARACTER *nearest_enemy;
	AREA_POINT step;
	int i;

	if (!character_has_attribute(ai->self, CA_BROKEN)) {

		ai_set_state(ai, ai->default_state);

		return;
	}

	nearest_enemy = nearest_noticed_enemy(ai->self);

	if (nearest_enemy == NULL) {

		action_do_nothing(ai->self);

		return;
	}

	step = ai->self->location;

	for (i = run_factor(ai->self); i > 0; --i) {

		ai_flee(&step, &nearest_enemy->location);

		if (!action_move(ai->self, &step)) {

			break;
		}
	}

	if (!ai->self->action_spent) {

		action_do_nothing(ai->self);
	}
}



/*
 * AI state: search
 */
static void ai_state_search(AI_CONTROL_DATA *ai)
{

	if (ai->target_distance == 0) {

		ai_set_state(ai, ai->default_state);
		return;
	}

	if (enemies_noticed(ai->self)) {

		ai_set_state(ai, AI_STATE_COMBAT);
		return;
	}

	if (!ai_run_towards_target(ai)) {

		ai_get_ready(ai);
	}
}



/*
 * AI state: combat
 */
static void ai_state_combat(AI_CONTROL_DATA *ai)
{
	const CHARACTER *nearest_enemy;

	/* no enemies noticed .. */
	if (!enemies_noticed(ai->self)) {

		/* .. PC party members return to their default state */
		if (ai->self->party == PARTY_PLAYER) {

			ai_set_state(ai, ai->default_state);

		/* .. other characters change their state to 'search' */
		} else {

			ai_set_state(ai, AI_STATE_SEARCH);
		}

		return;
	}

	/* enemies noticed .. */

	/* tactics: 'get ready' changes to 'plan A' */
	if (ai->tactic == TACTIC_GET_READY) {

		ai_set_tactic(ai, TACTIC_PLAN_A);

	/* tactics: 'stay close' makes the character follow the PCC */
	} else if (ai->tactic == TACTIC_STAY_CLOSE &&
		!ai->follow_failed && !ai_is_close_to_pcc(ai)) {

		ai_set_state(ai, AI_STATE_FOLLOW);
		return;
	}
	
	/* always target the nearest noticed enemy */
	nearest_enemy = nearest_noticed_enemy(ai->self);
	ai_set_target(ai, &nearest_enemy->location);
	
	/* try to attack */
	ai_attack(ai);
}




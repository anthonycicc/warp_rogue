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
 * Module: AI Combat Options
 */

#include "wrogue.h"


/*
 * This module is the core of the complex AI decision process used
 * by the 'combat' state. The other AI states use simpler processes.
 */

/*
 * When the AI decides what to do, it usually has to choose between multiple
 * options.
 *
 * To do this we do the following: 
 *
 * - we calculate the 'weight' of each option
 * - we choose the option with the highest weight
 *
 */

static AI_OPTION_WEIGHT get_option_weight(AI_CONTROL_DATA *, AI_OPTION);




/*
 * AI option choice
 * returns the option chosen by the AI
 */
AI_OPTION ai_choose_option(AI_CONTROL_DATA *ai)
{
	AI_OPTION_WEIGHT chosen_option_weight = AI_OPTION_WEIGHT_NIL;
	AI_OPTION chosen_option = AI_OPTION_NIL;
	AI_OPTION option;

	for (option = 0; option < MAX_AI_OPTIONS; option++) {
		AI_OPTION_WEIGHT option_weight = get_option_weight(ai, option);

		if (option_weight > chosen_option_weight) {

			chosen_option = option;
			chosen_option_weight = option_weight;
		}
	}
	
	return chosen_option;
}



/*
 * executes the passed option
 */
void ai_execute_option(AI_CONTROL_DATA *ai, AI_OPTION option)
{

	/* 
	 * option: NIL
	 */
	if (option == AI_OPTION_NIL) {
		
		/* do nothing */
		action_do_nothing(ai->self);
		return;
	}
	
	/*
	 * option: weapon or secondary weapon
	 */
	if (option == AI_OPTION_WEAPON || 
		option == AI_OPTION_SECONDARY_WEAPON) {
		
		/* secondary weapon chosen .. */
		if (option == AI_OPTION_SECONDARY_WEAPON) {
			
			/* switch weapons */
			action_switch_weapons(ai->self);
			if (ai->self->action_spent) return;
		}
		
		/* fix weapon if necessary */
		ai_fix_weapon(ai, true);
		if (ai->self->action_spent) return;
		
		/* reach attack distance */
		ai_reach_attack_distance(ai);
		if (ai->self->action_spent) return;

		/* use weapon */
		ai_use_weapon(ai);
	}
}



/*
 * returns the weight of the passed option
 */
static AI_OPTION_WEIGHT get_option_weight(AI_CONTROL_DATA *ai, 
	AI_OPTION option
)
{
	AI_OPTION_WEIGHT option_weight = AI_OPTION_WEIGHT_NIL;

	/* 
	 * option: weapon 
	 */
	if (option == AI_OPTION_WEAPON) {
		
		/* option weight = weapon attack weight */
		option_weight = ai_weapon_attack_weight(ai, 
			ai->self->weapon
		);
	
	/*
	 * option: secondary weapon
	 */
	} else if (option == AI_OPTION_SECONDARY_WEAPON) {
		
		/* option weight = weapon attack weight */
		option_weight = ai_weapon_attack_weight(ai, 
			ai->self->secondary_weapon
		);
	} 

	return option_weight;
}



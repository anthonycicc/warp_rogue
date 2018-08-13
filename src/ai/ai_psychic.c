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
 * Module: AI Psychic
 */

#include "wrogue.h"





/*
 * AI: choose psy power
 */
PSY_POWER ai_choose_psy_power(AI_CONTROL_DATA *ai)
{
	PSY_POWER chosen_power, power;
	bool choice_valid;
	
	power = chosen_power = random_int(0, MAX_PSY_POWERS - 1);
	choice_valid = false;

	do {

		if (character_has_psy_power(ai->self, power)) {
			const AREA_POINT *target_point;
			
			if (psy_power_is_hostile(power)) {
				
				target_point = &ai->target_point;
	
			} else {
				
				target_point = &ai->self->location;
			}
			
			if (ai_power_is_sensible_choice(power, 
				target_point)) {

				chosen_power = power;
				
				choice_valid = true;
					
				ai_set_target(ai, target_point);

				break;
			}
		}
		
		++power;
		
		if (power == MAX_PSY_POWERS) {
			
			power = 0;
		}

	} while (power != chosen_power);

	if (choice_valid) {

		return chosen_power;
	}

	return PSY_NIL;
}



/*
 * returns true if the passed psychic power is a sensible choice
 */
bool ai_power_is_sensible_choice(PSY_POWER power, 
	const AREA_POINT *target_point
)
{

	if (psy_power_is_tricky(power)) {
		
		return false;
	}

	if (!psy_power_valid_target(power, target_point)) {

		return false;
	}

	if (psy_power_in_effect(power, target_point)) {

		return false;
	}

	if (psy_power_target_is_immune(power, target_point)) {

		return false;
	}

	return true;
}






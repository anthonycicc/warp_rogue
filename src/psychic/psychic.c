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
 * Module: Psychic
 */

#include "wrogue.h"


static N_DICE 			n_psy_dice(const CHARACTER *);

static bool			psychic_test(const EVOCATION_DATA *);

static PSY_RANGE_MODIFIER	psy_range_modifier(const EVOCATION_DATA *);



/*
 * psychic power data
 */
static const PSY_POWER_DATA	PsychicPower[MAX_PSY_POWERS] = {


	{"Regenerate",		5,	false,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_regenerate,		NULL
	},

	{"Reactivate",		5,	false,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_reactivate,		NULL
	},
	

	{"Warp strength",	5,	false,	PTT_CHARACTER_BIO,
		CA_WARP_STRENGTH,
		psy_warp_strength,	psy_warp_strength_terminate
	},
	
	{"Purify blood",	6,	false,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_purify_blood,	NULL
	},

	{"Purge plague",	6,	false,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_purge_plague,	NULL
	},

	{"Mind view",		2,	false,	PTT_NONE,
		CA_NIL,
		psy_mind_view,		NULL
	},
	

	{"Psychic shriek",	6,	true,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_psychic_shriek,	NULL
	},

	{"Terrify",		15,	true,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_terrify,		NULL
	},

	{"Psychic shield",	10,	false,	PTT_CHARACTER_ALL,
		CA_PSYCHIC_SHIELD,
		psy_psychic_shield,	psy_psychic_shield_terminate
	},

	{"Banishment",		18,	true,	PTT_CHARACTER_BIO,
		CA_NIL,
		psy_banishment,		NULL
	}
};




/*
 * gives the passed character the pased psychic power
 */
void give_psy_power(CHARACTER *character, PSY_POWER power)
{

	character_set_psy_power(character, power);
}



/*
 * the psychic power screen
 */
void psy_power_screen(PSY_POWER power)
{

	command_bar_set(1, CM_EXIT);

	render_psy_power_screen(power);

	update_screen();

	command_bar_get_command();
}



/*
 * returns the description of a psychic power
 */
const char * psy_power_description(char *description,
	PSY_POWER power
)
{

	return data_file_psy_power_description(description,
		power
	);
}



/*
 * makes a character attempt to evoke a psychic power
 */
bool evoke_psy_power(CHARACTER *character, PSY_POWER evoked_power,
	const AREA_POINT *target_point
)
{
	/* the evoked power */
	const PSY_POWER_DATA *power = &PsychicPower[evoked_power];
	EVOCATION_DATA evocation; /* the evocation data */

	/* set evocation data */
	evocation.evoker = character;
	evocation.power = evoked_power;
	evocation.target_point = target_point;

	/* psychic test passed .. */
	if (psychic_test(&evocation)) {
		
		/* output evocation message */
		dynamic_message(MSG_EVOKE_PSY_POWER, character, 
			power->name, MOT_STRING
		);

		/* evoke power */
		(*power->evoke)(&evocation);
		
		return true;
		
	/* psychic test NOT passed .. */
	} else {

		/* output failure message */
		dynamic_message(MSG_EVOKE_PSY_POWER_FAILS, character, 
			power->name, MOT_STRING
		);

		return false;
	}
}



/*
 * terminates a psychic power
 */
void psy_power_terminate(EVENT *event)
{
	const PSY_POWER_DATA *power;

	power = &PsychicPower[event->psy_power];

	dynamic_message(MSG_PSY_POWER_TERMINATION, event->character,
		power->name, MOT_STRING
	);

	if (power->terminate != NULL) {

		(*power->terminate)(event);
	}
}



/*
 * returns true if the passed character is able to evoke psychic powers
 */
bool is_able_to_evoke_psy_powers(const CHARACTER *character)
{

	if (!character_has_perk(character, PK_PSYCHIC_I)) {

		return false;
	}

	if (character_has_attribute(character, CA_BROKEN)) {

		return false;
	}

	return true;
}



/*
 * returns the number of psychic powers the passed character has
 */
N_PSY_POWERS n_psy_powers(const CHARACTER *character)
{
	N_PSY_POWERS n_powers = 0;
	PSY_POWER power;
	
	iterate_over_psy_powers(power) {

		if (character_has_psy_power(character, power)) {

			++n_powers;
		}
	}

	return n_powers;
}



/*
 * returns a list which contains the names of all psychic power 
 * available to the passed character, or NULL if the passed character
 * has no psychic powers
 */
LIST * character_psy_powers(const CHARACTER *character)
{
	LIST *power_list = list_new();
	PSY_POWER power;

	/* build psychic power list */
	iterate_over_psy_powers(power) {

		if (character_has_psy_power(character, power)) {

			list_add(power_list, (void *)psy_power_name(power));
		}
	}
	
	/* no psychic powers .. */
	if (power_list->n_nodes == 0) {
	
		/* free list, return NULL */
		list_free(power_list);
		return NULL;
	}

	/* return power list */
	return power_list;
}



/*
 * returns true if the passed psychic power is implemented
 */
bool psy_power_implemented(PSY_POWER power)
{

	if (is_empty_string(PsychicPower[power].name)) {

		return false;
	}

	return true;
}



/*
 * returns true if the passed psychic power is hostile
 */
bool psy_power_is_hostile(PSY_POWER power)
{

	return PsychicPower[power].is_hostile;
}



/*
 * returns true if the passed psychic power requires a target
 */
bool psy_power_requires_target(PSY_POWER power)
{

	if (PsychicPower[power].target_type == PTT_NONE) {

		return false;
	}

	return true;
}



/*
 * returns true if the passed psychic power is tricky i.e.
 * the AI is not smart enough to use it in a sensible way
 */
bool psy_power_is_tricky(PSY_POWER power)
{
	bool is_tricky;

	switch (power) {

	case PSY_WARP_STRENGTH:
	case PSY_MIND_VIEW:
		is_tricky = true;
		break;

	default:
		is_tricky = false;
	}

	return is_tricky;
}



/*
 * returns true if the passed point is a valid target for
 * the passed psychic power
 */
bool psy_power_valid_target(PSY_POWER power, const AREA_POINT *target_point)
{
	CHARACTER *target;
	PSY_TARGET_TYPE target_type;

	target = character_at(target_point);

	target_type = PsychicPower[power].target_type;

	if (target == NULL) {

		if (target_type == PTT_CHARACTER_BIO ||
			target_type == PTT_CHARACTER_ALL) {

			return false;
		}

		return true;
	}

	if (character_has_attribute(target, CA_MACHINE) &&
		PsychicPower[power].target_type == PTT_CHARACTER_BIO) {

		return false;
	}

	if (power == PSY_REGENERATE) {

		if (target->injury == 0) {

			return false;
		}

	} else if (power == PSY_REACTIVATE) {

		if (!character_has_attribute(target, CA_STUNNED)) {

			return false;
		}

	} else if (power == PSY_PURIFY_BLOOD) {

		if (!character_has_attribute(target, CA_POISONED)) {

			return false;
		}

	} else if (power == PSY_PURGE_PLAGUE) {

		if (!character_has_attribute(target, CA_DISEASED)) {

			return false;
		}

	} else if (power == PSY_TERRIFY) {

		if (character_has_attribute(target, CA_BROKEN)) {

			return false;
		}

	} else if (power == PSY_PSYCHIC_SHRIEK) {

		if (character_has_attribute(target, CA_STUNNED)) {

			return false;
		}

	} else if (power == PSY_BANISHMENT) {

		if (!character_has_attribute(target, CA_DAEMONIC)) {

			return false;
		}
	}

	return true;
}



/*
 * returns true if passed psychic power is already in effect
 * at the passed point
 */
bool psy_power_in_effect(PSY_POWER power, const AREA_POINT *target_point)
{
	CHARACTER *target;

	if (PsychicPower[power].attribute == CA_NIL) {

		return false;
	}

	target = character_at(target_point);

	if (target == NULL) {

		return false;
	}

	if (character_has_attribute(target, PsychicPower[power].attribute)) {

		return true;
	}

	return false;
}



/*
 * returns true if the target is immune to the passed psychic power
 */
bool psy_power_target_is_immune(PSY_POWER power, const AREA_POINT *target_point)
{
	CHARACTER *target;

	target = character_at(target_point);

	if (target == NULL) {

		return false;
	}

	if (character_has_attribute(target, CA_MACHINE) &&
		PsychicPower[power].target_type == PTT_CHARACTER_BIO) {

		return true;
	}

	if (power == PSY_TERRIFY &&
		(character_has_perk(target, PK_IMMUNITY_TO_FEAR) ||
		character_has_attribute(target, CA_STOIC))) {

		return true;
	}

	if (power == PSY_BANISHMENT &&
		target->stat[S_TN].current >= 80) {

		return true;
	}

	return false;
}



/*
 * returns the name of a psychic power
 */
const char * psy_power_name(PSY_POWER power)
{

	return PsychicPower[power].name;
}



/*
 * returns the evocation value of the passed psychic power
 */
EVOCATION_VALUE psy_power_evocation_value(PSY_POWER power)
{

	return PsychicPower[power].evocation_value;
}



/*
 * the "Psychic Powers" screen
 */
void psy_powers_screen(const CHARACTER *character)
{

	command_bar_set(1, CM_EXIT);

	render_psy_powers_screen(character);

	update_screen();

	command_bar_get_command();
}



/*
 * name -> psy power index
 */
PSY_POWER name_to_psy_power(const char *name)
{
	PSY_POWER power;

	iterate_over_psy_powers(power) {

		if (!psy_power_implemented(power)) continue;

		if (strings_equal(name, PsychicPower[power].name)) {

			return power;
		}
	}

	die("*** CORE ERROR *** invalid psychic power: %s", name);

	/* never reached */
	return PSY_NIL;
}



/*
 * returns the number of psychic dice the passed character
 * gets when he does a psychic test
 */
static N_DICE n_psy_dice(const CHARACTER *character)
{
	N_DICE n_dice; /* number of psychic dice */

	/* Psychic III perk = 3 dice */ 
	if (character_has_perk(character, PK_PSYCHIC_III)) {
		
		n_dice = 3;
	
	/* Psychic II perk = 2 dice */
	} else if (character_has_perk(character, PK_PSYCHIC_II)) {
		
		n_dice = 2;
	
	/* Psychic I perk = 1 dice */
	} else if (character_has_perk(character, PK_PSYCHIC_I)) {

		n_dice = 1;
		
	} else {
	
		/* no Psychic perk i.e. invalid call */
		die("*** CORE ERROR *** invalid n_psy_dice call");
		
		/* never reached */
		return 0;
	}
	
	/* apply Nomat effect */
	if (character_has_attribute(character, CA_NOMAT)) {

		n_dice += 1;
	}

	return n_dice;
}



/*
 * the psychic test
 */
static bool psychic_test(const EVOCATION_DATA *evocation)
{
	/* the evoked power */
	const PSY_POWER_DATA *power = &PsychicPower[evocation->power];
	CHARACTER *evoker = evocation->evoker; /* the evoker */
	N_DICE n_dice;  /* number of psychic test */
	DICE_ROLL_RESULT evocation_roll_result; /* evocation roll result */
	PSY_TEST_VALUE test_value; /* psychic test value */

	/* determine the number of psychic dice */
	n_dice = n_psy_dice(evoker);
	
	/* evocation roll - D10 x number of psychic dice */
	evocation_roll_result = d10() * n_dice;

	/* all dice where 1s i.e. automatic failure */
	if (evocation_roll_result / n_dice == 1) return false;

	
	/*
	 * determine psychic test value
	 */
	
	/* 
	 * the base of the test value is the evocation value 
	 * of the evoked power 
	 */
	test_value = power->evocation_value;
	
	/* the evoker has NOT targetted himself .. */
	if (!area_points_equal(evocation->target_point, area_point_nil())) {
		/* get target character */
		CHARACTER *target = character_at(evocation->target_point);

		/* apply range modifier */
		test_value += psy_range_modifier(evocation);

		/* apply psychic resistance modifier if necessary */
		if (target != NULL && power->is_hostile &&
			character_has_perk(target, PK_PSYCHIC_RESISTANCE)) {

			test_value += PSYCHIC_RESISTANCE_MODIFIER;
		}
	}

	
	/* 
	 * the final test
	 *
	 * if the evocation roll result is equal to or higher than 
	 * the psychic test value, the evocation succeeds ..
	 */
	if (evocation_roll_result >= test_value) {

		return true;
	} 

	return false;
}



/*
 * returns the range-based modifier applied when evoking psychic powers
 */
static PSY_RANGE_MODIFIER psy_range_modifier(const EVOCATION_DATA *evocation)
{
	PSY_RANGE_MODIFIER mod;
	AREA_DISTANCE target_distance;
	const CHARACTER *evoker;

	evoker = evocation->evoker;

	target_distance = area_distance(&evoker->location,
		evocation->target_point
	);

	/* very long range - modifier is 4 */
	if (target_distance > MAX_LONG_RANGE) {

		mod = 4;

	/* long range - modifier is 2 */
	} else if (target_distance > MAX_MEDIUM_RANGE) {

		mod = 2;

	/* medium range - modifier is 1 */
	} else if (target_distance > MAX_SHORT_RANGE) {

		mod = 1;

	/* short range - no modifier */
	} else if (target_distance > MAX_CLOSE_COMBAT_RANGE) {

		mod = 0;

	/* close combat range */
	} else {
		
		/* 
		 * the combat evocation perk eliminates the 
		 * close combat evocation penalty
		 */
		if (character_has_perk(evoker, PK_COMBAT_EVOCATION)) {

			mod = 0;
		} else {

			mod = 3;
		}
	}

	/* 
	 * force channel effect: halves all distance penalties (rounding down) 
	 * except the close combat penalty
	 */
	if (target_distance > MAX_CLOSE_COMBAT_RANGE &&
		evoker->weapon != NULL &&
		object_has_attribute(evoker->weapon, OA_FORCE_CHANNEL) &&
		character_has_perk(evoker, PK_PSYCHIC_WEAPONS)) {

		mod /= 2;
	}

	return mod;
}



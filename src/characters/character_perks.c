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
 * Module: Character Perks
 */

#include "wrogue.h"





/*
 * perk data
 */
static const PERK_DATA	Perk[MAX_PERKS] = {

	{"Alertness",			false},
	{"Alien technology",		false},

	{"Battle tactics",		false},
	{"Blademaster",			false},

	{"Combat evocation",		false},
	{"Critical shot",		false},
	{"Critical strike",		false},
	{"Crushing strike",		false},
	
	{"Daemon lore",			false},
	{"Disarm",			true},
	{"Drug resistant",		false},
	
	{"Enhanced parry",		false},
	
	{"Fast draw",			false},
	{"First aid",			false},
	
	{"Grenadier",			false},
	
	{"Immunity to disease",		false},
	{"Immunity to fear",		false},
	{"Immunity to pain",		false},
	{"Immunity to poison",		false},

	{"Jump pack",			true},
	
	{"Leader",			false},

	{"Marksman",			false},

	{"Psychic I",			false},
	{"Psychic II",			false},
	{"Psychic III",			false},
	{"Psychic resistance",		false},
	{"Psychic weapons",		false},

	{"Quickload",			false},
	
	{"Rapid shot",			false},
	{"Rapid strike",		false},
	
	{"Silent evocation",		false},
	{"Sneak attack",		false},
	{"Spot trap",			false},
	{"Spot weakness",		false},
	{"Stealth",			true},
	
	{"Unarmed combat",		false},

	{"Uncanny dodge",		false},

	{"Weapon maintenance",		false}

};



/*
 * the perk screen
 */
void perk_screen(PERK perk)
{

	command_bar_set(1, CM_EXIT);

	render_perk_screen(perk);

	update_screen();

	command_bar_get_command();
}



/*
 * returns true if the passed perk is useable
 */
bool perk_useable(PERK perk)
{

	return Perk[perk].useable;
}



/*
 * returns the number of perks the passed character has
 */
N_PERKS character_n_perks(const CHARACTER *character)
{
	N_PERKS n_perks = 0;
	PERK perk;

	iterate_over_perks(perk) {

		if (character_has_perk(character, perk)) {

			++n_perks;
		}
	}

	return n_perks;
}


/*
 * returns a list containing the names of all useable perks
 * the passed character has, or NULL if the character has no
 * useable perks
 */
LIST * useable_perks_list(const CHARACTER *character)
{
	LIST *perk_list = list_new();
	PERK perk;
	
	iterate_over_perks(perk) {
		
		if (character_has_perk(character, perk) &&
			perk_useable(perk)) {

			list_add(perk_list, (void *)perk_name(perk));
		}		
	}
	
	/* no useable perks .. */
	if (perk_list->n_nodes == 0) {
		
		/* free list, return NULL */
		list_free(perk_list);
		return NULL;
	}
	
	return perk_list;
}



/*
 * returns the name of the passed perk
 */
const char * perk_name(PERK perk)
{

	return Perk[perk].name;
}



/*
 * name -> perk index
 */
PERK name_to_perk(const char *name)
{
	PERK perk;

	iterate_over_perks(perk) {

		if (strings_equal(name, Perk[perk].name)) {

			return perk;
		}
	}

	die("*** CORE ERROR *** invalid perk: %s", name);

	/* never reached */
	return PK_NIL;
}



/*
 * returns the description of a perk
 */
char * perk_description(char *description, PERK perk)
{

	return data_file_perk_description(description, perk);
}



/*
 * gives the passed character the passed perk
 */
void give_perk(CHARACTER *character, PERK perk)
{

	character_set_perk(character, perk);
}



/*
 * perk: stealth (use)
 */
bool stealth_use(CHARACTER *character)
{

	if (enemies_are_watching(character)) {

		if (is_player_controlled_character(character)) {

			message(SID_GAME, "You cannot use this perk " \
				"while you are being watched."
			);
		}

		return false;
	}

	if (character->armour != NULL) {

		if (character->armour->subtype != OSTYPE_LIGHT) {

			if (is_player_controlled_character(character)) {

				message(SID_GAME,
					"Your armour is too heavy."
				);
			}

			return false;
		}
	}

	if (noisy_object_in_use(character)) {

		if (is_player_controlled_character(character)) {

			message(SID_GAME, "Noisy object in use.");
		}

		return false;
	}

	activate_stealth(character);

	if (character->party == PARTY_PLAYER) {

		dynamic_message(MSG_FADE_INTO_SHADOWS,
			character, NULL, MOT_NIL
		);
	}

	return true;
}



/*
 * perk: disarm (use)
 */
void disarm_use(CHARACTER *character, CHARACTER *target)
{

	if (!d100_test_passed(character->stat[S_CC].current) ||
		d100_test_passed(target->stat[S_CC].current)) {

		dynamic_message(MSG_DISARM_FAILS, character, target,
			MOT_CHARACTER
		);

		return;
	}

	action_drop_object(target, target->weapon, true);

	dynamic_message(MSG_DISARM, character, target, MOT_CHARACTER);
}



/*
 * perk: uncanny dodge (hit modifier)
 */
HIT_MODIFIER uncanny_dodge_modifier(const CHARACTER *character)
{
	const OBJECT *armour;

	armour = character->armour;

	if (armour == NULL) {

		return UNCANNY_DODGE_MODIFIER;
	}

	if (armour->subtype == OSTYPE_HEAVY) {

		return 0;
	}

	return UNCANNY_DODGE_MODIFIER;
}



/*
 * perk: rapid strike
 * returns true if the requirements for a double strike are met
 */
bool rapid_strike_requirements_met(const CHARACTER *character,
	const OBJECT *character_weapon
)
{

	if (character_weapon == NULL) {

		return true;
	}

	if (character_weapon->weight > character->stat[S_ST].current / 2) {

		return false;
	}

	return true;
}









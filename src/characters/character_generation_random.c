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
 * Module: Random Character Generation
 */

#include "wrogue.h"



static void		give_random_ep(CHARACTER *);

static void		choose_career(CHARACTER *);

static void		choose_perks(CHARACTER *);
static bool		is_psychic_perk(PERK);
static PERK		pick_correct_psychic_perk(const CHARACTER *, 
				LIST *, LIST_NODE **
			);

static void		choose_psy_powers(CHARACTER *);

static void		choose_advances(CHARACTER *);

static void		roll_gender(CHARACTER *);



/*
 * EP table for the random character generator 
 */
static const EP_TABLE_ITEM	EP_Table[MAX_CAREER_TYPES] = {
	
	/* BASIC */	{0,	1000,	0},
	/* ADVANCED */	{1000,	2000,	EP_COST_CHANGE_CAREER_FOLLOW_PATH},
	/* ELITE */ 	{2000,	3000,	EP_COST_CHANGE_CAREER_FOLLOW_PATH * 2}

};



/*
 * generates a random recruitable character who belongs the player 
 * characters's race. his attributes will based on the race profile 
 * + career data.
 */
CHARACTER * generate_random_character(void)
{
	/* create new character */
	CHARACTER *character = character_new();

	/* set character symbol to the symbol of the player's race */
	character->symbol = player_race()->symbol;
	
	/* set the unique attribute */
	character_set_attribute(character, CA_UNIQUE);
	
	/* set character script */
	strcpy(character->script, FILE_SCRIPT_RANDOM_RECRUITABLE);
	
	/* set character faction */
	character->faction = FACTION_RANDOM_RECRUITABLE;
	
	/* roll the character's stats */
	roll_stats(character);
	
	/* roll the character's gender */
	roll_gender(character);
	
	/* roll the character's name */
	roll_name(character);

	/* choose a career */
	choose_career(character);

	/* give random amount of EP to the character*/
	give_random_ep(character);
	
	/* choose perks */
	choose_perks(character);
	
	/* choose psychic powers */
	choose_psy_powers(character);
	
	/* choose advances */
	choose_advances(character);

	/* give the character his career trappings */
	career_give_trappings(character);
	
	/* make the character equip the objects in his inventory */
	equip_objects(character);

	/* give back unspent EP */
	if (character->ep > 0) {
		character->ep_total -= character->ep;
		character->ep = 0;
	}

	return character;
}



/*
 * gives the passed character a random amount of EPs
 */
static void give_random_ep(CHARACTER *character)
{
	const EP_TABLE_ITEM *ep = &EP_Table[character->career->type];
	
	character->ep = character->ep_total = 
		random_int(ep->minimum / 100, ep->maximum / 100) * 100;
	
	character->ep_total += ep->base;
}



/*
 * chooses the passed character's career
 */
static void choose_career(CHARACTER *character)
{
	/* randomly select a career */
	CAREER_INDEX career_index = random_int(0, get_n_careers() - 1);

	/* set the chosen career */
	career_set(character, career_index);
}



/*
 * chooses the passed character's perks
 */
static void choose_perks(CHARACTER *character)
{
	EXPERIENCE_POINTS perk_ep;
	LIST *available_perks;
	PERK perk;
	
	/* 
	 * automatically choose the required perks
	 */
	iterate_over_perks(perk) {
		
		/* perk is required ..*/
		if (career_perk_required(character->career, perk)) {
			
			/* give perk */
			give_perk(character, perk);

			/* deduct EP */
			if (character->ep - EP_COST_BUY_PERK > 0) {
				
				character->ep -= EP_COST_BUY_PERK;
			} else {

				character->ep_total += EP_COST_BUY_PERK;
			}
		}
	}
	
	
	/* 1/3 of the remaining EP may be used for buying perks */
	perk_ep = (character->ep / 300) * 100;

	/* no EP for additional perks - return */
	if (perk_ep == 0) return;
		
	/* 
	 * more perk EP than necessary to buy the maximal number of 
	 * perks 
	 */
	if (perk_ep > MAX_CHARACTER_PERKS * EP_COST_BUY_PERK) {

		/* reduce perk EP */
		perk_ep = MAX_CHARACTER_PERKS * EP_COST_BUY_PERK;
	}

	/* build list of available perks */
	available_perks = list_new();
	iterate_over_perks(perk) {

		/* perk available AND not required .. */
		if (career_has_perk(character->career, perk) &&
			!career_perk_required(character->career, perk)) {

			/* add to list */
			list_add(available_perks, (void *)perk_name(perk));
		}
	}

	/* no perks available - return */
	if (available_perks->n_nodes == 0) {

		/* free available perks list*/
		list_free(available_perks);
		return;
	}

	/* more perk EP than necessary to buy all available perks .. */
	if (perk_ep > available_perks->n_nodes * EP_COST_BUY_PERK) {
		
		/* reduce perk EP */
		perk_ep = available_perks->n_nodes * EP_COST_BUY_PERK;
	}

	/* 
	 * substract the amount of EP assigned to perks 
	 * from the character's EP 
	 */
	character->ep -= perk_ep;

	/* 
	 * buy perks, randomly chosen from the list, until the perk EPs
	 * are exhausted
	 */
	while (perk_ep >= EP_COST_BUY_PERK) {
		/* randomly choose a perk */
		LIST_NODE_INDEX r = 
			random_int(0, available_perks->n_nodes - 1);
		LIST_NODE *perk_node = list_node_at(available_perks, r);
		
		perk = name_to_perk(perk_node->data);
		
		/* 
		 * ensure that the Psychic perks are picked 
		 * in correct order 
		 */
		if (is_psychic_perk(perk)) {
			
			perk = pick_correct_psychic_perk(character,
				available_perks, &perk_node
			);
		}

		/* give the chosen perk to the character */
		give_perk(character, perk);
		
		/* reduce perk EP */
		perk_ep -= EP_COST_BUY_PERK;

		/* remove the chosen perk from the list */
		list_remove_node(available_perks, perk_node);
	}

	/* free available perks list */
	list_free(available_perks);
}


/*
 * returns true if the passed perk is a Psychic perk
 */
static bool is_psychic_perk(PERK perk)
{
	if (perk == PK_PSYCHIC_I ||
		perk == PK_PSYCHIC_II ||
		perk == PK_PSYCHIC_III) {
		
		return true;
	}
	
	return false;
}



/*
 * used for picking Psychic perks in the correct order 
 */
static PERK pick_correct_psychic_perk(const CHARACTER *character, 
	LIST *perk_list, LIST_NODE **perk_node
)
{
	PERK correct_perk;
	
	if (character_has_perk(character, PK_PSYCHIC_II)) {
	
		correct_perk = PK_PSYCHIC_III;
	
	} else if (character_has_perk(character, PK_PSYCHIC_I)) {
	
		correct_perk = PK_PSYCHIC_II;
		
	} else {
	
		correct_perk = PK_PSYCHIC_I;
	}

	*perk_node = list_node_of(perk_list, perk_name(correct_perk));

	return correct_perk;
}



/*
 * chooses the passed character's psychic powers
 */
static void choose_psy_powers(CHARACTER *character)
{
	EXPERIENCE_POINTS psy_ep; /* EPs assigned to psychic powers */
	LIST *available_powers; /* list of available psychic powers */
	PSY_POWER power; /* the currently processed power */

	/* no psychic perk - return */
	if (!character_has_perk(character, PK_PSYCHIC_I)) return;

	/* 1/2 of the remaing EP may be used for buying psychic powers */
	psy_ep = (character->ep / 200) * 100;
	
	/* no psy EP - return */ 
	if (psy_ep == 0) return;

	/* 
	 * more psy EP than necessary to buy the maximal number of 
	 * psychic powers 
	 */
	if (psy_ep > MAX_CHARACTER_PSY_POWERS * EP_COST_LEARN_PSY_POWER) {

		/* reduce psy EP */
		psy_ep = MAX_CHARACTER_PSY_POWERS * EP_COST_LEARN_PSY_POWER;
	}

	/* build list of available powers */
	available_powers = list_new();
	iterate_over_psy_powers(power) {

		/* psychic power is implemented .. */
		if (psy_power_implemented(power)) {

			/* add to list */
			list_add(available_powers, 
				(void *)psy_power_name(power)
			);
		}
	}
	
	/* no powers available - return */
	if (available_powers->n_nodes == 0) {

		/* free available powers list */
		list_free(available_powers);
		return;
	}

	/* more psy EP than neccessary to buy all available powers .. */
	if (psy_ep > available_powers->n_nodes * EP_COST_LEARN_PSY_POWER) {

		/* reduce psy EP */
		psy_ep = available_powers->n_nodes * 
			EP_COST_LEARN_PSY_POWER;
	}

	/* substract psy EP from character EP */
	character->ep -= psy_ep;

	/* 
	 * buy powers, randomly chosen from the list, until the psy EPs
	 * are exhausted
	 */
	while (psy_ep >= EP_COST_LEARN_PSY_POWER) {
		/* randomly choose a power */
		LIST_NODE_INDEX r = 
			random_int(0, available_powers->n_nodes - 1);
		LIST_NODE *power_node = list_node_at(available_powers, r);

		power = name_to_psy_power(power_node->data);

		/* buy power */
		give_psy_power(character, power);
		psy_ep -= EP_COST_LEARN_PSY_POWER;
		
		/* remove the chosen power from the list */
		list_remove_node(available_powers, power_node);	
	}

	/* free available powers list */
	list_free(available_powers);
}



/*
 * chooses the passed character's stat advances
 */
static void choose_advances(CHARACTER *character)
{
	const CAREER *career = character->career;
	STAT stat;
	
	/* increase stats until the career requirements are met */
	iterate_over_stats(stat) {
		
		while (character->stat[stat].current < 
			career->stat[stat].required_minimum) {
			
			stat_advance(character, stat);

			if (character->ep - EP_COST_INCREASE_STAT > 0) {
				
				character->ep -= EP_COST_INCREASE_STAT;
			} else {
			
				character->ep_total += EP_COST_INCREASE_STAT;
			}
		}	
	}

	/* increase stats until the EP run out */
	while (character->ep >= EP_COST_INCREASE_STAT) {
		LIST *available_advancements = list_new();

		/* build list of available advancements */
		iterate_over_stats(stat) {

			if (character->career->stat[stat].advance_maximum > 
				character->stat[stat].advance) {

				list_add(available_advancements, 
					(void *)stat_short_name(stat)
				);
			}
		}
		
		/* no advancements available - return */
		if (available_advancements->n_nodes == 0) {
			
			list_free(available_advancements);
			return;
		
		/* increase a randomly chosen stat */
		} else {
			LIST_NODE_INDEX r = random_int(0, 
				available_advancements->n_nodes - 1);
			LIST_NODE *stat_node = list_node_at(
				available_advancements, r
			);
			
			stat = name_to_stat(stat_node->data, true);
	
			stat_advance(character, stat);

			character->ep -= EP_COST_INCREASE_STAT;
		}
	
		list_free(available_advancements);
	}
}



/*
 * rolls the passed character's gender
 */
static void roll_gender(CHARACTER *character)
{

	if (random_choice(MALE_CHARACTER_PROBABILITY)) {
		
		character->gender = GENDER_MALE;
	
	} else {

		character->gender = GENDER_FEMALE;
	}
}


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
 * Module: Timing
 */

#include "wrogue.h"



static void			hand_out_actions(void);

static void			character_actions(void);

static CHARACTER *		next_character_to_act(void);

static INITIATIVE		initiative(const CHARACTER *);



/*
 * the currently active character
 */
static CHARACTER *	ActiveCharacter = NULL;




/*
 * executes a game round
 */
void game_round(void)
{

	hand_out_actions();

	character_actions();

	handle_events();
}



/*
 * returns true if the passed character is currently active
 */
bool is_active_character(const CHARACTER *character)
{
	if (character == ActiveCharacter) return true;

	return false;
}



/*
 * returns true if the passed character can act
 */
bool character_can_act(const CHARACTER *character)
{

	if (!character_has_attribute(character, CA_BUSY) &&
		!character_has_attribute(character, CA_STUNNED)) {

		return true;
	}

	return false;
}





/*
 * hands out Actions to the characters
 */
static void hand_out_actions(void)
{
	LIST_NODE *node;

	for (node = area_character_list()->head;
		node != NULL;
		node = node->next) {
		CHARACTER *character = (CHARACTER *)node->data;

		character->action_spent = false;
	}
}




/*
 * lets the characters act
 */
static void character_actions(void)
{

	while ((ActiveCharacter = next_character_to_act()) != NULL) {

		update_perception_data(ActiveCharacter);

		if (ActiveCharacter->controller == CC_PLAYER) {

			player_control(ActiveCharacter);
		}

		/* switch character hack - do not merge these if statements! */
		if (ActiveCharacter->controller == CC_AI) {

			ai_control(ActiveCharacter);
		}

		if (character_killed(ActiveCharacter)) {

			character_destroy(ActiveCharacter);
		}
	}
}



/*
 * returns the character who acts next
 */
static CHARACTER * next_character_to_act(void)
{
	LIST_NODE *node = area_character_list()->head;
	CHARACTER *character_to_act = NULL;

	/* iterate over the entire character list */
	for (; node != NULL; node = node->next) {
		CHARACTER *character = (CHARACTER *)node->data;

		/* 
		 * the character has already spent his action OR
		 * he cannot act ..
		 */
		if (character->action_spent ||
			!character_can_act(character)) {
		
			/* he won't be the next character to act for sure */
			continue;
		}

		if (character_to_act == NULL) {

			character_to_act = character;
			continue;
		}

		if (initiative(character) > 
			initiative(character_to_act)) {

			character_to_act = character;
		}
	}

	return character_to_act;
}



/*
 * returns the Initiative of the passed character
 */
static INITIATIVE initiative(const CHARACTER *character)
{
	/* Initiative is usually equal to Agility */
	INITIATIVE initiative_value = character->stat[S_AG].current;
	
	/* the character's party has Battle tactics .. */
	if (character->party == PARTY_PLAYER &&
		party_has_battle_tactics()) {
	
		/* Initiative bonus */
		initiative_value += BATTLE_TACTICS_BONUS;
	}
	
	return initiative_value;
}



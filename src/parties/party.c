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
 * Module: Party
 */

#include "wrogue.h"



static void		party_reorder(void);



/*
 * list of all characters in the player's party
 */
static LIST *		PlayerParty = NULL;



/*
 * party module init
 */
void party_init(void)
{

	PlayerParty = list_new();
}



/*
 * party module clean up
 */
void party_clean_up(void)
{

	if (PlayerParty == NULL) {

		return;
	}

	list_free(PlayerParty);
}



/*
 * returns a list of all characters in the player's party
 */
LIST * party_player(void)
{

	return PlayerParty;
}



/*
 * makes a character join the player's party
 */
void party_join(CHARACTER *character)
{

	character->party = PARTY_PLAYER;

	ai_set_state(&character->ai, AI_STATE_FOLLOW);
	ai_set_default_state(&character->ai, AI_STATE_FOLLOW);

	list_add(PlayerParty, character);

	party_reorder();
}



/*
 * makes a character leave the player's party
 */
void party_leave(CHARACTER *character)
{

	character->party = PARTY_NIL;

	ai_set_state(&character->ai, character->ai.original_state);
	ai_set_default_state(&character->ai, character->ai.original_state);

	list_remove(PlayerParty, character);

	party_reorder();
}



/*
 * returns the number of characters in the player's party
 */
PARTY_SIZE party_n_members(void)
{

	if (PlayerParty == NULL) {

		return 0;
	}

	return PlayerParty->n_nodes;
}



/*
 * returns true if the passed character is a member of the player's party
 */
bool party_is_member(const CHARACTER *character)
{

	if (character->party == PARTY_PLAYER) return true;

	return false;
}



/*
 * sets the AI tactic of the player's party
 */
void party_set_tactic(AI_TACTIC tactic)
{
	LIST_NODE *node;

	iterate_over_list(PlayerParty, node) {
		CHARACTER *character = node->data;

		if (is_player_controlled_character(character)) {

			continue;
		}

		ai_set_tactic(&character->ai, tactic);
 	}
}



/*
 * returns the power level of the player's party
 */
POWER_LEVEL party_power_level(void)
{
	POWER_LEVEL party_power = PL_NIL;
	LIST_NODE *node;

	iterate_over_list(PlayerParty, node) {
		const CHARACTER *character = node->data;

		if (character->power_level > party_power) {

			party_power = character->power_level;
		}
 	}

	return party_power;
}



/*
 * returns true if the player's party contains at least
 * one character who has the 'Battle tactics' perk
 */
bool party_has_battle_tactics(void)
{
	LIST_NODE *node;
	bool has_tactics = false;

	iterate_over_list(PlayerParty, node) {
		const CHARACTER *character = node->data;

		if (character_has_perk(character, PK_BATTLE_TACTICS)) {
			
			has_tactics = true;
			break;
		}
 	}

	return has_tactics;
}



/*
 * returns true if the player's party contains at least
 * one character who has the 'Leader' perk
 */
bool party_has_leader(void)
{
	LIST_NODE *node;
	bool has_leader = false;

	iterate_over_list(PlayerParty, node) {
		const CHARACTER *character = node->data;

		if (character_has_perk(character, PK_LEADER)) {
			has_leader = true;
			break;
		}
 	}

	return has_leader;
}



/*
 * switches to another party character
 */
void switch_character(CHARACTER_TYPE character_type)
{
	LIST_NODE *node;

	iterate_over_list(PlayerParty, node) {
		CHARACTER *character = node->data;

		if (character->type == character_type) {

			set_player_controlled_character(character);
			break;
		}
 	}
}



/*
 * reorders player's party
 */
static void party_reorder(void)
{
	LIST_NODE *node;
	bool first_companion = true;

	iterate_over_list(PlayerParty, node) {
		CHARACTER *character = node->data;

		if (character->type == CT_PC) {

			continue;
		}

		if (first_companion) {

			character->type = CT_COMPANION_1;

			first_companion = false;

		} else {

			character->type = CT_COMPANION_2;
		}
 	}
}


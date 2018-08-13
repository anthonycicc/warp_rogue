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
 * Module: Character
 */

#include "wrogue.h"



static void *		character_new_full(void);
static void 		character_free_full(void *);

static CHARACTER_INDEX	name_to_character_index(const char *);

static void		character_screen_setup_command_bar(const CHARACTER *);

static void		character_screen_basics(const CHARACTER *);




/*
 * the player character pointer
 */
static CHARACTER *	PlayerCharacter = NULL;

/*
 * the player controlled character pointer
 */
static CHARACTER *	PlayerControlledCharacter = NULL;




/*
 * the character box
 */
static BOX *		CharacterBox = NULL;

/*
 * direct link to character data for fast accesss
 */
static void **		Character;






/*
 * character template (default values)
 */
static const CHARACTER CharacterTemplate = {

	/* NAME */ "", /* NUMERUS */ NUMERUS_SINGULAR,
	/* SYMBOL */ 'E', /* COLOUR */ C_NIL,
	/* GENDER */ GENDER_NEUTER, /* DESCRIPTION */ NULL,
	/* TYPE */ CT_OTHER, /* CONTROLLER */ CC_AI,
	/* LOCATION */ {AREA_COORD_NIL, AREA_COORD_NIL},
	/* FACTION */ FACTION_NIL, /* INJURY */ 0, /* BLOODY_FEET */ 0,
	/* ACTION_SPENT */ false,
	/* STATS */ {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	/* ATTRIBUTES */ {false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,false},
	/* PERKS */ {false,false,false,false,
	false,false,false,false,false,
	false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false},
	/* PSY_POWERS */ {false,false,false,false,false,
	false,false,false,false,false},
	/* CAREER */ NULL, /* EP */ 0,  /* EP_TOTAL */ 0, 
	/* POWER_LEVEL */ PL_NIL, /* PARTY */ PARTY_NIL,
	/* INVENTORY */ NULL,
	/* EQUIPMENT */ NULL, NULL, NULL, NULL, {0, 0},
	/* NOTICED_ENEMIES */ NULL,
	/* AI */ {
	AI_STATE_GUARD, AI_STATE_GUARD, AI_STATE_GUARD, TACTIC_PLAN_A,
	{AREA_COORD_NIL, AREA_COORD_NIL}, NULL, AREA_DISTANCE_NIL, NULL,
	false, NULL, false
	},
	/* SCRIPT */ "",
	/* SN_DATA */ SERIAL_NIL, /* SN_SAVE */ SERIAL_NIL

};




/*
 * Character module init
 */
void character_init(void)
{

	CharacterBox = box_new(character_new_full, character_free_full,
		CHARACTER_BOX_BUFFER_SIZE
	);
}



/*
 * Character module clean up
 */
void character_clean_up(void)
{

	if (CharacterBox != NULL) {

		box_free(CharacterBox);
	}
}



/*
 * adds a new character to the character box
 */
CHARACTER * character_box_new_character(void)
{
	CHARACTER *character = box_new_item(CharacterBox);

	character->sn_data = CharacterBox->current_size - 1;

	return character;
}



/*
 * optimises the character box
 */
void character_box_optimise(void)
{

	box_optimise(CharacterBox);

	Character = CharacterBox->item;
}



/*
 * returns the size of the character box
 */
N_CHARACTERS character_box_size(void)
{

	return CharacterBox->current_size;
}




/*
 * allocs a character
 */
void * character_new(void)
{
	CHARACTER *character;

	character = checked_malloc(sizeof *character);

	*character = CharacterTemplate;

	character->ai.self = character;

	character->inventory = list_new();

	character->noticed_enemies = list_new();

	return character;
}



/*
 * frees a character
 */
void character_free(void *data)
{
	CHARACTER *character;

	character = (CHARACTER *)data;

	list_free_with(character->inventory, object_free);

	list_free(character->noticed_enemies);

	free(character);
}




/*
 * creates a character (name based)
 */
CHARACTER * character_create(const char *character_name)
{
	CHARACTER_INDEX character_index = 
		name_to_character_index(character_name);

	return character_create_i(character_index);
}



/*
 * creates a character (index based)
 */
CHARACTER * character_create_i(CHARACTER_INDEX character_index)
{

	/* randomly generated, recruitable character requested .. */
	if (character_index == CHARACTER_INDEX_RANDOM_RECRUITABLE) {

		/* generate character and return it */
		return generate_random_character();
	} 

	/* 
	 * create the character by creating a clone of his prototype 
	 * return that clone
	 */
	return character_clone(Character[character_index]);
}



/*
 * destroys a character
 * i.e. free + handling the complex data management mess
 */
void character_destroy(void *data)
{
	CHARACTER *character;
	LIST *character_events;

	character = (CHARACTER *)data;

	character_events = list_new();
	remove_character(character, character_events);
	list_free_with(character_events, event_destroy);

	if (character == PlayerControlledCharacter) {

		PlayerControlledCharacter = NULL;
	}

	if (character == PlayerCharacter) {

		PlayerCharacter = NULL;
	}

	character_free(character);
}



/*
 * produces a copy of the passed character
 *
 * do NOT use this function to clone already created characters,
 * because this function clones everything, including AI data
 *
 */
CHARACTER * character_clone(const CHARACTER *character)
{
	CHARACTER *clone = checked_malloc(sizeof *clone);
	LIST_NODE *node;

	*clone = *character;

	clone->noticed_enemies = list_new();
	
	/* 
	 * clone inventory 
	 */
	clone->inventory = list_new();
	
	iterate_over_list(character->inventory, node) {
		const OBJECT *object = node->data;
		OBJECT *cloned_object;

		cloned_object = object_clone(object);

		inventory_add(clone, cloned_object);

		if (object == character->weapon) {

			clone->weapon = cloned_object;

		} else if (object == character->secondary_weapon) {

			clone->secondary_weapon = cloned_object;

		} else if (object == character->armour) {

			clone->armour = cloned_object;

		} else if (object == character->jump_pack) {

			clone->jump_pack = cloned_object;
		}
	}

	return clone;
}



/*
 * makes the passed character the new player character
 */
void set_player_character(CHARACTER *character)
{

	PlayerCharacter = character;
}



/*
 * returns the player character
 */
CHARACTER * player_character(void)
{

	return PlayerCharacter;
}



/*
 * makes the passed character the new player controlled character
 */
void set_player_controlled_character(CHARACTER *character)
{

	if (PlayerControlledCharacter != NULL) {

		PlayerControlledCharacter->controller = CC_AI;
		PlayerControlledCharacter->symbol = 'H';
	}

	PlayerControlledCharacter = character;

	if (PlayerControlledCharacter == NULL) {

		return;
	}

	PlayerControlledCharacter->controller = CC_PLAYER;
	PlayerControlledCharacter->symbol = '@';
}



/*
 * returns the player controlled character
 */
CHARACTER * player_controlled_character(void)
{

	return PlayerControlledCharacter;
}



/*
 * returns the description of the passed character
 */
const char * character_description(const CHARACTER *character)
{

	/* character is a predefined character .. */
	if (character->sn_data != SERIAL_NIL) {

		/* return the prototype description if available */
		const CHARACTER *prototype = Character[character->sn_data];
		
		if (!is_empty_string(prototype->description)) {

			return prototype->description;
		}
	}

	/* return career description if available */
	if (character->career != NULL) {
		
		return character->career->description;
	}

	return NULL;
}



/*
 * the character screen
 */
void character_screen(CHARACTER *character)
{
	COMMAND entered_command;

	do {

		character_screen_setup_command_bar(character);
		character_screen_basics(character);

		entered_command = command_bar_get_command();

		if (entered_command == CM_PSY_POWERS) {

			psy_powers_screen(character);

		} else if (entered_command == CM_CAREER) {

			career_screen(character->career);

		} else if (entered_command == CM_ADVANCEMENT) {

			character_advancement_screen(character);

		} else if (entered_command == CM_RENAME) {

			name_character(character);
		}

	} while (entered_command != CM_EXIT);


}



/*
 * the name character screen
 */
void name_character(CHARACTER *character)
{
	GET_TEXT_DIALOGUE dialogue;

	sprintf(dialogue.prompt,
		"Name (limit: %d characters / default: random): ",
		CHARACTER_NAME_SIZE - 1
	);

	dialogue.colour = C_FieldName;
	dialogue.max_length = CHARACTER_NAME_SIZE - 1;

	command_bar_set(1, CM_OK);

	render_name_character_screen();
	update_screen();

	enter_name_dialogue(&dialogue);

	strcpy(character->name, dialogue.entered_text);

	if (strlen(character->name) < 2) {

		roll_name(character);
	}
}



/*
 * effect of the recover action
 */
void character_recover(CHARACTER *character)
{
	int test_value;

	dynamic_message(MSG_RECOVER, character, NULL, MOT_NIL);

	test_value = character->stat[S_TN].current;

	if (character_has_perk(character, PK_FIRST_AID)) {

		test_value += FIRST_AID_RECOVERY_BONUS;
	}

	if (character_has_attribute(character, CA_DISEASED) &&
		d100_test_passed(test_value +
			DISEASED_RECOVERY_MODIFIER)) {

		effect_terminate(character, ET_DISEASED);
	}

	if (character->injury > 0) {

		if (d100_test_passed(test_value)) {

			character_recover_injuries(character);
		}
	}
}



/*
 * recover injuries effect
 */
void character_recover_injuries(CHARACTER *character)
{
	int bonus;

	bonus = stat_bonus(character, S_TN);

	character->injury -= dice(1, 3) + bonus;

	if (character->injury < 0) {

		character->injury = 0;
	}
}



/*
 * returns true if the character must recover to regain full strength
 */
bool character_must_recover(const CHARACTER *character)
{

	if (character->injury > 0 ||
		character_has_attribute(character, CA_DISEASED)) {

		return true;
	}

	return false;
}



/*
 * returns the maximal number of injuries the passed character can survive
 */
INJURY injury_max(const CHARACTER *character)
{
	INJURY maximum;

	maximum = character->stat[S_TN].total;

	return maximum;
}



/*
 * returns true if the character has a ranged attack
 */
bool has_ranged_attack(const CHARACTER *character)
{

	if (character->weapon != NULL &&
		character->weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {

		return true;
	}

	if (character->secondary_weapon != NULL &&
		character->secondary_weapon->type ==
			OTYPE_RANGED_COMBAT_WEAPON) {

		return true;
	}

	return false;
}





/*
 * allocates a character (full version)
 */
static void * character_new_full(void)
{
	CHARACTER *character = character_new();

	character->description = checked_malloc(DESCRIPTION_SIZE *
		sizeof *character->description
	);

	clear_string(character->description);

	return character;
}



/*
 * frees a character (full version)
 */
static void character_free_full(void *p)
{
	CHARACTER *character = (CHARACTER *)p;

	free(character->description);

	character_free(character);
}



/*
 * name -> character index
 */
static CHARACTER_INDEX name_to_character_index(const char *name)
{
	CHARACTER_INDEX i;

	if (strings_equal(name, "RANDOM_RECRUITABLE")) {

		return CHARACTER_INDEX_RANDOM_RECRUITABLE;
	}

	for (i = 0; i < CharacterBox->current_size; i++) {
		const CHARACTER *character = Character[i];

		if (strings_equal(name, character->name)) return i;
	}

	die("*** CORE ERROR *** invalid character: %s", name);

	/* never reached */
	return CHARACTER_INDEX_NIL;
}



/*
 * sets up the character screen command bar
 */
static void character_screen_setup_command_bar(const CHARACTER *character)
{

	command_bar_clear();

	if (n_psy_powers(character) > 0) {

		command_bar_add(1, CM_PSY_POWERS);
	}

	if (character->career != NULL) {

		command_bar_add(1, CM_CAREER);
	}

	if (character->party == PARTY_PLAYER) {

		command_bar_add(2, CM_ADVANCEMENT, CM_RENAME);
	}

	command_bar_add(1, CM_EXIT);
}



/*
 * the character screen basics
 */
static void character_screen_basics(const CHARACTER *character)
{
	render_character_screen_basics(character);

	update_screen();
}






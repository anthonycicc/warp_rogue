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
 * Module: Manual Character Generation
 */

#include "wrogue.h"



static void		choose_gender(CHARACTER *);

static void		roll_stats_screen(CHARACTER *);




/*
 * generates the player character
 */
void generate_player_character(void)
{
	CHARACTER *character;

	character = character_new();

	character->symbol = player_race()->symbol;
	character->type = CT_PC;
	character->faction = FACTION_PLAYER;
	character_set_attribute(character, CA_UNIQUE);

	choose_gender(character);

	new_career_path(character, true);

	roll_stats_screen(character);

	increase_stat(character, true);

	career_give_trappings(character);

	equip_objects(character);
	
	roll_name(character);

	set_player_character(character);

	set_player_controlled_character(character);

	party_join(character);
}




/*
 * rolls the passed character's name
 */
void roll_name(CHARACTER *character)
{
	NAME_SET chosen_set;
	
	if (character->gender == GENDER_MALE) {
		
		chosen_set = NAME_SET_MALE;
		
	} else if (character->gender == GENDER_FEMALE) {
	
		chosen_set = NAME_SET_FEMALE;
		
	} else {
		
		die("*** CORE ERROR *** roll_name() bug");
		
		/* NEVER REACHED */
		return;
	}

	strcpy(character->name, random_name(chosen_set));
}



/*
 * (re)rolls the stats of the passed character
 */
void roll_stats(CHARACTER *character)
{
	const RACE *race = player_race();
	STAT stat;
	
	iterate_over_stats(stat) {
		const DICE_ROLL *stat_roll = &race->stat[stat];

		character->stat[stat].current = 
			character->stat[stat].total = 
			dice_roll(stat_roll);
	}
}



/*
 * the choose gender screen
 */
static void choose_gender(CHARACTER *character)
{
	MENU *menu;
	LIST *items;

	items = list_new();
	list_add(items, "Male");
	list_add(items, "Female");
	menu = menu_create(items);

	do {
		command_bar_set(3, CM_UP, CM_DOWN, CM_OK);
		render_choose_gender_screen();
		update_screen();

		choose_gender_screen_menu(menu);

		if (menu->entered_command == CM_OK) {

			character->gender = (GENDER)menu->highlighted;
		}

	} while (menu->entered_command != CM_OK);

	menu_free(menu);
}



/*
 * the roll stats screen
 */
static void roll_stats_screen(CHARACTER *character)
{
	COMMAND entered_command;

	entered_command = CM_REROLL;

	while (entered_command == CM_REROLL) {
		
		roll_stats(character);
		
		command_bar_set(2, CM_OK, CM_REROLL);

		render_roll_stats_screen(character);

		update_screen();

		entered_command = command_bar_get_command();
	}
}








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
 * Module: Character Advancement
 */

#include "wrogue.h"



static MENU *	increase_stat_menu_create(const CHARACTER *);

static MENU *	buy_perk_menu_create(const CHARACTER *);

static MENU *	learn_psy_power_menu_create(const CHARACTER *);

static void	follow_career_path(CHARACTER *);
static MENU *	follow_career_path_menu_create(const CHARACTER *);

static MENU *	new_career_path_menu_create(void);

static void	give_career_perks(CHARACTER *);

static PERK	missing_required_perk(const CHARACTER *, PERK);



/*
 * gives the passed amount of EPs to the passed character
 */
void give_ep(CHARACTER *character, EXPERIENCE_POINTS ep)
{

	/* negative amount of EP OR EP limit already reached - return */
	if (ep <= 0 || character->ep == EP_LIMIT) return;

	/* the amount of EP is higher than the EP limit .. */
	if (ep > EP_LIMIT) {

		/* reduce the amount to EP limit */
		ep = EP_LIMIT;
	}

	/*  giving the character all EP would break the the EP limit .. */
	if (character->ep + ep > EP_LIMIT) {

		/*
		 * reduce the amount of EP to a value that pushes the
		 * character to the EP limit but not further
		 */
		ep = EP_LIMIT - character->ep;
	}

	/* give EP */
	character->ep += ep;
	character->ep_total += ep;
}




/*
 * the character advancement screen
 */
void character_advancement_screen(CHARACTER *character)
{
	LIST *items;
	MENU *menu;

	/* create advancement menu */
	items = list_new();
	list_add(items, "Increase stat");
	list_add(items, "Buy perk");
	list_add(items, "Learn psychic power");
	list_add(items, "Change career (follow path)");
	list_add(items, "Change career (new path)");
	menu = menu_create(items);

	while (true) {

		command_bar_set(4, CM_UP, CM_DOWN, CM_OK, CM_EXIT);
		render_character_advancement_screen(character);
		update_screen();

		character_advancement_screen_menu(menu);

		if (menu->entered_command == CM_EXIT) break;

		if (character->career == NULL) continue;

		switch (menu->highlighted) {

		case 0:
			increase_stat(character, false);
			break;

		case 1:
			new_perk(character);
			break;

		case 2:
			if (character_has_perk(character, PK_PSYCHIC_I) &&
				n_psy_powers(character) == 0) {

				learn_psy_power(character, true);

			} else {

				learn_psy_power(character, false);
			}
			break;

		case 3:
			follow_career_path(character);
			break;

		case 4:
			new_career_path(character, false);
			break;
		}
	}

	menu_free(menu);
}



/*
 * the new career path screen
 */
void new_career_path(CHARACTER *character, bool initial_career)
{
	MENU *menu;

	if (!initial_career &&
		(!career_completed(character) ||
		character->ep < EP_COST_CHANGE_CAREER_NEW_PATH)) {

		command_bar_set(1, CM_EXIT);

		render_new_career_path_screen(character, initial_career);
		update_screen();

		command_bar_get_command();
		return;
	}

	menu = new_career_path_menu_create();

	while (true) {
		CAREER_INDEX career_index;
		const char *career_name;
		CAREER *career;

		if (initial_career) {

			command_bar_set(4, CM_UP, CM_DOWN, CM_OK,
				CM_INFO
			);

		} else {

			command_bar_set(5, CM_UP, CM_DOWN, CM_OK, CM_INFO,
				CM_EXIT
			);
		}

		render_new_career_path_screen(character, initial_career);
		update_screen();

		new_career_path_screen_menu(menu, initial_career);

		if (menu->entered_command == CM_EXIT) {

			break;
		}

		career_name = (const char *)list_data_at(menu->items,
			menu->highlighted
		);

		career_index = name_to_career_index(career_name);
		career = get_career_pointer(career_index);

		if (menu->entered_command == CM_INFO) {

			career_screen(career);
			continue;

		} else if (menu->entered_command == CM_OK) {

			career_set(character, career_index);

			if (initial_career) {

				give_career_perks(character);
			} else {

				character->ep -=
					EP_COST_CHANGE_CAREER_NEW_PATH;

				message(SID_CHANGE_CAREER_NEW_PATH,
					"Career changed!"
				);
			}

			break;
		}

	}

	if (menu != NULL) {

		menu_free(menu);
	}
}



/*
 * the increase stat screen
 */
void increase_stat(CHARACTER *character, bool free_advance)
{
	MENU *menu;
	COMMAND entered_command;

	menu = increase_stat_menu_create(character);

	do {
		STAT stat;
		const char *stat_name;

		if (menu == NULL ||

			(!free_advance &&
				character->ep < EP_COST_INCREASE_STAT)
		) {

			command_bar_set(1, CM_EXIT);

			render_increase_stat_screen(character, menu,
				free_advance
			);

			update_screen();

			command_bar_get_command();

			break;
		}

		if (free_advance) {

			command_bar_set(3, CM_UP, CM_DOWN, CM_OK);

		} else {

			command_bar_set(4, CM_UP, CM_DOWN, CM_OK, CM_EXIT);
		}

		render_increase_stat_screen(character, menu,
			free_advance
		);

		increase_stat_screen_menu(menu, free_advance);

		stat_name = list_data_at(menu->items, menu->highlighted);
		stat = name_to_stat(stat_name, false);

		entered_command = menu->entered_command;

		if (entered_command == CM_OK) {

			stat_advance(character, stat);

			message(SID_INCREASE_STAT, "%s stat increased!  ",
				stat_long_name(stat)
			);

			if (free_advance) {

				break;
			}

			character->ep -= EP_COST_INCREASE_STAT;

			menu_free(menu);

			menu = increase_stat_menu_create(character);

			entered_command = CM_NIL;
		}

	} while (entered_command != CM_EXIT);

	if (menu != NULL) {

		menu_free(menu);
	}
}



/*
 * the new perk screen
 */
bool new_perk(CHARACTER *character)
{
	MENU *menu;
	bool perk_gained;

	perk_gained = false;

	while (true) {
		const char *chosen_perk;
		PERK perk;

		menu = buy_perk_menu_create(character);

		if (menu == NULL || character->ep < EP_COST_BUY_PERK ||
			character_n_perks(character) >=
				MAX_CHARACTER_PERKS) {

			command_bar_set(1, CM_EXIT);

			render_new_perk_screen(character, menu);
			update_screen();

			command_bar_get_command();
			break;
		}

		command_bar_set(5, CM_UP, CM_DOWN, CM_OK, CM_INFO, CM_EXIT);

		render_new_perk_screen(character, menu);
		update_screen();

		new_perk_screen_menu(menu);

		if (menu->entered_command == CM_EXIT) {

			break;
		}

		chosen_perk = (const char *)list_data_at(menu->items,
			menu->highlighted
		);

		perk = name_to_perk(chosen_perk);

		if (menu->entered_command == CM_OK) {
			/* check for missing required perk */
			PERK required_perk = missing_required_perk(
				character, perk
			);

			/* the character lacks a required perk .. */
			if (required_perk != PK_NIL) {

				/* tell him */
				message(SID_NEW_PERK,
					"Required perk missing: %s",
					perk_name(required_perk)
				);

			/* all requirements met .. */
			} else {

				/* buy perk */
				character->ep -= EP_COST_BUY_PERK;

				give_perk(character, perk);

				perk_gained = true;

				message(SID_NEW_PERK, "%s perk gained!  ",
					perk_name(perk)
				);
			}

		} else if (menu->entered_command == CM_INFO) {

			perk_screen(perk);
		}

		menu_free(menu);
	}

	if (menu != NULL) {

		menu_free(menu);
	}

	return perk_gained;
}



/*
 * the learn psychic power screen
 */
bool learn_psy_power(CHARACTER *character, bool free_power)
{
	MENU *menu;
	bool power_learned;

	power_learned = false;

	while (true) {
		PSY_POWER power;
		const char *power_name;

		menu = learn_psy_power_menu_create(character);

		if (menu == NULL ||

			!character_has_perk(character, PK_PSYCHIC_I) ||

			(!free_power && character->ep <
					EP_COST_LEARN_PSY_POWER) ||

			n_psy_powers(character) >= MAX_CHARACTER_PSY_POWERS) {

			command_bar_set(1, CM_EXIT);

			render_learn_psy_power_screen(character, menu,
				free_power
			);

			update_screen();

			command_bar_get_command();

			break;
		}

		command_bar_set(5, CM_UP, CM_DOWN, CM_OK, CM_INFO, CM_EXIT);

		render_learn_psy_power_screen(character, menu, free_power);
		update_screen();

		learn_psy_power_screen_menu(menu, free_power);

		if (menu->entered_command == CM_EXIT) {

			break;
		}

		power_name = (const char *)list_data_at(menu->items,
			menu->highlighted
		);

		power = name_to_psy_power(power_name);

		if (menu->entered_command == CM_OK) {

			if (!free_power) {

				character->ep -= EP_COST_LEARN_PSY_POWER;
			}

			give_psy_power(character, power);

			message(SID_LEARN_PSY_POWER, "%s learned!  ",
				psy_power_name(power)
			);

			power_learned = true;

			if (free_power) {

				break;
			}

		} else if (menu->entered_command == CM_INFO) {

			psy_power_screen(power);
		}

		menu_free(menu);
	}

	if (menu != NULL) {

		menu_free(menu);
	}

	return power_learned;
}



/*
 * creates a increase stat menu
 */
static MENU * increase_stat_menu_create(const CHARACTER *character)
{
	LIST *items = list_new();
	MENU *menu;
	STAT stat;

	iterate_over_stats(stat) {

		if (character->stat[stat].advance >=
			character->career->stat[stat].advance_maximum) {

			continue;
		}

		list_add(items, (void *)stat_long_name(stat));
	}

	if (items->n_nodes == 0) {

		list_free(items);
		return NULL;
	}

	menu = menu_create(items);

	return menu;
}



/*
 * creates the buy perk menu
 */
static MENU * buy_perk_menu_create(const CHARACTER *character)
{
	LIST *items = list_new();
	MENU *menu;
	PERK perk;

	iterate_over_perks(perk) {

		if (!character_has_perk(character, perk)
			&& career_has_perk(character->career, perk)) {

			list_add(items, (void *)perk_name(perk));
		}
	}

	if (items->n_nodes == 0) {

		list_free(items);
	        return NULL;
	}

	menu = menu_create(items);

	return menu;
}



/*
 * creates the learn psychic power menu
 */
static MENU * learn_psy_power_menu_create(const CHARACTER *character)
{
	LIST *items;
	MENU *menu;
	PSY_POWER power;

	items = list_new();

	iterate_over_psy_powers(power) {

		if (!character_has_psy_power(character, power)
			&& psy_power_implemented(power)) {

			list_add(items, (void *)psy_power_name(power));
		}
	}

	if (items->n_nodes == 0) {

		list_free(items);
		return NULL;
	}

	menu = menu_create(items);

	return menu;
}






/*
 * the follow career path screen
 */
static void follow_career_path(CHARACTER *character)
{
	MENU *menu;

	while (true) {
		CAREER_INDEX career_index;
		const char *career_name;
		CAREER *career;

		menu = follow_career_path_menu_create(character);

		if (!career_completed(character) || menu == NULL ||
			character->ep <
			EP_COST_CHANGE_CAREER_FOLLOW_PATH) {

			command_bar_set(1, CM_EXIT);

			render_follow_career_path_screen(character, menu);
			update_screen();

			command_bar_get_command();

			break;
		}

		command_bar_set(5, CM_UP, CM_DOWN, CM_OK, CM_INFO, CM_EXIT);

		render_follow_career_path_screen(character, menu);
		update_screen();

		follow_career_path_screen_menu(menu);

		if (menu->entered_command == CM_EXIT) {

			break;
		}

		career_name = (const char *)list_data_at(
			menu->items, menu->highlighted
		);
		career_index = name_to_career_index(career_name);
		career = get_career_pointer(career_index);

		if (menu->entered_command == CM_INFO) {

			career_screen(career);

		} else if (menu->entered_command == CM_OK) {

			character->ep -= EP_COST_CHANGE_CAREER_FOLLOW_PATH;

			career_set(character, career_index);

			message(SID_CHANGE_CAREER_FOLLOW_PATH,
				"Career changed!"
			);

			break;
		}

		menu_free(menu);
	}

	if (menu != NULL) {

		menu_free(menu);
	}
}



/*
 * creates the follow career path menu
 */
static MENU * follow_career_path_menu_create(const CHARACTER *character)
{
	LIST *items = list_new();
	MENU *menu;
	CAREER_INDEX i;

	for (i = 0; i < MAX_CAREER_EXITS; i++) {

		if (is_empty_string(character->career->exit[i])) break;

		list_add(items, character->career->exit[i]);
	}

	if (items->n_nodes == 0) {

		list_free(items);
		return NULL;
	}

	menu = menu_create(items);

	return menu;
}



/*
 * creates the new career path menu
 */
static MENU * new_career_path_menu_create(void)
{
	N_CAREERS n_careers = get_n_careers();
	LIST *items = list_new();
	CAREER_INDEX i;
	MENU *menu;

	for (i = 0; i < n_careers; i++) {
		CAREER *career = get_career_pointer(i);

		if (career->type != CAREER_TYPE_BASIC) continue;

		list_add(items, (void *)career->name);
	}

	menu = menu_create(items);

	return menu;
}



/*
 * gives the passed character all perks of his career
 */
static void give_career_perks(CHARACTER *character)
{
	PERK perk;

	/* iterate over all perks */
	iterate_over_perks(perk) {

		/*
		 * the character's career has the perk AND
		 * the character does NOT have the perk ..
		 */
		if (career_has_perk(character->career, perk) &&
			!character_has_perk(character, perk)) {

			/* give perk to character */
			give_perk(character, perk);

			/* free psychic power for novice psykers */
			if (perk == PK_PSYCHIC_I) {

				learn_psy_power(character, true);
			}
		}
	}
}



/*
 * returns a missing required perk (required for buying the passed perk)
 * or PK_NIL if no required perk is missing
 */
static PERK missing_required_perk(const CHARACTER *character, PERK perk)
{

	if (perk == PK_PSYCHIC_III &&
		!character_has_perk(character, PK_PSYCHIC_II)) {

		return PK_PSYCHIC_II;
	}

	if (perk == PK_PSYCHIC_II &&
		!character_has_perk(character, PK_PSYCHIC_I)) {

		return PK_PSYCHIC_I;
	}

	return PK_NIL;
}









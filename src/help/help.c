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
 * Module: Help
 */

#include "wrogue.h"



static void		additional_information_screen(void);

static void		career_list(const char *);
static void		perk_list(const char *);

static void		character_attribute_list(const char *);
static void		object_attribute_list(const char *);
static void		terrain_attribute_list(const char *);
static void		psy_power_list(const char *);



/*
 * help sections
 */
static const HELP_SECTION	HelpSection[MAX_HELP_SECTIONS] = {

 {"Stats",		{HELP_STATS,		NULL}},
 {"Advancement",	{HELP_ADVANCEMENT,	NULL}},
 {"Careers",		{NULL,			HELP_CAREERS}},
 {"Perks",		{NULL,			HELP_PERKS}},
 {"Timing",		{HELP_TIMING,		NULL}},
 {"Movement",		{HELP_MOVEMENT,		NULL}},
 {"Morale",		{HELP_MORALE,		NULL}},
 {"Combat",		{HELP_COMBAT,		NULL}},
 {"Evocation",		{HELP_EVOCATION,	NULL}},
 {"Psychic Powers",	{NULL,			HELP_PSYCHIC_POWERS}},
 {"Parties",		{HELP_PARTIES,		NULL}},
 {"Tactical Commands",	{HELP_TACTICS,		NULL}},
 {"Character Attributes",{NULL,			HELP_CHARACTER_ATTRIBUTES}},
 {"Object Attributes",	{NULL,			HELP_OBJECT_ATTRIBUTES}},
 {"Terrain Attributes",	{NULL,			HELP_TERRAIN_ATTRIBUTES}},
 {"Macros",		{HELP_MACROS,		NULL}}

};



/*
 * the help screen
 */
void help_screen(void)
{

	command_bar_set(2, CM_ADDITIONAL_INFORMATION, CM_EXIT);

	render_help_screen();

	update_screen();

	if (command_bar_get_command() == CM_EXIT) {

		return;
	}

	additional_information_screen();
}



/*
 * the additional information screen
 */
static void additional_information_screen(void)
{
	LIST *items;
	MENU *menu;
	FILE_COUNTER i;

	items = list_new();

	for (i = 0; i < MAX_HELP_SECTIONS; i++) {

		list_add(items, HelpSection[i].title);
	}

	menu = menu_create(items);

	while (true) {
		const HELP_SECTION *help;

		command_bar_set(4, CM_UP, CM_DOWN, CM_OK, CM_EXIT);

		render_additional_information_screen();

		additional_information_menu(menu);

		if (menu->entered_command == CM_EXIT) {

			break;
		}

		help = &HelpSection[menu->highlighted];

		if (help->link.function != NULL) {

			(*help->link.function)(help->title);

			continue;

		} else if (help->link.file_name != NULL) {

			show_text_file(help->title, DIR_HELP,
				help->link.file_name,
				false
			);
		}
	}

	menu_free(menu);
}



/*
 * shows a list of all careers
 */
static void career_list(const char *title)
{
	MENU *menu;
	LIST *items;
	N_CAREERS n_careers;
	CAREER_INDEX i;

	n_careers = get_n_careers();
	items = list_new();

	for (i = 0; i < n_careers; i++) {
		CAREER *career;

		career = get_career_pointer(i);

		list_add(items, (void *)career->name);
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			CAREER_INDEX career_index;
			const char *career_name;

			career_name = (const char *)list_data_at(menu->items,
				menu->highlighted
			);

			career_index = name_to_career_index(career_name);

			career_screen(get_career_pointer(career_index));
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



/*
 * shows a list of all perks
 */
static void perk_list(const char *title)
{
	MENU *menu;
	LIST *items;
	PERK perk;

	items = list_new();

	iterate_over_perks(perk) {

		list_add(items, (void *)perk_name(perk));
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			const char *name = (const char *)
				list_data_at(menu->items, menu->highlighted
			);

			perk = name_to_perk(name);

			perk_screen(perk);
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



/*
 * shows a list of all character attributes
 */
static void character_attribute_list(const char *title)
{
	MENU *menu;
	LIST *items;
	CHARACTER_ATTRIBUTE i;

	items = list_new();

	for (i = 0; i < MAX_CHARACTER_ATTRIBUTES; i++) {

		list_add(items, (void *)character_attribute_name(i));
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			const char *name;
			CHARACTER_ATTRIBUTE attribute;

			name = (const char *)list_data_at(menu->items,
				menu->highlighted
			);

			attribute = name_to_character_attribute(name);

			character_attribute_screen(attribute);
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



/*
 * shows a list of all object attributes
 */
static void object_attribute_list(const char *title)
{
	MENU *menu;
	LIST *items;
	OBJECT_ATTRIBUTE i;

	items = list_new();

	for (i = 0; i < MAX_OBJECT_ATTRIBUTES; i++) {

		list_add(items, (void *)object_attribute_name(i));
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			const char *name;
			OBJECT_ATTRIBUTE attribute;

			name = (const char *)list_data_at(menu->items,
				menu->highlighted
			);

			attribute = name_to_object_attribute(name);

			object_attribute_screen(attribute);
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



/*
 * shows a list of all terrain attributes
 */
static void terrain_attribute_list(const char *title)
{
	MENU *menu;
	LIST *items;
	TERRAIN_ATTRIBUTE i;

	items = list_new();

	for (i = 0; i < MAX_TERRAIN_ATTRIBUTES; i++) {

		list_add(items, (void *)terrain_attribute_name(i));
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			const char *name;
			TERRAIN_ATTRIBUTE attribute;

			name = (const char *)list_data_at(menu->items,
				menu->highlighted
			);

			attribute = name_to_terrain_attribute(name);

			terrain_attribute_screen(attribute);
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



/*
 * shows a list of all psychic powers
 */
static void psy_power_list(const char *title)
{
	MENU *menu;
	LIST *items;
	PSY_POWER power;

	items = list_new();

	iterate_over_psy_powers(power) {

		if (!psy_power_implemented(power)) continue;

		list_add(items, (void *)psy_power_name(power));
	}

	menu = menu_create(items);

	do {

		command_bar_set(4, CM_UP, CM_DOWN, CM_INFO, CM_EXIT);

		render_entity_info_list_screen(title);
		update_screen();

		entity_info_list_menu(menu);

		if (menu->entered_command == CM_INFO) {
			const char *name = (const char *)
				list_data_at(menu->items, menu->highlighted);

			power = name_to_psy_power(name);

			psy_power_screen(power);
		}

	} while (menu->entered_command != CM_EXIT);

	menu_free(menu);
}



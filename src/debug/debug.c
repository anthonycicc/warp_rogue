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
 * Module: Debug
 */
 
#include "wrogue.h"





/*
 * the debug console
 */
void debug_console(void)
{
	KEY_CODE key;

	key = get_key();

	if (key == 'x') {

		give_ep(player_controlled_character(), 1000);

	} else if (key == 'm') {
		OBJECT *money;

		money = object_create(MONEY_OBJECT_NAME);
		money->charge = 10000;
		inventory_add(player_controlled_character(), money);

	} else if (key == 'o') {
		char object_name[OBJECT_NAME_SIZE];

		ui_get_object_name(object_name);

		inventory_add(player_controlled_character(),
			object_create(object_name)
		);

	} else if (key == 'q') {
		char name[QUEST_NAME_SIZE];
		char status[QUEST_STATUS_SIZE];
		
		ui_get_quest_name(name);
		ui_get_quest_status(status);
		
		quest_set_status(name, status);
	}
}




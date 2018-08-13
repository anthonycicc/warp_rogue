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
 * Module: Character Talk
 */

#include "wrogue.h"





/*
 * the dialogue state
 */
static DIALOGUE_STATE	DialogueState;




/*
 * starts a dialogue
 */
void dialogue_start(CHARACTER *active_character, 
	CHARACTER *passive_character
)
{

	*script_pointer_active_character() = 
		DialogueState.active_character = 
		active_character;
	
	*script_pointer_self() = 
		DialogueState.passive_character = 
		passive_character;

	DialogueState.options = NULL;

	clear_string(DialogueState.text);

	if (character_has_attribute(active_character, CA_CANNOT_SPEAK)) {

		message(SID_GAME, "You cannot speak.");
		return;
	}

	if (character_has_attribute(passive_character, CA_CANNOT_SPEAK) ||
		hostility_between(active_character, passive_character) ||
		!character_has_script(passive_character)) {

		message(SID_GAME, "No reply.");
		return;
	}

	script_load(DIR_CHARACTER_SCRIPTS, passive_character->script);
	script_set_data("TRIGGERED", SCRIPT_TRUE);
	script_set_data("SELF", SCRIPT_SELF_PTR);
	script_set_data("ACTIVE_CHARACTER", SCRIPT_ACTIVE_CHARACTER_PTR);
	script_execute();
}



/*
 * makes the passive character say something
 */
void dialogue_say(const char *fmt, ...)
{
	char str[STRING_BUFFER_SIZE];
	va_list vl;

	va_start(vl, fmt);
	vsprintf(str, fmt, vl);
	va_end(vl);

	strcat(DialogueState.text, str);
}



/*
 * adds a dialogue option
 */
void dialogue_add_option(const char *str)
{

	if (DialogueState.options == NULL) {

		DialogueState.options = list_new();
	}

	list_add(DialogueState.options, (char *)str);
}



/*
 * lets the active character choose a dialogue option
 */
const char * dialogue_choice(void)
{
	const char *chosen_option;

	DialogueState.menu = menu_create(DialogueState.options);

	command_bar_set(3, CM_UP, CM_DOWN, CM_OK);
	render_dialogue_screen(&DialogueState);
	update_screen();

	dialogue_screen_menu(DialogueState.menu);

	chosen_option = (const char *)list_data_at(DialogueState.menu->items,
		DialogueState.menu->highlighted);

	menu_free(DialogueState.menu);

	DialogueState.options = NULL;

	clear_string(DialogueState.text);

	return chosen_option;
}






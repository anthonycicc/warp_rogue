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
 * Module: Macro
 */

#include "wrogue.h"




/*
 * the array that is used to save the macros
 */
static KEY_CODE		Macro[MAX_MACROS][MAX_MACRO_LENGTH];


static MACRO_INDEX	command_to_macro(COMMAND);

static void		clear_macro(MACRO_INDEX);





/*
 * Macro module init
 */
void macro_init(void)
{
	MACRO_INDEX i;

	for (i = 0; i < MAX_MACROS; i++) {

		clear_macro(i);
	}
}



/*
 * Macro module clean up
 */
void macro_clean_up(void)
{

	/* NOTHING TO DO */
}



/*
 * sets a macro
 */
void set_macro(MACRO_INDEX macro_index, const KEY_CODE *macro)
{

	memcpy(Macro[macro_index], macro, MAX_KEY_STACK * sizeof *macro);
}



/*
 * returns a macro
 */
KEY_CODE * get_macro(MACRO_INDEX macro_index)
{

	return Macro[macro_index];
}



/*
 * returns true if the macro referred to by macro_command is defined
 */
bool macro_defined(COMMAND macro_command)
{
    MACRO_INDEX macro_index;

    macro_index = command_to_macro(macro_command);

    if (Macro[macro_index][0] == MACRO_TERMINATION_KEY) {

	    return false;
    }

    return true;
}



/*
 * executes a macro
 */
COMMAND do_macro(COMMAND macro_command)
{
	MACRO_INDEX macro_index = command_to_macro(macro_command);
	KEY_CODE *k;

	k = Macro[macro_index];
	
	while (*k != MACRO_TERMINATION_KEY) {

		key_stack_push(*k);

		++k;
	}

	return command_bar_command(get_key());
}



/*
 * the macro setup screen
 */
void macro_setup_screen(void)
{
	COMMAND entered_command = CM_NIL;
	MACRO_INDEX active_macro_index = -1;

	command_bar_set(13,
		CM_MACRO_1, CM_MACRO_2, CM_MACRO_3, CM_MACRO_4,
		CM_MACRO_5, CM_MACRO_6, CM_MACRO_7, CM_MACRO_8,
		CM_MACRO_9, CM_MACRO_10, CM_MACRO_11, CM_MACRO_12,
	CM_EXIT);

	while (true) {

		render_macro_setup_screen(entered_command, active_macro_index);
		update_screen();

		entered_command = command_bar_get_command();

		if (entered_command == CM_EXIT) return;

		active_macro_index = command_to_macro(entered_command);

		clear_macro(active_macro_index);

		render_macro_setup_screen(entered_command, active_macro_index);
		update_screen();

		macro_input(active_macro_index);

		active_macro_index = -1;
	}
}



/*
 * returns the index of the macro linked to the passed command
 */
static MACRO_INDEX command_to_macro(COMMAND macro_command)
{

	return macro_command - CM_MACRO_1;
}



/*
 * clears a macro
 */
static void clear_macro(MACRO_INDEX macro_index)
{

	Macro[macro_index][0] = MACRO_TERMINATION_KEY;
}



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
 * Module: UI Command Bar
 */

#include "wrogue.h"



static int		calculate_command_bar_length(void);



/*
 * the game screen command bar
 */
static COMMAND		GameScreenCommandBar[MAX_COMMANDS] = {

	CM_UP,
	CM_DOWN,
	CM_LEFT,
	CM_RIGHT,
	CM_UP_LEFT,
	CM_UP_RIGHT,
	CM_DOWN_LEFT,
	CM_DOWN_RIGHT,
	CM_CURRENT_TILE,
	CM_WAIT,
	CM_ATTACK,
	CM_TRIGGER,
	CM_LOOK,
	CM_RELOAD_WEAPON,
	CM_UNJAM_WEAPON,
	CM_SWITCH_WEAPONS,
	CM_RUN,
	CM_USE_PERK,
	CM_EVOKE_PSY_POWER,
	CM_TACTICAL_COMMAND,
	CM_INVENTORY_SCREEN,
	CM_CHARACTER_SCREEN,
	CM_JOURNAL,
	CM_GAME_CONTROLS,
	CM_HELP,
	CM_DEBUG,
	CM_MACRO_1,
	CM_MACRO_2,
	CM_MACRO_3,
	CM_MACRO_4,
	CM_MACRO_5,
	CM_MACRO_6,
	CM_MACRO_7,
	CM_MACRO_8,
	CM_MACRO_9,
	CM_MACRO_10,
	CM_MACRO_11,
	CM_MACRO_12,
	CM_CHARACTER_1,
	CM_CHARACTER_2,
	CM_CHARACTER_3,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL,
	CM_NIL

};



/*
 * a pointer to the current command bar
 */
static COMMAND *	CommandBar;

/*
 * the length of the current command bar (in characters)
 */
static int 		CommandBarLength;

/*
 * an array used to temporally store custom commmand bars
 */
static COMMAND 		CustomCommandBar[MAX_COMMANDS];




/*
 * clears the current command bar
 */
void command_bar_clear(void)
{
	COMMAND command;

	CommandBar = CustomCommandBar;

	iterate_over_commands(command) {

		CommandBar[command] = CM_NIL;
	}

	CommandBarLength = calculate_command_bar_length();
}


/*
 * sets the current command bar
 */
void command_bar_set(N_COMMANDS n, ...)
{
	va_list arg_ptr;
	COMMAND command;

	CommandBar = CustomCommandBar;

	va_start(arg_ptr, n);

	iterate_over_commands(command) {

		if (command < n) {

			CommandBar[command] = va_arg(arg_ptr, COMMAND);

		} else {

			CommandBar[command] = CM_NIL;
		}
	}

	va_end(arg_ptr);

	CommandBarLength = calculate_command_bar_length();
}



/*
 * adds commands to the current command bar
 */
void command_bar_add(N_COMMANDS n, ...)
{
	va_list arg_ptr;
	COMMAND i;
	int max_index;

	CommandBar = CustomCommandBar;

	for (i = 0; CommandBar[i] != CM_NIL; i++) {
		/* EMPTY LOOP */;
	}
	max_index = i - 1;

	max_index += n;
	if (max_index >= MAX_COMMANDS) {

		die("*** CORE ERROR *** invalid command_bar_add() call");
	}

	va_start(arg_ptr, n);

	for (; i <= max_index; i++) {

		CommandBar[i] = va_arg(arg_ptr, COMMAND);
	}
	if (i < MAX_COMMANDS) CommandBar[i] = CM_NIL;

	va_end(arg_ptr);

	CommandBarLength = calculate_command_bar_length();
}



/*
 * adds the basic movement commands to the current command bar
 */
void command_bar_add_move_commands(void)
{

	command_bar_add(8, CM_UP, CM_DOWN, CM_LEFT, CM_RIGHT,
		CM_UP_LEFT, CM_UP_RIGHT, CM_DOWN_LEFT, CM_DOWN_RIGHT);
}



/*
 * returns the command which corresponds to the pressed key
 * or CM_NIL if the pressed key doesn't correspond to any command
 * in the current command bar
 */
COMMAND command_bar_command(KEY_CODE pressed_key)
{
	COMMAND i, command;

	iterate_over_commands(i) {

		command = CommandBar[i];

		if (command == CM_NIL) break;

		/* hack */
		if (command == CM_UP && pressed_key == KEY_UP) break;
		if (command == CM_DOWN && pressed_key == KEY_DOWN) break;
		if (command == CM_LEFT && pressed_key == KEY_LEFT) break;
		if (command == CM_RIGHT && pressed_key == KEY_RIGHT) break;

		if (command_key(command) == pressed_key) break;


	}

	return command;
}



/*
 * lets the user enter a command
 * only commands which are in the current command bar are accepted
 */
COMMAND command_bar_get_command(void)
{
	KEY_CODE pressed_key;
	COMMAND command;

	do {
		pressed_key = get_key();

		command = command_bar_command(pressed_key);

	} while (command == CM_NIL);

	return command;
}



/*
 * returns the nth item in the command bar
 */
COMMAND command_bar_item(int nth)
{

	return CommandBar[nth];
}



/*
 * makes the main screen commmand bar the current command bar
 */
void use_game_screen_command_bar(void)
{

	CommandBar = GameScreenCommandBar;
}



/*
 * returns the nth item in the game screen command bar
 */
COMMAND game_screen_command(int nth)
{

	return GameScreenCommandBar[nth];
}



/*
 * calculates the length of the command bar (in characters) and returns the result
 */
static int calculate_command_bar_length(void)
{
	COMMAND i, command;
	const char *p;
	int length = 0;

	iterate_over_commands(i) {
		KEY_CODE key;

		command = CommandBar[i];

		if (command == CM_NIL) break;

		/* movement commands aren't printed */
		if (is_move_command(command)) continue;

		if (command == CM_CURRENT_TILE) continue;

		key = command_key(command);

		length += strlen(key_name(key)) + 2;

		for (p = command_name(command); *p != '\0'; ++p) {

			if (*p == '@') {

				++p;
				++p;

				continue;
			}

			++length;
		}

		++length;
	}

	return length;
}


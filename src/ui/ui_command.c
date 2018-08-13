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
 * Module: UI Command
 */

#include "wrogue.h"



static void	render_macro_definitions(SCREEN_COORD);




/*
 * command data
 */
static COMMAND_DATA CommandData[MAX_COMMANDS] = {

	{"Up",				KEY_NIL},
	{"Down",			KEY_NIL},
	{"Left",			KEY_NIL},
	{"Right",			KEY_NIL},
	{"Up left",			KEY_NIL},
	{"Up right",			KEY_NIL},
	{"Down left",			KEY_NIL},
	{"Down right",			KEY_NIL},

	{"Ok",				KEY_NIL},
	{"Exit",			KEY_NIL},

	{"Drop",			KEY_NIL},
	{"Info",			KEY_NIL},
	{"Use/Abort use",		KEY_NIL},
	{"Reroll",			KEY_NIL},

	{"Advancement",			KEY_NIL},
	{"Psychic powers",		KEY_NIL},
	{"Nearest enemy",		KEY_NIL},
	{"Advanced target mode",	KEY_NIL},

	{"Attack",			KEY_NIL},
	{"Trigger",			KEY_NIL},
	{"Inventory screen",		KEY_NIL},
	{"Evoke psychic power",		KEY_NIL},
	{"Tactical command",		KEY_NIL},
	{"Character screen",		KEY_NIL},
	{"Look",			KEY_NIL},
	{"Reload weapon",		KEY_NIL},
	{"Unjam weapon",		KEY_NIL},
	{"Switch weapons",		KEY_NIL},
	{"Run",				KEY_NIL},
	{"Use perk",			KEY_NIL},
	{"Journal",			KEY_NIL},
	{"Game controls",		KEY_NIL},
	{"Help",			KEY_NIL},
	{"Recover/Wait",		KEY_NIL},
	{"Macro 1",			KEY_NIL},
	{"Macro 2",			KEY_NIL},
	{"Macro 3",			KEY_NIL},
	{"Macro 4",			KEY_NIL},
	{"Macro 5",			KEY_NIL},
	{"Macro 6",			KEY_NIL},
	{"Macro 7",			KEY_NIL},
	{"Macro 8",			KEY_NIL},
	{"Macro 9",			KEY_NIL},
	{"Macro 10",			KEY_NIL},
	{"Macro 11",			KEY_NIL},
	{"Macro 12",			KEY_NIL},
	{"List",			KEY_NIL},
	{"Current tile",		KEY_NIL},
	{"Equipment",			KEY_NIL},
	{"S",				KEY_NIL},
	{"SA",				KEY_NIL},
	{"A",				KEY_NIL},
	{"Debug",			KEY_NIL},
	{"Rename",			KEY_NIL},
	{"Career",			KEY_NIL},
	{"Character 1",			KEY_NIL},
	{"Character 2",			KEY_NIL},
	{"Character 3",			KEY_NIL},
	{"Page up",			KEY_NIL},
	{"Page down",			KEY_NIL},
	{"Additional information",	KEY_NIL},
	{"Buy",				KEY_NIL},
	{"Manufacture",			KEY_NIL}

};





/*
 * returns the key associated with the passed command
 */
KEY_CODE command_key(COMMAND command)
{

	return CommandData[command].key_code;
}



/*
 * sets the key associated with passed command
 */
void set_command_key(COMMAND command, KEY_CODE key)
{

	CommandData[command].key_code = key;
}



/*
 * returns true if the entered command is a movement command or false if it's not
 */
bool is_move_command(COMMAND entered_command)
{
	bool move_cmd;

	switch (entered_command) {
	case CM_UP:
	case CM_DOWN:
	case CM_LEFT:
	case CM_RIGHT:
	case CM_UP_LEFT:
	case CM_UP_RIGHT:
	case CM_DOWN_LEFT:
	case CM_DOWN_RIGHT:
		move_cmd = true;
		break;
	default: move_cmd = false;
	}

	return move_cmd;

}



/*
 * returns true if a command is a macro command or false
 * if it isn't
 */
bool is_macro_command(COMMAND command)
{

	if (command >= CM_MACRO_1 && command <= CM_MACRO_12) {

		return true;
	}

	return false;
}



/*
 * move command -> direction
 */
bool move_command_to_direction(DIRECTION *direction, COMMAND entered_command)
{

	switch (entered_command) {
	case CM_UP:		*direction = DIRECTION_UP; break;
	case CM_DOWN:		*direction = DIRECTION_DOWN; break;
	case CM_LEFT:		*direction = DIRECTION_LEFT; break;
	case CM_RIGHT:		*direction = DIRECTION_RIGHT; break;
	case CM_UP_LEFT:	*direction = DIRECTION_UP_LEFT; break;
	case CM_UP_RIGHT:	*direction = DIRECTION_UP_RIGHT; break;
	case CM_DOWN_LEFT:	*direction = DIRECTION_DOWN_LEFT; break;
	case CM_DOWN_RIGHT:	*direction = DIRECTION_DOWN_RIGHT; break;
	default: return false;
	}

	return true;
}



/*
 * name -> command
 */
COMMAND name_to_command(const char *name)
{
	COMMAND command;

	iterate_over_commands(command) {

		if (strings_equal(name, CommandData[command].name)) {

			return command;
		}
	}

	die("*** CORE ERROR *** invalid command: %s", name);

	/* never reached */
	return CM_NIL;
}



/*
 * returns the name of the passed command
 */
const char * command_name(COMMAND command)
{

	return CommandData[command].name;
}



/*
 * returns true if the passed command is supposed to show up
 * on the help screen
 */
bool show_on_help_screen(COMMAND command)
{

	if (is_move_command(command) ||
		is_macro_command(command) ||
		command == CM_CURRENT_TILE ||
		command == CM_DEBUG ||
		command == CM_CHARACTER_1 ||
		command == CM_CHARACTER_2 ||
		command == CM_CHARACTER_3 ||
		command == CM_HELP) {

		return false;
	}

	return true;
}



/*
 * renders the pick target screen
 */
void render_pick_target_screen(const char *prompt)
{

	render_game_screen();

	render_overlay_screen_prompt(prompt);
}



/*
 * renders the advanced target mode screen
 */
void render_advanced_target_mode_screen(const CHARACTER *character,
	AREA_POINT *cursor, TARGET_MODE target_mode
)
{
	char sector_info[STRING_BUFFER_SIZE];
	char range_info[STRING_BUFFER_SIZE];
	SCREEN_COORD wx, wy;
	SECTOR *sector;

	update_view_point(cursor);

	vaw_convert_coordinates(&wy, &wx, cursor->y, cursor->x);

	sector = sector_at(cursor);

	render_game_screen();

	if (target_mode == TMODE_SHOOT || target_mode == TMODE_PSY_POWER) {
		AREA_DISTANCE distance;

		distance = area_distance(&character->location, cursor);

		render_overlay_screen_prompt("Target: %s (%s)",
			sector_description(sector_info, sector),
			range_description(range_info, distance)
		);

	} else {

		render_overlay_screen_prompt("Target: %s",
			sector_description(sector_info, sector)
		);
	}

	place_cursor_at(wy, wx);

	if (target_mode == TMODE_SHOOT) {

		render_cursor(C_AggressiveCursor);

	} else if (target_mode == TMODE_PSY_POWER) {

		render_cursor(C_PsychicCursor);

	} else {

		render_cursor(C_Cursor);
	}
}



/*
 * renders a view screen
 */
void render_view_screen(const char *name, AREA_POINT *cursor,
	COLOUR cursor_colour
)
{
	char description[STRING_BUFFER_SIZE];
	SCREEN_COORD wx, wy;
	SECTOR *sector;

	update_view_point(cursor);

	vaw_convert_coordinates(&wy, &wx, cursor->y, cursor->x);

	sector = sector_at(cursor);

	render_game_screen();

	render_overlay_screen_prompt("%s: %s",
		name,
		sector_description(description, sector)
	);

	place_cursor_at(wy, wx);
	render_cursor(cursor_colour);
}



/*
 * renders the choose firing mode screen
 */
void render_choose_firing_mode_screen(void)
{

	render_overlay_screen_prompt("Attack - Which firing mode?");
}



/*
 * renders the number of shots screen
 */
void render_number_of_shots_screen(const CHARACTER *character)
{

	render_overlay_screen_prompt("Attack - Number of shots (%d - %d)?",
		character->weapon->firing_mode.min_sa_shots,
		character->weapon->firing_mode.max_sa_shots
	);
}



/*
 * renders the spread factor screen
 */
void render_spread_factor_screen(void)
{

	render_overlay_screen_prompt("Attack - Spread factor (%d - %d)?",
		A_MODE_SPREAD_MIN,
		A_MODE_SPREAD_MAX
	);
}




/*
 * renders the use elevator screen
 */
void render_use_elevator_screen(void)
{

	message_clear(SID_GAME);

	render_game_screen();
}



/*
 * use elevator dialogue
 */
void use_elevator_dialogue(GET_TEXT_DIALOGUE *dialogue)
{

	get_text_dialogue_execute(dialogue, GAME_SCREEN_MESSAGE_ROW, GAME_SCREEN_MIN_X);
}



/*
 * renders the help screen
 */
void render_help_screen(void)
{
	SCREEN_COORD row, col;
	COMMAND i;

	render_secondary_screen("-== Help ==-");

	row = SEC_SCREEN_MIN_Y;
	col = SEC_SCREEN_MIN_X;

	render_text_at(C_Text, row, col,
		"To move your character or the cursor use the arrow keys, " \
		"the number pad, and/or '1' to '9'."
	);
	render_text_at(C_Text, ++row , col,
		"If a command prompts you for a direction, and "
		"you want to target yourself, or the tile "
	);
	render_text_at(C_Text, ++row, col,
		"you are currently standing on, press '5'."
	);

	render_text_at(C_Text, row += 2, col, "Available Commands");

	++row;
	iterate_over_commands(i) {
		COMMAND command;

		command = game_screen_command(i);

		if (command == CM_NIL) break;

		if (!show_on_help_screen(command)) continue;

		place_cursor_at(++row, col);

		render_hot_key(command_key(command));

		render_text(C_Text, command_name(command));

		if (command == CM_TRIGGER) {

			render_text(C_Text, " (pick up, talk, use)");

		} else if (command == CM_GAME_CONTROLS) {

			render_text(C_Text, " (save, quit, options, "
				"macros, certificate)"
			);

		} else if (command == CM_CHARACTER_SCREEN) {

			render_text(C_Text, " (information, advancement, "
				"rename)"
			);
		}
	}



	render_text_at(C_Text, row += 2, col,
		"HINT: Use macros for long command sequences. "
		"The macro system is very easy to use, try it!"
	);

	render_text_at(
		C_Text,
		SEC_SCREEN_MAX_Y - 1,
		SEC_SCREEN_MIN_X,
		"For detailed information about the combat system etc. open " \
		"the additional information screen."
	);

}



/*
 * renders the game controls screen
 */
void render_game_controls_screen(void)
{

	render_secondary_screen("-== Game Controls ==-");
}



/*
 * game controls screen menu
 */
COMMAND game_controls_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y, MENU_AUTO,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the macro setup screen
 */
void render_macro_setup_screen(COMMAND entered_command,
	MACRO_INDEX active_macro_index
)
{
	SCREEN_COORD row, col;

	render_secondary_screen("-== Macro Setup ==-");

	row = SEC_SCREEN_MIN_Y + 2;
	col = SEC_SCREEN_MIN_X;

	render_text_at(C_Text, row, col,
		"Press the macro key which you want to define.."
	);

	render_text_at(C_Text, row += 4, col,
		"Macro Key:           Macro Definition:"
	);

	render_macro_definitions(row += 2);

	if (active_macro_index == -1) return;

	place_cursor_at(row + active_macro_index, col);
	render_key_name(C_Highlight, command_key(entered_command));

	render_text_at(C_Text, SEC_SCREEN_MIN_Y + 3, col,
		"Now type the macro.. (%c terminates definition)",
		MACRO_TERMINATION_KEY
	);
}



/*
 * renders the inventory screen
 */
void render_inventory_screen(const CHARACTER *character,
	int highlighted_object
)
{
	LIST_NODE *node;
	OBJECT *object;
	int i;
	SCREEN_COORD y, x;

	render_secondary_screen(
		"-== Inventory (Encumbrance: %d/%d) ==-",
		inventory_weight(character),
		ENCUMBRANCE_MAX(character)
	);

	/* empty inventory */
	if (character->inventory->head == NULL) return;

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;
	for (node = character->inventory->head, i = 0;
		node != NULL;
		node = node->next, i++, y++) {

		object = (OBJECT *)node->data;

		place_cursor_at(y, x);

		if (i == highlighted_object) {

			render_inventory_object(C_Highlight,
				character, object, true
			);

		} else {

			render_inventory_object(C_Text,
				character, object, true
			);
		}
	}
}



/*
 * attack confirmation dialogue
 */
void attack_confirmation_dialogue(CONFIRMATION_DIALOGUE *dialogue)
{

	message_clear(SID_GAME);

	render_game_screen();

	dialogue->position.y = GAME_SCREEN_MESSAGE_ROW;
	dialogue->position.x = GAME_SCREEN_MIN_X;
	dialogue->colour = C_Warning;

	confirmation_dialogue(dialogue);
}



/*
 * leave area confirmation dialogue
 */
void leave_area_dialogue(CONFIRMATION_DIALOGUE *dialogue)
{

	message_clear(SID_GAME);
	
	render_game_screen();

	dialogue->position.y = GAME_SCREEN_MESSAGE_ROW;
	dialogue->position.x = GAME_SCREEN_MIN_X;
	dialogue->colour = C_Text;

	confirmation_dialogue(dialogue);
}




/*
 * enter area confirmation dialogue
 */
void enter_area_dialogue(CONFIRMATION_DIALOGUE *dialogue)
{

	dialogue->position.y = GAME_SCREEN_MESSAGE_ROW;
	dialogue->position.x = GAME_SCREEN_MIN_X;
	dialogue->colour = C_Text;

	confirmation_dialogue(dialogue);
}



/*
 * renders the macro definitions
 */
static void render_macro_definitions(SCREEN_COORD start_y)
{
	int i, j;

	for (i = 0; i < MAX_MACROS; i++) {
		KEY_CODE *macro;
		KEY_CODE key;

		render_text_at(C_Text, start_y + i,
			SEC_SCREEN_MIN_X, "F%d", i + 1
		);

		j = 0;

		place_cursor_at(start_y + i, SEC_SCREEN_MIN_X + 21);

		macro = get_macro(i);

		while ((key = macro[j]) != MACRO_TERMINATION_KEY) {

			render_key_name(C_Text, key);

			++j;
		}
	}
}


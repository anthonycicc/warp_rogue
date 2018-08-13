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
 * Module: UI Input
 */

#include "wrogue.h"


static void	ui_gs_get_string(const char *, char *, int);

static char *	get_string(COLOUR, char *, int);



/*
 * The following variables belong to the key stack.
 * The key stack is needed by the macro function.
 */
static KEY_CODE	KeyStack[MAX_KEY_STACK]; /* FIFO stack for keys */
static int	KeyStackTop = -1; /* index of the uppermost stack item */
static int	KeyStackBottom = -1; /* index of the bottommost stack item */





/*
 * pushes a key on the key stack
 */
void key_stack_push(KEY_CODE key)
{
	++KeyStackTop;
	KeyStack[KeyStackTop] = key;
}



/*
 * pops a key from the key stack
 */
KEY_CODE key_stack_pop(void)
{

	++KeyStackBottom;
	if (KeyStackBottom > KeyStackTop) {
		KeyStackBottom = KeyStackTop = -1;
		return -1;
	}

	return KeyStack[KeyStackBottom];
}



/*
 * clears the key stack
 */
void key_stack_clear(void)
{
	KeyStackBottom = KeyStackTop = -1;
}


/*
 * returns true if the key stack is empty
 */
bool key_stack_is_empty(void)
{
	if (KeyStackBottom == KeyStackTop ||
		KeyStackBottom == -1) {

		return true;
	}

	return false;
}



/*
 * waits until the user has pressed one of the accepted keys
 */
KEY_CODE limited_get_key(const char *accepted_keys)
{
	KEY_CODE pressed_key;

	do {
		pressed_key = get_key();
	} while (strchr(accepted_keys, pressed_key) == NULL);

	return pressed_key;
}



/*
 * gets a key
 */
KEY_CODE get_key(void)
{
	KEY_CODE key;

	key = key_stack_pop();

	if (key == -1) {
		key = lowlevel_get_key();
	}

	return key;
}



/*
 * executes a get text dialogue
 */
char * get_text_dialogue_execute(GET_TEXT_DIALOGUE *dialogue, 
	SCREEN_COORD y,  SCREEN_COORD x
)
{
	clear_text_window(y, x, y, x + dialogue->max_length);

	place_cursor_at(y, x);

	if (!is_empty_string(dialogue->prompt)) {

		render_text(dialogue->colour, dialogue->prompt);
		update_screen();
	}

	get_string(C_Text, dialogue->entered_text, dialogue->max_length);

	return dialogue->entered_text;
}



/*
 * confirmation dialogue input routine
 */
void confirmation_dialogue_input(CONFIRMATION_DIALOGUE *dialogue)
{
	KEY_CODE pressed_key = limited_get_key("yn");

	if (pressed_key == 'y') {
		dialogue->confirmed = true;
	} else {
		dialogue->confirmed = false;
	}
}



/*
 * lets the user type a macro
 */
void macro_input(MACRO_INDEX macro_index)
{
	int i;
	KEY_CODE pressed_key;
	KEY_CODE *macro;
	SCREEN_COORD row, col;

	macro = get_macro(macro_index);

	row = SEC_SCREEN_MIN_Y + 8 + macro_index;
	col = SEC_SCREEN_MIN_X + 21;

	place_cursor_at(row, col);

	for (i = 0; i < MAX_KEY_STACK; i++) {

		pressed_key = get_key();

		macro[i] = pressed_key;

		if (pressed_key == MACRO_TERMINATION_KEY) break;

		render_key_name(C_Text, pressed_key);

		update_screen();
	}

}



/*
 * prompts the user to enter the name of an object
 */
void ui_get_object_name(char *object_name)
{

	ui_gs_get_string("Object name: ", object_name, OBJECT_NAME_SIZE);
}
	
	


/*
 * prompts the user to enter the name of a quest
 */
void ui_get_quest_name(char *quest_name)
{

	ui_gs_get_string("Quest name: ", quest_name, QUEST_NAME_SIZE);
}
	



/*
 * prompts the user to enter a quest status string
 */
void ui_get_quest_status(char *status_string)
{

	ui_gs_get_string("Quest status: ", status_string, 
		QUEST_STATUS_SIZE
	);
}
	



/*
 * prompts the user to enter a string
 * (game screen version)
 */
static void ui_gs_get_string(const char *prompt, char *string, 
	int string_size
)
{
	GET_TEXT_DIALOGUE dialogue;

	strcpy(dialogue.prompt, prompt);
	dialogue.colour = C_Text;
	dialogue.max_length = string_size - 1;
	
	command_bar_set(1, CM_OK);

	get_text_dialogue_execute(&dialogue, 
		GAME_SCREEN_MIN_Y, GAME_SCREEN_MIN_X
	);

	strcpy(string, dialogue.entered_text);
}
	


/*
 * gets a string
 */
static char * get_string(COLOUR colour, char *buf, int n)
{
	KEY_CODE pressed_key;
	int i;

	i = 0;

	while (true) {

		render_cursor(C_Highlight);
		update_screen();

		pressed_key = lowlevel_get_key();

		if (pressed_key == KEY_ENTER) {

			render_char(colour, ' ');
			break;

		} else if (pressed_key == KEY_BKSP ||
			pressed_key == KEY_DELETE) {

			if (i == 0) continue;

			--i;

			place_cursor_at(cursor_y(), cursor_x() - 1);
			render_char(colour, ' ');
			render_char(colour, ' ');
			place_cursor_at(cursor_y(), cursor_x() - 2);
			continue;
		}

		if (i >= n) continue;

		render_char(colour, pressed_key);

		buf[i] = (char)pressed_key;
		++i;

	}

	buf[i] = '\0';

	return buf;

}




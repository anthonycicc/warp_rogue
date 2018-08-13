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
 * Module: UI
 */

#include "wrogue.h"



static SCREEN_AREA *	get_command_bar_window(SCREEN_ID, SCREEN_AREA *);
static int		show_text_page(const char *, int);



/*
 * renders a header
 */
void render_header(const char *header)
{
	SCREEN_COORD x = cursor_x(), y = cursor_y();

	render_text(C_Header, header);

	place_cursor_at(++y, x);
}



/*
 * renders a field
 */
void render_field(const char *field_name, const char *fmt, ...)
{
	char field_value[STRING_BUFFER_SIZE];
	SCREEN_COORD x = cursor_x(), y = cursor_y();
	va_list vl;

	va_start(vl, fmt);
	vsprintf(field_value, fmt, vl);
	va_end(vl);

	render_text(C_FieldName, field_name);
	render_text(C_FieldValue, field_value);

	place_cursor_at(++y, x);
}



/*
 * renders a string centered at a specific row
 */
void render_text_centered(COLOUR colour, SCREEN_COORD y, const char *str)
{
	SCREEN_COORD x;

	x = (TEXT_COLUMNS / 2) - (strlen(str) / 2);
	render_text_at(colour, y, x, str);
}



/*
 * renders formatted text
 */
void render_text(COLOUR colour, const char *fmt, ...)
{
	va_list vl;
	char buffer[STRING_BUFFER_SIZE];
	const char *p;

	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);

	p = buffer;

	while (*p != '\0') {
		render_char(colour, *p);
		++p;
	}

}



/*
 * renders formatted text
 */
void render_text_at(COLOUR colour, SCREEN_COORD y, SCREEN_COORD x, const char *fmt, ...)
{
	va_list vl;
	char buffer[STRING_BUFFER_SIZE];
	const char *p;

	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);

	place_cursor_at(y, x);

	p = buffer;

	while (*p != '\0') {
		render_char(colour, *p);
		++p;
	}

}



/*
 * renders a string (with max. length limit)
 */
void render_lstring_at(COLOUR colour, SCREEN_COORD y, SCREEN_COORD x, const char *str, int max_len)
{
	int i;

	place_cursor_at(y, x);

	i = 0;
	while (i < max_len && str[i] != '\0') {
		render_char(colour, str[i]);
		++i;
	}

	if (i == max_len) {
		place_cursor_at(y, cursor_x() - 2);
		render_char(colour, '.');
		render_char(colour, '.');
	}
}



/*
 * renders long unformatted text; does automatic line breaks when
 * necessary
 */
SCREEN_COORD render_long_text(
	COLOUR colour, SCREEN_COORD start_y, SCREEN_COORD start_x,
	const char *text, SCREEN_COORD max_x
)
{
	char buffer[TEXT_BUFFER_SIZE];
	char *word;
	SCREEN_COORD y, x;
	int x_inc;

	strcpy(buffer, text);

	y = start_y;
	x = start_x;

	for (word = strtok(buffer, " ");
		word != NULL;
		word = strtok(NULL, " ")) {
		const char *letter;

		x_inc = strlen(word) + 1;

		if (x + x_inc - 1 > max_x) {
			++y;
			x = start_x;
		}

		place_cursor_at(y, x);
		letter = word;

		while (*letter != '\0') {

			render_char(colour, *letter);
			++letter;
		}

		render_char(colour, ' ');

		x += x_inc;
	}

	return y;
}



/*
 * dice roll -> string
 */
const char * dice_roll_string(const DICE_ROLL *roll)
{
	static char string[16];

	if (roll->modifier > 0) {

		sprintf(string, "%dD%d+%d", roll->n_dice, roll->n_sides,
			roll->modifier
		);

	} else if (roll->modifier < 0) {

		sprintf(string, "%dD%d%d", roll->n_dice, roll->n_sides,
			roll->modifier
		);

	} else {

		sprintf(string, "%dD%d", roll->n_dice, roll->n_sides);
	}

	return string;
}



/*
 * renders a key name
 */
void render_key_name(COLOUR colour, KEY_CODE key)
{

	render_text(colour, key_name(key));
}



/*
 * renders the stats of the passed character at the passed location
 */
void render_stats_at(const CHARACTER *character, SCREEN_COORD y,
	SCREEN_COORD x
)
{
	STAT stat;

	iterate_over_stats(stat) {
		STAT_VALUE stat_value;
		COLOUR colour;

		render_text_at(C_FieldName, y + stat, x, "%-13s: ",
			stat_long_name(stat)
		);

		stat_value = character->stat[stat].current;

		if (stat_value < 0) {

			stat_value = 0;
		}

		if (stat_value == character->stat[stat].total) {

			colour = C_FieldValue;

		} else if (stat_value < 
			character->stat[stat].total) {

			colour = C_FieldValueReduced;

		} else {

			colour = C_FieldValueIncreased;
		}

		render_text(colour, "%d", stat_value);
	}
}



/*
 * renders a secondary screen
 */
void render_secondary_screen(const char *fmt, ...)
{
	va_list vl;
	char screen_title[STRING_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(screen_title, fmt, vl);
	va_end(vl);

	render_background();

	clear_text_window(
		SEC_SCREEN_TITLE_ROW,
		SEC_SCREEN_MIN_X,
		SEC_SCREEN_TITLE_ROW,
		SEC_SCREEN_MAX_X
	);

	clear_text_window(
		SEC_SCREEN_MIN_Y,
		SEC_SCREEN_MIN_X,
		SEC_SCREEN_MAX_Y,
		SEC_SCREEN_MAX_X
	);

	clear_text_window(
		SEC_SCREEN_COMMAND_ROW,
		SEC_SCREEN_MIN_X,
		SEC_SCREEN_COMMAND_ROW,
		SEC_SCREEN_MAX_X
	);


	render_text_centered(C_SecondaryScreenTitle, SEC_SCREEN_TITLE_ROW,
		screen_title
	);

	render_command_bar(SID_SECONDARY);

	place_cursor_at(SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X);
}



/*
 * renders an overlay screen message
 */
void render_overlay_screen_message(const char *fmt, ...)
{
	va_list vl;
	char prompt[STRING_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(prompt, fmt, vl);
	va_end(vl);

	clear_text_window(
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MIN_X,
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MAX_X
	);

	render_text_at(C_Text, GAME_SCREEN_MESSAGE_ROW, GAME_SCREEN_MIN_X, prompt);
	render_char(C_Text, ' ');

	render_command_bar(SID_CURRENT_POSITION);

}



/*
 * renders an overlay screen prompt
 */
void render_overlay_screen_prompt(const char *fmt, ...)
{
	va_list vl;
	char prompt[STRING_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(prompt, fmt, vl);
	va_end(vl);

	clear_text_window(
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MIN_X,
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MAX_X
	);

	render_text_at(C_Text, GAME_SCREEN_MESSAGE_ROW, GAME_SCREEN_MIN_X, prompt);
	render_char(C_Text, ' ');

	render_command_bar(SID_CURRENT_POSITION);

}



/*
 * outputs a message
 */
void message(SCREEN_ID screen_id, const char *fmt, ...)
{
	va_list vl;
	char msg[STRING_BUFFER_SIZE];
	char *passed_msg;

	passed_msg = msg;

	if (fmt != NULL) {

		va_start(vl, fmt);
		vsprintf(msg, fmt, vl);
		va_end(vl);

	} else {

		passed_msg = NULL;
	}

	if (screen_id == SID_GAME) {

		game_screen_message(passed_msg);

	} else {

		command_bar_message(screen_id, passed_msg);
	}
}



/*
 * flushes the message buffer
 */
void message_flush(SCREEN_ID screen_id)
{

	if (screen_id == SID_GAME) {

		game_screen_message_flush();
	}
}



/*
 * clears the message buffer
 */
void message_clear(SCREEN_ID screen_id)
{

	if (screen_id != SID_GAME) {

		die("*** CORE ERROR *** invalid message_clear() call");
	}

	game_screen_message(NULL);
}



/*
 * outputs a command bar message
 */
void command_bar_message(SCREEN_ID screen_id, const char *msg)
{
	SCREEN_AREA rect;

	get_command_bar_window(screen_id, &rect);

	clear_text_window(rect.top, rect.left, rect.bottom, rect.right);

	render_text_at(C_Text, rect.top, rect.left, msg);

	update_screen();

	sec_sleep(1);

	render_command_bar(screen_id);

	update_screen();
}



/*
 * renders the status of the passed character
 */
void render_character_status(const CHARACTER *character,
	SCREEN_COORD y_max, bool show_all
)
{
	SCREEN_COORD y, x;
	CHARACTER_ATTRIBUTE i;

	y = cursor_y();
	x = cursor_x();

	if (character->injury > 0) {

		render_text_at(C_Injured, y, x, "Injured (%d/%d)",
			character->injury,
			injury_max(character)
		);

		if (++y > y_max) {

			return;
		}
	}

	for (i = 0; i < MAX_CHARACTER_ATTRIBUTES; i++) {
				
		if (!character_has_attribute(character, i)) {

			continue;
		}

		if (!show_all && 
			(
			i == CA_UNIQUE ||
			i == CA_NO_UNARMED_ATTACK
			)
		) continue;
			
		render_text_at(character_attribute_colour(i), y, x,
			character_attribute_name(i)
		);

		if (++y > y_max) {

			break;
		}
	}
}



/*
 * renders an inventory object
 */
void render_inventory_object(COLOUR colour, const CHARACTER *character,
	const OBJECT *object, bool show_full_info
)
{

	render_text(colour, object->name);

	if (show_full_info && object->charge != CHARGE_NIL) {

		render_text(colour, " [%d]", object->charge);
	}

	if (object == character->weapon) {

		render_text(colour, " (weapon)");

	} else if (object == character->secondary_weapon) {

		render_text(colour, " (secondary weapon)");

	} else if (object == character->armour) {

		render_text(colour, " (armour)");

	} else if (object == character->jump_pack) {

		render_text(colour, " (jump pack)");

	}	
}


/*
 * clears a screen area
 */
void clear_screen_window(SCREEN_COORD y1, SCREEN_COORD x1,
	SCREEN_COORD y2, SCREEN_COORD x2
)
{
	SCREEN_COORD i, j;
	
	for (i = y1; i <= y2; i++) {
	
		for (j = x1; j <= x2; j++) {

			render_char_at(0, i, j, ' ');
		}			
	}
}





/*
 * renders a menu
 */
void render_menu(MENU *menu, SCREEN_AREA *menu_area)
{
	SCREEN_COORD x2;
	int i;

	x2 = menu_area->right;

	for (i = menu->first_visible; i <= menu->last_visible; i++) {
		SCREEN_COORD y, x;
		COLOUR colour;
		const char *entry;

		if (i == menu->highlighted) {

			colour = C_Highlight;

		} else {

			colour = C_Text;
		}

		entry = list_data_at(menu->items, i);

		y = menu_area->top + i - menu->first_visible;
		x = menu_area->left;

		clear_screen_window(y, x, y, x2);

		place_cursor_at(y, x);

		render_text(colour, entry);
	}


	/* erase old scroll symbols */
	if (menu->first_visible > 0 || menu->last_visible < menu->last) {

		render_char_at(C_Text, menu_area->top,
			x2, ' '
		);

		render_char_at(C_Text, menu_area->bottom,
			x2, ' '
		);
	}

	/* draw new scroll symbols if necessary */
	if (menu->first_visible > 0) {

		render_char_at(C_HotKey, menu_area->top,
			x2, '+'
		);
	}

	if (menu->last_visible < menu->last) {

		render_char_at(C_HotKey, menu_area->bottom,
			x2, '+'
		);
	}
}


/*
 * renders a hot keys
 */
void render_hot_key(KEY_CODE key)
{	
	render_char(C_HotKeyBracket, '[');
	render_key_name(C_HotKey, key);
	render_char(C_HotKeyBracket, ']');
}



/*
 * renders the command bar
 */
void render_command_bar(SCREEN_ID screen_id)
{
	SCREEN_AREA rect;
	COMMAND i, command;

	get_command_bar_window(screen_id, &rect);

	clear_text_window(rect.top, rect.left, rect.bottom, rect.right);

	place_cursor_at(rect.top, rect.left);

	iterate_over_commands(i) {

		command = command_bar_item(i);

		if (command == CM_NIL) break;

		if (is_move_command(command)) continue;

		if (command == CM_CURRENT_TILE) continue;

		if (is_macro_command(command)) continue;

		render_hot_key(command_key(command));

		render_text(C_Text, "%s ", command_name(command));
	}
}



/*
 * confirmation dialogue
 */
void confirmation_dialogue(CONFIRMATION_DIALOGUE *dialogue)
{

	place_cursor_at(dialogue->position.y, dialogue->position.x);

	if (is_empty_string(dialogue->prompt)) {

		return;
	}

	render_text(dialogue->colour, dialogue->prompt);
	render_text(dialogue->colour, " [yn]");

	update_screen();

	confirmation_dialogue_input(dialogue);
}



/*
 * shows a text file
 */
void show_text_file(const char *title, PROGRAM_DIRECTORY directory,
	const char *file_name, bool confirmation
)
{
	char text[TEXT_BUFFER_SIZE];
	char screen_title[STRING_BUFFER_SIZE];
	COMMAND entered_command;
	int page;
	int last_page;

	sprintf(screen_title, "-== %s ==-", title);

	read_text_file(text, directory, file_name);

	/* hack */
	last_page = show_text_page(text, INT_MAX);

	page = 0;

	do {
		command_bar_clear();
		if (page > 0) command_bar_add(1, CM_PAGE_UP);
		if (page < last_page) command_bar_add(1, CM_PAGE_DOWN);
		if (confirmation) {
			command_bar_add(1, CM_OK);
		} else {
			command_bar_add(1, CM_EXIT);
		}

		render_secondary_screen(screen_title);

		show_text_page(text, page);

		update_screen();

		entered_command = command_bar_get_command();

		if (entered_command == CM_PAGE_UP) {
			--page;
			if (page < 0) page = 0;
		} else if (entered_command == CM_PAGE_DOWN) {
			++page;
			if (page > last_page) page = last_page;
		}

	} while (entered_command != CM_EXIT && entered_command != CM_OK);


}



/*
 * returns the screen area occupied by the command bar
 */
static SCREEN_AREA * get_command_bar_window(SCREEN_ID screen_id, SCREEN_AREA *rect)
{

	switch (screen_id) {

	case SID_CURRENT_POSITION:
		rect->top = cursor_y();
		rect->bottom = cursor_y();
		rect->left = cursor_x();
		rect->right = GAME_SCREEN_MAX_X;
		break;

	default:
		rect->top = SEC_SCREEN_COMMAND_ROW;
		rect->bottom = SEC_SCREEN_COMMAND_ROW;
		rect->left = SEC_SCREEN_MIN_X;
		rect->right = SEC_SCREEN_MAX_X;
		break;
	}

	return rect;
}



/*
 * shows a text page
 */
static int show_text_page(const char *text, int requested_page)
{
	char word[STRING_BUFFER_SIZE];
	const char *text_ptr;
	char *word_ptr;
	SCREEN_COORD y, x, x_inc;
	int current_page;

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;
	text_ptr = text;
	current_page = 0;

	do {

		word_ptr = word;

		while (*text_ptr != '\0' && *text_ptr != ' ' &&
			*text_ptr != ASCII_LF) {

			*word_ptr = *text_ptr;
			++word_ptr;
			++text_ptr;
		}

		*word_ptr = '\0';

		x_inc = strlen(word) + 1;

		if (x + x_inc > SEC_SCREEN_MAX_X) {

			++y;

			if (y > SEC_SCREEN_MAX_Y) {

				++current_page;
				y = SEC_SCREEN_MIN_Y;
			}

			x = SEC_SCREEN_MIN_X;
		}

		if (current_page == requested_page) {
			const char *letter;

			place_cursor_at(y, x);

			letter = word;

			while (*letter != '\0') {

				render_char(C_Text, *letter);
				++letter;
			}

			render_char(C_Text, ' ');
		}

		x += x_inc;

		if (*text_ptr == ASCII_LF) {

			++y;

			if (y > SEC_SCREEN_MAX_Y) {

				++current_page;
				y = SEC_SCREEN_MIN_Y;
			}

			x = SEC_SCREEN_MIN_X;

		} else if (*text_ptr == '\0') {

			break;
		}

		++text_ptr;

	} while (current_page <= requested_page);

	return current_page;
}







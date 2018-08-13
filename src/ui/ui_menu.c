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
 * Module: UI Menu
 */

#include "wrogue.h"


static int	string_list_width(const LIST *);






/*
 * renders the passed select list
 */
void render_select_list(const LIST *item_list)
{
	LIST_NODE *item_node;
	SCREEN_COORD y = ACTION_WINDOW_MIN_Y;
	SCREEN_COORD x = ACTION_WINDOW_MIN_X;
	KEY_CODE key = 'a';

	/* clear text window for list */
	clear_text_window(y, x,
		y + item_list->n_nodes - 1,
		x + string_list_width(item_list) + 4
	);

	/* render list */
	iterate_over_list(item_list, item_node) {
		const char * item_string = (const char *)item_node->data;

		place_cursor_at(y++, x);
		render_hot_key(key++);
		render_text(C_Text, " %s", item_string);
	}
}



/*
 * returns the index number of the chosen list item based on the 
 * passed key, or LIST_NODE_INDEX_NIL if the passed key is not
 * linked to any valid choice
 */
LIST_NODE_INDEX select_list_choice(const LIST *item_list, KEY_CODE key)
{
	
	/* choice is out of bounds .. */
	if (key - 'a' < 0 || key - 'a' > item_list->n_nodes - 1) {
		
		/* return LIST_NODE_INDEX_NIL */
		return LIST_NODE_INDEX_NIL;
	}

	return key - 'a';
}




/*
 * creates a menu
 */
MENU * menu_create(LIST *items)
{
	MENU *menu;

	menu = checked_malloc(sizeof *menu);

	menu->items = items;

	menu->entered_command = CM_NIL;

	menu->first_visible = menu->last_visible = menu->highlighted = 0;

	menu->last = menu->items->n_nodes - 1;

	return menu;
}



/*
 * frees a menu
 */
void menu_free(MENU *menu)
{

	list_free(menu->items);

	free(menu);
}



/*
 * free a menu and uses the passed destructor to free its items
 */
void menu_free_with(MENU *menu, void (*destructor)(void *))
{

	list_free_with(menu->items, destructor);

	free(menu);
}



/*
 * sets the appearance of a menu
 */
void menu_set_appearance(MENU *menu, SCREEN_AREA *menu_area)
{
	int visible_rows;

	if (menu_area->bottom == MENU_AUTO) {
		
		menu_area->bottom = menu_area->top + menu->last;
	}

	if (menu_area->right == MENU_AUTO) {
		
		menu_area->right = menu_area->left + 
			string_list_width(menu->items) - 1;
	}

	if (menu->first_visible > 0) {
		
		return;
	}

	visible_rows = menu_area->bottom - menu_area->top + 1;

	if (menu->highlighted >= visible_rows) {
		
		menu->first_visible = menu->highlighted;
		menu->last_visible  = menu->highlighted + visible_rows - 1;
		
		if (menu->last_visible > menu->last) {
			
			menu->last_visible = menu->last;
		}

	} else {
		
		menu->first_visible = 0;
		menu->last_visible = visible_rows - 1;
		
		if (menu->last_visible > menu->last) {
			
			menu->last_visible = menu->last;
		}
	}

}



/*
 * "executes" a menu (i.e. displays it on screen and lets the user select an item)
 */
COMMAND menu_execute(MENU *menu, SCREEN_AREA *menu_area)
{

	do {

		render_menu(menu, menu_area);
		update_screen();

		menu->entered_command = command_bar_get_command();

		if (menu->entered_command == CM_DOWN) {

			if (menu->highlighted < menu->last) {

				if (menu->highlighted == menu->last_visible) {
					menu->first_visible += 1;
					menu->last_visible += 1;
				}

				menu->highlighted += 1;

			}

		} else if (menu->entered_command == CM_UP) {

			if (menu->highlighted > 0) {

				if (menu->highlighted == menu->first_visible) {
					menu->first_visible -= 1;
					menu->last_visible -= 1;
				}

				menu->highlighted -= 1;
			}

		}


	} while (menu->entered_command == CM_DOWN || 
		menu->entered_command == CM_UP);

	return menu->entered_command;
}



/*
 * returns the length of the longest list item (in characters)
 */
static int string_list_width(const LIST *item_list)
{
	int max_length = 0;
	LIST_NODE *node;

	for (node = item_list->head; node != NULL; node = node->next) {
		int item_length = strlen((const char *)node->data);
		
		if (item_length > max_length) max_length = item_length;
	}

	return max_length;
}




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
 * Module: UI Misc
 */

#include "wrogue.h"



static void		render_dialogue_screen_active_window(DIALOGUE_STATE *);
static void		render_dialogue_screen_passive_window(DIALOGUE_STATE *);



/*
 * lets the user choose the difficulty level of the game
 */
GAME_DIFFICULTY ui_choose_difficulty(void)
{
	LIST *menu_items = list_new();
	MENU *menu;
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y + 5, MENU_AUTO,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};
	GAME_DIFFICULTY difficulty;
	char difficulty_info[TEXT_BUFFER_SIZE];

	list_add(menu_items, "Normal");
	list_add(menu_items, "Easy");

	menu = menu_create(menu_items);

	command_bar_set(3, CM_UP, CM_DOWN, CM_OK);

	render_secondary_screen("-== Choose Difficulty ==-");

	read_text_file(difficulty_info, DIR_HELP, FILE_DIFFICULTY_INFO);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X,
		difficulty_info, SEC_SCREEN_MAX_X
	);

	update_screen();

	menu_set_appearance(menu, &menu_area);

	menu_execute(menu, &menu_area);
	
	difficulty = menu->highlighted;
	
	menu_free(menu);

	return difficulty;
}




/*
 * renders the title screen of the game
 */
void render_game_title_screen(const char *warning_message)
{

	SCREEN_COORD row, col;
	COLOUR c;

	render_secondary_screen("");

	c = C_Metal;
	row = SEC_SCREEN_MIN_Y + 1;
	col = SEC_SCREEN_MIN_X + 9;

render_text_at(c,row++,col," _  _  _ _______  ______  _____        ______  _____   ______ _     _ _______");
render_text_at(c,row++,col," |  |  | |_____| |_____/ |_____]      |_____/ |     | |  ____ |     | |______");
render_text_at(c,row++,col," |__|__| |     | |    \\_ |            |    \\_ |_____| |_____| |_____| |______");

	c = C_Blood;

render_text_at(c,row++,col,"                                                                             ");
render_text_at(c,row++,col,"                                                                             ");
render_text_at(c,row++,col,"                                                                             ");
render_text_at(c,row++,col,"                           /     \\      /\\/\\/     \\                          ");
render_text_at(c,row++,col,"                          /       \\/\\  /           \\                         ");
render_text_at(c,row++,col,"                         /           \\/             )                        ");
render_text_at(c,row++,col,"                        (    / /\\          /\\ \\    /                         ");
render_text_at(c,row++,col,"                         \\  / /  \\        /  \\ \\  /                          ");
render_text_at(c,row++,col,"                          \\/ /____\\      /____\\ \\/                           ");
render_text_at(c,row++,col,"                          /                      \\                           ");
render_text_at(c,row++,col,"                         /                        \\                          ");
render_text_at(c,row++,col,"                        /            /\\            \\                         ");
render_text_at(c,row++,col,"                       /            /  \\            \\                        ");
render_text_at(c,row++,col,"                      /            /____\\            \\                       ");
render_text_at(c,row++,col,"                     <_________.            ._________>                      ");
render_text_at(c,row++,col,"                              / | |  |  |  |\\ \\                              ");
render_text_at(c,row++,col,"                             / /| |  |  |  | \\ \\                             ");
render_text_at(c,row++,col,"                            / / | |  |  |  |  \\ \\                            ");
render_text_at(c,row++,col,"                           /__\\ | |  |  |  |  /__\\                           ");
render_text_at(c,row++,col,"                                  |  |  |                                    ");
render_text_at(c,row++,col,"                                     |                                       ");



	row = SEC_SCREEN_MIN_Y + 11;
	col = SEC_SCREEN_MAX_X - 30;

	render_text_at(C_FieldName, row, col + 20, "Scenario:");
	render_text_at(C_FieldValue, ++row, col + 9, "%20s", scenario_name());
	render_text_at(C_FieldValue, ++row, col + 24, "%5s", scenario_version());


	row = SEC_SCREEN_MAX_Y - 2;
	col = SEC_SCREEN_MIN_X + 32;

	render_text_at(C_Metal, row - 1, col + 8, "Version %s", GAME_VERSION);
	render_text_at(C_Metal, ++row, col, GAME_WEBSITE);

	if (warning_message == NULL) {

		return;
	}

	render_text_at(C_Warning,
		SEC_SCREEN_TITLE_ROW, SEC_SCREEN_MIN_X,
		warning_message
	);
}



/*
 * game title screen menu
 */
COMMAND game_title_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y + 11, MENU_AUTO,
		SEC_SCREEN_MIN_X + 2, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * new game dialogue
 */
void new_game_dialogue(CONFIRMATION_DIALOGUE *dialogue)
{

	dialogue->position.y = SEC_SCREEN_TITLE_ROW;
	dialogue->position.x = SEC_SCREEN_MIN_X;
	dialogue->colour = C_Warning;

	confirmation_dialogue(dialogue);
}



/*
 * renders the galaxy generation screen
 */
void render_galaxy_generation_screen(bool generation_finished)
{
	const char *generation_message =
		"The program will now generate the game world. " \
		"This might take a while. ";

	render_secondary_screen("-= World Generation ==-");

	render_long_text(C_Text, SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X,
		generation_message, SEC_SCREEN_MAX_X
	);

	render_text_at(C_Text, SEC_SCREEN_MIN_Y + 4, SEC_SCREEN_MIN_X,
		"Generating world, please wait... "
	);

	if (generation_finished) {

		render_text(C_Text, "Finished!");
	}
}



/*
 * renders the shop screen
 */
void render_shop_screen(const CHARACTER *character)
{
	MONEY personal_funds;

	render_secondary_screen("-== Buy ==-");

	personal_funds = inventory_money(character);

	render_text_at(C_Text, SEC_SCREEN_MAX_Y - 1, SEC_SCREEN_MIN_X,
		"Personal funds: %dc", personal_funds
	);
}



/*
 * shop screen menu
 */
COMMAND shop_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y, SEC_SCREEN_MAX_Y - 3,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the dialogue screen
 */
void render_dialogue_screen(DIALOGUE_STATE *dialogue)
{
	render_background();

	render_dialogue_screen_passive_window(dialogue);

	render_dialogue_screen_active_window(dialogue);

	render_command_bar(SID_SECONDARY);
}



/*
 * renders the passive window of the dialogue screen
 */
static void render_dialogue_screen_passive_window(DIALOGUE_STATE *dialogue)
{

	clear_text_window(SEC_SCREEN_TITLE_ROW, SEC_SCREEN_MIN_X,
		SEC_SCREEN_TITLE_ROW, SEC_SCREEN_MAX_X);

	render_text_at(C_Text, SEC_SCREEN_TITLE_ROW, SEC_SCREEN_MIN_X,
		dialogue->passive_character->name);

	clear_text_window(
		SEC_SCREEN_MIN_Y,
		SEC_SCREEN_MIN_X,
		SEC_SCREEN_MAX_Y / 2,
		SEC_SCREEN_MAX_X
	);

	place_cursor_at(SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X,
		dialogue->text, SEC_SCREEN_MAX_X
	);

}



/*
 * renders the active window of the dialogue screen
 */
static void render_dialogue_screen_active_window(DIALOGUE_STATE *dialogue)
{
	NOT_USED(dialogue);

	clear_text_window(
		SEC_SCREEN_MAX_Y / 2 + 2,
		SEC_SCREEN_MIN_X,
		SEC_SCREEN_MAX_Y,
		SEC_SCREEN_MAX_X
	);

}



/*
 * dialogue screen menu
 */
COMMAND dialogue_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MAX_Y / 2 + 2, MENU_AUTO, SEC_SCREEN_MIN_X, MENU_AUTO};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the certificate of honour screen
 */
void render_certificate_screen(void)
{

	render_secondary_screen("-== Certificate of Honour ==-");

	render_text_at(C_Text, SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X,
		"Certificate of Honour written to %s.", FILE_CERTIFICATE
	);
}



/*
 * renders the sell screen
 */
void render_sell_screen(CHARACTER *seller, int highlighted_object)
{
	SCREEN_COORD y = SEC_SCREEN_MIN_Y;
	SCREEN_COORD x = SEC_SCREEN_MIN_X;
	LIST_NODE *node;
	int i;

	render_secondary_screen("-== Sell ==-");

	/* empty inventory - return */
	if (seller->inventory->head == NULL) return;

	for (node = seller->inventory->head, i = 0;
		node != NULL;
		node = node->next, i++, y++) {
		OBJECT *object = (OBJECT *)node->data;
		COLOUR colour = C_Text;

		if (i == highlighted_object) colour = C_Highlight;

		place_cursor_at(y, x);

		render_inventory_object(colour, seller, object, true);

		if (object->value != VALUE_NIL) {

			render_text(colour, " (%dc)", object->value / 2);
		}
	}
}



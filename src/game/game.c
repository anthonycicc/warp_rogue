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
 * Module: Game
 */

#include "wrogue.h"





static void		game_new(void);

static void		game_loop(void);

static bool		game_title_screen(void);

static void		game_choose_difficulty(void);





/*
 * game difficulty
 */
static GAME_DIFFICULTY	GameDifficulty;





/*
 * starts the game
 */
void game_run(void)
{

	program_init();

	while (!game_title_screen()) {

		/* EMPTY LOOP */;
	}
}



/*
 * returns the difficulty of the game
 */
GAME_DIFFICULTY game_difficulty(void)
{

	return GameDifficulty;
}



/*
 * sets the difficulty of the game
 */
void game_difficulty_set(GAME_DIFFICULTY difficulty)
{

	GameDifficulty = difficulty;
}



/*
 * the game controls screen
 */
void game_controls_screen(void)
{
	bool terminate_game = false;

	while (true) {
		LIST *items;
		MENU *menu;

		items = list_new();
		list_add(items, "Save and quit");
		list_add(items, "Quit");
		list_add(items, "Macros");
		list_add(items, "Certificate");
		if (fullscreen_mode()) {
			list_add(items, "Windowed mode");
		} else {
			list_add(items, "Fullscreen mode");
		}

		menu = menu_create(items);

		command_bar_set(4, CM_UP, CM_DOWN, CM_OK, CM_EXIT);

		render_game_controls_screen();

		game_controls_screen_menu(menu);

		if (menu->entered_command == CM_EXIT) break;

		if (menu->highlighted == 0) {

			save_point_create();

			terminate_game = true;

			break;

		} else if (menu->highlighted == 1) {

			saved_game_erase();

			terminate_game = true;

			break;

		} else if (menu->highlighted == 2) {

			macro_setup_screen();

		} else if (menu->highlighted == 3) {

			certificate_screen();

		} else if (menu->highlighted == 4) {

			change_screen_mode();
		}

		menu_free(menu);
	}



	if (terminate_game) {

		program_shutdown();
	}
}



/*
 * starts a new game
 */
static void game_new(void)
{

	scenario_intro();

	galaxy_generation();

	game_choose_difficulty();

	/* execute game start script */
	script_load(DIR_RULES, FILE_START_SCRIPT);
	script_execute();

	message(SID_GAME, "Press '?' to open the help screen.");

	game_loop();
}



/*
 * the main game loop
 */
static void game_loop(void)
{

	while (true) {

		game_round();
	}
}



/*
 * the title screen of the game
 */
static bool game_title_screen(void)
{
#define TS_NEW_GAME		0
#define TS_CONTINUE_GAME	1
#define TS_CREDITS		2
#define TS_QUIT			3

	bool saved_game = save_point_exists();
	LIST *menu_items = list_new();
	MENU *menu;
	int chosen_action;

	list_add(menu_items, "New game");
	list_add(menu_items, "Continue game");
	list_add(menu_items, "Credits");
	menu = menu_create(menu_items);

	command_bar_set(4, CM_UP, CM_DOWN, CM_OK, CM_EXIT);

	render_game_title_screen(false);
	update_screen();

	game_title_screen_menu(menu);

	if (menu->entered_command == CM_EXIT) {

		chosen_action = TS_QUIT;

	} else {

		chosen_action = menu->highlighted;
	}

	menu_free(menu);

	if (NO_SCENARIO_INSTALLED && (chosen_action == TS_NEW_GAME ||
		chosen_action == TS_CONTINUE_GAME)) {

		render_game_title_screen("No scenario installed");
		update_screen();

		get_key();

		return false;
	}

	if (chosen_action == TS_NEW_GAME) {

		if (saved_game) {
			CONFIRMATION_DIALOGUE dialogue;

			strcpy(dialogue.prompt,
				"You will lose your old game! Sure?"
			);

			dialogue.confirmed = false;

			new_game_dialogue(&dialogue);

			if (!dialogue.confirmed) {

				return false;
			}

			saved_game_erase();
		}

		game_new();

	} else if (chosen_action == TS_CONTINUE_GAME) {

		if (!saved_game) {

			render_game_title_screen("There is no saved game");
			update_screen();

			get_key();

			return false;
		}

		if (!save_point_load()) {

			render_game_title_screen("Saved game not compatible");
			update_screen();

			get_key();

			return false;
		}

		game_loop();

	} else if (chosen_action == TS_CREDITS) {

		show_text_file("Credits", DIR_INFO, FILE_CREDITS, false);

		return false;

	} else if (chosen_action == TS_QUIT) {

		program_shutdown();
	}

	return true;
}



/*
 * lets the player choose the difficulty of the game
 */
static void game_choose_difficulty(void)
{
	GAME_DIFFICULTY difficulty = ui_choose_difficulty();	

	game_difficulty_set(difficulty);
}



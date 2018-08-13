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
 * Module: UI Generation and Advancement
 */

#include "wrogue.h"



/*
 * renders the choose gender screen
 */
void render_choose_gender_screen(void)
{
	render_secondary_screen("-== Choose Gender ==-");
}



/*
 * choose gender screen menu
 */
COMMAND choose_gender_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MIN_Y, MENU_AUTO, SEC_SCREEN_MIN_X, MENU_AUTO};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the roll stats screen
 */
void render_roll_stats_screen(const CHARACTER *character)
{
	SCREEN_COORD y = SEC_SCREEN_MIN_Y, x = SEC_SCREEN_MIN_X;
	
	render_secondary_screen("-== Roll Stats ==-");

	render_stats_at(character, y, x);
}



/*
 * renders name character screen
 */
void render_name_character_screen(void)
{

	render_secondary_screen("-== Name Character ==-");
}



/*
 * enter name dialogue
 */
void enter_name_dialogue(GET_TEXT_DIALOGUE *dialogue)
{
	get_text_dialogue_execute(dialogue, SEC_SCREEN_MIN_Y + 5, SEC_SCREEN_MIN_X);
}



/*
 * renders the character advancement screen
 */
void render_character_advancement_screen(const CHARACTER *character)
{
	SCREEN_COORD y = SEC_SCREEN_MIN_Y + 1;
	SCREEN_COORD x = SEC_SCREEN_MIN_X + 34;
	STAT stat;
	
	render_secondary_screen("-== Character Advancement ==-");

	iterate_over_stats(stat) {

		render_text_at(C_FieldName, y, x, " %s ",
			stat_short_name(stat)
		);

		render_text_at(C_FieldValue, y + 1, x, "+%-3d",
			character->career->stat[stat].advance_maximum
		);

		render_text_at(C_FieldValue, y + 2, x, "+%-3d",
			character->stat[stat].advance
		);

		x += 5;
	}

	x = SEC_SCREEN_MIN_X + 18;

	render_text_at(C_FieldName, ++y, x, "Advance Scheme:");
	render_text_at(C_FieldName, ++y, x, "Total Advances:");

	y = SEC_SCREEN_MIN_Y + 5;

	render_text_at(C_FieldName, y, x,   "EP Total      : ");
	render_text(C_FieldValue, "%d", character->ep_total);

	y += 4;
	x = SEC_SCREEN_MIN_X;

	render_text_at(C_FieldName, y, x, "[Advancement Options]");
}



/*
 * character advancement screen menu
 */
COMMAND character_advancement_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MIN_Y + 10, MENU_AUTO,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the increase stat screen
 */
void render_increase_stat_screen(const CHARACTER *character, MENU *menu,
	bool free_advance)
{
	char text[TEXT_BUFFER_SIZE];
	SCREEN_COORD y, x;

	if (free_advance) {

		render_secondary_screen("-== Free Advance ==-");

	} else {

		render_secondary_screen("-== Increase Stat ==-");
	}

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;

	if (free_advance) {

		sprintf(text,
			"At the beginning of your career you gain a " \
			"free advance. The chosen stat will be " \
			"increased by %d points. Choose stat:",
			STANDARD_STAT_ADVANCE
		);

	} else {

		sprintf(text,
			"Increasing a stat costs %d EP, and will " \
			"increase the chosen stat by %d points. Which " \
			"stats you can increase depends on the " \
			"advancement scheme of your current career.",
			EP_COST_INCREASE_STAT, STANDARD_STAT_ADVANCE
		);
	}

	y = render_long_text(C_Text, y, x,
		text, SEC_SCREEN_MAX_X
	);

	if (menu == NULL) {

		render_text_at(C_Text, y += 2, x,
			"No increasable stats."
		);

		return;
	}

	if (free_advance) return;

	if (character->ep < EP_COST_INCREASE_STAT) {

		render_text_at(C_Text, y += 2, x,
			"Not enough EP."
		);

		return;
	}

	render_text_at(C_Text, y += 2, x,
		"You have %d EP. Please choose a stat:", character->ep
	);


}



/*
 * increase stat screen menu
 */
COMMAND increase_stat_screen_menu(MENU *menu, bool free_advance)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MIN_Y, MENU_AUTO,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	if (free_advance) {
		menu_area.top += 3;
	} else {
		menu_area.top += 5;
	}

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the new perk screen
 */
void render_new_perk_screen(const CHARACTER *character, MENU *menu)
{
	SCREEN_COORD y, x;

	render_secondary_screen("-== Buy Perk ==-");

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;

	render_text_at(C_Text, y, x,
		"Buying a perk costs %d EP. A character can only have " \
		"up to %d perks so choose wisely.",
		EP_COST_BUY_PERK, MAX_CHARACTER_PERKS
	);

	if (character_n_perks(character) >= MAX_CHARACTER_PERKS)  {

		render_text_at(C_Text, y += 2, x,
			"Maximal number of perks already reached."
		);

		return;
	}

	if (menu == NULL) {

		render_text_at(C_Text, y += 2, x,
			"No perks available."
		);

		return;
	}

	if (character->ep < EP_COST_BUY_PERK) {

		render_text_at(C_Text, y += 2, x,
			"Not enough EP."
		);

		return;
	}

	render_text_at(C_Text, y += 2, x,
		"You have %d EP. Choose perk:", character->ep
	);
}



/*
 * new perk screen menu
 */
COMMAND new_perk_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y + 4, SEC_SCREEN_MAX_Y - 3,
		SEC_SCREEN_MIN_X, SEC_SCREEN_MAX_X
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the learn psychic power screen
 */
void render_learn_psy_power_screen(const CHARACTER *character, MENU *menu,
	bool free_power
)
{
	char text[TEXT_BUFFER_SIZE];
	SCREEN_COORD y, x;

	if (free_power) {

		render_secondary_screen("-== Free Psychic Power ==-");

	} else {

		render_secondary_screen("-== Learn Psychic Power ==-");
	}

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;

	if (free_power) {

		sprintf(text, "A character can only learn up to %d " \
			"psychic powers so choose wisely.",
			MAX_CHARACTER_PSY_POWERS
		);

	} else {

		sprintf(text,
			"Learning a psychic power costs %d EP. A " \
			"character can only learn up to %d psychic " \
			"powers so choose wisely.",
			EP_COST_LEARN_PSY_POWER,
			MAX_CHARACTER_PSY_POWERS
		);
	}

	y = render_long_text(C_Text, y, x, text, SEC_SCREEN_MAX_X);

	if (!character_has_perk(character, PK_PSYCHIC_I))  {

		render_text_at(C_Text, y += 2, x,
			"You need the %s perk to learn psychic powers.",
			perk_name(PK_PSYCHIC_I)
		);

		return;
	}

	if (menu == NULL) {

		render_text_at(C_Text, y += 2, x,
			"No learnable psychic powers."
		);

		return;
	}

	if (n_psy_powers(character) >= MAX_CHARACTER_PSY_POWERS)  {

		render_text_at(C_Text, y += 2, x,
			"Maximal number of psychic powers already reached.",
			MAX_CHARACTER_PSY_POWERS
		);

		return;
	}

	if (!free_power && character->ep < EP_COST_LEARN_PSY_POWER) {

		render_text_at(C_Text, y += 2, x,
			"Not enough EP."
		);

		return;
	}

	if (free_power) {

		render_text_at(C_Text, y += 2, x, "Choose power:");

	} else {

		render_text_at(C_Text, y += 2, x,
			"You have %d EP. Choose power:", character->ep
		);
	}
}



/*
 * learn psychic power screen menu
 */
COMMAND learn_psy_power_screen_menu(MENU *menu, bool free_power)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y + 5, SEC_SCREEN_MAX_Y - 3,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	if (free_power) {
		menu_area.top -= 1;
	}

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the follow career path screen
 */
void render_follow_career_path_screen(const CHARACTER *character, MENU *menu)
{
	SCREEN_COORD y, x;
	const char *text =
		"You have to complete your current career before you can " \
		"start pursuing another one. Completing a career means " \
		"taking all available stat advancements. You do NOT have "\
		"to take all available perks!";

	render_secondary_screen("-== Change Career (Follow Path) ==-");

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;

	if (!career_completed(character)) {

		render_long_text(C_Text, y, x, text, SEC_SCREEN_MAX_X);
		return;
	}

	render_text_at(C_Text, y, x,
		"Changing careers by using a career exit costs %d EP.",
		EP_COST_CHANGE_CAREER_FOLLOW_PATH
	);

	if (menu == NULL) {
		render_text_at(C_Text, y += 2, x,
			"No career exits."
		);
		return;
	}

	if (character->ep < EP_COST_CHANGE_CAREER_FOLLOW_PATH) {
		render_text_at(C_Text, y += 2, x,
			"Not enough EP."
		);
		return;
	}

	render_text_at(C_Text, y += 2, x,
		"You have %d EP. Choose career:", character->ep
	);

}



/*
 * follow career path screen menu
 */
COMMAND follow_career_path_screen_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MIN_Y + 4, MENU_AUTO, SEC_SCREEN_MIN_X, MENU_AUTO};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the new career path screen
 */
void render_new_career_path_screen(const CHARACTER *character,
	bool initial_career
)
{
	SCREEN_COORD y, x;
	const char *text =
		"You have to complete your current career before you can " \
		"start pursuing another one. Completing a career means " \
		"taking all available stat advancements. You do NOT have "\
		"to take all available perks!";

	if (initial_career) {

		render_secondary_screen("-== Initial Career ==-");
		return;

	} else {

		render_secondary_screen("-== Change Career (New Path) ==-");
	}

	y = SEC_SCREEN_MIN_Y;
	x = SEC_SCREEN_MIN_X;

	if (!career_completed(character)) {

		render_long_text(C_Text, y, x, text, SEC_SCREEN_MAX_X);
		return;
	}

	render_text_at(C_Text, y, x,
		"Starting a new career path costs %d EP.",
		EP_COST_CHANGE_CAREER_NEW_PATH
	);

	if (character->ep < EP_COST_CHANGE_CAREER_NEW_PATH) {

		render_text_at(C_Text, y += 2, x,
			"Not enough EP."
		);
		return;
	}

	render_text_at(C_Text, y += 2, x,
		"You have %d EP. Choose career:", character->ep
	);

}



/*
 * new career path screen menu
 */
COMMAND new_career_path_screen_menu(MENU *menu, bool initial_career)
{
	SCREEN_AREA menu_area = {SEC_SCREEN_MIN_Y, MENU_AUTO, SEC_SCREEN_MIN_X, MENU_AUTO};

	if (!initial_career) {
		menu_area.top += 4;
	}

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}




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
 * Module: UI Game Screen
 */

#include "wrogue.h"





static void	render_action_window(void);

static void	render_sector(SCREEN_COORD, SCREEN_COORD, const SECTOR *);

static void	render_character_info_windows(const CHARACTER *);

static void	render_weapon_status_window(const CHARACTER *);
static void	render_armour_status_window(const CHARACTER *);

static void	render_party_window(void);

static void	render_location_windows(void);


/*
 * gore features
 */
static GORE_FEATURE GoreFeature[MAX_GORE_FEATURES] = {

	{"Zero",		SYM_NIL,	C_NIL},
	{"Blood",		SYM_NIL,	C_NIL},
	{"Body part",		SYM_NIL,	C_NIL},
	{"Plant parts",		SYM_NIL,	C_NIL},
	{"Machine parts",	SYM_NIL,	C_NIL},
	{"Poisoned corpse",	SYM_NIL,	C_NIL},
	{"Corpse",		SYM_NIL,	C_NIL}

};



/*
 * renders the game screen
 */
void render_game_screen(void)
{

	render_background();

	render_message_window();

	render_action_window();

	render_character_info_windows(player_controlled_character());

	render_party_window();
}



/*
 * renders the message window
 */
void render_message_window(void)
{

	clear_text_window(
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MIN_X,
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MAX_X
	);

	render_text_at(C_Text,
		GAME_SCREEN_MESSAGE_ROW, GAME_SCREEN_MIN_X,
		game_screen_message_buffer()
	);
}



/*
 * returns a pointer to the data of the requested gore feature
 */
GORE_FEATURE * gore_feature(const char *feature_name)
{
	GORE_LEVEL i;

	for (i = GORE_LEVEL_ZERO + 1; i < MAX_GORE_FEATURES; i++) {
		GORE_FEATURE *feature = &GoreFeature[i];

		if (strings_equal(feature_name, feature->name)) {

			return feature;
		}
	}

	return NULL;
}



/*
 * renders the action window
 */
static void render_action_window(void)
{
	const AREA_SECTION *visible_area;
	SCREEN_COORD y, x;

	clear_text_window(ACTION_WINDOW_MIN_Y, ACTION_WINDOW_MIN_X,
		ACTION_WINDOW_MAX_Y, ACTION_WINDOW_MAX_X
	);

	visible_area = get_visible_area();

	for (y = ACTION_WINDOW_MIN_Y; y <= ACTION_WINDOW_MAX_Y; y++) {
	for (x = ACTION_WINDOW_MIN_X; x <= ACTION_WINDOW_MAX_X; x++) {
		AREA_POINT p;

		p.y = visible_area->top + y - ACTION_WINDOW_MIN_Y;
		p.x = visible_area->left + x - ACTION_WINDOW_MIN_X;

		render_sector(y, x, sector_at(&p));
	}
	}
}



/*
 * renders the passed sector at the passed location
 */
static void render_sector(SCREEN_COORD y, SCREEN_COORD x,
	const SECTOR *sector
)
{
	const CHARACTER *character = sector->character;
	GORE_LEVEL gore_level = GORE_LEVEL_ZERO;
	SYMBOL symbol;
	COLOUR colour;

	/*
	 * character present AND
	 * (character is not hostile OR
	 * character is not unnoticed) ..
	 */
	if (character != NULL && (faction_relationship(character->faction, 
		FACTION_PLAYER) != FR_HOSTILE ||
		!character_unnoticed(character))) {

		/* render character */
		symbol = character->symbol;
		colour = character->colour;
		render_char_at(colour, y, x, symbol);

		return;
	}

	/* no character .. render object if present */
	if (sector->object != NULL) {
		const OBJECT *object = sector->object;

		symbol = object->symbol;
		colour = object->colour.current;
		gore_level = object->gore_level;

	/* no object either - render terrain */
	} else {
		const TERRAIN *terrain = sector->terrain;

		symbol = terrain->symbol;
		colour = terrain->colour.current;
		gore_level = terrain->gore_level;
	}

	/* apply gore features if necessary */
	if (gore_level != GORE_LEVEL_ZERO) {
		const GORE_FEATURE *feature = &GoreFeature[gore_level];

		if (feature->symbol != SYM_NIL) symbol = feature->symbol;
		colour = feature->colour;
	}

	render_char_at(colour, y, x, symbol);
}



/*
 * renders the character info windows
 */
static void render_character_info_windows(const CHARACTER *character)
{

	clear_text_window(3, INFO_BAR_MIN_X, 3, INFO_BAR_MAX_X);
	render_text_at(C_FieldValue, 3, INFO_BAR_MIN_X, character->name);

	clear_text_window(5, INFO_BAR_MIN_X, 5, INFO_BAR_MAX_X);

	place_cursor_at(5, INFO_BAR_MIN_X);

	render_text(C_FieldValue, "%d EP", character->ep);

	clear_text_window(7, INFO_BAR_MIN_X, 14, INFO_BAR_MAX_X);
	render_stats_at(character, 7, INFO_BAR_MIN_X);

	render_weapon_status_window(character);

	render_armour_status_window(character);

	render_location_windows();
}



/*
 * renders the weapon status window
 */
static void render_weapon_status_window(const CHARACTER *character)
{
	OBJECT *weapon = character->weapon;
	SCREEN_COORD y = 16;

	clear_text_window(y, INFO_BAR_MIN_X, y + 1, INFO_BAR_MAX_X);

	if (weapon == NULL) return;

	render_lstring_at(C_FieldValue, y, INFO_BAR_MIN_X,
		weapon->name, 24
	);

	if (weapon->charge != CHARGE_NIL) {

		render_text_at(C_FieldValue, ++y, INFO_BAR_MIN_X, "[%d]",
			weapon->charge
		);
	}

	if (object_has_attribute(weapon, OA_JAMMED)) {
		render_text(C_FieldValue, " (jammed)");
	}
}



/*
 * renders the armour status window
 */
static void render_armour_status_window(const CHARACTER *character)
{
	COLOUR colour = C_FieldValue;
	SCREEN_COORD y = 19;

	clear_text_window(y, INFO_BAR_MIN_X, y + 1, INFO_BAR_MAX_X);

	if (character->armour != NULL) {

		render_lstring_at(C_FieldValue, y, INFO_BAR_MIN_X,
			character->armour->name, 24
		);
	}

	render_text_at(C_FieldName, ++y, INFO_BAR_MIN_X,
		"%-13s: ", "Armour Value"
	);

	if (character->armour_rating.current >
		character->armour_rating.total) {

		colour = C_FieldValueIncreased;
	}

	render_text(colour,  "%d",
		character->armour_rating.current
	);
}



/*
 * renders the party window
 */
static void render_party_window(void)
{
	SCREEN_COORD min_y, max_y;
	LIST_NODE *node;

	min_y = PARTY_WINDOW_MIN_Y;
	max_y = PARTY_WINDOW_MAX_Y;

	for (node = party_player()->head;
		node != NULL;
		node = node->next) {
		SCREEN_COORD min_x, max_x;
		CHARACTER *character;

		character = (CHARACTER *)node->data;

		min_x = PARTY_WINDOW_MIN_X;
		min_x += character->type * (CHARACTER_WINDOW_WIDTH + 4);

		max_x = min_x + CHARACTER_WINDOW_WIDTH;

		clear_text_window(min_y, min_x, max_y, max_x);

		place_cursor_at(min_y, min_x);
		render_hot_key(command_key(CM_CHARACTER_1 +
			character->type)
		);

		render_lstring_at(C_FieldValue, min_y, min_x + 4,
			character->name,
			CHARACTER_WINDOW_WIDTH - 4
		);

		place_cursor_at(min_y + 1, min_x);

		render_character_status(character, max_y, false);
	}
}



/*
 * renders the location windows
 */
static void render_location_windows(void)
{
	AREA *area;

	area = active_area();

	clear_text_window(LOCATION_WINDOW_MIN_Y, INFO_BAR_MIN_X,
		LOCATION_WINDOW_MIN_Y + 1, INFO_BAR_MAX_X
	);
	render_text_at(C_FieldValue, LOCATION_WINDOW_MIN_Y,
		INFO_BAR_MIN_X, planet()->name
	);
	render_text_at(C_FieldValue, LOCATION_WINDOW_MIN_Y + 1,
		INFO_BAR_MIN_X, "%d, %d",
		area->location.y,
		area->location.x
	);

	clear_text_window(LOCATION_WINDOW_MIN_Y + 3, INFO_BAR_MIN_X,
		LOCATION_WINDOW_MIN_Y + 4, INFO_BAR_MAX_X
	);
	render_text_at(C_FieldValue, LOCATION_WINDOW_MIN_Y + 3,
		INFO_BAR_MIN_X, area->name
	);
	render_text_at(C_FieldValue, LOCATION_WINDOW_MIN_Y + 4,
		INFO_BAR_MIN_X, "Level %d", area->location.z
	);


}

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
 * Module: UI Information Screens
 */

#include "wrogue.h"



static void	render_career_exits_at(SCREEN_COORD, SCREEN_COORD,
			const CAREER *
		);

static void	render_trappings_at(SCREEN_COORD, SCREEN_COORD,
			const CAREER *
		);

static void	render_equipment_at(SCREEN_COORD, SCREEN_COORD,
			const CHARACTER *
		);

static void	render_perks_at(SCREEN_COORD, SCREEN_COORD, const bool *);

static void	render_career_perks_at(SCREEN_COORD, SCREEN_COORD,
			const CAREER *
		);

static void	render_advance_scheme_at(SCREEN_COORD, SCREEN_COORD,
			const CAREER *
		);

static void	render_psy_powers(SCREEN_ID, const CHARACTER *);

static void	render_object_screen_ccw(const OBJECT *);

static void	render_object_screen_rcw(const OBJECT *);

static const char *	firing_mode_string(const FIRING_MODE_DATA *);

static void	render_object_attributes_at(SCREEN_COORD,
			SCREEN_COORD, const OBJECT *
		);






/*
 * renders the planet map
 */
void render_planet_map_screen(const PLANET_POINT *current_position,
	const CHARACTER *character
)
{
	SCREEN_COORD x, y;
	PLANET_POINT p;

	render_background();

	/* message window */
	clear_text_window(
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MIN_X,
		GAME_SCREEN_MESSAGE_ROW,
		GAME_SCREEN_MAX_X
	);

	/* map screen */
	clear_text_window(PLANET_MAP_MIN_Y, PLANET_MAP_MIN_X,
		PLANET_MAP_MAX_Y, PLANET_MAP_MAX_X
	);

	/* planet name */
	clear_text_window(PLANET_INFO_MIN_Y, PLANET_INFO_MIN_X,
		PLANET_INFO_MIN_Y, PLANET_INFO_MAX_X
	);
	render_text_at(C_FieldValue, PLANET_INFO_MIN_Y, PLANET_INFO_MIN_X,
		planet()->name
	);


	/* planet description */
	clear_text_window(PLANET_INFO_MIN_Y + 2, PLANET_INFO_MIN_X,
		PLANET_POSITION_Y - 2, PLANET_INFO_MAX_X
	);

	render_long_text(C_Text, PLANET_INFO_MIN_Y + 2, PLANET_INFO_MIN_X,
		planet()->description, PLANET_INFO_MAX_X
	);


	/* location */
	clear_text_window(PLANET_POSITION_Y, PLANET_INFO_MIN_X,
		PLANET_POSITION_Y, PLANET_INFO_MAX_X
	);
	render_text_at(C_FieldValue, PLANET_POSITION_Y,
		PLANET_INFO_MIN_X, "%d, %d",
		current_position->y,
		current_position->x
	);

	clear_text_window(PLANET_POSITION_Y + 2, PLANET_INFO_MIN_X,
		PLANET_POSITION_Y + 2, PLANET_INFO_MAX_X
	);
	render_text_at(C_FieldValue, PLANET_POSITION_Y + 2,
		PLANET_INFO_MIN_X, planet_tile(current_position)->name
	);


	p.z = current_position->z;

	for (p.y = 0, y = SEC_SCREEN_MIN_Y; p.y < PLANET_HEIGHT; p.y++, y++) {
	for (p.x = 0, x = SEC_SCREEN_MIN_X; p.x < PLANET_WIDTH; p.x++, x++) {
		const PLANET_TILE *tile = planet_tile(&p);

		render_char_at(tile->colour.current, y, x,
			tile->screen_symbol
		);
	}
	}

	y = SEC_SCREEN_MIN_Y + current_position->y;
	x = SEC_SCREEN_MIN_X + current_position->x;

	render_char_at(character->colour, y, x, character->symbol);
}



/*
 * renders the career screen
 */
void render_career_screen(const CAREER *career)
{

	render_secondary_screen("-== Career ==-");

	render_field("Name: ", career->name);
	render_field("Type: ", career_type_name(career->type));

	render_advance_scheme_at(SEC_SCREEN_MIN_Y + 3, SEC_SCREEN_MIN_X, career);

	render_trappings_at(SEC_SCREEN_MIN_Y + 12, SEC_SCREEN_MIN_X,
		career
	);

	render_career_perks_at(SEC_SCREEN_MIN_Y, SEC_SCREEN_MAX_X / 2 - 12,
		career
	);

	render_career_exits_at(SEC_SCREEN_MIN_Y, SEC_SCREEN_MAX_X / 2 + 20,
		career
	);

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 24, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 25, SEC_SCREEN_MIN_X,
		career->description, SEC_SCREEN_MAX_X
	);

}



/*
 * renders the character screen basics
 */
void render_character_screen_basics(const CHARACTER *character)
{
	const char *description;

	render_secondary_screen("-== Character ==-");

	render_field("Name        : ", character->name);

	if (character->career == NULL) {

		render_field("Career      : ", "-");

	} else {

		render_field("Career      : ",
			character->career->name
		);
	}

	render_field("Power Level : ", "%d", character->power_level);
	render_field("Faction     : ", faction_name(character->faction));

	render_stats_at(character, SEC_SCREEN_MIN_Y + 5, SEC_SCREEN_MIN_X);

	render_text_at(C_Header, SEC_SCREEN_MIN_Y,
		SEC_SCREEN_MAX_X / 2 + 20,"[Attributes]"
	);

	place_cursor_at(SEC_SCREEN_MIN_Y + 1, SEC_SCREEN_MAX_X / 2 + 20);
	render_character_status(character, SEC_SCREEN_MAX_Y - 1, true);

	render_perks_at(SEC_SCREEN_MIN_Y, SEC_SCREEN_MAX_X / 2 - 12,
		character->perk
	);

	render_equipment_at(SEC_SCREEN_MIN_Y + 14, SEC_SCREEN_MIN_X,
		character
	);

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 24, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	description = character_description(character);
	if (description == NULL) return;

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 25, SEC_SCREEN_MIN_X,
		description,
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the character attribute screen
 */
void render_character_attribute_screen(CHARACTER_ATTRIBUTE attribute)
{
	char description[DESCRIPTION_SIZE];

	render_secondary_screen("-== Character Attribute ==-");

	render_field("Name: ", character_attribute_name(attribute));

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 2, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 3, SEC_SCREEN_MIN_X,
		character_attribute_description(description, attribute),
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the object screen
 */
void render_object_screen(const OBJECT *object)
{
	render_secondary_screen("-== Object ==-");

	render_field("Name     : ", object->name);
	render_field("Type     : ", object_type_name(object->type));

	if (object->subtype != OSTYPE_NIL &&
		object->type != OTYPE_ENVIRONMENT) {

		render_field("Subtype  : ",
			object_subtype_name(object->subtype)
		);
	}

	if (object->condition != CONDITION_INDESTRUCTABLE) {

		render_field("Condition: ", "%d/%d",
			object->condition, object->condition_max
		);
	}

	if (object->weight != WEIGHT_NIL) {

		render_field("Weight   : ", "%d", object->weight);
	}

	if (object->value != VALUE_NIL) {

		render_field("Value    : ", "%dc", object->value);

	} else if (object->type == OTYPE_MONEY) {

		render_field("Value    : ", "%dc", object->charge);
	}

	place_cursor_at(cursor_y() + 1, cursor_x());

	switch (object->type) {

	case OTYPE_RANGED_COMBAT_WEAPON:
		render_object_screen_rcw(object);
		break;

	case OTYPE_CLOSE_COMBAT_WEAPON:
		render_object_screen_ccw(object);
		break;

	case OTYPE_ARMOUR:
		render_field("Armour Value : ", "%d", object->armour_value);
		break;

	default:
		/* DO NOTHING */ ;

	}

	render_object_attributes_at(SEC_SCREEN_MIN_Y,
		SEC_SCREEN_MAX_X / 2 + SEC_SCREEN_MIN_X,
		object
	);

	render_text_at(C_Header, SEC_SCREEN_MAX_Y / 2 + 5, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MAX_Y / 2 + 6, SEC_SCREEN_MIN_X,
		object->description, SEC_SCREEN_MAX_X
	);
}



/*
 * renders the object attribute screen
 */
void render_object_attribute_screen(OBJECT_ATTRIBUTE attribute)
{
	char description[DESCRIPTION_SIZE];

	render_secondary_screen("-== Object Attribute ==-");

	render_field("Name: ", object_attribute_name(attribute));

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 2, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 3, SEC_SCREEN_MIN_X,
		object_attribute_description(description, attribute),
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the terrain screen
 */
void render_terrain_screen(const TERRAIN *terrain)
{
	SCREEN_COORD row, col;
	TERRAIN_ATTRIBUTE i;

	render_secondary_screen("-== Terrain ==-");

	row = SEC_SCREEN_MIN_Y;
	col = SEC_SCREEN_MIN_X;

	render_field("Name     : ", terrain->name);

	row = SEC_SCREEN_MIN_Y;
	col = SEC_SCREEN_MAX_X / 2 + SEC_SCREEN_MIN_X;

	render_text_at(C_Header, row, col, "[Attributes]");

	for (i = 0; i < MAX_TERRAIN_ATTRIBUTES; i++) {

		if (!terrain_has_attribute(terrain, i)) continue;

		render_text_at(C_Text, ++row, col,
			terrain_attribute_name(i)
		);
	}

	render_text_at(C_Header, SEC_SCREEN_MAX_Y / 2 + 4,
		SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MAX_Y / 2 + 5,
		SEC_SCREEN_MIN_X,
		terrain->description,
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the terrain attribute screen
 */
void render_terrain_attribute_screen(TERRAIN_ATTRIBUTE attribute)
{
	char description[DESCRIPTION_SIZE];

	render_secondary_screen("-== Terrain Attribute ==-");

	render_field("Name: ", terrain_attribute_name(attribute));

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 2, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 3, SEC_SCREEN_MIN_X,
		terrain_attribute_description(description, attribute),
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the psychic powers screen
 */
void render_psy_powers_screen(const CHARACTER *character)
{

	render_secondary_screen("-== Psychic Powers ==-");

	render_psy_powers(SID_CHARACTER, character);
}



/*
 * renders the psychic power screen
 */
void render_psy_power_screen(PSY_POWER power)
{
	char description[DESCRIPTION_SIZE];

	render_secondary_screen("-== Psychic Power ==-");

	render_field("Name           : ", psy_power_name(power));
	render_field("Evocation Value: ", "%d",
		psy_power_evocation_value(power)
	);

	place_cursor_at(cursor_y() + 1, cursor_x());
	render_header("[Description]");

	render_long_text(C_Text, cursor_y(), cursor_x(),
		psy_power_description(description, power),
		SEC_SCREEN_MAX_X
	);
}



/*
 * renders the perk screen
 */
void render_perk_screen(PERK perk)
{
	char description[DESCRIPTION_SIZE];

	render_secondary_screen("-== Perk ==-");

	render_text_at(C_FieldName, SEC_SCREEN_MIN_Y, SEC_SCREEN_MIN_X,
		"Name: "
	);
	render_text(C_FieldValue, "%s", perk_name(perk));

	render_text_at(C_Header, SEC_SCREEN_MIN_Y + 2, SEC_SCREEN_MIN_X,
		"[Description]"
	);

	render_long_text(C_Text, SEC_SCREEN_MIN_Y + 3, SEC_SCREEN_MIN_X,
		perk_description(description, perk), SEC_SCREEN_MAX_X
	);
}



/*
 * renders the additional information screen
 */
void render_additional_information_screen(void)
{

	render_secondary_screen("-== Additional Information ==-");
}



/*
 * additional information screen menu
 */
COMMAND additional_information_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y, MENU_AUTO,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders an entity info list screen
 */
void render_entity_info_list_screen(const char *title)
{

	render_secondary_screen("-== %s ==-", title);
}



/*
 * entity info list menu
 */
COMMAND entity_info_list_menu(MENU *menu)
{
	SCREEN_AREA menu_area = {
		SEC_SCREEN_MIN_Y, SEC_SCREEN_MAX_Y,
		SEC_SCREEN_MIN_X, MENU_AUTO
	};

	menu_set_appearance(menu, &menu_area);

	return menu_execute(menu, &menu_area);
}



/*
 * renders the list of career exits
 */
static void render_career_exits_at(SCREEN_COORD y, SCREEN_COORD x,
	const CAREER *career
)
{
	CAREER_EXIT_INDEX i;

	render_text_at(C_Header, y, x, "[Career Exits]");

	place_cursor_at(++y, x);

	for (i = 0; i < MAX_CAREER_EXITS; i++) {

		if (is_empty_string(career->exit[i])) {

			break;
		}

		render_text_at(C_Text, y, x, career->exit[i]);

		++y;
	}
}



/*
 * renders the career trappings
 */
static void render_trappings_at(SCREEN_COORD y, SCREEN_COORD x,
	const CAREER *career
)
{
	LIST_NODE *node;

	render_text_at(C_Header, y, x, "[Trappings]");

	for (node = career->trappings->head;
		node != NULL;
		node = node->next) {
		const OBJECT *object = node->data;

		render_text_at(C_Text, ++y, x, "%s", object->name);
	}
}



/*
 * renders equipment
 */
static void render_equipment_at(SCREEN_COORD y, SCREEN_COORD x,
	const CHARACTER *character
)
{
	LIST_NODE *node;

	render_text_at(C_Header, y, x, "[Equipment]");

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object;

		object = (OBJECT *)node->data;

		if (!object_in_use(character, object)) {

			continue;
		}

		++y;

		place_cursor_at(y, x);

		render_inventory_object(C_Text, character, object,
			false
		);
	}
}



/*
 * renders character perks
 */
static void render_perks_at(SCREEN_COORD start_y, SCREEN_COORD start_x,
	const bool *perks
)
{
	SCREEN_COORD y = start_y, x = start_x;
	PERK perk;

	render_text_at(C_Header, y++, x, "[Perks]");

	iterate_over_perks(perk) {

		if (!perks[perk]) continue;

		render_text_at(C_Text, y++, x, perk_name(perk));
	}
}



/*
 * renders career perks
 */
static void render_career_perks_at(SCREEN_COORD start_y, SCREEN_COORD start_x,
	const CAREER *career
)
{
	SCREEN_COORD y = start_y, x = start_x;
	const CAREER_PERK_INFO *perk_info = career->perk;
	PERK perk;

	render_text_at(C_Header, y++, x , "[Perks]");

	iterate_over_perks(perk) {

		/* perk not available */
		if (!perk_info[perk].available) continue;

		/* render perk name */
		render_text_at(C_Text, y++, x, perk_name(perk));
	}
}


/*
 * renders an advance scheme
 */
static void render_advance_scheme_at(SCREEN_COORD y, SCREEN_COORD x,
	const CAREER *career
)
{
	STAT stat;

	iterate_over_stats(stat) {

		render_text_at(C_FieldName, y++, x, "%-13s: ",
			stat_long_name(stat)
		);

		if (career->stat[stat].advance_maximum > 0) {

			render_text(C_FieldValue, "+%d",
				career->stat[stat].advance_maximum
			);
		}
	}
}



/*
 * renders psychic powers
 */
static void render_psy_powers(SCREEN_ID screen_id, const CHARACTER *character)
{
	SCREEN_COORD y = SEC_SCREEN_MIN_Y;
	SCREEN_COORD x = SEC_SCREEN_MIN_X;
	PSY_POWER power;

	if (screen_id != SID_CHARACTER) {

		die("*** CORE ERROR *** invalid render mode (pp)");
	}

	iterate_over_psy_powers(power) {

		if (!character_has_psy_power(character, power)) continue;

		render_text_at(C_Text, y++, x, "%s",
			psy_power_name(power)
		);
	}
}



/*
 * renders the close combat weapon specific parts of an object screen
 */
static void render_object_screen_ccw(const OBJECT *object)
{

	render_field("Damage       : ", dice_roll_string(&object->damage));

	render_field("Reach        : ", "%d", object->reach);

	if (!object_has_attribute(object, OA_CANNOT_PARRY)) {

		render_field("Parry Penalty: ", "%d", object->parry_penalty);
	}
}



/*
 * renders the ranged combat weapon specific parts of an object screen
 */
static void render_object_screen_rcw(const OBJECT *object)
{
	const FIRING_MODE_DATA * firing_mode = &object->firing_mode;

	render_field("Damage       : ", dice_roll_string(&object->damage));

	render_field("Shots        : ", "%d/%d",
		object->charge, object->charge_max
	);

	if (object->reload == RELOAD_NO) {
		render_field("Reload       : ", "No");
	} else {
		render_field("Reload       : ", "%d", object->reload);
	}

	render_field("Firing Mode  : ", firing_mode_string(firing_mode));

	if (firing_mode->accuracy != 0) {

		render_field("Accuracy Mod.: ", "%+d",
			firing_mode->accuracy
		);
	}
}



/*
 * firing mode -> string
 */
static const char * firing_mode_string(const FIRING_MODE_DATA *firing_mode)
{
	static char string[16];
	int n_modes = 0;

	clear_string(string);

	if (firing_mode->has[FMODE_S]) {

		strcat(string, firing_mode_name(FMODE_S));
		++n_modes;
	}

	if (firing_mode->has[FMODE_SA]) {

		if (n_modes > 0) string_cat(string, ",");

		if (firing_mode->min_sa_shots ==
			firing_mode->max_sa_shots) {

			string_cat(string, "%s(%d)",
				firing_mode_name(FMODE_SA),
				firing_mode->max_sa_shots
			);

		} else {

			string_cat(string, "%s(%d/%d)",
				firing_mode_name(FMODE_SA),
				firing_mode->min_sa_shots,
				firing_mode->max_sa_shots
			);
		}

		++n_modes;
	}

	if (firing_mode->has[FMODE_A]) {

		if (n_modes > 0) string_cat(string, ",");

		string_cat(string, "A(%d)", firing_mode->a_shots);
	}

	return string;
}



/*
 * renders the attributes of an object
 */
static void render_object_attributes_at(SCREEN_COORD row, SCREEN_COORD col,
	const OBJECT *object
)
{
	OBJECT_ATTRIBUTE i;

	render_text_at(C_Header, row, col, "[Attributes]");

	for (i = 0; i < MAX_OBJECT_ATTRIBUTES; i++) {

		if (!object_has_attribute(object, i)) {

			continue;
		}

		render_text_at(C_Text, ++row, col,
			object_attribute_name(i)
		);
	}
}


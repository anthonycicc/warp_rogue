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
 * Module: Character Files
 */

#include "wrogue.h"



static void		load_character_file(void);
static void		load_character_pp(CHARACTER *);

static void		read_gender_field(GENDER *);
static void		read_career_field(CHARACTER *);
static void		read_ai_state_field(CHARACTER *);
static void		read_stat_field(CHARACTER *, const char *);



/*
 * loads the character files
 */
void load_character_files(void)
{
	LIST *file_list = data_files(DIR_CHARACTERS);
	LIST_NODE *node;

	iterate_over_list(file_list, node) {
		const char *file_name = node->data;

		set_data_path(DIR_CHARACTERS, file_name);

		load_character_file();
	}

	list_free_with(file_list, free);

	character_box_optimise();
}



/*
 * loads a character file
 */
static void load_character_file(void)
{
	CHARACTER *character = NULL;

        rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			if (character != NULL) {

				load_character_pp(character);
			}

			character = character_box_new_character();

			read_name_field(character->name,
				CHARACTER_NAME_SIZE
			);
			
		} else if (strings_equal(field_name, "FACTION")) {

			character->faction = name_to_faction(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "NUMERUS")) {

			read_numerus_field(&character->numerus);

		} else if (strings_equal(field_name, "GENDER")) {

			read_gender_field(&character->gender);

		} else if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&character->symbol);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_colour_field(&character->colour);
		
		} else if (strings_equal(field_name, "POWER_LEVEL")) {
		
			character->power_level = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "CAREER")) {

			read_career_field(character);

		} else if (is_short_stat_name(field_name)) {

			read_stat_field(character, field_name);

		} else if (strings_equal(field_name, "OBJECT")) {
			OBJECT *object = read_object_field();

			inventory_add(character, object);

		} else if (strings_equal(field_name, "PERK")) {
			PERK perk = name_to_perk(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			give_perk(character, perk);

		} else if (strings_equal(field_name, "PSYCHIC_POWER")) {
			PSY_POWER power = name_to_psy_power(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			character_set_psy_power(character, power);

		} else if (strings_equal(field_name, "ATTRIBUTE")) {
			CHARACTER_ATTRIBUTE attribute;

			attribute = name_to_character_attribute(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			character_set_attribute(character, attribute);

		} else if (strings_equal(field_name, "AI_DEFAULT_STATE")) {

			read_ai_state_field(character);

		} else if (strings_equal(field_name, "SCRIPT")) {

			strcpy(character->script, 
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "DESCRIPTION")) {

			read_description_field(character->description, 
				character->name
			);

		} else {

			die("*** CORE ERROR *** invalid field name: %s (%s)",
				field_name,
				data_path()
			);
		}
        }

	if (character != NULL) {

		load_character_pp(character);
	}

	rdb_close();
}



/*
 * character loading post-processor
 */
static void load_character_pp(CHARACTER *character)
{

	equip_objects(character);

	if (character_has_perk(character, PK_STEALTH)) {

		activate_stealth(character);
	}
}



/*
 * reads gender field data
 */
static void read_gender_field(GENDER *gender)
{

	switch (rdb_data_token(0)[0]) {

	case 'M':
		*gender = GENDER_MALE;
		break;

	case 'F':
		*gender = GENDER_FEMALE;
		break;

	case 'N':
		*gender = GENDER_NEUTER;
		break;

	default:
		die("*** CORE ERROR *** invalid data in gender field: %c",
			rdb_data_token(0)[0]
		);
	}
}



/*
 * reads career field data
 */
static void read_career_field(CHARACTER *character)
{
	CAREER_INDEX career_index;

	career_index = name_to_career_index(
		rdb_data_token(RDB_NEXT_TOKEN)
	);

	career_set(character, career_index);
}



/*
 * reads AI state field data
 */
static void read_ai_state_field(CHARACTER *character)
{

	if (strings_equal(rdb_data_token(0), "Wait")) {

		character->ai.state =
			character->ai.default_state =
			character->ai.original_state =
			AI_STATE_WAIT;

	} else {

		die("*** CORE ERROR *** invalid data in " \
			"AI state field: %s", rdb_data_token(0)
		);
	}

}



/*
 * reads stat field data
 */
static void read_stat_field(CHARACTER *character, const char *field_name)
{
	STAT i;

	i = name_to_stat(field_name, true);

	character->stat[i].current = character->stat[i].total =
		atoi(rdb_data_token(0));

	if (rdb_optional_data_token(1) != NULL) {

		character->stat[i].advance = atoi(rdb_data_token(1));
	}
}


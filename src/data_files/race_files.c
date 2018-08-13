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
 * Module: Race Files
 */

#include "wrogue.h"


static void		load_profile_file(void);
static void		load_career_files(void);
static void		load_career_file(void);

static void		read_career_exit_field(CAREER *, CAREER_EXIT_INDEX);




/*
 * loads the race files
 */
void load_race_files(void)
{

	load_profile_file();

	load_career_files();
}



/*
 * loads the profile file
 */
static void load_profile_file(void)
{
	RACE *race = player_race();

	set_data_path(DIR_RACES, FILE_RACE_PROFILE);

	rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&race->symbol);

			strcpy(planet()->name,
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (is_short_stat_name(field_name)) {
			STAT i;

			i = name_to_stat(field_name, true);

			read_dice_roll_field(&race->stat[i]);

		} else {

			die("*** CORE ERROR *** invalid field name: %s (%s)",
				field_name,
				data_path()
			);
		}
	}

	rdb_close();
}



/*
 * loads the career files
 */
static void load_career_files(void)
{

	set_data_path(DIR_RACES, FILE_CAREERS);
	load_career_file();

	career_box_optimise();

#if defined(DEBUG)
	career_box_validate();
#endif

}



/*
 * loads a career file
 */
static void load_career_file(void)
{
	CAREER *career = NULL;
	CAREER_EXIT_INDEX career_exit_index = 0;
	SERIAL sn = 0;

	rdb_open(data_path(), RDB_READ);

        while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			career = career_box_new_career();

			career->sn = sn++;

			career_exit_index = 0;

			read_name_field(career->name,
				CAREER_NAME_SIZE
			);

		} else if (strings_equal(field_name, "TYPE")) {

			career->type = name_to_career_type(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_colour_field(&career->colour);

		} else if (is_short_stat_name(field_name)) {
			STAT i = name_to_stat(field_name, true);

			career->stat[i].advance_maximum = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);
			
			career->stat[i].required_minimum = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "PERK")) {
			PERK perk;
			bool required = false;

			perk = name_to_perk(rdb_data_token(0));

			/*
			 * second data token exists AND
			 * its value is 'Required' ..
			 */
			if (rdb_optional_data_token(1) != NULL &&
				strings_equal(rdb_data_token(1),
					"Required")) {

				/* perk is required */
				required = true;
			}

			career_set_perk(career, perk, required);

		} else if (strings_equal(field_name, "OBJECT")) {
			OBJECT *object;

			object = read_object_field();

			list_add(career->trappings, object);

		} else if (strings_equal(field_name, "EXIT")) {

			read_career_exit_field(career, career_exit_index);

			++career_exit_index;

		} else if (strings_equal(field_name, "DESCRIPTION")) {

			read_description_field(career->description,
				career->name
			);

		} else {

			die("*** CORE ERROR *** invalid field name: %s (%s)",
				field_name,
				data_path()
			);
		}
        }

        rdb_close();
}



/*
 * reads career exit field data
 */
static void read_career_exit_field(CAREER *career,
	CAREER_EXIT_INDEX career_exit_index
)
{
	const char *career_name;

	if (career_exit_index >= MAX_CAREER_EXITS) {

		die("*** CORE ERROR *** The career %s has "
		"too many exits", career->name);
	}

	career_name = rdb_data_token(RDB_NEXT_TOKEN);

	strcpy(career->exit[career_exit_index], career_name);
}



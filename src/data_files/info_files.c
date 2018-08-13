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
 * Module: Info Files
 */

#include "wrogue.h"



static void		load_scenario_info_file(void);
static char *		data_file_entry_description(char *, const char *);



/*
 * loads the info files
 */
void load_info_files(void)
{

	load_scenario_info_file();
}



/*
 * returns the description of a character attribute
 */
char * data_file_character_attribute_description(char *description,
	CHARACTER_ATTRIBUTE attribute
)
{
	clear_string(description);

	set_data_path(DIR_HELP, FILE_CHARACTER_ATTRIBUTES);

	data_file_entry_description(description,
		character_attribute_name(attribute)
	);

	return description;
}



/*
 * returns the description of an object attribute
 */
char * data_file_object_attribute_description(char *description,
	OBJECT_ATTRIBUTE attribute
)
{
	clear_string(description);

	set_data_path(DIR_HELP, FILE_OBJECT_ATTRIBUTES);

	data_file_entry_description(description,
		object_attribute_name(attribute)
	);

	return description;
}



/*
 * returns the description of a terrain attribute
 */
char * data_file_terrain_attribute_description(char *description,
	TERRAIN_ATTRIBUTE attribute
)
{
	clear_string(description);

	set_data_path(DIR_HELP, FILE_TERRAIN_ATTRIBUTES);

	data_file_entry_description(description,
		terrain_attribute_name(attribute)
	);

	return description;
}



/*
 * returns the description of a psychic power
 */
char * data_file_psy_power_description(char *description, PSY_POWER power)
{
	clear_string(description);

	set_data_path(DIR_HELP, FILE_PSY_POWERS);

	data_file_entry_description(description, psy_power_name(power));

	return description;
}



/*
 * returns the description of a perk
 */
char * data_file_perk_description(char *description, PERK perk)
{
	clear_string(description);

	set_data_path(DIR_HELP, FILE_PERKS);

	data_file_entry_description(description, perk_name(perk));

	return description;
}



/*
 * loads the scenario info file
 */
static void load_scenario_info_file(void)
{

	set_data_path(DIR_INFO, FILE_SCENARIO_INFO);

	rdb_open(data_path(), RDB_READ);

        while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			strcpy(scenario_name(),
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "VERSION")) {

			strcpy(scenario_version(),
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else {

			die("*** CORE ERROR *** invalid field name: " \
				"%s (%s)",
				field_name,
				data_path()
			);
		}
	}

	rdb_close();
}



/*
 * returns the description of a data file entry
 */
static char * data_file_entry_description(char *description,
	const char *entry_name
)
{
	bool entry_reached = false;

	rdb_open(data_path(), RDB_READ);

	clear_string(description);

	while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			if (entry_reached) break;

			if (strings_equal(rdb_data_token(RDB_NEXT_TOKEN),
				entry_name)) {

				entry_reached = true;
			}
			continue;
		}

		if (entry_reached &&
			strings_equal(field_name, "DESCRIPTION")) {

			read_description_field(description, entry_name);
		}
	}

	rdb_close();

	return description;
}



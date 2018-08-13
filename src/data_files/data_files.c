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
 * Module: Data Files
 */

#include "wrogue.h"



/*
 * the path of the currently processed file
 */
static char		DataPath[FILE_PATH_SIZE];





/*
 * sets the data path
 */
void set_data_path(PROGRAM_DIRECTORY directory, const char *file_name)
{

	get_file_path(DataPath, directory, file_name);
}



/*
 * returns the data path
 */
const char * data_path(void)
{

	return DataPath;
}



/*
 * loads the data files
 */
void load_data_files(void)
{

	load_ui_files();
	
	load_rule_files();

	load_info_files();
	
	load_terrain_files();

	load_object_files();

	load_race_files();

	load_character_files();

	load_planet_files();
}





/*
 * reads symbol field data
 */
void read_symbol_field(SYMBOL *symbol)
{
	const char *data_token;

	data_token = rdb_data_token(0);

	if (data_token[1] != '\0') {

		*symbol = name_to_symbol(data_token);

	} else {

		*symbol = data_token[0];
	}
}



/*
 * reads colour field data
 */
void read_colour_field(COLOUR *colour)
{

	*colour = name_to_colour(rdb_data_token(RDB_NEXT_TOKEN));
}



/*
 * reads dynamic colour field data
 */
void read_dynamic_colour_field(DYNAMIC_COLOUR *colour)
{

	colour->primary = name_to_colour(rdb_data_token(0));

	if (rdb_optional_data_token(1) == NULL) {

		colour->secondary = C_NIL;

		colour->frequency = 0;

		return;
	}

	colour->secondary = name_to_colour(rdb_data_token(1));

	colour->frequency = atoi(rdb_data_token(2));
}




/*
 * reads boolean field data
 */
void read_boolean_field(bool *data)
{

	if (strings_equal(rdb_data_token(0), "Yes")) {

		*data = true;

	} else if (strings_equal(rdb_data_token(0), "No")) {

		*data = false;

	} else {

		die("*** CORE ERROR *** invalid data in " \
			"boolean field: %s", rdb_data_token(0)
		);
	}

}



/*
 * reads name field data
 */
void read_name_field(char *name, unsigned int n)
{

	if (strlen(rdb_data_token(0)) < n) {

		strcpy(name, rdb_data_token(0));

		return;
	}

	die("*** CORE ERROR *** name too long: %s " \
		"(limit: %d characters)",
		rdb_data_token(0), n - 1
	);
}



/*
 * reads numerus field data
 */
void read_numerus_field(NUMERUS *numerus)
{
	const char *string;

	string = rdb_data_token(0);

	if (strings_equal(string, "Singular")) {

		*numerus = NUMERUS_SINGULAR;

	} else if (strings_equal(string, "Plural")) {

		*numerus = NUMERUS_PLURAL;

	} else {

		die("*** CORE ERROR *** invalid numerus field value");
	}
}



/*
 * reads object field data
 */
OBJECT * read_object_field(void)
{
	OBJECT *object;

	object = object_create(rdb_data_token(0));

	if (rdb_optional_data_token(1) == NULL) {

		return object;
	}

	object->charge = atoi(rdb_data_token(1));

	return object;
}



/*
 * reads description field data
 */
void read_description_field(char *description, const char *entry_name)
{

#if defined(DEBUG)
	char text_buffer[TEXT_BUFFER_SIZE];

	strcpy(text_buffer, description);
	strcat(text_buffer, rdb_data_token(RDB_NEXT_TOKEN));

	if (strlen(text_buffer) >= DESCRIPTION_SIZE) {

		die("*** CORE ERROR *** description of %s in %s "\
			"is too long (limit: %d characters)",
			entry_name, data_path(), DESCRIPTION_SIZE - 1
		);
	}

	strcpy(description, text_buffer);
#else

	strcat(description, rdb_data_token(RDB_NEXT_TOKEN));
#endif

}



/*
 * reads dice roll field data
 */
void read_dice_roll_field(DICE_ROLL *roll)
{

	roll->n_dice = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	roll->n_sides = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	if (rdb_optional_data_token(2) == NULL) {

		roll->modifier = 0;

		return;
	}

	roll->modifier = atoi(rdb_data_token(2));
}



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
 * Module: UI Files
 */

#include "wrogue.h"



static void		load_keybindings(void);
static void		load_colour_file(void);
static void		load_gore_file(void);

static void		read_key_field(KEY_CODE *);
static void		read_rgb_field(RGB_DATA *);






/*
 * loads the UI files
 */
void load_ui_files(void)
{

	load_keybindings();

	load_colour_file();

	load_gore_file();
}



/*
 * loads the keybindings
 */
static void load_keybindings(void)
{
	COMMAND command = CM_NIL;

	set_data_path(DIR_UI, FILE_KEYBINDINGS);

	rdb_open(data_path(), RDB_READ);

        while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "COMMAND")) {

			command = name_to_command(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "KEY")) {
			KEY_CODE key;

			read_key_field(&key);

			set_command_key(command, key);

		} else {

			die("*** CORE ERROR *** invalid field " \
				"name: %s (%s)",
				field_name,
				data_path()
			);
		}

	}

	rdb_close();

}




/*
 * loads the colour file
 */
static void load_colour_file(void)
{
	COLOUR_DATA *colour_data;

	set_data_path(DIR_UI, FILE_COLOUR);

	rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "COLOUR")) {

			colour_data = colour_new();

			strcpy(colour_data->name,
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			read_rgb_field(&colour_data->rgb);

		} else if (strings_equal(field_name, "UI")) {
			COLOUR *colour_ptr;

			colour_ptr = ui_colour(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			*colour_ptr = name_to_colour(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "CHARACTER_ATTRIBUTE")) {
			CHARACTER_ATTRIBUTE attribute;
			COLOUR colour;

			attribute = name_to_character_attribute(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			read_colour_field(&colour);

			character_attribute_set_colour(attribute, colour);

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
 * loads the gore file
 */
static void load_gore_file(void)
{
	GORE_FEATURE *feature = NULL;

	set_data_path(DIR_UI, FILE_GORE);

	rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "FEATURE")) {
			const char *feature_name =
				rdb_data_token(RDB_NEXT_TOKEN);

			feature = gore_feature(feature_name);

		} else if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&feature->symbol);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_colour_field(&feature->colour);

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
 * reads key field data
 */
static void read_key_field(KEY_CODE *key)
{

	*key = name_to_key_code(rdb_data_token(RDB_NEXT_TOKEN));
}



/*
 * reads RGB field data
 */
static void read_rgb_field(RGB_DATA *rgb_data)
{
	const char *hex_code;

	hex_code = rdb_data_token(RDB_NEXT_TOKEN);

	rgb_data->red = hex_byte_to_decimal(hex_code);
	rgb_data->green = hex_byte_to_decimal(&hex_code[2]);
	rgb_data->blue = hex_byte_to_decimal(&hex_code[4]);
}



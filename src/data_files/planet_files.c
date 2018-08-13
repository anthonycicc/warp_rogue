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
 * Module: Planet Files
 */

#include "wrogue.h"


static void		load_planet_file(void);

static void		read_planet_map_field(PLANET_POINT *);



/*
 * loads the planet files
 */
void load_planet_files(void)
{

	load_planet_file();
}




/*
 * loads the planet file
 */
static void load_planet_file(void)
{
	PLANET_POINT current_position = {0, 0, 0};
	PLANET_TILE *tile = NULL;
	bool planet_name_read = false;

	set_data_path(DIR_PLANETS, FILE_PLANET);

	rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			if (!planet_name_read) {

				strcpy(planet()->name,
					rdb_data_token(RDB_NEXT_TOKEN)
				);

				planet_name_read = true;

				continue;
			}

			strcpy(tile->name,
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "DESCRIPTION")) {

			strcat(planet()->description,
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "TILE")) {

			tile = planet_tile_box_new_tile();

			tile->screen_symbol = tile->symbol =
				rdb_data_token(RDB_NEXT_TOKEN)[0];

		} else if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&tile->screen_symbol);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_dynamic_colour_field(&tile->colour);

		} else if (strings_equal(field_name, "IMPASSABLE")) {

			read_boolean_field(&tile->impassable);

		} else if (strings_equal(field_name, "N_LEVELS")) {

			tile->n_levels = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "SCRIPT")) {

			strcpy(tile->script,
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "M")) {

			read_planet_map_field(&current_position);

		} else {

			die("*** CORE ERROR *** invalid field name: %s (%s)",
				field_name,
				data_path()
			);
		}

	}

	rdb_close();

	planet_tile_box_optimise();
}



/*
 * reads planet map field data
 */
static void read_planet_map_field(PLANET_POINT *current_position)
{
	const char *map_line;

	map_line = rdb_data_token(RDB_NEXT_TOKEN);

	current_position->x = 0;

	while (*map_line != '\0') {
		PLANET_TILE *tile;

		tile = planet_tile(current_position);

		*tile = *symbol_to_planet_tile(*map_line);

		determine_colour(&tile->colour);

		++map_line;
		++current_position->x;
	}

	++current_position->y;
}


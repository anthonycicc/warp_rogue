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
 * Module: Terrain Files
 */

#include "wrogue.h"


static void		load_terrain_file(void);



/*
 * loads the terrain files
 */
void load_terrain_files(void)
{
	LIST *file_list;
	LIST_NODE *node;

	file_list = data_files(DIR_TERRAIN);

	for (node = file_list->head; node != NULL; node = node->next) {
		const char *file_name;

		file_name = (const char *)node->data;

		set_data_path(DIR_TERRAIN, file_name);

		load_terrain_file();
	}

	list_free_with(file_list, free);

	terrain_box_optimise();
}



/*
 * loads a terrain file
 */
static void load_terrain_file(void)
{
	TERRAIN *terrain = NULL;

	rdb_open(data_path(), RDB_READ);

        while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			terrain = terrain_box_new_terrain();

			read_name_field(terrain->name,
				TERRAIN_NAME_SIZE
			);

		} else if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&terrain->symbol);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_dynamic_colour_field(&terrain->colour);

		} else if (strings_equal(field_name, "ATTRIBUTE")) {
			TERRAIN_ATTRIBUTE attribute;

			attribute = name_to_terrain_attribute(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			terrain_set_attribute(terrain, attribute);

		} else if (strings_equal(field_name, "DESCRIPTION")) {

			read_description_field(terrain->description,
				terrain->name
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



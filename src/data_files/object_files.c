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
 * Module: Object Files
 */

#include "wrogue.h"



static void		load_object_file(void);

static void		read_firing_mode_field(OBJECT *);
static void		read_reload_field(RELOAD_VALUE *);




/*
 * loads the object files
 */
void load_object_files(void)
{
	LIST *file_list;
	LIST_NODE *node;

	file_list = data_files(DIR_OBJECTS);

	for (node = file_list->head; node != NULL; node = node->next) {
		const char *file_name;

		file_name = (const char *)node->data;

		set_data_path(DIR_OBJECTS, file_name);

		load_object_file();
	}

	list_free_with(file_list, free);

	object_box_optimise();
}



/*
 * loads an object file
 */
static void load_object_file(void)
{
	OBJECT *object = NULL;

	rdb_open(data_path(), RDB_READ);

        while (rdb_next_field()) {
		const char *field_name;

                field_name = rdb_field_name();

		if (strings_equal(field_name, "NAME")) {

			object = object_box_new_object();

			read_name_field(object->name,
				OBJECT_NAME_SIZE
			);

		} else if (strings_equal(field_name, "NUMERUS")) {

			read_numerus_field(&object->numerus);

		} else if (strings_equal(field_name, "SYMBOL")) {

			read_symbol_field(&object->symbol);

		} else if (strings_equal(field_name, "COLOUR")) {

			read_dynamic_colour_field(&object->colour);

		} else if (strings_equal(field_name, "TYPE")) {

			object->type = name_to_object_type(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "SUBTYPE")) {

			object->subtype = name_to_object_subtype(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "CONDITION")) {

			object->condition = object->condition_max = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "VALUE")) {

			object->value = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "WEIGHT")) {

			object->weight = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "DAMAGE")) {

			read_dice_roll_field(&object->damage);

		} else if (strings_equal(field_name, "ARMOUR_VALUE")) {

			object->armour_value = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "ACCURACY")) {

			object->firing_mode.accuracy = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "SHOTS")) {

			object->charge = object->charge_max = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "FIRING_MODE")) {

			read_firing_mode_field(object);

		} else if (strings_equal(field_name, "RELOAD")) {

			read_reload_field(&object->reload);

		} else if (strings_equal(field_name, "ATTRIBUTE")) {
			OBJECT_ATTRIBUTE attribute;

			attribute = name_to_object_attribute(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			object->attribute[attribute] = true;

		} else if (strings_equal(field_name, "REACH")) {

			object->reach = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "PARRY_PENALTY")) {

			object->parry_penalty = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

		} else if (strings_equal(field_name, "DESCRIPTION")) {

			read_description_field(object->description,
				object->name
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
 * reads firing mode field data
 */
static void read_firing_mode_field(OBJECT *object)
{
	FIRING_MODE firing_mode;

	firing_mode = name_to_firing_mode(rdb_data_token(RDB_NEXT_TOKEN));

	object->firing_mode.has[firing_mode] = true;

	if (firing_mode == FMODE_S) {

		return;
	}

	if (firing_mode == FMODE_SA) {

		object->firing_mode.min_sa_shots =
			atoi(rdb_data_token(RDB_NEXT_TOKEN));

		object->firing_mode.max_sa_shots =
			atoi(rdb_data_token(RDB_NEXT_TOKEN));

	} else if (firing_mode == FMODE_A) {

		object->firing_mode.a_shots =
			atoi(rdb_data_token(RDB_NEXT_TOKEN));
	}
}



/*
 * reads reload field data
 */
static void read_reload_field(RELOAD_VALUE *reload_value)
{
	const char *data_token;

	data_token = rdb_data_token(RDB_NEXT_TOKEN);

	if (strings_equal(data_token, "No")) {

		*reload_value = RELOAD_NO;

	} else {

		*reload_value = atoi(data_token);
	}
}





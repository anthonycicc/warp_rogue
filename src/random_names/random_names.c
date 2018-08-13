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
 * Module: Random Names
 */

#include "wrogue.h"

#include "shake.h"





/* 
 * Random Names module init
 */
void random_names_init(void)
{
	
	shake_set_rng(random_int);

	set_data_path(DIR_RACES, FILE_NAMES_MALE);	
	shake_load_set(NAME_SET_MALE, data_path());

	shake_no_duplicates(NAME_SET_MALE, true);
	
	set_data_path(DIR_RACES, FILE_NAMES_FEMALE);	
	shake_load_set(NAME_SET_FEMALE, data_path());

	shake_no_duplicates(NAME_SET_FEMALE, true);
}




/*
 * Random Names module clean up
 */
void random_names_clean_up(void)
{

	shake_unload_set(NAME_SET_MALE);

	shake_unload_set(NAME_SET_FEMALE);
}



/*
 * returns a random character name 
 */
const char * random_name(NAME_SET name_set)
{
	const char *name;
	
	name = shake_get_string(name_set);
	
	if (name == NULL) {

		shake_no_duplicates(name_set, false);

		name = shake_get_string(name_set);
		
		if (name == NULL) {

			die("*** CORE ERROR *** name set %d empty",
				name_set
			);
		}
	}
	
	return name;
}





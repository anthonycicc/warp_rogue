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
 * Module: Rule Files
 */

#include "wrogue.h"


static void		load_factions_file(void);




/*
 * loads the rule files
 */
void load_rule_files(void)
{

	load_factions_file();
}





/*
 * loads the factions file
 */
static void load_factions_file(void)
{
	set_data_path(DIR_RULES, FILE_FACTIONS);

	rdb_open(data_path(), RDB_READ);

	while (rdb_next_field()) {
		const char *field_name = rdb_field_name();		
		
		if (strings_equal(field_name, "FACTION")) {
			
			faction_add(rdb_data_token(RDB_NEXT_TOKEN));

		} else {

			die("*** CORE ERROR *** invalid field name: %s (%s)",
				field_name,
				data_path()
			);
		}
	}

	rdb_close();

	factions_finalise();
}



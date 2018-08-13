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
 * Module: Galaxy
 */

#include "wrogue.h"



static void	execute_dungeon_script(const PLANET_TILE *,
			const PLANET_POINT *
		);



/*
 * galaxy generation
 */
void galaxy_generation(void)
{
	PLANET_POINT p;

	command_bar_clear();

	render_galaxy_generation_screen(false);
	update_screen();

	for (p.y = 0; p.y < PLANET_HEIGHT; p.y++) {
	for (p.x = 0; p.x < PLANET_WIDTH; p.x++) {
		const PLANET_TILE *tile = planet_tile(&p);

		if (is_empty_string(tile->script)) continue;

		execute_dungeon_script(tile, &p);
	}
	}
}



/*
 * executes a dungeon script
 */
static void execute_dungeon_script(const PLANET_TILE *tile,
	const PLANET_POINT *location
)
{

	AREA *area;
	PLANET_COORD z;

	script_load(DIR_DUNGEONS, tile->script);
	script_set_data("GENERATION", "Yes");

	area = active_area();

	for (z = 0; z < tile->n_levels; z++) {

		strcpy(area->name, tile->name);
		area->location = *location;
		area->location.z = z;

		script_set_data_numeric("LEVEL", z);
		script_execute();

		area_write(area_file_name(&area->location));

		area_clear();
	}
}



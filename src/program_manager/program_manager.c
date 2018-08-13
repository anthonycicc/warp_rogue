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
 * Module: Program Manager
 */

#include "wrogue.h"

#include "cellpnt.h"



static void		lib_init(void);
static void		lib_clean_up(void);




/*
 * initializes the program
 */
void program_init(void)
{

	lib_init();

	platform_init();

	log_init();

	rng_init();

	randomiser_init();

	random_names_init();

	planet_init();

	area_init();

	pathfinder_init();
	
	faction_init();

	terrain_init();

	object_init();

	character_init();

	career_init();

	party_init();

	quest_init();

	script_init();

	load_data_files();

	ui_init();

	macro_init();
}



/*
 * program shutdown
 */
void program_shutdown(void)
{

	macro_clean_up();

	ui_clean_up();

	script_clean_up();

	quest_clean_up();

	party_clean_up();

	career_clean_up();

	character_clean_up();

	object_clean_up();

	terrain_clean_up();
	
	faction_clean_up();

	pathfinder_clean_up();

	area_clean_up();

	planet_clean_up();

	random_names_clean_up();

	randomiser_clean_up();

	rng_clean_up();

	log_clean_up();

	platform_clean_up();

	lib_clean_up();

	exit(EXIT_SUCCESS);
}



/*
 * aborts the game in case of an internal program error
 */
void die(const char *fmt, ...)
{
	va_list vl;
	static char buffer[STRING_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);

	log_output(buffer);

	exit(EXIT_FAILURE);
}



/*
 * aborts the game in case of an internal program error
 * (no data path version)
 */
void die_no_data_path(const char *fmt, ...)
{
	va_list vl;
	static char buffer[STRING_BUFFER_SIZE];

	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);

	fprintf(stderr, buffer);

	exit(EXIT_FAILURE);
}



/*
 * initialise libraries
 */
static void lib_init(void)
{

	cpn_set_rng(random_int);
	ccd_set_rng(random_int);
	rdb_set_error_handler(die);
}



/*
 * libraries clean up
 */
static void lib_clean_up(void)
{

	/* NOTHING TO DO */
}



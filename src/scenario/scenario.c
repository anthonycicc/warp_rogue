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
 * Module: Scenario
 */

#include "wrogue.h"




/*
 * scenario name
 */
static char			ScenarioName[SCENARIO_NAME_SIZE] = "";



/*
 * scenario version
 */
static char			ScenarioVersion[SCENARIO_VERSION_SIZE] = "";





/*
 * returns the name of the scenario
 */
char * scenario_name(void)
{

	return ScenarioName;
}



/*
 * returns the version of the scenario
 */
char * scenario_version(void)
{

	return ScenarioVersion;
}



/*
 * the scenario intro
 */
void scenario_intro(void)
{

	show_text_file("Intro", DIR_INFO, FILE_SCENARIO_INTRO, true);
}



/*
 * the scenario end
 */
void scenario_end(const char *msg)
{

	message(SID_GAME, "%s Press Enter.", msg);
	render_game_screen();
	update_screen();

	while (get_key() != KEY_ENTER)
		/* DO NOTHING */;

	certificate_screen();

	saved_game_erase();

	program_shutdown();
}



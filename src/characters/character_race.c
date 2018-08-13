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
 * Module: Character Race
 */

#include "wrogue.h"



/*
 * the player character's race
 */
static RACE		PlayerRace;





/*
 * returns a pointer to the player character's race
 */
RACE * player_race(void)
{

	return &PlayerRace;
}







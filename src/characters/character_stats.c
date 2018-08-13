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
 * Module: Character Stats
 */

#include "wrogue.h"





/*
 * stat names
 */
static const STAT_NAME StatName[MAX_STATS] = {

	{"CC",	"Close Combat"},
	{"RC",	"Ranged Combat"},
	{"ST",	"Strength"},
	{"TN",	"Toughness"},
	{"AG",	"Agility"},
	{"IN",	"Intelligence"},
	{"WP",	"Will Power"},
	{"FL",	"Fellowship"}

};




/*
 * stat advance
 */
void stat_advance(CHARACTER *character, STAT i)
{
	CHARACTER_STAT *stat;

	stat = &character->stat[i];

	stat->current += STANDARD_STAT_ADVANCE;
	stat->total += STANDARD_STAT_ADVANCE;
	stat->advance += STANDARD_STAT_ADVANCE;
}



/*
 * returns the stat bonus modifier
 */
STAT_BONUS stat_bonus(const CHARACTER *character, STAT stat)
{
	STAT_BONUS bonus;

	bonus = (character->stat[stat].current - STAT_BONUS_BASE) / 10;

	if (bonus < 0) {

		bonus = 0;
	}

	return bonus;
}



/*
 * applies stat modifiers
 */
void stat_modifiers_apply(CHARACTER *character, STAT_MODIFIER *stat_modifier)
{
	STAT stat;

	iterate_over_stats(stat) {

		character->stat[stat].current += stat_modifier[stat];
	}
}



/*
 * reverts stat modifiers
 */
void stat_modifiers_revert(CHARACTER *character,
	const STAT_MODIFIER *stat_modifier, STAT_MODIFIER restore_max
)
{
	STAT stat;

	iterate_over_stats(stat) {
		STAT_MODIFIER revert_modifier;

		revert_modifier = -stat_modifier[stat];

		if (revert_modifier > 0 &&
			revert_modifier > restore_max) {


			character->stat[stat].total -=
				(revert_modifier - restore_max);

			revert_modifier = restore_max;
		}

		character->stat[stat].current += revert_modifier;
	}
}



/*
 * returns the abbreviated name of a stat
 */
const char * stat_short_name(STAT stat)
{

	return StatName[stat].short_name;
}



/*
 * returns the full name of a stat
 */
const char * stat_long_name(STAT stat)
{

	return StatName[stat].long_name;
}



/*
 * returns true if the passed string is an abbreviated stat name
 */
bool is_short_stat_name(const char *name)
{
	STAT stat;

	iterate_over_stats(stat) {

		if (strings_equal(name, StatName[stat].short_name)) {

			return true;
		}
	}

	return false;
}



/*
 * name -> stat index
 */
STAT name_to_stat(const char *name, bool abbreviated)
{
	STAT stat;
	
	iterate_over_stats(stat) {

		if (abbreviated &&
			strings_equal(name, StatName[stat].short_name)) {

			return stat;

		} else {

			if (strings_equal(name, StatName[stat].long_name)) {

				return stat;
			}
		}
	}

	die("*** CORE ERROR *** invalid character stat: %s", name);

	/* never reached */
	return S_NIL;
}





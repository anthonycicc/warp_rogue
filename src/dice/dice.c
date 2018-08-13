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
 * Module: Dice
 */

#include "wrogue.h"






/*
 * rolls dice and returns the result
 */
int dice(int n_dice, int n_sides)
{
	int i;
	int result;

	result = 0;

	for (i = 0; i < n_dice; i++) {

		result += random_int(1, n_sides);
	}

	return result;
}



/*
 * does a dice roll
 */
int dice_roll(const DICE_ROLL *roll)
{

	return dice(roll->n_dice, roll->n_sides) + roll->modifier;
}



/*
 * returns the average result of a dice roll
 */
int dice_roll_average(const DICE_ROLL *roll)
{
	int result;

	result = roll->n_sides / 2;
	result *= roll->n_dice;
	result += roll->modifier;

	return result;
}



/*
 * D100 test
 */
int d100_test(int roll, int test_value)
{
	int result;

	if (test_value < 0) {

		test_value = -1;
	}

	result = test_value - roll;

	if (roll <= D100_AUTOMATIC_SUCCESS) {

		if (result >= 0) {

			return result;
		}

		return 0;

	} else if (roll >= D100_AUTOMATIC_FAILURE) {

		if (result < 0) {

			return result;
		}

		return -1;
        }

	return result;
}



/*
 * D100 test against the given test value
 * returns true if passed
 */
bool d100_test_passed(int test_value)
{
	int roll;

	roll = d100();

	if (d100_test(roll, test_value) >= 0) {

		return true;
	}

	return false;
}




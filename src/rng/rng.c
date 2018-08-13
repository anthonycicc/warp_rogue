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
 * Module: RNG
 */

#include "wrogue.h"

#include "tt.h"



/* 
 * RNG module init
 */
void rng_init(void)
{
	
	tt_seed(time(NULL));
}




/* 
 * RNG module clean up
 */
void rng_clean_up(void)
{
	
	/* NOTHING TO DO */
}




/*
 * returns a random integer >= a and <= b
 */
int random_int(int a, int b)
{

	return tt_random_int(a, b);
}


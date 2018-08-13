/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Random number generator (True Twister)
 *
 *
 * NAMESPACE: TT_* / tt_* / mt_*
 *
 */


void		tt_seed(unsigned long);

unsigned long	tt_get_seed(void);


int		tt_random_int(int, int);


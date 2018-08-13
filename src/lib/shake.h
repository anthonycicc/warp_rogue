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
 * Shake
 *
 * NAMESPACE: SHAKE_* / shake_*
 *
 */

typedef int	SHAKE_SET_ID;


bool		shake_load_set(SHAKE_SET_ID, const char *);

void		shake_unload_set(SHAKE_SET_ID);

void		shake_set_rng(int (*)(int, int));

void		shake_no_duplicates(SHAKE_SET_ID, bool);

const char *	shake_get_string(SHAKE_SET_ID);





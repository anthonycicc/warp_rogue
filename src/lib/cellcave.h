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
 * Cell Cave Dungeon Builder
 *
 *
 * NAMESPACE: CCD_* / ccd_*
 *
 */
 
 
/*
 * dungeon tiles
 */
#define CCD_TILE_FLOOR			0
#define CCD_TILE_WALL			1


typedef int				CCD_TILE;
typedef int				CCD_SIZE;
typedef int				CCD_PROBABILITY;
typedef int				CCD_N_GENERATIONS;
typedef int				CCD_PARAMETER;



/*
 * dungeon types
 */
typedef enum {

	CCD_DT_NIL = -1,
	
	CCD_DT_CAVE,
	CCD_DT_TUNNELS,
	
	CCD_DT_OUT_OF_BOUNDS

} CCD_DUNGEON_TYPE;



/*
 * parameters structure
 */
typedef struct ccd_parameters_struct {

	CCD_PARAMETER		r1_cutoff;
	CCD_PARAMETER		r2_cutoff;
	
	CCD_PARAMETER		reps;

} CCD_PARAMETERS;
 


/*
 * dungeon structure
 */
typedef struct {

	CCD_SIZE		height;
	CCD_SIZE		width;
	
	CCD_PROBABILITY		wall_probability;
	CCD_N_GENERATIONS	n_generations;
	
	CCD_PARAMETERS *	parameters;

	CCD_TILE **		map;
	CCD_TILE **		map2;

} CCD_DUNGEON;







CCD_DUNGEON *	ccd_build_dungeon(CCD_SIZE, CCD_SIZE, CCD_DUNGEON_TYPE);

void		ccd_free_dungeon(CCD_DUNGEON *);

void		ccd_set_rng(int(*)(int, int));




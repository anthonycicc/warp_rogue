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
 *
 * Based on an article by Jim Babcock. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "cellcave.h"



#define CCD_CAVE_WALL_PROBABILITY		40
#define CCD_CAVE_N_GENERATIONS			2

#define CCD_TUNNELS_WALL_PROBABILITY		45
#define CCD_TUNNELS_N_GENERATIONS		2


#define CCD_VISITED				128


/* #define CCD_STAND_ALONE */


typedef int		CCD_COORD;
typedef int		CCD_GENERATION;
typedef int		CCD_N_TILES;



static void *		ccd_malloc(size_t);

static void		ccd_build_cave(void);

static CCD_TILE		ccd_random_pick(void);

static CCD_DUNGEON *	ccd_new_dungeon(CCD_SIZE, CCD_SIZE, 
				CCD_DUNGEON_TYPE
			);

static void		ccd_init_map(void);

static void		ccd_generation(const CCD_PARAMETERS *parameters);

static int		ccd_default_random_int(int, int);

static bool		ccd_full_access(void);
static void		ccd_flood_fill(int *, int, int);
static void		ccd_random_floor_tile(int *, int *);
static int		ccd_n_accessible_tiles(void);
static int		ccd_n_passable_tiles(void);



/*
 * RNG function
 */
static int		(*ccd_random_int)(int, int) = ccd_default_random_int;



/*
 * the currently processed dungeon
 */
static CCD_DUNGEON *	CCD_Dungeon = NULL;



/*
 * parameters for cave type
 */
static const CCD_PARAMETERS 
	CCD_CaveParameters[CCD_CAVE_N_GENERATIONS] = {

	{5, 2, 4}, 
	{5, -1, 3}
	
};



/*
 * parameters for tunnels type
 */
static const CCD_PARAMETERS 
	CCD_TunnelsParameters[CCD_TUNNELS_N_GENERATIONS] = {
	
	{5, -1, 5}, 
	{5, 0, 1}
	
};





/*
 * builds a dungeon
 */
CCD_DUNGEON * ccd_build_dungeon(CCD_SIZE height, CCD_SIZE width, 
	CCD_DUNGEON_TYPE type
)
{

	CCD_Dungeon = ccd_new_dungeon(height, width, type);
	
	do {

		ccd_build_cave();

	} while (!ccd_full_access());

	return CCD_Dungeon;
}



/*
 * frees a dungeon
 */
void ccd_free_dungeon(CCD_DUNGEON *dungeon)
{
	
	free(dungeon->map[0]);
	free(dungeon->map);
	
	free(dungeon->map2[0]);
	free(dungeon->map2);
	
	free(dungeon->parameters);
		
	free(dungeon); 
}



/*
 * sets the RNG function
 */
void ccd_set_rng(int (*random_int_function)(int, int))
{

	ccd_random_int = random_int_function;
}



/*
 * checked variant of malloc
 */
static void * ccd_malloc(size_t sz)
{
	void *p;

	p = malloc(sz);

	if (p == NULL) {

		exit(EXIT_FAILURE);
	}

	return p;
}



/*
 * builds a cave
 */
static void ccd_build_cave(void)
{
	CCD_GENERATION g;

	ccd_init_map();
 	
 	for(g = 0; g < CCD_Dungeon->n_generations; g++) {
		const CCD_PARAMETERS *parameters;
		CCD_PARAMETER rep;
		
 		parameters = &CCD_Dungeon->parameters[g];

		for(rep = 0; rep < parameters->reps; rep++) {
			
			ccd_generation(parameters);
		}
 	}
}



/*
 * picks either a wall or a floor tile based on the set wall probability
 */
static CCD_TILE ccd_random_pick(void)
{

	if(ccd_random_int(1, 100) <= CCD_Dungeon->wall_probability) {

		return CCD_TILE_WALL;
		
	} else {
		
 		return CCD_TILE_FLOOR;
	}
}



/*
 * creates a new dungeon
 */
static CCD_DUNGEON * ccd_new_dungeon(CCD_SIZE height, CCD_SIZE width, 
	CCD_DUNGEON_TYPE type
)
{
	CCD_COORD y;
	CCD_GENERATION g;
	CCD_DUNGEON *dungeon;
	const CCD_PARAMETERS *parameters;
	CCD_TILE *p, *p2;
	
	dungeon = ccd_malloc(sizeof *dungeon);

	dungeon->height = height;
	dungeon->width = width;
	
	if (type == CCD_DT_CAVE) {
	
		dungeon->wall_probability = 
			CCD_CAVE_WALL_PROBABILITY;
		
		dungeon->n_generations = 
			CCD_CAVE_N_GENERATIONS;
		
		parameters = CCD_CaveParameters;

	} else if (type == CCD_DT_TUNNELS) {

		dungeon->wall_probability = 
			CCD_TUNNELS_WALL_PROBABILITY;
		
		dungeon->n_generations = 
			CCD_TUNNELS_N_GENERATIONS;
		
		parameters = CCD_TunnelsParameters;
		
	} else {
	
		return NULL;
	}

	dungeon->parameters = ccd_malloc(dungeon->n_generations * 
		sizeof *dungeon->parameters
	);

	for (g = 0; g < dungeon->n_generations; g++) {

		dungeon->parameters[g] = parameters[g];
	}
	
	
	p = ccd_malloc(dungeon->height * dungeon->width * sizeof *p);
	p2 = ccd_malloc(dungeon->height * dungeon->width * sizeof *p2);

	dungeon->map = ccd_malloc(dungeon->height * sizeof *dungeon->map);
	dungeon->map2 = ccd_malloc(dungeon->height * sizeof *dungeon->map2);

	for (y = 0; y < dungeon->height; y++) {

		dungeon->map[y] = p + (y * dungeon->width);
		dungeon->map2[y] = p2 + (y * dungeon->width);
	}

	return dungeon;
}



/*
 * initializes the dungeon map
 */
static void ccd_init_map(void)
{
	CCD_COORD x, y;

	for(y = 1; y < CCD_Dungeon->height - 1; y++) {
	for(x = 1; x < CCD_Dungeon->width - 1; x++) {
			
		CCD_Dungeon->map[y][x] = ccd_random_pick();
	}
	}

	for(y = 0; y < CCD_Dungeon->height; y++) {
	for(x = 0; x < CCD_Dungeon->width; x++) {

		CCD_Dungeon->map2[y][x] = CCD_TILE_WALL;
	}
	}
	
	for(y = 0; y < CCD_Dungeon->height; y++) {

		CCD_Dungeon->map[y][0] = CCD_TILE_WALL;
		CCD_Dungeon->map[y][CCD_Dungeon->width - 1] = CCD_TILE_WALL;
	}
	
	for(x = 0; x < CCD_Dungeon->width; x++) {

		CCD_Dungeon->map[0][x] = CCD_TILE_WALL;
		CCD_Dungeon->map[CCD_Dungeon->height - 1][x] = CCD_TILE_WALL;
	}
}



/*
 * one generation of cellular automata
 */
static void ccd_generation(const CCD_PARAMETERS *parameters)
{
	CCD_COORD x, y, i, j; 
	
	for(y = 1; y < CCD_Dungeon->height - 1; y++) {
	for(x = 1; x < CCD_Dungeon->width - 1; x++) {
 		int adjcount_r1 = 0, adjcount_r2 = 0;
 		
 		for(i = -1; i <= 1; i++) {
		for(j = -1; j <= 1; j++) {
			
 			if(CCD_Dungeon->map[y + i][x + j] != 
				CCD_TILE_FLOOR) {

				++adjcount_r1;
			}
 		}
		}
		
 		for(i = y - 2; i <= y + 2; i++) {
 		for(j = x - 2; j <= x + 2; j++) {
			
 			if(abs(i - y) == 2 && abs(j - x) == 2) {
 				
				continue;
			}
			
 			if(i < 0 || j < 0 || 
				i >= CCD_Dungeon->height || 
				j >= CCD_Dungeon->width) {

				continue;
			}
			
 			if(CCD_Dungeon->map[i][j] != CCD_TILE_FLOOR) {
 				
				++adjcount_r2;
			}
 		}
		}
		
 		if(adjcount_r1 >= parameters->r1_cutoff ||
			adjcount_r2 <= parameters->r2_cutoff) {

			CCD_Dungeon->map2[y][x] = CCD_TILE_WALL;
			
		} else {

 			CCD_Dungeon->map2[y][x] = CCD_TILE_FLOOR;
		}
 	}
	}
 	
	for(y = 1; y < CCD_Dungeon->height - 1; y++) {
 	for(x = 1; x < CCD_Dungeon->width - 1; x++) {
	
		CCD_Dungeon->map[y][x] = CCD_Dungeon->map2[y][x];
	}
	}
} 
 


/*
 * shouldn't be used: supply a quality random number function instead.
 * the quality of the generated dungeons depends on the quality of the 
 * RNG function!
 */
static int ccd_default_random_int(int min, int max)
{
	static bool initialized = false;
	int r;

	if (!initialized) {

		srand(time(NULL));
		initialized = true;
	}

	r = rand() / (RAND_MAX / (max + 1 - min));
	r += min;

	return r;
}



/*
 * returns true if all areas of the dungeon are accessible
 */
static bool ccd_full_access(void)
{
	CCD_N_TILES n_accessible_tiles, n_passable_tiles;

	n_passable_tiles = ccd_n_passable_tiles();
	
	n_accessible_tiles = ccd_n_accessible_tiles();

	if (n_accessible_tiles != n_passable_tiles) {
		
		return false;
	}
	
	return true;
}



/*
 * this flood fill function is used for accessibilty checking
 */
static void ccd_flood_fill(CCD_N_TILES *n_accessible_tiles, 
	CCD_COORD y, CCD_COORD x
)
{

	if (y < 0 || y < 0 ||
		y >= CCD_Dungeon->height || 
		x >= CCD_Dungeon->width) {

		return;
	}

	switch (CCD_Dungeon->map[y][x]) {

	case CCD_TILE_FLOOR:

		CCD_Dungeon->map[y][x] += CCD_VISITED;
		
		break;

	default:
		return;
	}

	*n_accessible_tiles += 1;

	ccd_flood_fill(n_accessible_tiles, y, x + 1);
	ccd_flood_fill(n_accessible_tiles, y, x - 1);
	ccd_flood_fill(n_accessible_tiles, y + 1, x);
	ccd_flood_fill(n_accessible_tiles, y - 1, x);

	ccd_flood_fill(n_accessible_tiles, y + 1, x + 1);
	ccd_flood_fill(n_accessible_tiles, y - 1, x - 1);
	ccd_flood_fill(n_accessible_tiles, y + 1, x - 1);
	ccd_flood_fill(n_accessible_tiles, y - 1, x + 1);
}



/*
 * puts the coordinates of a randomly chosen floor tile
 * into the passed variables
 */
static void ccd_random_floor_tile(CCD_COORD *y, CCD_COORD *x)
{

	do {

		*y = ccd_random_int(0, CCD_Dungeon->height - 1);
		*x = ccd_random_int(0, CCD_Dungeon->width - 1);

	} while (CCD_Dungeon->map[*y][*x] != CCD_TILE_FLOOR);
}



/*
 * returns the number of accessible tiles
 */
static CCD_N_TILES ccd_n_accessible_tiles(void)
{
	CCD_N_TILES n_accessible_tiles = 0;
	CCD_COORD y, x;

	ccd_random_floor_tile(&y, &x);

	n_accessible_tiles = 0;

	ccd_flood_fill(&n_accessible_tiles, y, x);

	for (y = 0; y < CCD_Dungeon->height; y++) {
	for (x = 0; x < CCD_Dungeon->width; x++) {
		
		if (CCD_Dungeon->map[y][x] >= CCD_VISITED) {

			CCD_Dungeon->map[y][x] -= CCD_VISITED;
		}
	}
	}

	return n_accessible_tiles;
}



/*
 * returns the number of passable tiles
 */
static CCD_N_TILES ccd_n_passable_tiles(void)
{
	CCD_N_TILES n_passable_tiles;
	CCD_COORD x, y;

	n_passable_tiles = 0;

	for (y = 0; y < CCD_Dungeon->height; y++) {
	for (x = 0; x < CCD_Dungeon->width; x++) {

		switch (CCD_Dungeon->map[y][x]) {

		case CCD_TILE_FLOOR:

			++n_passable_tiles;

			break;

		default: /* DO NOTHING */;

		}
	}
	}

	return n_passable_tiles;
}




#if defined(CCD_STAND_ALONE)

static void		ccd_print_function(void);
static void		ccd_print_map(void);



/*
 * example 
 */
int main(void)
{
	CCD_DUNGEON *dungeon;

	dungeon = ccd_build_dungeon(40, 60, CCD_DT_CAVE);

	ccd_print_map();

	ccd_print_function();
	
	ccd_free_dungeon(dungeon);
	
	return EXIT_SUCCESS;
}


/*
 * prints the used function
 */
static void ccd_print_function(void)
{
	CCD_GENERATION g;
 	
	printf("W[0](p) = rand[0,100) < %i\n", 
		CCD_Dungeon->wall_probability
	);
 	
 	for(g = 0; g < CCD_Dungeon->n_generations; g++) {

 		printf("Repeat %i: W'(p) = R[1](p) >= %i",
 			CCD_Dungeon->parameters[g].reps, 
			CCD_Dungeon->parameters[g].r1_cutoff
		);
 		
 		if(CCD_Dungeon->parameters[g].r2_cutoff >= 0) {
 			
			printf(" || R[2](p) <= %i\n", 
				CCD_Dungeon->parameters[g].r2_cutoff
			);
			
		} else {
			
 			putchar('\n');
		}
 	}
}


 
/*
 * prints the dungeon map
 */
static void ccd_print_map(void)
{
	CCD_COORD x, y;
 	
 	for(y = 0; y < CCD_Dungeon->height; y++) {

		for(x = 0; x < CCD_Dungeon->width; x++) {
			
			switch(CCD_Dungeon->map[y][x]) {

			case CCD_TILE_WALL:  putchar('#'); break;
			case CCD_TILE_FLOOR: putchar('.'); break;
			}
		}
		
		putchar('\n');
 	}
}

#endif


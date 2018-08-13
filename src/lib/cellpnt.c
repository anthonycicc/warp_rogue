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
 * Cell Point Dungeon Builder
 *
 *
 * NAMESPACE: CPN_* / cpn_*
 *
 */

/*
 * TO DO:
 *
 * - requested rooms feature
 * - big rooms (V_DOUBLE, H_DOUBLE, QUAD)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#include "cellpnt.h"


#define CPN_MAX_ERROR_MESSAGE			100
#define CPN_N_DIRECTIONS			4

#define CPN_V_STEP				((CPN_MaxVerticalPadding \
							* 2) + 2)
							
#define CPN_H_STEP				((CPN_MaxHorizontalPadding \
							* 2) + 2)

#define CPN_MAX_ROOM_HEIGHT			((CPN_MaxVerticalPadding \
							* 2) + 1)

#define CPN_MAX_ROOM_WIDTH			((CPN_MaxHorizontalPadding \
							* 2) + 1)

#define CPN_DEFAULT_CELL_HEIGHT			11
#define CPN_DEFAULT_CELL_WIDTH			21

#define CPN_DEFAULT_CORRIDOR_FREQUENCY		50

#define CPN_DEFAULT_MIN_VERTICAL_PADDING	0
#define CPN_DEFAULT_MIN_HORIZONTAL_PADDING	0

#define CPN_DEFAULT_MIN_ROOM_HEIGHT		3
#define CPN_DEFAULT_MIN_ROOM_WIDTH		5

#define CPN_VISITED				128

#ifdef __LCC__
#	define CPN_NOT_USED(p)			((p) = (p))
#else
#	define CPN_NOT_USED(p)			(void)(p)
#endif


#define cpn_is_even(x)				((x) % 2 == 0)

#define cpn_max(a, b)				((a) < (b) ? (b) : (a))




/*
 * directions
 */
typedef enum {
	
	CPN_UP,
	CPN_DOWN,
	CPN_LEFT,
	CPN_RIGHT

} CPN_DIRECTION;







static void		cpn_error(const char *);
static void *		cpn_malloc(size_t);

static CPN_DUNGEON *	cpn_alloc_dungeon(int, int);

static void		cpn_init_rooms(void);
static void		cpn_init_rectangle(CPN_RECTANGLE *);

static void		cpn_generate_rooms(void);
static void		cpn_generate_room(CPN_ROOM *);

static void		cpn_build_rooms(void);
static void		cpn_build_room(CPN_ROOM *);

static void		cpn_connect_rooms(void);
static void		cpn_connect_room(CPN_ROOM *);

static bool		cpn_connection_built(const CPN_ROOM *, 
				CPN_DIRECTION
			);

static void		cpn_update_connectivity_data(CPN_ROOM *, 
				CPN_DIRECTION
			);

static void		cpn_build_connection(CPN_ROOM *, CPN_DIRECTION);

static void		cpn_corridor_start(int *, int *, CPN_ROOM *,
				CPN_DIRECTION
			);

static void		cpn_build_corridor_section(int, int, int,
				const CPN_ROOM *, CPN_DIRECTION
			);

static bool		cpn_destructable_obstacle_allowed(int, int,
				const CPN_ROOM *, const CPN_ROOM *
			);

static CPN_ROOM *	cpn_adjacent_room(const CPN_ROOM *, CPN_DIRECTION);

static CPN_ROOM *	cpn_room_at(int, int);

static void		cpn_shuffle_directions(CPN_DIRECTION *);

static bool		cpn_is_inside_room(int, int, const CPN_ROOM *);

static void		cpn_fill_dungeon(CPN_TILE);
static void		cpn_fill_rectangle(CPN_TILE, const CPN_RECTANGLE *);

static void		cpn_direction_step(int *, int *, CPN_DIRECTION, int);

static int		cpn_default_random_int(int, int);

static int		cpn_distance(int, int, int, int);

static bool		cpn_full_access(void);
static void		cpn_flood_fill(int *, int, int);
static void		cpn_random_floor_tile(int *, int *);
static int		cpn_n_accessible_tiles(void);
static int		cpn_n_passable_tiles(void);


static CPN_DUNGEON *	CPN_Dungeon = NULL;

static int		CPN_CellHeight = CPN_DEFAULT_CELL_HEIGHT;
static int		CPN_CellWidth = CPN_DEFAULT_CELL_WIDTH;

static int		CPN_MinVerticalPadding =
				CPN_DEFAULT_MIN_VERTICAL_PADDING;
				
static int		CPN_MaxVerticalPadding =
				(CPN_DEFAULT_CELL_HEIGHT - 1) / 2;
				
static int		CPN_MinHorizontalPadding =
				CPN_DEFAULT_MIN_HORIZONTAL_PADDING;
				
static int		CPN_MaxHorizontalPadding =
				(CPN_DEFAULT_CELL_WIDTH - 1) / 2;

static int		CPN_MinRoomHeight = CPN_DEFAULT_MIN_ROOM_HEIGHT;
static int		CPN_MinRoomWidth = CPN_DEFAULT_MIN_ROOM_WIDTH;

static int		CPN_CorridorFrequency = 
				CPN_DEFAULT_CORRIDOR_FREQUENCY;

static char		CPN_LastError[CPN_MAX_ERROR_MESSAGE];


static int		(*cpn_random_int)(int, int) = cpn_default_random_int;



/*
 * frees a dungeon
 */
void cpn_free_dungeon(CPN_DUNGEON *dungeon)
{

	free(dungeon->room);

	free(dungeon->map[0]);
	free(dungeon->map);

	free(dungeon);
}



/*
 * sets the cell size
 */
bool cpn_set_cell_size(int height, int width)
{
	if (cpn_is_even(height)) {
		
		cpn_error(
			"Failed attempt to set even cell height. "
			"Cell height must be odd."
		);
		
		return false;
	}

	if (cpn_is_even(width)) {
		
		cpn_error(
			"Failed attempt to set even cell width. "
			"Cell width must be odd."
		);
		
		return false;
	}

	CPN_CellHeight = height;
	CPN_CellWidth = width;
	CPN_MaxVerticalPadding = (height - 1) / 2;
	CPN_MaxHorizontalPadding = (width - 1) / 2;

	return true;

}



/*
 * sets the minimal room size
 */
bool cpn_set_minimal_room_size(int height, int width)
{


	if (height > CPN_MAX_ROOM_HEIGHT || width > CPN_MAX_ROOM_WIDTH ||
		height < 1 || width < 1) {

		cpn_error(
			"Failed attempt to set invalid minimal "
			"room size values."
		);

		return false;
	}


	CPN_MinRoomHeight = height;
	CPN_MinRoomWidth = width;

	return true;
}



/*
 * sets the RNG
 */
void cpn_set_rng(int (*random_int_function)(int, int))
{

	cpn_random_int = random_int_function;
}



/*
 * returns the last error message
 */
const char * cpn_get_error(void)
{

	return CPN_LastError;
}



/*
 * sets the corridor frequency
 */
void cpn_set_corridor_frequency(int frequency)
{
	CPN_CorridorFrequency = frequency;
}



/*
 * builds a dungeon
 */
CPN_DUNGEON *cpn_build_dungeon(int n_rows, int n_columns,
	CPN_ROOM *requested_rooms
)
{
	CPN_NOT_USED(requested_rooms);

	if (cpn_alloc_dungeon(n_rows, n_columns) == NULL) {
		
		return NULL;
	}

	do {

		cpn_fill_dungeon(CPN_WALL);

		cpn_init_rooms();

		cpn_generate_rooms();

		cpn_build_rooms();

		cpn_connect_rooms();

	} while (!cpn_full_access());


	return CPN_Dungeon;
}



/*
 * outputs the dungeon map and all important data to a plain text file
 */
bool cpn_output_dungeon(const char *file_name)
{
	FILE *out_file;
	int x, y, i;

	out_file = fopen(file_name, "w");
	
	if (out_file == NULL) {
		
		cpn_error("Couldn't open output file.");
		
		return false;
	}

	fprintf(out_file, "%d,%d,%d\n", CPN_Dungeon->height, 
		CPN_Dungeon->width, CPN_Dungeon->n_rooms
	);
	
	for (y = 0; y < CPN_Dungeon->height; y++) {

		for (x = 0; x < CPN_Dungeon->width; x++) {

			switch (CPN_Dungeon->map[y][x]) {
			
			case CPN_FLOOR:
				fprintf(out_file, ".");
				break;
			
			case CPN_WALL:
				fprintf(out_file, "#");
				break;
			
			case CPN_DESTRUCTABLE_OBSTACLE:
			case CPN_DESTRUCTABLE_OBSTACLE_2:
				fprintf(out_file, "|");
				break;

			default:
				/* DO NOTHING */;
			}

		}

		fprintf(out_file, "\n");
	}

	for (i = 0; i < CPN_Dungeon->n_rooms; i++) {
		CPN_ROOM *room;

		room = &CPN_Dungeon->room[i];
		
		fprintf(out_file, "%d,%d,%d\n", room->type, 
			room->source_y, room->source_x
		);
		
		fprintf(out_file, "%d,%d,%d,%d\n", 
			room->padding.top, room->padding.bottom, 
			room->padding.left, room->padding.right
		);
	}

	fclose(out_file);

	return true;
}



/*
 * copies the passed error message to the "last error" buffer
 */
static void cpn_error(const char *error_message)
{

	strcpy(CPN_LastError, error_message);
}



/*
 * checked variant of malloc
 */
static void * cpn_malloc(size_t sz)
{
	void *p;

	p = malloc(sz);

	if (p == NULL) {

		cpn_error("Memory allocation failed.");
	}

	return p;
}



/*
 * allocates a dungeon
 */
static CPN_DUNGEON * cpn_alloc_dungeon(int n_rows, int n_columns)
{
	int i;
	CPN_TILE *p;

	CPN_Dungeon = cpn_malloc(sizeof *CPN_Dungeon);
	
	if (CPN_Dungeon == NULL) {
		
		return NULL;
	}

	CPN_Dungeon->height = (n_rows * (CPN_CellHeight + 1)) + 1;
	CPN_Dungeon->width = (n_columns * (CPN_CellWidth + 1)) + 1;

	p = cpn_malloc(CPN_Dungeon->height * CPN_Dungeon->width * sizeof *p);

	if (p == NULL) return NULL;
	
	CPN_Dungeon->map = cpn_malloc(CPN_Dungeon->height * 
		sizeof *CPN_Dungeon->map
	);
	
	if (CPN_Dungeon->map == NULL) return NULL;

	for (i = 0; i < CPN_Dungeon->height; i++) {

		CPN_Dungeon->map[i] = p + (i * CPN_Dungeon->width);
	}

	CPN_Dungeon->n_rooms = n_rows * n_columns;

	CPN_Dungeon->room = cpn_malloc(CPN_Dungeon->n_rooms * 
		sizeof *CPN_Dungeon->room
	);
	
	if (CPN_Dungeon->room == NULL) {
		
		return NULL;
	}

	return CPN_Dungeon;
}



/*
 * initializes the rooms
 */
static void cpn_init_rooms(void)
{
	int y, x, i;

	i = 0;
	for (y = 1 + CPN_MaxVerticalPadding;
		y <= CPN_Dungeon->height - 2 - CPN_MaxVerticalPadding;
		y += CPN_V_STEP) {

		for (x = 1 + CPN_MaxHorizontalPadding;
			x <= CPN_Dungeon->width - 2 - 
			CPN_MaxHorizontalPadding;
			x += CPN_H_STEP) {
			CPN_ROOM *room;

			room = &CPN_Dungeon->room[i];

			room->source_y = y;
			room->source_x = x;

			room->type = CPN_ROOM_NOT_BUILT;

			room->n_connections = 0;

			cpn_init_rectangle(&room->padding);
			cpn_init_rectangle(&room->area);
			cpn_init_rectangle(&room->connection);

			++i;
		}
	}
}



/*
 * inits a rectangle data structure
 */
static void cpn_init_rectangle(CPN_RECTANGLE *rect)
{

	rect->top = rect->bottom = rect->left = rect->right = 0;
}



/*
 * fills the entire dungeon with the passed tile
 */
static void cpn_fill_dungeon(CPN_TILE tile)
{
	int x, y;

	for (y = 0; y < CPN_Dungeon->height; y++) {

		for (x = 0; x < CPN_Dungeon->width; x++) {

			CPN_Dungeon->map[y][x] = tile;
		}
	}
}



/*
 * generates the rooms
 */
static void cpn_generate_rooms(void)
{
	int i;

	for (i = 0; i < CPN_Dungeon->n_rooms; i++) {
		CPN_ROOM *room;

		room = &(CPN_Dungeon->room[i]);

		cpn_generate_room(room);
	}
}



/*
 * generates a room
 */
static void cpn_generate_room(CPN_ROOM *room)
{
	int room_height;
	int room_width;


	if (cpn_random_int(1, 100) <= CPN_CorridorFrequency) {
		
		room->type = CPN_ROOM_CORRIDOR_CONNECTION;
		
	} else {

	while (true) {

		room->padding.top = cpn_random_int(
			CPN_MinVerticalPadding,
			CPN_MaxVerticalPadding
		);

		room->padding.bottom = cpn_random_int(
			CPN_MinVerticalPadding,
			CPN_MaxVerticalPadding
		);

		room_height = room->padding.top + 1 + room->padding.bottom;

		if (room_height < CPN_MinRoomHeight) continue;

		room->padding.left = cpn_random_int(
			CPN_MinHorizontalPadding,
			CPN_MaxHorizontalPadding
		);

		room->padding.right = cpn_random_int(
			CPN_MinHorizontalPadding,
			CPN_MaxHorizontalPadding
		);

		room_width = room->padding.left + 1 + room->padding.right;

		if (room_width < CPN_MinRoomWidth) continue;

		break;

	}

	}

	room->area.top = room->source_y - room->padding.top;
	room->area.bottom = room->source_y + room->padding.bottom;
	room->area.left = room->source_x - room->padding.left;
	room->area.right = room->source_x + room->padding.right;
}



/*
 * builds the rooms
 */
static void cpn_build_rooms(void)
{
	int i;

	for (i = 0; i < CPN_Dungeon->n_rooms; i++) {
		CPN_ROOM *room;

		room = &(CPN_Dungeon->room[i]);

		cpn_build_room(room);

	}


}



/*
 * builds a room
 */
static void cpn_build_room(CPN_ROOM *room)
{
	cpn_fill_rectangle(CPN_FLOOR, &room->area);
}



/*
 * connects the rooms
 */
static void cpn_connect_rooms(void)
{
	int i;

	for (i = 0; i < CPN_Dungeon->n_rooms; i++) {
		cpn_connect_room(&CPN_Dungeon->room[i]);
	}

}



/*
 * connects a room
 */
static void cpn_connect_room(CPN_ROOM *room)
{
	CPN_DIRECTION connect_dirs[CPN_N_DIRECTIONS] = {
		CPN_UP, CPN_DOWN, CPN_LEFT, CPN_RIGHT
	};
	CPN_DIRECTION direction;
	int i;
	CPN_ROOM *target_room;

	cpn_shuffle_directions(connect_dirs);

	for (i = 0; i < CPN_N_DIRECTIONS; i++) {

		if (room->n_connections >= 2) {
			return;
		}

		direction = connect_dirs[i];

		if (cpn_connection_built(room, direction)) continue;

		target_room = cpn_adjacent_room(room, direction);
		if (target_room == NULL) continue;

		cpn_build_connection(room,  direction);
	}



}



/*
 * returns true if a connection has already been built in the passed direction
 */
static bool cpn_connection_built(const CPN_ROOM *room, CPN_DIRECTION direction)
{
	if (direction == CPN_UP && room->connection.top) return true;
	if (direction == CPN_DOWN && room->connection.bottom) return true;
	if (direction == CPN_LEFT && room->connection.left) return true;
	if (direction == CPN_RIGHT && room->connection.right) return true;

	return false;
}



/*
 * updates the connectivity data
 */
static void cpn_update_connectivity_data(CPN_ROOM *room, CPN_DIRECTION direction
) {
	CPN_ROOM *target_room;

	target_room = cpn_adjacent_room(room, direction);

	room->n_connections += 1;
	target_room->n_connections += 1;

	switch (direction) {
	case CPN_UP:
		room->connection.top = 1;
		target_room->connection.bottom = 1;
		break;
	case CPN_DOWN:
		room->connection.bottom = 1;
		target_room->connection.top = 1;
		break;
	case CPN_LEFT:
		room->connection.left = 1;
		target_room->connection.right = 1;
		break;
	case CPN_RIGHT:
		room->connection.right = 1;
		target_room->connection.left = 1;
		break;
	}
}



/*
 * fills the passed rectangle with the passed tile
 */
static void cpn_fill_rectangle(CPN_TILE tile, const CPN_RECTANGLE *rect)
{
	int y, x;

	for (y = rect->top; y <= rect->bottom; y++) {

		for (x = rect->left; x <= rect->right; x++) {

			CPN_Dungeon->map[y][x] = tile;
		}

	}

}



/*
 * shuffles a set of directions
 */
static void cpn_shuffle_directions(CPN_DIRECTION *dirs)
{
	int s1, s2, sn = 10;
	CPN_DIRECTION h;

	while (sn--) {

		do {

			s1 = cpn_random_int(0, CPN_N_DIRECTIONS - 1);
			s2 = cpn_random_int(0, CPN_N_DIRECTIONS - 1);

		} while (s1 == s2);

		h = dirs[s1];
		dirs[s1] = dirs[s2];
		dirs[s2] = h;
	}
}



/*
 * returns true if the passed point is inside the area of the passed room
 */
static bool cpn_is_inside_room(int y, int x, const CPN_ROOM *room)
{
	if ((y >= room->area.top && y <= room->area.bottom) &&
		(x >= room->area.left && x <= room->area.right)) {

		return true;
	}

	return false;
}



/*
 * returns the room which source point is equal passed point
 */
static CPN_ROOM * cpn_room_at(int y, int x)
{
	int i;

	for (i = 0; i < CPN_Dungeon->n_rooms; i++) {
		if (CPN_Dungeon->room[i].source_y == y &&
			CPN_Dungeon->room[i].source_x == x) {

			return &CPN_Dungeon->room[i];
		}
	}

	return NULL;
}



/*
 * returns a room adjacent to the passed room
 */
static CPN_ROOM * cpn_adjacent_room(const CPN_ROOM *room, 
	CPN_DIRECTION direction
)
{
	int ty, tx;

	ty = room->source_y;
	tx = room->source_x;

	switch(direction) {

	case CPN_UP:
		ty -= CPN_V_STEP;
		break;
	case CPN_DOWN:
		ty += CPN_V_STEP;
		break;
	case CPN_LEFT:
		tx -= CPN_H_STEP;
		break;
	case CPN_RIGHT:
		tx += CPN_H_STEP;
		break;
	}

	return cpn_room_at(ty, tx);

}



/*
 * returns the starting point of a corridor
 */
static void cpn_corridor_start(int *y, int *x, CPN_ROOM *room, 
	CPN_DIRECTION direction
)
{
	int start_y;
	int start_x;

	if (room->type == CPN_ROOM_CORRIDOR_CONNECTION) {
		
		*y = room->source_y;
		*x = room->source_x;
		
		return;
	}

	start_y = room->source_y;
	start_x = room->source_x;

	switch(direction) {

	case CPN_UP:
		start_y = room->area.top - 1;
		break;
	
	case CPN_DOWN:
		start_y = room->area.bottom + 1;
		break;
	
	case CPN_LEFT:
		start_x = room->area.left - 1;
		break;
	
	case CPN_RIGHT:
		start_x = room->area.right + 1;
		break;
	}

	*y = start_y;
	*x = start_x;

}



/*
 * builds a corridor section
 */
static void cpn_build_corridor_section(int y, int x, int corridor_width,
	const CPN_ROOM *room, CPN_DIRECTION direction)
{
	CPN_TILE tile;
	const CPN_ROOM *target_room;

	target_room = cpn_adjacent_room(room, direction);

	tile = CPN_FLOOR;
	if (cpn_random_int(1, 100) == 100 &&
		cpn_destructable_obstacle_allowed(y, x, room, target_room)) {

		if (cpn_random_int(0,1) == 0) {

			tile = CPN_DESTRUCTABLE_OBSTACLE;

		} else {

			tile = CPN_DESTRUCTABLE_OBSTACLE_2;
		}
	}

	CPN_Dungeon->map[y][x] = tile;

	if (corridor_width == 1) return;

	if (direction == CPN_UP || direction == CPN_DOWN) {

		CPN_Dungeon->map[y][x - 1] = tile;
		if (corridor_width == 2) return;
		CPN_Dungeon->map[y][x + 1] = tile;

	} else if (direction == CPN_LEFT || direction == CPN_RIGHT) {

		CPN_Dungeon->map[y - 1][x] = tile;
		if (corridor_width == 2) return;
		CPN_Dungeon->map[y + 1][x] = tile;

	}

}



/*
 * returns true if a destructable obstacle can be build at the passed point
 */
static bool cpn_destructable_obstacle_allowed(int y, int x,
	const CPN_ROOM *room, const CPN_ROOM *target_room
)
{
	if (room->type == CPN_ROOM_CORRIDOR_CONNECTION &&
		cpn_distance(y, x, room->source_y, room->source_x) <= 1) {

		return false;
	}

	if (target_room->type == CPN_ROOM_CORRIDOR_CONNECTION &&
		cpn_distance(y, x, target_room->source_y, 
			target_room->source_x) <= 1) {

		return false;
	}

	return true;
}



/*
 * builds a connection
 */
static void cpn_build_connection(CPN_ROOM *room, CPN_DIRECTION direction)
{
	CPN_ROOM *target_room;
	int y, x;
	int corridor_width;

	target_room = cpn_adjacent_room(room, direction);

	cpn_corridor_start(&y, &x, room, direction);

	corridor_width = cpn_random_int(1, 3);

	while (true) {

		cpn_build_corridor_section(y, x, corridor_width,
			room, direction
		);

		cpn_direction_step(&y, &x, direction, 1);

		if (cpn_is_inside_room(y, x, target_room)) {

			if (target_room->type == 
				CPN_ROOM_CORRIDOR_CONNECTION) {

				cpn_build_corridor_section(y, x, 
					corridor_width, room,
					direction
				);
			}

			break;
		}

	}

	cpn_update_connectivity_data(room, direction);
}



/*
 * makes n_steps in the passed direction
 */
static void cpn_direction_step(int *y, int *x, CPN_DIRECTION direction,
	int n_steps
)
{

	switch (direction) {

	case CPN_UP:
		*y -= n_steps;
		break;
	
	case CPN_DOWN:
		*y += n_steps;
		break;
	
	case CPN_LEFT:
		*x -= n_steps;
		break;
	
	case CPN_RIGHT:
		*x += n_steps;
		break;
	}


}



/*
 * shouldn't be used: supply a quality random number function instead.
 * the quality of the generated dungeons depends on the quality of the
 * RNG function!
 */
static int cpn_default_random_int(int min, int max)
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
 * returns the distance between two points
 */
static int cpn_distance(int y1, int x1, int y2, int x2)
{
	int xdiff, ydiff, distance;

	xdiff = abs(x1 - x2);
	ydiff = abs(y1 - y2);
	
	distance = xdiff + ydiff + cpn_max(xdiff, ydiff);
	distance >>= 1;

	return distance;
}



/*
 * checks whether or not all areas of the dungeon are accessible
 */
static bool cpn_full_access(void)
{
	int n_accessible_tiles, n_passable_tiles;

	n_accessible_tiles = cpn_n_accessible_tiles();

	n_passable_tiles = cpn_n_passable_tiles();

	if (n_passable_tiles == n_accessible_tiles) {
		return true;
	}

	return false;
}



/*
 * this flood fill function is used for accessibilty checking
 */
static void cpn_flood_fill(int *n_accessible_tiles, int y, int x)
{

	if (y < 0 || y < 0 ||
		y >= CPN_Dungeon->height || x >= CPN_Dungeon->width) {

		return;
	}

	switch (CPN_Dungeon->map[y][x]) {

	case CPN_FLOOR:
	case CPN_DESTRUCTABLE_OBSTACLE:
	case CPN_DESTRUCTABLE_OBSTACLE_2:

		CPN_Dungeon->map[y][x] += CPN_VISITED;

		break;

	default:
		return;
	}

	*n_accessible_tiles += 1;

	cpn_flood_fill(n_accessible_tiles, y, x + 1);
	cpn_flood_fill(n_accessible_tiles, y, x - 1);
	cpn_flood_fill(n_accessible_tiles, y + 1, x);
	cpn_flood_fill(n_accessible_tiles, y - 1, x);

	cpn_flood_fill(n_accessible_tiles, y + 1, x + 1);
	cpn_flood_fill(n_accessible_tiles, y - 1, x - 1);
	cpn_flood_fill(n_accessible_tiles, y + 1, x - 1);
	cpn_flood_fill(n_accessible_tiles, y - 1, x + 1);

}



/*
 * puts the coordinates of a randomly chosen floor tile
 * into the passed variables
 */
static void cpn_random_floor_tile(int *y, int *x)
{

	do {

		*y = cpn_random_int(0, CPN_Dungeon->height - 1);
		*x = cpn_random_int(0, CPN_Dungeon->width - 1);

	} while (CPN_Dungeon->map[*y][*x] != CPN_FLOOR);

}



/*
 * returns the number of accessible tiles
 */
static int cpn_n_accessible_tiles(void)
{
	int n_accessible_tiles = 0;
	int y, x;

	cpn_random_floor_tile(&y, &x);

	n_accessible_tiles = 0;
	cpn_flood_fill(&n_accessible_tiles, y, x);

	/* revert the map changes caused by the flood file function */
	for (y = 0; y < CPN_Dungeon->height; y++) {
	for (x = 0; x < CPN_Dungeon->width; x++) {

		if (CPN_Dungeon->map[y][x] < CPN_VISITED) {
			continue;
		}

		CPN_Dungeon->map[y][x] -= CPN_VISITED;
	}
	}

	return n_accessible_tiles;

}



/*
 * returns the number of passable tiles
 */
static int cpn_n_passable_tiles(void)
{
	int n_passable_tiles;
	int x, y;

	n_passable_tiles = 0;

	for (y = 0; y < CPN_Dungeon->height; y++) {
	for (x = 0; x < CPN_Dungeon->width; x++) {

		switch (CPN_Dungeon->map[y][x]) {

		case CPN_FLOOR:
		case CPN_DESTRUCTABLE_OBSTACLE:
		case CPN_DESTRUCTABLE_OBSTACLE_2:

			++n_passable_tiles;

			break;

		default: /* DO NOTHING */;

		}

	}
	}

	return n_passable_tiles;
}


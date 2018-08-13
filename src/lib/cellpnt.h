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
 * dungeon tiles
 */
#define CPN_FLOOR			0
#define CPN_DESTRUCTABLE_OBSTACLE	1
#define CPN_DESTRUCTABLE_OBSTACLE_2	2
#define CPN_WALL			3


typedef int				CPN_TILE;


/*
 * room types
 */
typedef enum {

	CPN_ROOM_NOT_BUILT,

	CPN_ROOM_CORRIDOR_CONNECTION,

	CPN_ROOM_NORMAL,

	CPN_ROOM_H_DOUBLE,
	CPN_ROOM_V_DOUBLE,
	CPN_ROOM_QUAD

} CPN_ROOM_TYPE;


/*
 * rectanlge
 */
typedef struct {

	int		top;
	int		bottom;
	int		left;
	int		right;

} CPN_RECTANGLE;


/*
 * room
 */
typedef struct {

	CPN_ROOM_TYPE	type; /* room type */

	/* room source point coordinates */
	int		source_y;
	int		source_x;

	CPN_RECTANGLE	padding;

	CPN_RECTANGLE	area;

	CPN_RECTANGLE	connection;

	int		n_connections;

} CPN_ROOM;


/*
 * dungeon
 */
typedef struct {

	int width;
	int height;
	CPN_TILE **	map;

	int n_rooms;
	CPN_ROOM *	room;

} CPN_DUNGEON;



void		cpn_free_dungeon(CPN_DUNGEON *);

bool		cpn_set_cell_size(int, int);

bool		cpn_set_minimal_room_size(int, int);

void		cpn_set_rng(int(*)(int, int));

const char *	cpn_get_error(void);

void		cpn_set_corridor_frequency(int);

CPN_DUNGEON *	cpn_build_dungeon(int, int, CPN_ROOM *);

bool		cpn_output_dungeon(const char *);




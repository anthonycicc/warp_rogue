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
 * Module: Area
 */

#include "wrogue.h"



static void	area_ruin_sector(const AREA_POINT *, const char *);




/*
 * the currently active (i.e. loaded) area
 */
static AREA                     Area;

/*
 * direct link to the sectors for fast accesss
 */
static SECTOR                   (* const AreaSector)[AREA_WIDTH] =
                                        Area.sector;



/*
 * the NIL point
 */
static const AREA_POINT         AreaPointNil = {

        AREA_COORD_NIL,
        AREA_COORD_NIL

};


/*
 * area bounds
 */
static const AREA_SECTION       AreaBounds = {

        0, AREA_HEIGHT -1,
        0, AREA_WIDTH -1

};



/*
 * area module init
 */
void area_init(void)
{
        Area.events = list_new();

        Area.characters = list_new();

        memset(AreaSector, 0, AREA_SIZE * sizeof(SECTOR));

        sector_reset_bounds();
}



/*
 * area module clean up
 */
void area_clean_up(void)
{

        area_clear();

        list_free(Area.events);

        list_free(Area.characters);
}



/*
 * clears the currently active area
 */
void area_clear(void)
{
        const AREA_SECTION *bounds = area_bounds();
        AREA_POINT p;

        list_clear_with(Area.events, event_free);

        list_clear_with(Area.characters, character_free);

        for (p.y = bounds->top; p.y <= bounds->bottom; p.y++) {
        for (p.x = bounds->left; p.x <= bounds->right; p.x++) {
                TERRAIN *terrain = terrain_at(&p);
                OBJECT *object = object_at(&p);

                terrain_free(terrain);

                if (object != NULL) object_free(object);
        }
        }

        memset(AreaSector, 0, AREA_SIZE * sizeof(SECTOR));
}



/*
 * returns the active area
 */
AREA * active_area(void)
{

        return &Area;
}



/*
 * returns the character list of the active area
 */
LIST * area_character_list(void)
{

        return Area.characters;
}



/*
 * returns the event list of the active area
 */
LIST * area_event_list(void)
{

        return Area.events;
}



/*
 * returns the number of characters in the active area
 */
N_CHARACTERS area_n_characters(void)
{

        return Area.characters->n_nodes;
}



/*
 * returns true if the coordinates of both area points are equal
 */
bool area_points_equal(const AREA_POINT *p1, const AREA_POINT *p2)
{

        if (p1->x == p2->x && p1->y == p2->y) {

                return true;
        }

        return false;
}



/*
 * moves an area point one step in the passed direction
 */
void move_area_point(AREA_POINT *area_point, DIRECTION direction)
{

        area_point->x += direction_modifier(COORD_X, direction);
        area_point->y += direction_modifier(COORD_Y, direction);
}



/*
 * returns the estimated euclidian distance between two area points
 */
AREA_DISTANCE area_distance(const AREA_POINT *p1, const AREA_POINT *p2)
{
        AREA_DISTANCE xdiff, ydiff, distance;

        xdiff = abs(p1->x - p2->x);
        ydiff = abs(p1->y - p2->y);

        distance = xdiff + ydiff + max(xdiff, ydiff);
        distance >>= 1;

        return distance;
}



/*
 * returns the area bounds
 */
const AREA_SECTION * area_bounds(void)
{

        return &AreaBounds;
}



/*
 * returns true if the passed point is out of bounds
 */
bool out_of_area_bounds(const AREA_POINT *location)
{

        if (location->y < 0 ||
                location->x < 0 ||
                location->y >= AREA_HEIGHT ||
                location->x >= AREA_WIDTH) {

                return true;
        }

        return false;
}



/*
 * return the area point NIL
 */
const AREA_POINT * area_point_nil(void)
{

        return &AreaPointNil;
}



/*
 * places a terrain at the passed point
 */
void place_terrain(TERRAIN *terrain, const AREA_POINT *target_point)
{

        sector_at(target_point)->terrain = terrain;
}



/*
 * removes a terrain object from the currently active area
 */
void remove_terrain(TERRAIN *terrain, LIST *terrain_events)
{

        remove_terrain_events(terrain, terrain_events);
}



/*
 * places an object at the passed point
 */
void place_object(OBJECT *object, const AREA_POINT *target_point)
{

        sector_at(target_point)->object = object;
}



/*
 * removes an object from the currently active area
 */
void remove_object(OBJECT *object, LIST *object_events)
{

        remove_object_events(object, object_events);
}



/*
 * places a character at the passed point
 */
void place_character(CHARACTER *character, const AREA_POINT *target_point)
{
        sector_at(target_point)->character = character;
        character->location = *target_point;

        if (list_node_of(Area.characters, character) != NULL) return;

        list_add(Area.characters, character);
}



/*
 * removes a character from the currently active area
 */
void remove_character(CHARACTER *character, LIST *character_events)
{
        LIST_NODE *node;

        remove_character_events(character, character_events);

        for (node = character->inventory->head;
                node != NULL;
                node = node->next) {
                OBJECT *object;

                object = (OBJECT *)node->data;

                remove_object(object, character_events);
        }

        list_remove(Area.characters, character);

        sector_at(&character->location)->character = NULL;
        character->location = *area_point_nil();
}



/*
 * returns the sector located at the passed point
 */
SECTOR * sector_at(const AREA_POINT *location)
{

        return &AreaSector[location->y][location->x];
}



/*
 * returns the terrain located at the passed point
 */
TERRAIN * terrain_at(const AREA_POINT *location)
{

        return AreaSector[location->y][location->x].terrain;
}



/*
 * returns the object located at the passed point
 */
OBJECT * object_at(const AREA_POINT *location)
{

        return AreaSector[location->y][location->x].object;
}



/*
 * returns the character located at the passed point
 */
CHARACTER * character_at(const AREA_POINT *location)
{

        return AreaSector[location->y][location->x].character;
}



/*
 * removes all gore features from the active area
 */
void area_remove_gore_features(void)
{
	const AREA_SECTION *bounds = area_bounds();
	AREA_POINT p;
	
	for (p.y = bounds->top; p.y <= bounds->bottom; p.y++) {
	for (p.x = bounds->left; p.x <= bounds->right; p.x++) {
		SECTOR *sector = sector_at(&p);

		if (sector->terrain != NULL) {

			sector->terrain->gore_level = GORE_LEVEL_ZERO;
		}

		if (sector->object != NULL) {

			sector->object->gore_level = GORE_LEVEL_ZERO;
		}
	}
	}
}





/*
 * turns the active area into ruins
 */
void area_make_ruins(const char *ruins)
{
	AREA_POINT p;
	
	/* iterate over all area sectors */
	for (p.y = 0 ; p.y < AREA_HEIGHT; p.y++) {
		
		for (p.x = 0; p.x < AREA_WIDTH; p.x++) {

			/* 33% chance that a sector becomes ruined */
			if (random_int(1, 100) <= 33) {
				
				area_ruin_sector(&p, ruins);
			}
		}
	}		
}



/*
 * adds random gore to the current area
 */
void area_add_random_gore(void)
{
	int n_features = random_int(4, 10);

	for ( ; n_features > 0; --n_features) {
		const AREA_POINT *point = random_sector(SC_FREE);
		
		splatter(point, GORE_LEVEL_CORPSE);
		
		splatter_body_parts(point);
	}		
}



/*
 * ruins the sector at the passed location
 */
static void area_ruin_sector(const AREA_POINT *sector_location,
	const char *ruins
)
{
	SECTOR *sector = sector_at(sector_location);
		
	/* object here .. */
	if (sector->object != NULL) {
			
		/* do not destroy connector objects */
		if (object_is_connector(sector->object)) return;

		/* destroy object */
		object_destroy(sector->object);
		sector->object = NULL;
	}

	/* air cannot be ruined */
	if (terrain_has_attribute(sector->terrain, TA_AIR)) return;
		
	/* destroy terrain */
	terrain_destroy(sector->terrain);
	sector->terrain = NULL;

	/* place ruins terrain */
	place_terrain(terrain_create(ruins), sector_location);
}






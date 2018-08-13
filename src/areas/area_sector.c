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
 * Module: Area Sector
 */

#include "wrogue.h"



static void	find_conforming_sectors(SECTOR_CLASS);

static bool	out_of_sector_bounds(const AREA_POINT *);

static bool	sc_any(const AREA_POINT *);

static bool	sc_free(const AREA_POINT *);

static bool	sc_free_object_location(const AREA_POINT *);
static bool	sc_free_object_location_antiblock(const AREA_POINT *);

static bool	sc_move_target(const AREA_POINT *);
static bool	sc_move_target_safe(const AREA_POINT *);

static bool	sc_elevator(const AREA_POINT *);
static bool	sc_way_up(const AREA_POINT *);
static bool	sc_way_down(const AREA_POINT *);

static bool	antiblock_check_passed(const AREA_POINT *);

static void	sector_description_add_gore(char *, GORE_LEVEL);




/*
 * sector class functions
 */
static bool (* const SectorClass[MAX_SECTOR_CLASSES])(const AREA_POINT *) = {

	sc_any,

	sc_free,

	sc_free_object_location,
	sc_free_object_location_antiblock,

	sc_move_target,
	sc_move_target_safe,

	sc_elevator,
	sc_way_up,
	sc_way_down

};


/*
 * sector function bounds
 */
static AREA_SECTION	SectorBounds;


/*
 * array holding the coordinates of the sectors which conform to
 * the requested sector class
 */
static AREA_POINT	ConformingSector[AREA_SIZE];

/* number of conforming sectors */
static N_SECTORS	N_ConformingSectors;




/*
 * sets the bounds for the sector functions
 */
void sector_set_bounds(const AREA_SECTION *bounds)
{

	SectorBounds = *bounds;
}



/*
 * sets the bounds for the sector functions back to the default bounds
 */
void sector_reset_bounds(void)
{

	SectorBounds = *area_bounds();
}



/*
 * returns a string which describes the passed sector
 */
char * sector_description(char *description_string, const SECTOR *sector)
{
	const CHARACTER *character;
	const OBJECT *object;
	const TERRAIN *terrain;

	character = sector->character;
	object = sector->object;
	terrain = sector->terrain;

	if (character != NULL && (faction_relationship(character->faction, 
		FACTION_PLAYER) != FR_HOSTILE ||
		!character_unnoticed(character))) {

		strcpy(description_string, character->name);

		sprintf(description_string, "%s (%d/%d)",
			character->name,
			character->injury,
			injury_max(character)
		);

	} else if (object != NULL) {

		if (object->charge != CHARGE_NIL) {

			sprintf(description_string, "%s [%d]",
				object->name,
				object->charge
			);

		} else {

			sprintf(description_string, "%s",
				object->name
			);
		}
		
		sector_description_add_gore(description_string, 
			object->gore_level
		);

	} else if (terrain != NULL) {

		strcpy(description_string, terrain->name);
		
		sector_description_add_gore(description_string, 
			terrain->gore_level
		);
	}

	return description_string;
}



/*
 * returns true if the passed sector conforms to the passed class
 */
bool sector_is_class(const AREA_POINT *sector_location,
	SECTOR_CLASS sector_class
)
{

	return (*SectorClass[sector_class])(sector_location);
}



/*
 * returns the location of an adjacent sector (adjacent to the 
 * passed point) which conforms to the requested class, or
 * NULL if no such sector could be found
 */
const AREA_POINT * adjacent_sector(const AREA_POINT *p, SECTOR_CLASS wanted_class)
{
	N_SECTORS i;
	AREA_DISTANCE adjacent_p_distance = AREA_DISTANCE_MAX;
	const AREA_POINT *adjacent_p = NULL;

	find_conforming_sectors(wanted_class);
	
	if (N_ConformingSectors == 0) return NULL;

	for (i = 0; i < N_ConformingSectors; i++) {
		AREA_DISTANCE distance;
		
		distance = area_distance(&ConformingSector[i], p);

		if (distance < adjacent_p_distance) {

			adjacent_p = &ConformingSector[i];
			adjacent_p_distance = distance;
		}
	}

	return adjacent_p;
}



/*
 * returns the location of a randomly chosen sector which conforms
 * to the requested class, or NULL if no such sector could be found
 */
const AREA_POINT * random_sector(SECTOR_CLASS wanted_class)
{

	find_conforming_sectors(wanted_class);
	
	if (N_ConformingSectors == 0) return NULL;

	return &ConformingSector[random_int(0, N_ConformingSectors - 1)];
}



/*
 * finds all sectors which conform to the requested class
 */
static void find_conforming_sectors(SECTOR_CLASS wanted_class)
{
	AREA_POINT p;
	bool (*conforms)(const AREA_POINT *);

	N_ConformingSectors = 0;
	conforms = SectorClass[wanted_class];

	for (p.y = SectorBounds.top; p.y <= SectorBounds.bottom; p.y++) {
	for (p.x = SectorBounds.left; p.x <= SectorBounds.right; p.x++) {

		if ((*conforms)(&p)) {
			
			ConformingSector[N_ConformingSectors++] = p;
		}
	}
	}
}



/*
 * returns true if the passed point is out of bounds
 */
static bool out_of_sector_bounds(const AREA_POINT *p)
{

	if (p->y < SectorBounds.top ||
		p->x < SectorBounds.left ||
		p->y > SectorBounds.bottom ||
		p->x > SectorBounds.right) {

		return true;
	}

	return false;
}



/*
 * sector class 'any'
 */
static bool sc_any(const AREA_POINT *sector_location)
{

	NOT_USED(sector_location);
	
	return true;
}



/*
 * sector class 'free'
 * i.e. a sector with plain terrain and without an object or a character
 */
static bool sc_free(const AREA_POINT *sector_location)
{
	const SECTOR *sector;

	sector = sector_at(sector_location);

	if (sector->character != NULL) {

		return false;
	}

	if (sector->object != NULL) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_IMPASSABLE)) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_DANGEROUS)) {

		return false;
	}

	return true;
}



/*
 * sector class 'free object location'
 * i.e. one can place an object there
 */
static bool sc_free_object_location(const AREA_POINT *sector_location)
{
	const SECTOR *sector;

	sector = sector_at(sector_location);

	if (sector->object != NULL) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_IMPASSABLE)) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_DANGEROUS)) {

		return false;
	}

	return true;
}



/*
 * sector class 'free object location antiblock'
 * i.e. one can place an impassable object there without blocking
 *      access to other sectors
 */
static bool sc_free_object_location_antiblock(const AREA_POINT *sector_location)
{

	if (sc_free_object_location(sector_location) &&
		antiblock_check_passed(sector_location)) {

		return true;
	}

	return false;
}



/*
 * sector class 'move target'
 * i.e. a character can move there
 */
static bool sc_move_target(const AREA_POINT *sector_location)
{
	const SECTOR *sector;

	sector = sector_at(sector_location);

	if (sector->character != NULL) {

		return false;
	}

	if (sector->object != NULL &&
		object_has_attribute(sector->object, OA_IMPASSABLE)) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_IMPASSABLE)) {

		return false;
	}

	return true;
}



/*
 * sector class 'move target safe'
 * i.e. a character can safely move there
 */
static bool sc_move_target_safe(const AREA_POINT *sector_location)
{
	const SECTOR *sector;

	sector = sector_at(sector_location);

	if (sector->character != NULL) {

		return false;
	}

	if (sector->object != NULL &&
		object_has_attribute(sector->object, OA_IMPASSABLE)) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_IMPASSABLE)) {

		return false;
	}

	if (terrain_has_attribute(sector->terrain, TA_DANGEROUS)) {

		return false;
	}

	return true;
}



/*
 * sector class 'elevator'
 * i.e. the sector contains an elevator object
 */
static bool sc_elevator(const AREA_POINT *sector_location)
{
	const OBJECT *object = object_at(sector_location);

	if (object == NULL) return false;

	if (object->subtype == OSTYPE_ELEVATOR) return true;

	return false;
}



/*
 * sector class 'way up'
 * i.e. the sector contains a way up object
 */
static bool sc_way_up(const AREA_POINT *sector_location)
{
	const OBJECT *object = object_at(sector_location);

	if (object == NULL) return false;

	if (object->subtype == OSTYPE_WAY_UP) return true;

	return false;
}



/*
 * sector class 'way down'
 * i.e. the sector contains a way down object
 */
static bool sc_way_down(const AREA_POINT *sector_location)
{
	const OBJECT *object = object_at(sector_location);

	if (object == NULL) return false;

	if (object->subtype == OSTYPE_WAY_DOWN) return true;

	return false;
}



/*
 * antiblock check
 */
static bool antiblock_check_passed(const AREA_POINT *sector_location)
{
	int y_mod, x_mod;

	for (y_mod = -1; y_mod <= 1; y_mod++) {
	for (x_mod = -1; x_mod <= 1; x_mod++) {
		AREA_POINT p;

		if (x_mod == 0 && y_mod == 0) {

			continue;
		}

		p.y = sector_location->y + y_mod;
		p.x = sector_location->x + x_mod;

		if (out_of_sector_bounds(&p)) {

			return false;
		}

		if (!sc_move_target(&p)) {

			return false;
		}
	}
	}

	return true;
}



/*
 * adds the gore description to a sector description 
 */
static void sector_description_add_gore(char *description, GORE_LEVEL gore_level)
{
	
	if (gore_level <= GORE_LEVEL_ZERO) {
		
		return;
	}

	strcat(description, " (");
	strcat(description, gore_level_name(gore_level));
	strcat(description, ")");
}



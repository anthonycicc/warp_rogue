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
 * Module: Area Transition
 */

#include "wrogue.h"



static LIST *		CharacterTransferList = NULL;
static LIST *		EventTransferList = NULL;


static void		grab_characters_to_transfer(void);

static void		transfer_characters(DIRECTION);

static AREA_POINT *	location_after_transition(AREA_POINT *, DIRECTION);




/*
 * moves the player's party to a different area
 */
void move_party_to_area(const PLANET_POINT *target_point,
	DIRECTION direction, bool from_nowhere
)
{
	/* regular area transition */
	if (!from_nowhere) {
		
		save_point_create();

		grab_characters_to_transfer();

		area_write(area_file_name(&active_area()->location));

		area_clear();

	/* special case: the PC is coming from "nowhere" */
	} else {

		CharacterTransferList = list_new();
		EventTransferList = list_new();

		list_add(CharacterTransferList, player_character());
	}

	area_read(area_file_name(target_point));

	transfer_characters(direction);

	update_view_character(player_controlled_character());
}



/*
 * grabs all characters which are supposed to be transfered
 */
static void grab_characters_to_transfer(void)
{
	LIST_NODE *node;

	CharacterTransferList = list_new();
	EventTransferList = list_new();

	for (node = area_character_list()->head;
		node != NULL;
		) {
		CHARACTER *character;

		character = (CHARACTER *)node->data;
		node = node->next;

		if (character->party != PARTY_PLAYER) {

			continue;
		}

		remove_character(character, EventTransferList);

		list_add(CharacterTransferList, character);
 	}
}



/*
 * transfers characters
 */
static void transfer_characters(DIRECTION move_direction)
{
	LIST_NODE *node;
	AREA_POINT spawn_start;

	location_after_transition(&spawn_start, move_direction);

	for (node = CharacterTransferList->head;
		node != NULL;
		node = node->next) {
		CHARACTER *character;
		const AREA_POINT *spawn_point;

		character = (CHARACTER *)node->data;

		if (sector_is_class(&spawn_start, SC_MOVE_TARGET_SAFE)) {

			spawn_point = &spawn_start;

		} else {

			spawn_point = adjacent_sector(&spawn_start,
				SC_MOVE_TARGET_SAFE
			);

			if (spawn_point == NULL) {

				die("*** CORE ERROR *** no room "\
					"to transfer characters"
				);
			}
		}

		place_character(character, spawn_point);
	}

	list_free(CharacterTransferList);

	list_add_list(area_event_list(), EventTransferList);

	list_free(EventTransferList);
}



/*
 * returns the location at which the party starts after transition
 */
static AREA_POINT * location_after_transition(AREA_POINT *location,
	DIRECTION move_direction
)
{
	const AREA_SECTION *bounds;

	bounds = area_bounds();

	if (move_direction == DIRECTION_ELEVATOR) {

		*location = *random_sector(SC_ELEVATOR);

	} else if (move_direction == DIRECTION_WAY_UP) {

		*location = *random_sector(SC_WAY_DOWN);

	} else if (move_direction == DIRECTION_WAY_DOWN) {

		*location = *random_sector(SC_WAY_UP);

	} else if (move_direction == DIRECTION_LEFT) {

		location->x = bounds->right;
		location->y = bounds->bottom / 2;

	} else if (move_direction == DIRECTION_RIGHT) {

		location->x = bounds->left;
		location->y = bounds->bottom / 2;

	} else if (move_direction == DIRECTION_UP) {

		location->x = bounds->right / 2;
		location->y = bounds->bottom;

	} else if (move_direction == DIRECTION_DOWN) {

		location->x = bounds->right / 2;
		location->y = bounds->top;

	} else if (move_direction == DIRECTION_UP_LEFT) {

		location->x = bounds->right;
		location->y = bounds->bottom;

	} else if (move_direction == DIRECTION_UP_RIGHT) {

		location->x = bounds->left;
		location->y = bounds->bottom;

	} else if (move_direction == DIRECTION_DOWN_LEFT) {

		location->x = bounds->right;
		location->y = bounds->top;

	} else if (move_direction == DIRECTION_DOWN_RIGHT) {

		location->x = bounds->left;
		location->y = bounds->top;
	}

	return location;
}


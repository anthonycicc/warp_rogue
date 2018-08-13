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
 * Module: Character Actions
 */

#include "wrogue.h"




/*
 * action: do nothing
 */
void action_do_nothing(CHARACTER *character)
{

	character->action_spent = true;
}



/*
 * action: recover
 */
void action_recover(CHARACTER *character)
{

	character_recover(character);

	character->action_spent = true;
}



/*
 * action: move
 */
bool action_move(CHARACTER *character, const AREA_POINT *target_point)
{

	if (!move_character(character, target_point)) {

		return false;
	}

	character->action_spent = true;

	return true;
}



/*
 * action: push past
 */
void action_push_past(CHARACTER *character, CHARACTER *c2)
{

	push_past(character, c2);

	character->action_spent = true;
}



/*
 * action: run
 */
bool action_run(CHARACTER *character, const PATH_NODE *path, AREA_DISTANCE n_steps)
{

	if (!run(character, path, n_steps)) {

		return false;
	}

	character->action_spent = true;

	return true;
}



/*
 * action: jump
 */
void action_jump(CHARACTER *character, const AREA_POINT *target_point)
{

	move_character(character, target_point);

	character->action_spent = true;
}



/*
 * action: strike
 */
void action_strike(CHARACTER *character, OBJECT *weapon,
	const AREA_POINT *target_point
)
{

	strike(character, weapon, target_point);

	character->action_spent = true;
}



/*
 * action: shoot
 */
void action_shoot(CHARACTER *character, OBJECT *weapon,
	FIRING_DATA *firing_data, const AREA_POINT *target_point
)
{

	shoot(character, weapon, firing_data, target_point);

	character->action_spent = true;
}



/*
 * action: reload weapon
 */
void action_reload_weapon(CHARACTER *character)
{
	OBJECT *weapon = character->weapon;
	EVENT *event;
	TIMER timer;

	if (object_has_attribute(weapon, OA_PARTIAL_RELOAD)) {

		action_partial_reload(character);
		return;
	}

	timer = weapon->reload;

	/* Quickload perk effect */
	if (character_has_perk(character, PK_QUICKLOAD) && 
		timer >= 2) {

		timer /= 2;
	}

	event = event_create(EVT_RELOAD_WEAPON, timer);
	event->character = character;
	event->object = weapon;
	event_set(event);

	character_set_attribute(character, CA_BUSY);

	character->action_spent = true;
}



/*
 * action: reload weapon (partial reload)
 */
void action_partial_reload(CHARACTER *character)
{
	const OBJECT *weapon;

	weapon = character->weapon;

	partial_reload(character);

	if (character_has_perk(character, PK_QUICKLOAD) &&
		weapon->charge < weapon->charge_max) {

		partial_reload(character);
	}

	character->action_spent = true;
}



/*
 * action: unjam weapon
 */
void action_unjam_weapon(CHARACTER *character)
{
	EVENT *event;

	event = event_create(EVT_UNJAM_WEAPON, 2);
	event->character = character;
	event->object = character->weapon;
	event_set(event);

	character_set_attribute(character, CA_BUSY);

	character->action_spent = true;
}



/*
 * action: switch weapons
 */
void action_switch_weapons(CHARACTER *character)
{

	switch_weapons(character);

	if (!character_has_perk(character, PK_FAST_DRAW)) {

		character->action_spent = true;
	}
}



/*
 * action: disarm
 */
void action_disarm(CHARACTER *character, CHARACTER *target)
{

	disarm_use(character, target);

	if (character_unnoticed(character)) {

		notice_check(character, false, true);
	}

	character->action_spent = true;
}



/*
 * action: evoke psychic power
 */
bool action_evoke_psy_power(CHARACTER *character, PSY_POWER power,
	const AREA_POINT *target_point
)
{
	bool silent;
	bool power_evoked;

	if (character_has_perk(character, PK_SILENT_EVOCATION)) {

		silent = true;

	} else {

		silent = false;
	}

	power_evoked = evoke_psy_power(character, power, target_point);

	if (character_unnoticed(character)) {

		notice_check(character, silent, true);
	}

	character->action_spent = true;

	return power_evoked;
}



/*
 * action: pick up object
 */
void action_pick_up_object(CHARACTER *character, const AREA_POINT *location)
{
	OBJECT *object;
	SECTOR *sector;

	sector = sector_at(location);

	object = sector->object;

	dynamic_message(MSG_PICK_UP, character, object,
		MOT_OBJECT
	);

	inventory_add(character, object);

	sector->object = NULL;

	character->action_spent = true;
}



/*
 * action: equip object
 */
bool action_equip_object(CHARACTER *character, OBJECT *object, OBJECT **slot)
{

	if (!equip_object(character, object, slot)) {

		return false;
	}

	character->action_spent = true;

	return true;
}



/*
 * action: unequip object
 */
bool action_unequip_object(CHARACTER *character, OBJECT *object)
{
	bool unequip_weapon;

	if (object == character->weapon) {

		unequip_weapon = true;

	} else {

		unequip_weapon = false;
	}

	if (!unequip_object(character, object)) {

		return false;
	}

	if (unequip_weapon &&
		!is_player_controlled_character(character)) {

		dynamic_message(MSG_FIGHTS_UNARMED, character,
			NULL, MOT_NIL
		);
	}

	character->action_spent = true;

	return true;
}



/*
 * action: drop object
 */
void action_drop_object(CHARACTER *character, OBJECT *object, bool passive)
{
	const AREA_POINT *target_point;

	if (!passive) {

		character->action_spent = true;
	}

	inventory_remove(character, object);

	if (sector_is_class(&character->location, SC_FREE_OBJECT_LOCATION)) {

		place_object(object, &character->location);

		return;
	}

	target_point = adjacent_sector(&character->location,
		SC_FREE_OBJECT_LOCATION
	);

	if (target_point == NULL) {

		object_destroy(object);
		return;
	}

	place_object(object, target_point);
}



/*
 * action: use drug
 */
bool action_use_drug(CHARACTER *character, OBJECT *object)
{

	if (!use_drug(character, object)) {

		return false;
	}

	character->action_spent = true;

	return true;
}



/*
 * action: stealth
 */
bool action_stealth(CHARACTER *character)
{

	if (stealth_use(character)) {

		character->action_spent = true;
		return true;
	}

	return false;
}





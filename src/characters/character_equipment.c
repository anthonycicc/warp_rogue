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
 * Module: Character Equipment
 */

#include "wrogue.h"


static void		use_weapon(CHARACTER *, OBJECT *);
static void		use_armour(CHARACTER *, OBJECT *);
static void		use_jump_pack(CHARACTER *, OBJECT *);

static void		equip_object_effect(CHARACTER *, OBJECT *);

static void		unequip_object_effect(CHARACTER *, OBJECT *);

static void		weapon_overheats(ATTACK_DATA *);

static void		weapon_explodes(ATTACK_DATA *);




/*
 * firing mode names
 */
static const char *	FiringModeName[MAX_FIRING_MODES] = {

	"S",
	"SA",
	"A"

};



/*
 * equipment object "Use"d
 */
void use_equipment_object(CHARACTER *character, OBJECT *object)
{

	switch (object->type) {

	case OTYPE_RANGED_COMBAT_WEAPON: /* FALLTHROUGH */
	case OTYPE_CLOSE_COMBAT_WEAPON:

		use_weapon(character, object);
		break;

	case OTYPE_ARMOUR:

		use_armour(character, object);
		break;

	case OTYPE_JUMP_PACK:

		use_jump_pack(character, object);
		break;

	default:
		die("*** CORE ERROR *** invalid use_equipment_object() call");
	}
}



/*
 * returns true if the passed character uses the passed object
 */
bool object_in_use(const CHARACTER *character, const OBJECT *object)
{

	if (character->weapon == object ||
		character->secondary_weapon == object ||
		character->armour == object ||
		character->jump_pack == object) {

		return true;
	}

	return false;
}



/*
 * returns true if the passed character uses a noisy object
 * at the moment
 */
bool noisy_object_in_use(const CHARACTER *character)
{
	LIST_NODE *node;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object;

		object = (OBJECT *)node->data;

		if (object_has_attribute(object, OA_NOISY) &&
			object_in_use(character, object)) {

			return true;
		}
	}

	return false;
}



/*
 * reload weapon event
 */
void reload_weapon(EVENT *event)
{

	event->object->charge = event->object->charge_max;

	character_remove_attribute(event->character, CA_BUSY);

	dynamic_message(MSG_RELOAD_WEAPON, event->character,
		NULL, MOT_NIL
	);
}



/*
 * partial reload
 */
void partial_reload(CHARACTER *character)
{
	OBJECT *weapon;

	weapon = character->weapon;

	weapon->charge += weapon->reload;

	if (weapon->charge > weapon->charge_max) {

		weapon->charge = weapon->charge_max;
	}

	dynamic_message(MSG_RELOAD_WEAPON, character, NULL, MOT_NIL);
}



/*
 * recharge weapon event
 */
void recharge_weapon(EVENT *event)
{

	event->object->charge = event->object->charge_max;

	dynamic_message(MSG_RECHARGE_WEAPON, event->character,
		event->object, MOT_OBJECT
	);
}



/*
 * switches a character's weapons
 */
void switch_weapons(CHARACTER *character)
{
	OBJECT *h;

	h = character->weapon;
	character->weapon = character->secondary_weapon;
	character->secondary_weapon = h;

	if (character->weapon != NULL &&
		object_has_attribute(character->weapon, OA_INHERENT)) {

		return;
	}

	dynamic_message(MSG_SWITCH_WEAPONS, character,
		character->weapon, MOT_OBJECT
	);
}



/*
 * equips an object
 */
bool equip_object(CHARACTER *character, OBJECT *object, OBJECT **slot)
{

	if (*slot != NULL) {

		return false;
	}

	*slot = object;

	equip_object_effect(character, object);

	return true;
}



/*
 * unequips an object
 */
bool unequip_object(CHARACTER *character, OBJECT *object)
{

	if (object == character->weapon) {
		EVENT *removed_event;

		removed_event = remove_character_event(
			character, EVT_RELOAD_WEAPON
		);

		if (removed_event != NULL) {

			event_destroy(removed_event);
		}

		removed_event = remove_character_event(
			character, EVT_UNJAM_WEAPON
		);

		if (removed_event != NULL) {

			event_destroy(removed_event);
		}

		removed_event = remove_character_event(
			character, EVT_RECHARGE_WEAPON
		);

		if (removed_event != NULL) {

			removed_event->character = NULL;
			event_set(removed_event);
		}

		character->weapon = NULL;

	} else if (object == character->secondary_weapon) {

		character->secondary_weapon = NULL;

	} else if (object == character->armour) {

		character->armour = NULL;

	} else if (object == character->jump_pack) {

		character->jump_pack = NULL;
	}

	unequip_object_effect(character, object);

	return true;
}



/*
 * automatically equips all suitable inventory items
 */
void equip_objects(CHARACTER *character)
{
	LIST_NODE *node;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object;

		object = (OBJECT *)node->data;

		if (object_in_use(character, object)) {

			continue;
		}

		if (object->type == OTYPE_CLOSE_COMBAT_WEAPON ||
			object->type == OTYPE_RANGED_COMBAT_WEAPON) {

			if (character->weapon == NULL) {

				equip_object(character, object,
					&character->weapon
				);

			} else if (character->secondary_weapon == NULL) {

				equip_object(character, object,
					&character->secondary_weapon
				);
			}

		} else if (object->type == OTYPE_ARMOUR) {

			if (character->armour != NULL) continue;

			equip_object(character, object, &character->armour);

		} else if (object->type == OTYPE_JUMP_PACK) {

			if (character->jump_pack != NULL) continue;

			equip_object(character, object,
				&character->jump_pack
			);
		}
	}
}



/*
 * weapon jamming check / effect
 * returns true if the character's weapon has jammed
 */
bool weapon_has_jammed(ATTACK_DATA *attack)
{
	OBJECT *weapon = attack->weapon;

	/* not a projectile weapon OR a reliable weapon .. */
	if (!object_has_attribute(weapon, OA_PROJECTILE) ||
		object_has_attribute(weapon, OA_RELIABLE)) {

		/* return false */
		return false;
	}

	/* character has weapon maintenance perk AND 
		hit roll < 100 .. */
	if (character_has_perk(attack->attacker, PK_WEAPON_MAINTENANCE) &&
		attack->hit_roll < 100) {

		/* return false */
		return false;
	}

	/* output weapon jams message */
	dynamic_message(MSG_WEAPON_JAMS, attack->attacker, NULL, MOT_NIL);

	/* jam weapon */
	object_set_attribute(weapon, OA_JAMMED);

	/* return true */
	return true;
}



/*
 * laser weapon malfunction check / effect
 */
bool laser_weapon_malfunction(ATTACK_DATA *attack)
{

	if (!object_has_attribute(attack->weapon, OA_LASER) ||
		object_has_attribute(attack->weapon, OA_RELIABLE)) {

		return false;
	}

	if (character_has_perk(attack->attacker, PK_WEAPON_MAINTENANCE) &&
		attack->hit_roll < 100) {

		return false;
	}

	dynamic_message(MSG_WEAPON_MALFUNCTIONS, attack->attacker,
		NULL, MOT_NIL
	);

	attack->weapon->charge = 0;

	return true;
}



/*
 * plasma weapon malfunction check / effect
 */
bool plasma_weapon_malfunction(ATTACK_DATA *attack)
{
	int result_roll;

	if (!object_has_attribute(attack->weapon, OA_PLASMA) ||
		object_has_attribute(attack->weapon, OA_RELIABLE)) {

		return false;
	}

	if (character_has_perk(attack->attacker, PK_WEAPON_MAINTENANCE) &&
		attack->hit_roll < 100) {

		return false;
	}

	result_roll = dice(1, 6);

	if (result_roll <= 4) {

		weapon_overheats(attack);

	} else {

		weapon_explodes(attack);
	}

	return true;
}



/*
 * unjam weapon event
 */
void unjam_weapon(EVENT *event)
{

	object_remove_attribute(event->object, OA_JAMMED);
	
	character_remove_attribute(event->character, CA_BUSY);

	dynamic_message(MSG_UNJAM_WEAPON, event->character,
		NULL, MOT_NIL
	);
}



/*
 * returns true if the passed object is a knife
 */
bool is_knife(const OBJECT *object)
{

	if (object == NULL) {

		return false;
	}

	if (object->subtype == OSTYPE_KNIFE) {

		return true;
	}

	return false;
}



/*
 * returns true if the passed object can be reloaded
 */
bool can_be_reloaded(const OBJECT *object)
{

	if (object->reload == RELOAD_NO ||
		object_has_attribute(object, OA_AUTOMATIC_RECHARGE)) {

		return false;
	}

	return true;
}



/*
 * firing mode index -> name
 */
const char * firing_mode_name(FIRING_MODE firing_mode)
{

	return FiringModeName[firing_mode];
}



/*
 * firing mode name -> index
 */
FIRING_MODE name_to_firing_mode(const char *name)
{
	FIRING_MODE i;

	for (i = 0; i < MAX_FIRING_MODES; i++) {

		if (strings_equal(name, FiringModeName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid firing mode: %s", name);

	return FMODE_NIL;
}



/*
 * weapon "Use"d
 */
static void use_weapon(CHARACTER *character, OBJECT *object)
{

	if (character->weapon == NULL &&
		object != character->secondary_weapon) {

		action_equip_object(character, object, &character->weapon);

	} else if (character->weapon != NULL &&
		!object_in_use(character, object)) {

		if (character->secondary_weapon != NULL) {

			message(SID_INVENTORY,
				"Already another weapon in use"
			);

			return;
		}

		action_equip_object(character, object,
			&character->secondary_weapon
		);

	} else {

		action_unequip_object(character, object);
	}
}



/*
 * armour "Use"ed
 */
static void use_armour(CHARACTER *character, OBJECT *object)
{

	if (character->armour == NULL) {

		action_equip_object(character, object, &character->armour);

	} else if (character->armour == object) {

		action_unequip_object(character, object);

	} else if (character->armour != NULL) {

		message(SID_INVENTORY, "Already another armour in use");
	}

}



/*
 * jump pack "Use"ed
 */
static void use_jump_pack(CHARACTER *character, OBJECT *object)
{

	if (character->jump_pack == NULL) {

		action_equip_object(character, object, &character->jump_pack);

	} else if (character->jump_pack == object) {

		action_unequip_object(character, object);

	} else if (character->jump_pack != NULL) {

		message(SID_INVENTORY, "Already another jump pack in use");
	}
}



/*
 * equip object effect
 */
static void equip_object_effect(CHARACTER *character, OBJECT *object)
{

	if (object->type == OTYPE_ARMOUR) {

		character->armour_rating.current +=
			object->armour_value;

		character->armour_rating.total +=
			object->armour_value;

		if (object->subtype != OSTYPE_LIGHT &&
			character_unnoticed(character)) {

			cancel_stealth(character, NULL);
		}
	}

	if (object_has_attribute(object, OA_STRENGTH_BOOST)) {

		character->stat[S_ST].current += STRENGTH_BOOST_BONUS;
	}

	if (object_has_attribute(object, OA_STRENGTH_BOOST_2)) {

		character->stat[S_ST].current += STRENGTH_BOOST_2_BONUS;
	}

	if (object_has_attribute(object, OA_NOISY) &&
		character_unnoticed(character)) {

		cancel_stealth(character, NULL);
	}
}



/*
 * unequip object effect
 */
static void unequip_object_effect(CHARACTER *character, OBJECT *object)
{

	if (object->type == OTYPE_ARMOUR) {

		character->armour_rating.current -=
			object->armour_value;

		character->armour_rating.total -=
			object->armour_value;
	}

	if (object->attribute[OA_STRENGTH_BOOST]) {
		character->stat[S_ST].current -= STRENGTH_BOOST_BONUS;
	}

	if (object->attribute[OA_STRENGTH_BOOST_2]) {
		character->stat[S_ST].current -= STRENGTH_BOOST_2_BONUS;
	}

}



/*
 * weapon overheats effect
 */
static void weapon_overheats(ATTACK_DATA *attack)
{

	dynamic_message(MSG_WEAPON_OVERHEATS, attack->attacker,
		NULL, MOT_NIL
	);

	action_drop_object(attack->attacker, attack->weapon, false);

	attack->weapon = NULL;
}



/*
 * weapon explodes effect
 */
static void weapon_explodes(ATTACK_DATA *attack)
{
	const DICE_ROLL *power;
	DAMAGE damage;
	CHARACTER *attacker;

	attacker = attack->attacker;

	power = &attack->weapon->damage;

	damage = dice(power->n_dice, power->n_sides) +
			power->modifier;

	damage -= attacker->armour_rating.current;

	if (damage <= 0) {

		return;
	}

	attacker->injury += damage;

	dynamic_message(MSG_WEAPON_EXPLODES, attacker, NULL, MOT_NIL);

	inventory_remove(attacker, attack->weapon);

	object_destroy(attack->weapon);

	attack->weapon = NULL;

	handle_destruction(&attacker->location, NULL, DT_WEAPON_EXPLOSION);
}



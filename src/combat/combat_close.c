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
 * Module: Combat Close
 */

#include "wrogue.h"



static void		hit_value_strike(ATTACK_DATA *);

static bool		strike_parried(const ATTACK_DATA *,
				const CHARACTER *
			);

static WEAPON_REACH	effective_reach(const CHARACTER *, const OBJECT *);

static REACH_MODIFIER	reach_modifier(const CHARACTER *, const OBJECT *,
				const CHARACTER *, const OBJECT *
			);




/*
 * executes a strike / strikes
 */
void strike(CHARACTER *attacker, OBJECT *attacker_weapon,
	const AREA_POINT *target_point
)
{
	ATTACK_DATA attack;
	N_STRIKES n_strikes;
	CHARACTER *target;

	attack_init(&attack);
	attack.type = AT_STRIKE;
	attack.attacker = attacker;
	attack.weapon = attacker_weapon;
	attack.target_point = *target_point;
	attack.distance = 1;

	if (character_has_perk(attacker, PK_SNEAK_ATTACK) &&
		character_unnoticed(attacker)) {

		attack.sneak = true;
	}

	n_strikes = 1;

	if (character_has_perk(attacker, PK_RAPID_STRIKE) &&
		rapid_strike_requirements_met(attacker,
			attacker_weapon)) {

		++n_strikes;

		attack.rapid = true;
	}

	target = character_at(target_point);

	while (n_strikes-- > 0) {

		attack.hit_roll = d100();

		hit_value_strike(&attack);

		if (!target_hit(&attack)) {

			miss(&attack);

			continue;
		}

		if (target != NULL &&
			strike_parried(&attack, target)) {

			dynamic_message(MSG_PARRY, target,
				attacker, MOT_CHARACTER
			);

			continue;
		}

		hit(&attack);
	}

	handle_destruction(target_point, attacker, DT_ATTACK);

	if (character_unnoticed(attacker)) {

		notice_check(attacker, false, true);
	}
}



/*
 * returns true if the passed character is involved in close combat
 */
bool involved_in_close_combat(const CHARACTER *character)
{
	int y_mod, x_mod;

	for (y_mod = -1; y_mod <= 1; y_mod++) {
	for (x_mod = -1; x_mod <= 1; x_mod++) {
		const CHARACTER *adjacent_character;
		AREA_POINT p;

		if (x_mod == 0 && y_mod == 0) {

			continue;
		}

		p.y = character->location.y + y_mod;
		p.x = character->location.x + x_mod;

		if (out_of_area_bounds(&p)) {

			continue;
		}

		adjacent_character = character_at(&p);

		if (adjacent_character != NULL &&
			hostility_between(character, adjacent_character)) {

			return true;
		}
	}
	}

	return false;
}



/*
 * returns the unarmed damage power of the passed character
 */
DICE_ROLL * unarmed_damage_power(const CHARACTER *character, 
	DICE_ROLL *damage_dice
)
{
	N_DICE n_dice = 1; /* default is 1 dice */
	
	/* Unarmed combat perk doubles damage dice */
	if (character_has_perk(character, PK_UNARMED_COMBAT)) {
		
		n_dice *= 2;
	}
	
	damage_dice->n_dice = n_dice;
	
	/* the number of damage dice sides depends on Strength */ 
	damage_dice->n_sides = character->stat[S_ST].current / 10;
	
	/* unarmed damage dice has no modifier */
	damage_dice->modifier = 0;
	
	return damage_dice;
}




/*
 * determines the hit value of a strike
 */
static void hit_value_strike(ATTACK_DATA *attack)
{
	SECTOR *sector;
	CHARACTER *target;

	sector = sector_at(&attack->target_point);
	target = sector->character;

	if (target == NULL) {

		if (sector->object != NULL) {

			attack->hit_value = OBJECT_HIT_VALUE;

		} else {

			attack->hit_value = 0;
		}

		return;
	}

	attack->hit_value = attack->attacker->stat[S_CC].current;

	attack->hit_value += reach_modifier(
		attack->attacker, attack->weapon,
		target, target->weapon
	);

	if (character_has_perk(target, PK_UNCANNY_DODGE)) {

		attack->hit_value += uncanny_dodge_modifier(target);
	}
}



/*
 * returns true if a strike has been parried
 */
static bool strike_parried(const ATTACK_DATA *attack, const CHARACTER *target)
{
	PARRY_VALUE parry_value;
	PARRY_PENALTY parry_penalty;

	if (!character_can_act(target) ||
		character_unnoticed(attack->attacker) ||
		(attack->weapon != NULL &&
		object_has_attribute(attack->weapon, OA_CANNOT_BE_PARRIED))) {

		return false;
	}

	if (target->weapon == NULL ||
		target->weapon->type != OTYPE_CLOSE_COMBAT_WEAPON ||
		object_has_attribute(target->weapon, OA_CANNOT_PARRY)) {

		return false;
	}

	parry_value = target->stat[S_CC].current / 2;

	parry_penalty = target->weapon->parry_penalty;

	if (character_has_perk(target, PK_ENHANCED_PARRY)) {

		parry_penalty += ENHANCED_PARRY_BONUS;

		if (parry_penalty > 0) {

			parry_penalty = 0;
		}
	}

	parry_value += parry_penalty;

	parry_value += reach_modifier(target, target->weapon,
		attack->attacker, attack->weapon
	);

	if (d100_test_passed(parry_value)) {

		return true;
	}

	return false;
}



/*
 * returns the effective weapon reach
 */
static WEAPON_REACH effective_reach(const CHARACTER *character,
	const OBJECT *weapon
)
{

	if (weapon == NULL) {

		return 0;
	}

	if (weapon->type != OTYPE_CLOSE_COMBAT_WEAPON) {

		return 0;
	}

	if (character_has_perk(character, PK_BLADEMASTER) &&
		weapon->subtype == OSTYPE_KNIFE) {

		return 4;
	}

	return weapon->reach;
}



/*
 * returns the reach modifier
 */
static REACH_MODIFIER reach_modifier(
	const CHARACTER *protagonist, const OBJECT *protagonist_weapon,
	const CHARACTER *antagonist, const OBJECT *antagonist_weapon
)
{
	WEAPON_REACH protagonist_reach, antagonist_reach;
	REACH_MODIFIER modifier;

	protagonist_reach = effective_reach(protagonist, protagonist_weapon);
	antagonist_reach = effective_reach(antagonist, antagonist_weapon);

	modifier = (protagonist_reach * 10) - (antagonist_reach * 10);

	return modifier;
}


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
 * Module: Combat
 */

#include "wrogue.h"



static void		hit_character(const ATTACK_DATA *, CHARACTER *);

static void		hit_character_message(const ATTACK_DATA *,
				const CHARACTER *
			);

static void		hit_object(const ATTACK_DATA *, OBJECT *);

static DAMAGE		base_damage(const ATTACK_DATA *);

static ARMOUR_VALUE	effective_armour_value(const ATTACK_DATA *,
				const CHARACTER *
			);
			
static bool		armour_penetrated(const ATTACK_DATA *,
				const CHARACTER *, DAMAGE
			);

static void 		pre_armour_penetration_effects(const ATTACK_DATA *,
				CHARACTER *, DAMAGE *
			);

static void		post_armour_penetration_effects(const ATTACK_DATA *,
				CHARACTER *
			);

static void		attack_effect_force(const ATTACK_DATA *, 
				CHARACTER *, DAMAGE *
			);

static void		attack_effect_shock(const ATTACK_DATA *, 
				CHARACTER *
			);

static void		attack_effect_pain(const ATTACK_DATA *, 
				CHARACTER *
			);

static void		attack_effect_disease(const ATTACK_DATA *, 
				CHARACTER *
			);

static void		attack_effect_poison(const ATTACK_DATA *, 
				CHARACTER *
			);

static bool		morale_test_passed(CHARACTER *);



/*
 * attack data template (default values)
 */
static const ATTACK_DATA AttackDataTemplate = {

	/* ATTACK TYPE */ AT_NIL, /* ATTACKER */ NULL, /* WEAPON */ NULL,
	/* FIRING DATA */ NULL,
	/* TARGET POINT */ {AREA_COORD_NIL, AREA_COORD_NIL},
	/* DISTANCE */ AREA_DISTANCE_NIL, /* SNEAK */ false,
	/* RAPID */ false, /* HIT VALUE */ 0, /* HIT ROLL */ 0,
	/* CRITICAL HIT */ false

};




/*
 * initializes an attack with default values
 */
void attack_init(ATTACK_DATA *attack)
{

	*attack = AttackDataTemplate;
}



/*
 * returns true if the target has been hit
 */
bool target_hit(ATTACK_DATA *attack)
{

	if (attack->sneak && !attack->rapid) {

		return true;
	}

	if (d100_test(attack->hit_roll, attack->hit_value) < 0) {

		return false;
	}

	return true;
}



/*
 * determines whether a critical hit has been scored
 */
void critical_hit_scored(ATTACK_DATA *attack)
{
	const CHARACTER *attacker;
	int critical_hit_value;

	attacker = attack->attacker;

	if (attack->sneak && !attack->rapid) {

		attack->critical_hit = true;

		return;
	}

	if (attack->type == AT_STRIKE &&
		character_has_perk(attacker, PK_BLADEMASTER) &&
		is_knife(attack->weapon)) {

		attack->critical_hit = true;

		return;
	}

	if (attack->hit_value <= 0) {

		return;
	}

	critical_hit_value =
		divide_and_round_up(attack->hit_value, 10);

	if (attack->type == AT_STRIKE &&
		character_has_perk(attacker, PK_CRITICAL_STRIKE)) {

		critical_hit_value *= 2;

	} else if (attack->type == AT_SHOT &&
		character_has_perk(attacker, PK_CRITICAL_SHOT) &&
		!attack->rapid) {

		critical_hit_value *= 2;
	}

	if (d100_test(attack->hit_roll, critical_hit_value) >= 0) {

		attack->critical_hit = true;
	}
}



/*
 * handles what happens in the case of a hit
 */
void hit(ATTACK_DATA *attack)
{
	SECTOR *sector;

	critical_hit_scored(attack);

	sector = sector_at(&(attack->target_point));

	if (sector->character != NULL) {

		hit_character(attack, sector->character);

	} else if (sector->object != NULL) {

		hit_object(attack, sector->object);
	}
}



/*
 * handles what happens in the case of a miss
 */
void miss(ATTACK_DATA *attack)
{
	SECTOR *sector;

	sector = sector_at(&(attack->target_point));

	if (sector->character != NULL) {

		dynamic_message(MSG_MISS,
			attack->attacker, sector->character,
			MOT_CHARACTER
		);

	} else if (sector->object != NULL) {

		dynamic_message(MSG_MISS,
			attack->attacker, sector->object,
			MOT_OBJECT
		);
	}
}



/*
 * hit character
 */
static void hit_character(const ATTACK_DATA *attack, CHARACTER *target)
{
	DAMAGE damage;

	/* output hit message */
	hit_character_message(attack, target);

	/* calculate base damage */
	damage = base_damage(attack);

	/* execute pre armour penetration effects */
	pre_armour_penetration_effects(attack, target, &damage);

	/* critical hit .. */
	if (attack->critical_hit) {

		/* multiply damage */
		damage *= CRITICAL_HIT_DAMAGE_MULTIPLIER;
	}

	/* deduct armour value from damage */
	damage -= effective_armour_value(attack, target);

	/* armour penetrated .. */
	if (armour_penetrated(attack, target, damage)) {
		
		/* execute post armour penetration effects */
		post_armour_penetration_effects(attack, target);
	}

	/* increase the injury value of the target*/ 
	target->injury += damage;

	/* morale test not passed .. */
	if (!morale_test_passed(target)) {
		
		/* activate morale broken effect */
		effect_activate(target, ET_BROKEN, 
			MORALE_BROKEN_EFFECT_DURATION
		);
	}
}



/*
 * hit character message
 */
static void hit_character_message(const ATTACK_DATA *attack,
	const CHARACTER *target
)
{

	if (attack->critical_hit) {

		dynamic_message(MSG_CRITICAL_HIT,
			attack->attacker, target,
			MOT_CHARACTER
		);

	} else {

		dynamic_message(MSG_HIT,
			attack->attacker, target,
			MOT_CHARACTER
		);
	}
}



/*
 * hit object
 */
static void hit_object(const ATTACK_DATA *attack, OBJECT *target)
{
	DAMAGE damage;

	dynamic_message(MSG_HIT, attack->attacker, target, MOT_OBJECT);

	damage = base_damage(attack);

	if (target->condition != CONDITION_INDESTRUCTABLE) {

		target->condition -= damage;
	}
}



/*
 * returns the base damage
 */
static DAMAGE base_damage(const ATTACK_DATA *attack)
{
	DICE_ROLL damage_dice;
	DAMAGE damage;

	if (attack->weapon == NULL) {

		unarmed_damage_power(attack->attacker, &damage_dice);

	} else {
		
		damage_dice = attack->weapon->damage;
	}

	damage = dice(damage_dice.n_dice, damage_dice.n_sides) +
		damage_dice.modifier;

	if (attack->type == AT_STRIKE) {
		int strength_bonus;

		strength_bonus = stat_bonus(attack->attacker, S_ST);

		if (character_has_perk(attack->attacker, 
			PK_CRUSHING_STRIKE)) {

			strength_bonus *=
				CRUSHING_STRIKE_STRENGTH_BONUS_MULTIPLIER;
		}

		damage += strength_bonus;
	}

	return damage;
}



/*
 * returns the effective armour value of the target
 */
static ARMOUR_VALUE effective_armour_value(const ATTACK_DATA *attack,
	const CHARACTER *target
)
{
	ARMOUR_VALUE armour_value;

	armour_value = target->armour_rating.current;

	if (character_has_perk(attack->attacker, PK_SPOT_WEAKNESS)) {

		armour_value /= SPOT_WEAKNESS_ARMOUR_VALUE_DIVIDER;
	}

	if (attack->weapon == NULL) {

		return armour_value;
	}

	if (object_has_attribute(attack->weapon, OA_POWER)) {

		armour_value /= POWER_WEAPON_ARMOUR_VALUE_DIVIDER;
	}

	if (object_has_attribute(attack->weapon, OA_IGNORE_ARMOUR)) {

		armour_value = 0;
	}

	return armour_value;
}



/*
 * returns true if the attack has penetrated the target's armour
 */
static bool armour_penetrated(const ATTACK_DATA *attack, 
	const CHARACTER *target, DAMAGE damage
)
{
	bool penetrated = false;

	/* no damage caused */
	if (damage <= 0) {
		const OBJECT *weapon = attack->weapon;
		
		/* no armour penetration except if 
			an environmental weapon was used AND
			the target has no environmental armour
		*/
		if (weapon != NULL && 
			object_has_attribute(weapon, OA_ENVIRONMENTAL) &&
			(target->armour == NULL ||
			!object_has_attribute(target->armour,
				OA_ENVIRONMENTAL))) {

			penetrated = true;
		}
		
	/* damage caused .. */
	}  else {
	
		/* armour penetraded */
		penetrated = true;
	}

	return penetrated;
}



/*
 * pre armour penetration effects
 */
static void pre_armour_penetration_effects(const ATTACK_DATA *attack,
	CHARACTER *target, DAMAGE *damage
)
{
	const CHARACTER *attacker = attack->attacker;
	const OBJECT *weapon = attack->weapon;

	/*
	 * weapon based effects 
	 */

	/* no weapon - return */
	if (weapon == NULL) return;

	/* force effect */
	if (object_has_attribute(weapon, OA_FORCE) &&
		character_has_perk(attacker, PK_PSYCHIC_WEAPONS)) {

		attack_effect_force(attack, target, damage);
	}
}



/*
 * post armour penetration effects
 */
static void post_armour_penetration_effects(const ATTACK_DATA *attack,
	CHARACTER *target
)
{
	const OBJECT *weapon = attack->weapon;
	
	/*
	 * weapon based effects 
	 */

	/* no weapon - return */
	if (weapon == NULL) return;

	/* shock effect */
	if (object_has_attribute(weapon, OA_SHOCK)) {
		attack_effect_shock(attack, target);
	}

	/* pain effect */
	if (object_has_attribute(weapon, OA_PAIN)) {
		attack_effect_pain(attack, target);
	}

	/* disease effect */
	if (object_has_attribute(weapon, OA_DISEASE)) {
		attack_effect_disease(attack, target);
	}

	/* poison effect */
	if (object_has_attribute(weapon, OA_POISON)) {
		attack_effect_poison(attack, target);
	}
}



/*
 * attack effect: force
 */
static void attack_effect_force(const ATTACK_DATA *attack, 
	CHARACTER *target, DAMAGE *damage
)
{
	const OBJECT *weapon = attack->weapon;
	int force_bonus;

	if (object_has_attribute(weapon, OA_FORCE_RUNE)) {

		force_bonus = FORCE_RUNE_DAMAGE_BONUS;

	} else {

		force_bonus = FORCE_DAMAGE_BONUS;
	}

	if (character_has_attribute(target, CA_DAEMONIC)) {

		force_bonus *= FORCE_VS_DAEMON_MULTIPLIER;
	}

	*damage += force_bonus;
}



/*
 * attack effect: shock
 */
static void attack_effect_shock(const ATTACK_DATA *attack, 
	CHARACTER *target
)
{
	NOT_USED(attack);

	/* toughness test passed .. */
	if (d100_test_passed(target->stat[S_TN].current)) {

		/* output resist message */
		dynamic_message(MSG_RESIST, target, NULL, MOT_NIL);

	/* test not passed .. */
	} else {

		/* activate stunned effect */
		effect_activate(target, ET_STUNNED, SHOCK_STUN_POWER);
	}
}



/*
 * attack effect: pain
 */
static void attack_effect_pain(const ATTACK_DATA *attack, 
	CHARACTER *target
)
{
	NOT_USED(attack);

	/* pain effect resisted */
	if (pain_resisted(target)) {

		/* output resist message */
		dynamic_message(MSG_RESIST, target,
			NULL, MOT_NIL
		);

	/* effect not resisted */
	} else {

		/* activate morale broken effect */
		effect_activate(target, ET_BROKEN,
			MORALE_BROKEN_EFFECT_DURATION
		);
	}
}



/*
 * attack effect: disease
 */
static void attack_effect_disease(const ATTACK_DATA *attack, 
	CHARACTER *target
)
{
	NOT_USED(attack);

	/* disease effect resisted */
	if (disease_resisted(target)) {

		/* output resist message */
		dynamic_message(MSG_RESIST, target,
			NULL, MOT_NIL
		);

	/* effect not resisted */
	} else {

		/* activate diseased effect */
		effect_activate(target, ET_DISEASED,
			TIMER_UNDETERMINED
		);
	}
}



/*
 * attack effect: poison
 */
static void attack_effect_poison(const ATTACK_DATA *attack, 
	CHARACTER *target
)
{
	NOT_USED(attack);

	/* poison effect resisted */
	if (poison_resisted(target)) {

		/* output resist message */
		dynamic_message(MSG_RESIST, target,
			NULL, MOT_NIL
		);

	/* effect not resisted */
	} else {

		/* activate poisoned effect */
		effect_activate(target, ET_POISONED,
			TIMER_UNDETERMINED
		);
	}
}



/*
 * the morale test
 *
 * returns true if passed
 */
static bool morale_test_passed(CHARACTER *character)
{
	int max_pain;

	max_pain = percent(character->stat[S_WP].current,
		injury_max(character)
	);

	if (character->injury <= max_pain) {

		return true;
	}

	if (pain_resisted(character)) {

		return true;
	}

	return false;
}


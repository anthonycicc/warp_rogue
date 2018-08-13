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
 * Module: Combat Ranged
 */

#include "wrogue.h"



static bool		fire_shot(ATTACK_DATA *);

static void		process_shot(ATTACK_DATA *);

static void		s_mode_fire(ATTACK_DATA *);

static void		sa_mode_fire(ATTACK_DATA *);

static void		a_mode_fire(ATTACK_DATA *);

static void		a_mode_targets(ATTACK_DATA *, AREA_POINT *);

static void		a_mode_targets_add_enemies(ATTACK_DATA *,
				AREA_POINT *, A_MODE_SPREAD_FACTOR *
			);

static void		a_mode_targets_add_free_sectors(ATTACK_DATA *,
				AREA_POINT *, A_MODE_SPREAD_FACTOR *
			);

static void		hit_value_shot(ATTACK_DATA *);

static void		hit_value_shot_base(ATTACK_DATA *);

static void		firing_mode_penalty(ATTACK_DATA *);

static void		range_modifier(ATTACK_DATA *);




/*
 * executes a shot / shots
 */
void shoot(CHARACTER *attacker, OBJECT *attacker_weapon,
	FIRING_DATA *firing_data, const AREA_POINT *target_point
)
{
	ATTACK_DATA attack;
	bool silent;

	attack_init(&attack);
	attack.type = AT_SHOT;
	attack.attacker = attacker;
	attack.weapon = attacker_weapon;
	attack.firing_data = firing_data;
	attack.target_point = *target_point;
	attack.distance = area_distance(&attacker->location, target_point);

	if (character_unnoticed(attacker) &&
		character_has_perk(attacker, PK_SNEAK_ATTACK)) {

		attack.sneak = true;
	}

	silent = object_has_attribute(attack.weapon, OA_SILENT);

	switch (firing_data->firing_mode) {

	case FMODE_S:

		s_mode_fire(&attack);

		break;

	case FMODE_SA:

		sa_mode_fire(&attack);

		break;

	case FMODE_A:

		a_mode_fire(&attack);

		break;

	default:

		die("*** CORE ERROR *** invalid firing mode");
	}

	if (character_unnoticed(attacker)) {

		notice_check(attacker, silent, true);
	}
}




/*
 * returns combat-related range info
 */
char * range_description(char *buffer, AREA_DISTANCE target_distance)
{

	if (target_distance > MAX_LONG_RANGE) {

		sprintf(buffer, "Very long distance, %d", target_distance);

	} else if (target_distance > MAX_MEDIUM_RANGE) {

		sprintf(buffer, "Long distance, %d", target_distance);

	} else if (target_distance > MAX_SHORT_RANGE) {

		sprintf(buffer, "Medium distance, %d", target_distance);

	} else if (target_distance > MAX_CLOSE_COMBAT_RANGE) {

		sprintf(buffer, "Short distance, %d", target_distance);

	} else if (target_distance == 1) {

		sprintf(buffer, "Close combat, %d", target_distance);

	} else {

		strcpy(buffer, "0");
	}

	return buffer;
}



/*
 * attempts to fire a shot
 * returns true if successful
 */
static bool fire_shot(ATTACK_DATA *attack)
{
	OBJECT *weapon;

	weapon = attack->weapon;

	if (weapon == NULL) {

		return false;
	}

	if (object_has_attribute(weapon, OA_JAMMED)) {

		return false;
	}

	if (weapon->charge <= 0) {

		return false;
	}

	attack->hit_roll = d100();

	weapon->charge -= 1;

	if (weapon->charge == 0) {

		if (object_has_attribute(weapon, OA_AUTOMATIC_RECHARGE)) {
			EVENT *event;

			event = event_create(EVT_RECHARGE_WEAPON,
				weapon->reload
			);

			event->character = attack->attacker;
			event->object = weapon;
			event_set(event);
		}
	}

	if (attack->hit_roll >= D100_AUTOMATIC_FAILURE) {

		if (weapon_has_jammed(attack) ||
			laser_weapon_malfunction(attack) ||
			plasma_weapon_malfunction(attack)) {

			return false;
		}
	}

	process_shot(attack);

	return true;
}



/*
 * processes a shot
 */
static void process_shot(ATTACK_DATA *attack)
{

	hit_value_shot(attack);

	if (target_hit(attack)) {

		hit(attack);

	} else {

		miss(attack);
	}
}



/*
 * S mode fire
 */
static void s_mode_fire(ATTACK_DATA *attack)
{

	attack->rapid = false;

	if (!fire_shot(attack)) {

		return;
	}

	handle_destruction(&attack->target_point, attack->attacker,
		DT_ATTACK
	);
}



/*
 * SA mode fire
 */
static void sa_mode_fire(ATTACK_DATA *attack)
{
	N_SHOTS n_shots;
	int i;

	attack->rapid = true;

	n_shots = attack->firing_data->n_shots;

	for (i = 0; i < n_shots; i++) {

		if (!fire_shot(attack)) {

			break;
		}
	}

	handle_destruction(&attack->target_point,
		attack->attacker, DT_ATTACK
	);
}



/*
 * A mode fire
 */
static void a_mode_fire(ATTACK_DATA *attack)
{
	AREA_POINT targeted_sector[A_MODE_SPREAD_MAX];
	AREA_POINT burst_center;
	N_SHOTS n_shots_per_target;
	int i;

	attack->rapid = true;

	burst_center = attack->target_point;

	a_mode_targets(attack, targeted_sector);

	n_shots_per_target = attack->firing_data->n_shots /
		attack->firing_data->spread;

	for (i = 0; i < attack->firing_data->spread; i++) {
		const AREA_POINT *target_point;
		N_SHOTS n_shots;
		int j;

		target_point = &targeted_sector[i];

		n_shots = n_shots_per_target;

		if (area_points_equal(target_point, &burst_center)) {

			n_shots += attack->firing_data->n_shots %
				attack->firing_data->spread;
		}

		attack->target_point = *target_point;

		attack->distance = area_distance(&attack->attacker->location,
			target_point
		);

		for (j = 0; j < n_shots; j++) {

			if (!fire_shot(attack)) {

				break;
			}
		}

		handle_destruction(&attack->target_point,
			attack->attacker, DT_ATTACK
		);
	}
}




/*
 * determines which sectors are targeted by A mode fire
 */
static void a_mode_targets(ATTACK_DATA *attack, AREA_POINT *targeted_sector)
{
	A_MODE_SPREAD_FACTOR spread;

	spread = 0;

	targeted_sector[spread] = attack->target_point;

	++spread;

	a_mode_targets_add_enemies(attack, targeted_sector, &spread);

	if (spread == attack->firing_data->spread) {

		return;
	}

	a_mode_targets_add_free_sectors(attack, targeted_sector, &spread);

	if (spread < attack->firing_data->spread) {

		attack->firing_data->spread = spread;
	}

}



/*
 * determines which sectors are targeted by A mode fire
 * (adds enemies)
 */
static void a_mode_targets_add_enemies(ATTACK_DATA *attack,
	AREA_POINT *targeted_sector, A_MODE_SPREAD_FACTOR *spread
)
{
	const AREA_POINT *attacker_location;
	const AREA_POINT *burst_center;
	int y_mod, x_mod;

	attacker_location = &attack->attacker->location;
	burst_center = &attack->target_point;

	for (y_mod = -1; y_mod <= 1; y_mod++) {
	for (x_mod = -1; x_mod <= 1; x_mod++) {
		AREA_POINT target_point;
		SECTOR *sector;

		if (y_mod == 0 && x_mod == 0) {

			continue;
		}

		target_point.y = burst_center->y + y_mod;
		target_point.x = burst_center->x + x_mod;

		if (out_of_area_bounds(&target_point)) {

			continue;
		}

		sector = sector_at(&target_point);

		if (sector->character == NULL) {

			continue;
		}

		if (!hostility_between(attack->attacker, sector->character) ||
			!los(attacker_location, &target_point)) {

			continue;
		}

		targeted_sector[*spread] = target_point;

		*spread += 1;

		if (*spread == attack->firing_data->spread) {

			return;
		}
	}
	}
}



/*
 * determines which sectors are targeted by A mode fire
 * (adds "free" - character free - sectors)
 */
static void a_mode_targets_add_free_sectors(ATTACK_DATA *attack,
	AREA_POINT *targeted_sector, A_MODE_SPREAD_FACTOR *spread
)
{
	const AREA_POINT *attacker_location;
	const AREA_POINT *burst_center;
	int y_mod, x_mod;

	attacker_location = &attack->attacker->location;
	burst_center = &attack->target_point;

	for (y_mod = -1; y_mod <= 1; y_mod++) {
	for (x_mod = -1; x_mod <= 1; x_mod++) {
		AREA_POINT target_point;

		if (y_mod == 0 && x_mod == 0) {

			continue;
		}

		target_point.y = burst_center->y + y_mod;
		target_point.x = burst_center->x + x_mod;

		if (out_of_area_bounds(&target_point)) {

			continue;
		}

		if (!los(attacker_location, &target_point)) {

			continue;
		}

		if (character_at(&target_point) != NULL) {

			continue;
		}

		targeted_sector[*spread] = target_point;

		*spread += 1;

		if (*spread == attack->firing_data->spread) {

			return;
		}
	}
	}
}



/*
 * determines the hit value of a shot
 */
static void hit_value_shot(ATTACK_DATA *attack)
{
	const CHARACTER *target;

	target = character_at(&attack->target_point);

	if (target == NULL) {

		if (object_at(&attack->target_point) != NULL) {

			attack->hit_value = OBJECT_HIT_VALUE;

		} else {

			attack->hit_value = 0;
		}

		return;
	}

	hit_value_shot_base(attack);

	firing_mode_penalty(attack);

	range_modifier(attack);

	if (attack->firing_data->firing_mode == FMODE_A) {

		attack->hit_value *= attack->firing_data->spread;

		return;
	}

	attack->hit_value += attack->weapon->firing_mode.accuracy;

	if (character_has_perk(target, PK_UNCANNY_DODGE)) {

		attack->hit_value += uncanny_dodge_modifier(target);
	}
}



/*
 * determines the base hit value of a shot
 */
static void hit_value_shot_base(ATTACK_DATA *attack)
{
	const CHARACTER *attacker;

	attacker = attack->attacker;

	if (attack->distance == 1) {

		attack->hit_value = attacker->stat[S_CC].current;

		return;
	}

	attack->hit_value = attacker->stat[S_RC].current;
}



/*
 * applies firing mode specific to hit penalties
 */
static void firing_mode_penalty(ATTACK_DATA *attack)
{
	FIRING_MODE firing_mode;

	firing_mode = attack->firing_data->firing_mode;

	if (firing_mode == FMODE_A) {

		attack->hit_value /= 5;

	} else if (firing_mode == FMODE_SA) {
		HIT_MODIFIER sa_penalty;

		sa_penalty = SA_MODE_TO_HIT_PENALTY;

		if (character_has_perk(attack->attacker, PK_RAPID_SHOT)) {

			sa_penalty /= RAPID_SHOT_SA_PENALTY_DIVIDER;
		}

		attack->hit_value += sa_penalty *
			attack->firing_data->n_shots;
	}
}



/*
 * modifies the hit value based on range
 */
static void range_modifier(ATTACK_DATA *attack)
{
	HIT_MODIFIER modifier;
	OBJECT_SUBTYPE weapon_subtype;

	weapon_subtype = attack->weapon->subtype;

	if (attack->distance > MAX_LONG_RANGE) {

		switch (weapon_subtype) {
		case OSTYPE_PISTOL:	modifier = -100; break;
		case OSTYPE_BASIC:	modifier = -50; break;
		case OSTYPE_HEAVY:	modifier = -20; break;
		default:		modifier = 0;
		}

	} else if (attack->distance > MAX_MEDIUM_RANGE) {

		switch (weapon_subtype) {
		case OSTYPE_PISTOL:	modifier = -30; break;
		case OSTYPE_BASIC:	modifier = -10; break;
		case OSTYPE_HEAVY:	modifier = -5; break;
		default:		modifier = 0;
		}

	} else if (attack->distance > MAX_SHORT_RANGE) {

		switch (weapon_subtype) {
		case OSTYPE_PISTOL:	modifier = 0; break;
		case OSTYPE_BASIC:	modifier = 0; break;
		case OSTYPE_HEAVY:	modifier = 5; break;
		default:		modifier = 0;
		}

	} else if (attack->distance > MAX_CLOSE_COMBAT_RANGE) {

		switch (weapon_subtype) {
		case OSTYPE_PISTOL:	modifier = 10; break;
		case OSTYPE_BASIC:	modifier = 5; break;
		case OSTYPE_HEAVY:	modifier = 5; break;
		default:		modifier = 0;
		}

	} else {

		modifier = 0;
	}

	if (modifier < 0 && 
		character_has_perk(attack->attacker, PK_MARKSMAN)) {

		modifier /= MARKSMAN_RANGE_PENALTY_DIVIDER;
	}

	attack->hit_value += modifier;
}




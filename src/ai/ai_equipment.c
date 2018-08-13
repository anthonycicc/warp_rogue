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
 * Module: AI Equipment
 */

#include "wrogue.h"



static AI_OPTION_WEIGHT ai_weapon_attack_weight_base(AI_CONTROL_DATA *,
				const OBJECT *
			);

static void		ai_choose_firing_mode(AI_CONTROL_DATA *,
				FIRING_DATA *
			);

static bool		ai_weapon_suitable(AI_CONTROL_DATA *,
				const OBJECT *
			);

static void		ai_ccw_attack_weight_modifier(AI_CONTROL_DATA *,
				const OBJECT *, AI_OPTION_WEIGHT *
			);

static bool		ai_ccw_suitable(AI_CONTROL_DATA *, const OBJECT *);

static void		ai_rcw_attack_weight_modifier(AI_CONTROL_DATA *,
				const OBJECT *, AI_OPTION_WEIGHT *
			);

static bool		ai_rcw_suitable(AI_CONTROL_DATA *, const OBJECT *);


static AI_OPTION_WEIGHT	ai_unarmed_attack_weight(AI_CONTROL_DATA *);




/*
 * makes the AI attempt to fix (unjam/reload) its weapon if necessary
 */
void ai_fix_weapon(AI_CONTROL_DATA *ai, bool do_wait)
{
	OBJECT *weapon = ai->self->weapon;

	/* the 'weapon' is an unarmed attack .. return */
	if (weapon == NULL) return;
	
	/* weapon is jammed .. */
	if (object_has_attribute(weapon, OA_JAMMED)) {

		/* unjam it */
		action_unjam_weapon(ai->self);
		return;
	}

	/* no ammo left */
	if (weapon->charge == 0) {

		/* the weapon recharges automatically .. */  
		if (object_has_attribute(weapon, OA_AUTOMATIC_RECHARGE)) {

			/* wait if do_wait is true */
			if (do_wait) action_do_nothing(ai->self);

			return;
		}
		
		/* reload */
		action_reload_weapon(ai->self);
		return;
	}

	/* weapon is ok */
	return;
}



/*
 * makes the AI use its weapon
 */
void ai_use_weapon(AI_CONTROL_DATA *ai)
{
	OBJECT *weapon = ai->self->weapon;

	/* ranged combat weapon .. */
	if (weapon != NULL &&
		weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {
		FIRING_DATA firing_data;

		/* choose firing mode */
		ai_choose_firing_mode(ai, &firing_data);

		/* shoot */
		action_shoot(ai->self, weapon, &firing_data,
			&ai->target_point
		);

	/* close combat weapon or unarmed attack */
	} else {

		/* strike */
		action_strike(ai->self, weapon, &ai->target_point);
	}
}





/*
 * returns the attack weight of the passed weapon
 * if NULL is passed, the weight of the character's unarmed attack
 * is returned
 */
AI_OPTION_WEIGHT ai_weapon_attack_weight(AI_CONTROL_DATA *ai,
	const OBJECT *weapon
)
{
	AI_OPTION_WEIGHT weight;

	/* weapon = NULL .. */
	if (weapon == NULL) {

		/* .. return unarmed attack weight */
		return ai_unarmed_attack_weight(ai);
	}

	/* weapon not suitable - weight is NIL */
	if (!ai_weapon_suitable(ai, weapon)) return AI_OPTION_WEIGHT_NIL;

	/* determine basic weapon attack weight */
	weight = ai_weapon_attack_weight_base(ai, weapon);
	
	/* apply weapon type specific modifiers */
	if (weapon->type == OTYPE_CLOSE_COMBAT_WEAPON) {

		ai_ccw_attack_weight_modifier(ai, weapon, &weight);

	} else if (weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {

		ai_rcw_attack_weight_modifier(ai, weapon, &weight);
	}

	return weight;
}



/*
 * returns the basic attack weight of the passed weapon
 * (weapon type independent)
 */
static AI_OPTION_WEIGHT ai_weapon_attack_weight_base(AI_CONTROL_DATA *ai,
	const OBJECT *weapon
)
{
	/* start at 0 */
	AI_OPTION_WEIGHT weight = 0;

	/* add average damage per hit */
	weight += dice_roll_average(&weapon->damage);

	/* the attack does not target a character .. */
	if (ai->target_character == NULL) return weight;

	/* .. increase the weight of force weapons */
	if (object_has_attribute(weapon, OA_FORCE)) {

		if (object_has_attribute(weapon, OA_FORCE_RUNE)) {

			weight += 8;

		} else {

			weight += 5;
		}
	};	
	
	return weight;
}




/*
 * AI: choose firing mode
 */
static void ai_choose_firing_mode(AI_CONTROL_DATA *ai,
	FIRING_DATA *firing_data
)
{
	const FIRING_MODE_DATA *firing_mode =
		&ai->self->weapon->firing_mode;

	if (firing_mode->has[FMODE_A]) {

		firing_data->firing_mode = FMODE_A;

		firing_data->n_shots = firing_mode->a_shots;

		firing_data->spread = random_int(
			A_MODE_SPREAD_MIN, A_MODE_SPREAD_MAX
		);

	} else if (firing_mode->has[FMODE_SA]) {

		firing_data->firing_mode = FMODE_SA;

		firing_data->n_shots = random_int(
			firing_mode->min_sa_shots,
			firing_mode->max_sa_shots
		);

	} else if (firing_mode->has[FMODE_S]) {

		firing_data->firing_mode = FMODE_S;

		firing_data->n_shots = 1;
	}
}



/*
 * returns true if the passed weapon can be used against the chosen target
 */
static bool ai_weapon_suitable(AI_CONTROL_DATA *ai, const OBJECT *weapon)
{

	if (weapon->type == OTYPE_CLOSE_COMBAT_WEAPON) {

		return ai_ccw_suitable(ai, weapon);

	} else if (weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {

		return ai_rcw_suitable(ai, weapon);

	} else {

		return false;
	}
}



/*
 * applies the CCW specific attack weight modifiers
 */
static void ai_ccw_attack_weight_modifier(AI_CONTROL_DATA *ai,
	const OBJECT *weapon, AI_OPTION_WEIGHT *weight
)
{

	NOT_USED(weapon);

	/* we would need more than two turns to reach the target .. */
	if (ai->target_distance > movement_speed_max(ai->self) * 2) {

		/* .. halve weight if possible */
		if (*weight >= 2) *weight /= 2;
	}
}



/*
 * returns true if the passed CCW can be used against the chosen target
 */
static bool ai_ccw_suitable(AI_CONTROL_DATA *ai, const OBJECT *weapon)
{

	NOT_USED(weapon);

	/* target not reachable - return false */
	if (!ai->target_reachable) return false;

	/* target distance > 1 and we cannot move - return false */
	if (ai->target_distance > 1 &&
		(ai->tactic == TACTIC_STAY_CLOSE ||
		ai->tactic == TACTIC_HOLD_POSITION ||
		movement_speed_max(ai->self) == 0)) {

		return false;
	}

	return true;
}



/*
 * applies the RCW specific attack weight modifiers
 */
static void ai_rcw_attack_weight_modifier(AI_CONTROL_DATA *ai,
	const OBJECT *weapon, AI_OPTION_WEIGHT *weight
)
{

	/*
	 * rapid fire weapons prefered, unless we have
	 * the "Critical shot" perk
	 */
	if (!character_has_perk(ai->self, PK_CRITICAL_SHOT)) {
		const bool *has = weapon->firing_mode.has;

		if (has[FMODE_A]) {

			*weight += 10;

		} else if (has[FMODE_SA]) {

			*weight += 5;
		}
	}

	/* loaded weapons prefered */
	if (weapon->charge > 0) *weight += 1;

	/* unjammed weapons prefered */
	if (!object_has_attribute(weapon, OA_JAMMED)) *weight += 1;

	/* basic and heavy weapons prefered for long range shots */
	if (ai->target_distance > MAX_MEDIUM_RANGE) {

		if (weapon->subtype == OSTYPE_BASIC ||
			weapon->subtype == OSTYPE_HEAVY) {

			*weight += 5;
		}
	}
}



/*
 * returns true if the passed RCW can be used against the chosen target
 */
static bool ai_rcw_suitable(AI_CONTROL_DATA *ai, const OBJECT *weapon)
{

	/* target too close - return false */
	if (ai->target_distance == 1 &&
		weapon->subtype != OSTYPE_PISTOL) {

		return false;
	}

	return true;
}



/*
 * returns the attack weight of the character's unarmed attack
 */
static AI_OPTION_WEIGHT ai_unarmed_attack_weight(AI_CONTROL_DATA *ai)
{
	DICE_ROLL damage_power; /* the character's unarmed damage power */

	/* the character has no unarmed attack - weight is NIL */
	if (character_has_attribute(ai->self, CA_NO_UNARMED_ATTACK)) {

		return AI_OPTION_WEIGHT_NIL;
	}

	/* target not reachable - weight is NIL */
	if (!ai->target_reachable) return AI_OPTION_WEIGHT_NIL;

	/* target distance > 1 and we cannot move .. */
	if (ai->target_distance > 1 &&
		(ai->tactic == TACTIC_STAY_CLOSE ||
		ai->tactic == TACTIC_HOLD_POSITION ||
		movement_speed_max(ai->self) == 0)) {

		/* .. weight is 0 */
		return AI_OPTION_WEIGHT_NIL;
	}

	return dice_roll_average(
		unarmed_damage_power(ai->self, &damage_power)
	);
}



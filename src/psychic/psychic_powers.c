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
 * Module: Psychic Powers
 */

#include "wrogue.h"





/*
 * psychic power: regenerate
 */
void psy_regenerate(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;

	target = character_at(evocation->target_point);

	target->injury -= dice(2,10);

	if (target->injury < 0) {

		target->injury = 0;
	}
}



/*
 * psychic power: reactivate
 */
void psy_reactivate(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;

	target = character_at(evocation->target_point);

	effect_terminate(target, ET_STUNNED);
}



/*
 * psychic power: warp strength
 */
void psy_warp_strength(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;
	STAT_MODIFIER stat_increase;
	EVENT *event;

	target = character_at(evocation->target_point);

	event = event_create(EVT_PSY_POWER_TERMINATION,
		dice(1, 50) + 50
	);

	event->character = target;

	event->psy_power = evocation->power;

	stat_increase = percent(dice(10, 10), target->stat[S_ST].total);

	event->stat_modifier[S_ST] += stat_increase;

	stat_modifiers_apply(target, event->stat_modifier);

	character_set_attribute(target, CA_WARP_STRENGTH);

	event_set(event);
}



/*
 * psychic power: warp strength (termination)
 */
void psy_warp_strength_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);

	character_remove_attribute(event->character, CA_WARP_STRENGTH);
}



/*
 * psychic power: purify blood
 */
void psy_purify_blood(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;

	target = character_at(evocation->target_point);

	effect_terminate(target, ET_POISONED);
}



/*
 * psychic power: purge plague
 */
void psy_purge_plague(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;

	target = character_at(evocation->target_point);

	effect_terminate(target, ET_DISEASED);
}



/*
 * psychic power: mind view
 */
void psy_mind_view(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;
	LIST_NODE *node;

	message_force_visibility(true);

	/* uncover all enemies with minds */
	for (node = area_character_list()->head;
		node != NULL;
		node = node->next) {

		target = (CHARACTER *)node->data;

		if (!hostility_between(evocation->evoker, target) ||
			character_has_attribute(target, CA_MACHINE)) {

			continue;
		}

		if (character_unnoticed(target)) {

			cancel_stealth(target, evocation->evoker);
		}
	}

	message_flush(SID_GAME);

	if (evocation->evoker == player_controlled_character()) {

		view_screen(evocation->evoker, "Mind view", C_PsychicCursor);
	}

	message_force_visibility(false);
}



/*
 * psychic power: psychic shriek
 */
void psy_psychic_shriek(const EVOCATION_DATA *evocation)
{
	CHARACTER *target = character_at(evocation->target_point);

	if (d100_test_passed(target->stat[S_TN].current)) {

		dynamic_message(MSG_RESIST, target, NULL, MOT_NIL);
		return;
	}

	effect_activate(target, ET_STUNNED, PSY_PSYCHIC_SHRIEK_DURATION);
}



/*
 * psychic power: terrify
 */
void psy_terrify(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;

	target = character_at(evocation->target_point);

	if (fear_resisted(target)) {

		dynamic_message(MSG_RESIST, target, NULL, MOT_NIL);

		return;
	}

	effect_activate(target, ET_BROKEN, PSY_TERRIFY_DURATION);
}



/*
 * psychic power: psychic shield
 */
void psy_psychic_shield(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;
	EVENT *event;

	target = character_at(evocation->target_point);

	event = event_create(EVT_PSY_POWER_TERMINATION,
		PSY_PSYCHIC_SHIELD_DURATION
	);

	event->character = target;

	event->psy_power = evocation->power;

	event->power = PSY_PSYCHIC_SHIELD_POWER;

	target->armour_rating.current += event->power;

	character_set_attribute(target, CA_PSYCHIC_SHIELD);

	event_set(event);
}



/*
 * psychic power: psychic shield (termination)
 */
void psy_psychic_shield_terminate(EVENT *event)
{

	event->character->armour_rating.current -= event->power;

	character_remove_attribute(event->character, CA_PSYCHIC_SHIELD);
}



/*
 * psychic power: banishment
 */
void psy_banishment(const EVOCATION_DATA *evocation)
{
	CHARACTER *target;
	int test_result;

	target = character_at(evocation->target_point);

	test_result = d100_test(d100(), target->stat[S_TN].current);

	if (test_result >= 0) {

		dynamic_message(MSG_RESIST, target, NULL, MOT_NIL);

		return;
	}

	if (test_result < -20) {

		target->injury = injury_max(target) * 2;

	} else {

		target->injury += dice(2, 6) + 4;
	}

	handle_destruction(evocation->target_point, evocation->evoker,
		DT_ATTACK
	);
}


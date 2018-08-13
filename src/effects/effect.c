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
 * Module: Effect
 */

#include "wrogue.h"






static bool		effect_already_in_effect(const CHARACTER *,
				EFFECT
			);

static void 		effect_poisoned(EVENT *);
static void 		effect_poisoned_terminate(EVENT *);

static void 		effect_diseased(EVENT *);
static void 		effect_diseased_terminate(EVENT *);

static void 		effect_broken(EVENT *);
static void 		effect_broken_terminate(EVENT *);

static void 		effect_stunned(EVENT *);
static void 		effect_stunned_terminate(EVENT *);





/*
 * effect data
 */
static const EFFECT_DATA Effect[N_EFFECTS] = {

	{"Poisoned",		CA_POISONED,
		effect_poisoned,
		effect_poisoned_terminate
	},

	{"Diseased",		CA_DISEASED,
		effect_diseased,
		effect_diseased_terminate
	},

	{"Broken",		CA_BROKEN,
		effect_broken,
		effect_broken_terminate
	},

	{"Stunned",		CA_STUNNED,
		effect_stunned,
		effect_stunned_terminate
	}

};



/*
 * activates an effect
 */
void effect_activate(CHARACTER *character, EFFECT effect, TIMER timer)
{
	const EFFECT_DATA *effect_data;
	EVENT *event;

	effect_data = &Effect[effect];

	if (effect_already_in_effect(character, effect)) {

		return;
	}

	if (effect == ET_BROKEN) {

		dynamic_message(MSG_MORALE_BROKEN,
			character,
			NULL, MOT_NIL
		);

	} else {

		dynamic_message(MSG_EFFECT_ACTIVATION, character,
			effect_data->name, MOT_STRING
		);
	}

	event = event_create(EVT_EFFECT_TERMINATION, timer);
	event->character = character;
	event->effect = effect;

	(*effect_data->activate)(event);

	character_set_attribute(character, effect_data->attribute);

	event_set(event);
}



/*
 * terminates an effect
 */
void effect_terminate(CHARACTER *character, EFFECT effect)
{
	EVENT *event = effect_termination_event(character, effect);

	if (event == NULL) return;

	event_execute(event);
}



/*
 * terminates an effect (event)
 */
void effect_terminate_event(EVENT *event)
{
	const EFFECT_DATA *effect_data;

	effect_data = &Effect[event->effect];

	if (event->effect == ET_BROKEN) {

		dynamic_message(MSG_MORALE_RECOVERED,
			event->character,
			NULL, MOT_NIL
		);

	} else {

		dynamic_message(MSG_EFFECT_TERMINATION,
			event->character,
			effect_data->name, MOT_STRING
		);
	}

	(*effect_data->terminate)(event);

	character_remove_attribute(event->character, effect_data->attribute);
}



/*
 * returns true if the character has managed to resist poison
 */
bool poison_resisted(const CHARACTER *character)
{

	if (character_has_attribute(character, CA_MACHINE)) {

		return true;
	}

	if (character_has_perk(character, PK_IMMUNITY_TO_POISON)) {

		return true;
	}

	return false;
}



/*
 * returns true if the character has managed to resist disease
 */
bool disease_resisted(const CHARACTER *character)
{

	if (character_has_attribute(character, CA_MACHINE) ||
		character_has_perk(character, PK_IMMUNITY_TO_DISEASE) ||
		d100_test_passed(character->stat[S_TN].current +
		DISEASE_RESISTANCE_TEST_MODIFIER)) {

		return true;
	}

	return false;
}


/*
 * returns true if the character has managed to resist pain
 */
bool pain_resisted(const CHARACTER *character)
{

	/* Machine, Plant, Stoic, or Immunity .. */
	if (character_has_attribute(character, CA_MACHINE) ||
		character_has_attribute(character, CA_PLANT) ||
		character_has_attribute(character, CA_STOIC) ||
		character_has_perk(character, PK_IMMUNITY_TO_PAIN)) {
		
		/* automatic success */
		return true;
	}
	
	/* Will Power test */
	if (d100_test_passed(character->stat[S_WP].current)) return true;

	/* Leader perk effect .. */
	if (character->party == PARTY_PLAYER &&
		party_has_leader()) {
			
		/* another chance to pass the test */
		if (d100_test_passed(character->stat[S_WP].current)) {
			
			return true;
		}
	}

	/* pain NOT resisted */
	return false;
}



/*
 * returns true if the character has managed to resist fear
 */
bool fear_resisted(const CHARACTER *character)
{

	/* Machine, Plant, Stoic, or Immunity .. */
	if (character_has_attribute(character, CA_MACHINE) ||
		character_has_attribute(character, CA_PLANT) ||
		character_has_attribute(character, CA_STOIC) ||
		character_has_perk(character, PK_IMMUNITY_TO_FEAR)) {
		
		/* automatic success */
		return true;
	}

	/* Will Power test */
	if (d100_test_passed(character->stat[S_WP].current)) return true;
		
	/* Leader perk effect .. */
	if (character->party == PARTY_PLAYER &&
		party_has_leader()) {
			
		/* another chance to pass the test */
		if (d100_test_passed(character->stat[S_WP].current)) {
			
			return true;
		}
	}

	/* fear NOT resisted */
	return false;
}



/*
 * poison damage event
 */
void poison_damage(EVENT *event)
{
	CHARACTER *character;

	character = event->character;

	character->injury += POISON_DAMAGE;

	handle_destruction(&character->location, NULL, DT_POISON);
}



/*
 * returns true if passed effect is already in effect (sic!)
 */
static bool effect_already_in_effect(const CHARACTER *character,
	EFFECT effect
)
{
	const EFFECT_DATA *effect_data;

	effect_data = &Effect[effect];

	if (character_has_attribute(character, effect_data->attribute)) {

		return true;
	}

	return false;
}



/*
 * effect: poisoned
 */
static void effect_poisoned(EVENT *event)
{
	EVENT *damage_event;

	damage_event = event_create(EVT_POISON_DAMAGE, TIMER_CONTINUOUS);
	damage_event->character = event->character;

	event_set(damage_event);
}



/*
 * effect: poisoned (termination)
 */
static void effect_poisoned_terminate(EVENT *event)
{
	EVENT *damage_event;

	damage_event = remove_character_event(event->character,
		EVT_POISON_DAMAGE
	);

	event_destroy(damage_event);
}



/*
 * effect: diseased
 */
static void effect_diseased(EVENT *event)
{
	STAT stat;
	
	iterate_over_stats(stat) {

		event->stat_modifier[stat] = -dice(2, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * effect: diseased (termination)
 */
static void effect_diseased_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * effect: broken
 */
static void effect_broken(EVENT *event)
{


	ai_set_state(&event->character->ai, AI_STATE_FLEE);
}



/*
 * effect: broken (termination)
 */
static void effect_broken_terminate(EVENT *event)
{

	NOT_USED(event);
}



/*
 * effect: stunned
 */
static void effect_stunned(EVENT *event)
{
	EVENT *removed_event;

	removed_event = remove_character_event(
		event->character, EVT_RELOAD_WEAPON
	);

	if (removed_event != NULL) {

		event_destroy(removed_event);
	}

	removed_event = remove_character_event(
		event->character, EVT_UNJAM_WEAPON
	);

	if (removed_event != NULL) {

		event_destroy(removed_event);
	}
}



/*
 * effect: stunned (termination)
 */
static void effect_stunned_terminate(EVENT *event)
{

	/* nothing to do here */

	NOT_USED(event);
}






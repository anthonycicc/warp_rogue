/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
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
 * Module: Object Drugs
 */

#include "wrogue.h"



static bool			use_antidote(CHARACTER *, OBJECT *);

static void			drug_flash(EVENT *);
static void			drug_flash_terminate(EVENT *);

static void			drug_stoic(EVENT *);
static void			drug_stoic_terminate(EVENT *);

static void			drug_nomat(EVENT *);
static void			drug_nomat_terminate(EVENT *);

static void			drug_focus(EVENT *);
static void			drug_focus_terminate(EVENT *);

static void			drug_bull(EVENT *);
static void			drug_bull_terminate(EVENT *);

static DRUG			name_to_drug(const char *);





/*
 * drug data
 */
static const DRUG_DATA Drug[MAX_DRUGS] = {

	{"Antidote",		CA_NIL,
		NULL,
		NULL
	},

	{"Flash",		CA_FLASH,
		drug_flash,
		drug_flash_terminate
	},

	{"Focus",		CA_FOCUS,
		drug_focus,
		drug_focus_terminate
	},

	{"Stoic",		CA_STOIC,
		drug_stoic,
		drug_stoic_terminate
	},

	{"Nomat",		CA_NOMAT,
		drug_nomat,
		drug_nomat_terminate
	},

	{"Bull",		CA_BULL,
		drug_bull,
		drug_bull_terminate
	}

};





/*
 * use drug
 */
bool use_drug(CHARACTER *character, OBJECT *object)
{
	DRUG drug;
	const DRUG_DATA *drug_data;
	EVENT *event;

	drug = name_to_drug(object->name);

	if (drug == DRUG_ANTIDOTE) {

		return use_antidote(character, object);
	}

	drug_data = &Drug[drug];

	if (character_has_attribute(character, drug_data->attribute)) {

		message(SID_GAME, "Already in effect.");
		return false;
	}

	dynamic_message(MSG_USE_DRUG, character, drug_data->name,
		MOT_STRING
	);

	event = event_create(EVT_DRUG_TERMINATION, DEFAULT_DRUG_DURATION);
	event->character = character;
	event->drug = drug;

	(*drug_data->use)(event);

	character_set_attribute(character, drug_data->attribute);

	event_set(event);

	inventory_remove(character, object);
	object_destroy(object);

	return true;
}



/*
 * terminates a drug effect
 */
void drug_terminate(EVENT *event)
{
	const DRUG_DATA *drug_data;

	drug_data = &Drug[event->drug];

	dynamic_message(MSG_DRUG_TERMINATION, event->character,
		drug_data->name, MOT_STRING
	);

	(*drug_data->terminate)(event);

	character_remove_attribute(event->character, drug_data->attribute);
}



/*
 * use antidote
 */
static bool use_antidote(CHARACTER *character, OBJECT *object)
{

	if (!character_has_attribute(character, CA_POISONED)) {

		message(SID_GAME, "Not poisoned.");

		return false;
	}

	dynamic_message(MSG_USE_DRUG, character, Drug[DRUG_ANTIDOTE].name,
		MOT_STRING
	);

	effect_terminate(character, ET_POISONED);

	inventory_remove(character, object);
	object_destroy(object);

	return true;
}



/*
 * drug: flash
 */
static void drug_flash(EVENT *event)
{

	event->stat_modifier[S_AG] = dice(4, 10);

	if (!character_has_perk(event->character, PK_DRUG_RESISTANT)) {

		event->stat_modifier[S_WP] = -dice(2, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * drug: flash (termination)
 */
static void drug_flash_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * drug: stoic
 */
static void drug_stoic(EVENT *event)
{

	if (character_has_attribute(event->character, CA_BROKEN)) {

		effect_terminate(event->character, ET_BROKEN);
	}

	if (!character_has_perk(event->character, PK_DRUG_RESISTANT)) {

		event->stat_modifier[S_AG] = -dice(2, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * drug: stoic (termination)
 */
static void drug_stoic_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * drug: nomat
 */
static void drug_nomat(EVENT *event)
{

	event->stat_modifier[S_FL] = dice(4, 10);

	if (!character_has_perk(event->character, PK_DRUG_RESISTANT)) {

		event->stat_modifier[S_RC] = -dice(4, 10);
		event->stat_modifier[S_CC] = -dice(4, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * drug: nomat (termination)
 */
static void drug_nomat_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * drug: focus
 */
static void drug_focus(EVENT *event)
{

	event->stat_modifier[S_RC] = dice(4, 10);

	if (!character_has_perk(event->character, PK_DRUG_RESISTANT)) {

		event->stat_modifier[S_AG] = -dice(2, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * drug: focus (termination)
 */
static void drug_focus_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * drug: bull
 */
static void drug_bull(EVENT *event)
{

	event->stat_modifier[S_ST] = dice(4, 10);

	if (!character_has_perk(event->character, PK_DRUG_RESISTANT)) {

		event->stat_modifier[S_RC] = -dice(4, 10);
		event->stat_modifier[S_IN] = -dice(4, 10);
		event->stat_modifier[S_FL] = -dice(4, 10);
	}

	stat_modifiers_apply(event->character, event->stat_modifier);
}



/*
 * drug: bull (termination)
 */
static void drug_bull_terminate(EVENT *event)
{

	stat_modifiers_revert(event->character, event->stat_modifier,
		STAT_MODIFIER_MAX
	);
}



/*
 * name -> drug index
 */
static DRUG name_to_drug(const char *str)
{
	DRUG i;

	for (i = 0; i < MAX_DRUGS; i++) {

		if (strings_equal(str, Drug[i].name)) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid drug: %s", str);

	return 0;
}


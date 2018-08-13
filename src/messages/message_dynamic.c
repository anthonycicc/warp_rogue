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
 * Module: Dynamic Message
 */

#include "wrogue.h"




/*
 * the currently processed message
 */
MESSAGE_DATA		Message;





static void	message_hit(void);
static void	message_critical_hit(void);

static void	message_miss(void);

static void	message_parry(void);

static void	message_disarm(void);
static void	message_disarm_fails(void);

static void	message_pick_up(void);

static void	message_reload_weapon(void);
static void	message_recharge_weapon(void);

static void	message_switch_weapons(void);
static void	message_fights_unarmed(void);

static void	message_weapon_jams(void);
static void	message_unjam_weapon(void);

static void	message_weapon_malfunctions(void);
static void	message_weapon_overheats(void);
static void	message_weapon_explodes(void);

static void	message_resist(void);

static void	message_recover(void);

static void	message_effect_activation(void);
static void	message_effect_termination(void);

static void	message_morale_broken(void);
static void	message_morale_recovered(void);

static void	message_evoke_psy_power(void);
static void	message_evoke_psy_power_fails(void);
static void	message_psy_power_termination(void);

static void	message_use_drug(void);
static void	message_drug_termination(void);

static void	message_fade_into_shadows(void);
static void	message_notice(void);

static void	message_fall_down(void);

static void	message_death(void);
static void	message_destruction(void);

static void	determine_visibility(void);

static void	sp_message(const char *);
static void	spo_message(const char *);
static void	spo_message_fail(const char *);
static void	possessive_message(const char *);



/*
 * table of message functions
 */
static void	(* const MessageFunction[MAX_MESSAGE_TYPES])(void) = {

	message_hit,
	message_critical_hit,
	message_miss,
	message_parry,

	message_disarm,
	message_disarm_fails,

	message_pick_up,

	message_reload_weapon,
	message_recharge_weapon,
	message_switch_weapons,
	message_fights_unarmed,
	message_weapon_jams,
	message_unjam_weapon,

	message_weapon_malfunctions,

	message_weapon_overheats,
	message_weapon_explodes,

	message_resist,
	message_recover,

	message_effect_activation,
	message_effect_termination,

	message_morale_broken,
	message_morale_recovered,

	message_evoke_psy_power,
	message_evoke_psy_power_fails,
	message_psy_power_termination,

	message_use_drug,
	message_drug_termination,

	message_fade_into_shadows,
	message_notice,

	message_fall_down,

	message_death,
	message_destruction

};



/*
 * visibility forced?
 */
static bool		ForceVisibility = false;



/*
 * outputs a dynamic message
 */
void dynamic_message(MESSAGE_TYPE message_type, const CHARACTER *subject,
	const void *object, MESSAGE_OBJECT_TYPE object_type
)
{
	bool send_message ;

	Message.subject = subject;
	Message.object = object;
	Message.object_type = object_type;

	determine_visibility();

	send_message = false;

	if (Message.subject_noticed) {

		send_message = true;

	} else if (Message.object_noticed) {

		if (object_type == MOT_CHARACTER) {

			send_message = true;
		}
	}

	if (send_message) {

		(MessageFunction[message_type])();
	}
}



/*
 * sets the 'force visibility' flag
 */
void message_force_visibility(bool status)
{

	ForceVisibility = status;
}



/*
 * hit message
 */
static void message_hit(void)
{

	spo_message("hit");
}



/*
 * critical hit message
 */
static void message_critical_hit(void)
{

	Message.object_article = ARTICLE_DEFINITE;

	message(SID_GAME, "%s *critically* %s %s.",
		subject_string(),
		conjugated_verb("hit",
			grammatical_person(Message.subject)
		),
		object_string()
	);
}



/*
 * miss message
 */
static void message_miss(void)
{

	spo_message("miss");
}



/*
 * parry message
 */
static void message_parry(void)
{

	Message.object_article = ARTICLE_DEFINITE;

	message(SID_GAME, "%s %s %s attack.",
		subject_string(),
		conjugated_verb("parry",
			grammatical_person(Message.subject)
		),
		possessive_form(object_string())
	);
}



/*
 * disarm message
 */
static void message_disarm(void)
{

	spo_message("disarm");
}



/*
 * disarm fails message
 */
static void message_disarm_fails(void)
{

	spo_message_fail("disarm");
}



/*
 * pick up message
 */
static void message_pick_up(void)
{

	Message.object_article = ARTICLE_INDEFINITE;

	message(SID_GAME, "%s %s up %s.",
		subject_string(),
		conjugated_verb("pick",
			grammatical_person(Message.subject)
		),
		object_string()
	);
}



/*
 * reload weapon message
 */
static void message_reload_weapon(void)
{

	sp_message("reload");
}



/*
 * recharge weapon message
 */
static void message_recharge_weapon(void)
{

	if (Message.subject == NULL) return;

	possessive_message("weapon has recharged");
}



/*
 * switch weapons message
 */
static void message_switch_weapons(void)
{

	if (Message.object == NULL) {

		message_fights_unarmed();

		return;
	}

	Message.object_article = ARTICLE_INDEFINITE;

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb("draw",
			grammatical_person(Message.subject)
		),
		object_string()
	);
}



/*
 * fights unarmed message
 */
static void message_fights_unarmed(void)
{

	message(SID_GAME, "%s now %s unarmed.",
		subject_string(),
		conjugated_verb("fight", grammatical_person(Message.subject))
	);
}



/*
 * weapon jams message
 */
static void message_weapon_jams(void)
{

	possessive_message("weapon jams");
}



/*
 * unjam weapon message
 */
static void message_unjam_weapon(void)
{

	message(SID_GAME, "%s %s %s weapon.",
		subject_string(),
		conjugated_verb("unjam", grammatical_person(Message.subject)),
		possessive_pronoun(Message.subject)
	);
}



/*
 * weapon malfunction message
 */
static void message_weapon_malfunctions(void)
{

	possessive_message("weapon malfunctions");
}



/*
 * weapon overheats message
 */
static void message_weapon_overheats(void)
{

	possessive_message("weapon overheats");

	message(SID_GAME, "%s %s the weapon.",
		subject_string(),
		conjugated_verb("drop",
			grammatical_person(Message.subject)
		)
	);
}



/*
 * weapon explodes message
 */
static void message_weapon_explodes(void)
{

	possessive_message("weapon explodes");
}



/*
 * resist message
 */
static void message_resist(void)
{

	sp_message("resist");
}



/*
 * recover message
 */
static void message_recover(void)
{

	sp_message("recover");
}



/*
 * effect activation message
 */
static void message_effect_activation(void)
{

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb("be",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * effect termination message
 */
static void message_effect_termination(void)
{

	message(SID_GAME, "%s %s no longer %s.",
		subject_string(),
		conjugated_verb("be",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * morale broken message
 */
static void message_morale_broken(void)
{

	message(SID_GAME, "%s morale is broken.",
		possessive_form(subject_string())
	);
}



/*
 * morale recovered message
 */
static void message_morale_recovered(void)
{

	message(SID_GAME, "%s %s %s morale.",
		subject_string(),
		conjugated_verb("recover",
			grammatical_person(Message.subject)
		),
		possessive_pronoun(Message.subject)
	);
}



/*
 * psychic power evocation message
 */
static void message_evoke_psy_power(void)
{

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb("evoke",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * psychic power evocation fails message
 */
static void message_evoke_psy_power_fails(void)
{
	char c_try[STRING_BUFFER_SIZE];

	strcpy(c_try, conjugated_verb("try",
		grammatical_person(Message.subject))
	);

	message(SID_GAME, "%s %s to evoke %s but %s.",
		subject_string(),
		c_try,
		lowercase_object_string(),
		conjugated_verb("fail", grammatical_person(Message.subject))
	);
}



/*
 * psychic power termination message
 */
static void message_psy_power_termination(void)
{

	message(SID_GAME, "%s %s no longer under the influence of %s.",
		subject_string(),
		conjugated_verb("be",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * use drug message
 */
static void message_use_drug(void)
{

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb("take",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * drug termination message
 */
static void message_drug_termination(void)
{

	message(SID_GAME, "%s %s no longer under the influence of %s.",
		subject_string(),
		conjugated_verb("be",
			grammatical_person(Message.subject)
		),
		lowercase_object_string()
	);
}



/*
 * fade into shadows message
 */
static void message_fade_into_shadows(void)
{

	message(SID_GAME, "%s %s into the shadows.",
		subject_string(),
		conjugated_verb("fade",
			grammatical_person(Message.subject)
		)
	);
}



/*
 * notice message
 */
static void message_notice(void)
{

	Message.object_article = ARTICLE_INDEFINITE;

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb("notice",
			grammatical_person(Message.subject)
		),
		object_string()
	);
}



/*
 * fall down message
 */
static void message_fall_down(void)
{

	message(SID_GAME, "%s %s down.",
		subject_string(),
		conjugated_verb("fall",
			grammatical_person(Message.subject)
		)
	);
}



/*
 * death message
 */
static void message_death(void)
{

	if (character_has_attribute(Message.subject, CA_MACHINE) ||
		character_has_attribute(Message.subject, CA_DAEMONIC)) {

		message(SID_GAME, "%s %s destroyed.",
			subject_string(),
			conjugated_verb("be",
				grammatical_person(Message.subject)
			)
		);

	} else {

		sp_message("die");
	}
}



/*
 * destruction message
 */
static void message_destruction(void)
{

	if (Message.subject == NULL) {

		message(SID_GAME, "%s is destroyed.",
			subject_string_o()
		);

		return;
	}

	Message.object_article = ARTICLE_NONE;

	message(SID_GAME, "%s %s is destroyed.",
		possessive_form(subject_string()),
		object_string()
	);
}



/*
 * determines whether or not the subject and the object are visible
 */
static void determine_visibility(void)
{

	if (ForceVisibility) {

		Message.subject_noticed = true;
		Message.object_noticed = true;

		return;
	}

	if (Message.subject != NULL) {

		if (party_can_see(Message.subject)) {

			Message.subject_noticed = true;

		} else {

			Message.subject_noticed = false;
		}
	}

	if (Message.object != NULL) {

		if (Message.object_type != MOT_CHARACTER) {

			/* hack - fix later */
			Message.object_noticed = true;

		} else if (party_can_see(Message.object)) {

			Message.object_noticed = true;

		} else {

			Message.object_noticed = false;
		}
	}
}



/*
 * standard SP message
 */
static void sp_message(const char *verb)
{

	message(SID_GAME, "%s %s.",
		subject_string(),
		conjugated_verb(verb,
			grammatical_person(Message.subject)
		)
	);
}



/*
 * standard SPO message
 */
static void spo_message(const char *verb)
{

	Message.object_article = ARTICLE_DEFINITE;

	message(SID_GAME, "%s %s %s.",
		subject_string(),
		conjugated_verb(verb,
			grammatical_person(Message.subject)
		),
		object_string()
	);
}



/*
 * standard SPO message + failure
 */
static void spo_message_fail(const char *verb)
{
	char c_try[STRING_BUFFER_SIZE];

	Message.object_article = ARTICLE_DEFINITE;

	strcpy(c_try, conjugated_verb("try",
		grammatical_person(Message.subject))
	);

	message(SID_GAME, "%s %s to %s %s but %s.",
		subject_string(),
		c_try,
		verb,
		object_string(),
		conjugated_verb("fail",
			grammatical_person(Message.subject)
		)
	);
}



/*
 * standard possessive message i.e. possessive form of subject + text
 */
static void possessive_message(const char *text)
{

	message(SID_GAME, "%s %s.",
		possessive_form(subject_string()),
		text
	);
}


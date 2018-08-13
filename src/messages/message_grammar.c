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
 * Module: Message Grammar
 */

#include "wrogue.h"




/*
 * the currently processed message
 */
extern MESSAGE_DATA	Message;






static char *		message_string(char *, const void *,
				MESSAGE_OBJECT_TYPE, ARTICLE, bool
			);

static bool		is_vowel(int);




/*
 * subject string buffer
 */
static char		SubjectString[STRING_BUFFER_SIZE];



/*
 * object string buffers
 */
static char		ObjectString[STRING_BUFFER_SIZE];
static char		LowercaseObjectString[STRING_BUFFER_SIZE];



/*
 * possessive pronouns
 */
static const char	PossessivePronoun[MAX_PRONOUNS][PRONOUN_SIZE] = {

	"his",
	"her",
	"its",
	"your"

};



/*
 * reflexive pronouns
 */
static const char	ReflexivePronoun[MAX_PRONOUNS][PRONOUN_SIZE] = {

	"himself",
	"herself",
	"itself",
	"yourself"

};



/*
 * conjugated verb buffer
 */
static char		ConjugatedVerb[STRING_BUFFER_SIZE];



/*
 * plural form buffer
 */
static char		PluralForm[STRING_BUFFER_SIZE];




/*
 * creates subject strings
 */
char * subject_string(void)
{

	if (!Message.subject_noticed) {

		strcpy(SubjectString, "Something");

		return SubjectString;
	}

	return message_string(SubjectString, Message.subject,
		MOT_CHARACTER, ARTICLE_DEFINITE, true
	);
}



/*
 * creates subject strings (version for OBJECTs)
 */
char * subject_string_o(void)
{

	if (!Message.subject_noticed) {

		strcpy(SubjectString, "Something");

		return SubjectString;
	}

	return message_string(SubjectString, Message.object,
		MOT_OBJECT, ARTICLE_DEFINITE, true
	);
}



/*
 * creates object strings
 */
char * object_string(void)
{

	if (Message.subject == Message.object) {

		strcpy(ObjectString,
			reflexive_pronoun(Message.subject)
		);

		return ObjectString;
	}

	if (!Message.object_noticed) {

		strcpy(ObjectString, "something");

		return ObjectString;
	}

	return message_string(ObjectString, Message.object,
		Message.object_type, Message.object_article,
		false
	);
}



/*
 * converts a message string to possessive form
 */
char * possessive_form(char *string)
{
	int string_length;

	if (strings_equal(string, "you") || strings_equal(string, "You")) {

		strcat(string, "r");

		return string;
	}

	string_length = strlen(string);

	if (string[string_length - 1] == 's') {

		strcat(string, "'");

	} else {

		strcat(string, "'s");
	}

	return string;
}



/*
 * converts a string to plural form
 */
const char * plural_form(const char *string)
{
	char last_char;

	strcpy(PluralForm, string);

	last_char = PluralForm[strlen(PluralForm) - 1];

	if (last_char == 's' || last_char == 'x') {

		strcat(PluralForm, "es");

	} else {

		strcat(PluralForm, "s");
	}

	return PluralForm;
}



/*
 * returns the possessive pronoun appropriate for the passed character
 */
const char * possessive_pronoun(const CHARACTER *character)
{

	if (grammatical_person(character) == GRAMMATICAL_PERSON_2ND) {

		return PossessivePronoun[PRONOUN_2ND_PERSON];
	}

	return PossessivePronoun[character->gender];
}



/*
 * returns the reflexive pronoun appropriate for the passed character
 */
const char * reflexive_pronoun(const CHARACTER *character)
{

	if (grammatical_person(character) == GRAMMATICAL_PERSON_2ND) {

		return ReflexivePronoun[PRONOUN_2ND_PERSON];
	}

	return ReflexivePronoun[character->gender];
}



/*
 * returns the conjugated form of a verb
 */
const char * conjugated_verb(const char *verb, GRAMMATICAL_PERSON person)
{
	char last_letter;
	int last_letter_index;

	if (strings_equal(verb,"be")) {

		if (person == GRAMMATICAL_PERSON_2ND) {

			strcpy(ConjugatedVerb, "are");

		} else if (person == GRAMMATICAL_PERSON_3RD) {

			strcpy(ConjugatedVerb, "is");
		}

		return ConjugatedVerb;
	}

	strcpy(ConjugatedVerb, verb);

	if (person == GRAMMATICAL_PERSON_2ND) {

		return ConjugatedVerb;
	}

	last_letter_index = strlen(verb) - 1;

	last_letter = verb[last_letter_index];

	if (last_letter == 'y') {

		ConjugatedVerb[last_letter_index] = 'i';

		strcat(ConjugatedVerb, "es");

	} else if (last_letter == 's') {

		strcat(ConjugatedVerb, "es");

	} else {

		strcat(ConjugatedVerb, "s");
	}

	return ConjugatedVerb;
}



/*
 * returns the grammatical person of a character
 */
GRAMMATICAL_PERSON grammatical_person(const CHARACTER *character)
{

	if (is_player_controlled_character(character)) {

		return GRAMMATICAL_PERSON_2ND;
	}

	return GRAMMATICAL_PERSON_3RD;
}



/*
 * returns the lowercase version of an object string
 */
char * lowercase_object_string(void)
{

	strcpy(LowercaseObjectString, Message.object);

	string_to_lowercase(LowercaseObjectString);

	return LowercaseObjectString;
}



/*
 * creates message strings like "you", "the ork", "A rock", etc.
 */
static char * message_string(char *message_object_string,
	const void *message_object,
	MESSAGE_OBJECT_TYPE message_object_type, ARTICLE article,
	bool is_subject
)
{
	char message_object_name[STRING_BUFFER_SIZE];
	bool add_article;

	if (message_object == NULL) {

		return message_object_string;
	}

	if (message_object_type == MOT_CHARACTER) {
		const CHARACTER *character;

		character = (const CHARACTER *)message_object;

		if (is_player_controlled_character(character)) {

			strcpy(message_object_name, "You");

			if (!is_subject) {

				message_object_name[0] = (char)tolower(
					message_object_name[0]
				);
			}

		} else {

			strcpy(message_object_name, character->name);
		}

		if (character_has_attribute(character, CA_UNIQUE)) {

			add_article = false;

		} else {

			add_article = true;

			message_object_name[0] = (char)tolower(
				message_object_name[0]
			);
		}

	} else if (message_object_type == MOT_OBJECT) {
		const OBJECT *object = message_object;

		strcpy(message_object_name, object->name);

		if (object_has_attribute(object, OA_UNIQUE) ||
			object->numerus == NUMERUS_PLURAL) {

			add_article = false;

		} else {

			add_article = true;
		}

		if (!object_has_attribute(object, OA_UNIQUE)) {

			message_object_name[0] = (char)tolower(
				message_object_name[0]
			);
		}

	} else {

		die("*** CORE ERROR *** invalid message object type");

		return message_object_string;
	}

	if (article == ARTICLE_NONE) {

		add_article = false;
	}

	if (add_article) {

		if (article == ARTICLE_DEFINITE) {

			strcpy(message_object_string, "the ");

		} else if (is_vowel(message_object_name[0])) {

			strcpy(message_object_string, "an ");

		} else {

			strcpy(message_object_string, "a ");
		}

		if (is_subject) {

			message_object_string[0] = (char)toupper(
				message_object_string[0]
			);
		}

		strcat(message_object_string, message_object_name);

	} else {

		strcpy(message_object_string, message_object_name);
	}

	return message_object_string;
}



/*
 * returns true if c is a vowel or false if it is not
 */
static bool is_vowel(int c)
{

	if (isupper(c)) {

		c = tolower(c);
	}

	if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {

		return true;
	}

	return false;
}


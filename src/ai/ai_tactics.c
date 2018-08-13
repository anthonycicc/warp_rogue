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
 * Module: AI Tactics
 */

#include "wrogue.h"



/*
 * AI tactic names
 */
static const char AiTacticName[MAX_AI_TACTICS][AI_TACTIC_NAME_SIZE] = {

	"Plan A!",
	"Get ready!",
	"Stay close!",
	"Hold position!"

};



/*
 * sets the tactic of the AI
 */
void ai_set_tactic(AI_CONTROL_DATA *ai, AI_TACTIC tactic)
{

	ai->tactic = tactic;
}



/*
 * returns the name of the passed AI tactic
 */
const char * ai_tactic_name(AI_TACTIC tactic)
{

	return AiTacticName[tactic];
}




/*
 * returns a list which contains the names of all available AI tactics
 */
LIST * tactics_list(void)
{
	LIST *list = list_new();
	AI_TACTIC tactic;

	/* build list */
	iterate_over_tactics(tactic) {

		list_add(list, (void *)AiTacticName[tactic]);
	}

	/* return list */
	return list;
}



/*
 * name -> AI tactic
 */
AI_TACTIC name_to_tactic(const char *name)
{
	AI_TACTIC tactic;

	iterate_over_tactics(tactic) {

		if (strings_equal(name, AiTacticName[tactic])) {

			return tactic;
		}
	}

	die("*** CORE ERROR *** invalid tactic: %s", name);

	/* never reached */
	return TACTIC_NIL;
}





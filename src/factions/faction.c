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
 * Module: Faction
 */

#include "wrogue.h"





/*
 * the faction relationship table
 */
static FACTION_RELATIONSHIP **	FactionRelationship = NULL;


/*
 * the faction list
 */
static LIST *		FactionList = NULL;



/*
 * faction relationship names
 */
static const char	FactionRelationshipName[MAX_FACTION_RELATIONSHIPS]
				[FACTION_RELATIONSHIP_NAME_SIZE] = {

	"Friendly",
	"Neutral",
	"Hostile"

};





/*
 * Faction module init
 */
void faction_init(void)
{

	/* allocate faction list */
	FactionList = list_new();

	/* add player faction */
	faction_add("Player");
}



/*
 * Faction module clean up
 */
void faction_clean_up(void)
{

	/* initialisation never happend - return */
	if (FactionList == NULL) return;

	/* free faction relationship table */
	if (FactionRelationship != NULL) {
		N_FACTIONS n = FactionList->n_nodes;
		N_FACTIONS i;	

		for (i = 0; i < n; i++) {

			free(FactionRelationship[i]);
		}
	
		free(FactionRelationship);		
	}

	/* free faction list */
	list_free_with(FactionList, free);
}




/*
 * adds a faction
 */
void faction_add(const char *name)
{
	char *item = checked_malloc(FACTION_NAME_SIZE);
	
	strcpy(item, name);
	
	list_add(FactionList, item);
}



/*
 * builds the faction relationship table 
 *
 * you can not add additional factions after this function has been called 
 */
void factions_finalise(void)
{
	N_FACTIONS n = FactionList->n_nodes;
	FACTION y, x;

	/* allocate table */
	FactionRelationship = checked_malloc(n * 
		sizeof *FactionRelationship
	);
	for (y = 0; y < n; y++) {

		FactionRelationship[y] = checked_malloc(n * 
			sizeof **FactionRelationship
		);
	}

	/* fill table will default values */
	for (y = 0; y < n; y++) {

		for (x = 0; x < n; x++) {

			FactionRelationship[y][x] = FR_DEFAULT;
		}
	}
}



/*
 * returns the relationship between the passed factions
 */
FACTION_RELATIONSHIP faction_relationship(FACTION f1, FACTION f2)
{

	return FactionRelationship[f1][f2];
}



/*
 * sets the relationship between the passed factions
 */
void faction_set_relationship(FACTION f1, FACTION f2, 
	FACTION_RELATIONSHIP relationship
)
{

	FactionRelationship[f1][f2] = relationship;
	FactionRelationship[f2][f1] = relationship;
}


/*
 * returns the name of the passed faction
 */
const char * faction_name(FACTION faction)
{
	
	return list_data_at(FactionList, faction);
}



/*
 * returns the number of factions
 */
N_FACTIONS n_factions(void)
{

	return FactionList->n_nodes;
}



/*
 * name -> faction
 */
FACTION name_to_faction(const char *name)
{
	LIST_NODE *node;
	FACTION faction = FACTION_LB;
	
	iterate_over_list(FactionList, node) {
		const char *f_name = node->data;

		if (strings_equal(f_name, name)) return faction;

		++faction;
	}

	die("*** CORE ERROR *** invalid faction: %s", name);
	
	return faction; /* never reached */
}



/*
 * name -> faction relationship
 */
FACTION_RELATIONSHIP name_to_faction_relationship(const char *name)
{
	FACTION_RELATIONSHIP i;

	for (i = FR_LB; i < MAX_FACTION_RELATIONSHIPS; i++) {

		if (strings_equal(name, FactionRelationshipName[i])) {

			return i;
		}
	}

	die("*** CORE ERRROR *** invalid faction relationship: %s", name);

	return FR_NIL; /* never reached */
}



/*
 * prevents the player from accidently attacking non-hostile characters
 * and executes the consequences if the player confirms the attack
 */
bool hostility_check(CHARACTER *target)
{
	CONFIRMATION_DIALOGUE dialogue;

	if (hostility_between(player_character(), target)) return true;

	strcpy(dialogue.prompt, "Attack against non-hostile character, sure?");
	dialogue.confirmed = false;

	attack_confirmation_dialogue(&dialogue);

	if (!dialogue.confirmed) return false;

	faction_set_relationship(FACTION_PLAYER, target->faction, FR_HOSTILE);

	return true;
}



/*
 * returns true if there is hostility between the passed characters
 */
bool hostility_between(const CHARACTER *c1, const CHARACTER *c2)
{

	if (FactionRelationship[c1->faction][c2->faction] == FR_HOSTILE) {
		
		return true;
	}

	return false;
}





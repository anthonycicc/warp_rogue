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
 * Module: Randomiser
 */

#include "wrogue.h"



static RANDOMISER_ITEM *	randomiser_item_create(const char *,
					RVALUE
				);



/*
 * the randomiser list
 */
static LIST *	RandomiserList = NULL;
static RVALUE	RV_Total;




/* 
 * Randomiser module init
 */
void randomiser_init(void)
{

	RandomiserList = list_new();
	RV_Total = 0;
}



/*
 * Randomiser module clean up
 */
void randomiser_clean_up(void)
{

	if (RandomiserList != NULL) {
		
		list_free_with(RandomiserList, free);
	}
}



/*
 * adds an item to the randomiser list
 */
void randomiser_add(const char *string, RVALUE rv)
{
	RANDOMISER_ITEM *randomiser_item = 
		randomiser_item_create(string, rv);
	
	list_add(RandomiserList, randomiser_item);

	RV_Total += rv;
}



/*
 * clears the randomiser list
 */
void randomiser_clear(void)
{

	list_clear_with(RandomiserList, free);

	RV_Total = 0;
}



/*
 * returns a randomly chosen item from randomiser list
 */
const char * randomiser(void)
{
	RANDOMISER_ITEM *item = NULL;
	RVALUE choice;
	RVALUE rv_current = 0;
	LIST_NODE *node;
	
	/* empty list - return "" */
	if (RandomiserList->n_nodes == 0) return "";

	/* 
	 * choose an item 
	 */
	choice = random_int(1, RV_Total);
	
	iterate_over_list(RandomiserList, node) {
		
		item = (RANDOMISER_ITEM *)node->data;

		rv_current += item->rv;

		if (choice <= rv_current) break;
	}
	
	/* this should never happend */
	if (item == NULL) {
		die("*** CORE ERROR *** invalid randomiser call");
		return NULL; /* never reached */
	}

	/* return item string */
	return item->string;
}



/*
 * creates a randomiser list item
 */
static RANDOMISER_ITEM * randomiser_item_create(const char *string, 
	RVALUE rv
)
{
	RANDOMISER_ITEM *randomiser_item = 
		checked_malloc(sizeof *randomiser_item);

	strcpy(randomiser_item->string, string);

	randomiser_item->rv = rv;
	
	return randomiser_item;
}


/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Box ADT
 *
 *
 * NAMESPACE: BOX_* / box_*
 *
 */


#include <stdlib.h>
#include <stdbool.h>


#include "box.h"



/*
 * box template (default values)
 */
static const BOX	BoxTemplate = {
	
	/* ITEM */ NULL, /* ITEM_NEW */ NULL, /* ITEM_FREE */ NULL,
	/* CURRENT_SIZE */ 0, /* MAX_SIZE */ 0, /* BUFFER_SIZE */ 0
	
};




/*
 * creates a new box
 */
BOX * box_new(void * (*item_new)(void), void (*item_free)(void *),
	BOX_SIZE buffer_size
)
{
	BOX *box;

	box = malloc(sizeof *box);
	
	if (box == NULL) return NULL;
	
	*box = BoxTemplate;
	
	box->item_new = item_new;
	box->item_free = item_free;

	box->buffer_size = buffer_size;
	
	return box;
}



/*
 * allocates space for a new item 
 */
void * box_new_item(BOX *box)
{
	void *item;
	BOX_ITEM_INDEX new_item_index;
	
	new_item_index = box->current_size;

	box->current_size += 1;

	if (box->current_size > box->max_size) {
		BOX_ITEM_INDEX i;
		
		box->max_size += box->buffer_size;

		box->item = realloc(box->item,
			box->max_size * sizeof *box->item
		);
		
		if (box->item == NULL) return NULL;
		
		for (i = new_item_index; i < box->max_size; i++) {
			
			box->item[i] = (*box->item_new)();

			if (box->item[i] == NULL) return NULL;
		}
		
	}

	item = box->item[new_item_index];

	return item;
}




/*
 * optimises a box
 */
bool box_optimise(BOX *box)
{
	BOX_ITEM_INDEX i;

	if (box->current_size == box->max_size) {
	
		return true;
	}
	
	for (i = box->current_size; i < box->max_size; i++) {
			
		(*box->item_free)(box->item[i]);
	}
		
	box->max_size = box->current_size;

	box->item = realloc(box->item, box->max_size * sizeof *box->item);
	
	if (box->item == NULL) return false;

	return true;
}



/*
 * frees a box
 */
void box_free(BOX *box)
{
	BOX_ITEM_INDEX i;

	for (i = 0; i < box->max_size; i++) {

		(*box->item_free)(box->item[i]);
	}

	free(box->item);

	free(box);
}



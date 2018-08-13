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

/*
 * box size type
 */
typedef int		BOX_SIZE;


/*
 * box item index type 
 */
typedef BOX_SIZE	BOX_ITEM_INDEX;


/*
 * box type
 */
typedef struct {
	
	/* box items */
	void **		item;
	
	/* box item alloc function */
	void *		(*item_new)(void);
	
	/* box item free function */
	void		(*item_free)(void *);

	/* number of items in the box */
	BOX_SIZE	current_size;

	/* allocated space for items */
	BOX_SIZE	max_size;
	
	/* box buffer size */
	BOX_SIZE	buffer_size;

} BOX;


BOX *	box_new(void * (*)(void), void (*)(void *), BOX_SIZE);

void *	box_new_item(BOX *);

bool	box_optimise(BOX *);

void	box_free(BOX *);




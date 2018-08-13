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
 * Linked List ADT
 *
 *
 * NAMESPACE: LIST_* / list_*
 *
 */


typedef int LIST_NODE_INDEX;
#define	LIST_NODE_INDEX_NIL		-1

typedef int LIST_N_NODES;


typedef struct list_node_struct		LIST_NODE;

struct list_node_struct {
	
	void *		data;
	
	LIST_NODE *	prev;
	LIST_NODE *	next;
	
};


typedef struct {
	
	LIST_NODE *	head;
	LIST_NODE *	tail;
	
	LIST_N_NODES	n_nodes;

} LIST;


#define iterate_over_list(l,i)	\
	for ((i) = (l)->head; (i) != NULL; (i) = (i)->next)


LIST *		list_new(void);

void		list_free(LIST *);
void		list_free_with(LIST *, void (*)(void *));

void		list_clear(LIST *);
void		list_clear_with(LIST *, void (*)(void *));

LIST_NODE *	list_add(LIST *, void *);
void		list_add_list(LIST *, LIST *);

LIST_NODE *	list_insert(LIST *, LIST_NODE *, void *);
LIST_NODE *	list_insert_sorted(LIST *, void *, int (*)(void *, void *));

void		list_remove(LIST *, const void *);
void		list_remove_node(LIST *, LIST_NODE *);

void *		list_data_at(LIST *, LIST_NODE_INDEX);

LIST *		list_clone(LIST *, size_t);

void		list_write(FILE *, const LIST *, size_t);
void		list_read(LIST *, FILE *, size_t);

LIST_NODE *	list_node_of(LIST *, const void *);
LIST_NODE *	list_node_at(LIST *, LIST_NODE_INDEX);



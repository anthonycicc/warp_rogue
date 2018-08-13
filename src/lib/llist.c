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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "llist.h"




/*
 * creates a new (empty) list
 */
LIST * list_new(void)
{
	LIST *new_list;

	new_list = malloc(sizeof *new_list);
	if (new_list == NULL) exit(EXIT_FAILURE);
	new_list->head = NULL;
	new_list->tail = NULL;
	new_list->n_nodes = 0;

	return new_list;
}



/*
 * frees a list
 */
void list_free(LIST *list)
{
	if (list == NULL) return;

	list_clear(list);

	free(list);
}



/*
 * frees a list
 * data items are freed using the passed destructor
 */
void list_free_with(LIST *list, void (*destructor)(void *))
{
	
	if (list == NULL) return;
		
	list_clear_with(list, destructor);
	
	free(list);
}



/*
 * removes all notes from a list
 */
void list_clear(LIST *list)
{
	LIST_NODE *node;
	LIST_NODE *next_node;

	if (list == NULL) return;

	node = list->head;

        while (node != NULL) {
		next_node = node->next;

		free(node);

		node = next_node;
        }

	list->head = list->tail = NULL;
	list->n_nodes = 0;
}



/*
 * removes all notes from a list
 * data items are freed using the passed destructor
 */
void list_clear_with(LIST *list, void (*destructor)(void *))
{
	LIST_NODE *node;
	LIST_NODE *next_node;

	if (list == NULL) return;

	node = list->head;

        while (node != NULL) {
		
		next_node = node->next;

		(*destructor)(node->data);

		free(node);

		node = next_node;
        }

	list->head = list->tail = NULL;
	list->n_nodes = 0;
}



/*
 * adds a new piece of data to a list
 * (attached at tail)
 */
LIST_NODE * list_add(LIST *list, void *data)
{
	LIST_NODE *new_node;

	new_node = malloc(sizeof *new_node);
	if (new_node == NULL) exit(EXIT_FAILURE);

	list->n_nodes += 1;

	new_node->data = data;

	if (list->head == NULL) {
		new_node->prev = NULL;
		new_node->next = NULL;
		list->head = list->tail = new_node;
		return new_node;
	}

	list->tail->next = new_node;
	new_node->prev = list->tail;
	new_node->next = NULL;
	list->tail = new_node;

	return new_node;
}



/*
 * adds a list of data items to a list
 * (attached at tail)
 */
void list_add_list(LIST *list, LIST *list_to_add)
{
	LIST_NODE *node;

	for (node = list_to_add->head;
		node != NULL;
		node = node->next) {

		list_add(list, node->data);
	}

}



/*
 * inserts a new piece of data into a list
 * the data is inserted after the passed node
 *
 * if NULL is passed the data is attached in front
 * of the current head node i.e. it becomes the new
 * head node
 */
LIST_NODE * list_insert(LIST *list, LIST_NODE *node, void *data)
{
	LIST_NODE *new_node;

	if (list->head == NULL || node == list->tail) {
		return list_add(list, data);
	}

	new_node = malloc(sizeof *new_node);
	if (new_node == NULL) exit(EXIT_FAILURE);

	list->n_nodes += 1;

	new_node->data = data;

	if (node == NULL) {

		list->head->prev = new_node;
		new_node->next = list->head;
		new_node->prev = NULL;
		list->head = new_node;

		return new_node;
	}

	new_node->prev = node;
	new_node->next = node->next;
	node->next = new_node;

	return new_node;
}



/*
 * inserts a new piece of data into a list
 * uses insert sort
 */
LIST_NODE * list_insert_sorted(LIST *list, void *data,
	int (*smaller_than)(void *, void *))
{
	LIST_NODE *node;

        for (node = list->head; node != NULL; node = node->next) {

		if ((*smaller_than)(data, node->data)) break;
        }

	if (node == NULL) {
		return list_add(list, data);
	}

	return list_insert(list, node->prev, data);
}



/*
 * removes a piece of data from a list
 */
void list_remove(LIST *list, const void *data)
{
	LIST_NODE *node;

	node = list_node_of(list, data);

	if (node == NULL) return;

	list_remove_node(list, node);
}



/*
 * removes a node from a list
 */
void list_remove_node(LIST *list, LIST_NODE *node)
{

	if (node->prev == NULL) {

		if (node->next == NULL) {
			list->head = list->tail = NULL;
		} else {
			list->head = node->next;
			node->next->prev = NULL;
		}

	} else if (node->next == NULL) {

		list->tail = node->prev;
		node->prev->next = NULL;

	} else {

		node->prev->next = node->next;
		node->next->prev = node->prev;

	}

	free(node);

	list->n_nodes -= 1;
}





/*
 * returns the data located at the passed index
 */
void * list_data_at(LIST *list, LIST_NODE_INDEX node_index)
{
        LIST_NODE *node;

	node = list_node_at(list, node_index);

	if (node == NULL) return NULL;

	return node->data;
}



/*
 * produces an identical copy of a list
 */
LIST * list_clone(LIST *src_list, size_t item_size)
{
	LIST *list;
	LIST_NODE *node;
	LIST_NODE *data;

	list = list_new();

	for (node = src_list->head;
		node != NULL;
		node = node->next) {

		data = malloc(item_size);
		if (data == NULL) exit(EXIT_FAILURE);

		memcpy(data, node->data, item_size);

		list_add(list, data);
	}

	return list;
}



/*
 * writes a simple (flat, uniform) list into a binary file stream
 */
void list_write(FILE *out_file, const LIST *list, size_t item_size)
{
	LIST_NODE *node;

	fwrite(&list->n_nodes, sizeof list->n_nodes, 1, out_file);

	for (node = list->head; node != NULL; node = node->next) {

		fwrite(node->data, item_size, 1, out_file);
	}

}



/*
 * reads a simple (flat, uniform) list from a binary file stream
 */
void list_read(LIST *list, FILE *in_file, size_t item_size)
{
	int i;
	char *item;
	int n_nodes;

	fread(&n_nodes, sizeof n_nodes, 1, in_file);

	for (i = n_nodes; i > 0; i--) {

		item = malloc(item_size);
		if (item == NULL) exit(EXIT_FAILURE);

		fread(item, item_size, 1, in_file);

		list_add(list, item);
	}
}



/*
 * returns a pointer to the list node of a data item
 */
LIST_NODE * list_node_of(LIST *list, const void *data)
{
        LIST_NODE *current_node;

	current_node = list->head;

        while (current_node != NULL) {

		if (current_node->data == data) {
			return current_node;
		}

		current_node = current_node->next;
        }

	return NULL;
}



/*
 * returns the node located at the passed index
 */
LIST_NODE * list_node_at(LIST *list, LIST_NODE_INDEX node_index)
{
        LIST_NODE *node;
	LIST_NODE_INDEX i;

	node = list->head;
	i = 0;
	while (node != NULL) {

		if (i == node_index) {
			return node;
		}

		node = node->next;
		++i;
        }

	return NULL;
}



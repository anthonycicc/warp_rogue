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
 * Module: Shop
 */

#include "wrogue.h"




static void		shop_menu_create(void);

static void		shop_destroy(void);

static bool		sell_object(CHARACTER *, OBJECT *);

static void		shop_exchange(CHARACTER *, OBJECT *);

static MONEY		shop_object_price(const OBJECT *);

static bool		customer_can_acquire(const CHARACTER *,
				const OBJECT *, MONEY
			);





/*
 * the current shop
 */
static SHOP		Shop = {

	/* OBJECT_LIST */ NULL, /* MENU */ NULL,
	/* PRICE_LEVEL */ 100

};





/*
 * the shop - sell mode
 */
void shop_sell_mode(CHARACTER *customer)
{
	COMMAND entered_command;
	OBJECT *object;
	bool object_removed = false;
	int highlighted_object_index = 0;
	int last_object_index;

	do {

		if (customer->inventory->head == NULL) {

			command_bar_set(1, CM_EXIT);

			render_sell_screen(customer,
				highlighted_object_index
			);
			update_screen();

			command_bar_get_command();
			return;
		}

		command_bar_set(5, CM_UP, CM_DOWN, CM_OK, CM_INFO, 
			CM_EXIT
		);

		render_sell_screen(customer, highlighted_object_index);
		update_screen();

		entered_command = command_bar_get_command();

		object = list_data_at(customer->inventory,
			highlighted_object_index
		);
		
		last_object_index = customer->inventory->n_nodes - 1;

		if (entered_command == CM_UP) {

			if (highlighted_object_index > 0) {

				--highlighted_object_index;
			}

		} else if (entered_command == CM_DOWN) {

			if (highlighted_object_index < last_object_index) {

				++highlighted_object_index;
			}

		} else if (entered_command == CM_OK) {
			
			if (sell_object(customer, object)) {
				
				object_removed = true;
			}

		} else if (entered_command == CM_INFO) {

			object_screen(object);
		}

		if (object_removed) {

			if (highlighted_object_index > 0) {

				--highlighted_object_index;
			}

			--last_object_index;

			object_removed = false;
		}

	} while (entered_command != CM_EXIT);	
}



/*
 * the shop - buy mode
 */
void shop_buy_mode(CHARACTER *customer)
{
	
	shop_menu_create();

	do  {
		OBJECT *object;

		command_bar_set(5, CM_UP, CM_DOWN, CM_OK,
			CM_INFO, CM_EXIT
		);

		render_shop_screen(customer);

		shop_screen_menu(Shop.menu);

		object = (OBJECT *)list_data_at(Shop.object_list,
			Shop.menu->highlighted
		);

		if (Shop.menu->entered_command == CM_OK) {

			shop_exchange(customer, object);

		} else if (Shop.menu->entered_command == CM_INFO) {

			object_screen(object);
		}

	} while (Shop.menu->entered_command != CM_EXIT);

	shop_destroy();
}



/*
 * adds an object to the shop
 */
void shop_add(const char *object_name)
{

	if (Shop.object_list == NULL) {

		Shop.object_list = list_new();
	}

	list_add(Shop.object_list, object_create(object_name));
}



/*
 * sets the price level of the shop
 */
void shop_set_price_level(PRICE_LEVEL price_level)
{

	Shop.price_level = price_level;
}



/*
 * creates the shop menu
 */
static void shop_menu_create(void)
{
	LIST *menu_items;
	LIST_NODE *node;

	menu_items = list_new();

	for (node = Shop.object_list->head;
		node != NULL;
		node = node->next) {
		OBJECT *object;
		char *entry;

		object = (OBJECT *)node->data;

		entry = checked_malloc(STRING_BUFFER_SIZE);

		sprintf(entry, "%s (%dc) ",
			object->name,
			shop_object_price(object)
		);

		list_add(menu_items, entry);
	}

	Shop.menu = menu_create(menu_items);
}



/* 
 * destroys the shop
 */
static void shop_destroy(void)
{
	
	menu_free_with(Shop.menu, free);
	Shop.menu = NULL;

	list_free_with(Shop.object_list, object_free);
	Shop.object_list = NULL;
}



/*
 * makes the customer sell the passed object
 */
static bool sell_object(CHARACTER *customer, OBJECT *object)
{

        if (object->type == OTYPE_MONEY ||
		object->value == VALUE_NIL) {

		message(SID_INVENTORY, "You cannot sell that");
		
		return false;
	}

	inventory_money_increase(customer, object->value / 2);

	message(SID_INVENTORY, "%s sold", object->name);
	
	inventory_remove(customer, object);

	object_destroy(object);

	return true;
}



/*
 * shop exchange: customer receives object, shop receives money
 */
static void shop_exchange(CHARACTER *customer, OBJECT *object)
{
	MONEY price = shop_object_price(object);

	if (!customer_can_acquire(customer, object, price)) {

		return;
	}

	message(SID_DIALOGUE, "%s received", object->name);

	inventory_money_reduce(customer, price);

	inventory_add(customer, object_clone(object));
}





/*
 * returns the price of an object
 */
static MONEY shop_object_price(const OBJECT *object)
{
	MONEY price = percent(Shop.price_level, object->value);

	return price;
}



/*
 * returns true if the customer can acquire the passed object
 */
static bool customer_can_acquire(const CHARACTER *customer,
	const OBJECT *object, MONEY price
)
{

	if (price > inventory_money(customer)) {

		message(SID_DIALOGUE, "Not enough credits");

		return false;
	}

	if (inventory_is_full(customer)) {

		message(SID_DIALOGUE, "Inventory is full");

		return false;
	}

	if (object->weight + inventory_weight(customer) > 
		ENCUMBRANCE_MAX(customer)) {

		message(SID_DIALOGUE, "You cannot carry any more");

		return false;
	}

	return true;
}



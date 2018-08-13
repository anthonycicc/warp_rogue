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
 * Module: Character Inventory
 */

#include "wrogue.h"




static OBJECT *	inventory_money_object(const CHARACTER *);

static void	inventory_command_drop(CHARACTER *, OBJECT *, bool *);
static void	inventory_command_use(CHARACTER *, OBJECT *, bool *, bool *);

static void	inventory_screen_close(void);




/*
 * the inventory screen
 */
void inventory_screen(CHARACTER *character)
{
	COMMAND entered_command;
	OBJECT *object;
	bool object_removed = false;
	bool screen_closed = false;
	int highlighted_object_index = 0;
	int last_object_index = character->inventory->n_nodes - 1;

	do {

		if (character->inventory->head == NULL) {

			command_bar_set(1, CM_EXIT);

			render_inventory_screen(character,
				highlighted_object_index
			);

			update_screen();

			command_bar_get_command();

			return;
		}

		command_bar_set(6, CM_UP, CM_DOWN, CM_USE,
			CM_DROP, CM_INFO, CM_EXIT
		);

		render_inventory_screen(character, highlighted_object_index);

		update_screen();

		entered_command = command_bar_get_command();

		object = list_data_at(character->inventory,
			highlighted_object_index
		);

		if (entered_command == CM_UP) {

			if (highlighted_object_index > 0) {

				--highlighted_object_index;
			}

		} else if (entered_command == CM_DOWN) {

			if (highlighted_object_index < last_object_index) {

				++highlighted_object_index;
			}

		} else if (entered_command == CM_INFO) {

			object_screen(object);

		} else if (entered_command == CM_DROP) {

			inventory_command_drop(character, object,
				&object_removed
			);

		} else if (entered_command == CM_USE) {

			inventory_command_use(character, object,
				&object_removed, &screen_closed
			);
		} 

		if (screen_closed) {

			return;
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
 * adds an object to a character's inventory
 */
void inventory_add(CHARACTER *character, OBJECT *object)
{

	if (object->type == OTYPE_MONEY) {
		OBJECT *money = inventory_money_object(character);

		if (money != NULL) {

			money->charge += object->charge;
			
			object_destroy(object);

			return;
		}
	}

	list_add(character->inventory, object);
}



/*
 * removes an object from a character's inventory
 */
void inventory_remove(CHARACTER *character, OBJECT *object)
{

	if (object_in_use(character, object)) {

		unequip_object(character, object);
	}

	list_remove(character->inventory, object);
}



/*
 * chooses a random object from a character's inventory and returns a
 * pointer to it
 */
OBJECT * inventory_random_object(CHARACTER *character)
{
	LIST_NODE *node;
	OBJECT *object;
	int n_nodes, i, choice;

	n_nodes = character->inventory->n_nodes;

	if (n_nodes == 0) {

		return NULL;
	}

	choice = random_int(0, n_nodes - 1);

	for (node = character->inventory->head, i = 0;
		node != NULL;
		node = node->next, i++) {

		object = (OBJECT *)node->data;

		if (i == choice) {

			return object;
		}
	}

	return NULL;
}



/*
 * searches for a specific object inside a character's inventory.
 * if the object is found a pointer to it is returned.
 */
OBJECT * inventory_find_object(CHARACTER *character, const char *object_name)
{
	LIST_NODE *node;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object = node->data;

		if (strings_equal(object->name, object_name)){

			return object;
		}
	}

	return NULL;
}



/*
 * returns true if the character's inventory is full
 */
bool inventory_is_full(const CHARACTER *character)
{

	if (character->inventory->n_nodes >=
		MAX_INVENTORY_OBJECTS) {

		return true;
	}

	return false;
}




/*
 * returns the value of the passed character's inventory
 */
MONEY inventory_value(const CHARACTER *character)
{
	LIST_NODE *node;
	MONEY cost = 0;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		const OBJECT *object = node->data;

		if (object->type == OTYPE_MONEY) {

			cost += object->charge;

		} else if (object->value > 0) {

			cost += object->value;
		}
	}

	return cost;
}



/*
 * returns the amount of money a character has
 */
MONEY inventory_money(const CHARACTER *character)
{
	OBJECT *object;

	object = inventory_money_object(character);

	if (object == NULL) return 0;

	return object->charge;
}



/*
 * reduces a character's money
 */
void inventory_money_reduce(CHARACTER *character, MONEY reduction)
{
	OBJECT *object;

	object = inventory_money_object(character);

	if (object == NULL) {
		die("*** CORE ERROR *** inventory_money_reduce() failed");
	}

	object->charge -= reduction;

	if (object->charge <= 0) {
		inventory_remove(character, object);
		object_destroy(object);
	}

}



/*
 * increases a character's money
 */
void inventory_money_increase(CHARACTER *character, MONEY increase)
{
	OBJECT *object;

	object = object_create(MONEY_OBJECT_NAME);
	object->charge = increase;

	inventory_add(character, object);

}



/*
 * prints an inventory object
 */
void inventory_object_print(const CHARACTER *character, const OBJECT *object, FILE *out_file)
{

	fprintf(out_file, "%s", object->name);

	if (object->charge != CHARGE_NIL) {

		fprintf(out_file, " [%d]", object->charge);
	}

	if (object == character->weapon) {

		fprintf(out_file, " (weapon)");

	} else if (object == character->secondary_weapon) {

	        fprintf(out_file, " (secondary weapon)");

	} else if (object == character->armour) {

		fprintf(out_file, " (armour)");

	} else if (object == character->jump_pack) {

		fprintf(out_file, " (jump pack)");
	}

	if (object_has_attribute(character->weapon, OA_JAMMED)) {

		fprintf(out_file, " (jammed)");
	}

	fprintf(out_file, "\n");
}



/*
 * returns the combined weight of all objects in the
 * character's inventory
 */
WEIGHT inventory_weight(const CHARACTER *character)
{
	const LIST_NODE *node;
	WEIGHT total_weight;

	total_weight = 0;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		const OBJECT *object;

		object = (const OBJECT *)node->data;

		if (object->weight == WEIGHT_NIL) continue;

		total_weight += object->weight;
	}

	return total_weight;
}



/*
 * returns a character's money object
 */
static OBJECT * inventory_money_object(const CHARACTER *character)
{
	LIST_NODE *node;

	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object = node->data;

		if (object->type == OTYPE_MONEY) {

			return object;
		}
	}

	return NULL;
}



/*
 * inventory command: drop
 */
static void inventory_command_drop(CHARACTER *character, OBJECT *object, bool *object_removed)
{

	if (object_has_attribute(object, OA_INHERENT)) {
	
		message(SID_INVENTORY, "You cannot drop inherent objects.");
		
		*object_removed = false;

		return;
	}

	message(SID_INVENTORY, "%s dropped", object->name);

	action_drop_object(character, object, false);

	*object_removed = true;
}



/*
 * inventory command: use
 */
static void inventory_command_use(CHARACTER *character, OBJECT *object, 
	bool *object_removed, bool *screen_closed
)
{

	*screen_closed = false;
	*object_removed = false;

	if (object_has_attribute(object, OA_ALIEN) &&
		!character_has_perk(character, PK_ALIEN_TECHNOLOGY)) {

		message(SID_INVENTORY,
			"You need the %s perk to use this object",
			perk_name(PK_ALIEN_TECHNOLOGY)
		);

		return;
	}
	
	if (object_has_attribute(object, OA_DAEMONIC) &&
		!character_has_perk(character, PK_DAEMON_LORE)) {

		message(SID_INVENTORY,
			"You need the %s perk to use this object",
			perk_name(PK_DAEMON_LORE)
		);

		return;
	}

	switch (object->type) {

	case OTYPE_RANGED_COMBAT_WEAPON: /* FALLTHROUGH */
	case OTYPE_CLOSE_COMBAT_WEAPON: /* FALLTHROUGH */
	case OTYPE_ARMOUR: /* FALLTHROUGH */
	case OTYPE_JUMP_PACK:

		use_equipment_object(character, object);
		break;

	case OTYPE_DRUG:

		inventory_screen_close();
		*screen_closed = true;

		if (action_use_drug(character, object)) {
			*object_removed = true;
		}
		break;

	default:

		message(SID_INVENTORY, "You cannot use this object");
	}
}



/*
 * closes the inventory screen
 */
static void inventory_screen_close(void)
{
	use_game_screen_command_bar();
	render_game_screen();
	update_screen();;
}



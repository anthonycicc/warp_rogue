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
 * Module: Script Functions
 */

#include "wrogue.h"


/*
 * script function prototypes
 */
static WCA_C_FUNCTION(sf_random_number);

static WCA_C_FUNCTION(sf_generate_player_character);
static WCA_C_FUNCTION(sf_move_party_to_area);

static WCA_C_FUNCTION(sf_dialogue_say);
static WCA_C_FUNCTION(sf_dialogue_add_option);
static WCA_C_FUNCTION(sf_dialogue_choice);

static WCA_C_FUNCTION(sf_character_has_perk);
static WCA_C_FUNCTION(sf_character_power_level);
static WCA_C_FUNCTION(sf_character_set_faction);

static WCA_C_FUNCTION(sf_party_power_level);
static WCA_C_FUNCTION(sf_party_n_members);
static WCA_C_FUNCTION(sf_party_is_member);
static WCA_C_FUNCTION(sf_party_join);
static WCA_C_FUNCTION(sf_party_leave);

static WCA_C_FUNCTION(sf_personal_funds);
static WCA_C_FUNCTION(sf_add_money);
static WCA_C_FUNCTION(sf_subtract_money);

static WCA_C_FUNCTION(sf_give_object);
static WCA_C_FUNCTION(sf_has_object);
static WCA_C_FUNCTION(sf_remove_object);

static WCA_C_FUNCTION(sf_shop_buy_mode);
static WCA_C_FUNCTION(sf_shop_sell_mode);
static WCA_C_FUNCTION(sf_shop_add);
static WCA_C_FUNCTION(sf_shop_set_price_level);

static WCA_C_FUNCTION(sf_quest_set_status);
static WCA_C_FUNCTION(sf_quest_status);

static WCA_C_FUNCTION(sf_scenario_end);

static WCA_C_FUNCTION(sf_dungeon_generate);
static WCA_C_FUNCTION(sf_dungeon_set_wall);
static WCA_C_FUNCTION(sf_dungeon_set_floor);
static WCA_C_FUNCTION(sf_dungeon_set_connections);
static WCA_C_FUNCTION(sf_dungeon_set_obstacles);
static WCA_C_FUNCTION(sf_dungeon_set_pools);
static WCA_C_FUNCTION(sf_dungeon_set_gate_level);
static WCA_C_FUNCTION(sf_dungeon_set_environment);

static WCA_C_FUNCTION(sf_cave_generate);
static WCA_C_FUNCTION(sf_cave_set_type);
static WCA_C_FUNCTION(sf_cave_set_wall);
static WCA_C_FUNCTION(sf_cave_set_floor);
static WCA_C_FUNCTION(sf_cave_set_connections);
static WCA_C_FUNCTION(sf_cave_set_gate_level);
static WCA_C_FUNCTION(sf_cave_set_environment);

static WCA_C_FUNCTION(sf_spawn_character);
static WCA_C_FUNCTION(sf_spawn_object);

static WCA_C_FUNCTION(sf_area_name);
static WCA_C_FUNCTION(sf_area_n_characters);
static WCA_C_FUNCTION(sf_area_remove_gore_features);
static WCA_C_FUNCTION(sf_area_make_ruins);
static WCA_C_FUNCTION(sf_area_add_random_gore);

static WCA_C_FUNCTION(sf_randomiser_add);
static WCA_C_FUNCTION(sf_randomiser);
static WCA_C_FUNCTION(sf_randomiser_spawn_characters);

static WCA_C_FUNCTION(sf_inventory_value);

static WCA_C_FUNCTION(sf_faction_set_relationship);







/*
 * adds the Warp Rogue specific functions to the interpreter
 */
void script_add_functions(void)
{

	wca_add_function("random_number", sf_random_number);

	wca_add_function("generate_player_character",
		sf_generate_player_character
	);
	
	wca_add_function("move_party_to_area",
		sf_move_party_to_area
	);

	wca_add_function("dialogue_say", sf_dialogue_say);
	wca_add_function("dialogue_add_option",	sf_dialogue_add_option);
	wca_add_function("dialogue_choice", sf_dialogue_choice);

	wca_add_function("character_has_perk", sf_character_has_perk);
	wca_add_function("character_power_level", 
		sf_character_power_level
	);
	wca_add_function("character_set_faction", sf_character_set_faction);

	wca_add_function("party_power_level", sf_party_power_level);
	wca_add_function("party_n_members", sf_party_n_members);
	wca_add_function("party_is_member", sf_party_is_member);
	wca_add_function("party_join", sf_party_join);
	wca_add_function("party_leave", sf_party_leave);

	wca_add_function("inventory_value", sf_inventory_value);
	wca_add_function("personal_funds", sf_personal_funds);
	wca_add_function("add_money", sf_add_money);
	wca_add_function("subtract_money", sf_subtract_money);
	
	wca_add_function("give_object", sf_give_object);
	wca_add_function("has_object",	sf_has_object);
	wca_add_function("remove_object", sf_remove_object);

	wca_add_function("shop_buy_mode", sf_shop_buy_mode);
	wca_add_function("shop_sell_mode", sf_shop_sell_mode);
	wca_add_function("shop_add", sf_shop_add);
	wca_add_function("shop_set_price_level", sf_shop_set_price_level);

	wca_add_function("quest_set_status", sf_quest_set_status);
	wca_add_function("quest_status", sf_quest_status);
	
	wca_add_function("scenario_end", sf_scenario_end);

	wca_add_function("dungeon_generate", sf_dungeon_generate);
	wca_add_function("dungeon_set_wall", sf_dungeon_set_wall);
	wca_add_function("dungeon_set_floor", sf_dungeon_set_floor);
	wca_add_function("dungeon_set_connections", 
		sf_dungeon_set_connections
	);
	wca_add_function("dungeon_set_obstacles", 
		sf_dungeon_set_obstacles
	);
	wca_add_function("dungeon_set_pools", sf_dungeon_set_pools);
	wca_add_function("dungeon_set_gate_level", 
		sf_dungeon_set_gate_level
	);
	wca_add_function("dungeon_set_environment", 
		sf_dungeon_set_environment
	);
	
	wca_add_function("cave_generate", sf_cave_generate);
	wca_add_function("cave_set_type", sf_cave_set_type);
	wca_add_function("cave_set_wall", sf_cave_set_wall);
	wca_add_function("cave_set_floor", sf_cave_set_floor);
	wca_add_function("cave_set_connections", 
		sf_cave_set_connections
	);
	wca_add_function("cave_set_gate_level", 
		sf_cave_set_gate_level
	);
	wca_add_function("cave_set_environment", 
		sf_cave_set_environment
	);

	wca_add_function("spawn_character", sf_spawn_character);
	wca_add_function("spawn_object", sf_spawn_object);
	
	wca_add_function("area_name", sf_area_name);
	wca_add_function("area_n_characters", sf_area_n_characters);
	wca_add_function("area_remove_gore_features",
		sf_area_remove_gore_features
	);
	wca_add_function("area_make_ruins", sf_area_make_ruins);
	wca_add_function("area_add_random_gore", sf_area_add_random_gore);

	wca_add_function("randomiser_add", sf_randomiser_add);
	wca_add_function("randomiser", sf_randomiser);
	wca_add_function("randomiser_spawn_characters",
		sf_randomiser_spawn_characters
	);

	wca_add_function("faction_set_relationship", 
		sf_faction_set_relationship
	);
}





/***************************************************************
 *                                                             *
 * Script functions                                            *
 *                                                             *
 * prefix sf_ (= script function)                              *
 *                                                             *
 ***************************************************************/



/*
 * function: random_number (lower_bound, upper_bound, return_value)
 *
 * returns a random number
 */
static WCA_C_FUNCTION(sf_random_number)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	WCA_NUMBER lb, ub;

	lb = wca_number(param[0]);
	ub = wca_number(param[1]);

	wca_store_number(param[0], random_int(lb, ub));
}



/*
 * function: generate_player_character ()
 */
static WCA_C_FUNCTION(sf_generate_player_character)
{
	
	generate_player_character();
}



/*
 * function: move_party_to_area (z, y, x, direction)
 */
static WCA_C_FUNCTION(sf_move_party_to_area)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	PLANET_POINT p;
	DIRECTION direction;
	
	p.z = wca_number(param[0]);
	p.y = wca_number(param[1]);
	p.x = wca_number(param[2]);
	direction = name_to_direction(wca_string(param[3]));

	move_party_to_area(&p, direction, true);
}




/*
 * function: dialogue_say (text, ...)
 */
static WCA_C_FUNCTION(sf_dialogue_say)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	WCA_PARAMETER_INDEX i;
	
	for (i = 0; param[i] != WCA_NIL; i++) {

		dialogue_say(wca_string(param[i]));
	}
}



/*
 * function: dialogue_add_option (option)
 */
static WCA_C_FUNCTION(sf_dialogue_add_option)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dialogue_add_option(wca_string(param[0]));
}



/*
 * function: dialogue_choice ($choice)
 */
static WCA_C_FUNCTION(sf_dialogue_choice)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_string(param[0], dialogue_choice());
}







/*
 * function: character_has_perk ($return_value, $character, perk)
 */
static WCA_C_FUNCTION(sf_character_has_perk)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;
	PERK perk;

	character = script_pointer(param[1]);

	perk = name_to_perk(wca_string(param[2]));

	if (character_has_perk(character, perk)) {

		wca_store_string(param[0], SCRIPT_TRUE);

	} else {

		wca_store_string(param[0], SCRIPT_FALSE);
	}
}






/*
 * function: character_power_level (character, return_value)
 *
 * returns the power level of the passed character
 */
static WCA_C_FUNCTION(sf_character_power_level)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	wca_store_number(param[1], character->power_level);
}



/*
 * function: character_set_faction (character, faction)
 *
 * sets the faction of the passed character
 */
static WCA_C_FUNCTION(sf_character_set_faction)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;
	FACTION faction;

	character = script_pointer(param[0]);
	faction = name_to_faction(wca_string(param[1]));

	character->faction = faction;
}



/*
 * function: party_power_level (return_value)
 *
 * returns the power level of the player character's party
 */
static WCA_C_FUNCTION(sf_party_power_level)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_number(param[0], party_power_level());
}



/*
 * function: party_n_members (return_value)
 *
 * returns the size of the player character's party
 */
static WCA_C_FUNCTION(sf_party_n_members)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_number(param[0], party_n_members());
}



/*
 * function: party_is_member (character, return_value)
 *
 * returns true if the passed character is a member of 
 * the player character's party
 */
static WCA_C_FUNCTION(sf_party_is_member)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	if (party_is_member(character)) {
	
		wca_store_string(param[1], SCRIPT_TRUE);
		
	} else {

		wca_store_string(param[1], SCRIPT_FALSE);
	}
}



/*
 * function: party_join (character)
 *
 * makes the passed character join the player character's party
 */
static WCA_C_FUNCTION(sf_party_join)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	party_join(character);
}



/*
 * function: party_leave (character)
 *
 * makes the passed character leave the player character's party
 */
static WCA_C_FUNCTION(sf_party_leave)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	party_leave(character);
}



/*
 * function: inventory value (character, return_value)
 *
 * returns the value of the passed character's inventory
 */
static WCA_C_FUNCTION(sf_inventory_value)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	wca_store_number(param[1],
		inventory_value(character)
	);
}



/*
 * function: personal_funds (character, return_value)
 *
 * returns the amount of money the passed character has
 */
static WCA_C_FUNCTION(sf_personal_funds)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	wca_store_number(param[1], inventory_money(character));
}



/*
 * function: add_money (character, amount)
 *
 * increases the passed character's personal funds by the passed amount
 */
static WCA_C_FUNCTION(sf_add_money)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	inventory_money_increase(character, wca_number(param[1]));
}


/*
 * function: subtract_money (character, amount)
 *
 * reduces the passed character's personal funds by the passed amount
 */
static WCA_C_FUNCTION(sf_subtract_money)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	inventory_money_reduce(character, wca_number(param[1]));
}





/*
 * function: give_object (character, object)
 *
 * gives the passed object to the passed character
 */
static WCA_C_FUNCTION(sf_give_object)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;
	OBJECT *object; 

	character = script_pointer(param[0]);

	object = object_create(wca_string(param[1]));

	if (param[2] != WCA_NIL) {
		object->charge = wca_number(param[2]);
	}		
	
	inventory_add(character, object);
}



/*
 * function: has_object (character, object, return_value)
 *
 * returns 'Yes' if the passed character has the passed object
 */
static WCA_C_FUNCTION(sf_has_object)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;
	const char *object_name;

	character = script_pointer(param[0]);
	
	object_name = wca_string(param[1]);

	if (inventory_find_object(character, object_name) != NULL) {
		
		wca_store_string(param[2], SCRIPT_TRUE);
		
	} else {

		wca_store_string(param[2], SCRIPT_FALSE);
	}
}



/*
 * function: remove_object (character, object)
 *
 * removes the passed object from the inventory of the passed character
 */
static WCA_C_FUNCTION(sf_remove_object)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;
	OBJECT *object;
	const char *object_name;

	character = script_pointer(param[0]);
	
	object_name = wca_string(param[1]);
	
	object = inventory_find_object(character, object_name);
	
	if (object != NULL) {

		inventory_remove(character, object);
	}
}



/*
 * function: shop_buy_mode ($character)
 */
static WCA_C_FUNCTION(sf_shop_buy_mode)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	shop_buy_mode(character);
}



/*
 * function: shop_sell_mode ($character)
 */
static WCA_C_FUNCTION(sf_shop_sell_mode)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	CHARACTER *character;

	character = script_pointer(param[0]);

	shop_sell_mode(character);
}




/*
 * function: shop_add (object_name)
 */
static WCA_C_FUNCTION(sf_shop_add)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	shop_add(wca_string(param[0]));
}



/*
 * function: shop_set_price_level (price_level)
 */
static WCA_C_FUNCTION(sf_shop_set_price_level)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	shop_set_price_level(wca_number(param[0]));
}



/*
 * function: quest_set_status (quest_name, quest_status)
 *
 * sets the status of the passed quest
 */
static WCA_C_FUNCTION(sf_quest_set_status)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	quest_set_status(
		wca_string(param[0]),
		wca_string(param[1])
	);
}



/*
 * function: quest_status (quest_name, return_value)
 *
 * returns the status of the passed quest
 */
static WCA_C_FUNCTION(sf_quest_status)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	const char *status;

	status = quest_status(wca_string(param[0]));

	wca_store_string(param[1], status);
}



/*
 * function: scenario_end (message)
 */
static WCA_C_FUNCTION(sf_scenario_end)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	scenario_end(wca_string(param[0]));
}



/*
 * function: dungeon_generate ()
 */
static WCA_C_FUNCTION(sf_dungeon_generate)
{

	dungeon_generate();
}



/*
 * function: dungeon_set_wall (wall)
 */
static WCA_C_FUNCTION(sf_dungeon_set_wall)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_wall(wca_string(param[0]));
}



/*
 * function: dungeon_set_floor (floor)
 */
static WCA_C_FUNCTION(sf_dungeon_set_floor)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_floor(wca_string(param[0]));
}



/*
 * function: dungeon_set_connections (way_up, way_down)
 */
static WCA_C_FUNCTION(sf_dungeon_set_connections)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_connections(
		wca_string(param[0]),
		wca_string(param[1])
	);
}



/*
 * function: dungeon_set_obstacles (obstacle, obstacle2)
 */
static WCA_C_FUNCTION(sf_dungeon_set_obstacles)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_obstacles(
		wca_string(param[0]),
		wca_string(param[1])
	);
}



/*
 * function: dungeon_set_pools (pool)
 */
static WCA_C_FUNCTION(sf_dungeon_set_pools)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_pools(wca_string(param[0]));
}



/*
 * function: dungeon_set_gate_level (gate_level)
 */
static WCA_C_FUNCTION(sf_dungeon_set_gate_level)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	dungeon_set_gate_level(wca_number(param[0]));
}



/*
 * function: dungeon_set_environment (terrain, object)
 */
static WCA_C_FUNCTION(sf_dungeon_set_environment)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	const char *terrain;
	const char *object = NULL;

	terrain = wca_string(param[0]);
	if (param[1] != WCA_NIL) object = wca_string(param[1]);

	dungeon_set_environment(terrain, object);
}



/*
 * function: cave_generate ()
 */
static WCA_C_FUNCTION(sf_cave_generate)
{

	cave_generate();
}


/*
 * function: cave_set_type (type_name)
 *
 * sets the type of the cave
 */
static WCA_C_FUNCTION(sf_cave_set_type)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	cave_set_type(wca_string(param[0]));
}



/*
 * function: cave_set_wall (wall)
 */
static WCA_C_FUNCTION(sf_cave_set_wall)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	cave_set_wall(wca_string(param[0]));
}



/*
 * function: cave_set_floor (floor)
 */
static WCA_C_FUNCTION(sf_cave_set_floor)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	cave_set_floor(wca_string(param[0]));
}



/*
 * function: cave_set_connections (way_up, way_down)
 */
static WCA_C_FUNCTION(sf_cave_set_connections)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	cave_set_connections(
		wca_string(param[0]),
		wca_string(param[1])
	);
}



/*
 * function: cave_set_gate_level (gate_level)
 */
static WCA_C_FUNCTION(sf_cave_set_gate_level)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	cave_set_gate_level(wca_number(param[0]));
}



/*
 * function: cave_set_environment (terrain, object)
 */
static WCA_C_FUNCTION(sf_cave_set_environment)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	DUNGEON_ENVIRONMENT environment;
	
	strcpy(environment.terrain_left, wca_string(param[0]));
	strcpy(environment.object_left, wca_string(param[1]));
	
	strcpy(environment.terrain_right, wca_string(param[2]));
	strcpy(environment.object_right, wca_string(param[3]));
	
	strcpy(environment.terrain_top, wca_string(param[4]));
	strcpy(environment.object_top, wca_string(param[5]));
	
	strcpy(environment.terrain_bottom, wca_string(param[6]));
	strcpy(environment.object_bottom, wca_string(param[7]));

	cave_set_environment(&environment);
}



/*
 * function: spawn_character (character, [n])
 *
 * spawns the passed character in the current area
 * the optional second parameter can be used to
 * spawn multiple copies of this character
 */
static WCA_C_FUNCTION(sf_spawn_character)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	const char *character;
	N_CHARACTERS n = 1;
	
	character = wca_string(param[0]);
	
	if (param[1] != WCA_NIL) {

		n = wca_number(param[1]);
	}
	
	for ( ; n > 0; --n) {

		spawn_character(character);
	}
}



/*
 * function: spawn_object (object)
 *
 * spawns the passed object in the current area
 */
static WCA_C_FUNCTION(sf_spawn_object)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	spawn_object(wca_string(param[0]));
}



/*
 * function: area_name (name)
 *
 * returns the name of the current area
 */
static WCA_C_FUNCTION(sf_area_name)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_string(param[0], active_area()->name);
}



/*
 * function: area_n_characters (n_characters)
 *
 * returns the number of characters in the current area
 */
static WCA_C_FUNCTION(sf_area_n_characters)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_number(param[0], area_n_characters());
}



/*
 * function: area_remove_gore_features ()
 *
 * removes all gore features from the current area
 */
static WCA_C_FUNCTION(sf_area_remove_gore_features)
{

	area_remove_gore_features();
}



/*
 * function: area_make_ruins (ruins_terrain)
 */
static WCA_C_FUNCTION(sf_area_make_ruins)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	area_make_ruins(wca_string(param[0]));
}



/*
 * function: area_add_random_gore ()
 *
 * adds random gore features to the current area
 */
static WCA_C_FUNCTION(sf_area_add_random_gore)
{
	
	area_add_random_gore();
}



/*
 * function: randomiser_add (item, randomiser_value)
 */
static WCA_C_FUNCTION(sf_randomiser_add)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	randomiser_add(wca_string(param[0]), wca_number(param[1]));
}



/*
 * function: randomiser ($item)
 */
static WCA_C_FUNCTION(sf_randomiser)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();

	wca_store_string(param[0], randomiser());
}



/*
 * function: randomiser_spawn_characters (n_characters)
 *
 * spawns (n_characters) on the current level based on the
 * randomiser list
 */
static WCA_C_FUNCTION(sf_randomiser_spawn_characters)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	N_CHARACTERS n;
	
	n = wca_number(param[0]);

	for ( ; n > 0; --n) spawn_character(randomiser());
}





/*
 * function: faction_set_relationship (faction1, faction2, relationship)
 *
 * sets the relationship between the passed factions
 */
static WCA_C_FUNCTION(sf_faction_set_relationship)
{
	const WCA_STORAGE_INDEX *param = wca_call_parameters();
	FACTION f1, f2;
	FACTION_RELATIONSHIP relationship;
	
	f1 = name_to_faction(wca_string(param[0]));
	f2 = name_to_faction(wca_string(param[1]));
	relationship = name_to_faction_relationship(wca_string(param[2]));
	
	faction_set_relationship(f1, f2, relationship);
}



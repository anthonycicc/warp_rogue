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
 * Module: Character Death
 */

#include "wrogue.h"




static void	character_death(CHARACTER *, CHARACTER *, DEATH_TYPE);
static void	player_character_death(CHARACTER *);
static void	execute_death_script(CHARACTER *);

static void	object_destruction(OBJECT *, const AREA_POINT *);

static void	splatter_remains(const CHARACTER *, DEATH_TYPE);

static void	death_drop(CHARACTER *);
static void	death_drop_all(CHARACTER *);

static void	give_ep_for_kill(CHARACTER *, const CHARACTER *);

static void	restart_from_save_point(void);



/*
 * gore level names
 */
static const char GoreLevelName[MAX_GORE_LEVELS][GORE_LEVEL_NAME_SIZE] = {

	"",
	"Blood",
	"Body part",
	"Plant parts",
	"Machine parts",
	"Poisoned corpse",
	"Corpse"

};





/*
 * checks the passed sector for destroyed characters and objects,
 * and calls the appropriate functions to handle the destructions
 * if there are any
 */
void handle_destruction(const AREA_POINT *location, CHARACTER *attacker,
	DEATH_TYPE death_type
)
{
	AREA_POINT p;
	CHARACTER *character;
	OBJECT *object;

	p = *location;

	character = character_at(&p);

	if (character != NULL && character_killed(character)) {

		character_death(character, attacker, death_type);
	}

	object = object_at(&p);

	if (object != NULL && object_destroyed(object)) {

		object_destruction(object, &p);
	}
}



/*
 * returns true if the passed character has been killed
 */
bool character_killed(const CHARACTER *character)
{

	if (character->injury > injury_max(character)) {

		return true;
	}

	return false;
}



/*
 * returns true if the passed object has been destroyed
 */
bool object_destroyed(const OBJECT *object)
{

	if (object->condition <= 0) {

		return true;
	}

	return false;
}



/*
 * returns the name of a gore level
 */
const char * gore_level_name(GORE_LEVEL gore_level)
{

	return GoreLevelName[gore_level];
}



/*
 * splatter
 */
void splatter(const AREA_POINT *location, GORE_LEVEL max_gore)
{
	SECTOR *sector = sector_at(location);
	OBJECT *object;
	TERRAIN *terrain;

	object = sector->object;

	/* the sector contains an object */
	if (object != NULL) {
		
		/* do not cover dangerous objects */
		if (object_has_attribute(object, OA_DANGEROUS)) return;

		/* environment object .. */
		if (object->type == OTYPE_ENVIRONMENT) {
			
			/* make it bloody if appropriate */
			if (max_gore == GORE_LEVEL_CORPSE ||
				max_gore == GORE_LEVEL_BODY_PART) {

				object->gore_level = GORE_LEVEL_BLOOD;
			}
			
			return;
		}
	
		/* impassable object - return */
		if (object_has_attribute(object, OA_IMPASSABLE)) return;
	}

	terrain = sector->terrain;

	/* do not cover impassable or dangerous terrain */
	if (terrain_has_attribute(terrain, TA_IMPASSABLE) ||
		terrain_has_attribute(terrain, TA_DANGEROUS)) {

		return;
	}
	
	/* wow, we can actually place a corpse/body parts! */
	switch (max_gore) {
	
	case GORE_LEVEL_BODY_PART:
		terrain->gore_level = GORE_LEVEL_BODY_PART;
		break;

	case GORE_LEVEL_CORPSE: 
		terrain->gore_level = GORE_LEVEL_CORPSE; 
		break;
	
	case GORE_LEVEL_POISONED_CORPSE:
		terrain->gore_level = GORE_LEVEL_POISONED_CORPSE;
		break;
	
	case GORE_LEVEL_MACHINE_PARTS:
		terrain->gore_level = GORE_LEVEL_MACHINE_PARTS;
		break;
	
	case GORE_LEVEL_PLANT_PARTS:
		terrain->gore_level = GORE_LEVEL_PLANT_PARTS;
		break;
	}
}



/*
 * splatter body parts
 */
void splatter_body_parts(const AREA_POINT *location)
{
	DIRECTION direction[MAX_REAL_DIRECTIONS];
	int i, dir_i;
	int n_body_parts;

	n_body_parts = random_int(
		SPLATTER_MIN_N_BODY_PARTS,
		SPLATTER_MAX_N_BODY_PARTS
	);

	randomised_directions(direction);

	for (i = dir_i = 0; i < n_body_parts; i++) {
		AREA_POINT target_point = *location;

		move_area_point(&target_point, direction[dir_i]);

		++dir_i;

		if (dir_i == MAX_REAL_DIRECTIONS) {

			dir_i = 0;
		}

		splatter(&target_point, GORE_LEVEL_BODY_PART);
	}
}



/*
 * handles character deaths
 */
static void character_death(CHARACTER *character, CHARACTER *killer,
	DEATH_TYPE death_type
)
{

	dynamic_message(MSG_DEATH, character, NULL, MOT_NIL);

	sector_at(&character->location)->character = NULL;

	if (death_type != DT_FALL) {

		death_drop(character);

		splatter_remains(character, death_type);
	}

	if (killer != NULL) {

		give_ep_for_kill(killer, character);

	/*
	 * hack - assumes that all deaths caused by poison are
	 * indirectly caused by the player
	 */
	} else if (death_type == DT_POISON) {

		give_ep_for_kill(player_character(), character);
	}

	if (character == player_character()) {
		
		if (game_difficulty() == DIFFICULTY_EASY) {

			restart_from_save_point();
			return;
		}

		player_character_death(character);
	}

	if (is_player_controlled_character(character)) {

		switch_character(CT_PC);
	}

	if (character->party == PARTY_PLAYER) {

		party_leave(character);
	}
	
	/* execute death script */	
	execute_death_script(character);

	if (!is_active_character(character)) {

		character_destroy(character);
	}
}



/*
 * handles the death of the player character
 */
static void player_character_death(CHARACTER *character)
{

	message(SID_GAME, "Game over. Press Enter.");
	render_game_screen();
	update_screen();

	while (get_key() != KEY_ENTER)
		/* DO NOTHING */;

	certificate_screen();

	character_destroy(character);

	saved_game_erase();

	program_shutdown();

}



/*
 * executes the death script of the killed character
 */
static void execute_death_script(CHARACTER *character)
{

	/* no script - return */
	if (!character_has_script(character)) return;

	/* set script pointer SELF */
	*script_pointer_self() = character;
	
	/* load character script */
	script_load(DIR_CHARACTER_SCRIPTS, character->script);
	
	/* set script data */
	script_set_data("KILLED", SCRIPT_TRUE);
	script_set_data("SELF", SCRIPT_SELF_PTR);

	/* execute script */
	script_execute();
}




/*
 * handles object destruction
 */
static void object_destruction(OBJECT *object,
	const AREA_POINT *object_location
)
{

	dynamic_message(MSG_DESTRUCTION, NULL, object, MOT_OBJECT);

	sector_at(object_location)->object = NULL;

	object_destroy(object);
}



/*
 * splatters the remains of the passed character 
 */
static void splatter_remains(const CHARACTER *character, 
	DEATH_TYPE death_type
)
{
	GORE_LEVEL max_gore = GORE_LEVEL_CORPSE;
	
	if (character_has_attribute(character, CA_MACHINE)) {
		
		max_gore = GORE_LEVEL_MACHINE_PARTS;
		
	} else if (character_has_attribute(character, CA_PLANT)) {
		
		max_gore = GORE_LEVEL_PLANT_PARTS;
		
	} else if (death_type == DT_POISON) {
	
		max_gore = GORE_LEVEL_POISONED_CORPSE;
	} 
	
	splatter(&character->location, max_gore);

	if (max_gore == GORE_LEVEL_CORPSE) {

		splatter_body_parts(&character->location);
	}	
}





/*
 * at death a character may drop objects
 * this function handles this
 */
static void death_drop(CHARACTER *character)
{

	if (character == player_character()) {

		return;
	}

	if (character_has_attribute(character, CA_UNIQUE)) {

		death_drop_all(character);

	} else {
		OBJECT *object;

		if (!random_choice(DEATH_DROP_PROBABILITY)) {

			return;
		}

		object = inventory_random_object(character);

		if (object == NULL ||
			object_has_attribute(object, OA_INHERENT)) {

			return;
		}

		action_drop_object(character, object, false);
	}
}



/*
 * drops all items
 */
static void death_drop_all(CHARACTER *character)
{
	LIST_NODE *node;
	OBJECT *object;

	for (node = character->inventory->head;
		node != NULL;) {

		object = (OBJECT *)node->data;
		node = node->next;

		if (object_has_attribute(object, OA_INHERENT)) {

			continue;
		}

		action_drop_object(character, object, false);
	}

}



/*
 * calculates EP reward for a kill and hands out the reward to
 * the killer's party
 */
static void give_ep_for_kill(CHARACTER *killer, 
	const CHARACTER *killed_character
)
{
	LIST_NODE *node;

	/* only the members of the player's party get EP */
	if (killer->party != PARTY_PLAYER) return;
	
	iterate_over_list(party_player(), node) {
		CHARACTER *character = (CHARACTER *)node->data;
		EXPERIENCE_POINTS ep_reward;
		POWER_DIFFERENCE power_difference;

		/* calculate power difference */
		power_difference = killed_character->power_level - 
			character->power_level;
	
		/* determine EP reward */
		switch (power_difference) {
		case 3: ep_reward = EP_REWARD_VERY_HARD; break;
		case 2: ep_reward = EP_REWARD_HARD; break;
		case 1: ep_reward = EP_REWARD_CHALLENGING; break;
		case 0: ep_reward = EP_REWARD_AVERAGE; break;
		case -1: ep_reward = EP_REWARD_ROUTINE; break;
		case -2: ep_reward = EP_REWARD_EASY; break;
		case -3: ep_reward = EP_REWARD_VERY_EASY; break;
		default: ep_reward = 0;
		}

		/* give reward */
		give_ep(character, ep_reward);
 	}
}



/*
 * restart from the last save point
 */
static void restart_from_save_point(void)
{

	message(SID_GAME,
		"Restart the game to continue from the last save point. "\
		"Press Enter."
	);

	render_game_screen();
	update_screen();

	while (get_key() != KEY_ENTER)
		/* DO NOTHING */;
	
	character_destroy(player_character());

	program_shutdown();
}



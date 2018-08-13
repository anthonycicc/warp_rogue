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
 * Module: Command
 */

#include "wrogue.h"



static void		handle_command(CHARACTER *);

static void		command_macro(CHARACTER *);

static void		command_wait(CHARACTER *);
static void		command_move(CHARACTER *);
static void		command_leave_area(CHARACTER *);
static void		command_game_controls_screen(CHARACTER *);
static void		command_inventory_screen(CHARACTER *);
static void		command_look(CHARACTER *);
static void		command_attack(CHARACTER *);
static void		command_shoot(CHARACTER *);
static void		command_shoot_abort(const CHARACTER *);
static void		command_strike(CHARACTER *);
static void		command_switch_character(CHARACTER *);
static CHARACTER_TYPE	command_to_character_type(COMMAND);
static void		command_trigger(CHARACTER *);
static void		command_use_elevator(CHARACTER *);
static void		command_use_way_up(CHARACTER *);
static void		command_use_way_down(CHARACTER *);
static void		command_pick_up(CHARACTER *, const AREA_POINT *);
static void		command_reload_weapon(CHARACTER *);
static void		command_unjam_weapon(CHARACTER *);
static void		command_switch_weapons(CHARACTER *);

static void		command_run(CHARACTER *);
static bool		command_run_find_path(CHARACTER *,
				const AREA_POINT *
			);

static void		ui_command_use_perk(CHARACTER *);
static void		command_evoke_psy_power(CHARACTER *);
static void		command_evoke_psy_power_abort(const CHARACTER *);
static void		command_character_screen(CHARACTER *);
static void		command_journal(CHARACTER *);

static void		command_help(CHARACTER *);
static void		command_stealth(CHARACTER *);
static void		command_disarm(CHARACTER *);

static void		command_jump_pack(CHARACTER *);
static const PATH_NODE *command_jump_pack_find_path(
				CHARACTER *, const AREA_POINT *
			);

static void		command_debug(CHARACTER *);

static bool		choose_firing_mode(CHARACTER *, FIRING_DATA *);
static void		ask_for_number_of_shots(CHARACTER *, FIRING_DATA *);
static void		ask_for_spread_factor(FIRING_DATA *);

static void 		evocation_inability_message(const CHARACTER *);
static bool 		choose_psy_power(PSY_POWER *, CHARACTER *);
static bool 		psy_power_choice_validation(CHARACTER *, PSY_POWER);

static void 		ui_command_tactical_command(CHARACTER *);




/*
 * command functions
 */
static void (* const CommandFunction[MAX_COMMANDS])(CHARACTER *) = {

	command_move,
	command_move,
	command_move,
	command_move,
	command_move,
	command_move,
	command_move,
	command_move,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,

	command_attack,
	command_trigger,
	command_inventory_screen,
	command_evoke_psy_power,
	ui_command_tactical_command,
	command_character_screen,
	command_look,
	command_reload_weapon,
	command_unjam_weapon,
	command_switch_weapons,
	command_run,
	ui_command_use_perk,

	command_journal,
	command_game_controls_screen,

	command_help,

	command_wait,

	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,
	command_macro,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	command_debug,
	NULL,
	NULL,

	command_switch_character,
	command_switch_character,
	command_switch_character,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};




/*
 * predefined target for the attack command
 */
static AREA_POINT	PredefinedAttackTarget = {AREA_COORD_NIL,
				AREA_COORD_NIL
			};



/*
 * the last command entered
 */
static COMMAND		EnteredCommand = CM_NIL;







/*
 * lets the player enter commands until his character has spent
 * his current action
 */
void player_control(CHARACTER *character)
{

	do {

		use_game_screen_command_bar();
		render_game_screen();
		update_screen();

		EnteredCommand = command_bar_get_command();

		message_clear(SID_GAME);

		handle_command(character);

		if (!is_player_controlled_character(character)) {

			break;
		}

	} while (!character->action_spent);
}



/*
 * handles the entered command
 */
static void handle_command(CHARACTER *character)
{
	void (* command)(CHARACTER *);

	command = CommandFunction[EnteredCommand];

	if (command == NULL) {

		message(SID_GAME, "Invalid command.");

		return;
	}

	(*command)(character);
}



/*
 * command: macro
 */
static void command_macro(CHARACTER *character)
{

	if (macro_defined(EnteredCommand)) {

		EnteredCommand = do_macro(EnteredCommand);

		handle_command(character);

	} else {

		message(SID_GAME, "Macro not defined.");
	}
}



/*
 * command: wait / recover
 */
static void command_wait(CHARACTER *character)
{

	if (character_must_recover(character)) {

		action_recover(character);

	} else {

		action_do_nothing(character);
	}
}



/*
 * command: move
 */
static void command_move(CHARACTER *character)
{
	DIRECTION direction;
	SECTOR *sector;
	AREA_POINT target_point;

	if (!move_command_to_direction(&direction, EnteredCommand)) {

		return;
	}

	target_point = character->location;

	move_area_point(&target_point, direction);

	if (out_of_area_bounds(&target_point)) {

		command_leave_area(character);

		return;
	}

	sector = sector_at(&target_point);

	if (sector->character != NULL &&
		hostility_between(character, sector->character) &&
		!character_unnoticed(sector->character)) {

		PredefinedAttackTarget = target_point;

		command_attack(character);

		return;
	}

	if (action_move(character, &target_point)) {

		return;
	}
}



/*
 * command: leave area
 */
static void command_leave_area(CHARACTER *character)
{
	CONFIRMATION_DIALOGUE dialogue;

	sprintf(dialogue.prompt, "Leave %s?", active_area()->name);

	leave_area_dialogue(&dialogue);

	if (!dialogue.confirmed) return;

	planet_map_screen(character);
}



/*
 * command: game controls screen
 */
static void command_game_controls_screen(CHARACTER *character)
{

	NOT_USED(character);

	game_controls_screen();
}



/*
 * command: reload weapon
 */
static void command_reload_weapon(CHARACTER *character)
{
	OBJECT *weapon = character->weapon;

	if (weapon == NULL) {

		message(SID_GAME, "No weapon to reload.");
		abort_macro();
		return;
	}

	if (weapon->reload == RELOAD_NO) {

		message(SID_GAME, "You cannot reload this weapon.");
		abort_macro();
		return;
	}

	if (object_has_attribute(weapon, OA_AUTOMATIC_RECHARGE)) {

		message(SID_GAME, "This weapon automatically recharges.");
		abort_macro();
		return;
	}

	if (weapon->charge == weapon->charge_max) {

		message(SID_GAME, "Weapon already fully loaded.");
		abort_macro();
		return;
	}

	action_reload_weapon(character);
}



/*
 * command: unjam weapon
 */
static void command_unjam_weapon(CHARACTER *character)
{
	const OBJECT *weapon = character->weapon;

	if (weapon == NULL) {

		message(SID_GAME, "No weapon to unjam.");
		abort_macro();
		return;

	} else if (!object_has_attribute(weapon, OA_JAMMED)) {

		message(SID_GAME, "Weapon isn't jammed.");
		abort_macro();
		return;
	}

	action_unjam_weapon(character);
}


/*
 * command: switch weapons
 */
static void command_switch_weapons(CHARACTER *character)
{

	action_switch_weapons(character);
}



/*
 * command: attack
 */
static void command_attack(CHARACTER *character)
{

	if (character_has_attribute(character, CA_BROKEN)) {

		message(SID_GAME, "Morale broken.");

		abort_macro();

		return;
	}

	if (character->weapon != NULL &&
		character->weapon->type == OTYPE_RANGED_COMBAT_WEAPON) {

		command_shoot(character);

		return;
	}

	command_strike(character);
}



/*
 * command: shoot
 */
static void command_shoot(CHARACTER *character)
{
	AREA_POINT target_point;
	CHARACTER *target;
	FIRING_DATA firing_data;
	OBJECT *weapon = character->weapon;

	if (involved_in_close_combat(character) &&
		(weapon->subtype == OSTYPE_BASIC ||
		weapon->subtype == OSTYPE_HEAVY)) {

		message(SID_GAME, "This weapon cannot be used in " \
			"close combat."
		);

		abort_macro();

		return;
	}

	if (object_has_attribute(weapon, OA_JAMMED)) {

		message(SID_GAME, "Weapon is jammed.");
		abort_macro();
		return;
	}

	if (weapon->charge <= 0) {

		message(SID_GAME, "Out of ammo.");

		abort_macro();

		return;
	}

	if (AREA_POINT_NOT_DEFINED(&PredefinedAttackTarget)) {

		if (!pick_target("Attack - Direction?", character,
			TMODE_SHOOT, 99, &target_point)) {

			command_shoot_abort(character);

			return;
		}

	} else {

		target_point = PredefinedAttackTarget;

		PredefinedAttackTarget = *area_point_nil();
	}

	if (!choose_firing_mode(character, &firing_data)) {

		command_shoot_abort(character);

		return;
	}

	target = character_at(&target_point);

	if (target != NULL && !hostility_check(target)) {

		command_shoot_abort(character);

		return;
	}

	if (firing_data.firing_mode != FMODE_A) {
		SECTOR *sector;

		sector = sector_at(&target_point);

		if (sector->character == NULL && sector->object == NULL) {

			message(SID_GAME, "No target.");

			command_shoot_abort(character);

			return;
		}
	}

	action_shoot(character, weapon, &firing_data, &target_point);

	update_view_character(character);
}



/*
 * command: shoot (abort)
 */
static void command_shoot_abort(const CHARACTER *character)
{

	abort_macro();

	update_view_character(character);
}



/*
 * command: strike
 */
static void command_strike(CHARACTER *character)
{
	AREA_POINT target_point;
	SECTOR *sector;

	if (character->weapon == 0 &&
		character_has_attribute(character, CA_NO_UNARMED_ATTACK)) {

		message(SID_GAME, "No unarmed attack.");
		abort_macro();
		return;
	}


	if (AREA_POINT_NOT_DEFINED(&PredefinedAttackTarget)) {

		if (!pick_target("Attack - Direction?", character,
			TMODE_STRIKE, 1, &target_point)) {

			abort_macro();
			return;
		}

	} else {

		target_point = PredefinedAttackTarget;

		PredefinedAttackTarget = *area_point_nil();
	}

	sector = sector_at(&target_point);

	if (sector->character != NULL &&
		!hostility_check(sector->character)) {

		return;
	}

	if (sector->character == NULL && sector->object == NULL) {

		message(SID_GAME, "No target.");
		abort_macro();
		return;
	}

	action_strike(character, character->weapon, &target_point);
}



/*
 * command: switch character
 */
static void command_switch_character(CHARACTER *character)
{
	CHARACTER_TYPE character_type;

	character_type = command_to_character_type(EnteredCommand);

	if (character_type == character->type) {

		return;
	}

	switch_character(character_type);
}



/*
 * command -> character type
 */
static CHARACTER_TYPE command_to_character_type(COMMAND command)
{
	CHARACTER_TYPE character_type;

	switch (command) {

	case CM_CHARACTER_1:
		character_type = CT_PC;
		break;

	case CM_CHARACTER_2:
		character_type = CT_COMPANION_1;
		break;

	case CM_CHARACTER_3:
		character_type = CT_COMPANION_2;
		break;

	default:
		character_type = CT_NIL;
	}

	return character_type;
}



/*
 * command: trigger
 */
static void command_trigger(CHARACTER *character)
{
	AREA_POINT target_point;
	bool current_sector_triggered = false;
	SECTOR *sector;

	if (!pick_target("Trigger - Direction?", character,
		TMODE_TRIGGER, 1, &target_point)) {

		return;
	}

	if (area_points_equal(&character->location, &target_point)) {

		current_sector_triggered = true;
	}

	sector = sector_at(&target_point);

	/* the player has triggered another character */
	if (sector->character != NULL && !current_sector_triggered) {
		CHARACTER *target;

		target = character_at(&target_point);

		if (character_has_attribute(target, CA_STUNNED)) {

			message(SID_GAME,
				"You cannot talk to stunned characters."
			);

			abort_macro();
			return;
		}

		dialogue_start(character, target);
		return;
	}

	if (sector->object == NULL) {

		message(SID_GAME, "Nothing to trigger.");

		abort_macro();

		return;

	} else {
		const OBJECT *object = sector->object;

		if (object->subtype == OSTYPE_ELEVATOR) {

			command_use_elevator(character);

			return;

		} else if (object->subtype == OSTYPE_WAY_UP) {

			command_use_way_up(character);

			return;

		} else if (object->subtype == OSTYPE_WAY_DOWN) {

			command_use_way_down(character);

			return;
		}

		if (object->type != OTYPE_ENVIRONMENT) {

			command_pick_up(character, &target_point);

			return;
		}
	}
}



/*
 * command: use elevator
 */
static void command_use_elevator(CHARACTER *character)
{
	GET_TEXT_DIALOGUE elevator_dialogue;
	PLANET_POINT target_area;
	PLANET_COORD level_min, level_max, chosen_level;

	NOT_USED(character);

	target_area = active_area()->location;

	level_min = 0;
	level_max = planet_tile(&target_area)->n_levels - 1;

	sprintf(elevator_dialogue.prompt,
		"Use elevator - Choose level (%d - %d): ",
       		level_min, level_max
	);

	elevator_dialogue.max_length = 2;
	elevator_dialogue.colour = C_Text;

	command_bar_set(0, "");

	render_use_elevator_screen();

	update_screen();

	use_elevator_dialogue(&elevator_dialogue);

	if (is_empty_string(elevator_dialogue.entered_text)) {

		return;
	}

	chosen_level = atoi(elevator_dialogue.entered_text);

	if (chosen_level < level_min || chosen_level > level_max ||
		chosen_level == target_area.z) {

		return;
	}

	target_area.z = chosen_level;

	move_party_to_area(&target_area, DIRECTION_ELEVATOR, false);

	/* hack */
	update_perception_data(character);
}



/*
 * command: use way up
 */
static void command_use_way_up(CHARACTER *character)
{
	PLANET_POINT target_area;

	NOT_USED(character);

	target_area = active_area()->location;
	target_area.z += 1;

	move_party_to_area(&target_area, DIRECTION_WAY_UP, false);

	/* hack */
	update_perception_data(character);
}



/*
 * command: use way down
 */
static void command_use_way_down(CHARACTER *character)
{
	PLANET_POINT target_area;

	NOT_USED(character);

	target_area = active_area()->location;
	target_area.z -= 1;

	move_party_to_area(&target_area, DIRECTION_WAY_DOWN, false);

	/* hack */
	update_perception_data(character);
}



/*
 * command: inventory screen
 */
static void command_inventory_screen(CHARACTER *character)
{

	inventory_screen(character);
}



/*
 * command: look
 */
static void command_look(CHARACTER *character)
{

	view_screen(character, "Look", C_Cursor);
}



/*
 * command: pick up
 */
static void command_pick_up(CHARACTER *character, const AREA_POINT *target)
{
	OBJECT *object = object_at(target);

	if (object == NULL) {

		message(SID_GAME, "Nothing to pick up.");

		abort_macro();

		return;
	}

	if (inventory_is_full(character)) {

		message(SID_GAME, "Inventory is full.");

		abort_macro();

		return;
	}

	if (object->weight + inventory_weight(character) >
		ENCUMBRANCE_MAX(character)) {

		message(SID_GAME, "You cannot carry any more.");

		abort_macro();

		return;
	}

	action_pick_up_object(character, target);
}



/*
 * command: run
 */
static void command_run(CHARACTER *character)
{
	AREA_POINT target_point;

	if (!pick_target("Run - Direction?", character, TMODE_RUN,
		run_factor(character), &target_point)) {

		return;
	}

	if (!sector_is_class(&target_point, SC_MOVE_TARGET)) {

		message(SID_GAME, "Invalid target.");
		return;
	}

	if (command_run_find_path(character, &target_point)) {

		action_run(character, pathfinder_path(),
			pathfinder_path_length()
		);
	}
}



/*
 * command: run (find path)
 */
static bool command_run_find_path(CHARACTER *character,
	const AREA_POINT *target_point
)
{
	PATHFINDER_CONFIG config;

	config.subject = character;
	config.start = &character->location;
	config.target = target_point;
	config.ignore_dangerous = true;
	config.ignore_destructable = false;
	config.ignore_characters = false;

	pathfinder_config(&config);

	if (pathfinder_find_path() == NULL) {
		message(SID_GAME, "No path.");
		return false;
	}

	if (pathfinder_path_length() > run_factor(character)) {
		message(SID_GAME, "Out of range.");
		return false;
	}

	return true;
}



/*
 * UI command: use perk
 */
static void ui_command_use_perk(CHARACTER *character)
{
	/* get list of useable perks */
	LIST *useable_perks = useable_perks_list(character);
	LIST_NODE_INDEX choice;
	KEY_CODE pressed_key;
	COMMAND entered_command;
	bool show_list = false;
	PERK perk;

	/* no useable perks .. */
	if (useable_perks == NULL) {

		/* output error message / abort macro */
		message(SID_GAME, "No useable perks.");
		abort_macro();
		return;
	}

	/* setup command bar */
	command_bar_set(2, CM_LIST, CM_EXIT);

	do {

		/* no prompts during macro execution */
		if (!macro_is_being_executed()) {
			render_game_screen();
			render_overlay_screen_prompt("Use perk - Which?");

			/* list should be shown .. */
			if (show_list) {
				/* show list */
				render_select_list(useable_perks);
			}

			update_screen();
		}

		/* let the user press a key */
		pressed_key = get_key();

		/* attempt to translate the key to a command */
		entered_command = command_bar_command(pressed_key);

		/* list command .. */
		if (entered_command == CM_LIST) {

			/* show list */
			show_list = true;
			continue;
		}

		/* attempt to translate the key to a select list choice */
		choice = select_list_choice(useable_perks, pressed_key);

		/* the user has chosen an item from the list .. */
		if (choice != LIST_NODE_INDEX_NIL) break;

	/* continue until the user enters the Exit command */
	} while (entered_command != CM_EXIT);

	/* perk chosen .. */
	if (entered_command != CM_EXIT) {

		/* get chosen perk */
		perk = name_to_perk(list_data_at(useable_perks, choice));

		/* execute appropriate command */
		if (perk == PK_STEALTH) {

			command_stealth(character);

		} else if (perk == PK_JUMP_PACK) {

			command_jump_pack(character);

		} else if (perk == PK_DISARM) {

			command_disarm(character);
		}
	}

	/* free perk list */
	list_free(useable_perks);
}







/*
 * command: evoke psychic power
 */
static void command_evoke_psy_power(CHARACTER *character)
{
	char prompt[STRING_BUFFER_SIZE];
	AREA_POINT target_point;
	PSY_POWER power;

	power = PSY_NIL;

	if (!is_able_to_evoke_psy_powers(character)) {

		abort_macro();

		evocation_inability_message(character);

		return;
	}

	if (!choose_psy_power(&power, character)) {

		abort_macro();

		return;
	}

	if (!psy_power_choice_validation(character, power)) {

		abort_macro();

		return;
	}

	sprintf(prompt, "%s - Direction?", psy_power_name(power));

	if (!psy_power_requires_target(power)) {

		action_evoke_psy_power(character, power, area_point_nil());

		update_view_character(character);

		return;
	};

	if (!pick_target(prompt, character, TMODE_PSY_POWER, 99,
			&target_point)) {

		command_evoke_psy_power_abort(character);

		return;
	}

	if (!psy_power_valid_target(power, &target_point)) {

		message(SID_GAME, "Invalid target.");

		command_evoke_psy_power_abort(character);

		return;
	}

	if (psy_power_in_effect(power, &target_point)) {

		message(SID_GAME, "Already in effect.");

		command_evoke_psy_power_abort(character);

		return;
	}

	if (psy_power_is_hostile(power)) {
		CHARACTER *target;

		target = character_at(&target_point);

		if (target != NULL && !hostility_check(target)) {

			command_evoke_psy_power_abort(character);

			return;
		}
	}

	action_evoke_psy_power(character, power, &target_point);

	update_view_character(character);
}



/*
 * command: evoke psychic power (abort)
 */
static void command_evoke_psy_power_abort(const CHARACTER *character)
{

	abort_macro();

	update_view_character(character);
}



/*
 * command: character screen
 */
static void command_character_screen(CHARACTER *character)
{

	character_screen(character);
}



/*
 * command: journal
 */
static void command_journal(CHARACTER *character)
{
	message(SID_GAME, "Journal not implemented yet.");

	NOT_USED(character);
}



/*
 * command: help
 */
static void command_help(CHARACTER *character)
{

	NOT_USED(character);

	help_screen();
}



/*
 * command: stealth
 */
static void command_stealth(CHARACTER *character)
{


	action_stealth(character);
}



/*
 * command: disarm
 */
static void command_disarm(CHARACTER *character)
{
	AREA_POINT target_point;
	SECTOR *sector;

	if (character_has_attribute(character, CA_BROKEN)) {

		message(SID_GAME, "Morale broken.");
		abort_macro();
		return;
	}

	if (!pick_target("Disarm - Direction?", character, TMODE_DISARM, 1,
		&target_point)) {

		abort_macro();
		return;
	}

	sector = sector_at(&target_point);

	if (sector->character == NULL) {

		message(SID_GAME, "No target.");
		abort_macro();
		return;
	}

	if (sector->character->weapon == NULL) {

		message(SID_GAME, "The target is not armed.");
		abort_macro();
		return;
	}


	if (object_has_attribute(sector->character->weapon,
		OA_INHERENT)) {

		message(SID_GAME, "The weapon is inherent.");
		abort_macro();
		return;
	}

	if (!hostility_check(sector->character)) {

		return;
	}

	action_disarm(character, sector->character);
}



/*
 * command: jump pack
 */
static void command_jump_pack(CHARACTER *character)
{
	AREA_POINT target_point;
	AREA_DISTANCE n_steps;
	const PATH_NODE *path;

	if (character->jump_pack == NULL) {

		message(SID_GAME, "No jump pack equipped.");
		return;
	}

	if (!pick_target("Jump - Direction?", character, TMODE_JUMP,
		MAX_JUMP_PACK_RANGE, &target_point)) {

		return;
	}

	if (!sector_is_class(&target_point, SC_MOVE_TARGET)) {

		message(SID_GAME, "Invalid target.");
		return;
	}

	path = command_jump_pack_find_path(character, &target_point);
	if (path == NULL) {
		message(SID_GAME, "No path.");
		return;
	}

	n_steps = pathfinder_path_length();

	if (n_steps > MAX_JUMP_PACK_RANGE) {

		message(SID_GAME, "Out of range.");
		return ;
	}

	action_jump(character, &target_point);
}




/*
 * command: jump pack (pathfinding)
 */
static const PATH_NODE * command_jump_pack_find_path(
	CHARACTER *character, const AREA_POINT *target
)
{
	PATHFINDER_CONFIG config;

	config.subject = character;
	config.start = &character->location;
	config.target = target;
	config.ignore_characters = true;
	config.ignore_dangerous = true;
	config.ignore_destructable = false;

	pathfinder_config(&config);

	return pathfinder_find_path();
}



/*
 * command: debug
 */
static void command_debug(CHARACTER *character)
{

	NOT_USED(character);

	debug_console();
}



/*
 * lets the player choose a firing mode
 */
static bool choose_firing_mode(CHARACTER *character, FIRING_DATA *fire)
{
	const OBJECT *weapon;
	const bool *has_firing_mode;

	weapon = character->weapon;

	has_firing_mode = weapon->firing_mode.has;

	if (has_firing_mode[FMODE_S] && !has_firing_mode[FMODE_SA] &&
		!has_firing_mode[FMODE_A]) {

		fire->firing_mode = FMODE_S;

	} else if (!has_firing_mode[FMODE_S] && has_firing_mode[FMODE_SA] &&
		!has_firing_mode[FMODE_A]) {

		fire->firing_mode = FMODE_SA;

	} else if (!has_firing_mode[FMODE_S] && !has_firing_mode[FMODE_SA] &&
		has_firing_mode[FMODE_A]) {

		fire->firing_mode = FMODE_A;

	} else {
		COMMAND entered_command;

		command_bar_clear();

		if (has_firing_mode[FMODE_S]) {

			command_bar_add(1, CM_S);
		}

		if (has_firing_mode[FMODE_SA]) {

			command_bar_add(1, CM_SA);
		}

		if (has_firing_mode[FMODE_A]) {

		        command_bar_add(1, CM_A);
		}

		command_bar_add(1, CM_EXIT);

		if (!macro_is_being_executed()) {

			render_choose_firing_mode_screen();

			update_screen();
		}

		entered_command = command_bar_get_command();

		if (entered_command == CM_S) {

			fire->firing_mode = FMODE_S;

		} else if (entered_command == CM_SA) {

			fire->firing_mode = FMODE_SA;

		} else if (entered_command == CM_A) {

			fire->firing_mode = FMODE_A;

		} else if (entered_command == CM_EXIT) {

			return false;
		}


	}

	if (fire->firing_mode == FMODE_S) {

		fire->n_shots = 1;

	} else if (fire->firing_mode == FMODE_SA) {

		ask_for_number_of_shots(character, fire);

	} else if (fire->firing_mode == FMODE_A) {

		fire->n_shots = weapon->firing_mode.a_shots;

		ask_for_spread_factor(fire);
	}

	return true;
}


/*
 * asks the player how many shots he wants to fire
 */
static void ask_for_number_of_shots(CHARACTER *character, FIRING_DATA *fire)
{
	const FIRING_MODE_DATA *firing_mode;

	firing_mode = &character->weapon->firing_mode;

	if (firing_mode->min_sa_shots == firing_mode->max_sa_shots) {

		fire->n_shots = firing_mode->max_sa_shots;
		return;
	}

	if (!macro_is_being_executed()) {
		command_bar_clear();
		render_number_of_shots_screen(character);
		update_screen();
	}

	do {

		fire->n_shots = ascii_to_integer(get_key());

	} while (fire->n_shots < firing_mode->min_sa_shots ||
		fire->n_shots > firing_mode->max_sa_shots
	);

}



/*
 * asks the player to enter the spread factor
 */
static void ask_for_spread_factor(FIRING_DATA *fire)
{

	if (!macro_is_being_executed()) {
		command_bar_clear();
		render_spread_factor_screen();
		update_screen();
	}

	do {

		fire->spread = ascii_to_integer(get_key());

	} while (fire->spread < A_MODE_SPREAD_MIN ||
		fire->spread > A_MODE_SPREAD_MAX);


}



/*
 * evocation inability message
 */
static void evocation_inability_message(const CHARACTER *character)
{

	if (character_has_attribute(character, CA_BROKEN)) {

		message(SID_GAME,
			"Morale broken."
		);

		return;
	}

	if (!character_has_perk(character, PK_PSYCHIC_I)) {

		message(SID_GAME,
			"You need the %s perk to evoke psychic powers.",
			perk_name(PK_PSYCHIC_I)
		);

		return;
	}
}



/*
 * lets the player choose a psychic power
 */
static bool choose_psy_power(PSY_POWER *power, CHARACTER *character)
{
	LIST *power_list = character_psy_powers(character);
	LIST_NODE_INDEX choice;
	KEY_CODE pressed_key;
	COMMAND entered_command;
	bool show_list = false;

	/* the character has no psychic powers .. */
	if (power_list == NULL) {

		/* output error message / abort macro */
		message(SID_GAME, "No psychic powers.");
		abort_macro();
		return false;
	}

	/* setup command bar */
	command_bar_set(2, CM_LIST, CM_EXIT);

	do {

		/* no prompts during macro execution */
		if (!macro_is_being_executed()) {
			render_game_screen();
			render_overlay_screen_prompt(
				"Evoke psychic power - Which?"
			);

			/* list should be shown .. */
			if (show_list) {
				/* show list */
				render_select_list(power_list);
			}

			update_screen();
		}

		/* let the user press a key */
		pressed_key = get_key();

		/* attempt to translate the key to a command */
		entered_command = command_bar_command(pressed_key);

		/* list command .. */
		if (entered_command == CM_LIST) {

			/* show list */
			show_list = true;
			continue;
		}

		/* attempt to translate the key to a select list choice */
		choice = select_list_choice(power_list, pressed_key);

		/* the user has chosen an item from the list .. */
		if (choice != LIST_NODE_INDEX_NIL) break;

	/* continue until the user enters the Exit command */
	} while (entered_command != CM_EXIT);

	/* psy power chosen .. */
	if (entered_command != CM_EXIT) {

		/* get chosen power */
		*power = name_to_psy_power(
			list_data_at(power_list, choice)
		);

	/* no psy power chosen */
	} else {

		/* return PSY_NIL */
		*power = PSY_NIL;
	}

	/* free power list */
	list_free(power_list);

	/* return true if a power was chosen, false otherwise */
	if (*power == PSY_NIL) return false;
	return true;
}



/*
 * returns true if the chosen power is a valid choice
 */
static bool psy_power_choice_validation(CHARACTER *character, PSY_POWER power)
{

	if (!character_has_psy_power(character, power)) {

		message(SID_GAME,
			"You have not mastered this power."
		);
		return false;

	}

	return true;
}



/*
 * lets the player choose a tactical command
 */
static void ui_command_tactical_command(CHARACTER *character)
{
	LIST *list = tactics_list();
	LIST_NODE_INDEX choice;
	KEY_CODE pressed_key;
	COMMAND entered_command;
	bool show_list = false;
	AI_TACTIC tactic;

	NOT_USED(character);

	/* setup command bar */
	command_bar_set(2, CM_LIST, CM_EXIT);

	do {

		/* no prompts during macro execution */
		if (!macro_is_being_executed()) {
			render_game_screen();
			render_overlay_screen_prompt(
				"Tactical command - Which?"
			);

			/* list should be shown .. */
			if (show_list) {
				/* show list */
				render_select_list(list);
			}

			update_screen();
		}

		/* let the user press a key */
		pressed_key = get_key();

		/* attempt to translate the key to a command */
		entered_command = command_bar_command(pressed_key);

		/* list command .. */
		if (entered_command == CM_LIST) {

			/* show list */
			show_list = true;
			continue;
		}

		/* attempt to translate the key to a select list choice */
		choice = select_list_choice(list, pressed_key);

		/* the user has chosen an item from the list .. */
		if (choice != LIST_NODE_INDEX_NIL) break;

	/* continue until the user enters the Exit command */
	} while (entered_command != CM_EXIT);

	/* tactical command chosen .. */
	if (entered_command != CM_EXIT) {

		/* get chosen tactic */
		tactic = name_to_tactic(list_data_at(list, choice));

		/* output message */
		message(SID_GAME, ai_tactic_name(tactic));

		/* set tactic */
		party_set_tactic(tactic);
	}

	/* free tactical commands list */
	list_free(list);
}








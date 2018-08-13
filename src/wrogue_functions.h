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
 *
 * Warp Rogue Functions
 *
 */



/*
 * ai.c
 */
void		ai_control(CHARACTER *);

void		ai_fix_self(AI_CONTROL_DATA *);

void		ai_use_drugs_to_fix_self(AI_CONTROL_DATA *);

void		ai_use_stealth(AI_CONTROL_DATA *);

void		ai_get_ready(AI_CONTROL_DATA *);

bool		ai_is_close_to_pcc(AI_CONTROL_DATA *);



/*
 * ai_combat.c
 */
void			ai_attack(AI_CONTROL_DATA *);

void			ai_reach_attack_distance(AI_CONTROL_DATA *);



/*
 * ai_combat_options.c
 */

AI_OPTION		ai_choose_option(AI_CONTROL_DATA *);

void			ai_execute_option(AI_CONTROL_DATA *, AI_OPTION);






/*
 * ai_equipment.c
 */
void			ai_fix_weapon(AI_CONTROL_DATA *, bool);

void			ai_use_weapon(AI_CONTROL_DATA *);

AI_OPTION_WEIGHT	ai_weapon_attack_weight(AI_CONTROL_DATA *,
				const OBJECT *
			);




/*
 * ai_movement.c
 */
const PATH_NODE *	ai_find_path(AI_CONTROL_DATA *);

bool			ai_run_towards_target(AI_CONTROL_DATA *);

void			ai_flee(AREA_POINT *, const AREA_POINT *);



/*
 * ai_psychic.c
 */
PSY_POWER		ai_choose_psy_power(AI_CONTROL_DATA *);

bool			ai_power_is_sensible_choice(PSY_POWER,
				const AREA_POINT *
			);



/*
 * ai_states.c
 */
void		ai_state_execute(AI_CONTROL_DATA *);

void		ai_set_state(AI_CONTROL_DATA *, AI_STATE);
void		ai_set_default_state(AI_CONTROL_DATA *, AI_STATE);



/*
 * ai_tactics.c
 */
#define iterate_over_tactics(i) \
	for ((i) = TACTIC_LB; (i) < MAX_AI_TACTICS; (i)++)

void		ai_set_tactic(AI_CONTROL_DATA *, AI_TACTIC);

const char *	ai_tactic_name(AI_TACTIC);

LIST *		tactics_list(void);

AI_TACTIC	name_to_tactic(const char *);




/*
 * ai_target.c
 */
void		ai_set_target(AI_CONTROL_DATA *, const AREA_POINT *);
bool		ai_has_target(AI_CONTROL_DATA *);
void		ai_erase_target(AI_CONTROL_DATA *);



/*
 * area.c
 */
void			area_init(void);
void			area_clean_up(void);

void			area_clear(void);

AREA *			active_area(void);

LIST *			area_character_list(void);

LIST *			area_event_list(void);

N_CHARACTERS		area_n_characters(void);

bool			area_points_equal(const AREA_POINT *, const AREA_POINT *);

void			move_area_point(AREA_POINT *, DIRECTION);

AREA_DISTANCE		area_distance(const AREA_POINT *, const AREA_POINT *);

const AREA_SECTION *	area_bounds(void);

bool			out_of_area_bounds(const AREA_POINT *);

const AREA_POINT *	area_point_nil(void);

void			place_terrain(TERRAIN *, const AREA_POINT *);
void			remove_terrain(TERRAIN *, LIST *);

void			place_object(OBJECT *, const AREA_POINT *);
void			remove_object(OBJECT *, LIST *);

void			place_character(CHARACTER *, const AREA_POINT *);
void			remove_character(CHARACTER *, LIST *);

SECTOR *		sector_at(const AREA_POINT *);
TERRAIN *		terrain_at(const AREA_POINT *);
OBJECT *		object_at(const AREA_POINT *);
CHARACTER *		character_at(const AREA_POINT *);

void			area_remove_gore_features(void);
void			area_make_ruins(const char *);
void			area_add_random_gore(void);



/*
 * area_cave.c
 */
void		cave_generate(void);

void		cave_reset(void);

void		cave_set_type(const char *);
void		cave_set_wall(const char *);
void		cave_set_floor(const char *);
void		cave_set_connections(const char *, const char *);
void		cave_set_gate_level(PLANET_COORD);
void		cave_set_environment(const DUNGEON_ENVIRONMENT *);
void		cave_set_random_gore(bool);


/*
 * area_dungeon.c
 */
void		dungeon_generate(void);

void		dungeon_reset(void);

void		dungeon_set_wall(const char *);
void		dungeon_set_floor(const char *);
void		dungeon_set_connections(const char *, const char *);
void		dungeon_set_obstacles(const char *, const char *);
void		dungeon_set_pools(const char *);
void		dungeon_set_gate_level(PLANET_COORD);
void		dungeon_set_environment(const char *, const char *);



/*
 * area_generation.c
 */
void		add_connections(const AREA_SECTION *, const char *,
			const char *
		);

void		add_environment(const char *, const char *);

void		build_gate(AREA_POINT *, AREA_COORD, AREA_COORD);

bool		spawn_character(const char *);
bool		spawn_object(const char *);



/*
 * area_sector.c
 */
void			sector_set_bounds(const AREA_SECTION *);

void			sector_reset_bounds(void);

char *			sector_description(char *, const SECTOR *);

bool			sector_is_class(const AREA_POINT *, SECTOR_CLASS);

const AREA_POINT *	adjacent_sector(const AREA_POINT *, SECTOR_CLASS);

const AREA_POINT *	random_sector(SECTOR_CLASS);



/*
 * area_transition.c
 */
void			move_party_to_area(const PLANET_POINT *, DIRECTION, bool);



/*
 * character.c
 */

#define is_player_controlled_character(c)	((c)->controller == CC_PLAYER)

void			character_init(void);
void			character_clean_up(void);

CHARACTER *		character_box_new_character(void);
void			character_box_optimise(void);
N_CHARACTERS		character_box_size(void);

void *			character_new(void);
void			character_free(void *);

CHARACTER *		character_create(const char *);
CHARACTER *		character_create_i(CHARACTER_INDEX);

void			character_destroy(void *);

CHARACTER *		character_clone(const CHARACTER *);

void			set_player_character(CHARACTER *);
CHARACTER *		player_character(void);

void			set_player_controlled_character(CHARACTER *);
CHARACTER *		player_controlled_character(void);

const char *		character_description(const CHARACTER *);

void			character_screen(CHARACTER *);

void			name_character(CHARACTER *);

void			character_recover(CHARACTER *);
void			character_recover_injuries(CHARACTER *);
bool			character_must_recover(const CHARACTER *);

INJURY			injury_max(const CHARACTER *);

bool			has_ranged_attack(const CHARACTER *);




/*
 * character_actions.c
 */
void		action_do_nothing(CHARACTER *);

void		action_recover(CHARACTER *);

bool		action_move(CHARACTER *, const AREA_POINT *);

void		action_push_past(CHARACTER *, CHARACTER *);

bool		action_run(CHARACTER *, const PATH_NODE *, AREA_DISTANCE);

void		action_jump(CHARACTER *, const AREA_POINT *);

void		action_strike(CHARACTER *, OBJECT *, const AREA_POINT *);

void		action_shoot(CHARACTER *, OBJECT *, FIRING_DATA *,
			const AREA_POINT *
		);

void		action_reload_weapon(CHARACTER *);

void		action_partial_reload(CHARACTER *);

void		action_unjam_weapon(CHARACTER *);

void		action_switch_weapons(CHARACTER *);

void		action_disarm(CHARACTER *, CHARACTER *);

bool		action_evoke_psy_power(CHARACTER *, PSY_POWER,
			const AREA_POINT *
		);

void		action_pick_up_object(CHARACTER *, const AREA_POINT *);

bool		action_equip_object(CHARACTER *, OBJECT *, OBJECT **);

bool		action_unequip_object(CHARACTER *, OBJECT *);

void		action_drop_object(CHARACTER *, OBJECT *, bool);

bool		action_use_drug(CHARACTER *, OBJECT *);

bool		action_stealth(CHARACTER *);



/*
 * character_advancement.c
 */
void		give_ep(CHARACTER *, EXPERIENCE_POINTS);

void		character_advancement_screen(CHARACTER *);

void		new_career_path(CHARACTER *, bool);

void		increase_stat(CHARACTER *, bool);

bool		new_perk(CHARACTER *);

bool		learn_psy_power(CHARACTER *, bool);



/*
 * character_attributes.c
 */
#define character_set_attribute(c,a) \
	((c)->attribute[(a)] = true)

#define character_has_attribute(c,a) \
	((c)->attribute[(a)])

#define character_remove_attribute(c,a) \
	((c)->attribute[(a)] = false)

char *		character_attribute_description(char *, CHARACTER_ATTRIBUTE);

void		character_attribute_screen(CHARACTER_ATTRIBUTE);

const char *	character_attribute_name(CHARACTER_ATTRIBUTE);
COLOUR		character_attribute_colour(CHARACTER_ATTRIBUTE);
void		character_attribute_set_colour(CHARACTER_ATTRIBUTE, COLOUR);

CHARACTER_ATTRIBUTE	name_to_character_attribute(const char *);



/*
 * character_career.c
 */

#define career_has_perk(c,p)		((c)->perk[(p)].available)
#define career_perk_required(c,p)	((c)->perk[(p)].required)

void		career_init(void);
void		career_clean_up(void);

CAREER *	career_box_new_career(void);
void		career_box_optimise(void);

void		career_set(CHARACTER *, CAREER_INDEX);

void		career_set_perk(CAREER *, PERK, bool);

CAREER *	get_career_pointer(CAREER_INDEX);

N_CAREERS	get_n_careers(void);

bool		career_completed(const CHARACTER *);

N_PERKS		career_n_required_perks(const CAREER *);

void		career_screen(const CAREER *);

void		career_give_trappings(CHARACTER *);

const char *	career_type_name(CAREER_TYPE);

CAREER_INDEX	name_to_career_index(const char *);
CAREER_TYPE	name_to_career_type(const char *);


#if defined(DEBUG)

void		career_box_validate(void);

#endif



/*
 * character_death.c
 */
void		handle_destruction(const AREA_POINT *, CHARACTER *,
			DEATH_TYPE
		);

bool		character_killed(const CHARACTER *);

bool		object_destroyed(const OBJECT *);

void		place_random_gore_feature(SECTOR *);

const char *	gore_level_name(GORE_LEVEL);

void		splatter(const AREA_POINT *, GORE_LEVEL);

void		splatter_body_parts(const AREA_POINT *);







/*
 * character_equipment.c
 */
void			use_equipment_object(CHARACTER *, OBJECT *);

bool			object_in_use(const CHARACTER *, const OBJECT *);

bool			noisy_object_in_use(const CHARACTER *);

void			reload_weapon(EVENT *);

void			partial_reload(CHARACTER *);

void			recharge_weapon(EVENT *);

bool			weapon_has_jammed(ATTACK_DATA *);

bool			laser_weapon_malfunction(ATTACK_DATA *);

bool			plasma_weapon_malfunction(ATTACK_DATA *);

void			unjam_weapon(EVENT *);

void			switch_weapons(CHARACTER *);

bool			equip_object(CHARACTER *, OBJECT *, OBJECT **);
bool			unequip_object(CHARACTER *, OBJECT *);

void			equip_objects(CHARACTER *);

bool			is_knife(const OBJECT *);

bool			can_be_reloaded(const OBJECT *);

const char *		firing_mode_name(FIRING_MODE);

FIRING_MODE		name_to_firing_mode(const char *);




/*
 * character_files.c
 */
void			load_character_files(void);



/*
 * character_generation_manual.c
 */
void			generate_player_character(void);

void			roll_name(CHARACTER *);
void			roll_stats(CHARACTER *);



/*
 * character_generation_random.c
 */
CHARACTER *		generate_random_character(void);






/*
 * character_inventory.c
 */
#define inventory_n_objects(c)		((c)->inventory->n_nodes)

void		inventory_screen(CHARACTER *);

void		inventory_add(CHARACTER *, OBJECT *);
void		inventory_remove(CHARACTER *, OBJECT *);

OBJECT *	inventory_random_object(CHARACTER *);

OBJECT *	inventory_find_object(CHARACTER *, const char *);

bool		inventory_is_full(const CHARACTER *);

MONEY		inventory_value(const CHARACTER *);

MONEY		inventory_money(const CHARACTER *);
void		inventory_money_reduce(CHARACTER *, MONEY);
void		inventory_money_increase(CHARACTER *, MONEY);

void		inventory_object_print(const CHARACTER *, const OBJECT *,
			FILE *
		);

WEIGHT		inventory_weight(const CHARACTER *);




/*
 * character_movement.c
 */
bool		character_can_move(const CHARACTER *);

bool		move_character(CHARACTER *, const AREA_POINT *);

AREA_DISTANCE	run_factor(const CHARACTER *);

AREA_DISTANCE	movement_speed_max(const CHARACTER *);

bool		run(CHARACTER *, const PATH_NODE *, AREA_DISTANCE);

bool		can_push_past(const CHARACTER *, const CHARACTER *);

void		push_past(CHARACTER *, CHARACTER *);






/*
 * character_perception.c
 */
#define los(p1, p2)		(line_of_sight((p1), (p2)) || line_of_sight((p2), (p1)))

#define enemies_noticed(c)	((c)->noticed_enemies->head != NULL)

void		view_screen(const CHARACTER *, const char *, COLOUR);

void		update_perception_data(CHARACTER *);

void		activate_stealth(CHARACTER *);

void		cancel_stealth(CHARACTER *, const CHARACTER *);

bool		character_unnoticed(const CHARACTER *);

void		notice_check(CHARACTER *, bool, bool);

bool		party_can_see(const CHARACTER *);

bool		enemies_are_watching(const CHARACTER *);

CHARACTER *	nearest_noticed_enemy(const CHARACTER *);

bool		unnoticed_enemy_at(const CHARACTER *, const AREA_POINT *);

bool		line_of_sight(const AREA_POINT *, const AREA_POINT *);







/*
 * character_perks.c
 */
#define character_set_perk(c,p)			((c)->perk[(p)] = true)
#define character_has_perk(c,p)			((c)->perk[(p)])

#define character_has_script(c)	\
	(!is_empty_string((c)->script))

#define iterate_over_perks(i)	for ((i) = PK_LB; (i) < MAX_PERKS; (i)++)

void			perk_screen(PERK);

bool			perk_useable(PERK);

N_PERKS			character_n_perks(const CHARACTER *);

LIST *			useable_perks_list(const CHARACTER *);

const char *		perk_name(PERK);

PERK			name_to_perk(const char *);

char *			perk_description(char *, PERK);

void			give_perk(CHARACTER *, PERK);

bool			stealth_use(CHARACTER *);

void			disarm_use(CHARACTER *, CHARACTER *);

HIT_MODIFIER		uncanny_dodge_modifier(const CHARACTER *);

bool			rapid_strike_requirements_met(const CHARACTER *,
				const OBJECT *
			);




/*
 * character_race.c
 */
RACE *		player_race(void);





/*
 * character_stats.c
 */
#define iterate_over_stats(i) \
	for ((i) = S_LB; (i) < MAX_STATS; (i)++)


void			stat_advance(CHARACTER *, STAT);

STAT_BONUS		stat_bonus(const CHARACTER *, STAT);

void			stat_modifiers_apply(CHARACTER *, STAT_MODIFIER *);
void			stat_modifiers_revert(CHARACTER *,
				const STAT_MODIFIER *, STAT_MODIFIER
			);

const char *		stat_short_name(STAT);
const char *		stat_long_name(STAT);

bool			is_short_stat_name(const char *);

STAT			name_to_stat(const char *, bool);



/*
 * character_talk.c
 */
void		dialogue_start(CHARACTER *, CHARACTER *);
void		dialogue_say(const char *, ...);
void		dialogue_add_option(const char *);
const char *	dialogue_choice(void);









/*
 * combat.c
 */
void	attack_init(ATTACK_DATA *);

bool	target_hit(ATTACK_DATA *);

void	critical_hit_scored(ATTACK_DATA *);

void	hit(ATTACK_DATA *);

void	miss(ATTACK_DATA *);



/*
 * combat_close.c
 */
void		strike(CHARACTER *, OBJECT *, const AREA_POINT *);

bool		involved_in_close_combat(const CHARACTER *);

DICE_ROLL *	unarmed_damage_power(const CHARACTER *, DICE_ROLL *);


/*
 * combat_ranged.c
 */
void		shoot(CHARACTER *, OBJECT *, FIRING_DATA *,
			const AREA_POINT *
		);

char *		range_description(char *, AREA_DISTANCE);





/*
 * command.c
 */
void		player_control(CHARACTER *);








/*
 * data_files.c
 */
void		set_data_path(PROGRAM_DIRECTORY, const char *);
const char *	data_path(void);

void		load_data_files(void);

void		read_symbol_field(SYMBOL *);
void		read_colour_field(COLOUR *);
void		read_dynamic_colour_field(DYNAMIC_COLOUR *);
void		read_boolean_field(bool *);
void		read_name_field(char *, unsigned int);
void		read_numerus_field(NUMERUS *);
OBJECT *	read_object_field(void);
void		read_description_field(char *, const char *);
void		read_dice_roll_field(DICE_ROLL *);






/*
 * debug.c
 */
void		debug_console(void);




/*
 * dice.c
 */
#define d100()		dice(1, 100)
#define d10()		dice(1, 10)

int		dice(int, int);
int		dice_roll(const DICE_ROLL *);
int		dice_roll_average(const DICE_ROLL *);
int		d100_test(int, int);
bool		d100_test_passed(int);

/* random choice macro */
#define random_choice(p)		(random_int(1, 100) <= (p))








/*
 * effect.c
 */
void		effect_activate(CHARACTER *, EFFECT, TIMER);
void		effect_terminate(CHARACTER *, EFFECT);
void		effect_terminate_event(EVENT *);

bool		poison_resisted(const CHARACTER *);
bool		disease_resisted(const CHARACTER *);
bool		pain_resisted(const CHARACTER *);
bool		fear_resisted(const CHARACTER *);

void		poison_damage(EVENT *);



/*
 * event.c
 */
void *		event_new(void);
void		event_free(void *);

EVENT *		event_create(EVENT_TYPE, TIMER);

void		event_destroy(void *);

void		event_set(EVENT *);

void		handle_events(void);

void		event_execute(EVENT *);

EVENT *		remove_character_event(const CHARACTER *, EVENT_TYPE);
void		remove_character_events(const CHARACTER *, LIST *);

void		remove_object_events(const OBJECT *, LIST *);

void		remove_terrain_events(const TERRAIN *, LIST *);

EVENT *		effect_termination_event(CHARACTER *, EFFECT);




/*
 * faction.c
 */
void		faction_init(void);
void		faction_clean_up(void);
 
void		faction_add(const char *);

void		factions_finalise(void);

FACTION_RELATIONSHIP	faction_relationship(FACTION, FACTION);

void			faction_set_relationship(FACTION, FACTION, 
				FACTION_RELATIONSHIP
			);

const char *		faction_name(FACTION);

N_FACTIONS		n_factions(void);

FACTION			name_to_faction(const char *);
FACTION_RELATIONSHIP	name_to_faction_relationship(const char *);
 
bool		hostility_check(CHARACTER *);

bool		hostility_between(const CHARACTER *, const CHARACTER *);


/*
 * galaxy.c
 */
void		galaxy_generation(void);


/*
 * game.c
 */
void		game_run(void);

GAME_DIFFICULTY game_difficulty(void);

void 		game_difficulty_set(GAME_DIFFICULTY);

void		game_controls_screen(void);



/*
 * help.c
 */
void		help_screen(void);



/*
 * honour.c
 */
void		certificate_screen(void);


/*
 * info_files.c
 */
void		load_info_files(void);

char *		data_file_character_attribute_description(char *,
			CHARACTER_ATTRIBUTE
		);

char *		data_file_object_attribute_description(char *,
			OBJECT_ATTRIBUTE
		);

char *		data_file_terrain_attribute_description(char *,
			TERRAIN_ATTRIBUTE
		);

char *		data_file_psy_power_description(char *,
			PSY_POWER
		);

char *		data_file_perk_description(char *, PERK);




/*
 * loadsave.c
 */
void		save_point_create(void);
bool		save_point_load(void);
bool		save_point_exists(void);

void		saved_game_erase(void);

void		area_write(const char *);
void		area_read(const char *);

const char *	area_file_name(const PLANET_POINT *);

bool		area_exists(const PLANET_POINT *);







/*
 * log.c
 */
void		log_init(void);
void		log_clean_up(void);

void		log_output(const char *, ...);




/*
 * macro.c
 */
void		macro_init(void);
void		macro_clean_up(void);

void		set_macro(MACRO_INDEX, const KEY_CODE *);
KEY_CODE *	get_macro(MACRO_INDEX);

bool		macro_defined(COMMAND);

void		macro_setup_screen(void);

COMMAND		do_macro(COMMAND);







/*
 * message_dynamic.c
 */
void	dynamic_message(MESSAGE_TYPE, const CHARACTER *, const void *,
		MESSAGE_OBJECT_TYPE
	);

void	message_force_visibility(bool);



/*
 * message_grammar.c
 */
char *			subject_string(void);
char *			subject_string_o(void);

char *			object_string(void);

char *			possessive_form(char *);

const char *		plural_form(const char *);

const char *		possessive_pronoun(const CHARACTER *);
const char *		reflexive_pronoun(const CHARACTER *);

const char *		conjugated_verb(const char *, GRAMMATICAL_PERSON);

GRAMMATICAL_PERSON	grammatical_person(const CHARACTER *);

char *			lowercase_object_string(void);



/*
 * object.c
 */
void			object_init(void);
void			object_clean_up(void);

OBJECT *		object_box_new_object(void);
void			object_box_optimise(void);
BOX_SIZE		object_box_size(void);

void *			object_new(void);
void			object_free(void *);

OBJECT *		object_prototype(OBJECT_INDEX);

OBJECT *		object_create(const char *);
OBJECT *		object_create_i(OBJECT_INDEX);

void			object_destroy(void *);

OBJECT *		object_clone(const OBJECT *);

bool			object_is_connector(const OBJECT *);

void			object_screen(const OBJECT *);

const char *		object_type_name(OBJECT_TYPE);
const char *		object_subtype_name(OBJECT_SUBTYPE);

OBJECT_TYPE		name_to_object_type(const char *);
OBJECT_SUBTYPE		name_to_object_subtype(const char *);





/*
 * object_attributes.c
 */
#define object_set_attribute(o,attr) \
	((o)->attribute[(attr)] = true)

#define object_remove_attribute(o,attr)	\
	((o)->attribute[(attr)] = false)

#define object_has_attribute(o,attr)		((o)->attribute[(attr)])

void			object_attribute_screen(OBJECT_ATTRIBUTE);

char *			object_attribute_description(char *,
				OBJECT_ATTRIBUTE
			);

const char *		object_attribute_name(OBJECT_ATTRIBUTE);

OBJECT_ATTRIBUTE	name_to_object_attribute(const char *);



/*
 * object_drugs.c
 */
bool			use_drug(CHARACTER *, OBJECT *);

void			drug_terminate(EVENT *);




/*
 * object_files.c
 */
void			load_object_files(void);




/*
 * party.c
 */
void		party_init(void);
void		party_clean_up(void);

LIST *		party_player(void);

void		party_join(CHARACTER *);

void		party_leave(CHARACTER *);

PARTY_SIZE	party_n_members(void);
bool		party_is_member(const CHARACTER *);

void		party_set_tactic(AI_TACTIC);

POWER_LEVEL	party_power_level(void);

bool		party_has_battle_tactics(void);
bool		party_has_leader(void);

void		switch_character(CHARACTER_TYPE);






/*
 * pathfinder.c
 */
#define path_first_step(path)		(&(path)->p)

void			pathfinder_init(void);
void			pathfinder_clean_up(void);

void			pathfinder_config(const PATHFINDER_CONFIG *);

const PATH_NODE *	pathfinder_find_path(void);

const PATH_NODE *	pathfinder_path(void);

AREA_DISTANCE		pathfinder_path_length(void);

const PATH_NODE *	find_jump_path(const CHARACTER *,
				const AREA_POINT *
			);

bool			destructable_obstacle_at(const AREA_POINT *);



/*
 * planet.c
 */
void			planet_init(void);
void			planet_clean_up(void);

PLANET *			planet(void);
PLANET_TILE *		planet_tile(const PLANET_POINT *);

PLANET_TILE *		planet_tile_box_new_tile(void);
void			planet_tile_box_optimise(void);

bool			planet_points_equal(const PLANET_POINT *,
				const PLANET_POINT *
			);

void			move_planet_point(PLANET_POINT *, DIRECTION);

bool			out_of_planet_bounds(const PLANET_POINT *);

void			planet_map_screen(CHARACTER *);

const PLANET_TILE *	symbol_to_planet_tile(SYMBOL);



/*
 * planet_files.c
 */
void			load_planet_files(void);





/*
 * platform_*.c
 */
void		platform_init(void);
void		platform_clean_up(void);

char *		get_file_path(char *, PROGRAM_DIRECTORY, const char *);

LIST *		data_files(PROGRAM_DIRECTORY);

void		sec_sleep(int);

void		ui_init(void);
void		ui_clean_up(void);

bool		fullscreen_mode(void);
void		change_screen_mode(void);

void		clear_screen(void);
void		update_screen(void);

void		place_cursor_at(SCREEN_COORD, SCREEN_COORD);

SCREEN_COORD	cursor_y(void);
SCREEN_COORD	cursor_x(void);

void		render_cursor(COLOUR);

void		render_char(COLOUR, SYMBOL);

void		render_char_at(COLOUR, SCREEN_COORD, SCREEN_COORD,
			SYMBOL
		);

KEY_CODE	lowlevel_get_key(void);

void		clear_text_window(SCREEN_COORD, SCREEN_COORD,
			SCREEN_COORD, SCREEN_COORD
		);

void		render_background(void);




/*
 * program_manager.c
 */
void	program_init(void);
void	program_shutdown(void);

void	die(const char *, ...);
void	die_no_data_path(const char *, ...);




/*
 * psychic.c
 */
#define psy_power_key(p)	('a' + (p))
#define key_to_psy_power(k)	((k) - 'a')
#define iterate_over_psy_powers(i) \
	for ((i) = PSY_LB; (i) < MAX_PSY_POWERS; (i)++)
#define character_has_psy_power(c,p)	((c)->psy_power[(p)] == true)
#define character_set_psy_power(c,p)	((c)->psy_power[(p)] = true)


void			give_psy_power(CHARACTER *, PSY_POWER);

void			psy_power_screen(PSY_POWER);

const char *		psy_power_description(char *, PSY_POWER);

bool			evoke_psy_power(CHARACTER *, PSY_POWER,
				const AREA_POINT *
			);

void			psy_power_terminate(EVENT *);

bool			is_able_to_evoke_psy_powers(const CHARACTER *);

N_PSY_POWERS		n_psy_powers(const CHARACTER *);

LIST *			character_psy_powers(const CHARACTER *);

bool			psy_power_implemented(PSY_POWER);

bool			psy_power_is_hostile(PSY_POWER);

bool			psy_power_requires_target(PSY_POWER);

bool			psy_power_is_tricky(PSY_POWER);
bool			psy_power_valid_target(PSY_POWER, const AREA_POINT *);
bool			psy_power_in_effect(PSY_POWER, const AREA_POINT *);
bool			psy_power_target_is_immune(PSY_POWER,
				const AREA_POINT *
			);

const char *		psy_power_name(PSY_POWER);

EVOCATION_VALUE		psy_power_evocation_value(PSY_POWER);

void			psy_powers_screen(const CHARACTER *);

PSY_POWER		name_to_psy_power(const char *);



/*
 * psychic_powers.c
 */
void	psy_regenerate(const EVOCATION_DATA *);
void	psy_reactivate(const EVOCATION_DATA *);
void	psy_warp_strength(const EVOCATION_DATA *);
void	psy_warp_strength_terminate(EVENT *);
void	psy_purify_blood(const EVOCATION_DATA *);
void	psy_purge_plague(const EVOCATION_DATA *);

void	psy_mind_view(const EVOCATION_DATA *);
void	psy_psychic_shriek(const EVOCATION_DATA *);
void	psy_terrify(const EVOCATION_DATA *);

void	psy_psychic_shield(const EVOCATION_DATA *);
void	psy_psychic_shield_terminate(EVENT *);

void	psy_banishment(const EVOCATION_DATA *);




/*
 * quest.c
 */
void		quest_init(void);
void		quest_clean_up(void);

LIST *		quest_list(void);

void *		quest_new(void);
void		quest_free(void *);

void		quest_set_status(const char *, const char *);
const char *	quest_status(const char *);




/*
 * race_files.c
 */
void		load_race_files(void);




/*
 * randomiser.c
 */
void		randomiser_init(void);
void		randomiser_clean_up(void);

void		randomiser_add(const char *, RVALUE);
void		randomiser_clear(void);
const char *	randomiser(void);







/*
 * random_names.c
 */
void		random_names_init(void);
void		random_names_clean_up(void);

const char *	random_name(NAME_SET);





/*
 * rng.c
 */
void		rng_init(void);
void		rng_clean_up(void);

int		random_int(int, int);



/*
 * rule_files.c
 */
void		load_rule_files(void);



/*
 * scenario.c
 */
#define NO_SCENARIO_INSTALLED		(scenario_name()[0] == '\0')

char *		scenario_name(void);

char *		scenario_version(void);

void		scenario_intro(void);

void		scenario_end(const char *);





/*
 * script.c
 */
void		script_init(void);
void		script_clean_up(void);

bool		script_load(PROGRAM_DIRECTORY, const char *);
void		script_execute(void);

void		script_set_data(const char *, const char *);
void		script_set_data_numeric(const char *, int);

void *		script_pointer(WCA_STORAGE_INDEX);

CHARACTER **	script_pointer_self(void);
CHARACTER **	script_pointer_active_character(void);

bool		script_bool_to_C_bool(WCA_STORAGE_INDEX);




/*
 * script_functions.c
 */
void		script_add_functions(void);




/*
 * shop.c
 */
void	shop_sell_mode(CHARACTER *);

void	shop_buy_mode(CHARACTER *);

void	shop_add(const char *);

void	shop_set_price_level(PRICE_LEVEL);




/*
 * targeter.c
 */
bool	pick_target(const char *, const CHARACTER *,
		TARGET_MODE, int, AREA_POINT *
	);







/*
 * terrain.c
 */
void			terrain_init(void);
void			terrain_clean_up(void);

TERRAIN *		terrain_box_new_terrain(void);
void			terrain_box_optimise(void);

void *			terrain_new(void);
void			terrain_free(void *);

TERRAIN *		terrain_create(const char *);
TERRAIN *		terrain_create_i(TERRAIN_INDEX);

void			terrain_destroy(void *);

TERRAIN *		terrain_clone(const TERRAIN *);

void			terrain_screen(const TERRAIN *);

void			terrain_cause_disease(CHARACTER *);

bool			terrain_dangerous_for(const CHARACTER *,
				const TERRAIN *
			);



/*
 * terrain_attributes.c
 */
#define terrain_set_attribute(t,attr)	((t)->attribute[attr] = true)
#define terrain_has_attribute(t,attr)	((t)->attribute[attr])

void			terrain_attribute_screen(TERRAIN_ATTRIBUTE);

const char *		terrain_attribute_name(TERRAIN_ATTRIBUTE);

char *			terrain_attribute_description(char *,
				TERRAIN_ATTRIBUTE
			);

TERRAIN_ATTRIBUTE	name_to_terrain_attribute(const char *);



/*
 * terrain_files.c
 */
void		load_terrain_files(void);





/*
 * timing.c
 */
void		game_round(void);

bool		is_active_character(const CHARACTER *);

bool		character_can_act(const CHARACTER *);





/*
 * ui.c
 */
void		render_header(const char *);
void		render_field(const char *, const char *, ...);

void		render_text_centered(COLOUR, SCREEN_COORD, const char *);

void		render_text(COLOUR, const char *, ...);
void		render_text_at(COLOUR, SCREEN_COORD, SCREEN_COORD,
			const char *, ...
		);

void		render_lstring_at(COLOUR, SCREEN_COORD, SCREEN_COORD,
			const char *, int
		);

SCREEN_COORD	render_long_text(COLOUR, SCREEN_COORD, SCREEN_COORD,
			const char *, SCREEN_COORD
		);

void		render_stats_at(const CHARACTER *, SCREEN_COORD, SCREEN_COORD);

const char *	dice_roll_string(const DICE_ROLL *);

void		render_key_name(COLOUR, KEY_CODE);

void		render_secondary_screen(const char *, ...);

void		render_overlay_screen_message(const char *, ...);

void		render_overlay_screen_prompt(const char *, ...);

void		message(SCREEN_ID, const char *, ...);
void		message_flush(SCREEN_ID);

void		message_clear(SCREEN_ID);

void		command_bar_message(SCREEN_ID, const char *);

void		render_character_status(const CHARACTER *, SCREEN_COORD, bool);

void		render_inventory_object(COLOUR, const CHARACTER *,
			const OBJECT *, bool
		);

void		clear_screen_window(SCREEN_COORD, SCREEN_COORD,
			SCREEN_COORD, SCREEN_COORD
		);

void		render_menu(MENU *, SCREEN_AREA *);

void		render_hot_key(KEY_CODE);

void		render_command_bar(SCREEN_ID);

void		confirmation_dialogue(CONFIRMATION_DIALOGUE *);

void		show_text_file(const char *, PROGRAM_DIRECTORY,
			const char *, bool
		);



/*
 * ui_colour.c
 */

#if !defined(UI_COLOUR_MODULE)

/*
 * UI colours
 */
extern COLOUR C_Text;
extern COLOUR C_Highlight;
extern COLOUR C_FieldValue;
extern COLOUR C_FieldValueIncreased;
extern COLOUR C_FieldValueReduced;
extern COLOUR C_FieldName;
extern COLOUR C_Header;
extern COLOUR C_HotKey;
extern COLOUR C_HotKeyBracket;
extern COLOUR C_SecondaryScreenTitle;
extern COLOUR C_Warning;
extern COLOUR C_Injured;
extern COLOUR C_Cursor;
extern COLOUR C_AggressiveCursor;
extern COLOUR C_PsychicCursor;
extern COLOUR C_Metal;
extern COLOUR C_Blood;

#endif

COLOUR_DATA *		colour_new(void);

const RGB_DATA *	colour_rgb(COLOUR);

N_COLOURS		n_colours(void);

COLOUR *		ui_colour(const char *);

void			determine_colour(DYNAMIC_COLOUR *);

COLOUR			name_to_colour(const char *);



/*
 * ui_command.c
 */
#define iterate_over_commands(i) \
	for ((i) = CM_LB; (i) < MAX_COMMANDS; (i)++)

KEY_CODE	command_key(COMMAND);
void		set_command_key(COMMAND, KEY_CODE);

bool		is_move_command(COMMAND);
bool		is_macro_command(COMMAND);

bool		move_command_to_direction(DIRECTION *, COMMAND);

COMMAND		name_to_command(const char *);
const char *	command_name(COMMAND);

bool		show_on_help_screen(COMMAND);

void		render_pick_target_screen(const char *);
void		render_advanced_target_mode_screen(const CHARACTER *,
			AREA_POINT *, TARGET_MODE
		);

void		render_view_screen(const char *, AREA_POINT *, COLOUR);

void		render_choose_firing_mode_screen(void);

void		render_number_of_shots_screen(const CHARACTER *);
void		render_spread_factor_screen(void);

void		render_use_elevator_screen(void);
void		use_elevator_dialogue(GET_TEXT_DIALOGUE *);

void		render_help_screen(void);

void		render_game_controls_screen(void);
COMMAND		game_controls_screen_menu(MENU *);

void		render_macro_setup_screen(COMMAND, MACRO_INDEX);

void		render_inventory_screen(const CHARACTER *, int);

void		attack_confirmation_dialogue(CONFIRMATION_DIALOGUE *);

void		leave_area_dialogue(CONFIRMATION_DIALOGUE *);
void		enter_area_dialogue(CONFIRMATION_DIALOGUE *);



/*
 * ui_command_bar.c
 */
void		command_bar_clear(void);

void		command_bar_set(N_COMMANDS, ...);

void		command_bar_add(N_COMMANDS, ...);
void		command_bar_add_move_commands(void);

COMMAND		command_bar_command(KEY_CODE);
COMMAND		command_bar_get_command(void);

COMMAND		command_bar_item(int);

void		use_game_screen_command_bar(void);
COMMAND		game_screen_command(int);


/*
 * ui_files.c
 */
void		load_ui_files(void);


/*
 * ui_game.c
 */
void		render_game_screen(void);

void 		render_message_window(void);

GORE_FEATURE *	gore_feature(const char *);



/*
 * ui_gen_adv.c
 */
void		render_choose_gender_screen(void);
COMMAND		choose_gender_screen_menu(MENU *);

void		render_roll_stats_screen(const CHARACTER *);

void		render_name_character_screen(void);
void		enter_name_dialogue(GET_TEXT_DIALOGUE *);

void		render_character_advancement_screen(const CHARACTER *);
COMMAND		character_advancement_screen_menu(MENU *);

void		render_increase_stat_screen(const CHARACTER *, MENU *, bool);
COMMAND		increase_stat_screen_menu(MENU *, bool);

void		render_new_perk_screen(const CHARACTER *, MENU *);
COMMAND		new_perk_screen_menu(MENU *);

void		render_learn_psy_power_screen(const CHARACTER *, MENU *, bool);
COMMAND		learn_psy_power_screen_menu(MENU *, bool);

void		render_follow_career_path_screen(const CHARACTER *, MENU *);
COMMAND		follow_career_path_screen_menu(MENU *);

void		render_new_career_path_screen(const CHARACTER *, bool);
COMMAND		new_career_path_screen_menu(MENU *, bool);



/*
 * ui_info.c
 */
void		render_planet_map_screen(const PLANET_POINT *,
			const CHARACTER *
		);

void		render_career_screen(const CAREER *);

void		render_character_screen_basics(const CHARACTER *);
void		render_character_attribute_screen(CHARACTER_ATTRIBUTE);

void		render_object_screen(const OBJECT *);
void		render_object_attribute_screen(OBJECT_ATTRIBUTE);

void		render_terrain_screen(const TERRAIN *);
void		render_terrain_attribute_screen(TERRAIN_ATTRIBUTE);

void		render_psy_powers_screen(const CHARACTER *);
void		render_psy_power_screen(PSY_POWER);
void		render_perk_screen(PERK);

void		render_additional_information_screen(void);
COMMAND		additional_information_menu(MENU *);

void		render_entity_info_list_screen(const char *);
COMMAND		entity_info_list_menu(MENU *);



/*
 * ui_input.c
 */

#define abort_macro()			key_stack_clear()

#define macro_is_being_executed()	(!key_stack_is_empty())

KEY_CODE	key_stack_pop(void);
void		key_stack_push(KEY_CODE);
void		key_stack_clear(void);
bool		key_stack_is_empty(void);

KEY_CODE 	limited_get_key(const char *);

KEY_CODE	get_key(void);

char *		get_text_dialogue_execute(GET_TEXT_DIALOGUE *,
			SCREEN_COORD, SCREEN_COORD
		);

void		confirmation_dialogue_input(CONFIRMATION_DIALOGUE *);

void		macro_input(MACRO_INDEX);

void		ui_get_object_name(char *);
void		ui_get_quest_name(char *);
void		ui_get_quest_status(char *);



/*
 * ui_keys.c
 */
KEY_CODE	name_to_key_code(const char *);

const char *	key_name(KEY_CODE);



/*
 * ui_menu.c
 */
void		render_select_list(const LIST *);
LIST_NODE_INDEX select_list_choice(const LIST *, KEY_CODE);

MENU *		menu_create(LIST *);

void		menu_free(MENU *);
void		menu_free_with(MENU *, void (*)(void *));

void		menu_set_appearance(MENU *, SCREEN_AREA *);

COMMAND		menu_execute(MENU *, SCREEN_AREA *);



/*
 * ui_message.c
 */
void		game_screen_message(char *);

void		game_screen_message_flush(void);

char *		game_screen_message_buffer(void);




/*
 * ui_misc.c
 */
GAME_DIFFICULTY	ui_choose_difficulty(void);

void		render_game_title_screen(const char *);

COMMAND		game_title_screen_menu(MENU *);

void		new_game_dialogue(CONFIRMATION_DIALOGUE *);

void		render_galaxy_generation_screen(bool);

void		render_shop_screen(const CHARACTER *);
COMMAND		shop_screen_menu(MENU *);

void		render_dialogue_screen(DIALOGUE_STATE *);
COMMAND		dialogue_screen_menu(MENU *);

void		render_certificate_screen(void);

void		render_sell_screen(CHARACTER *, int);



/*
 * ui_symbols.c
 */
SYMBOL		name_to_symbol(const char *);




/*
 * ui_vision.c
 */
void			update_view(const AREA_POINT *, bool, bool);
void			update_view_point(const AREA_POINT *);
void			update_view_character(const CHARACTER *);

bool			outside_of_visible_area(const AREA_COORD, const AREA_COORD);

bool			vaw_convert_coordinates(SCREEN_COORD *, SCREEN_COORD *, AREA_COORD, AREA_COORD);

void			set_visible_area(const AREA_SECTION *);
const AREA_SECTION *	get_visible_area(void);





/*
 * utility.c
 */
#define ascii_to_integer(c)		((c) - '0')

#ifndef max
#define max(a, b)               ((a) < (b) ? (b) : (a))
#endif

#ifndef min
#define min(a, b)               ((a) < (b)? (a) : (b))
#endif

COORD_MODIFIER	direction_modifier(COORD_TYPE, DIRECTION);
DIRECTION	name_to_direction(const char *);
DIRECTION *	randomised_directions(DIRECTION *);

void *		checked_malloc(size_t);
void *		checked_realloc(void *, size_t);

int		divide_and_round_up(int, int);

int		hex_byte_to_decimal(const char *);

FILE *		open_file(const char *, const char *);
void		close_file(FILE *);
bool		file_exists(const char *);
long int	file_length(FILE *);

char *		read_text_file(char *, PROGRAM_DIRECTORY, const char *);


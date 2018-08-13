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
 * Module: Loading and Saving
 */

#include "wrogue.h"





static void		state_write(void);
static bool		state_read(void);

static void		restore_tables_alloc(void);
static void		restore_tables_free(void);

static void		serials_reset(void);

static void		header_write(void);
static void		header_read(void);

static void		sectors_write(void);
static void		sector_write(SECTOR *);

static void		sectors_read(void);
static void		sector_read(SECTOR *);

static void		terrain_write(TERRAIN *);
#ifdef QQQ
static void		terrain_write_pointer(const TERRAIN *);
#endif


static TERRAIN *	terrain_read(void);
#ifdef QQQ
static TERRAIN *	terrain_read_pointer(void);
#endif

static void		object_write(OBJECT *);
static void		object_write_pointer(const OBJECT *);

static OBJECT *		object_read(void);
static OBJECT *		object_read_pointer(void);

static void		character_write(CHARACTER *);
static void		character_write_attributes(const CHARACTER *);
static void		character_write_stats(const CHARACTER_STAT *);
static void		character_write_inventory(CHARACTER *);
static void		character_write_pointer(const CHARACTER *);

static CHARACTER *	character_read(void);
static void		character_read_attributes(CHARACTER *);
static void		character_read_stats(CHARACTER_STAT *);
static void		character_read_inventory(CHARACTER *);
static void		character_recreate_status(CHARACTER *);
static CHARACTER *	character_read_pointer(void);

static void		events_write(void);
static void		event_write(const EVENT *);

static void		events_read(void);
static EVENT *		event_read(void);

static void		version_write(void);
static bool		file_is_compatible(void);

static void		difficulty_write(void);
static void		difficulty_read(void);

static void		visible_area_write(void);
static void		visible_area_read(void);

static void		macros_write(void);
static void		macros_read(void);

static void		quests_write(void);
static void		quests_read(void);

static void		faction_relationships_write(void);
static void		faction_relationships_read(void);

static void		planet_point_write(const PLANET_POINT *);
static void		planet_point_read(PLANET_POINT *);

static void		area_point_write(const AREA_POINT *);
static void		area_point_read(AREA_POINT *);

static void		area_section_write(const AREA_SECTION *);
static void		area_section_read(AREA_SECTION *);

static void		ai_data_write(const AI_CONTROL_DATA *);
static void		ai_data_read(AI_CONTROL_DATA *);

static void		bool_array_write(const bool *, int);
static void		bool_array_read(bool *, int);

static void		career_pointer_write(const CAREER *);
static CAREER *		career_pointer_read(void);




/*
 * these arrays are used to restore pointers
 */
static TERRAIN **	TerrainRestore = NULL;
static OBJECT **	ObjectRestore = NULL;
static CHARACTER **	CharacterRestore = NULL;


/*
 * the serial number iteration variables
 */
static SERIAL		TerrainSN;
static SERIAL		ObjectSN;
static SERIAL		CharacterSN;



/*
 * creates a save point
 */
void save_point_create(void)
{

	/* save the game state */
	state_write();

	/* save the active area */
	area_write(FILE_SAVE_POINT_AREA);
}




/*
 * loads a save point
 */
bool save_point_load(void)
{

	/* load the game state */
	if (!state_read()) return false;

	/* load the active area */
	area_read(FILE_SAVE_POINT_AREA);

	return true;
}



/*
 * returns true if there is a save point exists
 */
bool save_point_exists(void)
{

	/* we figure this out by checking for a game state file */
	set_data_path(DIR_USER_DATA, FILE_GAME_STATE);

	if (file_exists(data_path())) return true;

	return false;
}



/*
 * deletes the saved game
 */
void saved_game_erase(void)
{
	LIST *file_list = data_files(DIR_USER_DATA);
	LIST_NODE *node;

	/* delete all .rdb files in the USER_DATA directory ... */
	for (node = file_list->head; node != NULL; node = node->next) {
		const char *file_name = node->data;

		set_data_path(DIR_USER_DATA, file_name);
		remove(data_path());
	}

	list_free_with(file_list, free);
}



/*
 * writes the active area
 */
void area_write(const char *file_name)
{

	serials_reset();

	set_data_path(DIR_USER_DATA, file_name);

	rdb_open(data_path(), RDB_WRITE);

	header_write();

	sectors_write();

	events_write();

	rdb_close();
}



/*
 * reads an area
 */
void area_read(const char *file_name)
{

	restore_tables_alloc();

	serials_reset();

	set_data_path(DIR_USER_DATA, file_name);

	rdb_open(data_path(), RDB_READ);

	header_read();

	sectors_read();

	events_read();

	rdb_close();

	restore_tables_free();
}



/*
 * returns the file name of an area
 */
const char * area_file_name(const PLANET_POINT *area_location)
{
	static char file_name[FILE_NAME_SIZE];

	sprintf(file_name, "%02d%02d%02d.rdb",
		area_location->z, area_location->y, area_location->x
	);

	return file_name;
}



/*
 * returns true if an area exists at the passed point
 */
bool area_exists(const PLANET_POINT *planet_point)
{

	set_data_path(DIR_USER_DATA, area_file_name(planet_point));

	if (file_exists(data_path())) {

		return true;
	}

	return false;
}






/*
 * writes the game state file
 */
static void state_write(void)
{

	set_data_path(DIR_USER_DATA, FILE_GAME_STATE);

	rdb_open(data_path(), RDB_WRITE);

	version_write();
	
	difficulty_write();

	visible_area_write();

	macros_write();

	quests_write();
	
	faction_relationships_write();

	rdb_close();
}



/*
 * reads the game state file
 */
static bool state_read(void)
{

	set_data_path(DIR_USER_DATA, FILE_GAME_STATE);

	rdb_open(data_path(), RDB_READ);

	if (!file_is_compatible()) return false;
		
	difficulty_read();

	visible_area_read();

	macros_read();

	quests_read();
	
	faction_relationships_read();

	rdb_close();

	return true;
}



/*
 * allocates memory for the pointer restoration tables
 */
static void restore_tables_alloc(void)
{

	TerrainRestore = checked_malloc(AREA_SIZE *
		sizeof *TerrainRestore
	);

	ObjectRestore = checked_malloc(
		AREA_MAX_OBJECTS * sizeof *ObjectRestore
	);

	CharacterRestore = checked_malloc(
		AREA_MAX_CHARACTERS * sizeof *CharacterRestore
	);
}



/*
 * frees the pointer restoration tables
 */
static void restore_tables_free(void)
{

	free(TerrainRestore);
	free(ObjectRestore);
	free(CharacterRestore);
}



/*
 * resets the serial number iteration variables
 */
static void serials_reset(void)
{

	TerrainSN = 0;
	ObjectSN = 0;
	CharacterSN = 0;
}



/*
 * writes the area header
 */
static void header_write(void)
{
	const AREA *area = active_area();

	/* write area name */
	rdb_write_field_name("N");
	rdb_write_string(area->name);

	/* write area location */
	planet_point_write(&area->location);
}



/*
 * reads the area header
 */
static void header_read(void)
{
	AREA *area = active_area();

	/* read area name */
	rdb_next_field();
	strcpy(area->name, rdb_data_token(RDB_NEXT_TOKEN));

	/* read area location */
	planet_point_read(&area->location);
}



/*
 * writes the area sectors
 */
static void sectors_write(void)
{
	const AREA_SECTION *bounds = area_bounds();
	AREA_POINT p;

	/* iterate over the entire area, write all sectors */
	for (p.y = bounds->top; p.y <= bounds->bottom; p.y++) {
	for (p.x = bounds->left; p.x <= bounds->right; p.x++) {
		SECTOR *sector = sector_at(&p);

		sector_write(sector);
	}
	}
}



/*
 * writes an area sector
 */
static void sector_write(SECTOR *sector)
{

	/* write terrain */
	terrain_write(sector->terrain);

	/* write object */
	object_write(sector->object);

	/* write character */
	character_write(sector->character);
}



/*
 * reads the area sectors
 */
static void sectors_read(void)
{
	const AREA_SECTION *bounds = area_bounds();
	AREA_POINT p;

	/* iterate over the entire area, read all sectors */
	for (p.y = bounds->top; p.y <= bounds->bottom; p.y++) {
	for (p.x = bounds->left; p.x <= bounds->right; p.x++) {
		SECTOR *sector = sector_at(&p);

		sector_read(sector);
	}
	}
}



/*
 * reads an area sector
 */
static void sector_read(SECTOR *sector)
{

	/* read terrain */
	sector->terrain = terrain_read();

	/* read object */
	sector->object = object_read();

	/* read character */
	sector->character = character_read();
}



/*
 * writes a terrain
 */
static void terrain_write(TERRAIN *terrain)
{

	/* if the terrain is a NULL terrain.. */
	if (terrain == NULL) {

		/* write NULL entry and return */
		rdb_write_field_name("T");
		rdb_write_string("");
		return;
	}

	/* write terrain data */
	rdb_write_field_name("T");
	rdb_write_integer(terrain->sn_data);
	rdb_write_integer(terrain->colour.current);
	rdb_write_integer(terrain->gore_level);

	terrain->sn_save = TerrainSN++;
}


#ifdef QQQ

/*
 * writes a terrain pointer
 */
static void terrain_write_pointer(const TERRAIN *terrain)
{

	if (terrain == NULL) {

		rdb_write_string("");

		return;
	}

	rdb_write_integer(terrain->sn_save);
}

#endif


/*
 * reads a terrain
 */
static TERRAIN * terrain_read(void)
{
	TERRAIN *terrain;
	SERIAL sn;

	rdb_next_field();

	/* if the terrain is a NULL terrain.. */
	if (is_empty_string(rdb_data_token(0))){

		/* .. return NULL */
		return NULL;
	}

	/* recreate terrain */
	sn = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	terrain = terrain_create_i(sn);
	terrain->colour.current = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	terrain->gore_level = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	/* add it to the terrain pointer restoration table */
	TerrainRestore[TerrainSN++] = terrain;

	return terrain;
}


#ifdef QQQ

/*
 * reads a terrain pointer
 */
static TERRAIN * terrain_read_pointer(void)
{
	const char *data_token = rdb_data_token(RDB_NEXT_TOKEN);

	if (is_empty_string(data_token)) {

		return NULL;
	}

	return TerrainRestore[atoi(data_token)];
}

#endif


/*
 * writes an object
 */
static void object_write(OBJECT *object)
{

	/* if the object is a NULL object.. */
	if (object == NULL) {

		/* write NULL entry and return */
		rdb_write_field_name("O");
		rdb_write_string("");
		return;
	}

	/* write object data */
	rdb_write_field_name("O");
	rdb_write_integer(object->sn_data);
	rdb_write_integer(object->colour.current);
	rdb_write_integer(object->gore_level);
	rdb_write_integer(object->condition);
	rdb_write_integer(object->charge);
	rdb_write_field_name("A");
	bool_array_write(object->attribute, MAX_OBJECT_ATTRIBUTES);

	object->sn_save = ObjectSN++;
}



/*
 * writes an object pointer
 */
static void object_write_pointer(const OBJECT *object)
{

	if (object == NULL) {

		rdb_write_string("");

		return;
	}

	rdb_write_integer(object->sn_save);
}



/*
 * reads an object
 */
static OBJECT * object_read(void)
{
	OBJECT *object;
	SERIAL sn;

	rdb_next_field();

	/* if the object is a NULL object.. */
	if (is_empty_string(rdb_data_token(0))){

		/* .. return NULL */
		return NULL;
	}

	/* recreate the object */
	sn = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	object = object_create_i(sn);
	object->colour.current = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	object->gore_level = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	object->condition = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	object->charge = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	rdb_next_field();
	bool_array_read(object->attribute, MAX_OBJECT_ATTRIBUTES);

	/* add it to the object pointer restoration table */
	ObjectRestore[ObjectSN++] = object;

	return object;
}



/*
 * reads an object pointer
 */
static OBJECT * object_read_pointer(void)
{
	const char *data_token = rdb_data_token(RDB_NEXT_TOKEN);

	if (is_empty_string(data_token)) {

		return NULL;
	}

	return ObjectRestore[atoi(data_token)];
}



/*
 * writes a character
 */
static void character_write(CHARACTER *character)
{

	/* if the character is a NULL character.. */
	if (character == NULL) {

		/* write NULL entry and return */
		rdb_write_field_name("C");
		rdb_write_string("");
		return;
	}

	/* write character data */
	character_write_attributes(character);
	character_write_inventory(character);

	character->sn_save = CharacterSN++;
}



/*
 * writes a character's attributes (i.e. everything except his inventory)
 */
static void character_write_attributes(const CHARACTER *character)
{

	rdb_write_field_name("C");
	rdb_write_string(character->name);
	rdb_write_string(character->script);
	rdb_write_integer(character->type);
	rdb_write_integer(character->controller);
	rdb_write_integer(character->party);
	rdb_write_integer(character->faction);
	rdb_write_integer(character->numerus);
	rdb_write_integer(character->symbol);
	rdb_write_integer(character->colour);
	rdb_write_integer(character->gender);
	career_pointer_write(character->career);
	rdb_write_integer(character->ep);
	rdb_write_integer(character->ep_total);
	rdb_write_integer(character->power_level);
	rdb_write_integer(character->action_spent);
	rdb_write_integer(character->bloody_feet);
	rdb_write_integer(character->armour_rating.current);
	rdb_write_integer(character->armour_rating.total);
	rdb_write_integer(character->injury);
	rdb_write_integer(character->sn_data);

	area_point_write(&character->location);

	ai_data_write(&character->ai);

	character_write_stats(character->stat);

	rdb_write_field_name("F");
	bool_array_write(character->attribute, MAX_CHARACTER_ATTRIBUTES);

	rdb_write_field_name("K");
	bool_array_write(character->perk, MAX_PERKS);

	rdb_write_field_name("Y");
	bool_array_write(character->psy_power, MAX_PSY_POWERS);
}



/*
 * writes a character's stats
 */
static void character_write_stats(const CHARACTER_STAT *p)
{
	STAT stat;

	rdb_write_field_name("S");

	iterate_over_stats(stat) {

		rdb_write_integer(p[stat].current);
		rdb_write_integer(p[stat].total);
		rdb_write_integer(p[stat].advance);
	}
}



/*
 * writes a character's inventory
 */
static void character_write_inventory(CHARACTER *character)
{
	LIST_NODE *node;

	/* write number of objects */
	rdb_write_field_name("I");
	rdb_write_integer(character->inventory->n_nodes);

	/* write objects */
	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {
		OBJECT *object = node->data;

		object_write(object);
	}

	/* write equipment pointers */
	rdb_write_field_name("E");
	object_write_pointer(character->weapon);
	object_write_pointer(character->secondary_weapon);
	object_write_pointer(character->armour);
	object_write_pointer(character->jump_pack);
}



/*
 * writes a character pointer
 */
static void character_write_pointer(const CHARACTER *character)
{

	if (character == NULL) {

		rdb_write_string("");

		return;
	}

	rdb_write_integer(character->sn_save);
}



/*
 * reads a character
 */
static CHARACTER * character_read(void)
{
	CHARACTER *character;

	rdb_next_field();

	/* if the character is a NULL character.. */
	if (is_empty_string(rdb_data_token(0))){

		/* .. return NULL */
		return NULL;
	}

	/* recreate character */
	character = character_new();
	character_read_attributes(character);
	character_read_inventory(character);
	character_recreate_status(character);

	/* add him to the character pointer restoration table */
	CharacterRestore[CharacterSN++] = character;

	/* add him to the character list of the area */
	list_add(area_character_list(), character);

	return character;
}



/*
 * reads a character's attributes (i.e. everything except his inventory)
 */
static void character_read_attributes(CHARACTER *character)
{

	strcpy(character->name, rdb_data_token(RDB_NEXT_TOKEN));
	strcpy(character->script, rdb_data_token(RDB_NEXT_TOKEN));
	character->type = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->controller = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->party = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->faction = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->numerus = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->symbol = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->colour = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->gender = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->career = career_pointer_read();
	character->ep = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->ep_total = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->power_level = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->action_spent = (bool)atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->bloody_feet = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->armour_rating.current = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->armour_rating.total = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->injury = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	character->sn_data = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	area_point_read(&character->location);

	ai_data_read(&character->ai);

	character_read_stats(character->stat);

	rdb_next_field();
	bool_array_read(character->attribute, MAX_CHARACTER_ATTRIBUTES);

	rdb_next_field();
	bool_array_read(character->perk, MAX_PERKS);

	rdb_next_field();
	bool_array_read(character->psy_power, MAX_PSY_POWERS);
}



/*
 * reads a character's stats
 */
static void character_read_stats(CHARACTER_STAT *p)
{
	STAT stat;

	rdb_next_field();

	iterate_over_stats(stat) {

		p[stat].current = atoi(rdb_data_token(RDB_NEXT_TOKEN));
		p[stat].total = atoi(rdb_data_token(RDB_NEXT_TOKEN));
		p[stat].advance = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	}
}



/*
 * reads a character's inventory
 */
static void character_read_inventory(CHARACTER *character)
{
	N_OBJECTS n_objects;

	/* read number of objects */
	rdb_next_field();
	n_objects = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	/* read objects */
	for ( ; n_objects > 0; n_objects--) {
		OBJECT *object = object_read();

		list_add(character->inventory, object);
	}

	/* read equipment pointers */
	rdb_next_field();
	character->weapon = object_read_pointer();
	character->secondary_weapon = object_read_pointer();
	character->armour = object_read_pointer();
	character->jump_pack = object_read_pointer();
}



/*
 * recreates a character's status
 */
static void character_recreate_status(CHARACTER *character)
{

	if (character->type == CT_PC) {

		set_player_character(character);
	}

	if (character->controller == CC_PLAYER) {

		set_player_controlled_character(character);
	}

	if (character->party == PARTY_PLAYER) {

		list_add(party_player(), character);
	}
}



/*
 * reads a character pointer
 */
static CHARACTER * character_read_pointer(void)
{
	const char *data_token = rdb_data_token(RDB_NEXT_TOKEN);

	if (is_empty_string(data_token)) {

		return NULL;
	}

	return CharacterRestore[atoi(data_token)];
}



/*
 * writes the area events
 */
static void events_write(void)
{
	N_EVENTS n_events = area_event_list()->n_nodes;
	LIST_NODE *node;

	rdb_write_field_name("N");
	rdb_write_integer(n_events);

	for (node = area_event_list()->head;
		node != NULL;
		node = node->next) {
		const EVENT *event = node->data;

		event_write(event);
	}
}



/*
 * writes an event
 */
static void event_write(const EVENT *event)
{
	STAT stat;

	rdb_write_field_name("E");
	rdb_write_integer(event->type);
	rdb_write_integer(event->timer);

	character_write_pointer(event->character);
	object_write_pointer(event->object);

	rdb_write_integer(event->drug);
	rdb_write_integer(event->psy_power);
	rdb_write_integer(event->effect);

	rdb_write_integer(event->power);

	iterate_over_stats(stat) {

		rdb_write_integer(event->stat_modifier[stat]);
	}
}



/*
 * reads the area events
 */
static void events_read(void)
{
	AREA *area = active_area();
	N_EVENTS n_events;

	rdb_next_field();
	n_events = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	for ( ; n_events > 0; n_events--) {
		EVENT *event = event_read();

		list_add(area->events, event);
	}
}


/*
 * reads an event
 */
static EVENT * event_read(void)
{
	EVENT *event = checked_malloc(sizeof *event);
	STAT stat;

	rdb_next_field();

	event->type = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	event->timer = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	event->character = character_read_pointer();
	event->object = object_read_pointer();

	event->drug = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	event->psy_power = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	event->effect = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	event->power = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	iterate_over_stats(stat) {

		event->stat_modifier[stat] = atoi(
			rdb_data_token(RDB_NEXT_TOKEN)
		);
	}

	return event;
}





/*
 * writes the save file version
 */
static void version_write(void)
{

	rdb_write_field_name("V");
	rdb_write_string(GAME_VERSION);
	rdb_write_string(scenario_version());
}



/*
 * reads the save file version, and returns true
 * if the file is compatible
 */
static bool file_is_compatible(void)
{

	rdb_next_field();

	if (strings_equal(GAME_VERSION, rdb_data_token(RDB_NEXT_TOKEN)) &&
		strings_equal(scenario_version(),
			rdb_data_token(RDB_NEXT_TOKEN))) {

		return true;
	}

	return false;
}



/*
 * writes the game difficulty
 */
static void difficulty_write(void)
{

	rdb_write_field_name("D");

	rdb_write_integer(game_difficulty());
}



/*
 * reads the game difficulty
 */
static void difficulty_read(void)
{
	GAME_DIFFICULTY difficulty;

	rdb_next_field();

	difficulty = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	game_difficulty_set(difficulty);
}



/*
 * writes the visible area
 */
static void visible_area_write(void)
{

	area_section_write(get_visible_area());
}



/*
 * reads the visible area
 */
static void visible_area_read(void)
{
	AREA_SECTION section;

	area_section_read(&section);

	set_visible_area(&section);
}



/*
 * writes the macros
 */
static void macros_write(void)
{
	int i, j;

	for (i = 0; i < MAX_MACROS; i++) {
		KEY_CODE *macro = get_macro(i);

		rdb_write_field_name("M");

		for (j = 0; j < MAX_MACRO_LENGTH; j++) {

			rdb_write_integer(macro[j]);

			if (macro[j] == MACRO_TERMINATION_KEY) {
				break;
			}
		}
	}
}



/*
 * reads the macros
 */
static void macros_read(void)
{
	int i, j;

	for (i = 0; i < MAX_MACROS; i++) {
		KEY_CODE macro[MAX_MACRO_LENGTH];

		rdb_next_field();

		for (j = 0; j < MAX_MACRO_LENGTH; j++) {

			macro[j] = atoi(rdb_data_token(RDB_NEXT_TOKEN));

			if (macro[j] == MACRO_TERMINATION_KEY) {
				break;
			}
		}

		set_macro(i, macro);
	}
}



/*
 * writes the quests
 */
static void quests_write(void)
{
	LIST *qlist = quest_list();
	LIST_NODE *node;

	rdb_write_field_name("N_QUESTS");
	rdb_write_integer(qlist->n_nodes);

	for (node = qlist->head; node != NULL; node = node->next) {
		const QUEST *quest = node->data;

		rdb_write_field_name("QUEST");
		rdb_write_string(quest->name);
		rdb_write_string(quest->status);
	}
}



/*
 * reads the quests
 */
static void quests_read(void)
{
	LIST *qlist = quest_list();
	N_QUESTS n_quests;

	rdb_next_field();
	n_quests = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	for ( ; n_quests > 0; --n_quests) {
		QUEST *quest;

		quest = quest_new();

		rdb_next_field();

		strcpy(quest->name, rdb_data_token(RDB_NEXT_TOKEN));
		strcpy(quest->status, rdb_data_token(RDB_NEXT_TOKEN));

		list_add(qlist, quest);
	}
}




/*
 * writes the faction relationships
 */
static void faction_relationships_write(void)
{
	N_FACTIONS n = n_factions();
	FACTION y, x;

	rdb_write_field_name("N_FACTIONS");
	rdb_write_integer(n);

	for (y = FACTION_LB; y < n; y++) {
		
		rdb_write_field_name("FR");
		
		for (x = FACTION_LB; x < n; x++) {
			
			rdb_write_integer(faction_relationship(y, x));
		}
	}
}



/*
 * reads the faction relationships
 */
static void faction_relationships_read(void)
{
	N_FACTIONS n;
	FACTION y, x;

	rdb_next_field();
	n = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	for (y = FACTION_LB; y < n; y++) {
		
		rdb_next_field();
		
		for (x = FACTION_LB; x < n; x++) {
			FACTION_RELATIONSHIP r = atoi(
				rdb_data_token(RDB_NEXT_TOKEN)
			);

			faction_set_relationship(y, x, r);
		}
	}
}




/*
 * writes a planet point
 */
static void planet_point_write(const PLANET_POINT *p)
{

	rdb_write_field_name("W");
	rdb_write_integer(p->z);
	rdb_write_integer(p->y);
	rdb_write_integer(p->x);
}



/*
 * reads a planet point
 */
static void planet_point_read(PLANET_POINT *p)
{

	rdb_next_field();
	p->z = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	p->y = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	p->x = atoi(rdb_data_token(RDB_NEXT_TOKEN));
}



/*
 * writes an area point
 */
static void area_point_write(const AREA_POINT *p)
{

	rdb_write_integer(p->y);
	rdb_write_integer(p->x);
}



/*
 * reads an area point
 */
static void area_point_read(AREA_POINT *p)
{

	p->y = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	p->x = atoi(rdb_data_token(RDB_NEXT_TOKEN));
}



/*
 * writes an area section
 */
static void area_section_write(const AREA_SECTION *section)
{

	rdb_write_field_name("R");
	rdb_write_integer(section->top);
	rdb_write_integer(section->bottom);
	rdb_write_integer(section->left);
	rdb_write_integer(section->right);
}



/*
 * reads an area section
 */
static void area_section_read(AREA_SECTION *section)
{

	rdb_next_field();
	section->top = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	section->bottom = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	section->left = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	section->right = atoi(rdb_data_token(RDB_NEXT_TOKEN));
}



/*
 * writes AI data
 */
static void ai_data_write(const AI_CONTROL_DATA *p)
{

	rdb_write_integer(p->state);
	rdb_write_integer(p->default_state);
	rdb_write_integer(p->original_state);

	rdb_write_integer(p->tactic);

	area_point_write(&p->target_point);
}



/*
 * reads AI data
 */
static void ai_data_read(AI_CONTROL_DATA *p)
{

	p->state = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	p->default_state = atoi(rdb_data_token(RDB_NEXT_TOKEN));
	p->original_state = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	p->tactic = atoi(rdb_data_token(RDB_NEXT_TOKEN));

	area_point_read(&p->target_point);
}



/*
 * writes a bool array
 */
static void bool_array_write(const bool *p, int n_items)
{
	int i;

	for (i = 0; i < n_items; i++) {

		rdb_write_integer(p[i]);
	}
}



/*
 * reads a bool array
 */
static void bool_array_read(bool *p, int n_items)
{
	int i;

	for (i = 0; i < n_items; i++) {

		p[i] = (bool)atoi(rdb_data_token(RDB_NEXT_TOKEN));
	}
}



/*
 * writes a career pointer
 */
static void career_pointer_write(const CAREER *career)
{

	if (career == NULL) {

		rdb_write_string("");
		return;
	}

	rdb_write_integer(career->sn);
}



/*
 * reads a career pointer
 */
static CAREER * career_pointer_read(void)
{
	const char *data_token = rdb_data_token(RDB_NEXT_TOKEN);

	if (is_empty_string(data_token)) return NULL;

	return get_career_pointer(atoi(data_token));
}


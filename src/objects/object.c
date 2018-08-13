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
 * Module: Object
 */

#include "wrogue.h"





static OBJECT_INDEX	name_to_object_index(const char *);

static void *		object_new_full(void);
static void		object_free_full(void *);




/*
 * object type names
 */
static const char	ObjectTypeName[MAX_OBJECT_TYPES]
				[OBJECT_TYPE_NAME_SIZE] = {

	"Close combat weapon",
	"Ranged combat weapon",

	"Armour",

	"Drug",

	"Environment",

	"Money",

	"Jump pack"

};



/*
 * object subtype names
 */
static const char	ObjectSubtypeName[MAX_OBJECT_SUBTYPES]
				[OBJECT_SUBTYPE_NAME_SIZE] = {

	"Knife",

	"Pistol",
	"Basic",

	"Light",
	"Medium",
	"Heavy",

	"Elevator",
	"Way up",
	"Way down"

};


/*
 * the object box
 */
static BOX *			ObjectBox = NULL;

/*
 * direct link to object data for fast accesss
 */
static void **			Object;



/*
 * object template (default values)
 */
static const OBJECT ObjectTemplate = {

	/* GORE_LEVEL */ GORE_LEVEL_ZERO, 
	/* CONDITION */ CONDITION_INDESTRUCTABLE,
	/* CHARGE */ CHARGE_NIL,
	/* NAME */ NULL, /* NUMERUS */ NUMERUS_SINGULAR, 
	/* SYMBOL */ '!', /* COLOUR */ {C_NIL, C_NIL, C_NIL, 0}, 
	/* DESCRIPTION */ NULL, 
	/* ATTRIBUTES */ {false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,false,
	false,false,false,false},
	/* CONDITION_MAX */ CONDITION_INDESTRUCTABLE,
	/* CHARGE_MAX */ CHARGE_NIL, /* TYPE */ OTYPE_NIL,
	/* SUBTYPE */ OSTYPE_NIL, /* WEIGHT */ WEIGHT_NIL,
	/* VALUE */ VALUE_NIL, /* DAMAGE */ {0, 0, 0},
	/* FIRING_MODE_DATA */ {{false, false, false}, 0, 0, 0, 0},
	/* RELOAD_VALUE */ RELOAD_NO, /* REACH */ 0, /* PARRY_PENALTY */ 0,
	/* ARMOUR_VALUE */ 0, 
	/* SN_DATA */ SERIAL_NIL, /* SN_SAVE */ SERIAL_NIL

};



/*
 * Object module init
 */
void object_init(void)
{

	ObjectBox = box_new(object_new_full, object_free_full,
		OBJECT_BOX_BUFFER_SIZE
	);
}



/*
 * Object module clean up
 */
void object_clean_up(void)
{

	if (ObjectBox != NULL) {

		box_free(ObjectBox);
	}
}



/*
 * adds an object to the object box
 */
OBJECT * object_box_new_object(void)
{

	return box_new_item(ObjectBox);
}



/*
 * optimises the object box
 */
void object_box_optimise(void)
{

	box_optimise(ObjectBox);

	Object = ObjectBox->item;
}



/*
 * returns the current size of the object box
 */
BOX_SIZE object_box_size(void)
{

	return ObjectBox->current_size;
}




/*
 * allocs an object
 */
void * object_new(void)
{
	OBJECT *object;
	
	object = checked_malloc(sizeof *object);
	
	*object = ObjectTemplate;

	return object;
}



/*
 * frees an object
 */
void object_free(void *p)
{

	free(p);
}



/*
 * returns a pointer to the requested object prototype
 */
OBJECT * object_prototype(OBJECT_INDEX object_index)
{

	return Object[object_index];
}



/*
 * creates an object (name based)
 */
OBJECT * object_create(const char *object_name)
{
	OBJECT_INDEX object_index = name_to_object_index(object_name);

	return object_create_i(object_index);
}



/*
 * creates an object (index based)
 */
OBJECT * object_create_i(OBJECT_INDEX object_index)
{
	OBJECT *object = object_clone(Object[object_index]);

	object->sn_data = object_index;

	determine_colour(&object->colour);

	return object;
}



/*
 * destroys an object
 * i.e. free + handling the complex data management mess
 */
void object_destroy(void *p)
{
	OBJECT *object;
	LIST *object_events;

	object = (OBJECT *)p;

	object_events = list_new();

	remove_object(object, object_events);

	list_free_with(object_events, event_destroy);

	object_free(object);
}




/*
 * produces an identical copy of the passed object
 */
OBJECT * object_clone(const OBJECT *object)
{
	OBJECT *clone;

	clone = object_new();

	*clone = *object;

	return clone;
}



/*
 * returns true if the passed object is a connector object
 */
bool object_is_connector(const OBJECT *object)
{

	switch(object->subtype) {
	case OSTYPE_ELEVATOR:
	case OSTYPE_WAY_UP:
	case OSTYPE_WAY_DOWN: return true;
	}

	return false;
}



/*
 * the object screen
 */
void object_screen(const OBJECT *object)
{
	command_bar_set(1, CM_EXIT);

	render_object_screen(object);

	update_screen();

	command_bar_get_command();
}



/*
 * returns the name of an object type
 */
const char * object_type_name(OBJECT_TYPE object_type)
{

	return ObjectTypeName[object_type];
}



/*
 * returns the name of an object subtype
 */
const char * object_subtype_name(OBJECT_SUBTYPE object_subtype)
{

	return ObjectSubtypeName[object_subtype];
}



/*
 * name -> object type
 */
OBJECT_TYPE name_to_object_type(const char *name)
{
	OBJECT_TYPE i;

	for (i = 0; i < MAX_OBJECT_TYPES; i++) {

		if (strings_equal(name, ObjectTypeName[i])) {

			return i;
		}
	}

	die("*** CORE ERRROR *** invalid object type: %s", name);

	/* NEVER REACHED */
	return OTYPE_NIL;
}



/*
 * name -> object subtype
 */
OBJECT_SUBTYPE name_to_object_subtype(const char *name)
{
	OBJECT_SUBTYPE i;

	for (i = 0; i < MAX_OBJECT_SUBTYPES; i++) {

		if (strings_equal(name, ObjectSubtypeName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid object subtype: %s", name);

	/* NEVER REACHED */
	return OSTYPE_NIL;
}



/*
 * name -> object index
 */
static OBJECT_INDEX name_to_object_index(const char *name)
{
	OBJECT_INDEX i;

	for (i = 0; i < ObjectBox->current_size; i++) {
		const OBJECT *object = Object[i];

		if (strings_equal(name, object->name)) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid object: %s", name);

	/* NEVER REACHED */
	return OBJECT_INDEX_NIL;
}



/*
 * allocs an object (full version) 
 */
static void * object_new_full(void)
{
	OBJECT *object;

	object = checked_malloc(sizeof *object);
	
	*object = ObjectTemplate;
	
	object->name = checked_malloc(OBJECT_NAME_SIZE);
	object->description = checked_malloc(DESCRIPTION_SIZE);

	clear_string(object->name);
	clear_string(object->description);

	return object;
}



/*
 * frees an object (full version)
 */
static void object_free_full(void *p)
{
	OBJECT *object = p;

	free(object->description);
	free(object->name);
	free(object);
}



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
 * Module: Character Career
 */

#include "wrogue.h"




#if defined(DEBUG)

static void		career_validate(const CAREER *);

#endif

static void *		career_new(void);
static void		career_free(void *);





/*
 * career type names
 */
static const char	CareerTypeName[MAX_CAREER_TYPES]
				[CAREER_TYPE_NAME_SIZE] = {

	"Basic",
	"Advanced",
	"Elite"

};



/*
 * the career box
 */
static BOX  *		CareerBox = NULL;

/*
 * direct link to career data for fast accesss
 */
static void **		Career;




/*
 * career template (default values)
 */
static const CAREER CareerTemplate = {

	/* NAME */ "",  /* COLOUR */ C_NIL, /* DESCRIPTION */ "",
	/* TYPE */ CAREER_TYPE_NIL,
	/* STAT */ {{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{0, 0}, {0, 0}, {0, 0}, {0, 0}},
	/* PERK */ {{false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false}, 
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false}, 
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}, {false, false}, {false, false}, {false, false},
	{false, false}},
	/* TRAPPINGS */ NULL, /* EXITS */ {"", "", "", "", ""},
	/* SN */ SERIAL_NIL

};



/*
 * Career module init
 */
void career_init(void)
{

	CareerBox = box_new(career_new, career_free,
		CAREER_BOX_BUFFER_SIZE
	);
}



/*
 * Career module clean up
 */
void career_clean_up(void)
{

	if (CareerBox != NULL) {

		box_free(CareerBox);

		return;
	}
}



/*
 * adds a career to the career box
 */
CAREER * career_box_new_career(void)
{

	return box_new_item(CareerBox);
}



/*
 * optimises the career box
 */
void career_box_optimise(void)
{

	box_optimise(CareerBox);

	Career = CareerBox->item;
}



/*
 * sets a character's career
 */
void career_set(CHARACTER *character, CAREER_INDEX career_index)
{
	CAREER *career = get_career_pointer(career_index);

	/* set career */
	character->career = career;
	character->colour = career->colour;

	/* update power level if necessary */
	if (character->power_level < career->type + 1) {

		character->power_level = career->type + 1;
	}
}



/*
 * adds the passed perk to the passed career
 * whether or not the perk will be marked as required depends on
 * the 'required' parameter
 */
void career_set_perk(CAREER *career, PERK perk, bool required)
{
	CAREER_PERK_INFO *perk_info = &career->perk[perk];

	perk_info->available = true;
	perk_info->required = required;
}



/*
 * returns a career pointer
 */
CAREER * get_career_pointer(CAREER_INDEX career_index)
{

	if (career_index == CAREER_NIL) return NULL;

	return Career[career_index];
}



/*
 * returns the number of available careers
 */
N_CAREERS get_n_careers(void)
{

	return CareerBox->current_size;
}



/*
 * checks whether a character has completed his current career or not
 */
bool career_completed(const CHARACTER *character)
{
	STAT stat;

	iterate_over_stats(stat) {

		if (character->stat[stat].advance <
			character->career->stat[stat].advance_maximum) {

			return false;
		}
	}

	return true;
}



/*
 * returns the number of perks required by the passed career
 */
N_PERKS career_n_required_perks(const CAREER *career)
{
	PERK perk; /* the currently processed perk */
	PERK n_perks_required = 0; /* number of required perks */

	/* iterate over all perks */
	iterate_over_perks(perk) {
		
		/* perk required .. */
		if (career_perk_required(career, perk)) {
			
			/* increment required perks counter */
			++n_perks_required;
		}
	}

	/* return number of required perks */
	return n_perks_required;
}



/*
 * the "Career" screen
 */
void career_screen(const CAREER *career)
{

	command_bar_set(1, CM_EXIT);

	render_career_screen(career);

	update_screen();

	command_bar_get_command();
}



/*
 * hands out the trappings of a career to the passed character
 */
void career_give_trappings(CHARACTER *character)
{
	LIST_NODE *node;

	for (node = character->career->trappings->head;
		node != NULL;
		node = node->next) {
		OBJECT *object = object_clone((OBJECT *)node->data);

		inventory_add(character, object);
	}
}



/*
 * name -> career index
 */
CAREER_INDEX name_to_career_index(const char *name)
{
	CAREER_INDEX i;

	for (i = 0; i < CareerBox->current_size; i++) {
		const CAREER *career;

		career = Career[i];

		if (strings_equal(name, career->name)) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid career: %s", name);

	return 0;
}



/*
 * returns the name of a career type
 */
const char * career_type_name(CAREER_TYPE career_type)
{

	return CareerTypeName[career_type];
}



/*
 * name -> career type
 */
CAREER_TYPE name_to_career_type(const char *name)
{
	CAREER_TYPE i;

	for (i = 0; i < MAX_CAREER_TYPES; i++) {

		if (strings_equal(name, CareerTypeName[i])) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid career type: %s", name);

	return 0;
}


#if defined(DEBUG)

/*
 * validates the content of the career box
 */
void career_box_validate(void)
{
	CAREER_INDEX i;

	for (i = 0; i < CareerBox->current_size; i++) {
		const CAREER *career;

		career = Career[i];

		career_validate(career);
	}
}

#endif


#if defined(DEBUG)

/*
 * validates a career
 */
static void career_validate(const CAREER *career)
{
	CAREER_EXIT_INDEX i;

	for (i = 0; i < MAX_CAREER_EXITS; i++) {

		if (is_empty_string(career->exit[i])) {

			break;
		}

		name_to_career_index(career->exit[i]);
	}
}

#endif



/*
 * allocs a career
 */
static void * career_new(void)
{
	CAREER *career;

	career = checked_malloc(sizeof *career);

	*career = CareerTemplate;

	career->trappings = list_new();

	return career;
}



/*
 * frees a career
 */
static void career_free(void *data)
{
	CAREER *career;

	career = (CAREER *)data;

	list_free_with(career->trappings,
		object_destroy
	);

	free(career);
}



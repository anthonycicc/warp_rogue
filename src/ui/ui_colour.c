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
 * Module: UI Colour
 */

#define UI_COLOUR_MODULE

#include "wrogue.h"



/*
 * UI colours
 */
COLOUR C_Text;
COLOUR C_Highlight;
COLOUR C_FieldValue;
COLOUR C_FieldValueIncreased;
COLOUR C_FieldValueReduced;
COLOUR C_FieldName;
COLOUR C_Header;
COLOUR C_HotKey;
COLOUR C_HotKeyBracket;
COLOUR C_SecondaryScreenTitle;
COLOUR C_Warning;
COLOUR C_Injured;
COLOUR C_Cursor;
COLOUR C_AggressiveCursor;
COLOUR C_PsychicCursor;
COLOUR C_Metal;
COLOUR C_Blood;






/*
 * colour defintions
 */
static COLOUR_DATA	Colour[MAX_COLOURS];
static N_COLOURS	N_Colours = 0;



/*
 * UI colours
 */
static UI_COLOUR UiColour[MAX_UI_COLOURS] = {

	{"Text",			&C_Text},
	{"Highlight",			&C_Highlight},
	{"Field value",			&C_FieldValue},
	{"Field value increased",	&C_FieldValueIncreased},
	{"Field value reduced",		&C_FieldValueReduced},
	{"Field name",			&C_FieldName},
	{"Header",			&C_Header},
	{"Hot key",			&C_HotKey},
	{"Hot key bracket",		&C_HotKeyBracket},
	{"Secondary screen title",	&C_SecondaryScreenTitle},
	{"Warning",			&C_Warning},
	{"Injured",			&C_Injured},
	{"Cursor",			&C_Cursor},
	{"Aggressive cursor",		&C_AggressiveCursor},
	{"Psychic cursor",		&C_PsychicCursor},
	{"Metal",			&C_Metal},
	{"Blood",			&C_Blood}

};





/* 
 * adds a new colour
 */
COLOUR_DATA * colour_new(void)
{

	return &Colour[N_Colours++];
}



/*
 * returns the RGB values of a colour
 */
const RGB_DATA * colour_rgb(COLOUR colour_index)
{
	
	return &Colour[colour_index].rgb;
}



/*
 * returns the number of colours
 */
N_COLOURS n_colours(void)
{

	return N_Colours;
}



/*
 * returns a pointer to the requested UI colour
 */
COLOUR * ui_colour(const char *name)
{
	N_COLOURS i;

	for (i = 0; i < MAX_UI_COLOURS; i++) {
	
		if (strings_equal(name, UiColour[i].name)) {
			
			return UiColour[i].colour_ptr;
		}
	}

	die("*** CORE ERROR *** UI colour not found (%s)", name);

	/* NEVER REACHED */
	return NULL;
}



/*
 * determines the passed dynamic colour 
 */
void determine_colour(DYNAMIC_COLOUR *colour)
{

	colour->current = colour->primary;

	if (colour->secondary == C_NIL) return;

	if (random_int(COLOUR_FREQUENCY_MIN, COLOUR_FREQUENCY_MAX) >
		colour->frequency) {

		return;
	}

	colour->current = colour->secondary;
}



/*
 * colour name -> index
 */
COLOUR name_to_colour(const char *colour_name)
{
	COLOUR i;

	for (i = 0; i < N_Colours; i++) {

		if (strings_equal(colour_name, Colour[i].name)) {

			return i;
		}
	}

	die("*** CORE ERROR *** invalid colour: %s", colour_name);

	return C_NIL;
}





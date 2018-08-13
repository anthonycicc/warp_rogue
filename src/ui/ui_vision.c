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
 * Module: UI Vision
 */

#include "wrogue.h"


static void		update_view_horizontal_realign(const AREA_POINT *);
static void		update_view_vertical_realign(const AREA_POINT *);



/*
 * the currently visible area
 */
static AREA_SECTION	VisibleArea = {0, 0, 0, 0};



/*
 * updates the visible area
 */
void update_view(const AREA_POINT *view_point, bool horizontal_realign,
	bool vertical_realign
)
{

	if (horizontal_realign) {

		update_view_horizontal_realign(view_point);
	}


	if (vertical_realign) {

		update_view_vertical_realign(view_point);
	}
}



/*
 * updates the visible area
 */
void update_view_point(const AREA_POINT *view_point)
{
	bool horizontal_realign;
	bool vertical_realign;
	const AREA_SECTION *bounds;

	bounds = area_bounds();
	
	horizontal_realign = vertical_realign = false;

	if ((view_point->y >= VisibleArea.bottom - H_EDGE &&
		VisibleArea.bottom != bounds->bottom) ||
		(view_point->y <= VisibleArea.top + H_EDGE &&
			VisibleArea.top != bounds->top)) {

		horizontal_realign = true;
	}

	if ((view_point->x >= VisibleArea.right - V_EDGE &&
		VisibleArea.right != bounds->right) ||
		(view_point->x <= VisibleArea.left + V_EDGE &&
			VisibleArea.left != bounds->left)) {

		vertical_realign = true;
	}

	update_view(view_point, horizontal_realign, vertical_realign);
}



/*
 * updates the visible area
 */
void update_view_character(const CHARACTER *character)
{
	const AREA_POINT *view_point;

	view_point = &(character->location);

	update_view_point(view_point);
}



/*
 * returns true if the passed point (y, x) is outside of the visible area
 */
bool outside_of_visible_area(const AREA_COORD y, const AREA_COORD x)
{

	if (y < VisibleArea.top || y > VisibleArea.bottom ||
		x < VisibleArea.left || x > VisibleArea.right) {

		return true;
	}

	return false;
}



/*
 * converts game area coordinates to visible area window coordinates
 */
bool vaw_convert_coordinates(SCREEN_COORD *wy, SCREEN_COORD *wx, AREA_COORD y, AREA_COORD x)
{

	if (outside_of_visible_area(y, x)) {

		return false;
	}

	*wx = x - VisibleArea.left + 1;
	*wy = y - VisibleArea.top + 3;

	return true;
}



/*
 * sets the visible area
 */
void set_visible_area(const AREA_SECTION *section)
{

	VisibleArea = *section;
}



/*
 * returns the visible area
 */
const AREA_SECTION * get_visible_area(void)
{

	return &VisibleArea;
}



/*
 * updates the visible area (horizontal realign)
 */
static void update_view_horizontal_realign(const AREA_POINT *view_point)
{
	AREA_COORD diff;
	const AREA_SECTION *bounds;

	bounds = area_bounds();
	
	diff = 0;

	VisibleArea.top = view_point->y - VY_MOD;

	if (VisibleArea.top < bounds->top) {

		diff = bounds->top - VisibleArea.top;

		VisibleArea.top = bounds->top;
	}

	VisibleArea.bottom = view_point->y + VY_MOD + diff;

	if (VisibleArea.bottom > bounds->bottom) {

		diff = VisibleArea.bottom - bounds->bottom;

		VisibleArea.bottom = bounds->bottom;
	}

	VisibleArea.top -= diff;

	if (VisibleArea.top < bounds->top) {

		VisibleArea.top = bounds->top;
	}
}



/*
 * updates the visible area (vertical realign)
 */
static void update_view_vertical_realign(const AREA_POINT *view_point)
{
	AREA_COORD diff;
	const AREA_SECTION *bounds;

	bounds = area_bounds();

	diff = 0;

	VisibleArea.left = view_point->x - VX_MOD;

	if (VisibleArea.left < bounds->left) {

		diff = bounds->left - VisibleArea.left;

		VisibleArea.left = bounds->left;
	}

	VisibleArea.right = view_point->x + VX_MOD + diff;

	if (VisibleArea.right > bounds->right) {

		diff = VisibleArea.right - bounds->right;

		VisibleArea.right = bounds->right;
	}

	VisibleArea.left -= diff;

	if (VisibleArea.left < bounds->left) {

		VisibleArea.left = bounds->left;
	}
}



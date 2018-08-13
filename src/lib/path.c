/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Pathfinder (PTF)
 *
 *
 * NAMESPACE: PTF_* / ptf_*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"


#define ptf_at(y, x)		((y) * PTF_ScanAreaWidth + (x))


static void			ptf_reset(int, int);

static void			ptf_push_adjacent_nodes(const PTF_PATH_NODE *);

static void			ptf_open_stack_push(const PTF_PATH_NODE *);
static const PTF_PATH_NODE *	ptf_open_stack_pop(void);

static const PTF_PATH_NODE *	ptf_total_stack_push(const PTF_PATH_NODE *);

static int			ptf_distance(int, int, int, int);

static int			ptf_out_of_bounds(int, int);


static PTF_PATH_NODE * 	PTF_TotalStack = NULL;
static PTF_PATH_NODE *	PTF_TotalStackPtr = NULL;

static PTF_PATH_NODE *	PTF_OpenStack = NULL;
static PTF_PATH_NODE *	PTF_OpenStackPtr = NULL;

static unsigned char *	PTF_ClosedSet = NULL;

static int		PTF_ScanAreaWidth;
static int		PTF_ScanAreaHeight;
static int		PTF_ScanAreaSize;

static int		PTF_TargetY;
static int		PTF_TargetX;

static int		PTF_SearchDepth;

static int		(*PTF_ObstacleAt)(int, int);


/*
 * PTF setup
 */
void ptf_setup(int height, int width, int search_depth, int (*obstacle_at)(int, int))
{

	PTF_ScanAreaHeight = height;
	PTF_ScanAreaWidth = width;
	PTF_ScanAreaSize = height * width;

	PTF_TotalStack = realloc(PTF_TotalStack, PTF_ScanAreaSize *
		sizeof *PTF_TotalStack
	);

	PTF_OpenStack = realloc(PTF_OpenStack, PTF_ScanAreaSize *
		sizeof *PTF_OpenStack
	);

	PTF_ClosedSet = realloc(PTF_ClosedSet, PTF_ScanAreaSize);

	PTF_TotalStackPtr = PTF_TotalStack;
	PTF_OpenStackPtr = PTF_OpenStack;

	PTF_SearchDepth = search_depth;

	PTF_ObstacleAt = obstacle_at;
}



/*
 * PTF clean up
 */
void ptf_clean_up(void)
{
	free(PTF_TotalStack);
	free(PTF_OpenStack);
	free(PTF_ClosedSet);
}



/*
 * returns the length of a path
 */
int ptf_path_length(const PTF_PATH_NODE *start_node)
{
	const PTF_PATH_NODE *node;
	int length;

	length = 0;

	for (node = start_node;
		node != NULL;
		node = node->parent) {

		++length;
	}

	return length;
}



/*
 * attempts to find the shortest path between two points. if there is
 * a path, the returned path starts at the first step towards the target
 */
const PTF_PATH_NODE * ptf_find_path(int start_y, int start_x,
	int target_y, int target_x)
{
	PTF_PATH_NODE start_node;
	const PTF_PATH_NODE *node;
	int search_depth;

	ptf_reset(start_y, start_x);

	start_node.y = target_y;
	start_node.x = target_x;
	start_node.parent = NULL;

	ptf_open_stack_push(&start_node);
	PTF_ClosedSet[ptf_at(target_y, target_x)] = 1;

	for (node = ptf_open_stack_pop(), search_depth = PTF_SearchDepth;
		node != NULL;
		node = ptf_open_stack_pop()) {

		if (node->y == PTF_TargetY &&
			node->x == PTF_TargetX) {

			return node->parent;
		}

		if (search_depth-- == 0) break;

		node = ptf_total_stack_push(node);

		ptf_push_adjacent_nodes(node);
	}

	return NULL;
}



/*
 * PTF reset
 */
static void ptf_reset(int target_y, int target_x)
{
	PTF_TotalStackPtr = PTF_TotalStack;
	PTF_OpenStackPtr = PTF_OpenStack;
	memset(PTF_ClosedSet, 0, PTF_ScanAreaSize);

	PTF_TargetY = target_y;
	PTF_TargetX = target_x;
}



/*
 * pushes all adjacent nodes on the OPEN stack
 */
static void ptf_push_adjacent_nodes(const PTF_PATH_NODE *node)
{
	int y_mod, x_mod;

	for (y_mod = -1; y_mod <= 1; y_mod++) {
	for (x_mod = -1; x_mod <= 1; x_mod++) {
		PTF_PATH_NODE adjacent_node;
		int i;

		if (x_mod == 0 && y_mod == 0) {
			
			continue;
		}

		adjacent_node.y = node->y + y_mod;
		adjacent_node.x = node->x + x_mod;

		if (ptf_out_of_bounds(adjacent_node.y, adjacent_node.x)) {

			continue;
		}

		i = ptf_at(adjacent_node.y, adjacent_node.x);

		if (PTF_ClosedSet[i]) {
			
			continue;
		}

		PTF_ClosedSet[i] = 1;

		if ((*PTF_ObstacleAt)(adjacent_node.y, adjacent_node.x)) {
			
			continue;
		}

		adjacent_node.parent = node;

		ptf_open_stack_push(&adjacent_node);
	}
	}
}



/*
 * pushes a node on the OPEN stack
 */
static void ptf_open_stack_push(const PTF_PATH_NODE *new_node)
{
	PTF_PATH_NODE *node;
	int node_distance;

	*PTF_OpenStackPtr = *new_node;

	node = PTF_OpenStackPtr;

	++PTF_OpenStackPtr;

	node_distance = ptf_distance(node->y, node->x,
		PTF_TargetY, PTF_TargetX
	);

	while (node != PTF_OpenStack) {
		PTF_PATH_NODE *node_below;
		int node_below_distance;

		node_below = node - 1;

		node_below_distance = ptf_distance(node_below->y,
			node_below->x, PTF_TargetY, PTF_TargetX
		);

		if (node_below_distance < node_distance) {
			PTF_PATH_NODE tmp;

			tmp = *node;
			*node = *node_below;
			*node_below = tmp;

			--node;
		} else {
			break;
		}

	}

}



/*
 * pops a node from the OPEN stack
 */
static const PTF_PATH_NODE * ptf_open_stack_pop(void)
{

	if (PTF_OpenStackPtr == PTF_OpenStack) {
		return NULL;
	}

	--PTF_OpenStackPtr;

	return PTF_OpenStackPtr;
}



/*
 * pushes a node on the TOTAL stack
 */
static const PTF_PATH_NODE * ptf_total_stack_push(const PTF_PATH_NODE *node)
{
	PTF_PATH_NODE *ret;

	*PTF_TotalStackPtr = *node;

	ret = PTF_TotalStackPtr;

	++PTF_TotalStackPtr;

	return ret;
}



/*
 * returns the distance between two points
 */
static int ptf_distance(int y1, int x1, int y2, int x2)
{
	int distance, yd, xd;

	yd = y1 - y2;
	if (yd < 0) yd = -yd;

	xd = x1 - x2;
	if (xd < 0) xd = -xd;

	distance = yd + xd;

	return distance;
}



/*
 * returns 1 if the passed point is out of bounds
 * or 0 if it isn't
 */
static int ptf_out_of_bounds(int y, int x)
{
	if (y < 0 || y >= PTF_ScanAreaHeight ||
		x < 0 || x >= PTF_ScanAreaWidth) {

		return 1;
	}

	return 0;
}



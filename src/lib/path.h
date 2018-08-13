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


typedef struct ptf_path_node_struct	PTF_PATH_NODE;

/*
 * path node
 */
 struct ptf_path_node_struct {
	
	const PTF_PATH_NODE *	parent;
	
	int			y;
	int			x;
	
};



void			ptf_setup(int, int, int, int (*)(int, int));
void			ptf_clean_up(void);

int			ptf_path_length(const PTF_PATH_NODE *);

const PTF_PATH_NODE *	ptf_find_path(int, int, int, int);


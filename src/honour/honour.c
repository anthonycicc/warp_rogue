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
 * Module: Honour
 */

#include "wrogue.h"




static void		certificate_print(void);

static void		print_basics(FILE *, const CHARACTER *);
static void		print_stats(FILE *, const CHARACTER *);
static void		print_perks(FILE *, const CHARACTER *);
static void		print_psy_powers(FILE *, const CHARACTER *);
static void		print_inventory(FILE *, const CHARACTER *);
static void		print_quests(FILE *);




/*
 * the certificate of honour screen
 */
void certificate_screen(void)
{

	certificate_print();

	command_bar_set(1, CM_EXIT);
	render_certificate_screen();
	update_screen();

	command_bar_get_command();
}



/*
 * prints the certificate of honour
 */
static void certificate_print(void)
{
	FILE *out_file;
	const CHARACTER *character;

	character = player_character();
	
	set_data_path(DIR_USER_DATA, FILE_CERTIFICATE);
	
	out_file = open_file(data_path(), "w");

	fprintf(out_file, "              -== Certificate of Honour ==-\n\n");

	print_basics(out_file, character);
	
	print_stats(out_file, character);

	print_perks(out_file, character);

	print_psy_powers(out_file, character);

	print_inventory(out_file, character);

	print_quests(out_file);
	
	fprintf(out_file, "\n\n");
	
	close_file(out_file);
}



/*
 * prints the basics
 */
static void print_basics(FILE *out_file, const CHARACTER *character)
{

	fprintf(out_file, "Name         : %s\n", character->name);

	fprintf(out_file, "Career       : ");
	
	if (character->career == NULL) {
		
		fprintf(out_file, "-\n");
		
	} else {

		fprintf(out_file, "%s\n", character->career->name);
	}

	fprintf(out_file, "Power Level  : %d\n\n", character->power_level);
}



/*
 * prints the stats
 */
static void print_stats(FILE *out_file, const CHARACTER *character)
{
	STAT stat;
	
	iterate_over_stats(stat) {

		fprintf(out_file, "%-13s: %d\n", stat_long_name(stat),
			character->stat[stat].current
		);
	}
}



/*
 * prints the perks
 */
static void print_perks(FILE *out_file, const CHARACTER *character)
{
	PERK perk;

	fprintf(out_file, "\n[Perks]\n");
	
	iterate_over_perks(perk) {
		
		if (character_has_perk(character, perk)) {

			fprintf(out_file, "%s\n", perk_name(perk));
		}
	}
}



/*
 * prints the psychic powers
 */
static void print_psy_powers(FILE *out_file, const CHARACTER *character)
{
	PSY_POWER power;

	fprintf(out_file, "\n[Psychic Powers]\n");
	
	iterate_over_psy_powers(power) {

		if (!character_has_psy_power(character, power)) continue;
		
		fprintf(out_file, "%s\n", psy_power_name(power));
	}
}



/*
 * prints the inventory
 */
static void print_inventory(FILE *out_file, const CHARACTER *character)
{
	LIST_NODE *node;

	fprintf(out_file, "\n[Inventory]\n");
	
	for (node = character->inventory->head;
		node != NULL;
		node = node->next) {

		inventory_object_print(character, (OBJECT *)node->data,
			out_file
		);
	}
}



/*
 * prints the quests
 */
static void print_quests(FILE *out_file)
{
	LIST_NODE *node;
	
	fprintf(out_file, "\n[Quests]\n");

	iterate_over_list(quest_list(), node) {
		QUEST *quest = node->data;
		
		if (is_empty_string(quest->status)) continue;

		fprintf(out_file, "%-32s %16s\n", quest->name, quest->status);
	}
}






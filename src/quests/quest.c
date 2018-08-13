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
 * Module: Quest
 */

#include "wrogue.h"





static QUEST *		quest_add(const char *);
static QUEST *		quest_find(const char *);





/*
 * the quest list
 */
static LIST *		QuestList = NULL;



/*
 * quest template (default values)
 */
static const QUEST	QuestTemplate = {

	/* NAME */ "", /* STATUS */ ""

};




/*
 * quest module init
 */
void quest_init(void)
{

	QuestList = list_new();
}



/*
 * quest module clean up
 */
void quest_clean_up(void)
{

	if (QuestList != NULL) list_free_with(QuestList, quest_free);
}



/*
 * returns the quest list
 */
LIST * quest_list(void)
{

	return QuestList;
}



/*
 * allocs a quest
 */
void * quest_new(void)
{
	QUEST *quest;
	
	quest = checked_malloc(sizeof *quest);
	
	*quest = QuestTemplate;
	
	return quest;
}



/*
 * frees a quest
 */
void quest_free(void *data)
{

	free(data);
}



/*
 * sets the status of the passed quest
 */
void quest_set_status(const char *quest_name, const char *status)
{
	QUEST *quest;
	
	quest = quest_find(quest_name);
	
	if (quest == NULL) {

		quest = quest_add(quest_name);
	}

	strcpy(quest->status, status);
}



/*
 * returns the status of the passed quest
 */
const char * quest_status(const char *quest_name)
{
	QUEST *quest;
	
	quest = quest_find(quest_name);
	
	if (quest == NULL) {

		return "";
	}

	return quest->status;
}



/*
 * adds a quest to the quest list
 * returns a pointer to the added quest
 */
static QUEST * quest_add(const char *quest_name)
{
	QUEST *quest;
	
	quest = quest_new();
	
	strcpy(quest->name, quest_name);
	
	list_add(QuestList, quest);
	
	return quest;
}



/*
 * searches the quest list in an attempt to find passed quest
 * returns a pointer to the quest, or NULL if the quest could
 * not be found
 */
static QUEST * quest_find(const char *quest_name)
{
	LIST_NODE *node;

	for (node = QuestList->head; node != NULL; node = node->next) {
		QUEST *quest;
		
		quest = (QUEST *)node->data;
		
		if (strings_equal(quest->name, quest_name)) {
			
			return quest;
		}
	}
	
	return NULL;
}


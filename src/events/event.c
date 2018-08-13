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
 * Module: Event
 */

#include "wrogue.h"



/*
 * resets the event queue
 */
#define event_queue_reset()	EventQueueNextNode = area_event_list()->head;



static EVENT *		event_queue_next_event(void);

static void 		event_queue_remove(const EVENT *);




/*
 * event template (default values)
 */
static const EVENT EventTemplate = {
	
	/* TYPE */ EVT_NIL, /* TIMER */ TIMER_NIL,
	/* CHARACTER */ NULL, /* OBJECT */ NULL, /* DRUG */ DRUG_NIL,
	/* PSY_POWER */ PSY_NIL, /* EFFECT */ ET_NIL, /* POWER */ 0,
	/* STAT_MODIFIER */ {0, 0, 0, 0, 0, 0, 0, 0}

};
	



/*
 * event execution functions
 */
static void (* const EventFunction[MAX_EVENT_TYPES])(EVENT *) = {

	reload_weapon,
	recharge_weapon,
	unjam_weapon,

	effect_terminate_event,

	psy_power_terminate,

	drug_terminate,

	poison_damage,

};



/*
 * the event which is currently being executed
 */
static EVENT *		ExecutedEvent = NULL;



/*
 * the next node in the event queue
 */
static LIST_NODE *	EventQueueNextNode = NULL;




/*
 * allocs an event
 */
void * event_new(void)
{
	EVENT *event = checked_malloc(sizeof(EVENT));
	
	*event = EventTemplate;

	return event;
}



/*
 * frees an evnet
 */
void event_free(void *p)
{

	free(p);
}



/*
 * creates an event
 */
EVENT * event_create(EVENT_TYPE type, TIMER timer)
{
	EVENT *event = event_new();

	event->type = type;
	event->timer = timer;

	return event;
}



/*
 * destroys an event
 * i.e. free + handling the complex data management mess
 */
void event_destroy(void *p)
{

	event_free(p);
}



/*
 * sets an event
 */
void event_set(EVENT *event)
{

	list_add(area_event_list(), event);
}



/*
 * handles events
 */
void handle_events(void)
{
	EVENT *event;

	event_queue_reset();

	while ((event = event_queue_next_event()) != NULL) {

		if (event->timer == TIMER_UNDETERMINED) {

			continue;
		}

		if (event->timer == TIMER_CONTINUOUS) {

			event_execute(event);

			continue;
		}

		event->timer -= 1;

		if (event->timer > 0) {

			continue;
		}

		event_execute(event);
	}
}



/*
 * executes an event
 */
void event_execute(EVENT *event)
{

	ExecutedEvent = event;

	(*EventFunction[event->type])(event);

	if (ExecutedEvent == NULL) {

		return;
	}

	if (event->timer != TIMER_CONTINUOUS) {

		event_queue_remove(event);

		event_destroy(event);
	}

	ExecutedEvent = NULL;
}



/*
 * removes a specific character event
 */
EVENT * remove_character_event(const CHARACTER *character, EVENT_TYPE event_type)
{
	EVENT *event;

	event_queue_reset();

	while ((event = event_queue_next_event()) != NULL) {

		if (event->type == event_type &&
			event->character == character) {

			event_queue_remove(event);

			event_queue_reset();

			return event;
		}
	}

	event_queue_reset();

	return NULL;
}



/*
 * removes all events which contain a reference to the passed character
 */
void remove_character_events(const CHARACTER *character, LIST *character_events)
{
	EVENT *event;

	event_queue_reset();

	while ((event = event_queue_next_event()) != NULL) {

		if (event->character != character) {

			continue;
		}

		event_queue_remove(event);

		list_add(character_events, event);
	}

	event_queue_reset();
}



/*
 * remove all events which contain a reference to the passed object
 */
void remove_object_events(const OBJECT *object, LIST *object_events)
{
	EVENT *event;

	event_queue_reset();

	while ((event = event_queue_next_event()) != NULL) {

		if (event->object != object) {

			continue;
		}

		event_queue_remove(event);

		list_add(object_events, event);
	}

	event_queue_reset();
}



/*
 * remove all events which contain a reference to the passed terrain
 * object
 */
void remove_terrain_events(const TERRAIN *terrain, LIST *terrain_events)
{

	/* no terrain events yet */

	NOT_USED(terrain);
	NOT_USED(terrain_events);
}



/*
 * returns a pointer to a specific effect termination event
 */
EVENT * effect_termination_event(CHARACTER *character, EFFECT effect)
{
	EVENT *event;
	bool event_found;

	event_queue_reset();

	event_found = false;

	while ((event = event_queue_next_event()) != NULL) {

		if (event->type != EVT_EFFECT_TERMINATION) {

			continue;
		}

		if (event->character == character &&
			event->effect == effect) {

			event_found = true;

			break;
		}
	}

	event_queue_reset();

	if (!event_found) {

		return NULL;
	}

	return event;
}



/*
 * returns the next event in the event queue
 */
static EVENT * event_queue_next_event(void)
{
	EVENT *event;

	if (EventQueueNextNode == NULL) {

		return NULL;
	}

	event = (EVENT *)EventQueueNextNode->data;

	EventQueueNextNode = EventQueueNextNode->next;

	return event;
}



/*
 * removes an event from the event queue
 */
static void event_queue_remove(const EVENT *event)
{

	if (EventQueueNextNode != NULL &&
		event == EventQueueNextNode->data) {

		EventQueueNextNode = EventQueueNextNode->next;
	}

	if (event == ExecutedEvent) {

		ExecutedEvent = NULL;
	}

	list_remove(area_event_list(), event);
}




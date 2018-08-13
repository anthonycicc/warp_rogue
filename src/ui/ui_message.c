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
 * Module: UI Message
 */

#include "wrogue.h"




static void	game_screen_message_clear_buffer(void);
static void	game_screen_message_more_prompt(void);



static char	MessageBuffer[MESSAGE_BUFFER_SIZE];
static int	MessageBufferLength;




/*
 * outputs a game screen message
 */
void game_screen_message(char *msg)
{
	int message_length;

	if (msg == NULL) {
		game_screen_message_clear_buffer();
		return;
	}

	message_length = strlen(msg) + 1;

	if (MessageBufferLength + message_length + (MORE_PROMPT_LENGTH + 1) <=
		MESSAGE_WINDOW_SIZE) {

		/* everything's fine = nothing to do */

	} else if (message_length <= MESSAGE_WINDOW_SIZE) {

		/* time for 'more' */
		game_screen_message_more_prompt();
		game_screen_message_clear_buffer();

	} else {

		/* very long message - break it down into words */
		int word_length;
		char *word;

		word = strtok(msg, " ");
		while (word != NULL) {

			word_length = strlen(word) + 1;

			if (MessageBufferLength + word_length +
				(MORE_PROMPT_LENGTH + 1) > MESSAGE_WINDOW_SIZE) {

				/* time for 'more' */
				game_screen_message_more_prompt();
				game_screen_message_clear_buffer();
			}

			strcat(MessageBuffer, word);
			strcat(MessageBuffer, " ");
			MessageBufferLength += word_length;

			word = strtok(NULL, " ");

		}

		render_message_window();
		return;
	}


	strcat(MessageBuffer, msg);
	strcat(MessageBuffer, " ");
	MessageBufferLength += message_length;

	render_message_window();
	return;
}



/*
 * flushes the game screen message buffer
 */
void game_screen_message_flush(void)
{

	game_screen_message_more_prompt();
	game_screen_message_clear_buffer();
}



/*
 * returns the game screen message buffer
 */
char * game_screen_message_buffer(void)
{
	return MessageBuffer;
}



/*
 * clears the game screen message buffer
 */
static void game_screen_message_clear_buffer(void)
{
	clear_string(MessageBuffer);
	MessageBufferLength = 0;
	render_message_window();
}



/*
 * game screen message more prompt
 */
static void game_screen_message_more_prompt(void)
{
	strcat(MessageBuffer, MORE_PROMPT);

	render_game_screen();
	update_screen();
	get_key();

}




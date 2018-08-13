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
 * platform code stub
 *
 * you can use this as a template if you want to implement support for
 * a new platform
 *
 */

#include "wrogue.h"



/*
 * platform init
 */
void platform_init(void)
{

}



/*
 * platform clean up
 */
void platform_clean_up(void)
{

}




/*
 * returns the path of a file
 */
char * get_file_path(char *path, PROGRAM_DIRECTORY directory,
	const char *file_name
)
{

	NOT_USED(path);
	NOT_USED(directory);
	NOT_USED(file_name);

	return NULL;
}



/*
 * returns all .rdb files in the passed directory
 */
LIST * data_files(PROGRAM_DIRECTORY directory)
{

	NOT_USED(directory);

	return NULL;
}



/*
 * waits n seconds
 */
void sec_sleep(int n_seconds)
{

	NOT_USED(n_seconds);
}



/*
 * platform UI init
 */
void ui_init(void)
{

}



/*
 * platform UI clean up
 */
void ui_clean_up(void)
{

}



/*
 * returns true if the fullscreen mode is activated
 */
bool fullscreen_mode(void)
{

	return true;
}



/*
 * changes the screen mode
 */
void change_screen_mode(void)
{


}



/*
 * updates the screen
 */
void update_screen(void)
{

}



/*
 * clears the screen
 */
void clear_screen(void)
{

}



/*
 * places the cursor at the specified location
 */
void place_cursor_at(SCREEN_COORD y, SCREEN_COORD x)
{

	NOT_USED(y);
	NOT_USED(x);
}



/*
 * returns the current Y coordinate of the cursor
 */
SCREEN_COORD cursor_y(void)
{
	return 0;
}



/*
 * returns the current X coordinate of the cursor
 */
SCREEN_COORD cursor_x(void)
{

	return 0;
}



/*
 * renders the cursor at its current position
 */
void render_cursor(COLOUR colour)
{

	NOT_USED(colour);
}



/*
 * renders a character at the current cursor position
 */
void render_char(COLOUR colour, SYMBOL ch)
{

	NOT_USED(colour);
	NOT_USED(ch);
}



/*
 * renders a character at the specified position
 */
void render_char_at(COLOUR colour, SCREEN_COORD y, SCREEN_COORD x, SYMBOL c)
{

	NOT_USED(colour);
	NOT_USED(y);
	NOT_USED(x);
	NOT_USED(c);
}



/*
 * get key
 */
KEY_CODE lowlevel_get_key(void)
{

	return 0;
}



/*
 * clears a section of the screen for text display
 */
void clear_text_window(SCREEN_COORD y1, SCREEN_COORD x1,
	SCREEN_COORD y2, SCREEN_COORD x2
)
{

	NOT_USED(y1);
	NOT_USED(x1);
	NOT_USED(y2);
	NOT_USED(x2);
}



/*
 * renders the background
 */
void render_background(void)
{

}



/*
 * main
 */
int main(int argc, char *argv[])
{

	NOT_USED(argc);
	NOT_USED(argv);

	game_run();

	return 0;
}



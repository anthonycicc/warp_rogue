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
 * Module: Log
 */

#include "wrogue.h"



/* log file pointer */
static FILE *	LogFile = NULL;




/*
 * Log module init
 */
void log_init(void)
{

	/* create new log file */
	set_data_path(DIR_USER_DATA, FILE_LOG);
	LogFile = open_file(data_path(), "w");
}



/*
 * Log module clean up 
 */
void log_clean_up(void)
{

	/* close log file */
	close_file(LogFile);
}



/*
 * outputs a log message
 */
void log_output(const char *fmt, ...)
{
	char msg[STRING_BUFFER_SIZE];
	va_list vl;

	va_start(vl, fmt);
	vsprintf(msg, fmt, vl);
	va_end(vl);
	
	fprintf(LogFile, "%s\n", msg);
}



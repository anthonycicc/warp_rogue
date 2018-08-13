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
 * Microsoft Windows platform code
 */

/*
 * Most of the needed platform functionality is actually provided by 
 * the portable SDL platform
 */

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <shlobj.h>

#include "wrogue.h"



/* user data directory path */
static char UserDataPath[FILE_PATH_SIZE] = "";



/*
 * program directories
 */
static const char *	ProgramDirectory[MAX_PROGRAM_DIRECTORIES] = {

	"./data/",	
	
	"./data/dungeons/",
	"./data/characters/",
	"./data/characters/scripts/",
	"./data/galaxy/",
	"./data/info/",
	"./data/info/help/",
	"./data/objects/",
	"./data/planets/",
	"./data/races/",
	"./data/rules/",
	"./data/terrain/",
	"./data/ui/",

	UserDataPath

};



/*
 * platform init
 */
void platform_init(void)
{

	/* get user data path */		
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | 
		CSIDL_FLAG_CREATE, NULL, 0, UserDataPath))) {
			
		strcat(UserDataPath, "\\");
		strcat(UserDataPath, GAME_NAME);
		
		/* create user data directory if necessary */
		CreateDirectory(UserDataPath, NULL);
		
		strcat(UserDataPath, "\\");

	} else {

		die_no_data_path("*** PL_WIN ERROR *** "
			"could not create user data directory"
		);
	}				
}



/*
 * platform clean up
 */
void platform_clean_up(void)
{

	/* NOTHING TO DO */
}



/*
 * returns the path of a file
 */
char * get_file_path(char *path, PROGRAM_DIRECTORY directory, 
	const char *file_name
)
{

	strcpy(path, ProgramDirectory[directory]);
	
	strcat(path, file_name);

	return path;
}



/*
 * returns all .rdb files in the passed directory
 */
LIST * data_files(PROGRAM_DIRECTORY directory)
{
	HANDLE fHandle;
	WIN32_FIND_DATA wfd;
	char path[FILE_PATH_SIZE];
	LIST *file_list;
	
	file_list = list_new();
	
	get_file_path(path, directory, "*.rdb");

	fHandle = FindFirstFile(path, &wfd);

	do {
		char *file_name;

		/* ignore '.' and '..' */ 
		if (strcmp(wfd.cFileName, ".") == 0 || 
			strcmp(wfd.cFileName, "..") == 0) {

			continue;
		}

		/* ignore directories */
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			continue;
		}
			
		file_name = checked_malloc(FILE_NAME_SIZE * 
			sizeof *file_name
		);

		strcpy(file_name, wfd.cFileName);
		
		list_add(file_list, file_name);
	}

	while (FindNextFile(fHandle,&wfd));

	FindClose(fHandle);

	return file_list;
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



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
 * UNIX platform code
 */

/*
 * most of the needed platform functionality is actually provided by
 * the portable SDL platform
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>


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
	char *home;
	struct stat sb;

	/* get user data path */
	home = getenv("HOME");
	if (home == NULL) {
		die_no_data_path("*** PL_UNIX ERROR *** "
			"no HOME environment variable set"
		);
	}
	strcpy(UserDataPath, home);
	strcat(UserDataPath, "/.wrogue/");

	/* everything is fine, directory exists, finished */
	if(stat(UserDataPath, &sb) == 0) return;

	/* something went wrong, abort */
	if (errno != ENOENT) {

		die_no_data_path("*** PL_UNIX ERROR *** " \
			"could not access '%s' (%s)",
			UserDataPath, strerror(errno)
		);

		return;
	}

	/* create user data directory */
	if(mkdir(UserDataPath, S_IRUSR | S_IWUSR | S_IXUSR) != 0) {

		die_no_data_path("*** PL_UNIX ERROR *** " \
			"could not create directory '%s' (%s)",
			UserDataPath, strerror(errno)
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
	DIR *d;
	struct dirent *ent;
	char *file_name;
	char *file_ext;
	LIST *file_list;
	
	file_list = list_new();

	if (NULL == (d = opendir(ProgramDirectory[directory]))) {

		return file_list;
	}

	while (NULL != (ent = readdir(d))) {
		
		/* ignore dot-files */
		if (ent->d_name[0] == '.') {
			continue;
		}

		/* ignore directories */
		if (ent->d_type == DT_DIR) {
			continue;
		}
		
		/* Ignore non-.rdb files */
		file_ext = strrchr(ent->d_name, '.');
		if (NULL == file_ext || strncmp(file_ext, ".rdb", 4))
		{

			continue;
		}

		file_name = checked_malloc(FILE_NAME_SIZE);

		strcpy(file_name, ent->d_name);

		list_add(file_list, file_name);
	}

	closedir(d);

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

	return EXIT_SUCCESS;
}


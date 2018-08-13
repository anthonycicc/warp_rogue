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
 * Mac OS X platform code
 */

/*
 * Most of the needed platform functionality is actually provided by 
 * the portable SDL platform
 */


#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <CoreServices/CoreServices.h>

#include <SDL/SDL.h>


#include "wrogue.h"



/*
 * program directories
 */
static const char *	ProgramDirectory[MAX_PROGRAM_DIRECTORIES] = {

	"%s/%s",

	"%s/dungeons/%s",
	"%s/characters/%s",
	"%s/characters/scripts/%s",
	"%s/galaxy/%s",
	"%s/info/%s",
	"%s/info/help/%s",
	"%s/objects/%s",
	"%s/planets/%s",
	"%s/races/%s",
	"%s/rules/%s",
	"%s/terrain/%s",
	"%s/ui/%s",

	NULL

};


static char *app_resources_path = NULL;
static char *user_app_support_path = NULL;


int mac_check_paths();




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
	struct stat s;

    if (!mac_check_paths())
    {
        return NULL;
    }

    if (DIR_USER_DATA == directory)
    {
        /*
         * File doesn't need to exist, and will always be written
         * in app support
         */

        snprintf(path, FILE_PATH_SIZE, ProgramDirectory[directory],
            user_app_support_path, file_name);

        return path;
    }
    else
    {
        /*
         *  Look first in app support for a customized file
         */
        snprintf(path, FILE_PATH_SIZE, ProgramDirectory[directory],
            user_app_support_path, file_name);

        if (!stat(path, &s))
        {
            return path;
        }

        /*
         * Not found there, look in the app bundle
         */
        snprintf(path, FILE_PATH_SIZE, ProgramDirectory[directory],
            app_resources_path, file_name);

        if (!stat(path, &s))
        {
            return path;
        }
    }

    die_no_data_path("*** BE_MAC ERROR *** file not found");

    return NULL;
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
	char path[FILE_PATH_SIZE];
	LIST *file_list;
	
	file_list = list_new();
	
	if (!mac_check_paths()) {

		return file_list;
	}

	/*
	 * Try the app resources dir first
	 */
	snprintf(path, FILE_PATH_SIZE, ProgramDirectory[directory],
		user_app_support_path, "");

	if (NULL == (d = opendir(path))) {
		
		/*
		 * No joy, read it from the app bundle instead
		 */
		snprintf(path, FILE_PATH_SIZE, ProgramDirectory[directory],
			app_resources_path, "");

		if (NULL == (d = opendir(path))) {
			
			/*
			 * Still no joy, return the empty list
			 */
			return file_list;
		}
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

		asprintf(&file_name, "%s", ent->d_name);

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

	return 0;
}

int mac_check_paths()
{
	FSRef fsref;
	CFBundleRef bundleref;
	struct stat s;
	char path[FILE_PATH_SIZE];

	if (NULL == app_resources_path)
	{
        /*
         * If the path to the application's Contents/Resources directory
         * hasn't been found yet, find it.
         */
		if ((bundleref = CFBundleGetMainBundle()) != NULL) 
		{
            CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundleref);
			if (url)
			{
                if (CFURLGetFSRef(url, &fsref))
                {
                    if (!FSRefMakePath( &fsref, (UInt8 *)path,
                        FILE_PATH_SIZE))
                    {
                        asprintf(&app_resources_path, "%s", path);
                    }
                }

                CFRelease(url);
            }
		}
		
		/*
		 * If it still hasn't been found, whine and bail
		 */
        if (NULL == app_resources_path)
        {
            die_no_data_path("*** BE_MAC ERROR *** could not find Resources");
            return 0;
        }
    }

    if (NULL == user_app_support_path) {
        /*
         * If the path to ~Library/Application Support/ENGINE NAME
         * hasn't been found yet, find it.
         */

		if (!FSFindFolder(kUserDomain, 
			kApplicationSupportFolderType, 
			kDontCreateFolder, &fsref))
        {

			if (!FSRefMakePath( &fsref, (UInt8 *)path, 
				FILE_PATH_SIZE ))
            {
                asprintf(&user_app_support_path, "%s/%s", path, GAME_NAME);

                if (stat(user_app_support_path, &s))
                {
                    mkdir(user_app_support_path, 0700);
                }

                snprintf(path, FILE_PATH_SIZE, "%s/scenario",
                    user_app_support_path);
                if (stat(path, &s))
                {
                    mkdir(path, 0700);
                }

                snprintf(path, FILE_PATH_SIZE, "%s/scenario/user",
                    user_app_support_path);
                if (stat(path, &s))
                {
                    mkdir(path, 0700);
                }
            }
        }

		/*
		 * If it still hasn't been found, whine and bail
		 */
        if (NULL == user_app_support_path)
        {
            die_no_data_path("*** BE_MAC ERROR *** could not find app support");
            return 0;
        }
    }

    return 1;
}

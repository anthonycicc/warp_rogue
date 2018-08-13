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
 * Module: Script
 */

#include "wrogue.h"




static void		script_add_magic_variables(void);






/*
 * misc. character pointers for scripts
 */
static CHARACTER *	CharacterSelf = NULL;
static CHARACTER *	CharacterActiveCharacter = NULL;





/*
 * Script module init
 */
void script_init(void)
{

	wca_start();
	
	script_add_functions();
	
	script_add_magic_variables();
}



/*
 * Script module clean up
 */
void script_clean_up(void)
{

	wca_end();
}



/*
 * loads a script
 */
bool script_load(PROGRAM_DIRECTORY directory, const char *file_name)
{

	set_data_path(directory, file_name);
	
	return wca_load(data_path());
}



/*
 * executes a script
 */
void script_execute(void)
{

	/* reset all script functions */ 
	randomiser_clear();
	dungeon_reset();
	cave_reset();

	/* execute the loaded script */
	wca_execute();
}



/*
 * sets the value of a magic variable
 */
void script_set_data(const char *m_variable, const char *string)
{

	wca_magic_variable_set(m_variable, string);
}



/*
 * sets the value of a magic variable (numeric version)
 */
void script_set_data_numeric(const char *m_variable, int n)
{
	char tmp[STRING_BUFFER_SIZE];
	
	sprintf(tmp, "%d", n);

	wca_magic_variable_set(m_variable, tmp);
}



/*
 * hack: handles magic variables which refer to C pointers
 */
void * script_pointer(WCA_STORAGE_INDEX param)
{
	const char *string;
	void *ptr;
	
	string = wca_string(param);

	if (strings_equal(string, "SELF_PTR")) {

		ptr = *script_pointer_self();
		
	} else if (strings_equal(string, "ACTIVE_CHARACTER_PTR")) {

		ptr = *script_pointer_active_character();

	} else {
		
		ptr = NULL;
	}

	return ptr;
}



/*
 * returns a pointer to the $SELF character pointer
 */
CHARACTER ** script_pointer_self(void)
{

	return &CharacterSelf;
}



/*
 * returns a pointer to the $ACTIVE_CHARACTER pointer
 */
CHARACTER ** script_pointer_active_character(void)
{

	return &CharacterActiveCharacter;
}



/*
 * script bool -> C bool
 */
bool script_bool_to_C_bool(WCA_STORAGE_INDEX param)
{
	const char *script_bool = wca_string(param);

	if (strings_equal(script_bool, SCRIPT_TRUE)) {
		
		return true;
		
	} else if (strings_equal(script_bool, SCRIPT_FALSE)) {

		return false;

	} else {

		die("*** WCA ERROR *** boolean conversion failed");
	}
	
	/* never reached */
	return false;
}





/*
 * adds the Warp Rogue specific magic variables to the interpreter
 */
static void script_add_magic_variables(void)
{

	wca_magic_variable_add("SELF");
	wca_magic_variable_add("ACTIVE_CHARACTER");
	
	wca_magic_variable_add("LEVEL");
	wca_magic_variable_add("GENERATION");

	wca_magic_variable_add("TRIGGERED");
	wca_magic_variable_add("KILLED");	
}











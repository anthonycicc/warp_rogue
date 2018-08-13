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
 *
 * Warp Rogue Constants
 *
 */




/*
 *
 * Game Constants
 *
 */

/*
 * game name
 */
#define GAME_NAME	"Warp Rogue"

/*
 * game version
 */
#define GAME_VERSION	"0.8.0"

/*
 * game website
 */
#define GAME_WEBSITE	"http://todoom.sourceforge.net"

/*
 * credits file
 */
#define FILE_CREDITS	"credits.txt"

/*
 * difficulty info file
 */
#define FILE_DIFFICULTY_INFO	"modes.txt"

/*
 * game start script file
 */
#define FILE_START_SCRIPT	"start.wca"




/*
 *
 * Misc. Constants
 *
 */
#define MAX_PROGRAM_DIRECTORIES		14

#define FILE_PATH_SIZE			256
#define FILE_NAME_SIZE			48

#define	STRING_BUFFER_SIZE		200
#define TEXT_BUFFER_SIZE		16000
#define LINE_BUFFER_SIZE           	16000

#define DESCRIPTION_SIZE		512


#define MAX_REAL_DIRECTIONS		8




/*
 *
 * LoadSave Constants
 *
 */

/*
 * game state file
 */
#define FILE_GAME_STATE		"state.rdb"

/*
 * save point area file
 */
#define FILE_SAVE_POINT_AREA	"sp_area.rdb"


/*
 *
 * Data File Constants
 *
 */

/* config files */
#define FILE_KEYBINDINGS		"keys.rdb"

/* scenario info file */
#define FILE_SCENARIO_INFO		"scenario.rdb"

/* colour file */
#define FILE_COLOUR			"colours.rdb"

/* planet file */
#define FILE_PLANET			"proelium.rdb"

/* start file */
#define FILE_START			"start.rdb"

/* factions file */
#define FILE_FACTIONS			"factions.rdb"

/* race profile file */
#define FILE_RACE_PROFILE		"profile.rdb"

/* career file */
#define FILE_CAREERS			"careers.rdb"

/* gore file */
#define FILE_GORE			"gore.rdb"

/* description files */
#define FILE_PSY_POWERS			"powers.rdb"
#define FILE_PERKS			"perks.rdb"
#define FILE_CHARACTER_ATTRIBUTES	"character_attributes.rdb"
#define FILE_OBJECT_ATTRIBUTES		"object_attributes.rdb"
#define FILE_TERRAIN_ATTRIBUTES		"terrain_attributes.rdb"




/*
 *
 * Log File Constants
 *
 */

/* log file */
#define FILE_LOG			"logfile.txt"





/*
 *
 * UI Constants
 *
 */
#define MAX_ASCII_SYMBOLS		95
#define MAX_CUSTOM_SYMBOLS		14
#define MAX_SYMBOLS			(MAX_ASCII_SYMBOLS + \
						MAX_CUSTOM_SYMBOLS)


#define MAX_KEY_STACK			20
#define MAX_MACROS			12
#define MAX_MACRO_LENGTH		MAX_KEY_STACK

#define MACRO_TERMINATION_KEY		'$'

#define SYMBOL_OFFSET		32
#define CUSTOM_SYMBOL_OFFSET	(SYMBOL_OFFSET + MAX_ASCII_SYMBOLS)

#define TEXT_ROWS			37
#define TEXT_COLUMNS			100

#define SCREEN_MIN_Y			1
#define SCREEN_MIN_X			1
#define SCREEN_MAX_Y			(TEXT_ROWS - 2)
#define SCREEN_MAX_X			(TEXT_COLUMNS - 2)

#define SEC_SCREEN_TITLE_ROW		1
#define SEC_SCREEN_MIN_Y		3
#define SEC_SCREEN_MIN_X		1
#define SEC_SCREEN_MAX_Y		(TEXT_ROWS - 4)
#define SEC_SCREEN_MAX_X		(TEXT_COLUMNS - 2)
#define SEC_SCREEN_COMMAND_ROW		(TEXT_ROWS - 2)

#define GAME_SCREEN_MESSAGE_ROW		1
#define GAME_SCREEN_MIN_X		1
#define GAME_SCREEN_MAX_X		(TEXT_COLUMNS - 2)
#define GAME_SCREEN_MIN_Y		1
#define GAME_SCREEN_MAX_Y		(TEXT_ROWS - 2)

#define MAX_COLOURS		128
#define COLOUR_NAME_SIZE	32


#define MAX_UI_COLOURS		17
#define UI_COLOUR_NAME_SIZE	32

#define MAX_COMMANDS			63
#define COMMAND_NAME_SIZE	32

/* gore feature name size */
#define GORE_FEATURE_NAME_SIZE	16


#define ACTION_WINDOW_WIDTH	68
#define ACTION_WINDOW_HEIGHT	24

#define ACTION_WINDOW_MIN_Y	3
#define ACTION_WINDOW_MIN_X	GAME_SCREEN_MIN_X
#define ACTION_WINDOW_MAX_Y	(ACTION_WINDOW_MIN_Y + ACTION_WINDOW_HEIGHT)
#define ACTION_WINDOW_MAX_X	(ACTION_WINDOW_MIN_X + ACTION_WINDOW_WIDTH)

#define PARTY_WINDOW_MIN_Y	(ACTION_WINDOW_MAX_Y + 2)

#define PARTY_WINDOW_MIN_X	GAME_SCREEN_MIN_X
#define PARTY_WINDOW_MAX_Y	GAME_SCREEN_MAX_Y

#define LOCATION_WINDOW_MIN_Y	PARTY_WINDOW_MIN_Y

#define CHARACTER_WINDOW_WIDTH	20


#define INFO_BAR_WIDTH		(CHARACTER_WINDOW_WIDTH + 6)
#define INFO_BAR_MIN_X		(ACTION_WINDOW_MAX_X + 3)
#define INFO_BAR_MAX_X		(INFO_BAR_MIN_X + INFO_BAR_WIDTH)


/* maximal number of gore features */
#define MAX_GORE_FEATURES	7

#define PLANET_MAP_MIN_Y			3
#define PLANET_MAP_MIN_X			GAME_SCREEN_MIN_X
#define PLANET_MAP_MAX_Y			GAME_SCREEN_MAX_Y
#define PLANET_MAP_MAX_X			GAME_SCREEN_MIN_X + 68

#define PLANET_INFO_MIN_Y		PLANET_MAP_MIN_Y
#define PLANET_INFO_MIN_X		PLANET_MAP_MAX_X + 4
#define PLANET_INFO_MAX_X		PLANET_MAP_MAX_X + 28

#define PLANET_POSITION_Y		GAME_SCREEN_MAX_Y - 5

/*
 * key codes
 */
#define KEY_NIL		-1
#define KEY_ESC		27
#define KEY_ENTER	13
#define KEY_BKSP	8
#define KEY_TAB		9
#define KEY_SPACE	32
#define KEY_DELETE	127
#define KEY_UP		273
#define KEY_DOWN	274
#define KEY_RIGHT	275
#define KEY_LEFT	276
#define KEY_F1		282
#define KEY_F2		283
#define KEY_F3		284
#define KEY_F4		285
#define KEY_F5		286
#define KEY_F6		287
#define KEY_F7		288
#define KEY_F8		289
#define KEY_F9		290
#define KEY_F10		291
#define KEY_F11		292
#define KEY_F12		293
#define KEY_PAGE_UP	294
#define KEY_PAGE_DOWN	295


#define N_SPECIAL_KEYS		20


#define MESSAGE_WINDOW_SIZE	(SEC_SCREEN_MAX_X - SEC_SCREEN_MIN_X)

#define MESSAGE_BUFFER_SIZE	256

#define MORE_PROMPT		"-more-"
#define MORE_PROMPT_LENGTH	6


#define SYMBOL_NAME_SIZE	16


/*
 * visible area config
 */
#define VX_MOD		34
#define VY_MOD		12
#define H_EDGE		3
#define V_EDGE		4





/*
 *
 * Dice Constants
 *
 */
#define D100_AUTOMATIC_SUCCESS			5
#define D100_AUTOMATIC_FAILURE			96






/*
 *
 * Randomiser Constants
 *
 */

/* randomiser string size */
#define RANDOMISER_STRING_SIZE		128







/*
 *
 * Random Name Constants
 *
 */

/*
 * random name files
 */
#define FILE_NAMES_MALE		"male.nam"
#define FILE_NAMES_FEMALE	"female.nam"





/*
 *
 * Planet Constants
 *
 */

#define PLANET_HEIGHT		33
#define PLANET_WIDTH		69

#define PLANET_SURFACE_Z		0

#define PLANET_NAME_SIZE		20

/*
 * planet tile box buffer size
 */
#define PLANET_TILE_BOX_BUFFER_SIZE	16





/*
 *
 * Area Constants
 *
 */
#define AREA_NAME_SIZE		20

#define	AREA_HEIGHT		64
#define	AREA_WIDTH		128
#define AREA_SIZE		(AREA_HEIGHT * AREA_WIDTH)

#define AREA_MAX_CHARACTERS	8000
#define AREA_MAX_OBJECTS	32000



/*
 *
 * Sector Constants
 *
 */
#define MAX_SECTOR_CLASSES	9




/*
 *
 * Cave Area Generator Constants
 *
 */
#define CAVE_Y_MIN		2
#define CAVE_X_MIN		8
#define CAVE_Y_MAX		(AREA_HEIGHT - CAVE_Y_MIN - 1)
#define CAVE_X_MAX		(AREA_WIDTH - CAVE_X_MIN - 1)

#define CAVE_GATE_HEIGHT	5
#define CAVE_GATE_SIDE_EX	2

#define CAVE_HEIGHT		(AREA_HEIGHT - CAVE_Y_MIN * 2)
#define CAVE_WIDTH		(AREA_WIDTH - CAVE_X_MIN * 2)




/*
 *
 * Dungeon Area Generator Constants
 *
 */
#define MIN_POOLS		2
#define MAX_POOLS		6
#define MIN_POOL_EXTENSION	0
#define MAX_POOL_EXTENSION	5

#define DUNGEON_Y_MIN		1
#define DUNGEON_X_MIN		8
#define DUNGEON_Y_MAX		61
#define DUNGEON_X_MAX		118

#define DUNGEON_GATE_HEIGHT	5
#define DUNGEON_GATE_SIDE_EX	2



/*
 *
 * Faction Constants
 *
 */
#define FACTION_NAME_SIZE		64

#define MAX_FACTION_RELATIONSHIPS	3
#define FACTION_RELATIONSHIP_NAME_SIZE	16




/*
 *
 * Psychic Constants
 *
 */
#define PSYCHIC_RESISTANCE_MODIFIER	1




/*
 *
 * Psychic Power Constants
 *
 */
#define PSY_TERRIFY_DURATION			4
#define PSY_PSYCHIC_SHRIEK_DURATION		2
#define PSY_PSYCHIC_SHIELD_DURATION		(dice(1, 50) + 50)
#define PSY_PSYCHIC_SHIELD_POWER		dice(1, 10)






/*
 *
 * AI Constants
 *
 */

/*
 * the AI will only attempt to destroy an obstacle if
 * its condition is less than or equal to this value
 */
#define AI_OBSTACLE_DESTRUCTION_MAX	100


/*
 * the maximal distance between the AI and its target the AI will
 * tolerate while in "Follow" mode
 */
#define AI_MAX_FOLLOW_DISTANCE		1


/*
 * maximal number of AI states
 */
#define MAX_AI_STATES			7



/*
 * AI decision probabilities (1% - 100% chance)
 */
#define AI_SWITCH_TO_GUARD_PROBABILITY		2
#define AI_SWITCH_TO_PATROL_PROBABILITY		25


/*
 * maximal number of AI tactics
 */
#define MAX_AI_TACTICS			4


/*
 * AI tactic name size
 */
#define AI_TACTIC_NAME_SIZE		20


#define MAX_AI_OPTIONS			3




/*
 *
 * Object Constants
 *
 */

/*
 * misc. object related contants
 */
#define	MONEY_OBJECT_NAME		"Credits"

#define OBJECT_BOX_BUFFER_SIZE		64

#define OBJECT_TYPE_NAME_SIZE		32
#define OBJECT_SUBTYPE_NAME_SIZE	16




/*
 *
 * Object Attribute Constants
 *
 */

#define OBJECT_ATTRIBUTE_NAME_SIZE	32

#define	POWER_WEAPON_ARMOUR_VALUE_DIVIDER	2
#define	SHOCK_STUN_POWER			1
#define	FORCE_DAMAGE_BONUS			dice(1, 10)
#define	FORCE_RUNE_DAMAGE_BONUS			dice(2, 8)
#define	FORCE_VS_DAEMON_MULTIPLIER		2



/*
 *
 * Object Drug Constants
 *
 */
#define MAX_DRUGS			6

#define DRUG_NAME_SIZE			16

#define DEFAULT_DRUG_DURATION		(dice(1, 50) + 50)









/*
 *
 * Character Constants
 *
 */
#define CHARACTER_NAME_SIZE		32


#define CHARACTER_BOX_BUFFER_SIZE	64





/*
 *
 * Character Attribute Constants
 *
 */
#define MAX_CHARACTER_ATTRIBUTES	19

#define CHARACTER_ATTRIBUTE_NAME_SIZE	32




/*
 *
 * Character Advancement Constants
 *
 */
#define EP_LIMIT				3000

#define EP_COST_INCREASE_STAT			100
#define EP_COST_BUY_PERK			100
#define EP_COST_LEARN_PSY_POWER			100
#define EP_COST_CHANGE_CAREER_FOLLOW_PATH	100
#define EP_COST_CHANGE_CAREER_NEW_PATH		200

#define STANDARD_STAT_ADVANCE			5

/*
 * EP rewards
 */
#define EP_REWARD_VERY_EASY			5
#define EP_REWARD_EASY				10
#define EP_REWARD_ROUTINE			15
#define EP_REWARD_AVERAGE			20
#define EP_REWARD_CHALLENGING			30
#define EP_REWARD_HARD				40
#define EP_REWARD_VERY_HARD			50





/*
 *
 * Character Career Constants
 *
 */
#define MAX_CAREER_TYPES		3

#define CAREER_BOX_BUFFER_SIZE		32

#define CAREER_TYPE_NAME_SIZE		16

#define CAREER_NAME_SIZE		25

#define MAX_CAREER_EXITS		6




/*
 *
 * Character Perk Constants
 *
 */

#define MAX_PERKS			38

#define MAX_CHARACTER_PERKS		10

#define PERK_NAME_SIZE			25

#define RAPID_SHOT_SA_PENALTY_DIVIDER			2

#define MARKSMAN_RANGE_PENALTY_DIVIDER			5

#define CRUSHING_STRIKE_STRENGTH_BONUS_MULTIPLIER	2

#define ENHANCED_PARRY_BONUS				10

#define	BATTLE_TACTICS_BONUS				20

#define	UNCANNY_DODGE_MODIFIER				-20

#define FIRST_AID_RECOVERY_BONUS			50

#define SPOT_WEAKNESS_ARMOUR_VALUE_DIVIDER		2




/*
 *
 * Character Stat Constants
 *
 */
#define STAT_BONUS_BASE		40

#define STAT_SHORT_NAME_SIZE	4
#define STAT_LONG_NAME_SIZE	16

#define MAX_STATS		8




/*
 *
 * Character Equipment Constants
 *
 */
#define	STRENGTH_BOOST_BONUS		10
#define	STRENGTH_BOOST_2_BONUS		20




/*
 *
 * Character Inventory Constants
 *
 */
#define	MAX_INVENTORY_OBJECTS		20





/*
 *
 * Character Movement Constants
 *
 */
#define	MAX_JUMP_PACK_RANGE		12

#define MAX_PATROL_TARGET_DISTANCE	20

#define BLOODY_FEET_FACTOR		2

#define RUN_FACTOR_DIVISOR		30




/*
 *
 * Character Race Constants
 *
 */
#define MALE_CHARACTER_PROBABILITY	75



/*
 *
 * Character Talk Constants
 *
 */
#define DIALOGUE_TEXT_SIZE		1000




/*
 *
 * Character Death Constants
 *
 */
#define DEATH_DROP_PROBABILITY		50

#define	SPLATTER_MIN_N_BODY_PARTS	0
#define	SPLATTER_MAX_N_BODY_PARTS	3

#define MAX_GORE_LEVELS			7

#define GORE_LEVEL_NAME_SIZE		20





/*
 *
 * Object Constants
 *
 */
#define OBJECT_NAME_SIZE		25

#define MAX_OBJECT_TYPES		7
#define MAX_OBJECT_SUBTYPES		9
#define MAX_OBJECT_ATTRIBUTES		31

#define MAX_FIRING_MODES		3





/*
 *
 * Pathfinder Constants
 *
 */
#define PATHFINDER_MAX_PATH		128

#define PATHFINDER_MAX_SHORT_PATH	30





/*
 *
 * Terrain Constants
 *
 */
#define TERRAIN_NAME_SIZE		20

#define TERRAIN_BOX_BUFFER_SIZE		32




/*
 *
 * Terrain Attribute Constants
 *
 */
#define MAX_TERRAIN_ATTRIBUTES		5

#define TERRAIN_ATTRIBUTE_NAME_SIZE	32





/*
 *
 * Party Constants
 *
 */
#define MAX_PARTY_SIZE		3






/*
 *
 * Effect Constants
 *
 */
#define N_EFFECTS				4

#define EFFECT_NAME_SIZE			20

#define POISON_DAMAGE				1

#define DISEASE_RESISTANCE_TEST_MODIFIER	-20

#define DISEASED_RECOVERY_MODIFIER		-90






/*
 *
 * Psychic Constants
 *
 */
#define MAX_PSY_POWERS			10

#define PSY_POWER_NAME_SIZE		25

#define MAX_CHARACTER_PSY_POWERS	8




/*
 *
 * Combat Constants
 *
 */
#define OBJECT_HIT_VALUE			100

#define CRITICAL_HIT_DAMAGE_MULTIPLIER		2

#define MORALE_BROKEN_EFFECT_DURATION		8

/*
 * combat range definitions
 */
#define MAX_LONG_RANGE		20
#define MAX_MEDIUM_RANGE	12
#define MAX_SHORT_RANGE		6
#define MAX_CLOSE_COMBAT_RANGE	1


/*
 * A mode spread limits
 */
#define A_MODE_SPREAD_MIN	1
#define A_MODE_SPREAD_MAX	4



#define SA_MODE_TO_HIT_PENALTY			-10








/*
 *
 * Timing Constants
 *
 */

/*
 * number of phases per round
 */
#define N_PHASES		10

/*
 * maximal number of actions a character can perform in a single
 * round - equal to N_PHASES
 */
#define MAX_ACTIONS		N_PHASES









/*
 *
 * Event Constants
 *
 */
#define MAX_EVENT_TYPES		7




/*
 *
 * Dynamic Message Constants
 *
 */
#define MAX_MESSAGE_TYPES	32



/*
 *
 * Message Grammar Constants
 *
 */
#define MAX_PRONOUNS		4
#define PRONOUN_2ND_PERSON	3

#define PRONOUN_SIZE		16





/*
 *
 * Help Constants
 *
 */

/*
 * help sections
 */
#define MAX_HELP_SECTIONS		16
#define HELP_STATS			"stats.txt"
#define HELP_PERKS			perk_list
#define HELP_MORALE			"morale.txt"
#define HELP_CHARACTER_ATTRIBUTES	character_attribute_list
#define	HELP_OBJECT_ATTRIBUTES		object_attribute_list
#define HELP_TERRAIN_ATTRIBUTES		terrain_attribute_list
#define HELP_PARTIES			"parties.txt"
#define HELP_ADVANCEMENT		"advance.txt"
#define HELP_CAREERS			career_list
#define HELP_TIMING			"timing.txt"
#define HELP_MOVEMENT			"movement.txt"
#define HELP_COMBAT			"combat.txt"
#define HELP_EVOCATION			"evoke.txt"
#define HELP_PSYCHIC_POWERS		psy_power_list
#define HELP_MACROS			"macro.txt"
#define HELP_TACTICS			"tactics.txt"



/*
 *
 * Honour Constants
 *
 */

/*
 * certificate of honour file
 */
#define FILE_CERTIFICATE		"honour.txt"






/*
 *
 * Quest Constants
 *
 */
#define QUEST_NAME_SIZE		64
#define QUEST_STATUS_SIZE	64



/*
 *
 * Scenario Constants
 *
 */
#define SCENARIO_NAME_SIZE	20
#define SCENARIO_VERSION_SIZE	8

#define FILE_SCENARIO_INTRO	"intro.txt"



/*
 *
 * Script Constants
 *
 */
#define SCRIPT_SELF_PTR			"SELF_PTR"
#define SCRIPT_ACTIVE_CHARACTER_PTR	"ACTIVE_CHARACTER_PTR"
#define SCRIPT_TRUE			"Yes"
#define SCRIPT_FALSE			"No"
#define FILE_SCRIPT_RANDOM_RECRUITABLE	"random_recruitable.wca"



/*
 *
 * Utility Constants
 *
 */
#define ASCII_LF			0x0A
#define ASCII_CR			0x0D

#define MAX_DIRECTIONS			11





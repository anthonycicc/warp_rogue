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
 * Warp Rogue Data Types
 *
 */



/*
 * hack - replace with stdints
 */
#if INT_MAX < 2147483647
#	error int is too narrow (must be at least 32bit wide)
#endif




/*
 * Incomplete Types
 *
 * these declarations are needed to avoid dependency issues
 *
 */
typedef struct character_struct		CHARACTER;
typedef struct object_struct		OBJECT;
typedef struct terrain_struct		TERRAIN;
typedef struct event_struct		EVENT;





/*
 *
 * Platform Types
 *
 */

/*
 * program directory type
 */
enum {

	DIR_NIL = -1,

	DIR_DATA,

	DIR_DUNGEONS,
	DIR_CHARACTERS,
	DIR_CHARACTER_SCRIPTS,
	DIR_GALAXY,
	DIR_INFO,
	DIR_HELP,
	DIR_OBJECTS,
	DIR_PLANETS,
	DIR_RACES,
	DIR_RULES,
	DIR_TERRAIN,
	DIR_UI,

	DIR_USER_DATA,

	DIR_OUT_OF_BOUNDS

};
typedef int	PROGRAM_DIRECTORY;






/*
 *
 * UI Types
 *
 */
typedef int				SCREEN_COORD;
#define MENU_AUTO			-1

typedef int				KEY_CODE;

typedef int				MACRO_INDEX;

typedef int				COLOUR;
#define C_NIL				-1
typedef int				COLOUR_VALUE;
typedef int				COLOUR_FREQUENCY;
#define COLOUR_FREQUENCY_MIN		1
#define COLOUR_FREQUENCY_MAX		100

/*
 * symbol type
 */
typedef int	SYMBOL;
#define SYM_NIL		-1


/*
 * screen area type
 */
typedef struct {

	SCREEN_COORD	top;
	SCREEN_COORD	bottom;
	SCREEN_COORD	left;
	SCREEN_COORD	right;

} SCREEN_AREA;

/*
 * screen point type
 */
typedef struct {

	SCREEN_COORD	y;
	SCREEN_COORD	x;

} SCREEN_POINT;

/*
 * RGB data
 */
typedef struct {

	COLOUR_VALUE	red;
	COLOUR_VALUE	green;
	COLOUR_VALUE	blue;

} RGB_DATA;

/*
 * dynamic colour
 */
typedef struct {

	/* the current colour */
	COLOUR			current;

	/* the primary colour */
	COLOUR			primary;

	/* the secondary colour */
	COLOUR			secondary;

	/*
	 * frequency of the secondary colour i.e. how often will the
	 * secondary colour be chosen instead of the primary colour
	 *
	 * valid values are 0 - 100 (0% to 100% chance of being chosen)
	 */
	COLOUR_FREQUENCY	frequency;

} DYNAMIC_COLOUR;

/*
 * command type
 */
enum {

	CM_NIL = -1,
	CM_LB = 0,

	CM_UP = 0,
	CM_DOWN,

	CM_LEFT,
	CM_RIGHT,
	CM_UP_LEFT,
	CM_UP_RIGHT,
	CM_DOWN_LEFT,

	CM_DOWN_RIGHT,
	CM_OK,
	CM_EXIT,
	CM_DROP,
	CM_INFO,
	CM_USE,
	CM_REROLL,
	CM_ADVANCEMENT,
	CM_PSY_POWERS,
	CM_NEAREST_ENEMY,

	CM_ADV_TARGET_MODE,
	CM_ATTACK,
	CM_TRIGGER,
	CM_INVENTORY_SCREEN,
	CM_EVOKE_PSY_POWER,
	CM_TACTICAL_COMMAND,
	CM_CHARACTER_SCREEN,
	CM_LOOK,

	CM_RELOAD_WEAPON,
	CM_UNJAM_WEAPON,
	CM_SWITCH_WEAPONS,
	CM_RUN,
	CM_USE_PERK,
	CM_JOURNAL,
	CM_GAME_CONTROLS,
	CM_HELP,
	CM_WAIT,

	CM_MACRO_1,
	CM_MACRO_2,
	CM_MACRO_3,
	CM_MACRO_4,
	CM_MACRO_5,
	CM_MACRO_6,
	CM_MACRO_7,
	CM_MACRO_8,
	CM_MACRO_9,
	CM_MACRO_10,

	CM_MACRO_11,
	CM_MACRO_12,
	CM_LIST,
	CM_CURRENT_TILE,
	CM_EQUIPMENT,
	CM_S,
	CM_SA,
	CM_A,
	CM_DEBUG,
	CM_RENAME,

	CM_CAREER,
	CM_CHARACTER_1,
	CM_CHARACTER_2,
	CM_CHARACTER_3,
	CM_PAGE_UP,
	CM_PAGE_DOWN,
	CM_ADDITIONAL_INFORMATION,
	CM_BUY,
	CM_MANUFACTURE,

	CM_OUT_OF_BOUNDS

};
typedef int	COMMAND;

/*
 * menu type
 */
typedef struct {

	LIST *		items;

	int		first_visible;
	int 		last_visible;
	int 		highlighted;
	int		last;

	COMMAND		entered_command;

} MENU;

/*
 * get text dialogue type
 */
typedef struct {

	char		prompt[STRING_BUFFER_SIZE];

	COLOUR		colour;

	char		entered_text[STRING_BUFFER_SIZE];

	int		max_length;

} GET_TEXT_DIALOGUE;

/*
 * confirmation dialogue type
 */
typedef struct {

	char		prompt[STRING_BUFFER_SIZE];

	COLOUR		colour;
	SCREEN_POINT	position;

	bool		confirmed;

} CONFIRMATION_DIALOGUE;

/*
 * dialogue state type
 */
typedef struct {

	char text[DIALOGUE_TEXT_SIZE];

	CHARACTER *active_character;

	CHARACTER *passive_character;

	LIST *options;

	MENU *menu;

} DIALOGUE_STATE;

/*
 * message object type type
 */
enum {

	MOT_NIL = -1,

	MOT_CHARACTER,
	MOT_OBJECT,
	MOT_STRING,

	MOT_OUT_OF_BOUNDS

};
typedef int	MESSAGE_OBJECT_TYPE;

/*
 * screen ID type
 */
enum {

	SID_NIL = -1,

	SID_GAME,
	SID_SECONDARY,
	SID_DIALOGUE,
	SID_CHARACTER,
	SID_TITLE,
	SID_INVENTORY,
	SID_INCREASE_STAT,
	SID_NEW_PERK,
	SID_LEARN_PSY_POWER,
	SID_CHANGE_CAREER_FOLLOW_PATH,
	SID_CHANGE_CAREER_NEW_PATH,
	SID_CURRENT_POSITION,

	SID_OUT_OF_BOUNDS

};
typedef int	SCREEN_ID;

/*
 * number of colours type
 */
typedef int		N_COLOURS;

/*
 * colour data type
 */
typedef struct {

	char		name[COLOUR_NAME_SIZE];

	RGB_DATA	rgb;

} COLOUR_DATA;

/*
 * UI colour type
 */
typedef struct {

	const char	name[UI_COLOUR_NAME_SIZE];

	COLOUR * const	colour_ptr;

} UI_COLOUR;


/*
 * special key type
 */
typedef struct {

	const char *	name;

	KEY_CODE	key_code;

} SPECIAL_KEY;


typedef int	SPECIAL_KEY_INDEX;





/*
 * command data type
 */
typedef struct {

	const char	name[COMMAND_NAME_SIZE];

	KEY_CODE	key_code;

} COMMAND_DATA;

/*
 * number of commmands type
 */
typedef int	N_COMMANDS;

/*
 * gore feature type
 */
typedef struct {

	/* feature name */
	char	name[GORE_FEATURE_NAME_SIZE];

	/* feature symbol */
	SYMBOL	symbol;

	/* feature colour */
	COLOUR	colour;

} GORE_FEATURE;







/*
 *
 * Dynamic Message Types
 *
 */

/*
 * message type type
 */
enum {

	MSG_NIL = -1,

	MSG_HIT,
	MSG_CRITICAL_HIT,
	MSG_MISS,
	MSG_PARRY,

	MSG_DISARM,
	MSG_DISARM_FAILS,

	MSG_PICK_UP,

	MSG_RELOAD_WEAPON,
	MSG_RECHARGE_WEAPON,
	MSG_SWITCH_WEAPONS,
	MSG_FIGHTS_UNARMED,
	MSG_WEAPON_JAMS,
	MSG_UNJAM_WEAPON,

	MSG_WEAPON_MALFUNCTIONS,

	MSG_WEAPON_OVERHEATS,
	MSG_WEAPON_EXPLODES,

	MSG_RESIST,
	MSG_RECOVER,

	MSG_EFFECT_ACTIVATION,
	MSG_EFFECT_TERMINATION,

	MSG_MORALE_BROKEN,
	MSG_MORALE_RECOVERED,

	MSG_EVOKE_PSY_POWER,
	MSG_EVOKE_PSY_POWER_FAILS,
	MSG_PSY_POWER_TERMINATION,

	MSG_USE_DRUG,
	MSG_DRUG_TERMINATION,

	MSG_FADE_INTO_SHADOWS,
	MSG_NOTICE,

	MSG_FALL_DOWN,

	MSG_DEATH,

	MSG_DESTRUCTION,

	MSG_OUT_OF_BOUNDS

};
typedef int	MESSAGE_TYPE;

/*
 * article type
 */
enum {

	ARTICLE_NIL = -1,

	ARTICLE_NONE,
	ARTICLE_DEFINITE,
	ARTICLE_INDEFINITE,

	ARTICLE_OUT_OF_BOUNDS

};
typedef int	ARTICLE;

/*
 * message data type
 */
typedef struct {

	const CHARACTER *	subject;
	bool			subject_noticed;

	const void *		object;
	MESSAGE_OBJECT_TYPE	object_type;
	ARTICLE			object_article;
	bool			object_noticed;

} MESSAGE_DATA;




/*
 *
 * Message Grammar Types
 *
 */

/*
 * numerus type
 */
enum {

	NUMERUS_NIL = -1,

	NUMERUS_SINGULAR,
	NUMERUS_PLURAL,

	NUMERUS_OUT_OF_BOUNDS

};
typedef int	NUMERUS;

/*
 * grammatical person type
 */
enum {

	GRAMMATICAL_PERSON_NIL = -1,

	GRAMMATICAL_PERSON_1ST,
	GRAMMATICAL_PERSON_2ND,
	GRAMMATICAL_PERSON_3RD,

	GRAMMARICAL_PERSON_OUT_OF_BOUNDS

};
typedef int	GRAMMATICAL_PERSON;







/*
 *
 * Utility Types
 *
 */

/*
 * direction type
 */
enum {

	DIRECTION_NIL = -1,

	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,

	DIRECTION_UP_LEFT,
	DIRECTION_UP_RIGHT,
	DIRECTION_DOWN_LEFT,
	DIRECTION_DOWN_RIGHT,

	DIRECTION_ELEVATOR,

	DIRECTION_WAY_UP,
	DIRECTION_WAY_DOWN,

	DIRECTION_OUT_OF_BOUNDS

};
typedef int	DIRECTION;


typedef int	COORD_MODIFIER;

typedef enum {

	COORD_Y,
	COORD_X

} COORD_TYPE;













/*
 *
 * Data File Types
 *
 */

/*
 * file counter type
 */
typedef int				FILE_COUNTER;


/*
 * serial type
 */
typedef short				SERIAL;
#define SERIAL_NIL			-1



/*
 *
 * Game Types
 *
 */

/*
 * game difficulty type
 */
enum {
	
	DIFFICULTY_NIL = -1,
	
	DIFFICULTY_NORMAL,
	DIFFICULTY_EASY,
	
	DIFFICULTY_OUT_OF_BOUNDS

};
typedef int	GAME_DIFFICULTY;





/*
 *
 * Dice Types
 *
 */

/* number of dice type */
typedef int		N_DICE;

/* number of dice sides type */
typedef int		N_SIDES;

/* dice roll modifier type */
typedef int		DICE_ROLL_MODIFIER;

/* dice roll result type */
typedef int		DICE_ROLL_RESULT;


/*
 * dice roll type
 */
typedef struct {

	/* number of dice */
	N_DICE			n_dice;

	/* number of dice sides */
	N_SIDES			n_sides;

	/* dice roll modifier */
	DICE_ROLL_MODIFIER	modifier;

} DICE_ROLL;







/*
 *
 * Randomiser Types
 *
 */

/* randomiser value type */
typedef int RVALUE;
#define RVALUE_NIL	-1

/*
 * randomiser item type
 */
typedef struct {

	char	string[RANDOMISER_STRING_SIZE];

	RVALUE	rv;

} RANDOMISER_ITEM;













/*
 *
 * Random Name Types
 *
 */

/*
 * name set type
 */
enum {

	NAME_SET_NIL = -1,

	NAME_SET_MALE,
	NAME_SET_FEMALE,

	NAME_SET_OUT_OF_BOUNDS

};
typedef int	NAME_SET;





/*
 *
 * Timing Types
 *
 */

/*
 * initiative type
 */
typedef int				INITIATIVE;


/*
 * timer type
 */
typedef int				TIMER;
#define TIMER_NIL			-1
#define TIMER_UNDETERMINED		-2
#define TIMER_CONTINUOUS		-3


/*
 * number of actions type
 */
typedef int				N_ACTIONS;








/*
 *
 * Planet Types
 *
 */

/*
 * planet coordinate type
 */
typedef int				PLANET_COORD;
#define PLANET_COORD_NIL			-1

/*
 * planet point type
 */
typedef struct {

	PLANET_COORD	z;
	PLANET_COORD	y;
	PLANET_COORD	x;

} PLANET_POINT;

/*
 * number of levels type
 */
typedef int	N_LEVELS;

/*
 * planet tile type
 */
typedef struct {

	/* tile name */
	char *		name;

	/* tile script file name */
	char *		script;

	SYMBOL		symbol;
	SYMBOL		screen_symbol;

	/* tile colour */
	DYNAMIC_COLOUR	colour;

	bool		impassable;

	N_LEVELS	n_levels;

} PLANET_TILE;

/*
 * planet type
 */
typedef struct {

	char		name[PLANET_NAME_SIZE];

	char		description[DESCRIPTION_SIZE];

	PLANET_TILE	map[PLANET_HEIGHT][PLANET_WIDTH];

	PLANET_POINT	starting_position;
	DIRECTION	starting_direction;

} PLANET;



/*
 *
 * Sector Types
 *
 */

/*
 * sector type
 */
typedef struct {

	TERRAIN *	terrain;

	OBJECT *	object;

	CHARACTER *	character;

} SECTOR;

/*
 * sector class type
 */
enum {

	SC_NIL = -1,

	SC_ANY,

	SC_FREE,

	SC_FREE_OBJECT_LOCATION,
	SC_FREE_OBJECT_LOCATION_ANTIBLOCK,

	SC_MOVE_TARGET,
	SC_MOVE_TARGET_SAFE,

	SC_ELEVATOR,
	SC_WAY_UP,
	SC_WAY_DOWN,

	SC_OUT_OF_BOUNDS

};
typedef int SECTOR_CLASS;

/*
 * number of sectors type
 */
typedef int	N_SECTORS;



/*
 *
 * Area Types
 *
 */

/*
 * area coordinate type
 */
typedef int				AREA_COORD;
#define AREA_COORD_NIL			-1
#define AREA_POINT_NOT_DEFINED(p)	((p)->y == AREA_COORD_NIL)

/*
 * area distance type
 */
typedef int				AREA_DISTANCE;
#define AREA_DISTANCE_MAX		INT_MAX
#define AREA_DISTANCE_NIL		-1

/*
 * area point type
 */
typedef struct {

	AREA_COORD	y;
	AREA_COORD	x;

} AREA_POINT;

/*
 * area section type
 */
typedef struct {

	AREA_COORD	top;
	AREA_COORD	bottom;
	AREA_COORD	left;
	AREA_COORD	right;

} AREA_SECTION;

/*
 * area type
 */
typedef struct {

	char		name[AREA_NAME_SIZE];

	PLANET_POINT	location;

	SECTOR		sector[AREA_HEIGHT][AREA_WIDTH];

	LIST *		characters;

	LIST *		events;

} AREA;




/*
 *
 * Pathfinder Types
 *
 */

/*
 * path node type
 */
typedef struct path_node_struct		PATH_NODE;

struct path_node_struct {

	AREA_POINT	p;

	PATH_NODE *	parent;

};

/*
 * pathfinder configuration type
 */
typedef struct {

	/* the character who is trying to find a path */
	CHARACTER *		subject;

	/* the starting point of the path */
	const AREA_POINT *	start;

	/* the target point of the path */
	const AREA_POINT *	target;

	/* ignore dangerous terrain */
	bool			ignore_dangerous;

	/* ignore destructable obstacles */
	bool			ignore_destructable;

	/* ignore characters */
	bool			ignore_characters;

} PATHFINDER_CONFIG;





/*
 *
 * Faction Types
 *
 */

/*
 * faction relationship type
 */
enum {
	
	FR_NIL = -1,
	FR_LB = 0,

	FR_FRIENDLY = 0,
	FR_NEUTRAL,
	FR_HOSTILE,
	
	FR_OUT_OF_BOUNDS

};
#define FR_DEFAULT	FR_NEUTRAL
typedef int	FACTION_RELATIONSHIP;

/*
 * number of factions type
 */
typedef int	N_FACTIONS;

/* 
 * faction type
 */
#define FACTION_NIL	-1
#define FACTION_LB	0
#define FACTION_PLAYER	0
/* hack */
#define FACTION_RANDOM_RECRUITABLE	1
typedef int	FACTION;





/*
 *
 * Party Types
 *
 */

/*
 * party type
 */
enum {

	PARTY_NIL = -1,

	PARTY_PLAYER,

	PARTY_OUT_OF_BOUNDS

};
typedef int PARTY;

/*
 * party size type
 */
typedef int	PARTY_SIZE;




/*
 *
 * Power Types
 *
 */

/*
 * power level type
 */
enum {

	PL_NIL = -1,

	PL_BASIC,
	PL_ADVANCED,
	PL_ELITE,
	PL_HERO,

	PL_OUT_OF_BOUNDS

};
typedef int	POWER_LEVEL;

/*
 * power difference type
 */
typedef int	POWER_DIFFERENCE;





/*
 *
 * Combat Types
 *
 */
typedef int				N_SHOTS;
typedef int				N_STRIKES;
typedef int				N_TARGETS;

typedef int				HIT_ROLL;
typedef int				HIT_VALUE;
typedef int				HIT_MODIFIER;

typedef int				REACH_MODIFIER;

typedef int				PARRY_VALUE;

typedef int				A_MODE_SPREAD_FACTOR;

typedef int				DAMAGE;

/*
 * target mode type
 */
enum {

	TMODE_NIL = -1,

	TMODE_SHOOT,
	TMODE_STRIKE,
	TMODE_PSY_POWER,
	TMODE_TRIGGER,
	TMODE_RUN,
	TMODE_JUMP,
	TMODE_DISARM,

	TMODE_OUT_OF_BOUNDS

};
typedef int	TARGET_MODE;

/*
 * firing mode type
 */
enum {

	FMODE_NIL = -1,

	FMODE_S,
	FMODE_SA,
	FMODE_A,

	FMODE_OUT_OF_BOUNDS

};
typedef int	FIRING_MODE;

/*
 * attack type type
 */
enum {

	AT_NIL = -1,

	AT_STRIKE,
	AT_SHOT,

	AT_OUT_OF_BOUNDS

};
typedef int	ATTACK_TYPE;

/*
 * firing data type
 */
typedef struct {

	FIRING_MODE			firing_mode;

	N_SHOTS				n_shots;

	A_MODE_SPREAD_FACTOR		spread;

} FIRING_DATA;

/*
 * attack data type
 */
typedef struct {

	ATTACK_TYPE		type;

	CHARACTER *		attacker;

	OBJECT *		weapon;

	FIRING_DATA *		firing_data;

	AREA_POINT		target_point;

	AREA_DISTANCE		distance;

	bool			sneak;

	bool			rapid;

	HIT_VALUE		hit_value;

	HIT_ROLL		hit_roll;

	bool			critical_hit;

} ATTACK_DATA;





/*
 *
 * Character Death Types
 *
 */

/*
 * gore level enum type
 */
enum {

	GORE_LEVEL_NIL = -1,

	GORE_LEVEL_ZERO,

	GORE_LEVEL_BLOOD,

	GORE_LEVEL_BODY_PART,

	GORE_LEVEL_PLANT_PARTS,
	GORE_LEVEL_MACHINE_PARTS,
	GORE_LEVEL_POISONED_CORPSE,
	GORE_LEVEL_CORPSE,

	GORE_LEVEL_OUT_OF_BOUNDS

};
typedef int	GORE_LEVEL;

/*
 * death type type
 */
enum {

	DT_NIL = -1,

	DT_ATTACK,
	DT_FALL,
	DT_WEAPON_EXPLOSION,
	DT_POISON,

	DT_OUT_OF_BOUNDS

};
typedef int	DEATH_TYPE;





/*
 *
 * AI Types
 *
 */

/*
 * AI state type
 */
enum {

	AI_STATE_NIL = -1,

	AI_STATE_WAIT,
	AI_STATE_GUARD,
	AI_STATE_PATROL,

	AI_STATE_FOLLOW,
	AI_STATE_FLEE,
	AI_STATE_SEARCH,

	AI_STATE_COMBAT,

	AI_STATE_OUT_OF_BOUNDS

};
typedef int	AI_STATE;

/*
 * AI tactic type
 */
enum {

	TACTIC_NIL = -1,
	TACTIC_LB = 0,

	TACTIC_PLAN_A = 0,
	TACTIC_GET_READY,
	TACTIC_STAY_CLOSE,
	TACTIC_HOLD_POSITION,

	TACTIC_OUT_OF_BOUNDS

};
typedef int	AI_TACTIC;

/*
 * AI control data type
 */
typedef struct {

	/* the currrent state of the AI */
	AI_STATE		state;

	/*
	 * the default state of the AI i.e. the basic state to which
	 * the AI jumps back, once it has finished its current task
	 */
	AI_STATE		default_state;

	/* the AI state which was set during character creation */
	AI_STATE		original_state;

	/* the current AI tactic */
	AI_TACTIC		tactic;

	/* the current target point */
	AREA_POINT		target_point;

	/* the currently targeted character */
	CHARACTER *		target_character;

	/* target distance */
	AREA_DISTANCE		target_distance;

	/* the path to the target */
	const PATH_NODE *	path;

	/* true if the target is reachable */
	bool			target_reachable;

	/* the AI controlled character */
	CHARACTER *		self;

	/* true if the most recent follow PCC attempt failed */
	bool			follow_failed;

} AI_CONTROL_DATA;

/*
 * AI option weight type
 */
typedef int	AI_OPTION_WEIGHT;
#define AI_OPTION_WEIGHT_NIL		-1



/*
 * AI option type
 */
enum {

	AI_OPTION_NIL = -1,

	AI_OPTION_WEAPON,
	AI_OPTION_SECONDARY_WEAPON,

	AI_OPTION_OUT_OF_BOUNDS = MAX_PSY_POWERS + 2

};
typedef int	AI_OPTION;








/*
 *
 * Object Types
 *
 */
typedef int				ARMOUR_VALUE;

typedef int				MONEY;
#define VALUE_NIL			-1

typedef int				WEIGHT;
#define WEIGHT_NIL			-1
#define ENCUMBRANCE_MAX(c)		((c)->stat[S_ST].current * 10)

typedef BOX_ITEM_INDEX			OBJECT_INDEX;
#define OBJECT_INDEX_NIL		-1

typedef int				WEAPON_REACH;

typedef int				PARRY_PENALTY;

typedef int				CONDITION;
#define CONDITION_MAX			INT_MAX
#define CONDITION_INDESTRUCTABLE	CONDITION_MAX

typedef int				RELOAD_VALUE;
#define RELOAD_NO			-1

typedef int				CHARGE;
#define	CHARGE_NIL			-1

typedef int				N_OBJECTS;


/*
 * object type type
 */
enum {

	OTYPE_NIL = -1,

	OTYPE_CLOSE_COMBAT_WEAPON,
	OTYPE_RANGED_COMBAT_WEAPON,

	OTYPE_ARMOUR,

	OTYPE_DRUG,

	OTYPE_ENVIRONMENT,

	OTYPE_MONEY,

	OTYPE_JUMP_PACK,

	OTYPE_OUT_OF_BOUNDS

};
typedef int	OBJECT_TYPE;

/*
 * object subtype type
 */
enum {

	OSTYPE_NIL = -1,

	OSTYPE_KNIFE,

	OSTYPE_PISTOL,
	OSTYPE_BASIC,

	OSTYPE_LIGHT,
	OSTYPE_MEDIUM,
	OSTYPE_HEAVY,

	OSTYPE_ELEVATOR,
	OSTYPE_WAY_UP,
	OSTYPE_WAY_DOWN,

	OSTYPE_OUT_OF_BOUNDS

};
typedef int	OBJECT_SUBTYPE;


/*
 * object firing mode data type
 */
typedef struct {

	bool			has[MAX_FIRING_MODES];

	N_SHOTS			min_sa_shots;
	N_SHOTS			max_sa_shots;

	N_SHOTS			a_shots;

	HIT_MODIFIER		accuracy;

} FIRING_MODE_DATA;

/*
 * object type
 */
struct object_struct {

	GORE_LEVEL		gore_level;

	CONDITION		condition;

	CHARGE			charge;


	/* common data ****************************/
	char *			name;

	NUMERUS			numerus;

	SYMBOL			symbol;

	DYNAMIC_COLOUR		colour;

	char *			description;

	bool			attribute[MAX_OBJECT_ATTRIBUTES];

	CONDITION		condition_max;

	CHARGE			charge_max;

	OBJECT_TYPE		type;
	OBJECT_SUBTYPE		subtype;

	WEIGHT			weight;

	MONEY			value;

	/******************************************/

	/* weapons only ***************************/
	DICE_ROLL		damage;
	/******************************************/

	/* ranged combat weapons only *************/
	FIRING_MODE_DATA	firing_mode;
	RELOAD_VALUE		reload;
	/******************************************/

	/* close combat weapons only **************/
	WEAPON_REACH		reach;
	PARRY_PENALTY		parry_penalty;
	/******************************************/

	/* armours only ***************************/
	ARMOUR_VALUE		armour_value;
	/******************************************/

	/* the serial numbers of the object ********/
	SERIAL			sn_data;
	SERIAL			sn_save;
	/******************************************/

};





/*
 *
 * Object Attribute Types
 *
 */

/*
 * object attribute type
 */
enum {

	OA_NIL = -1,

	OA_ALIEN,
	OA_AUTOMATIC_RECHARGE,
	OA_BLOCKS_LINE_OF_SIGHT,
	OA_CANNOT_BE_PARRIED,
	OA_CANNOT_PARRY,
	OA_DAEMONIC,
	OA_DANGEROUS,
	OA_DISEASE,
	OA_ENVIRONMENTAL,
	OA_FORCE,
	OA_FORCE_CHANNEL,
	OA_FORCE_RUNE,
	OA_HINDERS_MOVEMENT,
	OA_IGNORE_ARMOUR,
	OA_IMPASSABLE,
	OA_INHERENT,
	OA_JAMMED,
	OA_LASER,
	OA_NOISY,
	OA_PAIN,
	OA_PARTIAL_RELOAD,
	OA_PLASMA,
	OA_POISON,
	OA_POWER,
	OA_PROJECTILE,
	OA_RELIABLE,
	OA_SHOCK,
	OA_SILENT,
	OA_STRENGTH_BOOST,
	OA_STRENGTH_BOOST_2,
	OA_UNIQUE,

	OA_OUT_OF_BOUNDS

};
typedef int	OBJECT_ATTRIBUTE;










/*
 *
 * Character Stat Types
 *
 */

/*
 * stat value type
 */
typedef int		STAT_VALUE;

/*
 * stat modifier type
 */
typedef int				STAT_MODIFIER;
#define STAT_MODIFIER_MAX		INT_MAX

/*
 * character stat type
 */
typedef struct {

	STAT_VALUE	current;

	STAT_VALUE	total;

	STAT_MODIFIER	advance;

} CHARACTER_STAT;

/*
 * character stat index type
 */
enum {

	S_NIL = -1,
	S_LB = 0,

	S_CC = 0,
	S_RC,
	S_ST,
	S_TN,
	S_AG,
	S_IN,
	S_WP,
	S_FL,

	S_OUT_OF_BOUNDS

};
typedef int	STAT;

/*
 * character stat bonus modifier type
 */
typedef int		STAT_BONUS;

/*
 * stat name type
 */
typedef struct {

	/* the abbreviated name of the stat */
	const char	short_name[STAT_SHORT_NAME_SIZE];

	/* the full name of the stat */
	const char	long_name[STAT_LONG_NAME_SIZE];

} STAT_NAME;





/*
 *
 * Character Perk Types
 *
 */

/*
 * perk type
 */
enum {

	PK_NIL = -1,
	PK_LB,

	PK_ALERTNESS = 0,
	PK_ALIEN_TECHNOLOGY,

	PK_BATTLE_TACTICS,
	PK_BLADEMASTER,

	PK_COMBAT_EVOCATION,
	PK_CRITICAL_SHOT,
	PK_CRITICAL_STRIKE,
	PK_CRUSHING_STRIKE,

	PK_DAEMON_LORE,
	PK_DISARM,
	PK_DRUG_RESISTANT,

	PK_ENHANCED_PARRY,

	PK_FAST_DRAW,
	PK_FIRST_AID,

	PK_GRENADIER,

	PK_IMMUNITY_TO_DISEASE,
	PK_IMMUNITY_TO_FEAR,
	PK_IMMUNITY_TO_PAIN,
	PK_IMMUNITY_TO_POISON,

	PK_JUMP_PACK,

	PK_LEADER,

	PK_MARKSMAN,

	PK_PSYCHIC_I,
	PK_PSYCHIC_II,
	PK_PSYCHIC_III,
	PK_PSYCHIC_RESISTANCE,
	PK_PSYCHIC_WEAPONS,

	PK_QUICKLOAD,

	PK_RAPID_SHOT,
	PK_RAPID_STRIKE,

	PK_SILENT_EVOCATION,
	PK_SNEAK_ATTACK,
	PK_SPOT_TRAP,
	PK_SPOT_WEAKNESS,
	PK_STEALTH,

	PK_UNARMED_COMBAT,

	PK_UNCANNY_DODGE,

	PK_WEAPON_MAINTENANCE,

	PK_OUT_OF_BOUNDS

};
typedef int	PERK;

/*
 * number of perks type
 */
typedef int	N_PERKS;

/*
 * perk data type
 */
typedef struct {

	/* the name of the perk */
	const char	name[PERK_NAME_SIZE];

	/* true if the perk can be used actively */
	bool		useable;

} PERK_DATA;




/*
 *
 * Character Career Types
 *
 */

/*
 * career index type
 */
typedef BOX_ITEM_INDEX		CAREER_INDEX;
#define CAREER_NIL		-1

/*
 * career exit index type
 */
typedef int			CAREER_EXIT_INDEX;

/*
 * number of careers type
 */
typedef int	N_CAREERS;

/*
 * career type type
 */
enum {

	CAREER_TYPE_NIL = -1,

	CAREER_TYPE_BASIC,
	CAREER_TYPE_ADVANCED,

	CAREER_TYPE_OUT_OF_BOUNDS

};
typedef int	CAREER_TYPE;

/*
 * career perk info type
 */
typedef struct {

	/* true if the perk is available */
	bool	available;

	/* true if the perk is required */
	bool	required;

} CAREER_PERK_INFO;


/*
 * career stat info type
 */
typedef struct {
	
	/* stat advancement limit */
	STAT_MODIFIER		advance_maximum;

	/* stat requirement */
	STAT_VALUE		required_minimum;

} CAREER_STAT_INFO;



/*
 * career type
 */
typedef struct {

	/* the name */
	char			name[CAREER_NAME_SIZE];

	/* the colour */
	COLOUR			colour;

	/* the description */
	char			description[DESCRIPTION_SIZE];

	/* the career type */
	CAREER_TYPE		type;
	
	/* the career stats */
	CAREER_STAT_INFO	stat[MAX_STATS];

	/* the career perks */
	CAREER_PERK_INFO	perk[MAX_PERKS];

	/* the trappings, a list of objects */
	LIST *			trappings;

	/* the career exits, a list of career names */
	char			exit[MAX_CAREER_EXITS][CAREER_NAME_SIZE];

	/* the serial number of the object */
	SERIAL			sn;

} CAREER;





/*
 *
 * Character types
 *
 */


/*
 * character index type
 */
typedef BOX_ITEM_INDEX			CHARACTER_INDEX;
#define CHARACTER_INDEX_NIL			-1
#define CHARACTER_INDEX_RANDOM_RECRUITABLE	-2



/*
 * character attributes type
 */
enum {

	CA_NIL = -1,

	CA_BROKEN,
	CA_BULL,
	CA_BUSY,
	CA_CANNOT_SPEAK,
	CA_DAEMONIC,
	CA_DISEASED,
	CA_FLASH,
	CA_FOCUS,
	CA_MACHINE,
	CA_NO_UNARMED_ATTACK,
	CA_NOMAT,
	CA_PLANT,
	CA_POISONED,
	CA_PSYCHIC_SHIELD,
	CA_STOIC,
	CA_STUNNED,
	CA_UNIQUE,
	CA_UNNOTICED,
	CA_WARP_STRENGTH,

	CA_OUT_OF_BOUNDS

};
typedef int	CHARACTER_ATTRIBUTE;



/*
 * character attribute data type
 */
typedef struct {

	const char	name[CHARACTER_ATTRIBUTE_NAME_SIZE];

	COLOUR		colour;

} CHARACTER_ATTRIBUTE_DATA;

/*
 * character type type
 */
enum {

	CT_NIL = -1,

	CT_PC,

	CT_COMPANION_1,
	CT_COMPANION_2,

	CT_OTHER,

	CT_OUT_OF_BOUNDS

};
typedef int CHARACTER_TYPE;

/*
 * number of characters type
 */
typedef int	N_CHARACTERS;

/*
 * experience points type
 */
typedef int	EXPERIENCE_POINTS;

/*
 * injury type
 */
typedef int	INJURY;

/*
 * bloody feet value type
 */
typedef int	BLOODY_FEET_VALUE;

/*
 * gender type
 */
enum {

	GENDER_NIL = -1,

	GENDER_MALE,
	GENDER_FEMALE,

	GENDER_NEUTER,

	GENDER_OUT_OF_BOUNDS

};
typedef int	GENDER;

/*
 * character controller type
 */
enum {

	CC_NIL = -1,

	CC_PLAYER,
	CC_AI,

	CC_OUT_OF_BOUNDS

};
typedef int CHARACTER_CONTROLLER;

/*
 * character armour rating type
 */
typedef struct {

	ARMOUR_VALUE	current;
	ARMOUR_VALUE	total;

} ARMOUR_RATING;

/*
 * character type
 */
struct character_struct {

	/* representation data ****************************/
	char			name[CHARACTER_NAME_SIZE];

	NUMERUS			numerus;

	SYMBOL			symbol;
	COLOUR			colour;

	GENDER			gender;

	char *			description;
	/**************************************************/

	/* state data *************************************/
	CHARACTER_TYPE		type;
	CHARACTER_CONTROLLER 	controller;

	AREA_POINT		location;

	FACTION			faction;

	INJURY			injury;

	BLOODY_FEET_VALUE	bloody_feet;

	bool			action_spent;
	/**************************************************/


	/* stats ******************************************/
	CHARACTER_STAT		stat[MAX_STATS];
	/**************************************************/


	/* flags, perks, psychic powers *******************/
	bool			attribute[MAX_CHARACTER_ATTRIBUTES];

	bool			perk[MAX_PERKS];

	bool			psy_power[MAX_PSY_POWERS];
	/**************************************************/


	/* advancement data *******************************/
	CAREER *		career;

	EXPERIENCE_POINTS	ep;
	EXPERIENCE_POINTS	ep_total;

	POWER_LEVEL		power_level;
	/**************************************************/


	/* party data *************************************/
	PARTY			party;
	/**************************************************/


	/* inventory, equipment ****************************/
	LIST *			inventory;

	OBJECT *		weapon;
	OBJECT *		secondary_weapon;
	OBJECT *		armour;
	OBJECT *		jump_pack;

	ARMOUR_RATING		armour_rating;
	/**************************************************/


	/* perception data ********************************/
	LIST *			noticed_enemies;
	/**************************************************/


	/* AI control data ********************************/
	AI_CONTROL_DATA		ai;
	/**************************************************/

	/* the character's script */
	char			script[FILE_NAME_SIZE];

	/* the character's serial numbers ******************/
	SERIAL			sn_data;
	SERIAL			sn_save;
	/**************************************************/

};




/*
 *
 * Character Race Types
 *
 */

/*
 * race structure
 */
typedef struct {

	SYMBOL		symbol;

	DICE_ROLL	stat[MAX_STATS];

} RACE;



/*
 *
 * Character Generation Types
 *
 */

/*
 * EP table item type
 */
typedef struct {
	
	/* minimal amount of EP */
	EXPERIENCE_POINTS	minimum;
	
	/* maximal amount of EP */
	EXPERIENCE_POINTS	maximum;

	/* EP total base */
	EXPERIENCE_POINTS	base;

} EP_TABLE_ITEM;







/*
 *
 * Psychic Power Types
 *
 */

/*
 * psychic power type
 */
enum {

	PSY_NIL = -1,
	PSY_LB = 0,

	PSY_REGENERATE = 0,
	PSY_REACTIVATE,
	PSY_WARP_STRENGTH,
	PSY_PURIFY_BLOOD,
	PSY_PURGE_PLAGUE,
	PSY_MIND_VIEW,
	PSY_PSYCHIC_SHRIEK,
	PSY_TERRIFY,
	PSY_PSYCHIC_SHIELD,
	PSY_BANISHMENT,

	PSY_OUT_OF_BOUNDS

};
typedef int	PSY_POWER;







/*
 *
 * Psychic Types
 *
 */

/*
 * evocation data type
 */
typedef struct {

	/* the character who evoked the psychic power */
	CHARACTER *		evoker;

	/* the evoked power */
	PSY_POWER		power;

	/* the target point of the evoked power */
	const AREA_POINT *	target_point;

} EVOCATION_DATA;

typedef int		N_PSY_POWERS;

typedef int		EVOCATION_VALUE;

typedef int PSY_TEST_VALUE;

typedef int PSY_RANGE_MODIFIER;

/*
 * psychic power target types
 */
typedef enum {

	PTT_NIL = -1,

	PTT_NONE,
	PTT_CHARACTER_BIO,
	PTT_CHARACTER_ALL,
	PTT_OBJECT,
	PTT_ANY,

	PTT_OUT_OF_BOUNDS

} PSY_TARGET_TYPE;

/*
 * psychic power data structure
 */
typedef struct {

	/* the name of the psychic power */
	const char			name[PSY_POWER_NAME_SIZE];

	/* the evocation value */
	const EVOCATION_VALUE		evocation_value;

	/* true if the psychic power is considered hostile */
	const bool			is_hostile;

	/* determines which targets are valid */
	const PSY_TARGET_TYPE		target_type;

	/* the associated character attribute */
	const CHARACTER_ATTRIBUTE	attribute;

	/* called when the psychic power is evoked */
	void				(* const evoke)\
						(const EVOCATION_DATA *);

	/* called when the psychic power is terminated */
	void				(* const terminate)(EVENT *);

} PSY_POWER_DATA;







/*
 *
 * Effect Types
 *
 */

/*
 * effect type
 */
enum {

	ET_NIL = -1,

	ET_POISONED,
	ET_DISEASED,
	ET_BROKEN,
	ET_STUNNED,
	ET_PSYCHIC_OVERLOAD,

	ET_OUT_OF_BOUNDS

};
typedef int	EFFECT;

/*
 * effect data type
 */
typedef struct {

	/* the name of the effect */
	const char			name[EFFECT_NAME_SIZE];

	/* the associated character attribute */
	const CHARACTER_ATTRIBUTE	attribute;

	/* called when the effect is activated */
	void				(* const activate)(EVENT *);

	/* called when the effect is terminated */
	void				(* const terminate)(EVENT *);

} EFFECT_DATA;












/*
 *
 * Object Drug Types
 *
 */

/*
 * drug type
 */
enum {

	DRUG_NIL = -1,

	DRUG_ANTIDOTE,
	DRUG_FLASH,
	DRUG_STOIC,
	DRUG_NOMAT,
	DRUG_ZEN,
	DRUG_BULL,

	DRUG_OUT_OF_BOUNDS

};
typedef int	DRUG;

/*
 * drug data type
 */
typedef struct {

	/* the name of the drug */
	const char			name[DRUG_NAME_SIZE];

	/* the associated character attribute */
	const CHARACTER_ATTRIBUTE	attribute;

	/* called when the drug used */
	void				(* const use)(EVENT *);

	/* called when the drug effect wears off */
	void				(* const terminate)(EVENT *);

} DRUG_DATA;







/*
 *
 * Terrain Types
 *
 */

/*
 * terrain index type
 */
typedef BOX_ITEM_INDEX			TERRAIN_INDEX;
#define TERRAIN_INDEX_NIL		-1

/*
 * terrain type
 */
struct terrain_struct {

	/* the name of the terrain */
	char *			name;

	/* the symbol used to represent the terrain */
	SYMBOL			symbol;

	/* the colour of the terrain */
	DYNAMIC_COLOUR		colour;

	/* the current gore level of the terrain */
	GORE_LEVEL		gore_level;

	/* the description of the terrain */
	char *			description;

	/* the attributes of the terrain */
	bool			attribute[MAX_TERRAIN_ATTRIBUTES];

	/* the serial numbers of terrain */
	SERIAL			sn_data;
	SERIAL			sn_save;

};

/*
 * number of terrains type
 */
typedef int		N_TERRAINS;






/*
 *
 * Terrain Attribute Types
 *
 */

/*
 * terrain attribute type
 */
enum {

	TA_NIL = -1,

	TA_AIR,
	TA_BLOCKS_LINE_OF_SIGHT,
	TA_CAUSES_DISEASE,
	TA_DANGEROUS,
	TA_IMPASSABLE,

	TA_OUT_OF_BOUNDS

};
typedef int	TERRAIN_ATTRIBUTE;





/*
 * dungeon environment type
 */
typedef struct {

	char terrain_left[TERRAIN_NAME_SIZE];
	char object_left[OBJECT_NAME_SIZE];

	char terrain_right[TERRAIN_NAME_SIZE];
	char object_right[OBJECT_NAME_SIZE];

	char terrain_top[TERRAIN_NAME_SIZE];
	char object_top[OBJECT_NAME_SIZE];

	char terrain_bottom[TERRAIN_NAME_SIZE];
	char object_bottom[OBJECT_NAME_SIZE];

} DUNGEON_ENVIRONMENT;




/*
 *
 * Cave Area Generator Types
 *
 */

/*
 * cave generator configuration type
 */
typedef struct {
	
	CCD_DUNGEON_TYPE	type;

	char			wall[OBJECT_NAME_SIZE];
	char			floor[TERRAIN_NAME_SIZE];

	char			way_up[OBJECT_NAME_SIZE];
	char			way_down[OBJECT_NAME_SIZE];

	PLANET_COORD		gate_level;

	DUNGEON_ENVIRONMENT	environment;

} CAVE_CONFIG;




/*
 *
 * Dungeon Area Generator Types
 *
 */

/*
 * dungeon generator configuration type
 */
typedef struct {

	char		wall[OBJECT_NAME_SIZE];
	char		floor[TERRAIN_NAME_SIZE];

	char		way_up[OBJECT_NAME_SIZE];
	char		way_down[OBJECT_NAME_SIZE];

	char		obstacle[OBJECT_NAME_SIZE];
	char		obstacle2[OBJECT_NAME_SIZE];

	char		pool[TERRAIN_NAME_SIZE];

	PLANET_COORD	gate_level;

	char		env_terrain[TERRAIN_NAME_SIZE];
	char		env_object[OBJECT_NAME_SIZE];

} DUNGEON_CONFIG;






/*
 *
 * Event Types
 *
 */

/*
 * number of events type
 */
typedef int			N_EVENTS;

/*
 * event power type
 */
typedef int			EVENT_POWER;

/*
 * event type type
 */
enum {

	EVT_NIL = -1,

	EVT_RELOAD_WEAPON,
	EVT_RECHARGE_WEAPON,
	EVT_UNJAM_WEAPON,
	EVT_EFFECT_TERMINATION,
	EVT_PSY_POWER_TERMINATION,
	EVT_DRUG_TERMINATION,
	EVT_POISON_DAMAGE,

	EVT_OUT_OF_BOUNDS

};
typedef int	EVENT_TYPE;

/*
 * event type
 */
struct event_struct {

	EVENT_TYPE		type;
	
	TIMER			timer;

	CHARACTER *		character;

	OBJECT *		object;

	DRUG			drug;
	PSY_POWER		psy_power;
	EFFECT			effect;

	EVENT_POWER		power;

	STAT_MODIFIER		stat_modifier[MAX_STATS];

};






/*
 *
 * Help Types
 *
 */

/*
 * help section link structure
 */
typedef struct {

	/* help file */
	const char *	file_name;

	/* help function */
	void 		(*function)(const char *);

} HELP_LINK;

/*
 * help section structure
 */
typedef struct {

	/* title of the help section */
	char *		title;

	/* help section link */
	HELP_LINK	link;

} HELP_SECTION;




/*
 *
 * Shop Types
 *
 */

/*
 * price level type
 */
typedef int		PRICE_LEVEL;


/*
 * shop type
 */
typedef struct {

	LIST *		object_list;

	MENU *		menu;

	PRICE_LEVEL	price_level;

} SHOP;








/*
 *
 * Quest Types
 *
 */

 /*
  * quest type
  */
 typedef struct {

	char	name[QUEST_NAME_SIZE];

	char	status[QUEST_STATUS_SIZE];

} QUEST;


typedef int	N_QUESTS;






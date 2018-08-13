/*
 * Wicca - a Public Domain Wicca interpreter
 *
 * Version ALPHA
 *
 * NAMESPACE: WCA_ / wca_
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "wicca.h"



/* data storage size */
#define WCA_STORAGE_SIZE		(WCA_MAX_NORMAL_VARIABLES + \
						WCA_MAX_MAGIC_VARIABLES + \
						WCA_MAX_LITERALS)

/* token size */
#define WCA_TOKEN_SIZE			WCA_STORAGE_BLOCK_SIZE

/* WCA_Execute size */
#define WCA_EXECUTE_SIZE		(WCA_MAX_DEPTH + 1)

/* magic variable storage index macro */
#define WCA_MAGIC_VARIABLE(n)		(WCA_MAX_NORMAL_VARIABLES + (n))

/* literal storage index macro */
#define WCA_LITERAL(n)			(WCA_MAX_NORMAL_VARIABLES + \
						WCA_MAX_MAGIC_VARIABLES + \
						(n))


/*
 * function index macros
 */
#define WCA_FUNCTION_NIL		-1
#define WCA_FUNCTION_IF			4
#define WCA_FUNCTION_ELSE_IF		5
#define WCA_FUNCTION_ELSE		6
#define WCA_FUNCTION_END_IF		7
#define WCA_FUNCTION_LOOP		8
#define WCA_FUNCTION_CONTINUE		9
#define WCA_FUNCTION_BREAK		10
#define WCA_FUNCTION_END_LOOP		11
#define WCA_FUNCTION_END		12


/*
 * Wicca symbols
 */
#define WCA_OVERRIDE			'\\'
#define WCA_COMMENT			'~'
#define WCA_CHAIN			'-'
#define WCA_INTERPRET			'\''
#define WCA_VARIABLE_PREFIX		'$'


/*
 * ASCII codes
 */
#define WCA_LF				0x0A
#define WCA_CR				0x0D
#define WCA_TAB				0x09
#define WCA_SPACE			0x20


/*
 * syntax sugar
 */
#define WCA_N_INSTRUCTIONS	WCA_InstructionNumber

#define WCA_NEW_TOKEN		memset(WCA_Token, 0x00, sizeof WCA_Token); \
					WCA_TokenI = 0

#define wca_copy_block(a,b)	memcpy(WCA_Storage[(a)], WCA_Storage[(b)],\
					WCA_STORAGE_BLOCK_SIZE)

#define wca_clear_block(a)	memset(WCA_Storage[(a)], 0x00, \
					WCA_STORAGE_BLOCK_SIZE)

#define wca_blocks_equal(a,b)	(memcmp(WCA_Storage[(a)], WCA_Storage[(b)],\
					WCA_STORAGE_BLOCK_SIZE) == 0)

#define wca_block_equals_string(b, s) \
			(strcmp((const char *)WCA_Storage[(b)], (s)) == 0)



/*
 * types
 */
typedef unsigned char		WCA_BYTE;

typedef int			WCA_FUNCTION_INDEX;

typedef int			WCA_LINE_NUMBER;

typedef int			WCA_INSTRUCTION_NUMBER;

typedef int			WCA_LITERAL_NUMBER;

typedef int			WCA_VARIABLE_NUMBER;

typedef int			WCA_BYTE_INDEX;

typedef WCA_BYTE		WCA_DEPTH;



/* 
 * function link type 
 */
typedef struct {

	/* function name */
	WCA_CHAR		name[WCA_FUNCTION_NAME_SIZE];

	/* linked C function */
	void			(* call)(void);

} WCA_FUNCTION_LINK;


/* 
 * function call type 
 */
typedef struct {

	WCA_DEPTH		depth;

	WCA_FUNCTION_INDEX	function;

	WCA_STORAGE_INDEX	param[WCA_MAX_FUNCTION_PARAMETERS];

} WCA_FUNCTION_CALL;



static void		 	wca_process_function_token(void);
static void			wca_process_parameter_token(void);
static void			wca_process_variable_token(void);
static void			wca_process_literal_token(void);



static void			wca_function_get(void);
static void			wca_function_set(void);
static void			wca_function_store(void);
static void			wca_function_expr(void);
static void			wca_function_if(void);
static void			wca_function_else_if(void);
static void			wca_function_else(void);
static void			wca_function_end_if(void);
static void			wca_function_loop(void);
static void			wca_function_continue(void);
static void			wca_function_break(void);
static void			wca_function_end_loop(void);
static void			wca_function_end(void);




/*
 * the function links
 */
static WCA_FUNCTION_LINK	WCA_Function[WCA_MAX_FUNCTIONS];

/* number of functions */
static WCA_NUMBER		WCA_N_Functions = 0;



/*
 * magic variable names
 */
static WCA_CHAR			WCA_MagicVariable[WCA_MAX_MAGIC_VARIABLES]
					[WCA_MAGIC_VARIABLE_NAME_SIZE];

/* number of magic variables */
static WCA_NUMBER		WCA_N_MagicVariables = 0;



/*
 * the tokenized program
 */
static WCA_FUNCTION_CALL	WCA_Program[WCA_MAX_INSTRUCTIONS];



/*
 * the data (variables and literals) storage
 */
static WCA_BYTE			WCA_Storage[WCA_STORAGE_SIZE]
					[WCA_STORAGE_BLOCK_SIZE];



/*
 * the most recent function call
 */
static WCA_FUNCTION_CALL *	WCA_Call;



/*
 * function call template (default values)
 */
static const WCA_FUNCTION_CALL	WCA_CallTemplate = {
	/* DEPTH */ 0, /* FUNCTION */ WCA_FUNCTION_NIL,
	/* PARAMETERS */ {WCA_NIL, WCA_NIL, WCA_NIL, WCA_NIL,
	WCA_NIL, WCA_NIL}
};



/*
 * array of boolean values which is used to determine whether or not
 * a function call should be executed
 */
bool				WCA_Execute[WCA_EXECUTE_SIZE];



/*
 * array of loop jump instruction numbers (i.e. where to jump to)
 */
static WCA_INSTRUCTION_NUMBER	WCA_LoopJump[WCA_MAX_DEPTH];

static bool			WCA_IfJump;



/* depth of the current loop */
static WCA_DEPTH		WCA_CurrentLoop;


/*
 * instruction pointer
 */
static WCA_INSTRUCTION_NUMBER	WCA_Ip;

/*
 * execution depth
 */
static WCA_DEPTH		WCA_Depth;

/* the current line number */
static WCA_LINE_NUMBER		WCA_LineNumber;

/* the current instruction number */
static WCA_INSTRUCTION_NUMBER	WCA_InstructionNumber;

/* the current variable number */
static WCA_VARIABLE_NUMBER	WCA_VariableNumber;

/* the current literal number */
static WCA_LITERAL_NUMBER	WCA_LiteralNumber;

/* function call token */
static WCA_CHAR			WCA_Token[WCA_TOKEN_SIZE];
static WCA_BYTE_INDEX		WCA_TokenI;

/* parameter iterator */
static WCA_PARAMETER_INDEX	WCA_ParamI;

/*
 * variable name -> index translation table
 */
static WCA_CHAR			WCA_Variable[WCA_MAX_NORMAL_VARIABLES]
					[WCA_VARIABLE_NAME_SIZE];


/* path of the loaded file */
static const WCA_CHAR *		WCA_Path;




/*
 * inits the interpreter
 */
void wca_start(void)
{

	memset(WCA_MagicVariable, 0x00, sizeof WCA_MagicVariable);

	memset(WCA_Function, 0x00, sizeof WCA_Function);

	wca_add_function("get",		wca_function_get);
	wca_add_function("set",		wca_function_set);
	wca_add_function("store",	wca_function_store);
	wca_add_function("expr",	wca_function_expr);
	
	wca_add_function("if",		wca_function_if);
	wca_add_function("else_if",	wca_function_else_if);
	wca_add_function("else",	wca_function_else);
	wca_add_function("end_if",	wca_function_end_if);
	wca_add_function("loop",	wca_function_loop);
	wca_add_function("continue",	wca_function_continue);
	wca_add_function("break",	wca_function_break);
	wca_add_function("end_loop",	wca_function_end_loop);
	wca_add_function("end",		wca_function_end);
}



/*
 * interpreter clean up
 */
void wca_end(void)
{

	WCA_N_MagicVariables = 0;
	WCA_N_Functions = 0;
}



/*
 * adds a function to the interpreter
 */
void wca_add_function(const WCA_CHAR *name, void (*function)(void)) 
{
	WCA_FUNCTION_LINK *link;
	
	if (WCA_N_Functions == WCA_MAX_FUNCTIONS) return;
	
	link = &WCA_Function[WCA_N_Functions++];
	
	strcpy(link->name, name);
	link->call = function;
}



/*
 * loads and executes a program
 */
bool wca_run(const WCA_CHAR *path)
{

	if (!wca_load(path)) {

		return false;
	}

	wca_execute();

	return true;
}



/*
 * loads a program
 */
bool wca_load(const WCA_CHAR *path)
{
	FILE *stream;
	bool function_read = false;
	bool is_comment = false;
	bool interpret = true;
	bool character_read = false;

	WCA_Path = path;

	stream = fopen(WCA_Path, "rb");
	if (stream == NULL) return false;

	/* init program */
	memset(WCA_Program, 0, sizeof WCA_Program);

	/* init storage */
	memset(WCA_Storage, 0, sizeof WCA_Storage);

	WCA_Call = WCA_Program;

	WCA_LineNumber = 1;
	WCA_InstructionNumber = 0;
	WCA_LiteralNumber = 0;
	WCA_VariableNumber = 0;
	WCA_Depth = 0;
	WCA_ParamI = 0;

	*WCA_Call = WCA_CallTemplate;

	WCA_NEW_TOKEN;

	while (true) {
		int c;

		c = getc(stream);

		if (c == EOF && !character_read) break;

		if (c == WCA_OVERRIDE) {

			WCA_Token[WCA_TokenI++] = getc(stream);
			character_read = true;

			continue;

		} else if (c == WCA_COMMENT) {

			is_comment = (is_comment ? false : true);

			continue;
		}

		if (is_comment) continue;

		if (c == WCA_INTERPRET) {

			interpret = (interpret ? false : true);

			continue;
		}

		if (!interpret) {

			WCA_Token[WCA_TokenI++] = c;
			character_read = true;

			continue;
		}

		/* handle new lines / EOF */
		if (c == WCA_CR || c == WCA_LF || c == EOF) {

			++WCA_LineNumber;

			if (c == WCA_CR) {
				c = getc(stream);
				if (c != WCA_LF) ungetc(c, stream);
			}

			if (!function_read && !character_read) continue;

			/* call terminated by new line / EOF */
			++WCA_InstructionNumber;

			if (function_read) {

				/* last token is parameter */
				wca_process_parameter_token();

			} else {

				/* last token is function name */
				wca_process_function_token();
			}

			function_read = false;
			character_read = false;
			WCA_ParamI = 0;
			++WCA_Call;
			WCA_NEW_TOKEN;

			continue;
		}

		if (!isspace(c)) {

			if (!character_read && !function_read) {

				/* new call started */
				*WCA_Call = WCA_CallTemplate;
			}

			WCA_Token[WCA_TokenI++] = c;
			character_read = true;
			continue;
		}

		if (!character_read) continue;

		if (!function_read) {

			/* function read */
			wca_process_function_token();

			function_read = true;

			WCA_NEW_TOKEN;
			character_read = false;

		} else {

			/* parameter read */
			wca_process_parameter_token();

			++WCA_ParamI;

			WCA_NEW_TOKEN;
			character_read = false;
		}
	}

	fclose(stream);

	return true;
}



/*
 * executes the loaded program
 */
void wca_execute(void)
{

	memset(WCA_Execute, false, WCA_EXECUTE_SIZE);
	*WCA_Execute = true;
	WCA_Ip = 0;
	WCA_IfJump = false;

	while (WCA_Ip < WCA_N_INSTRUCTIONS) {

		WCA_Call = &WCA_Program[WCA_Ip];

		if (WCA_Execute[WCA_Call->depth]) {
			void (*call)(void);

			call = WCA_Function[WCA_Call->function].call;

			(*call)();
		}

		++WCA_Ip;
	}
}



/*
 * returns the parameters of the current function call
 */
const WCA_STORAGE_INDEX * wca_call_parameters(void)
{

	return WCA_Call->param;
}



/*
 * returns the content of storage block in numeric form
 */
WCA_NUMBER wca_number(WCA_STORAGE_INDEX idx)
{

	return atoi((const char *)WCA_Storage[idx]);
}



/*
 * returns the content of storage block in string form
 */
const WCA_CHAR * wca_string(WCA_STORAGE_INDEX idx)
{

	return (const char *)WCA_Storage[idx];
}



/*
 * stores a number
 */
void wca_store_number(WCA_STORAGE_INDEX idx, WCA_NUMBER number)
{
	static const WCA_CHAR digit[] = "0123456789";
	WCA_CHAR buf[WCA_NUMBER_MAX_DIGITS + 1];
	WCA_CHAR *p;
	WCA_BYTE *s;

	s = WCA_Storage[idx];

	if (number < 0) {

		*s++ = '-';

		number = -number;
	}

	p = buf + sizeof buf;
	*--p = '\0';

	do {
		*--p = digit[number % 10];

		number /= 10;

	} while (number != 0);

	while (*p != '\0') *s++ = *p++;
	*s = '\0';
}



/*
 * stores a string
 */
void wca_store_string(WCA_STORAGE_INDEX idx, const WCA_CHAR *string)
{
	WCA_BYTE *p;

	wca_clear_block(idx);

	p = WCA_Storage[idx];

	while (*string != '\0') *p++ = *string++;
}



/*
 * returns the storage index of magic variable
 */
WCA_STORAGE_INDEX wca_magic_variable(const WCA_CHAR *name)
{
	WCA_VARIABLE_NUMBER i;
	
	for (i = 0; i < WCA_MAX_MAGIC_VARIABLES; i++) {

		if (strcmp(name, WCA_MagicVariable[i]) == 0) {

			return WCA_MAGIC_VARIABLE(i);
		}
	}

	return WCA_NIL;	
}



/*
 * adds a magic variable to the interpreter
 */
void wca_magic_variable_add(const WCA_CHAR *name)
{
	
	if (WCA_N_MagicVariables == WCA_MAX_MAGIC_VARIABLES) return;
	
	strcpy(WCA_MagicVariable[WCA_N_MagicVariables++], name);
}



/*
 * sets the value of a magic variable
 */
void wca_magic_variable_set(const WCA_CHAR *name, const WCA_CHAR *value)
{
	WCA_STORAGE_INDEX idx;
	
	idx = wca_magic_variable(name);

	wca_store_string(idx, value);
}



/*
 * processes a function token
 */
static void wca_process_function_token(void)
{
	WCA_FUNCTION_INDEX i;

	WCA_Call->depth = WCA_Depth;

	for (i = 0; i < WCA_MAX_FUNCTIONS; i++) {

		if (strcmp(WCA_Token, WCA_Function[i].name) != 0) continue;

		switch (i) {

		/* new logical block started */
		case WCA_FUNCTION_IF:
			++WCA_Depth;
			break;

		case WCA_FUNCTION_ELSE_IF:
		case WCA_FUNCTION_ELSE:
			--WCA_Call->depth;
			break;

		case WCA_FUNCTION_LOOP:
			WCA_LoopJump[WCA_Call->depth] =
				WCA_InstructionNumber - 1;
			++WCA_Depth;
			break;

		/* end of logical block */
		case WCA_FUNCTION_END_IF: /* FALLTHROUGH */
		case WCA_FUNCTION_END_LOOP:
			WCA_Call->depth = --WCA_Depth;
			break;
		}

		WCA_Call->function = i;
		return;
	}
}



/*
 * processes a function parameter token
 */
static void wca_process_parameter_token(void)
{

	if (WCA_Token[0] == WCA_VARIABLE_PREFIX) {

		/* it is a variable */

		if (islower(WCA_Token[1])) {

			/* it is a normal variable */
			wca_process_variable_token();

		} else {

			/* it is a magic variable */
			WCA_Call->param[WCA_ParamI] = 
				wca_magic_variable(&WCA_Token[1]);
		}

	} else {

		/* it is a literal */
		wca_process_literal_token();
	}
}



/*
 * processes a variable token
 */
static void wca_process_variable_token(void)
{
	WCA_VARIABLE_NUMBER i;
	const WCA_CHAR *variable_name;

	variable_name = &WCA_Token[1];

	for (i = 0; i < WCA_VariableNumber; i++) {

		if (strcmp(variable_name, WCA_Variable[i]) == 0) {

			break;
		}
	}

	if (i == WCA_VariableNumber) {
		/* variable not found - add it */

		strcpy(WCA_Variable[i], variable_name);

		++WCA_VariableNumber;
	}

	WCA_Call->param[WCA_ParamI] = i;
}



/*
 * processes a literal token
 */
static void wca_process_literal_token(void)
{
	WCA_STORAGE_INDEX i;

	/* it is NIL */
	if (WCA_Token[0] == '\0') return;

	i = WCA_LITERAL(WCA_LiteralNumber++);

	memcpy(WCA_Storage[i], WCA_Token, WCA_TokenI);

	WCA_Call->param[WCA_ParamI] = i;
}






/*
 *
 * Wicca functions
 *
 */

/*
 * function: get (return value, source, byte index)
 */
static void wca_function_get(void)
{
	WCA_STORAGE_INDEX *param;
	WCA_BYTE_INDEX byte_idx;

	param = WCA_Call->param;

	byte_idx = wca_number(param[2]) - 1;

	wca_clear_block(param[0]);

	WCA_Storage[param[0]][0] = WCA_Storage[param[1]][byte_idx];
}



/*
 * function: set (to modify, byte index, byte)
 */
static void wca_function_set(void)
{
	WCA_STORAGE_INDEX *param;
	WCA_BYTE_INDEX byte_idx;
	WCA_BYTE byte;

	param = WCA_Call->param;

	byte_idx = wca_number(param[1]) - 1;
	byte = wca_number(param[2]);

	WCA_Storage[param[0]][byte_idx] = byte;
}



/*
 * function: store
 */
static void wca_function_store(void)
{

	if (WCA_Call->param[1] == WCA_NIL) {

		wca_clear_block(WCA_Call->param[0]);

		return;
	}

	wca_copy_block(WCA_Call->param[0], WCA_Call->param[1]);
}



/*
 * function: expr
 */
static void wca_function_expr(void)
{
	static WCA_NUMBER stack[WCA_EXPR_STACK_SIZE];
	static WCA_NUMBER s_top;
	WCA_PARAMETER_INDEX i;
	WCA_STORAGE_INDEX *param;

	param = WCA_Call->param;

	for (i = 1, s_top = 0;
		i < WCA_MAX_FUNCTION_PARAMETERS;
		i++) {
		WCA_STORAGE_INDEX p;
		WCA_NUMBER r = 0;

		p = param[i];

		if (p == WCA_NIL) break;

		switch (WCA_Storage[p][0]) {
		case '+':
			r = stack[s_top - 2] + stack[s_top - 1];
			s_top -= 2;
			break;
		case '-':
			r = stack[s_top - 2] - stack[s_top - 1];
			s_top -= 2;
			break;
		case '*':
			r = stack[s_top - 2] * stack[s_top - 1];
			s_top -= 2;
			break;
		case '/':
			r = stack[s_top - 2] / stack[s_top - 1];
			s_top -= 2;
			break;
		case '%':
			r = stack[s_top - 2] % stack[s_top - 1];
			s_top -= 2;
			break;
		default:
			r = wca_number(p);
		}

		stack[s_top++] = r;
	}

	wca_store_number(WCA_Call->param[0], stack[--s_top]);
}



/*
 * function: if
 */
static void wca_function_if(void)
{
	bool result = false;
	WCA_STORAGE_INDEX *param;

	param = WCA_Call->param;

	if (wca_block_equals_string(param[1], "=")) {

		if (wca_blocks_equal(WCA_Call->param[0],
			WCA_Call->param[2])) {

			result = true;
		}

	} else if (wca_block_equals_string(param[1], "!=")) {

		if (!wca_blocks_equal(WCA_Call->param[0],
			WCA_Call->param[2])) {

			result = true;
		}

	} else if (wca_block_equals_string(param[1], ">")) {

		if (wca_number(WCA_Call->param[0]) >
			wca_number(WCA_Call->param[2])) {

			result = true;
		}

	} else if (wca_block_equals_string(param[1], "<")) {

		if (wca_number(WCA_Call->param[0]) <
			wca_number(WCA_Call->param[2])) {

			result = true;
		}
	} else if (wca_block_equals_string(param[1], ">=")) {

		if (wca_number(WCA_Call->param[0]) >=
			wca_number(WCA_Call->param[2])) {

			result = true;
		}

	} else if (wca_block_equals_string(param[1], "<=")) {

		if (wca_number(WCA_Call->param[0]) <=
			wca_number(WCA_Call->param[2])) {

			result = true;
		}
	}

	WCA_Execute[WCA_Call->depth + 1] = result;
}



/*
 * function: else_if
 */
static void wca_function_else_if(void)
{

	if (WCA_IfJump) return;

	if (WCA_Execute[WCA_Call->depth + 1]) {
		
		WCA_Execute[WCA_Call->depth + 1] = false;
		WCA_IfJump = true;
		return;
	}

	wca_function_if();
}



/*
 * function: else
 */
static void wca_function_else(void)
{
	
	if (WCA_IfJump) return;

	if (WCA_Execute[WCA_Call->depth + 1]) {

		WCA_Execute[WCA_Call->depth + 1] = false;
		WCA_IfJump = true;
		return;
	}

	WCA_Execute[WCA_Call->depth + 1] = true;
}



/*
 * function: end_if
 */
static void wca_function_end_if(void)
{

	WCA_IfJump = false;

	WCA_Execute[WCA_Call->depth + 1] = false;
}



/*
 * function: loop
 */
static void wca_function_loop(void)
{

	WCA_CurrentLoop = WCA_Call->depth;

	WCA_Execute[WCA_CurrentLoop + 1] = true;
}



/*
 * function: continue
 */
static void wca_function_continue(void)
{

	WCA_Ip = WCA_LoopJump[WCA_CurrentLoop];
}



/*
 * function: end_loop
 */
static void wca_function_end_loop(void)
{

	WCA_Ip = WCA_LoopJump[WCA_CurrentLoop];
}



/*
 * function: break
 */
static void wca_function_break(void)
{
	WCA_NUMBER nw;

	nw = 1;

	while (++WCA_Ip < WCA_N_INSTRUCTIONS) {

		++WCA_Call;

		switch (WCA_Call->function) {
		case WCA_FUNCTION_LOOP: ++nw; break;
		case WCA_FUNCTION_END_LOOP:
			--nw;
			if (nw == 0) return;
			break;
		}
	}

	puts("invalid break");
	exit(1);
}



/*
 * function: end
 */
static void wca_function_end(void)
{

	WCA_Ip = WCA_InstructionNumber;
}





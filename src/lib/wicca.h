/*
 * Wicca - a Public Domain Wicca interpreter
 *
 * Version ALPHA
 *
 * NAMESPACE: WCA_ / wca_ 
 *
 */

/*
 * limits
 */
#define WCA_MAX_NORMAL_VARIABLES	128	
#define WCA_MAX_LITERALS		1028
#define WCA_MAX_MAGIC_VARIABLES		64
#define WCA_MAGIC_VARIABLE_NAME_SIZE	64

/* function name size */
#define WCA_FUNCTION_NAME_SIZE		64

/* max. number of functions */
#define WCA_MAX_FUNCTIONS		256

/* maximal number of function parameters */
#define WCA_MAX_FUNCTION_PARAMETERS	8

/* variable / literal size limit */
#define WCA_STORAGE_BLOCK_SIZE		256

/* variable name size */
#define WCA_VARIABLE_NAME_SIZE		64

/* maximal number of instructions per program */
#define WCA_MAX_INSTRUCTIONS		8000

/* maximal conditional / loop nesting depth */
#define WCA_MAX_DEPTH			16

/* max. WCA_NUMBER digits */
#define WCA_NUMBER_MAX_DIGITS		31

/* expr function stack size */
#define WCA_EXPR_STACK_SIZE		16


/* NIL value */
#define WCA_NIL				-1

/* prototype macro for C functions which are meant to be exported to Wicca */
#define WCA_C_FUNCTION(f)	void (f)(void)



/*
 * types
 */
typedef char			WCA_CHAR;
typedef int			WCA_NUMBER;

typedef int			WCA_STORAGE_INDEX;

typedef int			WCA_PARAMETER_INDEX;



void				wca_start(void);
void				wca_end(void);

void				wca_add_function(const WCA_CHAR *, 
					void (*)(void)
				);

bool				wca_run(const WCA_CHAR *);

bool				wca_load(const WCA_CHAR *);
void				wca_execute(void);

const WCA_STORAGE_INDEX *	wca_call_parameters(void);

WCA_NUMBER			wca_number(WCA_STORAGE_INDEX);
const WCA_CHAR *		wca_string(WCA_STORAGE_INDEX);

void				wca_store_number(WCA_STORAGE_INDEX,
					WCA_NUMBER
				);
				
void				wca_store_string(WCA_STORAGE_INDEX,
					const WCA_CHAR *
				);

WCA_STORAGE_INDEX		wca_magic_variable(const WCA_CHAR *);
void				wca_magic_variable_add(const WCA_CHAR *);
void				wca_magic_variable_set(const WCA_CHAR *, 
					const WCA_CHAR *
				);




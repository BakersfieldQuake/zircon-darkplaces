// prvm_exec_baker_track.c.h

// globals (sorted by index or by name)
// fields (sorted by index or by name)
// functions (sorted by file) function prototypes
// locals  with type indicating constant or no
RELATED_ (mfunction_t)
//typedef struct _bfunction_s
//{
//REF___	mfunction_t		ref;
//		char			name[64];
//		char			protoype[128];
//}
//bfunction_s;

typedef struct _bprog_s {
	stringlist_t	list_files;			// numstatements ccs *filename = PRVM_GetString(prog, f->s_file);

	RELATED_ (stringlist_append_functions_unique prog->numglobals)
	stringlist_t	list_functions;		// stringlist_append_functions_unique prog->numfunctions

	stringlist_t	list_globals;		// numglobals
	stringlist_t	list_fields;		// 
} bprog_s;
RELATED_ (PRVM_Functions_f stringlist_append_functions_unique prog->numfunctions)


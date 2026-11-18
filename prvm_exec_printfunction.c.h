// prvm_exec_printfunction.c.h



/*
=================
PRVM_PrintStatement
=================
*/
extern cvar_t prvm_coverage;
extern cvar_t prvm_statementprofiling;
extern cvar_t prvm_timeprofiling;
#define Logf BakerString_CatCFmt
#define LogLinef BakerString_CatCFmt

////////////////////////////////////////
#include "prvm_exec_baker_track.c.h"
#include "prvm_exec_decompile.c.h"


static void PRVM_PrintStatement(prvm_prog_t *prog, mstatement_t *s)
{
	int opnum = (int)(s - prog->statements);
	char valuebuf[MAX_INPUTLINE_16384];
	const char *opname;

	Con_Printf ( "s%d: ", opnum);
	if ( prog->statement_linenums ) { // .LNO available
		ccs *filename = PRVM_GetString( prog, prog->xfunction->s_file_m );
		if ( prog->statement_columnnums )
			Con_Printf ( "%s:%d:%d: ", filename , prog->statement_linenums[ opnum ], prog->statement_columnnums[ opnum ] );
		else
			Con_Printf ( "%s:%d: ", filename, prog->statement_linenums[ opnum ] );
	}

	if (prvm_statementprofiling.integer /*d:9*/)
		Con_Printf ( "%7.0f ", prog->statement_profile[s - prog->statements]);

	// DEV: August 18 2026 - it checks out.
	//size_t numy1 = sizeof(prvm_opnames)/sizeof(prvm_opnames[0]);
	//size_t numy2 = ARRAY_COUNT(prvm_opnames);

	if ( (unsigned)s->op < ARRAY_COUNT(prvm_opnames) && prvm_opnames[s->op])
		opname = prvm_opnames[s->op];
	else
		opname = valuebuf, dpsnprintf(valuebuf, sizeof(valuebuf), "OPCODE_%u", (unsigned)s->op);

	Con_Printf ( "%s ",  opname);
	size_t i = strlen(opname);
	// don't count a preceding color tag when padding the name
	if (opname[0] == STRING_COLOR_TAG)
		i -= 2;
	for ( ; i < 10 ; i++)
		Con_Printf ( " ");

	if (s->operand[0] >= 0) Con_Printf (   "%s", PRVM_GlobalString(prog, s->operand[0], valuebuf, sizeof(valuebuf)));
	if (s->operand[1] >= 0) Con_Printf ( ", %s", PRVM_GlobalString(prog, s->operand[1], valuebuf, sizeof(valuebuf)));
	if (s->operand[2] >= 0) Con_Printf ( ", %s", PRVM_GlobalString(prog, s->operand[2], valuebuf, sizeof(valuebuf)));
	if (s->jumpabsolute >= 0) Con_Printf ( ", statement %d", s->jumpabsolute);
	Con_Printf ( NEWLINE);
}

CALLERS_ ()
void PRVM_PrintFunctionStatements (prvm_prog_t *prog, ccs *name)
{
	mfunction_t *func = PRVM_ED_FindFunction (prog, name);
	if (!func) {
		Con_PrintLinef ("%s progs: no function named %s", prog->name, name);
		return;
	}
	int firststatement = func->first_statement;
	if (firststatement < 0) {
		Con_PrintLinef ("%s progs: function %s is builtin #%d", prog->name, name, -firststatement);
		return;
	}

	// find the end statement
	int endstatement = prog->numstatements;
	for (int i = 0; i < prog->numfunctions; i ++) {
		if (endstatement > prog->functions[i].first_statement 
			&& firststatement < prog->functions[i].first_statement)

			endstatement = prog->functions[i].first_statement;
	} // for prog->numfunctions

	// now print the range of statements
	Con_PrintLinef ("%s progs: disassembly of function %s (statements %d-%d, locals %d-%d):", prog->name, name, firststatement, endstatement, func->parm_start, func->parm_start + func->locals - 1);
	prog->xfunction = func;
	for (int i = firststatement; i < endstatement; i ++) {
		PRVM_PrintStatement (prog, prog->statements + i);
		if (!(prvm_coverage.integer /*d:0*/ & 4)) // DEVNOTE: per-statement coverage is 4
			prog->statement_profile[i] = 0;
	} // < endstatement

	if (prvm_coverage.integer /*d:0*/ & 4)
		Con_PrintLinef ("Collecting statement coverage, not flushing statement profile.");
}

/*
============
PRVM_PrintFunction_f

============
*/
void PRVM_PrintFunction_f(cmd_state_t *cmd)
{	
	if (cmd_argc != 3) {
		Con_PrintLinef ("usage: prvm_printfunction <program name> <function name>");
		return;
	}

	ccs *s_progname = cmd_argv_parm1;
	ccs *s_function_name = cmd_argv_parm2;

	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return;
	}

	//baker_string_s *bsa = BakerString_Create_Malloc (""); 
	PRVM_PrintFunctionStatements (prog, s_function_name);
	//BakerString_ConPrintLines (bsa);
	//BakerString_Destroy_And_Null_It (&bsa);
}

#include "prvm_exec_prototype_locals.c.h"
void PRVM_PrintFunctionPrototype_f(cmd_state_t *cmd)
{	
	if (cmd_argc != 3) {
		Con_PrintLinef ("usage: %s <program name> <function name>", cmd_argv_command_0);
		return;
	}

	ccs *s_progname = cmd_argv_parm1;
	ccs *s_function_name = cmd_argv_parm2;

	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return;
	}

	//baker_string_s *bsa = BakerString_Create_Malloc (""); 
    if (!prog || !s_function_name || !s_function_name[0]) return;

    mfunction_t *f = PRVM_ED_FindFunction(prog, s_function_name);
    if (!f) {
        Con_PrintLinef ("Function '%s' not found in program.", s_function_name);
        return;
    }

	RELATED_ (AnalyzeAndPrintLocals)
	PRVM_PrintFunctionPrototype2 (prog, f, s_function_name);
	
	//BakerString_ConPrintLines (bsa);
	//BakerString_Destroy_And_Null_It (&bsa);
}


void PRVM_DecompileFunction_f(cmd_state_t *cmd)
{	
	if (cmd_argc != 3) {
		Con_PrintLinef ("usage: prvm_decompilefunction <program name> <function name>");
		return;
	}

	ccs *s_progname = cmd_argv_parm1;
	ccs *s_function_name = cmd_argv_parm2;

	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return;
	}

#if 1 
	baker_string_s *bsa = BakerString_Create_Malloc (""); 
	PRVM_PrintFunctionStatementsByName /*Decompile*/(/*bsa,*/ prog, s_function_name);
	BakerString_ConPrintLines (bsa);
	BakerString_Destroy_And_Null_It (&bsa);
#else
	baker_string_s *bsa = BakerString_Create_Malloc (""); 
	PRVM_PrintFunctionStatements /*Decompile*/(bsa, prog, s_function_name);
	BakerString_ConPrintLines (bsa);
	BakerString_Destroy_And_Null_It (&bsa);
#endif
}


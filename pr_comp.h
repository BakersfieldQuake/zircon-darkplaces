/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// this file is shared by quake and qcc

#ifndef PR_COMP_H
#define PR_COMP_H

typedef unsigned int	func_t;
typedef int	string_t;

typedef enum etype_e {ev_void, ev_string, ev_float, ev_vector, ev_entity, ev_field, ev_function, ev_pointer} etype_t;


#define ev_void_0		ev_void
#define ev_string_1		ev_string
#define ev_float_2		ev_float
#define ev_vector_3		ev_vector
#define ev_entity_4		ev_entity
#define ev_field_5		ev_field
#define ev_function_6	ev_function
#define ev_pointer_7	ev_pointer		// Baker: Level of support in DP uncertain, DarkPlaces 2014 does have it


#define	OFS_NULL		0
#define	OFS_RETURN		1
#define	OFS_PARM0		4		// leave 3 ofs for each parm to hold vectors
#define	OFS_PARM1		7
#define	OFS_PARM2		10
#define	OFS_PARM3		13
#define	OFS_PARM4		16
#define	OFS_PARM5		19
#define	OFS_PARM6		22
#define	OFS_PARM7		25
#define	RESERVED_OFS	28


typedef enum opcode_e
{
	// NOTE: List mostly generated using `./fteqcc.bin -TDP_20250104 -Fdumpopcodes`.
	OP_DONE = 0,
	OP_MUL_F = 1,
	OP_MUL_V = 2,
	OP_MUL_FV = 3,
	OP_MUL_VF = 4,
	OP_DIV_F = 5,
	OP_ADD_F = 6,
	OP_ADD_V = 7,
	OP_SUB_F = 8,
	OP_SUB_V = 9,

	OP_EQ_F = 10,
	OP_EQ_V = 11,
	OP_EQ_S = 12,
	OP_EQ_E = 13,
	OP_EQ_FNC = 14,

	OP_NE_F = 15,
	OP_NE_V = 16,
	OP_NE_S = 17,
	OP_NE_E = 18,
	OP_NE_FNC = 19,

	OP_LE = 20, // Baker: FTE OP_LE_F
	OP_GE = 21,
	OP_LT = 22,
	OP_GT = 23,

	OP_LOAD_F = 24,
	OP_LOAD_V = 25,
	OP_LOAD_S = 26,
	OP_LOAD_ENT = 27,
	OP_LOAD_FLD = 28,
	OP_LOAD_FNC = 29,

	OP_ADDRESS = 30,

	OP_STORE_F = 31,
	OP_STORE_V = 32,
	OP_STORE_S = 33,
	OP_STORE_ENT = 34,
	OP_STORE_FLD = 35,
	OP_STORE_FNC = 36,

	OP_STOREP_F = 37,
	OP_STOREP_V = 38,
	OP_STOREP_S = 39,
	OP_STOREP_ENT = 40,
	OP_STOREP_FLD = 41,
	OP_STOREP_FNC = 42,

	OP_RETURN = 43,
	OP_NOT_F = 44,
	OP_NOT_V = 45,
	OP_NOT_S = 46,
	OP_NOT_ENT = 47,
	OP_NOT_FNC = 48,
	OP_IF = 49,
	OP_IFNOT = 50,
	OP_CALL0 = 51,
	OP_CALL1 = 52,
	OP_CALL2 = 53,
	OP_CALL3 = 54,
	OP_CALL4 = 55,
	OP_CALL5 = 56,
	OP_CALL6 = 57,
	OP_CALL7 = 58,
	OP_CALL8 = 59,
	OP_STATE = 60,
	OP_GOTO = 61,
	OP_AND = 62,
	OP_OR = 63,

	OP_BITAND = 64,
	OP_BITOR = 65,

	// TODO: actually support Hexen 2?
	
	OP_MULSTORE_F = 66,	//66 redundant, for h2 compat
	OP_MULSTORE_VF = 67,	//67 redundant, for h2 compat
	OP_MULSTOREP_F = 68,	//68
	OP_MULSTOREP_VF = 69,//69

	OP_DIVSTORE_F = 70,	//70 redundant, for h2 compat
	OP_DIVSTOREP_F = 71,	//71

	OP_ADDSTORE_F = 72,	//72 redundant, for h2 compat
	OP_ADDSTORE_V = 73,	//73 redundant, for h2 compat
	OP_ADDSTOREP_F = 74,	//74
	OP_ADDSTOREP_V = 75,	//75

	OP_SUBSTORE_F = 76,	//76 redundant, for h2 compat
	OP_SUBSTORE_V = 77,	//77 redundant, for h2 compat
	OP_SUBSTOREP_F = 78,	//78
	OP_SUBSTOREP_V = 79,	//79

	OP_FETCH_GBL_F = 80,	//80 has built-in bounds check
	OP_FETCH_GBL_V = 81,	//81 has built-in bounds check
	OP_FETCH_GBL_S = 82,	//82 has built-in bounds check
	OP_FETCH_GBL_E = 83,	//83 has built-in bounds check
	OP_FETCH_GBL_FNC = 84,//84 has built-in bounds check

	OP_CSTATE = 85,		//85
	OP_CWSTATE = 86,		//86

	OP_THINKTIME = 87,	//87 shortcut for OPA.nextthink=time+OPB

	OP_BITSETSTORE_F = 88,	//88 redundant, for h2 compat
	OP_BITSETSTOREP_F = 89,	//89
	OP_BITCLRSTORE_F = 90,	//90
	OP_BITCLRSTOREP_F = 91,	//91

	OP_RAND0 = 92,		//92	OPC = random()
	OP_RAND1 = 93,		//93	OPC = random()*OPA
	OP_RAND2 = 94,		//94	OPC = random()*(OPB-OPA)+OPA
	OP_RANDV0 = 95,		//95	//3d/box versions of the above.
	OP_RANDV1 = 96,		//96
	OP_RANDV2 = 97,		//97

	OP_SWITCH_F = 98,	//98	switchref=OPA; PC += OPB   --- the jump allows the jump table (such as it is) to be inserted after the block.
	OP_SWITCH_V = 99,	//99
	OP_SWITCH_S = 100,	//100
	OP_SWITCH_E = 101,	//101
	OP_SWITCH_FNC = 102,	//102

	OP_CASE = 103,		//103	if (OPA===switchref) PC += OPB
	OP_CASERANGE = 104,	//104   if (OPA<=switchref&&switchref<=OPB) PC += OPC

	//hexen2 calling convention (-TH2 requires us to remap OP_CALLX to these on load, -TFTE just uses these directly.)
	OP_CALL1H = 105,	//OFS_PARM0=OPB
	OP_CALL2H = 106,	//OFS_PARM0,1=OPB,OPC
	OP_CALL3H = 107,	//no extra args
	OP_CALL4H = 108,
	OP_CALL5H = 109,
	OP_CALL6H = 110,
	OP_CALL7H = 111,
	OP_CALL8H = 112,

	OP_STORE_I = 113,
	OP_STORE_IF = 114,
	OP_STORE_FI = 115,

	OP_ADD_I = 116,
	OP_ADD_FI = 117,
	OP_ADD_IF = 118,

	OP_SUB_I = 119,
	OP_SUB_FI = 120,
	OP_SUB_IF = 121,

	OP_CONV_IF = 122,
	OP_CONV_FI = 123,
	
	OP_LOADP_IF = 124,
	OP_LOADP_FI = 125,

	OP_LOAD_I = 126,

	OP_STOREP_I = 127,
	OP_STOREP_IF = 128,
	OP_STOREP_FI = 129,

	OP_BITAND_I = 130,
	OP_BITOR_I = 131,

	OP_MUL_I = 132,
	OP_DIV_I = 133,
	OP_EQ_I = 134,
	OP_NE_I = 135,

	OP_IFNOT_S = 136,

	OP_IF_S = 137,

	OP_NOT_I = 138,

	OP_DIV_VF = 139,

	OP_BITXOR_I = 140,
	OP_RSHIFT_I = 141,
	OP_LSHIFT_I = 142,

	OP_GLOBALADDRESS = 143,
	OP_ADD_PIW = 144,

	OP_LOADA_F = 145,
	OP_LOADA_V = 146,	
	OP_LOADA_S = 147,
	OP_LOADA_ENT = 148,
	OP_LOADA_FLD = 149,
	OP_LOADA_FNC = 150,
	OP_LOADA_I = 151,

	OP_STORE_P = 152,
	OP_LOAD_P = 153,

	OP_LOADP_F = 154,
	OP_LOADP_V = 155,	
	OP_LOADP_S = 156,
	OP_LOADP_ENT = 157,
	OP_LOADP_FLD = 158,
	OP_LOADP_FNC = 159,
	OP_LOADP_I = 160,

	OP_LE_I = 161,
	OP_GE_I = 162,
	OP_LT_I = 163,
	OP_GT_I = 164,
	
	OP_LE_IF = 165,
	OP_GE_IF = 166,
	OP_LT_IF = 167,
	OP_GT_IF = 168,

	OP_LE_FI = 169,
	OP_GE_FI = 170,
	OP_LT_FI = 171,
	OP_GT_FI = 172,

	OP_EQ_IF = 173,
	OP_EQ_FI = 174,

	OP_ADD_SF = 175,
	OP_SUB_S = 176,
	OP_STOREP_C = 177,
	OP_LOADP_C = 178,

	OP_MUL_IF = 179,
	OP_MUL_FI = 180,
	OP_MUL_VI = 181,
	OP_MUL_IV = 182,
	OP_DIV_IF = 183,
	OP_DIV_FI = 184,
	OP_BITAND_IF = 185,
	OP_BITOR_IF = 186,
	OP_BITAND_FI = 187,
	OP_BITOR_FI = 188,
	OP_AND_I = 189,
	OP_OR_I = 190,
	OP_AND_IF = 191,
	OP_OR_IF = 192,
	OP_AND_FI = 193,
	OP_OR_FI = 194,
	OP_NE_IF = 195,
	OP_NE_FI = 196,

	OP_GSTOREP_I = 197,
	OP_GSTOREP_F = 198,
	OP_GSTOREP_ENT = 199,
	OP_GSTOREP_FLD = 200,
	OP_GSTOREP_S = 201,
	OP_GSTOREP_FNC = 202,		
	OP_GSTOREP_V = 203,
	OP_GADDRESS = 204,
	OP_GLOAD_I = 205,
	OP_GLOAD_F = 206,
	OP_GLOAD_FLD = 207,
	OP_GLOAD_ENT = 208,
	OP_GLOAD_S = 209,
	OP_GLOAD_FNC = 210,
	OP_BOUNDCHECK = 211,

	OP_UNUSED = 212,	//used to be OP_STOREP_P, which is now emulated with OP_STOREP_I, fteqcc nor fte generated it
	OP_PUSH = 213,	//push 4octets onto the local-stack (which is ALWAYS poped on function return). Returns a pointer.
	OP_POP = 214,		//pop those ones that were pushed (don't over do it). Needs assembler.
	OP_SWITCH_I = 215,

	OP_GLOAD_V = 216,

	OP_IF_F = 217,		//compares as an actual float, instead of treating -0 as positive.
	OP_IFNOT_F = 218,

	OP_STOREF_V = 219,	//3 elements...
	OP_STOREF_F = 220,	//1 fpu element...
	OP_STOREF_S = 221,	//1 string reference
	OP_STOREF_I = 222,	//1 non-string reference/int
}
opcode_t;

// Statements (16 bit format) - 8 bytes each
typedef struct statement16_s
{
	uint16_t	op;
	int16_t		a, b, c;
}
dstatement16_t;

// Statements (32 bit format) - 16 bytes each
typedef struct statement32_s
{
	uint32_t	op;
	int32_t		a, b, c;
}
dstatement32_t;

// Global and fielddefs (16 bit format) - 8 bytes each
typedef struct ddef16_s
{
	uint16_t	type;		// if DEF_SAVEGLOBAL bit is set
							// the variable needs to be saved in savegames
	uint16_t	ofs;
	int32_t	s_name;
}
ddef16_t, dfield16_t;

// Global and fielddefs (32 bit format) - 12 bytes each
typedef struct ddef32_s
{
	uint32_t	type;		// if DEF_SAVEGLOBAL bit is set
							// the variable needs to be saved in savegames
	uint32_t	ofs;
	int32_t	s_name;
}
ddef32_t, dfield32_t, mdef_t;

#define	DEF_SAVEGLOBAL	(1<<15)

#define	MAX_PARMS_8	8

// Functions - 36 bytes each
typedef struct dfunction_s
{
	int32_t		first_statement;	// negative numbers are builtins
	int32_t		parm_start;			// first local
	int32_t		locals;				// total ints of parms + locals

	int32_t		profile;			// runtime

	int32_t		s_name;				// function name
	int32_t		s_file_d;			// source file defined in

	int32_t		numparms;			// number of args
	uint8_t		parm_size[MAX_PARMS_8]; // and size
}
dfunction_t;

typedef struct mfunction_s
{
	int32_t		first_statement;	// negative numbers are builtins
	int32_t		parm_start;
	int32_t		locals;				// total ints of parms + locals

	// these are doubles so that they can count up to 54bits or so rather than 32bit
	double  tprofile;           // realtime in this function
	double  tbprofile;          // realtime in builtins called by this function (NOTE: builtins also have a tprofile!)
	double	profile;			// runtime
	double	builtinsprofile;	// cost of builtin functions called by this function
	double	callcount;			// times the functions has been called since the last profile call
	double  totaltime;			// total execution time of this function DIRECTLY FROM THE ENGINE
	double	tprofile_total;		// runtime (NOTE: tbprofile_total makes no real sense, so not accumulating that)
	double	profile_total;		// runtime
	double	builtinsprofile_total; // cost of builtin functions called by this function
	int     recursion;

	int32_t		s_name;
	int32_t		s_file_m;			// source file defined in   Baker: ccs *filename = PRVM_GetString(prog, f->s_file);

	int32_t		numparms;
	uint8_t		parm_size[MAX_PARMS_8];
}
mfunction_t;

typedef struct mstatement_s
{
	opcode_t	op;
	int			operand[3]; // always a global or -1 for unused
	int			jumpabsolute; // only used by IF, IFNOT, GOTO
}
mstatement_t;

// Header - 64 bytes
#define	PROG_VERSION_6		6
#define	PROG_VERSION_FTE_7	7


typedef struct dprograms_s
{
	int32_t	version;		// Version (usually 6)
	int32_t	crc;			// CRC-16 of header file. Last 2 bytes are skipped

	// Sizes of and offsets to each section
	uint32_t	ofs_statements;
	uint32_t	numstatements;	// statement 0 is an error

	uint32_t	ofs_globaldefs;
	uint32_t	numglobaldefs;

	uint32_t	ofs_fielddefs;
	uint32_t	numfielddefs;

	uint32_t	ofs_functions;
	uint32_t	numfunctions;	// function 0 is an empty

	uint32_t	ofs_strings;
	uint32_t	numstrings;		// first string is a null string

	uint32_t	ofs_globals;
	uint32_t	numglobals;

	uint32_t	entityfields;
}
dprograms_t;

typedef struct dprograms_v7_s
{	//extended header written by fteqcc.
	dprograms_t	v6;	//for easier casting.

	//debug / version 7 extensions
	uint32_t	ofsfiles;			//ignored. deprecated, should be 0. source files can instead be embedded by simply treating the .dat as a zip.
	uint32_t	ofslinenums;		//ignored. alternative to external .lno files.
	uint32_t	ofsbodylessfuncs;	//unsupported. function names imported from other modules. must be 0.
	uint32_t	numbodylessfuncs;	//unsupported. must be 0.

	uint32_t	ofs_types;			//unsupported+deprecated. rich type info. must be 0.
	uint32_t	numtypes;			//unsupported+deprecated. rich type info. must be 0.
	uint32_t	blockscompressed;	//unsupported. per-block compression. must be 0.

	int32_t		secondaryversion;	//if not known then its kkqwsv's v7, qfcc's v7, or uhexen2's v7, or something. abandon all hope when not recognised.
#define PROG_SECONDARYVERSION16 ((('1'<<0)|('F'<<8)|('T'<<16)|('E'<<24))^(('P'<<0)|('R'<<8)|('O'<<16)|('G'<<24)))	//regular 16bit statements.
#define PROG_SECONDARYVERSION32 ((('1'<<0)|('F'<<8)|('T'<<16)|('E'<<24))^(('3'<<0)|('2'<<8)|('B'<<16)|(' '<<24)))	//statements+globaldefs+fielddefs extended to 32bit.
}
dprograms_v7_t;

#endif


// csg_global_macros.h

#define TIMESTAMP_WAS_SET_PREFIX_0_SPACE_MINUS "0 -"

#define X1_UX_0					0	// tex_axis_s values
#define X1_UY_1					1
#define X1_UZ_2					2
#define X1_UOFFSET_3			3

#define X2_VX_0					0	// tex_axis_t values
#define X2_VY_1					1
#define X2_VZ_2					2
#define X2_VOFFSET_3			3

#define FT_ROTATION_0			0
#define FT_USCALE_1				1
#define FT_VSCALE_2				2
#define FT_SURFACEFLAGS_3		3	// Optional .. // Map editor specific, these do not matter to q3map2
#define FT_CONTENTFLAGS_4		4	// Optional .. // Detail
#define FT_Q2VALUE_5			5	// Optional .. // Quake 2 surface light ... we don't care.

#define CONTENTS_DETAIL 0x08000000  // 134 217 728 2^27 power
#define CONTENTS_STRUCTURAL 0x10000000  //    268,435,456 2^28 power
#define E_CONT_DETAIL               0x8000000	// 134217728  Baker: From q3map2.  Pretty sure map editor sets this.
#define E_CONT_STRUCTURAL           0x10000000  // Baker: JACK sets to not write this.

typedef enum {
    FACE_INVALID_NEG_1      = -1,
    FACE_WEST_0             = 0,
    FACE_EAST_1             = 1,
    FACE_SOUTH_2            = 2,
    FACE_NORTH_3            = 3,
    FACE_FLOOR_4            = 4,
    FACE_CEIL_5             = 5,
    FACE_MAX_6              = 6,

    FACE_AXIS_X_7           = 7,    /* ??  Can't remember the recent plan here.  September 19 2026 */
    FACE_AXIS_Y_8           = 8,
    FACE_AXIS_Z_9           = 9,

    FACE_TRIANGLE_FRIEND_17 = 17,   /* quadrant_triangle_friend_7 */
    FACE_ADJACENT_ANY_18    = 18,    /* quadrant_adjacent_any_8 */
	FACE_DIAGONAL_ADJACENT_ANY_19   = 19,    /* quadrant_diag_adjacent_any_9 */
} _FACE_e;
#define FACE_INT_e int

// Deprecated ...
typedef enum {
    TEXSIDE_NONE_0	 = 0,
	TEXSIDE_NORTH_1  = 1,
    TEXSIDE_SOUTH_2  = 2,
    TEXSIDE_EAST_4   = 4,
    TEXSIDE_WEST_8   = 8,
    TEXSIDE_CEIL_16  = 16,
    TEXSIDE_FLOOR_32 = 32,
    TEXSIDE_ALL_63   = 63,

	TEXSIDE_NORTHWEST_57	= TEXSIDE_NORTH_1 | TEXSIDE_WEST_8 | TEXSIDE_CEIL_16 | TEXSIDE_FLOOR_32,
	TEXSIDE_SOUTHEAST_54	= TEXSIDE_SOUTH_2 | TEXSIDE_EAST_4 | TEXSIDE_CEIL_16 | TEXSIDE_FLOOR_32,


	TEXSIDE_NORTHEAST_53	= TEXSIDE_NORTH_1 | TEXSIDE_EAST_4 | TEXSIDE_CEIL_16 | TEXSIDE_FLOOR_32,
	TEXSIDE_SOUTHWEST_58	= TEXSIDE_SOUTH_2 | TEXSIDE_WEST_8 | TEXSIDE_CEIL_16 | TEXSIDE_FLOOR_32,

} _TEXSIDE_e;

#define TEXSIDE_INT_e int





// Baker: May 23 2025 - to the best of my knowledge, UOFFSET 3 and VOFFSET 3 are texture sized pixels.
// This means we need to know the texture size.

//    0 degrees:	ambientcg/Rock051 [ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.5 0.5
// rot 45:0 degrees:ambientcg/Rock051 [ 0.707107 0 -0.707107 0 ] [ -0.707107 0 -0.707107 0 ] 45 0.5 0.5

// texture is on Y plane                cos(30) 0 sin(30)    -sin(30) 0 -cos(30)
// tx at 0 pixels is ... ?
// ty at 0 pixels is ... ?
// rot 30:0 degrees:ambientcg/Rock051 [ 0.866025 0 -0.5 0 ] [ -0.5 0 -0.866025 0 ] 30 0.5 0.5

// Z is the most likely
// Z 45 degrees:	ambientcg/Rock051 [ 0.707107 0.707107 0 104.706 ] [ 0 0 -1 0 ] 0 0.5 0.5  cos sin
// X 45 degrees:	ambientcg/Rock051 [ 1 0 0 0 ] [ 0 0.707107 -0.707107 -195.216 ] 0 0.5 0.5 (tilt back like ceiling) tx unaffected, ty affected
// Y 45 degrees:	ambientcg/Rock051 [ 0.707107 0 -0.707107 512 ] [ -0.707107 0 -0.707107 212.077 ] 0 0.5 0.5



//
// MAZE GEN
//

typedef enum {
	nsew_solid_neg1		= -1,
	nsew_none_0			= 0,
	nsew_north_1		= 1,
	nsew_south_2		= 2,
	nsew_east_4			= 4,
	nsew_west_8			= 8,

	// HALLS
	nsew_ns_3			= 3,
	nsew_ew_12			= 12,

	// T junctions
	nsew_no_north_14	= 14,
	nsew_no_south_13	= 13,
	nsew_no_east_11		= 11,
	nsew_no_west_7		= 7,

	nsew_all_15			= 15,

	//nsew_northeast_16	= 16,	// Diag
	//nsew_northwest_32	= 32,	// Diag
	//nsew_southeast_64	= 64,	// Diag
	//nsew_southwest_128	= 128,	// Diag
	nsew_ceil_256		= 256,
	nsew_floor_512		= 512,
} nsew_e;



//
// MAZE GEN
//

#define HALL_NORTH_SOUTH_PIPE_CHAR	'|' // 124
#define HALL_EAST_WEST_MINUS_CHAR	'-'	// minus = 45 ascii
#define	HALL_CH_EW_MINUS			'-'
#define HALL_CH_NS_PIPE				'|'

// An intersection has as many connectors as neighbors dictate.
// All neighbors are forced to connect to an intersection.
// If a neighbor next to an intersection must be promoted, then do it.
#define HALL_INTERSECTION_PLUS_CHAR	'+' // plus = 43 ascii . Does not occur!!!!  March 20 2026
#define HALL_ROOM_CHAR_48_ZERO		48	// Aka ZERO '0' Room starts at 48 zero and goes up
#define HALL_ROOM_CHAR_MAX_123		123	// Aka ZERO '0' Room starts at 48 zero and goes up
#define HALL_EMPTY_SPACE_CELL_32	32	//

#define HALL_CH_INTER_PLUS			'+'
#define HALL_CH_NO_NORTH_DOLLAR		'$'
#define HALL_CH_NO_SOUTH_PERCENT	'%'
#define HALL_CH_NO_EAST_LPAREN		'('
#define HALL_CH_NO_WEST_RPAREN		')'

RELATED_ (nsew_e)

#define HALL_GET_CELL(ss, rowz, colz)			ss[(rowz) * numcolumns + (colz)]
#define HALL_GET_CELL_FORCE(ss, rowz, colz)		hall_get_cell_force_char(ss, rowz, colz, numrows, numcolumns)

#define CH_IS_ROOM(ich)							in_range(HALL_ROOM_CHAR_48_ZERO, ich, HALL_ROOM_CHAR_MAX_123)
#define CH_IS_INTER(ich)						(ich == HALL_INTERSECTION_PLUS_CHAR)
#define CH_IS_EW(ich)							(ich == HALL_EAST_WEST_MINUS_CHAR)
#define CH_IS_NS(ich)							(ich == HALL_NORTH_SOUTH_PIPE_CHAR)
#define CH_IS_NSEW_HALL(ich)					isin2(ich, HALL_EAST_WEST_MINUS_CHAR, HALL_NORTH_SOUTH_PIPE_CHAR)
#define CH_IS_EMPTY(ich)						(isin2 (ich, CHAR_NULL_0, HALL_EMPTY_SPACE_CELL_32))

//
// END MAZE GEN
//



#define AFTER_WORLD_1	1
#define ENTITY_WORLD_0	0	// entityx_s	*p_world = &plist->entity[ENTITY_WORLD_0];
#define WORLD_0			0

#define GET_WORLD_0(PLIST, P_WORLD) \
	entityx_s	*P_WORLD = &PLIST->entity[ENTITY_WORLD_0] // Ender

#define GETL_WORLD_0(LIST, P_WORLD) \
	entityx_s	*P_WORLD = &LIST.entity[ENTITY_WORLD_0] // Ender


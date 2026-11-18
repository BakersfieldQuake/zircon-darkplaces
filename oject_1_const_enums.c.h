// oject_1_const_enums.c.h

#include "a_zircon_ui_0_defines.h"
#include "a_zircon_ui_1_macros.h"
#include "a_zircon_ui_2_enums.h"

// Directives ...


#define DO_PRINT							1		// Debug print of property parser

//
RELATED_ (Rect_Clip_Area_Result)
#define CLIPPED_2							2
#define CULLED_1							1
#define UNCULLED_0							0




#define	OK_NOERROR_NULL						NULL
#define	OK_NOERROR_0						0
#define	IERR_PROP_NOT_FOUND_1				1
#define IERR_PROP_SET_ERROR_2				2


 
//#define DOT_GAP_0							0









#define WHITE_TEXTURE_NULL					NULL






RELATED_ (DOUBLE_CLICK_0_5) // Windows double-click time

typedef enum {
	DUMP_DETAIL_SAVE_FILE_0 =				0,		// Save file, no servants "ui_set no args"
													// vid_restart and form state to clipboard.
	DUMP_DETAIL_PLUS_SERVANTS_1 =			1,		
	DUMP_DETAIL_PLUS_CLIPPING_2 = 			2,
	DUMP_DETAIL_NAMES_ONLY_3 = 				3,		// Used to print a list
	DUMP_DETAIL_UNCONTAINED_4 = 			4,		// "uncontained" uses.
	DUMP_DETAIL_SINGLE_LINE_5 = 			5,		// 
	DUMP_DETAIL_NORECURSE_6 = 				6,		// USED.  To dump a single control. ui_set.
} dump_detail_e;


#define POLYGON_2D_2						2
#define POLYGON_2D_FLOAT_SEPARATOR_SPACE	" "
#define POLYGON_2D_ELEMENT_SEPARATOR_COMMA	","
#define COLUMN_SEPARATOR_COMMA				","
#define POLYGON_WHITE_TEXTURE_NULL			NULL	// Polygon draw



#define PROPERTY_COLON_DELIM				":"		// Properties colon delimiter





#define EVENT_VALSTR_NULL				 NULL


RELATED_ (things oject_s)







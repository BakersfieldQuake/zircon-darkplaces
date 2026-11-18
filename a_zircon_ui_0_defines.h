// a_zircon_ui_0_defines.h  - use of #define to set constant values

#pragma once

//
// C vs. QuakeC bridge
//

// The engine form will only support ascii
// The QuakeC will be able to unicode
// Watch out for character range exclusions, they will
// block QuakeC from full unicode support

#define K_DEL K_DELETE  // K_DEL is an official DP/FTE QuakeC key string a function can return

//A
#define ENGINE_CLIPBOARD_ZALLOC() Sys_Clipboard_Get_Text_ZAlloc()
#define ENGINE_ZFREE(x) Mem_FreeNull_(x)

//B
//#define istrlen(x)		(int)strlen(x) // Engine signed compiler warning baker.h
#define string_charcode_at(s, pos) s[pos]
#define string_mid(s, pos) &s[pos]

//C
#define floormod(x,y)	((x) % (y))
#define floordiv(x,y)	((x) / (int)(y))
#define ifloor(X)		(int)(X)
// swapi is a C function in engine code
#define iceil(X)		((int)ceil(X))
#define iround(X)		((int)((X) + 0.5))

//// Not same as q_rint which rounds towards 0, this always rounds up
//#define Math_Round(X) ((int)((X) + 0.5)) // 1.1 ---> 1  and 1.5 ---> 2 and 1.7 ---> 2

//D
#define ktrl			struct _oject_s*
//#define int			int
//#define qbool			qbool
#define PSTRING			const char *
#define PCOLOR			const float *
#define Host_RealTime()	host.realtime

//E
#define zpr				Con_PrintLinef
#define zprf			Con_PrintLinef
#define KP(x)			k->x
#define KLINEP(x)		kline->x
#define KPA(x)			&k->x // "AddressOf" form in engine
#define KXP(k,x)		k->x
#define KXPA(k,x)		&k->x
#define KCLASS(x)		x->po->enum_id
#define FP(x)			f->x
#define KFP(x)			k->fctrl->x
#define ADDRESSOF(x)	&x	// QC does not do addressof here

#define string_ascii_at(s,pos) \
	s[pos] // Ender
	//int ch = k->text_a[startpos];

// Engine - a float, QC a vector.
#define KFONTSIZECEIL(k)	ceil(k->fontsize)
#define KFONTSIZE(k)		k->fontsize

//PSTRING s = k->list_strings_a.strings[n];
#define SLISTGETINDEX(ray, idx) \
	ray.strings[idx]

// k->columnwidths_a.ints[col] = newwidth;
//stringlist_set_indexf (k.columnwidths_a, col, newwidth); //k.columnwidths_a.ints[col] = newwidth;
#define INTS_SET_INDEX_VALUE(ray, idx, val) \
	ray.ints[idx] = val

#define SLISTCOUNT(X) \
	(X).numstrings

#define SLISTRESET(X) \
	stringlistfreecontents (X) // Ender


#define RECT_COPY(dst,src) \
	dst = src // Ender

#define VEC3_COPY(dst,src) \
	dst[0] = src[0], dst[1] = src[1], dst[2] = src[2]

#define VEC3_SET(dst, a, b, c) \
	dst[0] = a, dst[1] = b, dst[2] = c

// c_assert_msg_ is in baker.h

// #define printvarf(var) Con_PrintLinef (STRINGIFY(var) " " FLOAT_LOSSLESS_FORMAT, (float)(var))

//
// END C vs. QuakeC bridge
//

#define oject_s			struct _oject_s
#define ktrl			struct _oject_s*
#define oject_ref_s		struct _oject_s			// Marked differently like container
#define oject_array_s	struct _oject_array_st
#define	things_s		struct _things_s

#define EXO___
#define LENGTH___

#define LIFE_FREE___			// Marks something important freed
#define LIFE_ALLO___			// Marks something important allocated
#define LIFE_CLEAR___			// Marks something important reset.

#define NPA__	// DANGEROUS .. manually free them! NPA: Non-allocated property that every object must free -
#define PPA__	// Property, destroy object works ALL properties and destroys



//
// REAL ENUMS
//

#define BORDER_TRUE							true

#ifdef QUAKEC // Highlight style is different for engine vs. QuakeC
	#define BOXWIDTH_2_3					3		// Extra padding for highlight outline #define BOXWIDTH_2 2
#else // ENGINE ...
	#define BOXWIDTH_2_3					2		// Engine halo - yes HERESY but QuakeC uses 3 for different style
#endif // QUAKEC

#define BUTTON_PRESS_TIME_0_3				0.3		// Amount of time for button press appearance.
#define CLIPPING_ON 						(k->fctrl->frm.disable_clipping == false)
#define CM_DOTCOUNT_COUNT_9					9
#define CM_MIDDLE_DOT_4						4
#define	CM_SERIES_COUNT_4					4
#define	CM_0_TEXT_0							0		// ContextMenu
#define	CM_1_CHECKED_1						1		// Checked .. for value use string
#define	CM_2_STRING_ID_2					2		// String id
#define	CM_3_FUNCSTRING_3					3

// colors are const vec3_t in oject_4_globals.c.h
// color3_gray_25 etc.

#define CREATE_NULL							NULL 	// Engine only
#define CORNER_LEFT_TOP_1					1
#define CORNER_RIGHT_TOP_2					2
#define CORNER_LEFT_BOTTOM_4				4
#define CORNER_RIGHT_BOTTOM_8				8
#define DIR_BACKWARDS_N1					-1
#define DIR_FORWARD_1						1
#define	DDOT_SIZE_8							8		//
// #define DOUBLE_CLICK_0_5 				0.5		// Engine: menu.h How quick a 2nd mouse down becomes a double click
#define DRAG_IDX_PLUS1_MIN_1				1
//#define FOCUS_CHANGE_NOSOUND_TRUE			true	// MenuQC only: Form_Focus_Find in QC
//#define FOCUS_CHANGE_NOSOUND_FALSE		false	// MenuQC only: Form_Focus_Find in QC
//#define FOCUS_END_2						2		// MenuQC only
#define FOCUS_HOME_NEG2						-2
#define FOCUS_NEXT_1						1
#define FOCUS_PREV_NEG1						-1
#define FORM_NULL							NULL	// Engine only.
#define FORM_KCAN_NULL						NULL	// Engine only. Form have no container. draw with owner as a zebra (Z-Ordered Top).
#define GRADIENT_STYLE_2
#define HEADERS_ROW_HEADERS_1				1		// Imagegrid .. k->gridheaders
#define HEADERS_COL_HEADERS_2				2
#define HEADERS_BOTH_HEADERS_3				3
//#define HORIZ_FALSE						0		// Scrollbar
//#define HORIZ_TRUE						1		// Scrollbar
#define	isbuttonaction_true					true	// Engine: Form_Hit_Drawn_Any and we decided this was bad.  Kill it.
#define isbuttonaction_false				false	// Engine: Form_Hit_Drawn_Any and we decided this was bad.  Kill it.
#define	isdone_false						false
#define	isdone_true							true
#define isdown_true							true	// SizeDot
#define isdown_false						false	// SizeDot
#define	KNOB_SIZE_16						16
#define LENGTH_0							0
#define LEFTOP_RIGHTBOT_1					1
#define LEFBOT_RIGHTTOP_2					2
#define LV_COLUMN_TEXTMARGIN_1				1		// Listview draw subtraction from column width/height
#define LV_DEFAULT_WIDTH_100				100
#define LV_DRAG_COLSIZE_10					10
#define LV_HEADERROW_ADD_6					6
#define LV_MIN_WIDTH_0						0
//#define LV_MIN_WIDTH_25					25
#define LV_SORT_PRESS_COLUMN_NEG2			-2		// encodes/decodes column number where -2 is "0", -3 is "1" ...
#define LV_TEXTMARGIN_10					10		// Listview: Column and data text indent
#define MOUSEDOWN_1							1
#define MOUSEUP_0							0
#define OFCAN_FOCUS_2						2		// A things flag
#define OFCAN_MOUSE_4						4		// A things flag
#define OFCAN_CONTAIN_8						8		// A things flag
#define OFCAN_RUNTIME_ONLY_NO_SAVE_16		16		// Like servant to what
#define OFCAN_AUTOSIZE_32					32		// Can specify its own size
#define OFCAN_NOHIGHLIGHT_64				64		// Does not draw highlight
#define OFCAN_ONLYFOCUS_128					128		// Context Menu - disappears on lost focus (replies cancel)
#define OUTSET_FALSE						false
#define OUTSET_TRUE							true
#define PRESS_AMOUNT_2						2		// Button shifting in (sadly) real-pixels
#define	q_indent_false						false	// Object_Dump_Less_Recursive
#define	q_indent_true						true
#define q_sethook_false						false
#define q_sethook_true						true
#define RECT_WIDTH_CULLED_ZERO				0		// QC equiv in baker.h (do not try to align this constant with QC)
#define SB_ACTION_DOWN_1					1
#define SB_ACTION_END_11					11
#define SB_ACTION_HOME_10					10
#define SB_ACTION_PGDOWN_3					3
#define SB_ACTION_PGUP_2					2
#define SB_ACTION_THUMB_4					4
#define SB_ACTION_UP_0						0
#define SCROLLBAR_HOOK_REFIRE_0_1			0.1
#define SERVANT_KCAN_NULL					NULL	// Servants do not have a container, servant container NULL = unrestricted clip during draw.
#define	SERVANT_TO_NULL						NULL
#define	shallrecurse_false					false	// This is the norm for Object_Event_OnChange_Recursive
#define	shallrecurse_true					true	// Object_Event_OnChange_Recursive will OnChange for everything control contains (particularly a form)
#define THIN_FALSE							false
#define THIN_TRUE							true
#define THREED_TRUE							true
#define TRANSPARENT_FALSE					false
#define	TXT_CURSOR_WIDTH_2					2		// Textbox cursor width ...
#define TXT_BORDER_WIDTH_2					2
#define TXT_MARGIN_4						4		// Around the text area
#define TXT_SCROLL_HORZ_AMOUNT_0_25			0.25	// Percent of textarea



// a_zircon_ui_enums.h - enumerations and typedefs

typedef enum {
	appearance_default_0			= 0,
	appearance_flat_1				= 1,
} appearance_e;

RELATED_ (Label_Rect_Edit_Align XY_Set_Dot)
typedef enum {
	alignment_top_left_0			= 0,
	alignment_top_center_1			= 1,
	alignment_top_right_2			= 2,
	alignment_middle_left_3			= 3,
	alignment_middle_center_4		= 4, // CM_MIDDLE_DOT_4
	alignment_middle_right_5		= 5,
	alignment_bottom_left_6			= 6,
	alignment_bottom_center_7		= 7,
	alignment_bottom_right_8		= 8,
} alignment_e;

typedef enum {
	arrow4_left_0					= 0,
	arrow4_right_1					= 1,
	arrow4_up_2						= 2,
	arrow4_down_3					= 3,
} arrow4_e;

typedef enum {
	border_Wall_none_0				= 0,
	border_Wall_left_1				= 1,
	border_Wall_top_2				= 2,
	border_Wall_right_4				= 4,  
	border_Wall_topless_1_4_8_13	= 13,
	border_Wall_bottomless_1_2_4_7	= 7,
	border_Wall_bottom_8			= 8,
} border_wall_e;

typedef enum {
	drawbut_thin_inset_0			= 0,
	drawbut_thin_outset_1			= 1,
	drawbut_normal_inset_2			= 2,
	drawbut_normal_outset_3			= 3,
	drawbut_command_up_4			= 4,
	drawbut_command_pressed_5		= 5,
	drawbut_command_thick_up_6		= 6,
//	drawbut_command_thick_pressed_7 = 7,
} drawbut_e;

typedef enum {
	focusstyle_default_0			= 0,
	focusstyle_nohighlight_1		= 1,
} focusstyle_e;

typedef enum {
	fontnameindex_system_neg1		= -1,	// Force Roboto
	fontnameindex_default_0			= 0,	// You get Roboto
	fontnameindex_console_1			= 1,	// FONT_CONSOLE = 1
	fontnameindex_sbar_2			= 2,	// 'sbar', used on hud, must be fixed width
	fontnameindex_notify_3			= 3,    // 'notify', used on sprint/bprint
	fontnameindex_chat_4			= 4,	// 'chat'
	fontnameindex_centerprint_5		= 5,	// 'centerprint'
	fontnameindex_infobar_6			= 6,    // 'infobar'
	fontnameindex_menu_7			= 7,	// 'menu', should be fixed width
	fontnameindex_user0_8			= 8,
	fontnameindex_user1_9			= 9,
	fontnameindex_user2_10			= 10,
	fontnameindex_user3_11			= 11,
	fontnameindex_user4_12			= 12,
	fontnameindex_user5_13			= 13,
	fontnameindex_user6_14			= 14,
	fontnameindex_user7_15			= 15,
} fontnameindex_e;

typedef enum {
	mouse_index_none_0				= 0,
	mouse_index_left_1				= 1,
	mouse_index_right_2				= 2,
	mouse_index_middle_3			= 3,
	mouse_index_4					= 4,
	mouse_index_5					= 5,
} mouse_index_15_e;

#if 0 // ENGINE HAS THESE IN HIGHER HEADER
// Zircon Beta: system enum these are mostly SDL constants
typedef enum {
//	mousepointer_invalid_0 = 0,
//
	mousepointer_arrow_default_0	= 0,		// Default
	mousepointer_text_beam_1		= 1,		// Text
	mousepointer_hourglass_2		= 2,		// Hourglass
	mousepointer_crosshair_3		= 3,		// Think image editor
	mousepointer_waitarrow_4		= 4,		// Hourglass-ish 
	mousepointer_size_nwse_5		= 5,		// Size diagonal
	mousepointer_size_nesw_6		= 6,		// Size diagonal
	mousepointer_size_weast_7		= 7,		// Size east west
	mousepointer_size_northso_8		= 8,		// Size north south
	mousepointer_size_all_move_9	= 9,		// Move something
	mousepointer_prohibited_10		= 10,		// Button or action, in theory, in practice that's an arrow
	mousepointer_hand_11			= 11,		// Button or action, in theory, in practice that's an arrow
	
//	mousepointer_other_12,				// Text
	mousepointer_baker_column_20 = 20,	// Column sizing

	mousepointer_COUNT_32 = 32
} mousepointer_e;
#endif // ENGINE HAS THESE IN HIGHER HEADER

// Baker: Jan 23 2026 - These do not appear to be used so far.
// Object_Refresh_Recursive has refresh_reason_preautosize_1 in comments
typedef enum {
	refresh_reason_normal_0			= 0,	// Size change or simple recalc
	refresh_reason_preautosize_1	= 1,	// Early refresh ask autosize control its size 
	refresh_reason_finalized_2		= 2,	// Single pass when a form is fully parsed
											// Chance to fill in "required" data that is missing
											// Like column widths for a listview
} refresh_reason_e;

COMPILE_TIME_ASSERT (refresh_reason_e, sizeof(refresh_reason_e) == 4); // Move along ...

typedef enum { // Baker: Indicate the conditions when we want these because I don't get it.
	// Cursor set wants best match for cursor positioning
	// But selection extend wants to favor getting more text selected ...
	// EXPLAIN THIS.
// roundtype_yes_1 was formerly roundtype_0_50_1
	roundtype_none_0				= 0,	// Trying a cursor set, do not round?
	roundtype_yes_1					= 1,	// Trying a cursor extend, round to select more text. Amounts greater than 0.5 mean right
} roundtype_e;

typedef enum _vartype_e {
	vtstring_1			= 1,	// ev_string_1
	vtfloat_2			= 2,	// ev_float_2
	metric_rectf_10		= 10,	// float rect (assumed metric) - NOT USED JAN 10 2025
	metric_recti_11		= 11,	// float rect (RECT)
	vtinteger_19		= 19,	// Best used for boolean, enumerations
	vtlist_str_20		= 20,	// list of strings
	vtrgb_21			= 21,	// vec3_t rgb triplet of floats 0.0 to 1.0 (maybe higher too? up to 8?)
	mvertflist_22		= 22,	// List of polygon vertex
	vtcontrolref_23		= 23,	// List of integers
	vtint32list_24		= 24,	// NO CURRENT USERS Jan 10 2025

	metric_float_102	= 102,	// metric float .. FontSize
	metric_integer_119	= 119,	// Metric integer .. Width (Who uses this) NO ONE YET January 10 2025
	mint32list_124		= 124,	// Column sizes

// Enum-likes - Only fontnameindex writes "No Number Suffix Enums" the rest do ... for now
	vtappearance_200	= 200,	// normal_0 flat_1 (form only?)
	vtfontnameindex_201 = 201,	// (-1)system (0)default_0 console sbar notify chat centerprint infobar menu user0 ...
	vtfocusstyle_202	= 202,	// normal_0 nohighlight_1 (form only)

	vtevent_100			= 100,	// list of strings
} vartype_e;

// "Things" 
typedef enum { // things_e
	thing_none_0, 

	class_start,
	class_form_1 = 1,	// Form must be 1 and the first property.  Hardcoded elsewhere.
	
	// This should be in alphabetical order.  Values mean nothing.
	class_button,		// TODO: January 12 2025
	class_checkbox,		// TODO: January 12 2025
	class_contextmenu,
	class_ext_closer,
	class_ext_mover,
	//class_ext_sizer,
	class_image,
	class_imagegrid,
	class_label, 
	class_listbox, 
	class_listview, 
	class_polygon,
	class_rectangle, // Draw and refresh only
	class_scrollbar,
	class_sizedot,
	class_tabselect, 
	class_textbox, 
	
	class_end,

	prop_start,

// Example: ImageGrid Name:ImageGridMap Rect:"324,63,542,526" AutoSize:1 BackAlpha:1 BackColor:#005ae6  
	prop_name = 100, 
	prop_container = 101,
	prop_rect = 102,			// Relative rect.

// Sort these
	
	prop_alignment, 
	prop_appearance,
	prop_autosize,
	prop_backalpha,
	prop_backcolor,
	prop_backcolorfocusselected,
	prop_backcolorselected,
	prop_caption,
	prop_cellpaddingxpct,
	prop_cellpaddingypct,
	prop_cellspacingypct,
	prop_columncount,
	prop_columnheaders,
	prop_columnwidths,
	prop_cursorimagename, // Engine: Image grid drag/drop
	prop_didconsumekey,
	prop_dragenabled,
	prop_filelist,
	prop_flatcolor,
	prop_focusstyle,
	prop_fontdescendpct,
	prop_fontnameindex,
	prop_fontsize,
	prop_forceminimumwidth,
	prop_forealpha,
	prop_forecolor,
	prop_forecolorselected,	
	prop_gradientstyle,
	prop_gridautosizecolumns,
	prop_gridautosizerows,
	prop_gridcellbackcolor,
	prop_gridcellheight,
	prop_gridcellpadding,
	prop_gridcellpaddingcolor,
	prop_gridcellwidth,
	prop_gridcolumns,
	prop_gridheaders,
	prop_gridmaxcount,
	prop_gridrowheaderwidth,
	prop_gridrows,
	prop_gridvalues,
	prop_haveextcloser,
	prop_haveextmover,
	prop_haveextsizer,
	prop_imagealpha,
	prop_imagecolor,
	prop_imagemargin,
	prop_imagelist,
	prop_imagename, 
	prop_imagesize,
	prop_iscutout,
	prop_isdisabled,
	prop_ishidden, 
	prop_ishorizontal, 
	prop_iskeypreview,
	prop_ismultiselect, // Engine listview multiselect
	prop_isoutlined, 
	prop_isquake1bsptexture, // Engine: Allows drawing Q1 textures
	prop_isrounded, 
	prop_issorted, 

#if 1 // May 8
	prop_columnalignments,	// LVW May 8 2026
	
	prop_ismarkup,
	prop_isnobackground,
	prop_isnoscrollbars,
	prop_isnoheaderrow,

	prop_rowheight,
	prop_rowstripealpha1,
	prop_rowstripecolor1,
	prop_rowstripealpha2,
	prop_rowstripecolor2,
#endif // May 8 2026

	prop_linestyle,
	prop_linewidth,
	prop_list,
	prop_maxvalue,
	prop_notext,
	prop_notextcolor,
	prop_popupanswer, // DevPopUpEnded - context menu?
	prop_popupquestion,
	prop_rowheaders,
	prop_scrollbarcolor,
	prop_scrollbarthumbcolor,
	prop_selectedindex,
	prop_selectedlength, // Engine listview multiselect
	prop_servantowner,
	prop_sortcolumndescending,
	prop_sortcolumnindex,
	prop_text,
	prop_tooltip, // Engine: Tooltip support - ToolTip_Deploy_If_Target_ToolTipText
	prop_userdata,
	prop_userdata2,
	prop_userdata3,
	prop_userstring,
	prop_userstring2,
	prop_userstring3,
	prop_value,
	prop_vertexes,
	prop_wasinitialized,

// Events are sort of like properties actually.
// January 18 2025: However, these are rather useless ... we need to manually do them
// And these don't really mean anything and are entirely unused for CSQC interface

// January 23 2026 - We are parsing them in QuakeC.  We probably want them to load in the engine too.
	events_start,

	events_OnKeyPreview, 
	events_OnChange,
	events_BeforeClose,
	events_OnLoad, 
	events_OnMouseButtonAction,
	events_OnMouseMove,
	events_OnPopUpEnded,
	events_OnSort,
	
	
	events_Activate,	// QC Only
	events_Click,		// QC Only - ish ....
	events_Deactivate,	// QC Only
	events_DoubleClick,	// QC Only - ish ....
	events_KeyEvent,	// QC Only
	events_PostDraw,	// QC Only
		
	events_end,

	prop_end,

	RELATED_ (Object_Get_ControlIndex)
	demiprop_controls,			// Access frm.controls_a like a list and get the object name 
	demiprop_controlindex,		// Runs Object_Get_ControlIndex and finds the current index.  This actually can change (Zorder).
	demiprop_controlcount,		// Form. Returns f->frm.controls_a->numitems
	demiprop_controltype,		// Returns k->po->name like "ListView"
	demiprop_filelistcount,		// FileListCount -- retrieves file list num strings
	demiprop_focused,			// Form. Returns f->frm.kfocused->cm.name_a or f->cm.name_a
	demiprop_handle,			// Returns f->ui.csqc_handle_plus1
	demiprop_imagelistcount,	// ImageListCount -- retrieves image list num strings
	demiprop_key,				// Form "Key"/"Value" system treats List as 2 column array.
	demiprop_listcount,			// ListCount -- retrieves list num strings
	demiprop_selectedrow,		// Alias for SelectedIndex, particularly for ListView because multi-column

	meth_start,
	
	// meth_cmd = no arguments - Like properties, methods are caseless.

	meth_cmd_filelistclear,
	meth_cmd_imagelistclear,
	meth_cmd_listclear,
	
	meth_cmd_setfocus,

	meth_additem,				// item string
	meth_addimage,				// image string
	meth_addfile,				// file string
//	meth_clipboardsetimage,		// Form method?  Set
	meth_clipboardsettext,		// Form method?  Set
	meth_popup,					// arg becomes userstring like "data/myfilename.file"
	meth_listappendfilelines,	// filename to read.  Busts the 16384 limit to a listview.
	//meth_textwrap,				// Not implemented yet.

	meth_end,

	// Events QuakeC is alerted to ...
	evx_sendhandle,
	evx_initializedwithmessage,
	//evx_click,
	//evx_doubleclick,
	//evx_onchange,
	evx_keypreview,
	evx_popupclosed,

// These things are special funtions we know
	builtin_func,
} things_e;


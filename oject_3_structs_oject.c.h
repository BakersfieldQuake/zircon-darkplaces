// oject_3_structs_oject.c.h

// Zebras -- top most things like popups or tooltips, zebras are exclusive to form (right?)

RELATED_ (things _things_s)
typedef struct _oject_s {
// Assume all objects have all properties

//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////
	oject_ref_s			*fctrl;				// Form fctrl is SELF!
	oject_ref_s			*container;			// Only a form has this NULL
	const things_s		*po;
	oject_ref_s			*servant_owner;		// Unfocusable contained child (scrollbar, etc.)

	oject_ref_s			*hover_candidate;
	float				*hover_when_time_is;

	viewcalc_s			viewcalc;			// Listbox visibility info
	viewcalc_s			viewcalcx;			// Horiz

	struct {
			int			startcol;			// Nov 8 2025 - Not used? (Baker: Keep for now)
			int			beyondcol;			// Nov 8 2025 - k->columncount
			int			headerrowheight;	// Nov 8 2025 - ceil(k->fontsize) + LV_HEADERROW_ADD_6;
			int			columnsallwidths;	// Nov 8 2025 - colwidths(k, &k->xl.is_phantom, &k->xl.phantom_width);
			//int		fakecolumnwidth;	// Nov 8 2025 - Not used?
			int			is_phantom;			// Nov 8 2025 - A fake extra column that displays to fill space.
			int			phantom_width;

			// Part of footprint
			rect_s		r_header;
			rect_s		r_header_clipped;

// Imagegrid ... gridheaders
			rect_s		r_rowheader;		// Row header (LEFT)
			rect_s		r_rowheader_clipped;

			int			headercolwidth;		// Dec 27	 2025 - ceil(k->fontsize) + LV_HEADERROW_ADD_6;
//			int			gridrealcellwidth;	// k->gridcellwidth + k->gridcellpadding
//			int			gridrealcellheight;	// k->gridcellheight + k->gridcellpadding

			rect_s		r_interior_full;	// Size without scrollbars reducing it. (ImageGrid)
			int			gridheaders_bound;

			int			corner_bits;
			rect_s		corner_LT;
			rect_s		corner_RT;
			rect_s		corner_LB;
			rect_s		corner_RB;
	} xl;

	struct {								// FIELDS: x, y, width, height
			rect_s		_relative_rect_orig;
		    rect_s		relative_rect;		// Container interior relative
PPA__		char		*name_a;			// FIELDS: name
// Baker: creation_idx is always 1 or greater.  For form this is a counter and increments.  For others, it is assigned once.
			int			creation_idx;		// Really used! For unnamed controls!  For controls only, assigned.  For form, given.
			//int		controlindex;		// The actual form array control index.  NO it can change at run-time with ZOrder
			xy_pairi_s	dot_origin;			// We only tell the dot a size.  It decides what to do with that information.
			qbool		is_finalized;		// Occurs when Object_Refresh_Recursive is run on it.
											// Means that it should be ready for rendering with initialization completed.
			oject_ref_s	*kcutout;			// Draw around this.

			int		in_recursion;		// Setting a property that can trigger a chain.
	} cm;

	// Z-Ordered Servants That Share our footprint, and do not use our interior canvas 
	// and draw last for the owner control. (ex: scrollbar for a listbox)
	// These can never receive focus, but can obtain mouse focus by hold clicking them.  
	// Therefore they cannot highlight.
	struct {
		oject_ref_s		*kscrollx;
		oject_ref_s		*kscrolly;
		oject_ref_s		*kcolumnresize_line;
		oject_ref_s		*kcontextmenu;		// Baker: This is the servo edit mode context menu.
		oject_ref_s		*kdots[CM_DOTCOUNT_COUNT_9];
		oject_ref_s		*ktooltip;
		oject_ref_s		*kcloser;
		oject_ref_s		*kmover;
		oject_ref_s		*ksizer;
	} servo;

	struct {
		mousepointer_e	mousepointer;		// Form sees mousemove will use this as cursor if over control.
											// Probably also if off footprint if mouse1 pressed.
		//int			is_mouse_1_down;	// May 4 2024 - Right now, we only respond to mouse 1 + wheel
		xy_pairi_s		mouse_down_origin;	// int x, y .. probably window screen origin
		int				is_move_thresh_met;	// 

		timey_dbl_t		mouse_down_time;	// double

		// Values of mouse_down_thingi
		// O_ListView_ColumnDragStart	... LV_DRAG_COLSIZE_10 + column
		// O_SizeDot_PlacementDragContinue		... k->fctrl->frm.kmousedown
		// Recommend that O_ImageGrid use idx plus1

		int			mouse_down_thingi;	// Column number for a ListView
		int			mouse_down_valuei;	// Column width for a ListView
		
		rect_s			mouse_down_rect;		// sizedots - current relative rect of control before sizing
		rect_s			mouse_down_shadow_rect;	// sizedots

		xy_pairi_s		mouse_down_thumb_into;	// How many px into thumb are we?

		// EXPLAIN .. Nov 8 2025 .. When the mouse is downed on the thumb, this is where it was.
		// Since the thumb moves during the process we need to know where it started.
		xy_pairi_s		mouse_down_thumb_at_mousedown_px;
		xy_pairi_s		mouse_down_forced_thumb_pos;	// Mouse actions can forcibly move the thumb to a specific location.

		double /*time*/ button_down_end_time; // Pressing sets to a time.

		// SIZEOIC - Translate all coordinates by this.  columnwidths, fontsizes
		// SIZEOIC - Translate all mouse input by this.
		// Q: Is magnification an integer always: YES

		mag_s			magnix; // 	int magnifier; offset_xx; int offset_yy;
		int				csqc_handle_plus1;
		func_t			form_event_qcfunc;
		func_t			form_close_qcfunc;
		oject_s			*fModalChild;		// This form IS covering our form.
		oject_s			*fModalBoss;		// We are cover hModalBoss
PPA__	char			*sMessageClose;
		qbool			form_is_closing;	// Form_Close event.  Must wait a frame.

		
	} ui;

//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////
	struct {
		oject_ref_s		*kfocused;			// Focused control
NPA__	oject_array_s	*controls_a;		// Everything is in this, EXCEPT NOT THE FORM.
NPA__	oject_array_s	*drawn_list_a;		// List of drawn controls.  But NOT THE FORM.

		oject_s			*kmousepresshook;		// This is in a mousedown
		mouse_hook_fn_t	hook_fn;
		timey_dbl_t		hook_nextthink;		// Fire when

		qbool			disable_clipping;	// CLIPPING_ON, defaults off.
		oject_s			*kmousedown;		// A mousedown means the mouseup/mousemove owned by this.
		oject_s			*kpopup;			// The context menu being rendered.
		double			kmousedown_time;
		int /*qbool*/	kmousedown_click_count;

		int				editmode;
		//oject_s		*keditmode;			// Baker: November 2 2025 - unused in source

		qbool			has_metric_run;
		int				numrefreshes;
		int				refreshneeded;
		int				refreshdots;

		oject_s			*kcontextoldfocus;	// For escape / enter
		oject_s			*kpopup_initiator;

		qbool			has_cutout;
	} frm;

	// ALL METRICS IN FP ARE IN REAL SCREEN
	
	// .clipped is for SetClip/Scissor Area only and footprint is used for mouse click collision
	// .unclipped is for all draws.

	RELATED_ (Object_Refresh_Recursive Object_Draw_Recursive_Not_Form)
#define ORR // Baker: Feb 11 2025 -- ORR Means what? May 2 2026 - Probably Object Refresh Recursive
	rect_s	ORR			r_halo;				// Footprint + a fixed amount. BOXWIDTH_2
	rect_s	ORR			r_halo_clipped;		//

	// r_interior is set in Object_Refresh_Recursive (k->r_screen = k->cm.relative_rect;, k->r_screen.left	+= k->container->r_interior.left;)
	rect_s	ORR			r_screen;			// Here is your r_drawn
	rect_s	ORR			r_screen_clipped;

	// r_interior is set in Object_Refresh_Recursive (SET___ k->r_interior = k->r_screen;)
	rect_s	ORR			r_interior;			// The "interior" (footprint less border) -- does Object_Refresh set?
	rect_s	ORR			r_interior_clipped;
	rect_s				r_scrollport;		// Contents size and scroll: height is row_height_px * members count

	rect_s	ORR			r_cutout_top;		// Here is your r_drawn
	rect_s	ORR			r_cutout_left;		// Here is your r_drawn
	rect_s	ORR			r_cutout_right;		// Here is your r_drawn
	rect_s	ORR			r_cutout_bottom;	// Here is your r_drawn

	vec2_t				r_cutout_top_st0, r_cutout_top_st1;
	vec2_t				r_cutout_left_st0, r_cutout_left_st1;
	vec2_t				r_cutout_right_st0, r_cutout_right_st1;
	vec2_t				r_cutout_bottom_st0, r_cutout_bottom_st1;	

//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////
	struct {
		rect_s			r_textarea;			// Who uses?
		int				force_draw_cursor;	// Occurs 1 frame on any cursor or key action.
		int				cursor_pos;			//
		int				cursor_length;		// Non-zero is a selection.  Negative is right to left selection.
		int				_cursor_start;		// smallest of cursor_pos and cursor_pos + cursor_length
		int				_cursor_beyond;		// _cursor_start + abs(cursor_length)
		int				_cursor_length_abs;	// _cursor_start + abs(cursor_length)
		float			_sel_left_px;
		float			_sel_len_px;
		float			_sel_real_px;
		vec3_t			_antiforecolor;		// XOR color
		vec3_t			_antibackcolor;
		rect_s			r_cursor;			// Draw fill cursor area.  Size is ONE if length 0.
		frect			r_cursor_highlight; // Draw fill cursor area.  Size is ONE if length 0.
		NPA__ char		*seg1_a;			// TextBox text before cursor
		int				seg1_left_px;
		NPA__ char		*seg2_a;			// TextBox text highlighted cursor
		int				seg2_left_px;
		NPA__ char		*seg3_a;			// TextBox text after cursor
		int				seg3_left_px;
		int				is_viewported_x;	// Text length
		int				stringwidthi_px;	// ceil
	} txta;

// Q: How are these freed?
// A: Property destroy unfrees every property by vartype.

	// Keep this alpha and the names as close to a match
//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////

// REAL PROPERTIES ONLY ...
	int					alignment;			RELATED_(alignment_e)
	int					appearance;			RELATED_(appearance_e)
	int					autosize;
	float				backalpha;
	vec3_t				backcolor;
	//vec3_t			backcolordisabled;
	vec3_t				backcolorselected;
	vec3_t				backcolorfocusselected;
	PPA__ char          *caption_a;			// Button_Finalize will make a NULL into ""
	float               cellpaddingxpct;	// percent of fontsize how much to pad the text for a button
	float               cellpaddingypct;	// percent of fontsize how much to pad the text for a button
	float               cellspacingypct;	// percent of fontsize for row
	int					columncount;		// Listview
	PPA__ stringlist_t  columnheaders_a;
	PPA__ stringlist_t  rowheaders_a;
	PPA__ int32list_s	_columnwidths_a_orig;
	NPA__ int32list_s	columnwidths_a;		// NPA: Non-allocated property that every object must free.

	// #pragma message ("Changing listview data with alignment requires refresh to update pre-compute widths")

	// May 9 2026 - Here is the deal ...
	// We will store every string length when it needs calculated.
	// For alignment.
	// If you change the font, change the text, must do a refresh.  That includes sort.
	// If you are using columnalignment, refreshing becomes important.
	// A refresh clears all the lengths.

	NPA__ int32list_s	stringwidths_rt_a;	// NPA: Computed during ListView draw when needed.


	PPA__ char          *cursor_image_name_a;// image grid
	int					didconsumekey;		// If KeyPreview is true, this indicates the keyevent was consumed by QuakeC
	int					dragenabled;
	int					direction;
	
	// EVENTS
	struct {
		// Baker: Jan 4 2026 - Do these guys get freed?  Maybe?  Mar 1 2026 - Seems likely, they are properties.
		char			*onload_a;
		char 			*onchange_a;
		
		char 			*beforeclose_a;
		char 			*onkeypreview_a;
		char 			*onmousebuttonaction_a; // Form only.  Mouse 2 only right now.
		char 			*onmousemove_a; // Form only.  With no button down at this time.

		RELATED_ (DevPopUpEnded) // "OnPopUpEnded", "DevPopUpEnded"
		char 			*onpopupended_a; // Used to query popup contextmenu result, usually with form.
		char 			*onsort_a;
		
		char 			*qc_activate_a;
		char 			*qc_click_a;
		char			*qc_doubleclick_a;
		char 			*qc_deactivate_a;
		char 			*qc_keyevent_a;		// No references?  Mar 1 2026?
		char 			*qc_postdraw_a;
	} eve;

	PPA__ stringlist_t  filelist_a;			// 
	vec3_t				flatcolor;			// Flat color for button faces
	int					focusstyle;			RELATED_(focusstyle_e vtfocusstyle_202)
	float				fontdescendpct;
	float				forceminimumwidth;	
	
	float				_fontsize_orig;
	float				fontsize;
	int					fontnameindex;
	float				forealpha;
	vec3_t				forecolor;
	//vec3_t			forecolordisabled;
	vec3_t				forecolorselected;

	int					gradientstyle;
	//vec3_t			gradientcolor;
	//vec3_t			gradientcolorselected;

	int					gridautosizecolumns;
	int					gridautosizerows;

	int					gridmaxcount; // Cells beyond this are not drawn and cannot be selected.
	int					gridcolumns;
	int					gridrows;

	int					_gridrowheaderwidth_orig;
	int					gridrowheaderwidth;

	int					_gridcellpadding_orig;
	int					gridcellpadding;		// Dec 10 2025 - empty space surrounding the image (2 px or whatever)
	vec3_t				gridcellpaddingcolor;
//NO	int				gridcellspacing;		// Dec 10 2025 - space between the cells (2 px or whatever)
	vec3_t				gridcellbackcolor;
							
	int					_gridcellwidth_orig;
	int					gridcellwidth;
	int					_gridcellheight_orig;
	int					gridcellheight;

	int					gridheaders;			// Dec 27 2025 - If 3 there are row and column headers.  (1 row, 2 column, 3 both)
// NO	int				gridmargin;				// Dec 10 2025 - space before the first cell and after the last (reduces interior)
	
	//vec3_t			gridpaddingcolor;
	int					have_ext_closer;
	int					have_ext_mover;
	int					have_ext_sizer;
	
	PPA__ int32list_s	grid_values_a;			// For a grid control we discontinued in favor of imagegrid.
	
	float				imagealpha;
	vec3_t				imagecolor;
	float				_imagemargin_orig;
	float				imagemargin;			// Imagegrid, metric float.
	PPA__ stringlist_t  imagelist_a;			// Imagegrid
	float				_imagesize_orig;		// Imagesize metric float (button image width and image height when no text)
	float				imagesize;				// Imagesize metric float (button image width and image height when no text)
	float				image_slow;
	float				image_tlow;
	float				image_shigh;
	float				image_thigh;
	int					is_cutout;
	int					is_disabled;
	int					is_hidden;				// is_visible, except is_hidden can default false
	int					is_horizontal;
	int					is_keypreview;
	int					is_multiselect;			// Listview does multiselect
	int					is_outlined;
	int					is_quake1_bsp_texture;	// Q1 hack for Image control
	int					is_rounded;
	int					is_sorted;


#if 1 // May 8 2026
	PPA__ int32list_s	columnalignments_a;		// Plus1 - should be an int list, but for now no.
	int					is_markup;				// Let's list markup to draw an image. <img></img>
	int					is_nobackground;		// Listview
	int					is_noscrollbars;		// Listview - no scrollbar/no header row
	int					is_noheaderrow;			// Listview
	
	float				rowstripealpha1;
	vec3_t				rowstripecolor1;		// Table stripe color.
	float				rowstripealpha2;		// Table stripe color.
	vec3_t				rowstripecolor2;

	int					_rowheight_orig;		// Listview
	int					rowheight_rt;			// Listview
	int					rowheightpad_rt;		// Listview - not intended for header row right now May 9 2026

	// Alpha level?  Encoded.
	//PPA__ int32list_s	rowbackcolors;			// Markup.  Markup parses to imagelist_a and rowbackcolors? <img gfx/myimage><backcolor>
#endif // May 8 2026

	int					linestyle;
	float				linewidth;
	float				_linewidth_orig;

	PPA__ stringlist_t	list_strings_a;			// image how ..

	RELATED_ (DevListViewOnChange)
	PPA__ stringlist_t	list_strings_back_a;	// The unfiltered copy of list_strings_a 
	float				maxvalue;
	PPA__ char          *notext_a;				// When TextBox text is empty, it shows this (hint to what it is)
	vec3_t				notext_color;			// And this is the color
	PPA__ char          *popup_answer_a;		// 
	PPA__ char          *popup_question_a;		// 
	vec3_t				scrollbarcolor;
	vec3_t				scrollbarthumbcolor;
	int /*runtime*/		selectedindex;
	int /*runtime*/		selectedlength;
	int					sortcolumnindex;
	int					sortcolumndescending;
	PPA__ char          *text_a;				// different from caption how? Baker: Don't matter, it's here.
	PPA__ char          *tooltip_a;				// 
	float               value;
	PPA__ floatlist_s	vertexlist_a;
	int					wasinitialized;
	float				userdata;
	float				userdata2;
	float				userdata3;
	PPA__ char          *userstring;		// 
	PPA__ char          *userstring2;		// 
	PPA__ char          *userstring3;		// 

// dubious ...
//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////
    PPA__ char          *image_name_a;		// not used yet
	
	struct {
		int				cellpaddingx_i;		// percent of fontsize how much to pad the text for a button
		int				cellpaddingy_i;		// percent of fontsize how much to pad the text for a button
		int				cellspacing_i;		// percent of fontsize for row
		int				stringwidthmaxpx;
		rect_s			r_button;
		rect_s			r_leftbar;
		int				buttonstep;
		int				bottomof_last;
	} tb;
	//float				buttonheight_px_rt;	// fontsize * cellpadding
	//float				buttonwidth_px_rt;	// fontsize * cellpadding
	//float				cellspacing_px_rt;	// fontsize * cellspacing
	//
	//float				descender_pct_rt;
	//float				descender_px_rt;	// descender * fontsize

// Run-time
//  // 5 HEAD //		// 25				// 45 //////////////////////////////////////////////////////
		int				is_checked;			// Checkbox


	struct {
		rect_s			r_top;
		rect_s			r_track;
		rect_s			r_bottom;

		rect_s			r_thumb;
		int				thumbsize_px;
		//int			empty_track_px;		// track less thumbsize -- 60 minus thumb 10 = 50
		int				num_top_positions;	// track_empty_px + 1
		int				track_empty_px;		// num_top_positions - 1
		
// value (0 to x - 1)
// maxvalue (num lines)
// number of top positions = tracksize less thumb - 1
// percent that each top position represents = maxvalue / (number of top positions)
// track position to value
	} sb;


} oject_st;

//typedef oject_s* ktrl;





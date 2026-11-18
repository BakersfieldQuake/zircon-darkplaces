// k_imagegrid.c.h

// First stop is no scroll list.
// Need to calculate the cell positions
// Draw the image list


// It is ok for the selected idx to fall out of range if the scrollbar did it.

//
// 0  =====   -8  BOB     8 =======
//    BOB		  ======    MARY
//    MARY        MARY      TODD

RELATED_(O_ListView_Spawn)
ktrl O_ImageGrid_Spawn (ktrl k)
{
	return Lister_Spawn(k); // Baker: Spawns xscroll yscroll
}

RELATED_ (Object_Refresh_Recursive O_Label_Refresh_Plus_Early)
RELATED_ (O_ListView_Refresh)

// Baker: Dec 10 2025 - EARLY == autosize support
RELATED_ (O_ListView_Refresh Lister_ListViewX_FixedY_Refresh)
ktrl O_ImageGrid_Refresh_Plus_Early (ktrl k)
{
#ifdef _DEBUG
	RELATED_ (___VarObjectFont)
	VarMyFont(myfont); // Picks the font index
#endif
//	int listcount = KP(imagelist_a.numstrings);

	KP(xl.gridheaders_bound) = bound(0, KP(gridheaders), 3);
	KP(xl.headerrowheight) = 0; // Variable
	KP(xl.headercolwidth) = 0; // Variable
	// Row headers should use array

	switch (KP(xl.gridheaders_bound)) {
	default: // None
	case_break HEADERS_ROW_HEADERS_1:	KP(xl.headercolwidth)	= KFONTSIZECEIL(k) + LV_HEADERROW_ADD_6;
		if (KP(gridrowheaderwidth))
			KP(xl.headercolwidth) = KP(gridrowheaderwidth);
		// Row headers are to the left!  So they are headercolwidth
	case_break HEADERS_COL_HEADERS_2:	KP(xl.headerrowheight)	= KFONTSIZECEIL(k) + LV_HEADERROW_ADD_6; // ?
	case_break HEADERS_BOTH_HEADERS_3:	KP(xl.headerrowheight)	= KFONTSIZECEIL(k) + LV_HEADERROW_ADD_6;
								KP(xl.headercolwidth)	= KFONTSIZECEIL(k) + LV_HEADERROW_ADD_6; // ?
		if (KP(gridrowheaderwidth)) // Set column width of row header
			KP(xl.headercolwidth) = KP(gridrowheaderwidth);

	} // sw

	// Autosize forces minimal footprint before r_screen and r_interior are calculated
	if (KP(autosize) || KP(cm.relative_rect.width) == 0 || KP(cm.relative_rect.height) == 0) {
		int ac_columns = 1;
		int ac_rows = 1;
		if (KP(gridautosizecolumns) > 0) ac_columns = KP(gridautosizecolumns);
		if (KP(gridautosizerows) > 0)	ac_rows = KP(gridautosizerows);

		float interior_size_w = KP(gridcellwidth) * ac_columns;
		float interior_size_h = KP(gridcellheight) * ac_rows;

		if (KP(xl.headercolwidth))	interior_size_w	+= KP(xl.headercolwidth);
		if (KP(xl.headerrowheight))	interior_size_h	+= KP(xl.headerrowheight);

		// Add in

		// Add scrollbars in if applicable.
		if (ac_rows < KP(gridrows)) interior_size_w	+= KNOB_SIZE_16;
		if (ac_columns < KP(gridcolumns)) interior_size_h	+= KNOB_SIZE_16;

		KP(cm.relative_rect.width) = interior_size_w;
		KP(cm.relative_rect.height) = interior_size_h;
	}

	// Baker: Dec 10 2025 - Adjust interior.  ListView does this in Lister_ListViewX_FixedY_Refresh
	//
starting_interior: // Baker: We may cut away from the interior with scrollbar space.

	RECT_SET (KP(xl.r_interior_full),
		KP(r_screen.left) +  KP(xl.headercolwidth),
		KP(r_screen.top) + KP(xl.headerrowheight),
		KP(r_screen.width) - KP(xl.headercolwidth) ,
		KP(r_screen.height) - KP(xl.headerrowheight) );

	// Set interior to full .. we may cut away with scrollbar space
	RECT_COPY(KP(r_interior), KP(xl.r_interior_full));

interior:
    ; // oblig
	int calc_again_xscroll_showed = false;

	// Baker: Needs to happen ...
	KP(viewcalc.rowheight) = KP(gridcellheight);
	KP(viewcalcx.rowheight) = KP(gridcellwidth);

	int rowcount = Lister_ListCountY(k); // KP(gridrows but we are using the generic interface
	int colcount = Lister_ListCountX(k);

	KP(r_scrollport.height)		= KP(viewcalc.rowheight) * rowcount;

	qbool did_remove_height = false;

xscroll_appeared_recalc_yscroll:
	Lister_FixedY_Refresh (k); // Calcs Y scroll

#if 1 // Lister_FixedY_Refresh sets bad r_scrollport.width
	KP(r_scrollport.width)		= KP(viewcalcx.rowheight) * colcount; ;// KP(r_interior.width); // Why?
#endif

	RELATED_ (Lister_ListViewX_FixedY_Refresh)
	ktrl kscrollx = KP(servo.kscrollx);
	ktrl kscrolly = KP(servo.kscrolly);
	if (KP(gridcolumns) == 0) {
		// Baker: This needs to always set 0 scrollbars
		KXP(kscrollx, is_hidden) = KXP(kscrolly, is_hidden) = true;
		KP(r_scrollport.left) = KP(r_scrollport.top) = 0;
		return NULL;
	}

	if (!rowcount) {
		// Baker: This needs to always set 0 scrollbars
		KXP(kscrollx, is_hidden) = KXP(kscrolly, is_hidden) = true;
		KP(r_scrollport.left) = KP(r_scrollport.top) = 0;
		return NULL;
	}

	KP(xl.startcol)		= 0;
	KP(xl.beyondcol)	= KP(gridcolumns);

	// We always must recalc this, even if we are on 2nd recalc
	if (1) {
		RELATED_ (ViewCalc_FixedAnyXY_Set)
		ViewCalc_Set_KX (k, KP(r_interior), KP(r_scrollport.left), colcount);

first_col_calc:
		RECT_SET (
			KP(viewcalcx.r_row_first),
			KP(r_interior.left) - KP(r_scrollport.left) + KP(viewcalcx.first_vis_row) * KP(viewcalcx.rowheight),
			KP(r_interior.top) - KP(r_scrollport.top),
			KP(viewcalcx.rowheight), // Width
			KP(viewcalc.rowheight) // Height
		);

		KXP(kscrollx, is_hidden) = (KP(viewcalcx.show_scrollbar) == false);

		if (KP(viewcalcx.show_scrollbar)) {
			rect_s r_scroll;
			RECT_COPY (r_scroll, KP(xl.r_interior_full));
			r_scroll.left	= KP(xl.r_interior_full.left);
			r_scroll.width	= KP(xl.r_interior_full.width) - KP(viewcalc.show_scrollbar) * KNOB_SIZE_16;
			r_scroll.top	= RECT_BOTTOMOF(KP(xl.r_interior_full)) - KNOB_SIZE_16;
			r_scroll.height = KNOB_SIZE_16;

			RELATED_ (Lister_SetPctHorzListView)

			// PERFORM ADJUSTMENTS
			RECT_COPY (KXP(kscrollx,cm.relative_rect), r_scroll);

			if (did_remove_height == false) {
				KP(r_interior.height) -= KNOB_SIZE_16;
				did_remove_height = true;
			}

			// Y SCROLL WILL ALWAYS NEED TO RECALC IF WE DID THIS
			if (calc_again_xscroll_showed == false) {
				// Restore interior width so yscroll can calc position/cutaway again ...
				if (KP(viewcalc.show_scrollbar))
					KP(r_interior.width) += KNOB_SIZE_16; // Undo Y scrollbar area
				calc_again_xscroll_showed = true;
				goto xscroll_appeared_recalc_yscroll;
			}
		}
	} // calc xscroll


interior_done:

	; // Oblig

#ifdef _DEBUG
	int xbey2 = RECT_RIGHTOF(KP(r_interior));
#endif

	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(r_interior),
		SET___ KPA(r_interior_clipped),
		KPA(r_screen_clipped), // parent
		//RECT_SEND(KP(r_interior))
		KP(r_interior.left), KP(r_interior.top), KP(r_interior.width), KP(r_interior.height)
	);

	int row_lead	= KP(xl.headerrowheight); // Y
	int row_trail	= KP(viewcalcx.show_scrollbar) * KNOB_SIZE_16; // Y
	int col_lead	= KP(xl.headercolwidth); // X
	int col_trail	= KP(viewcalc.show_scrollbar) * KNOB_SIZE_16; // X

	// If row + col ==> LT
	// If row + x scroll ==> L bottom
	// If col + y scroll ==> Right top
	// If x scroll + y scroll ==> right bottom

	KP(xl.corner_bits) = 0;
	if (col_lead && row_lead) {
		Flag_Add_To (KP(xl.corner_bits), CORNER_LEFT_TOP_1);
		RECT_SET (KP(xl.corner_LT),
			KP(r_screen.left),
			KP(r_screen.top),
			col_lead,
			row_lead);
	}
	if (col_lead && col_trail) {
		Flag_Add_To (KP(xl.corner_bits), CORNER_RIGHT_TOP_2);
		RECT_SET (KP(xl.corner_RT),
			RECT_RIGHTOF(KP(r_interior)),
			KP(r_screen.top),
			col_trail,
			row_lead);
	}
	if (KP(xl.headercolwidth) && KP(viewcalcx.show_scrollbar)) {
		Flag_Add_To (KP(xl.corner_bits), CORNER_LEFT_BOTTOM_4);
		RECT_SET (KP(xl.corner_LB),
			KP(r_screen.left),
			RECT_BOTTOMOF(KP(r_interior)),
			col_lead,
			row_trail);
	}
	if (KP(viewcalc.show_scrollbar) && KP(viewcalcx.show_scrollbar)) {
		// after the interior KNOB_SIZE
		Flag_Add_To (KP(xl.corner_bits), CORNER_RIGHT_BOTTOM_8);
		RECT_SET (KP(xl.corner_RB),
			RECT_RIGHTOF(KP(r_interior)),
			RECT_BOTTOMOF(KP(r_interior)),
			col_trail,
			row_trail);
	}


calcrowheader:
	if (Have_Flag (KP(xl.gridheaders_bound), HEADERS_ROW_HEADERS_1)) {
		// ROW HEADERS ONLY // LEFT
		// (R) [  ] [ ]
		// (R) [  ] [ ]
		// (R) [  ] [ ]
		// (R) [  ] [ ]
		// ( ) ============
		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ KPA(xl.r_rowheader),
			SET___ KPA(xl.r_rowheader_clipped),
			KPA(r_screen_clipped),
			KP(r_screen.left) + 0, // Row header always X == 0
			KP(r_screen.top) + row_lead, // Add space for any column header.
			KP(xl.headercolwidth), // Width of row header
			KP(r_screen.height) - row_lead - row_trail  // Less any column header row, less any X scrollbar
		);
	}

	if (Have_Flag (KP(xl.gridheaders_bound), HEADERS_COL_HEADERS_2)) {
		// ROW HEADERS ONLY
		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ KPA(xl.r_header),
			SET___ KPA(xl.r_header_clipped),
			KPA(r_screen_clipped),
			KP(r_screen.left) + col_lead, // Add space for any row header.
			KP(r_screen.top),
			KP(r_screen.width) - col_lead - col_trail,    // Less any row header col, less any Y scrollbar
			KP(xl.headerrowheight)  // Height of col header
		);
	}

	// Dead scrollbars mean scroll is 0 for their dimension.
	if (KP(viewcalc.show_scrollbar) == false && KP(r_scrollport.top) != 0)
		KP(r_scrollport.top) = 0;

	if (KP(viewcalcx.show_scrollbar) == false && KP(r_scrollport.left) != 0)
		KP(r_scrollport.left) = 0;

	RELATED_ (O_Rectangle_Refresh)

	return k;
}


RELATED_ (O_ListView_ColumnDragContinue Lister_Spawn Object_Draw_Recursive_Not_Form)
RELATED_ (O_ListView_Draw)


void O_ImageGrid_Draw (ktrl k)
{
	VarMyFont(myfont);

	Form_Get (f,k);

	Draw_Rect		(KPA(r_screen), KP(backcolor), alpha_1_0); // BLUE

	rect_s r_cursor;
	int beyond_row	= KP(viewcalc.first_vis_row) + KP(viewcalc.rowcount);
	int beyond_col	= KP(viewcalcx.first_vis_row) + KP(viewcalcx.rowcount);

corners_4:
    ; // oblig for gcc
	rect_s r_inset;

	if (Have_Flag (KP(xl.corner_bits), CORNER_LEFT_TOP_1)) {
		Draw_Rect	(KPA(xl.corner_LT), color3_gray_75, alpha_1_0); // Header button backcolor

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (KPA(xl.corner_LT), drawbut_command_up_4);
		} else {
			Draw_Rect	(KPA(xl.corner_LT), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, KP(xl.corner_LT), 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(KPA(xl.corner_LT), FP(flatcolor), alpha_1_0);
		}
	}
	if (Have_Flag (KP(xl.corner_bits), CORNER_RIGHT_TOP_2)) {
		Draw_Rect	(KPA(xl.corner_RT), color3_gray_75, alpha_1_0); // Header button backcolor

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (KPA(xl.corner_RT), drawbut_command_up_4);
		} else {
			Draw_Rect	(KPA(xl.corner_RT), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, KP(xl.corner_RT), 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(KPA(xl.corner_RT), FP(flatcolor), alpha_1_0);
		}
	}
	if (Have_Flag (KP(xl.corner_bits), CORNER_LEFT_BOTTOM_4)) {
		Draw_Rect	(KPA(xl.corner_LB), color3_gray_75, alpha_1_0); // Header button backcolor

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (KPA(xl.corner_LB), drawbut_command_up_4);
		} else {
			Draw_Rect	(KPA(xl.corner_LB), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, KP(xl.corner_LB), 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(KPA(xl.corner_LB), FP(flatcolor), alpha_1_0);
		}
	}
	if (Have_Flag (KP(xl.corner_bits), CORNER_RIGHT_BOTTOM_8)) {
		Draw_Rect	(KPA(xl.corner_RB), color3_gray_75, alpha_1_0); // Header button backcolor

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (KPA(xl.corner_RB), drawbut_command_up_4);
		} else {
			Draw_Rect	(KPA(xl.corner_RB), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, KP(xl.corner_RB), 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(KPA(xl.corner_RB), FP(flatcolor), alpha_1_0);
		}
	}

rowheaderdraw:
	if (false == Have_Flag (KP(xl.gridheaders_bound), HEADERS_ROW_HEADERS_1))
		goto colheaderdraw;

	RECT_COPY (r_cursor, KP(xl.r_rowheader));
	r_cursor.height = KP(gridcellheight);

	if (FP(appearance) != appearance_flat_1) {
		Draw_Rect	(KPA(xl.r_rowheader), color3_gray_75, alpha_1_0); // Header button backcolor
	}

	// Draw row headers.
	for (int row = KP(viewcalc.first_vis_row); row < beyond_row; row ++, r_cursor.top += KP(viewcalc.rowheight)) {
		RELATED_ (O_ListView_Draw)
		DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);

		PSTRING s_header = rowheadery(k, row); //ccs *s_header = colheaderx(k, col);
		rect_s r_column, r_column_clipped;
		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ ADDRESSOF(r_column),
			SET___ ADDRESSOF(r_column_clipped),
			KPA(xl.r_rowheader_clipped),
			//RECT_SEND(r_cursor)
			r_cursor.left, r_cursor.top, r_cursor.width, r_cursor.height
		);

		INCREASE___ Draw_Clip_Set_Rect (ADDRESSOF(r_column_clipped));

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);
		} else {
			Draw_Rect	(ADDRESSOF(r_cursor), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, r_cursor, 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(ADDRESSOF(r_inset), FP(flatcolor), alpha_1_0);
		}

		rect_s r_text, r_textarea;
		r_text.width = Draw_StringWidthInt (myfont, s_header, KP(fontsize));
		r_text.height = KFONTSIZE(k);

		RECT_SET_ADJUST (r_textarea, r_cursor, +LV_TEXTMARGIN_10, +LV_TEXTMARGIN_10, -LV_TEXTMARGIN_10*2, -LV_TEXTMARGIN_10*2); // Margin 3
		Label_Rect_Edit_Align (ADDRESSOF(r_text), ADDRESSOF(r_textarea), alignment_middle_left_3);
		Draw_String (ADDRESSOF(r_text), s_header, KP(fontsize), color3_black, alpha_1_0);
	}

colheaderdraw:
	if (false == Have_Flag (KP(xl.gridheaders_bound), HEADERS_COL_HEADERS_2))
		goto cells_draw;

	RECT_COPY (r_cursor, KP(xl.r_header));
	r_cursor.width = KP(gridcellwidth);

	INCREASE___ Draw_Clip_Set_Rect (KPA(r_screen_clipped));
	Draw_Rect	(KPA(xl.r_header), color3_gray_75, alpha_1_0); // Header button backcolor

	// Draw col headers.
	for (int col = KP(viewcalcx.first_vis_row); col < beyond_col; col ++, r_cursor.left += KP(viewcalcx.rowheight)) {
		DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);

		PSTRING s_header = colheaderx(k, col);
		rect_s r_column, r_column_clipped;
		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ ADDRESSOF(r_column),
			SET___ ADDRESSOF(r_column_clipped),
			KPA(xl.r_header_clipped),
			r_cursor.left, r_cursor.top, r_cursor.width, r_cursor.height
		);

		INCREASE___ Draw_Clip_Set_Rect (ADDRESSOF(r_column_clipped));

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);
		} else {
			Draw_Rect	(ADDRESSOF(r_cursor), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, r_cursor, 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(ADDRESSOF(r_inset), FP(flatcolor), alpha_1_0);
		}

		rect_s r_text, r_textarea;
		if (s_header[0]) {
			r_text.width = Draw_StringWidthInt (myfont, s_header, KP(fontsize));
			r_text.height = KFONTSIZE(k);

			RECT_SET_ADJUST (r_textarea, r_cursor, +LV_TEXTMARGIN_10, +LV_TEXTMARGIN_10, -LV_TEXTMARGIN_10*2, -LV_TEXTMARGIN_10*2); // Margin 3
			Label_Rect_Edit_Align (ADDRESSOF(r_text), ADDRESSOF(r_textarea), alignment_middle_left_3);
			Draw_String (ADDRESSOF(r_text), s_header, KP(fontsize), color3_black, alpha_1_0);
		}
	} // col headers


cells_draw:

	// CELLS DRAW

	RECT_COPY (r_cursor, KP(viewcalc.r_row_first)); // updated in refresh
	r_cursor.width = KP(gridcellwidth); // Does not have spacing in it.
	r_cursor.height = KP(gridcellheight); // Does not have spacing in it.


	INCREASE___ Draw_Clip_Set_Rect (KPA(r_interior_clipped));

	qbool has_image_background = false;

	if (k->image_name_a) {
		has_image_background = true;
		RELATED_ (O_Image_Draw)
		//Draw_Rect	(KPA(r_interior), color3_white, k->imagealpha);
		cachepic_t *p = Draw_CachePic_Flags (KP(image_name_a), CACHEPICFLAG_NOTPERSISTENT | CACHEPICFLAG_QUIET); // Baker March 9 2026
		Draw_ImageCachePic (KPA(r_interior), p, KP(imagecolor), KP(imagealpha));

	}

	int imagelistcount = KP(imagelist_a.numstrings);
	for (int row = KP(viewcalc.first_vis_row); row < beyond_row; row ++, r_cursor.top += KP(viewcalc.rowheight)) {

		r_cursor.left = KP(viewcalcx.r_row_first.left); // column 0
		int row_idx = row * KP(gridcolumns);
		int idx = row_idx + KP(viewcalcx.first_vis_row);

		for (int col = KP(viewcalcx.first_vis_row); col < beyond_col; idx ++, col ++, r_cursor.left += KP(viewcalcx.rowheight)) {

			if (KP(gridmaxcount)) {
				if (idx >= KP(gridmaxcount))
					continue; // Cell cannot be drawn
			}

			PSTRING simg = NULL;
			if (idx < imagelistcount) {
				PSTRING s = k->imagelist_a.strings[idx];
				if (strlen(s))
					simg = s;
			}

			rect_s rimag = r_cursor;	// But padding indents the leftbar

			// If image background, don't draw over it except for cursor.
			if (false == has_image_background)
				Draw_Rect (ADDRESSOF(r_cursor), KP(gridcellpaddingcolor), alpha_1_0);

			rimag.left += KP(gridcellpadding);
			rimag.top += KP(gridcellpadding);
			rimag.width -= KP(gridcellpadding) * 2;
			rimag.height -= KP(gridcellpadding) * 2;

			qbool is_selected = (idx == KP(selectedindex));
			if (is_selected) {
				if (k->cursor_image_name_a) {
					cachepic_t *p = Draw_CachePic_Flags (KP(cursor_image_name_a), CACHEPICFLAG_NOTPERSISTENT | CACHEPICFLAG_QUIET); // Baker March 9 2026
					if (FP(frm.kfocused) == k)
						Draw_ImageCachePic (ADDRESSOF(r_cursor), p, KP(backcolorfocusselected), alpha_1_0);
					else
						Draw_ImageCachePic (ADDRESSOF(r_cursor), p, KP(backcolorselected), alpha_1_0);
				} else {
					if (FP(frm.kfocused) == k)
						Draw_Rect (ADDRESSOF(rimag), KP(backcolorfocusselected), alpha_1_0);
					else
						Draw_Rect (ADDRESSOF(rimag), KP(backcolorselected), alpha_1_0);
				}
			}
			else {
				// If image background, don't draw over it except for cursor.
				if (false == has_image_background)
					Draw_Rect (ADDRESSOF(rimag), KP(gridcellbackcolor), alpha_1_0);
			}

			if (KP(imagemargin)) {
				rimag.left += KP(imagemargin);
				rimag.top += KP(imagemargin);
				rimag.width -= KP(imagemargin) * 2;
				rimag.height -= KP(imagemargin) * 2;
			}
			if (simg) {
				cachepic_t *p = Draw_CachePic_Flags (simg, CACHEPICFLAG_NOTPERSISTENT);
				Draw_ImageCachePic (ADDRESSOF(rimag), p, KP(imagecolor), KP(imagealpha));
			}

		} // col
	} // row
}

// If mouse down is set AFTER.
ktrl O_ImageGrid_ItemDragContinue (ktrl k, int x, int y, int is_done)
{
	Form_Get(f,k);

#if 0
	float	delta_x		= x - KP(ui.mouse_down_origin.x);
	float	delta_y		= y - KP(ui.mouse_down_origin.y);
#endif // 0

	// Check what we are over?

	RELATED_ (Form_MouseButtonAction)
	ktrl khit = Form_Hit_Drawn_Any(f, x, y);

	int newidx = not_found_neg1;
	int oldidx = UNPLUS1(KP(ui.mouse_down_thingi));

	if (khit == k) {
		// Over self.
		int newcol = ViewCalc_ColumnCollide (k, x, y);
		int newrow = ViewCalc_RowCollide (k, x, y);

		if (newcol != not_found_neg1) {
			if (newrow != not_found_neg1) {
				newidx = newrow * KP(gridcolumns) + newcol;
				// QC: Ask self if drag accept
				if (newidx != oldidx) {
					KP(ui.mousepointer)			= mousepointer_crosshair_3;
				} else {
					KP(ui.mousepointer)			= mousepointer_waitarrow_4;
				}
			} // row
		} // col

		if (newidx == not_found_neg1) {
			KP(ui.mousepointer)			= mousepointer_prohibited_10;
		}
	} else {
		// QC: Ask other if drag accept
		KP(ui.mousepointer)			= mousepointer_prohibited_10;
	}

	if (is_done) {
		// Potential drag end.
		KP(ui.mousepointer)			= mousepointer_arrow_default_0;
		KP(ui.mouse_down_thingi)		= 0;
	}

	//Form_QueueRefresh (KP(fctrl)); // This is the only form refresh available right now!

	return k;
}

RELATED_ (Lister_MouseButtonAction O_ListView_MouseButtonAction)

ktrl O_ImageGrid_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	// Mouse Up + In Cell Drag?  Do a DragContinue with is_done == true
	if (is_down == false && KP(ui.mouse_down_thingi) >= DRAG_IDX_PLUS1_MIN_1) {
		// END THE DRAG
		return O_ImageGrid_ItemDragContinue(k,x,y, isdone_true);
	}

	// MOUSEDOWN
	KP(ui.mouse_down_thingi)		= 0; // Clear it.

	int newcol = ViewCalc_ColumnCollide (k, x, y);
	int newrow = ViewCalc_RowCollide (k, x, y);

	if (newcol == not_found_neg1) {
		return k; // Baker: Moving over a row header probably does this
	}

	if (newrow == not_found_neg1)
		return k; // Hope this can't happen - Baker: Moving over a col header probably does this

	int newidx = newrow * KP(gridcolumns) + newcol;

	if (newidx != KP(selectedindex)) {
		KP(selectedindex) = newidx;
		if (in_range(KP(viewcalcx.first_full_row), newcol, KP(viewcalcx.last_full_row)) == false)
			Lister_VisRangeCheck_GridX_Refresh (k, /*pagesize_col*/ 0, newcol);

		// Baker: Don't scroll unless we have to
		if (in_range(KP(viewcalc.first_full_row), newrow, KP(viewcalc.last_full_row)) == false)
			Lister_VisRangeCheck_GridY_Refresh (k, /*pagesize_row*/ 0, newrow);

		//Lister_VisRangeCheckY_Refresh (k, /*movecount*/ 0);
		Object_Event_OnChange_Recursive (k, shallrecurse_false); // Jan 17 2025
	}

	CALLERS_ (Form_MouseButtonAction)
	RELATED_ (QC_ImageGrid_DragIsPossible) // chance to ask QC if ok to drag this

	// Baker: We cannot get here without a valid newidx, excluded above.

	int imagelistcount = KP(imagelist_a.numstrings);
	if (KP(dragenabled)) {
		qbool isdragpossible = false;
		if (in_range_beyond(0, newidx, imagelistcount)) {
			PSTRING simg = KP(imagelist_a.strings[newidx]);
			if (strlen(simg) > 0) {
				 // QC_ImageGrid_DragIsPossible
				isdragpossible = true; // imaged item
			}
		} // imaged item

		if (isdragpossible) {
			// O_ImageGrid_ItemDragStore
			// Baker: These should cover.
			KP(ui.mouse_down_thingi)	= PLUS1(newidx);
			KP(ui.mouse_down_time)		= Host_RealTime();
			KP(ui.mouse_down_origin.x)	= x;
			KP(ui.mouse_down_origin.y)	= y;
			KP(ui.is_move_thresh_met)	= false;
			KP(ui.mousepointer)			= mousepointer_arrow_default_0;
		}
	}
	return k;
}


RELATED_ (O_ListView_MouseMove)
ktrl O_ImageGrid_MouseMove (ktrl k, int x, int y)
{
	if (KFP(frm.kmousedown) == NULL) {
		// Mousemove with no buttons down - Event mousemove here?
		return NULL;
	}

	if (KP(ui.mouse_down_thingi) == 0)
		return NULL;

	// DRAG

	if (KP(ui.is_move_thresh_met) == false) {
		// O_ImageGrid_ItemDragStart ? -- Drag possible
#if 0
		float	delta_x		= x - KP(ui.mouse_down_origin.x);
		float	delta_y		= y - KP(ui.mouse_down_origin.y);
#endif // 0

		qbool threshold_met = true;

		if (threshold_met) {
			// O_ImageGrid_ItemDragStart
			KP(ui.is_move_thresh_met) = true;
			KP(ui.mousepointer) = mousepointer_hand_11;
		}
	}

	// DRAGGING
	if (KP(ui.is_move_thresh_met) == false)
		return NULL;

	return O_ImageGrid_ItemDragContinue (k, x, y, isdone_false);
}

// This must return null if no action handled
// Feb 3 2026 - Only getting KeyDowns right now.
ktrl O_ImageGrid_KeyDown (ktrl k, int key, int unicode)
{
#if 0
	int colcount = KP(gridrows);
	int rowcount = KP(gridcolumns);
#endif // 0

	qbool is_paging = false; // Paging, we want the new index to be top or bottom.
#if 0
	qbool is_horizontal_move = false;
	qbool is_vertical_move = false;
#endif // 0

	int oldselidx = KP(selectedindex);
	int oldselcol = floormod(KP(selectedindex), KP(gridcolumns)); // Remainder of integer division (CORRECT)
	int oldselrow = ifloor(KP(selectedindex) / KP(gridcolumns)); // Floor divison (CORRECT)

	int newcol = oldselcol;
	int newrow = oldselrow;

	// Baker: Dec 11 2025 - First we need to figure out if key is horizontal?
	// 1. Get arrow keys working first.

	switch (key) {
	default:					return NULL; // NO ACTION
	case_break K_LEFTARROW:		newcol = oldselcol - 1;
	case_break K_RIGHTARROW:	newcol = oldselcol + 1;
	case_break K_UPARROW:		newrow  = oldselrow - 1;
	case_break K_MWHEELUP:		newrow  = oldselrow - 1;
	case_break K_DOWNARROW:		newrow  = oldselrow + 1;
	case_break K_MWHEELDOWN:	newrow  = oldselrow + 1;
	case_break K_HOME:			newrow = 0;	newcol = 0;
	case_break K_END:			newrow = KP(gridrows) - 1;	newcol = KP(gridcolumns) - 1;
	case_break K_PGUP:			newrow -= KP(viewcalc.pageamount); is_paging = true;
	case_break K_PGDN:			newrow += KP(viewcalc.pageamount); is_paging = true;
	} // sw

	// If we came here, we did something
	vclamp (newcol, 0, KP(gridcolumns) - 1);
	vclamp (newrow, 0, KP(gridrows) - 1);

	KP(selectedindex) = newrow * KP(gridcolumns) + newcol;
	//vclamp (KP(selectedindex), 0, listcount - 1);

	int pagesize_row = 0;
	int pagesize_col = 0;
	int distcol = newcol - oldselcol;
	int distrow = newrow - oldselrow;
	if (is_paging) {
		//pagesize = KP(selectedindex) - oldsel;
		pagesize_col = distcol;
		pagesize_row = distrow;
	}

	if (KCLASS(k) != class_contextmenu) { // January 16 2025 - For now ...
		// Baker: Don't scroll unless we have to
		if (in_range(KP(viewcalcx.first_full_row), newcol, KP(viewcalcx.last_full_row)) == false)
			Lister_VisRangeCheck_GridX_Refresh (k, pagesize_col, newcol);

		if (in_range(KP(viewcalc.first_full_row), newrow, KP(viewcalc.last_full_row)) == false)
			Lister_VisRangeCheck_GridY_Refresh (k, pagesize_row, newrow);
	}

	if (oldselidx != KP(selectedindex)) {
		Object_Event_OnChange_Recursive (k, shallrecurse_false);
	}

	return k;
}

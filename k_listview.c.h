// k_listview.c.h

RELATED_ (O_ListView_ColumnDragContinue)
ktrl O_ListView_Spawn (ktrl k)
{
	// This occurs too early to spawn things dependent on properties, no properties have been read yet ...

	return Lister_Spawn(k); // Does listview specific things too.
}


RELATED_ (Lister_Spawn ViewCalc_FixedAnyXY_Set O_ListBox_Draw Object_Draw_Recursive_Not_Form)

// Baker:
// Q: This can run multiple times?  March 3 2026
// A: YES.  Because QuakeC does not finalize the same way.  We need the same flow everywhere.
// A: So yes, this can run twice, but should be only twice.  Anyway, don't do anything twice unsafely ....
RELATED_ (O_TextBox_Finalize)
void O_ListView_Finalize (ktrl k)
{
	if (k->have_ext_closer && KP(servo.kcloser) == NULL) {
		RELATED_ (O_ListView_Refresh) // <========== Sizing occurs

		ktrl kext /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_ext_closer, KPA(servo.kcloser),
			KP(fctrl), /*container*/ SERVANT_KCAN_NULL, /*servantto*/ k);
		Object_Property_Set (kext, "ToolTip", "Close");
	}
	if (k->have_ext_mover && KP(servo.kmover) == NULL) {
		ktrl kext /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_ext_mover, KPA(servo.kmover),
		KP(fctrl), SERVANT_KCAN_NULL, k);
	}
	if (k->have_ext_sizer && KP(servo.ksizer) == NULL) {
		ktrl kext /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_ext_mover, KPA(servo.ksizer),
		KP(fctrl), SERVANT_KCAN_NULL, k);
		kext->direction = alignment_bottom_right_8;
	}

	// NO COLUMN WIDTHS?  TIME TO DEFAULT THEM.
	int column_widths_count = k->columnwidths_a.count;

#if 0
	// Baker: Not sure how I feel about this Jan 14 2026
	if (KP(columncount) == 0)
		k.columncount = column_widths_count;
#endif

	// Baker: Nov 9 2025 - make sure column widths are all filled in.
	for (int col = column_widths_count; col < KP(columncount); col ++) {
		int32s_add1 (&k->columnwidths_a, LV_DEFAULT_WIDTH_100);
	} // for

	for (int col = 0; col < KP(columncount); col ++) {
		vclamp_lo (k->columnwidths_a.ints[col], LV_MIN_WIDTH_0);
	} // for
}

// k.r_screen has been updated and all draw properties known.
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF)
RELATED_ (Lister_SetPctHorzListView O_ListBox_Refresh)
ktrl O_ListView_Refresh (ktrl k)
{
	//zpr ("O_ListView_Refresh");
	// Adjust interior.
	if (k->_rowheight_orig == 0) {
		// Baker: This is a refresh.  It is fine.
		KP(rowheight_rt) = KFONTSIZECEIL(k); // Because fontsize is a float
		KP(rowheightpad_rt) = iround((KP(rowheight_rt) - KFONTSIZE(k)) / 2.0);
	}

	if (k->is_noheaderrow)
		KP(xl.headerrowheight) = 0;
	else
		KP(xl.headerrowheight) = KFONTSIZECEIL(k) + LV_HEADERROW_ADD_6; // Not my_rowheight at this time.  Only for cells.

	RECT_SET (KP(r_interior), KP(r_screen.left), KP(r_screen.top) + KP(xl.headerrowheight),
		KP(r_screen.width), KP(r_screen.height) - KP(xl.headerrowheight));

	if (KP(servo.kcloser)) {
		// PERFORM ADJUSTMENTS
		int sizey = iceil(KP(fontsize) * 0.8);
		int inny = iceil(sizey / 3.0);// ifloor(KP(fontsize) * 0.20);
		k->servo.kcloser->cm.relative_rect.left	= RECT_RIGHTOF(k->r_screen);// - sizey - inny;// * autocvar_magnification;
		k->servo.kcloser->cm.relative_rect.width = sizey;
		k->servo.kcloser->cm.relative_rect.top	= k->r_screen.top + inny;
		k->servo.kcloser->cm.relative_rect.height = sizey;
	}
	if (KP(servo.kmover)) {
		// PERFORM ADJUSTMENTS
		k->servo.kmover->cm.relative_rect.left	= k->r_screen.left + ifloor(k->r_screen.width / 4);
		k->servo.kmover->cm.relative_rect.width = ifloor(k->r_screen.width / 2);
		k->servo.kmover->cm.relative_rect.top	= k->r_screen.top;
		k->servo.kmover->cm.relative_rect.height = ifloor(KP(fontsize) * 0.8);
	}
	if (KP(servo.ksizer)) {
		// PERFORM ADJUSTMENTS
		//int sizey = ifloor(KP(xl.headerrowheight) * 0.60);
		//int inny = ifloor(KP(xl.headerrowheight) * 0.20);
		int sizey = iround(KP(fontsize) * 0.8);
//		int inny = ifloor(sizey / 3.0);// ifloor(KP(fontsize) * 0.20);
		k->servo.ksizer->cm.relative_rect.left	= RECT_RIGHTOF(k->r_screen);// - sizey - inny;// * autocvar_magnification;
		k->servo.ksizer->cm.relative_rect.width = sizey;
		k->servo.ksizer->cm.relative_rect.top	= RECT_BOTTOMOF(k->r_screen);// - sizey - inny;
		k->servo.ksizer->cm.relative_rect.height = sizey;
	}

	int calc_again_xscroll_showed = false;
	qbool did_remove_height = false;

xscroll_appeared_recalc_yscroll:
	Lister_FixedY_Refresh (k); // Calcs Y scroll ---- REFRESH VIEWCALC OCCURS HERE

	ktrl kscrollx = KP(servo.kscrollx);
	ktrl kscrolly = KP(servo.kscrolly);

	if (KP(columncount) == 0) {
		// Baker: This cannot render
		// Baker: This needs to always set 0 scrollbars
		KXP(kscrollx, is_hidden) = KXP(kscrolly, is_hidden) = true;
		KP(r_scrollport.left) = KP(r_scrollport.top) = 0;
		return NULL;
	}

	int rowcount = Lister_ListCountY(k);
	if (!rowcount) {
		// Baker: This should render with no data.
		// Baker: This needs to always set 0 scrollbars
		KXP(kscrollx, is_hidden) = KXP(kscrolly, is_hidden) = true;
		KP(r_scrollport.left) = KP(r_scrollport.top) = 0;
		// return NULL; NO ... RENDER!
	}

	KP(xl.startcol)		= 0;
	KP(xl.beyondcol)	= KP(columncount);

	// We always must recalc this, even if we are on 2nd recalc
	if (1) {
		// Nov 9 2025: viewcalc_s vcx = ViewCalcColumns_Set (k, &KP(viewcalcx), &KP(r_interior));
		ViewCalcColumns_SetX (k, KP(r_interior));

		KP(r_scrollport.width)		= KP(r_interior.width);
		KP(r_scrollport.height)		= KP(viewcalc.rowheight) * rowcount;

		if (!rowcount || k->is_noscrollbars) { // X Scroll
			// Phantom column screws up calcs with no row ... just hide the X scroll
			KP(viewcalcx.show_scrollbar) = false;
			KXP(kscrollx, is_hidden) = true;
		} else {
			// NORMAL CALC WITH ROWS
			KXP(kscrollx, is_hidden) = (KP(viewcalcx.show_scrollbar) == false); //(vcx.show_scrollbar == false);
		}

		if (KP(viewcalcx.show_scrollbar)) { //if (vcx.show_scrollbar) {
			rect_s r_scroll; // This is after y scroll.
			RECT_COPY (r_scroll, KP(r_screen));
			r_scroll.left	= KP(r_screen.left);
			r_scroll.width	= KP(r_screen.width) - KP(viewcalc.show_scrollbar) * KNOB_SIZE_16;
			r_scroll.top	= RECT_BOTTOMOF(KP(r_screen)) - KNOB_SIZE_16;
			r_scroll.height = KNOB_SIZE_16;

			RELATED_ (Lister_SetPctHorzListView)

			// PERFORM ADJUSTMENTS
			RECT_COPY (KXP(kscrollx, cm.relative_rect), r_scroll);

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

	// Calculate header interior
	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(xl.r_header),
		SET___ KPA(xl.r_header_clipped),
		KPA(r_screen_clipped),
		KP(r_screen.left),
		KP(r_screen.top),
		KP(r_screen.width) - KP(viewcalc.show_scrollbar) * KNOB_SIZE_16,  // Less Y scrollbar
		KP(xl.headerrowheight)
	);

	// Calculate interior
	// Calculate header interior
	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(r_interior),
		SET___ KPA(r_interior_clipped),
		KPA(r_screen_clipped),
		KP(r_screen.left),
		KP(r_screen.top) + KP(xl.r_header.height),
		KP(r_screen.width) - KP(viewcalc.show_scrollbar) * KNOB_SIZE_16,  // Less Y scrollbar
		KP(r_screen.height) - KP(viewcalcx.show_scrollbar) * KNOB_SIZE_16 - KP(xl.r_header.height)	// Less X scrollbar
	);

	// Dead scrollbars mean scroll is 0 for their dimension.
	if (KP(viewcalc.show_scrollbar) == false && KP(r_scrollport.top) != 0) {
		if (k->is_noscrollbars == false) {
			KP(r_scrollport.top) = 0;
		} else {
			// Still scrolls for noscrollbars style
		}
	}

	if (KP(viewcalcx.show_scrollbar) == false && KP(r_scrollport.left) != 0)
		KP(r_scrollport.left) = 0;

	if (KP(ui.mouse_down_thingi) >= LV_DRAG_COLSIZE_10) {
		RELATED_ (O_ListView_ColumnDragContinue)
	}

	//Vid_SetWindowTitlef ("x scroll %d .. all cols %d r_data %d", KP()r_scrollport.left, KP()xl.columnsallwidths, KP()xl.r_data.width);

// PRECOMPUTE STRING LENGTH FOR ALL CELLS == METRIC REFRESH
precompute_alignment_pixels_lengths:
	// Easier to compute in place.
#ifdef QUAKEC
	stringlist_reset (k.stringwidths_rt_a);
#else
	NPA__ int32s_freecontents (&k->stringwidths_rt_a);
#endif

	return k;
}

// Draw enters with clip set to screen
void O_ListView_Draw (ktrl k)
{
	rect_s r_inset;

	if (!KP(columncount)) {
		Draw_Rect	(KPA(r_screen), KP(backcolor), alpha_1_0);
		return;
	}

	int rowcount = Lister_ListCountY(k);
	int stringscount = SLISTCOUNT(KP(list_strings_a));

	VarMyFont(myfont); // This is a setfont

	Form_Get (f,k);

	if (KP(is_nobackground))
		goto nobackground;
	switch (FP(appearance)) {
	default:
		Draw_Rect	(KPA(r_screen), color3_gray_50, alpha_1_0);
		if (KP(is_noheaderrow) == false) {
			Draw_Rect	(KPA(xl.r_header), color3_gray_75, alpha_1_0); // Header button backcolor
		}
		Draw_Rect	(KPA(r_interior), KP(backcolor), alpha_1_0);
		break;

	case appearance_flat_1:
		Draw_Rect	(KPA(r_screen), FP(scrollbarcolor), alpha_1_0);
		{
			rect_s r_fake;  RECT_COPY (r_fake, KP(r_screen));
			//r_fake.width = k->xl.r_header.width;
			r_fake.height = KP(xl.r_header.height);
			//rect_s r_inset;
			RECT_SET_ADJUST (r_inset, r_fake, 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(ADDRESSOF(r_fake), color3_black, alpha_1_0);
			Draw_Rect	(ADDRESSOF(r_inset), FP(flatcolor), alpha_1_0);
		}
		//Draw_Rect	(&k->r_screen, f->scrollbarcolor, alpha_1_0);
		if (KP(is_noheaderrow) == false) {
			Draw_Rect	(KPA(xl.r_header), FP(scrollbarthumbcolor), alpha_1_0); // Header button backcolor
		}
		Draw_Rect	(KPA(r_interior), KP(backcolor), alpha_1_0);
		break;

	} // sw

nobackground:
#ifndef QUAKEC
    ; // Oblig for mingw
#endif // !QUAKEC
	// Draw each column
	rect_s r_text, r_textarea;
	r_text.height = KFONTSIZECEIL(k);

	// There is a clipzone for each column
	rect_s r_cursor; RECT_COPY (r_cursor, KP(xl.r_header));
	r_cursor.left -= KP(r_scrollport.left); // Scroll

	// Determine if the sort is pressed.
	// If pressed, column header background = different color
	int sortpress = KP(ui.mouse_down_thingi);
	int sortcolumnpress = not_found_neg1;
	if (sortpress) {
		sortcolumnpress = -KP(ui.mouse_down_thingi) + LV_SORT_PRESS_COLUMN_NEG2; // Decode the column
		//printvarf(sortcolumnpress);
	}

	if (k->is_noheaderrow)
		goto noheaderrow;

	// We draw 1 column too many for a phantom column if it is needed to fill the interior
	for (int col = 0; col <= KP(columncount); col ++, r_cursor.left += r_cursor.width) {
		r_cursor.top = KP(r_screen.top); // Reset to top
		r_cursor.height = KP(xl.headerrowheight);
		r_cursor.width	= colwidthx (k,col);

		if (RECT_RIGHTOF(r_cursor) <= KP(xl.r_header.left)) continue;
		if (RECT_RIGHTOF(KP(xl.r_header)) <= r_cursor.left) continue; // Not break due to phantom

//phantom:
		// PHANTOM HEADER - extra column
		//Dec 22 2024 unused ... int is_phantom_now = col >= k->columncount && k->xl.is_phantom;
		if (col == KP(xl.beyondcol)) {
			if (KP(xl.is_phantom)) {
				r_cursor.width = KP(xl.phantom_width);
				INCREASE___ Draw_Clip_Set_Rect (KPA(xl.r_header_clipped)); // Object_Draw sets to r_screen
				if (FP(appearance) != appearance_flat_1) {
					DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);
				} else {
					Draw_Rect	(ADDRESSOF(r_cursor), color3_black, alpha_1_0);
					RECT_SET_ADJUST (r_inset, r_cursor, 1, 1, -1*2, -1*2); // Margin
					Draw_Rect	(ADDRESSOF(r_inset), FP(flatcolor), alpha_1_0);
				}
			}
			break; // End phantom
		}

//header:
		// Column header
		PSTRING s_header = colheaderx(k, col);
		rect_s r_column, r_column_clipped;
		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ ADDRESSOF(r_column),
			SET___ ADDRESSOF(r_column_clipped),
			KPA(xl.r_header_clipped),
			r_cursor.left,
			r_cursor.top,
			r_cursor.width,
			KP(xl.r_header.height)
		);

		INCREASE___ Draw_Clip_Set_Rect (ADDRESSOF(r_column_clipped));

		if (FP(appearance) != appearance_flat_1) {
			DrawKit_Button_Except_Face (ADDRESSOF(r_cursor), drawbut_command_up_4);
#if 1 // SORTDRAW
			// SORT DRAW
			// If sort press is going on for this column do some blue
			if_2 (sortpress, sortcolumnpress == col) {
				Draw_Rect (ADDRESSOF(r_cursor), color3_blue, alpha_0_25);
			}

			// If sorted by this column, draw a small arrow up or down.
			if_2 (KP(is_sorted), KP(sortcolumnindex) == col) {
				// Center, top how?
				rect_s r_centop;
				Rect_Top_Centered_For_Width_Height (ADDRESSOF(r_centop), &r_cursor, 30 * 0.5, 30 * 0.25);
				r_centop.top += 2;
				if (KP(sortcolumndescending)) {
					//drawpic (rtov(r_centop), "engine/bicons/arrow_down.png", rsizetov(r_centop), gray_555, alpha_1_0, DRAWFLAG_NORMAL_0); // metric!

					cachepic_t *p = Draw_CachePic_Flags ("engine/bicons/arrow_down.png", CACHEPICFLAG_NOTPERSISTENT);
					Draw_ImageCachePic (&r_centop, p, color3_gray_50, alpha_1_0);
				}
				else {
					//drawpic (rtov(r_centop), "engine/bicons/arrow_up.png", rsizetov(r_centop), gray_555, alpha_1_0, DRAWFLAG_NORMAL_0); // metric!
					cachepic_t *p = Draw_CachePic_Flags ("engine/bicons/arrow_up.png", CACHEPICFLAG_NOTPERSISTENT);
					Draw_ImageCachePic (&r_centop, p, color3_gray_50, alpha_1_0);
				}
			} // sorted and this is the column
#endif // SORTDRAW

		} else {
			// FLAT APPEARANCE
			Draw_Rect	(ADDRESSOF(r_cursor), color3_black, alpha_1_0);
			RECT_SET_ADJUST (r_inset, r_cursor, 1, 1, -1*2, -1*2); // Margin
			Draw_Rect	(ADDRESSOF(r_inset), FP(flatcolor), alpha_1_0);
		}

		r_text.width = Draw_StringWidthInt (myfont, s_header, KP(fontsize));

		RECT_SET_ADJUST (r_textarea, r_cursor, +LV_TEXTMARGIN_10, +LV_TEXTMARGIN_10, -LV_TEXTMARGIN_10*2, -LV_TEXTMARGIN_10*2); // Margin 3
		Label_Rect_Edit_Align (ADDRESSOF(r_text), ADDRESSOF(r_textarea), alignment_middle_left_3);
		Draw_String (ADDRESSOF(r_text), s_header, KP(fontsize), color3_black, alpha_1_0);

	} // for

noheaderrow:
#ifndef QUAKEC
    ; // Oblig for mingw
#endif // !QUAKEC
	rect_s r_selected; RECT_COPY (r_selected, KP(r_interior));

	int abs_sellength = abs(KP(selectedlength));
	int firstrow = KP(selectedindex) - KP(selectedlength);
	int lastrow  = KP(selectedindex);
	if (firstrow > lastrow) swapi (&firstrow, &lastrow);

	r_selected.top = KP(r_interior.top) - KP(r_scrollport.top) + firstrow * KP(viewcalc.rowheight);
	r_selected.height =KP(viewcalc.rowheight) * (abs_sellength + 1);

draw_stripe:
	if (Vector3_IsZeros(k->rowstripecolor1) && Vector3_IsZeros(k->rowstripecolor1))
		goto draw_selection;

	RECT_COPY (r_cursor, KP(r_interior));
	r_cursor.height = KP(viewcalc.rowheight);
	for (int row = KP(viewcalc.first_vis_row); row <= KP(viewcalc.last_partial_row); row ++, r_cursor.top += r_cursor.height) {
		PCOLOR	stripecolor	= Math_IsOdd(row) ? KP(rowstripecolor2) : KP(rowstripecolor1);
		float	stripealpha = Math_IsOdd(row) ? KP(rowstripealpha2) : KP(rowstripealpha1);

		if (Vector3_IsZeros (stripecolor))
			continue; // No color specified ... yeah no black
		rect_s r_stripe = { KP(r_interior.left), r_cursor.top, KP(r_interior.width), KP(rowheight_rt)};
		if (stripealpha)
			Draw_Rect	(ADDRESSOF(r_stripe), stripecolor, stripealpha);
		else
			Draw_Rect	(ADDRESSOF(r_stripe), stripecolor, alpha_1_0);
	} // Rows

draw_selection:
	if (rowcount) {
		INCREASE___ Draw_Clip_Set_Rect (KPA(r_interior_clipped)); // Object_Draw sets to r_screen
 		if (k->is_nobackground == false) {
			PCOLOR backcolor3 = KIsFocused (k) ? KP(backcolorfocusselected) : KP(backcolorselected);
			Draw_Rect (ADDRESSOF(r_selected), backcolor3, alpha_1_0);
		}
	}

	RECT_COPY (r_cursor, KP(r_interior));
	r_cursor.left -= KP(r_scrollport.left); // Subtract out the positive scroll.

draw_text:
	for (int col = 0 ; col < KP(columncount); col ++, r_cursor.left += r_cursor.width) {
		r_cursor.height = KP(viewcalc.rowheight);
		r_cursor.top = KP(r_interior.top);
		r_cursor.width	= colwidthx (k,col);

		if (RECT_RIGHTOF(r_cursor) <= KP(r_interior.left)) continue;
		if (RECT_RIGHTOF(KP(r_interior)) <= r_cursor.left) break;

		rect_s r_column, r_column_clipped;

		Rect_Calc_Clipped_Unclipped_From_XYWH (
			SET___ ADDRESSOF(r_column),
			SET___ ADDRESSOF(r_column_clipped),
			KPA(r_interior_clipped),
			r_cursor.left,
			r_cursor.top,
			r_cursor.width - LV_COLUMN_TEXTMARGIN_1, // This is less Y scrollbar and text margin
			KP(r_interior.height) // This is less X scrollbar
		);

		INCREASE___ Draw_Clip_Set_Rect (ADDRESSOF(r_column_clipped));

		for (int row = KP(viewcalc.first_vis_row); row <= KP(viewcalc.last_partial_row); row ++, r_cursor.top += r_cursor.height) {
			int		is_selected0	= in_range(firstrow, row, lastrow); // == KP(selectedindex));
			int		is_selected1	= is_selected0 && KP(is_nobackground) == false;
			PCOLOR	forecolor3		= is_selected1 ? KP(forecolorselected) : KP(forecolor);
			int		listidx		= rowcol_2idx(k,row, col);

			if (listidx >= stringscount)
				continue;

			PSTRING s	= SLISTGETINDEX (KP(list_strings_a), listidx);

			// Don't waste time drawing an empty string.
			if (!s[0])
				continue;

			if (k->is_markup && s[0] == '$') { // <img>
				ccs *simage = &s[1];
				RELATED_ (O_Image_Draw)
				rect_s r_image = { r_cursor.left + LV_TEXTMARGIN_10, r_cursor.top, k->rowheight_rt, k->rowheight_rt};
				cachepic_t *p = Draw_CachePic_Flags (simage, CACHEPICFLAG_NOTPERSISTENT | CACHEPICFLAG_QUIET); // Baker March 9 2026
				Draw_ImageCachePic (ADDRESSOF(r_image), p, color3_white, alpha_1_0);
			} else {
				if (k->columnalignments_a.count > col && k->columnalignments_a.ints[col] == alignment_top_right_2) {
					int my_string_width = k->stringwidths_rt_a.count > listidx ? k->stringwidths_rt_a.ints[listidx] : 0;
					if (my_string_width == 0) {
						if (listidx >= k->stringwidths_rt_a.count) {
							int32s_resize (&k->stringwidths_rt_a, k->list_strings_a.numstrings);
						}
						int new_string_width = my_string_width = Draw_StringWidthInt (myfont, s, KP(fontsize));
						k->stringwidths_rt_a.ints[listidx] = new_string_width;
					}
					Draw_String_XY (RECT_RIGHTOF(r_cursor) - LV_TEXTMARGIN_10 - my_string_width, r_cursor.top + KP(rowheightpad_rt), s, KP(fontsize), forecolor3, alpha_1_0);
				} else {
					Draw_String_XY (r_cursor.left + LV_TEXTMARGIN_10, r_cursor.top + KP(rowheightpad_rt), s, KP(fontsize), forecolor3, alpha_1_0);
				}
			}
		} // Rows
	} // Cols
}

ktrl O_ListView_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	int column; // used twice
	// Mouse Up + In Column Drag?  Do a DragContinue with is_done == true
	if (is_down == false && KP(ui.mouse_down_thingi) >= LV_DRAG_COLSIZE_10) {
		// Baker: isdown == false means this ends the drag
		return O_ListView_ColumnDragContinue(k,x,y, isdone_true);
	}

	// Baker: mouseup + downed on sort indicator means sort change mouseup
#if 1 // SORT UP
	if (is_down == false && KP(ui.mouse_down_thingi) <= LV_SORT_PRESS_COLUMN_NEG2) {
		if (KP(selectedlength)) { // Multiselect
			KP(selectedlength) = 0;
		}

		// SORT MOUSEUP
		// Up .. fire the sortable.  k.is_sortable is true if we are here.
		column = -KP(ui.mouse_down_thingi) + LV_SORT_PRESS_COLUMN_NEG2; // Decode the column
		if (KP(sortcolumnindex) == column)
			KP(sortcolumndescending) = !KP(sortcolumndescending); // If already selected toggle ascend/descend
		else
			KP(sortcolumnindex) = column;

		RELATED_ (DevListViewSort LvwServersSort)

		if (k->eve.onsort_a && k->eve.onsort_a[0]) {
			exofn_t onsort_fn = Event_Find_Function(k->eve.onsort_a);
			if (onsort_fn) {
				char *s_error_za = onsort_fn (k);
				if (s_error_za) {
					Con_PrintLinef ("ERROR: %s: " QUOTED_S, k->eve.onsort_a, s_error_za);
					Z_FreeNull_ (s_error_za);
				}
			} else {
				c_assert_msg_ (0, "Event_onsort not found!");
			}
		}

		Form_QueueRefresh (KP(fctrl)); // Drag started
	} // SORT MOUSEUP

#endif // SORT MOUSEUP
#if 0
	int scount = SLISTCOUNT (KP(list_strings_a));
#endif // 0
	if (is_down && /*scount &&*/ KP(columncount)
		&& RECT_HIT (KP(xl.r_header), x, y) ) {
			// Store the origin
			column = Hit_Sizer_Return_Column(k,x,y);
			if (column == not_found_neg1) {
				return NULL;
			}

#if 1
			// SORT MOUSEDOWN
			if (column <= LV_SORT_PRESS_COLUMN_NEG2) {
				// Missed the sizer.
				// Are we sortable?
				if (!KP(is_sorted))
					return NULL; // We do not sort

				//column = LV_SORT_PRESS_COLUMN_NEG2 - column;  // -2 + -2 = -4 for column 2
				// Column is -column -2
				// 0 - 2 = -2
				// 1 - 2 = -3
				//column = -column + LV_SORT_PRESS_COLUMN_NEG2; // -4 ===> -(-4) - 2 Turn back into column number

				//zpr ("Being down for column %d", column);
				KP(ui.mouse_down_time) = Host_RealTime();
				KP(ui.mouse_down_origin.x) = x;
				KP(ui.mouse_down_origin.y) = y;
				KP(ui.mouse_down_thingi) = column; // LV_SORT_PRESS_COLUMN_NEG2 - column; // Encode the column:  0 => -2, 1 => -3, ..

				//Form_QueueRefresh (k.fctrl); // Drag started
				return k;
			}  // SORT
#endif // 1 SORT

			// O_ListView_ColumnDragStart
			KP(ui.mouse_down_time) = Host_RealTime();
			KP(ui.mouse_down_origin.x) = x;
			KP(ui.mouse_down_origin.y) = y;
			KP(ui.mouse_down_thingi) = LV_DRAG_COLSIZE_10 + column;
			KP(ui.mouse_down_valuei) = colwidthx (k, column);
			KP(ui.is_move_thresh_met) = true;
			KP(ui.mousepointer) = mousepointer_baker_column_20;
			//Vid_SetWindowTitlef ("Listview mouse down sizing %d", k->ui.mouse_down_valuei);
			//Con_PrintLinef ("Listview mouse down sizing %d", k->ui.mouse_down_valuei);

			// Baker: Technically a refresh is needed if one
			// of the internals change.  This is mouse down
			// (NOT mouse move) and we just are storing things
			// like mousedown position and time.
			// So no refresh needs to occur.
			//Form_QueueRefresh (k.fctrl); // Drag started
			return k;
			//Con_PrintLinef ("Mousedown at %d", k->mousedown_thumb_at_mousedown_px.y);
	}


	if (Lister_MouseButtonAction (k, x, y, is_down)) { // This acts as listbox row check (interior collide)
		if (KP(selectedlength)) { // MULTISELECT
			KP(selectedlength) = 0;
		}
		return k;
	}

	return NULL;
}


ktrl O_ListView_MouseButtonAction2 (ktrl k, int x, int y, int is_down)
{
	if (Lister_MouseButtonAction (k, x, y, is_down)) { // This acts as listbox row check (interior collide)
		if (KP(selectedlength)) {
			KP(selectedlength) = 0;
		}
		return k;
	}

	return NULL;
}

// Return here is important. NULL means mousepointer returns to default.
ktrl O_ListView_MouseMove (ktrl k,int x, int y)
{
	if (KFP(frm.kmousedown) == k) {
		// Baker: mouse_down_thingi will be LV_DRAG_COLSIZE_10 or greater if a column drag is occuring
		if (KP(ui.mouse_down_thingi) >= LV_DRAG_COLSIZE_10) {
			return O_ListView_ColumnDragContinue (k, x, y, isdone_false);
			//return O_ListView_ColumnDragContinue (k,x,y);
		}
	}

	// See if we hit a column header hotspot.
	if (!KP(columncount)) return NULL;
//#if 0
//	int listcount = Lister_ListCountY(k);
//#endif // 0
	//if (!listcount) return NULL;

	if (RECT_HIT (KP(xl.r_header), x, y) == false)
		return NULL;

	int ret = Hit_Sizer_Return_Column(k,x,y);
	if (ret >=0) {
		KP(ui.mousepointer) = mousepointer_baker_column_20;
		return k;
	}

	return NULL;
}



// Return NULL if input not processed.
RELATED_ (ZForm_MouseMove)
CALLERS_ (Form_KeyDown)
#if 0
ktrl O_ListView_KeyDown (ktrl k, int key, int unicode)
{
	return Lister_KeyDown(k, key, unicode);
}
#endif

// selectedindex is the cursor
// If I am at 5 and I press SHIFT + down
//		1) selected index goes to 6
//		2) selectedlength goes to 1.
void O_ListView_SelectedIndex_Set (ktrl k, int selectedindex)
{
	int rowcount = Lister_ListCountY (k);
	k->selectedindex = selectedindex;
	// If we came here, we did something
	vclamp (KP(selectedindex), 0, rowcount - 1);
	if (rowcount == 0) return; // No listcount

	Lister_VisRangeCheckY_Refresh (k, /*pagesize*/ 0); // March 3 2026 - what does this do?
	Form_QueueRefresh (k->fctrl);
}

ktrl O_ListView_KeyDown (ktrl k, int key, int unicode)
{
	int rowcount = Lister_ListCountY (k);
	if (rowcount == 0) return NULL; // No listcount

	qbool is_paging = false; // Paging, we want the new index to be top or bottom.
	int pagesize = 0;
	int oldsel = KP(selectedindex);

	RELATED_ (DevListViewOnKeyPreview)
	if (KM_CTRL && key == 'c') {
		int firstrow = KP(selectedindex) - KP(selectedlength);
		int lastrow  = KP(selectedindex);
		if (firstrow > lastrow) swapi (&firstrow, &lastrow);

		qbool is_last_row_copy = false;
		if (k->columncount > 1 && firstrow == lastrow) {
			int numcolumns = k->columncount;
			int row = firstrow;
			int col = k->columncount - 1;
			int cell_idx = row * numcolumns + col;
			if (in_range_beyond(0, cell_idx, k->list_strings_a.numstrings)) {
				ccs *sxy = k->list_strings_a.strings[cell_idx];
				//int slen = istrlen(sxy);

				// Google AI says the odds of a normal string decompressing from base64 are low
				if (1) {
					va_copy_16384 (sxy2, sxy);
					String_Edit_Unquote(sxy2);
					//int slen = istrlen (sxy2);
					char *s_za = unbase64unzip_zalloc(sxy2);
					int slen2 = istrlen(s_za);
					if (slen2 > 0) {
						Clipboard_Set_Text (s_za);
						is_last_row_copy = true;
					}
					Mem_FreeNull_ (s_za);
				}
			}
		} //

		if (is_last_row_copy == false)
		stringlistclipboardset_columns_firstlastrow (&k->list_strings_a, k->columncount, firstrow, lastrow);

		Cbuf_AddTextLinef (cmd_local, "play %s", sv_sound_land.string);
		return k;
	}

	if (KM_CTRL && key == 'a') {
		KP(selectedindex) = rowcount - 1; // Bottom
		KP(selectedlength) = rowcount - 1; // Positive cursor length means start is further back
		goto ctrl_a_handled;
	}

	switch (key) {
	default:		return NULL; // NO ACTION
	case_break K_UPARROW:	KP(selectedindex) --;
	case_break K_DOWNARROW:	KP(selectedindex) ++;
	case_break K_MWHEELUP:	KP(selectedindex) -=1;
	case_break K_MWHEELDOWN:KP(selectedindex) +=1;
	case_break K_HOME:		KP(selectedindex) = 0;
	case_break K_PGUP:		KP(selectedindex) -= KP(viewcalc.pageamount); is_paging = true;
	case_break K_PGDN:		KP(selectedindex) += KP(viewcalc.pageamount); is_paging = true;
	case_break K_END:		KP(selectedindex) = rowcount - 1;
	} // sw

	// If we came here, we did something
	vclamp (KP(selectedindex), 0, rowcount - 1);

	if (is_paging) {
		pagesize = KP(selectedindex) - oldsel;
	}

	if ((KM_SHIFT || KM_CTRL_SHIFT)) {
		if (KP(selectedindex) != oldsel) {
			// Shift.
			int add_amount = KP(selectedindex) - oldsel; // 5 to 6 increases by 1, 6 to 5 decreases by 1
			KP(selectedlength) += add_amount;
		}
	} else {
		// No shift
		KP(selectedlength) = 0;
	}

ctrl_a_handled:

	if (KCLASS(k) != class_contextmenu) { // January 16 2025 - For now ...
		// Baker: This sets scrolling range r_scrollport.top and updates scrollbars
		// pagesize tries to move the viewport by that amount
		Lister_VisRangeCheckY_Refresh (k, pagesize); // March 3 2026 - what does this do?
	}

	if (oldsel != KP(selectedindex)) {
		Object_Event_OnChange_Recursive (k, shallrecurse_false);
	}
	return k;
}


// ka_lister.c.h

ktrl Lister_Generic_Refresh (ktrl k)
{
	switch (KCLASS(k)) {
	case class_contextmenu:	return O_ContextMenu_Refresh_Plus_Early (k);
	case class_imagegrid:	return O_ImageGrid_Refresh_Plus_Early (k);
	case class_listbox:		return O_ListBox_Refresh (k);
	case class_listview:	return O_ListView_Refresh (k);
	case class_tabselect:	return O_TabSelect_Refresh_Plus_Early (k);
	} // sw

	c_assert_msg_ (0, "Impossible3");
	return NULL;
}

CALLERS_ (many Lister_SetPct)
int Lister_ListCountY (ktrl k) // Row count
{
	switch (KCLASS(k)) {
	case class_contextmenu:	return floordiv (SLISTCOUNT(KP(list_strings_a)), CM_SERIES_COUNT_4);
	case class_imagegrid:	return KP(gridrows);
	case class_listbox:		return SLISTCOUNT(KP(list_strings_a));
	case class_listview:	if (!KP(columncount)) 
								return 0;
							return floordiv (SLISTCOUNT(KP(list_strings_a)), KP(columncount));
	case class_tabselect:	return SLISTCOUNT(KP(list_strings_a));
	} // sw

	c_assert_msg_ (0, "Impossible2");
	return not_found_neg1;
}

int Lister_ListCountX (ktrl k) // Col count
{
	switch (KCLASS(k)) {
	case class_imagegrid:	return KP(gridcolumns);
	} // sw

	c_assert_msg_ (0, "Impossible2");
	return not_found_neg1;
}

// Tries to ensure an item is fully visible.
CALLERS_ (Lister_KeyDown, Lister_MouseButtonAction)
void Lister_VisRangeCheckY_Refresh (ktrl k, int page_movecount)
{
	int rowcount = Lister_ListCountY(k);

	// Make sure selected idx is seen as a full row
	ViewCalc_Set_KY (k, KP(r_interior), KP(r_scrollport.top), rowcount);

	int viewport_move_units = 0;

	if (page_movecount) {
		// Try to have the adjustment be an entire page
		viewport_move_units = page_movecount;// * vc->rowheight;
	}

	else if (KP(selectedindex) < KP(viewcalc.first_full_row)) {
		viewport_move_units = KP(selectedindex) - KP(viewcalc.first_full_row);
	}
	else if (KP(selectedindex) > KP(viewcalc.last_full_row)) {
		viewport_move_units = KP(selectedindex) - KP(viewcalc.last_full_row);
	}

	if (viewport_move_units)
		SET___ KP(r_scrollport.top) += viewport_move_units * KP(viewcalc.rowheight); // scrolltop += 12 * 10, moving top y down on screen

	SET___ vclamp (KP(r_scrollport.top), 0, KP(viewcalc.viewportmax));

	if (KP(viewcalc.show_scrollbar)) {
		float pct = KP(r_scrollport.top) / (float)KP(viewcalc.viewportmax);
		KP(cm.in_recursion) ++;
			if (KP(servo.kscrolly))
				O_Scrollbar_SetPct (KP(servo.kscrolly), pct);
		KP(cm.in_recursion) --;
	}

	Lister_Generic_Refresh (k);
}

// Baker: selcol must be used, k->selectedindex cannot be used at all.
RELATED_ (ImageGrid)
void Lister_VisRangeCheck_GridX_Refresh (ktrl k, int page_movecount, int selcol)
{
	int colcount = Lister_ListCountX(k);

	// Make sure selected idx is seen as a full row
	ViewCalc_Set_KX (k, KP(r_interior), KP(r_scrollport.left), colcount);

	int viewport_move_units = 0;

	if (page_movecount) {
		// Try to have the adjustment be an entire page
		viewport_move_units = page_movecount;// * vc->rowheight;
	}

	else if (selcol /*k->selectedindex*/ < KP(viewcalcx.first_full_row)) {
		viewport_move_units = selcol /*k->selectedindex*/ - KP(viewcalcx.first_full_row);
	}
	else if (selcol /*k->selectedindex*/ > KP(viewcalcx.last_full_row)) {
		viewport_move_units = selcol /*k->selectedindex*/ - KP(viewcalcx.last_full_row);
	}

	if (viewport_move_units)
		SET___ KP(r_scrollport.left) += viewport_move_units * KP(viewcalcx.rowheight); // scrolltop += 12 * 10, moving top y down on screen

	SET___ vclamp (KP(r_scrollport.left), 0, KP(viewcalcx.viewportmax));

	if (KP(viewcalcx.show_scrollbar)) {
		float pct = KP(r_scrollport.left) / (float)KP(viewcalcx.viewportmax);
		KP(cm.in_recursion) ++;
			if (KP(servo.kscrollx))
				O_Scrollbar_SetPct (KP(servo.kscrollx), pct);
		KP(cm.in_recursion) --;
	}

	Lister_Generic_Refresh (k); // Baker: Should we do this here?
}

// Baker: selrow must be used, k->selectedindex cannot be used at all.
void Lister_VisRangeCheck_GridY_Refresh (ktrl k, int page_movecount, int selrow)
{
	int rowcount = Lister_ListCountY(k);

	// Make sure selected idx is seen as a full row
	ViewCalc_Set_KY (k, KP(r_interior), KP(r_scrollport.top), rowcount);

	int viewport_move_units = 0;

	if (page_movecount) {
		// Try to have the adjustment be an entire page
		viewport_move_units = page_movecount;// * vc->rowheight;
	}

	else if (selrow /*k->selectedindex*/  < KP(viewcalc.first_full_row)) {
		viewport_move_units = selrow /*k->selectedindex*/ - KP(viewcalc.first_full_row);
	}
	else if (selrow /*k->selectedindex*/ > KP(viewcalc.last_full_row)) {
		// Baker: Dec 28 2025 - This should try to do the least scroll possible unless paging is wanted.
		// The smallest scroll required to have the selected index in the viewport
		viewport_move_units = selrow /*k->selectedindex*/ - KP(viewcalc.last_full_row);
	}

	if (viewport_move_units)
		SET___ KP(r_scrollport.top) += viewport_move_units * KP(viewcalc.rowheight); // scrolltop += 12 * 10, moving top y down on screen

	SET___ vclamp (KP(r_scrollport.top), 0, KP(viewcalc.viewportmax));

	if (KP(viewcalc.show_scrollbar)) {
		float pct = KP(r_scrollport.top) / (float)KP(viewcalc.viewportmax);
		KP(cm.in_recursion) ++;
			if (KP(servo.kscrolly))
				O_Scrollbar_SetPct (KP(servo.kscrolly), pct);
		KP(cm.in_recursion) --;
	}

	Lister_Generic_Refresh (k); // Baker: Should we do this here?
}


CALLERS_ (O_ImageGrid_Refresh_Plus_Early)
ktrl Lister_FixedY_Refresh (ktrl k)
{
	int rowcount = Lister_ListCountY(k);

	if (isin2 (KCLASS(k), class_listview, class_imagegrid) == false)
		RECT_COPY (KP(r_interior), KP(r_screen)); // FOR NOW, no border consideration.

	if (isin1(KCLASS(k), class_imagegrid)) {
		// Baker: ImageGrid rowheight is specified.
		KP(viewcalc.rowheight) = KP(gridcellheight);
	}
	else if (isin1(KCLASS(k), class_listview)) {
		// Baker: ImageGrid rowheight is specified.
		KP(viewcalc.rowheight) = k->rowheight_rt;
	} else {
		// Baker: ListBox, ListView rowheight is size of font
		KP(viewcalc.rowheight) = KFONTSIZECEIL(k);
	}

	// With rowcount, rowheight established, continue ...
	ViewCalc_Set_KY (k, KP(r_interior), KP(r_scrollport.top), rowcount);

	// Determine scrollbar rect
	RELATED_ (O_ListBox_Spawn Object_Refresh_Recursive)

tabselect_comes_here_too_have_no_scroller:
	if_2 (KP(servo.kscrolly),  KP(is_noscrollbars)== false) {
		ktrl kscrolly = KP(servo.kscrolly);
		KXP(kscrolly, is_hidden) = (KP(viewcalc.show_scrollbar) == false);

		if (isin2(KCLASS(k), class_listview, class_imagegrid))
			KP(viewcalc.show_scrollbar) = rowcount > ifloor(KP(viewcalc.num_visiblef));

		if (KP(viewcalc.show_scrollbar)) {
			rect_s r_scroll;
			RECT_COPY (r_scroll, KP(r_interior));
			r_scroll.left	= RECT_RIGHTOF (KP(r_interior)) - KNOB_SIZE_16;
			r_scroll.width	= KNOB_SIZE_16;

			// PERFORM ADJUSTMENTS
			RECT_COPY (KXP(kscrolly, cm.relative_rect), r_scroll);

			KP(r_interior.width) -= KNOB_SIZE_16;
		}
	} // if scroll y

	// Baker: k->r_scrollport.height is the interior content width
	KP(r_scrollport.width)		= KP(r_interior.width);
	KP(r_scrollport.height)		= KP(viewcalc.rowheight) * rowcount;

first_row_calc:
	RECT_SET (
		KP(viewcalc.r_row_first),
		KP(r_interior.left) + KP(r_scrollport.left),
		KP(r_interior.top) - KP(r_scrollport.top) + KP(viewcalc.first_vis_row) * KP(viewcalc.rowheight),
		KP(r_interior.width),
		KP(viewcalc.rowheight)
	);

	//Vid_SetWindowTitlef ("scroll %d r_top " RECTI_4PRINTF, k->r_scrollport.top, RECT_SEND(k->viewcalc.r_row_first) );
	return k;
}

ktrl Lister_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
#if 0
	// Baker: March 5 2026 - Only scrollbars set press hook, so why was this here?
	if (is_down == false) {
		Form_PressHook_Clear (KP(fctrl), k);
		return NULL;
	}
#endif
	int newidx = ViewCalc_RowCollide (k, x, y);

	if (newidx == not_found_neg1)
		return k;

	if (newidx != KP(selectedindex)) {
		KP(selectedindex) = newidx;
		Lister_VisRangeCheckY_Refresh (k, /*movecount*/ 0);
		Object_Event_OnChange_Recursive (k, shallrecurse_false);  // Jan 17 2025 ListView Item Mouse Down
	}

	return k;
}

// This must return null if no action handled
RELATED_ (m_draw x = O_ListBox_KeyDown)
ktrl Lister_KeyDown (ktrl k, int key, int unicode)
{
	int listcount = Lister_ListCountY (k);
	if (listcount == 0) return NULL; // No listcount

	qbool is_paging = false; // Paging, we want the new index to be top or bottom.
	int oldsel = KP(selectedindex);

	switch (key) {
	default:		return NULL; // NO ACTION
	case_break K_UPARROW:	KP(selectedindex) --;
	case_break K_DOWNARROW:	KP(selectedindex) ++;
	case_break K_MWHEELUP:	KP(selectedindex) -=1;
	case_break K_MWHEELDOWN:KP(selectedindex) +=1;
	case_break K_HOME:		KP(selectedindex) = 0;
	case_break K_PGUP:		KP(selectedindex) -= KP(viewcalc.pageamount); is_paging = true;
	case_break K_PGDN:		KP(selectedindex) += KP(viewcalc.pageamount); is_paging = true;
	case_break K_END:		KP(selectedindex) = listcount - 1;
	} // sw

	// If we came here, we did something
	vclamp (KP(selectedindex), 0, listcount - 1);

	int pagesize = 0;
	if (is_paging) {
		pagesize = KP(selectedindex) - oldsel;
	}
	if (KCLASS(k) != class_contextmenu) // January 16 2025 - For now ...
		Lister_VisRangeCheckY_Refresh (k, pagesize);

	if (oldsel != KP(selectedindex)) {
		Object_Event_OnChange_Recursive (k, shallrecurse_false);
	}

	return k;
}

// Baker: ksb is shorthand for -- "k" scrollbar
// k here is typically a listview
void Lister_SetPctHorzListView (ktrl k, float pct, ktrl ksb)
{
	if (KP(cm.in_recursion))
		return;

	// The idea here is to set the viewtop.
	//int listcount = Lister_ListCountY(k);
	//int num_visible = (int)k->viewcalc.num_visiblef;
	//int not_seen_fully_rows = listcount - (int)k->viewcalc.num_visiblef; // 11 - 10 // 1
	//int num_top_positions = not_seen_fully_rows + 1; // 1 + 1 = 2
	//int this_top_row = Math_Round (pct * not_seen_fully_rows);
	int num_scrollbar_positions = KXP(ksb, sb.r_track.width) - KXP(ksb, sb.r_thumb.width);
	int num_viewport_positions = KP(xl.columnsallwidths) - KP(r_interior.width);
	if (num_scrollbar_positions > 0 && num_viewport_positions > 0) {
		//float pixels_per_position = (k->xl.columnsallwidths - k->r_interior.width) / (float)num_positions;
		KP(r_scrollport.left) = Math_Round (num_viewport_positions/*pixels_per_position*/ * pct);
		//Vid_SetWindowTitlef ("x scroll %d", k->r_scrollport.left);
//		int j = 5;
	} else {
		KP(r_scrollport.left) = 0;
		//c_assert_msg_ (0, "div 0 num_positions is 0");
	}

	Lister_Generic_Refresh (k);
}

CALLERS_ (Lister_SetPct exclusive) // imagegrid
void Lister_SetPct_Horz (ktrl k, float pct, ktrl ksb)
{
	// Baker: Dec 11 2025 - Appears the idea is to ..
	// * Set r_scrollport.left and do nothing else.
	int colcount = Lister_ListCountX(k);
	int not_seen_fully_rows = colcount - ifloor(KP(viewcalcx.num_visiblef)); // 11 - 10 // 1
	int this_left_row = Math_Round (pct * not_seen_fully_rows);
	KP(r_scrollport.left) = this_left_row * KP(viewcalcx.rowheight);

	Lister_Generic_Refresh (k);
}

void Lister_SetPct (ktrl k, float pct, ktrl ksb)
{
	if (KP(cm.in_recursion))
		return;

	if (KXP(ksb, is_horizontal)) {
		switch (KCLASS(k)) {
		default: // Unsupported
		case_break class_listview:	Lister_SetPctHorzListView (k, pct, ksb);
		case_break class_imagegrid: Lister_SetPct_Horz (k, pct, ksb);
		}
		return;
	}

	// The idea here is to set the viewtop.
	int listcount = Lister_ListCountY(k);
	//int num_visible = (int)k->viewcalc.num_visiblef;
	int not_seen_fully_rows = listcount - ifloor(KP(viewcalc.num_visiblef)); // 11 - 10 // 1
	//int num_top_positions = not_seen_fully_rows + 1; // 1 + 1 = 2
	int this_top_row = Math_Round (pct * not_seen_fully_rows);

	KP(r_scrollport.top) = this_top_row * KP(viewcalc.rowheight);
	//Vid_SetWindowTitlef ("y scroll %d", k->r_scrollport.top);
	//varpf (pct);
	//varpi (listcount);
	//varpi (num_visible);
	//varpi (not_seen_fully_rows);
	//varpi (num_top_positions);
	//varpi (this_top_row);
	//varpi (k->r_scrollport.top);

    //#pragma message ("Baker: We aren't setting scrollbar value here?  Are we called by scrollbar?")
	// Lister_Scrollbar_Property_Set_Float (k, prop_value, pct);

	Lister_Generic_Refresh (k);
}

// O_TabSelect does not come here, it does not spawn
// Baker: Nov 9 2025 - ListBox and ListView spawn on creation
// And their servants are made.  ListView gets a vertical
// line servant for column sizing.
RELATED_ (O_ListBox_Spawn Lister_Scrollbar_Property_Set_Float O_Scrollbar_SetPct)
ktrl Lister_Spawn (ktrl k)
{
	ktrl kscrollx;
	ktrl kscrolly /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_scrollbar, KPA(servo.kscrolly),
		KP(fctrl), SERVANT_KCAN_NULL, k);

	if (isin1(KCLASS(k), class_imagegrid)) {
		kscrollx = Object_Create_And_Assign_By_ClassEnum(class_scrollbar, KPA(servo.kscrollx), KP(fctrl), SERVANT_KCAN_NULL, k);
		KXP(kscrollx, is_horizontal) = true;
		// Baker: No kline - The column resize line
	}
	else if (isin1(KCLASS(k), class_listview)) {
		kscrollx = Object_Create_And_Assign_By_ClassEnum(class_scrollbar, KPA(servo.kscrollx), KP(fctrl), SERVANT_KCAN_NULL, k);

		KXP(kscrollx, is_horizontal) = true;

		// Has a true ZEBRA ("Z-Order top draw servant that draws topmost on form")
		//Dec 22 2024 unused ... Form_Get(f,k);

		ktrl kline =
			Object_Create_And_Assign_By_ClassEnum (
				class_rectangle,
				KPA(servo.kcolumnresize_line),
				/*form*/ KP(fctrl),
				/*container*/ SERVANT_KCAN_NULL,
				/*servant to*/ KP(fctrl)
			);
		RECT_SET (KLINEP(cm.relative_rect), 200, 200, 200, 200); // Baker: Dec 9 2025 - Just to give it something?
		RELATED_ (O_Rectangle_Refresh)
		KLINEP(is_hidden) = true;
		switch (KFP(appearance)) {
		default: VEC3_SET (KXP(kline, backcolor), 1.0, 0.5, 0.5);
		case_break appearance_flat_1:
			VEC3_COPY (KXP(kline,backcolor), KFP(scrollbarthumbcolor));
		} // sw appearance
		//printvarv (kline.backcolor);
	}

	return k;
}



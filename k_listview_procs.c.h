// k_listview_procs.c.h - sizer columndrag set pct

CALLERS_ (Form_MouseButtonAction, Lister_Spawn)
// If mouse down is set AFTER.
// If isdown == false, this is column drag end.
ktrl O_ListView_ColumnDragContinue (ktrl k, int x, int y, int is_done)
{
	// Do we get silly unrelated mouseups?  I bet we do
	float	delta_x		= x - KP(ui.mouse_down_origin.x);
	int		oldwidth	= KP(ui.mouse_down_valuei);
	int		newwidth	=  oldwidth + delta_x;
	int		col			= KP(ui.mouse_down_thingi) - LV_DRAG_COLSIZE_10;

	vclamp_lo (newwidth, LV_MIN_WIDTH_0);

	// We need the width it will go to.
	//stringlist_set_indexf (k.columnwidths_a, col, newwidth); //k.columnwidths_a.ints[col] = newwidth;
	//k->columnwidths_a.ints[col] = newwidth;
	INTS_SET_INDEX_VALUE(KP(columnwidths_a), col, newwidth);

	float adjusted = newwidth;
#ifdef QUAKEC
	if (1) {
#else // ENGINE ...
	if (k->fctrl->ui.csqc_handle_plus1 == false) {
#endif // QUAKEC vs. ENGINE
		adjusted = newwidth / autocvar_magnification;
	}
	INTS_SET_INDEX_VALUE(KP(_columnwidths_a_orig), col, adjusted);

	ktrl kline = KP(servo.kcolumnresize_line);

	int columnright = columnleftscreenx(k, col) + newwidth;
	RECT_SET (
		KLINEP(cm.relative_rect),				// Refresh sets r_screen to relative rect
		columnright /*x*/, // Needs locked to column position
		KP(r_interior.top),
		2,
		KP(r_interior.height)
	);
	
	//Vid_SetWindowTitlef ("Listview mouse up sizing delta %d from %d to %d",
	//	delta_x,
	//	oldwidth,
	//	newwidth
	//);
	//Con_PrintLinef ("Listview mouse up sizing delta %d from %d to %d",
	//	delta_x,
	//	oldwidth,
	//	newwidth
	//);
	if (is_done) {
		// O_ListView_ColumnDragEnd
		KP(ui.mousepointer) = mousepointer_arrow_default_0;
		KLINEP(is_hidden) = true;
	} else {
		KLINEP(is_hidden) = false;
	}

	// Q: Why are we using form refresh here?
	// A: We changed scrollbar visibility, must recalc drawn list.
	// Baker: The refreshes here seem both superfluous and correct.
	// Superfluous because I think the current design, any form refresh will refresh everything.
	// Correct because with a control specific refresh design, these will need to occur.
	// And I could be wrong about the superfluous.
	O_ListView_Refresh (k);
	O_ScrollBar_Refresh (KP(servo.kscrollx));
	O_ScrollBar_Refresh (KP(servo.kscrolly));
	O_Rectangle_Refresh (KP(servo.kcolumnresize_line));
	Form_QueueRefresh (KP(fctrl)); // This is the only form refresh available right now!

	return NULL;
}

CALLERS_ (Lister_Scrollbar_Property_Set_Float)
void O_ListView_SetPct (ktrl k, float pct, ktrl ksb)
{
	Lister_SetPct (k, pct, ksb);
}



// -1 if not found.  Otherwise the column being sized.
int Hit_Sizer_Return_Column (ktrl k, int x, int y)
{
	//Dec 22 2024 unused ...  viewcalc_s *vcx = &k->viewcalcx;
	rect_s r_cursor; RECT_COPY (r_cursor, KP(xl.r_header));
	r_cursor.left -= KP(r_scrollport.left);

	for (int col = 0 ; col < KP(columncount); col ++, r_cursor.left += r_cursor.width) {
		r_cursor.top = KP(xl.r_header.top); // Reset to top
		r_cursor.height = KP(xl.headerrowheight);
		r_cursor.width	= colwidthx (k,col);

		if (RECT_RIGHTOF(r_cursor) <= KP(xl.r_header.left))		continue;
		if (RECT_RIGHTOF(KP(xl.r_header)) <= r_cursor.left)		break; // I guess?

		// First 4 and last 4 of a column
		rect_s r_hot = r_cursor;
		r_hot.width = 4;
		if (RECT_HIT(r_hot,x,y))
			return col - 1;

		r_hot.left = RECT_RIGHTOF(r_cursor) - 4;

		if (RECT_HIT(r_hot,x,y))
			return col;

#if 1
		if (RECT_HIT(r_cursor,x,y)) {
			//zprf ("Column header hit %d", col);
			return (LV_SORT_PRESS_COLUMN_NEG2 - col);
		}
#endif
	} // for

	return not_found_neg1;
}


// ka_lister_procs.c.h

int rowcol_2idx(ktrl k, int row, int col)
{
	int idx = row * KP(columncount) + col;
	return idx;
}

void Label_Rect_Edit_Align (rect_s *r_text, const rect_s *r_area, alignment_e alignment)
{
	switch ((int)alignment / 3) {
	case 0:		r_text->top = r_area->top;
				break;

	case 1:		r_text->top = r_area->top + (r_area->height / 2.0 - r_text->height / 2.0);
				break;

	case 2:		r_text->top = RECT_BOTTOMOF (*r_area) - r_text->height; // right
				break;
	}

	switch ((int)alignment % 3) {
	case 0:		r_text->left = r_area->left;
				break;

	case 1:		r_text->left = r_area->left + (r_area->width / 2.0 - r_text->width / 2.0);
				break;

	case 2:		r_text->left = RECT_RIGHTOF (*r_area) - r_text->width;
				break;
	}
}

RELATED_ (O_ListView_Draw)

int colwidthx(ktrl k, int col)
{
	int is_entry = k->columnwidths_a.count > col;
	int w = is_entry ? k->columnwidths_a.ints[col] : LV_DEFAULT_WIDTH_100;
	vclamp_lo (w, LV_MIN_WIDTH_0);
	return w;
}

// Returns width of all columns.  Will include the phantom column if indicated
// phantom column is an extra column that displays if all column widths < size of listview control
int colwidths(ktrl k, int *pis_phantom, int *pphantom_width)
{
	int tot = 0;
	for (int col = 0; col < KP(columncount); col ++) {
		int w = colwidthx(k,col);
		tot += w;
	} //
	int is_phantom = tot < KP(r_interior.width);
	SET___ (*pphantom_width) = 0;
	if (is_phantom) {
		int phantom_amount =  KP(r_interior.width) - tot;
		tot += phantom_amount;
		SET___ (*pphantom_width) = phantom_amount;
	}
	SET___ (*pis_phantom) = is_phantom;
	return tot;
}

int columnleftscreenx(ktrl k, int colwanted)
{
	rect_s r_cursor; RECT_COPY(r_cursor, KP(xl.r_header));
	r_cursor.left -= KP(r_scrollport.left); // Scroll

	for (int col = 0; col < colwanted; col ++, r_cursor.left += r_cursor.width) {
		r_cursor.width	= colwidthx (k,col);
	}
	return r_cursor.left;
}

PSTRING colheaderx(ktrl k, int col)
{
	int headers_count = k->columnheaders_a.numstrings;
	if (col < headers_count) {
		PSTRING s_header = k->columnheaders_a.strings[col];
		return s_header;
	}
	return "";
}

PSTRING rowheadery(ktrl k, int row)
{
//	char *s_header = k->rowheaders_a.numstrings > row ? k->rowheaders_a.strings[row] : "";
//	return s_header;
	int headers_count = k->rowheaders_a.numstrings;
	if (in_range_beyond (0, row, headers_count)) {
		PSTRING s_header = k->rowheaders_a.strings[row];
		return s_header;
	}
	return "";
}

// Baker: Nov 10 2025 - viewcalc is row
int ViewCalc_RowCollide (ktrl k, int x, int y)
{
	rect_s r_cursor; RECT_COPY (r_cursor, KP(viewcalc.r_row_first));	// updated in refresh
	int beyond_idx	= KP(viewcalc.first_vis_row) + KP(viewcalc.rowcount);

	for (int row = KP(viewcalc.first_vis_row); row < beyond_idx; row ++, r_cursor.top += KP(viewcalc.rowheight)) {
		if (RECT_HIT_Y (r_cursor, y))
			return row;
	} // for
	return not_found_neg1;
}

// Imagegrid calls this
int ViewCalc_ColumnCollide (ktrl k, int x, int y)
{
	rect_s r_cursor;
	RECT_COPY (r_cursor, KP(viewcalcx.r_row_first));	// updated in refresh

	int beyond_idx	= KP(viewcalcx.first_vis_row) + KP(viewcalcx.rowcount);

	for (int col = KP(viewcalcx.first_vis_row); col < beyond_idx; col ++, r_cursor.left += KP(viewcalcx.rowheight)) {
		//if (RECT_HIT (r_cursor, x, y)) return n;
		// Baker: Y is outside our concern
		if (RECT_HIT_X (r_cursor, x))
			return col;
	} // for
	return not_found_neg1;
}

// Baker: Sets k.viewcalcx
void ViewCalcColumns_SetX (ktrl k, rect_s pr_interior)
{
	//Struct_VC_Memset0 (k.viewcalcx); // memset (vc, 0, sizeof(*vc));
//	size_t sz= sizeof(k->viewcalcx);
	memset (&k->viewcalcx, 0, sizeof(k->viewcalcx));

	if (KP(columncount) == 0)
		return; // return vc;

	// First visible column
	//int intox = k.r_scrollport.left;
	KP(xl.columnsallwidths) = colwidths(k, KPA(xl.is_phantom), KPA(xl.phantom_width));

	if (KP(xl.columnsallwidths) <= KP(r_interior.width)) {
		KP(viewcalcx.show_scrollbar) = false;
		KP(viewcalcx.firstcolumn) = 0;
		KP(viewcalcx.lastcolumn) = KP(columncount) - 1;
		KP(viewcalcx.leftdrawoffset) = 0;
		return; //return vc;
	}

	int cume = -KP(r_scrollport.left);
	KP(viewcalcx.firstcolumn) = not_found_neg1;
	KP(viewcalcx.lastcolumn) = not_found_neg1;
	//int first_col = not_found_neg1;
	int vleft = 0;//-k.r_scrollport.left;
	int vbeyond = vleft + KP(r_interior.width);
	int cleft;
	int cbeyond;
	int w;
	int col;
	for (col = 0; col < KP(columncount); col ++) { // NOTE < not <= no phantom here
		w = colwidthx(k, col);
		cleft = cume;
		cbeyond = cume + w;
		int is_seen_left = in_range_beyond (vleft, cleft, vbeyond);
		int is_seen_right = in_range_beyond (vleft, cbeyond, vbeyond);
		int is_seen = is_seen_left || is_seen_right;
		int is_last = is_seen && cbeyond >= vbeyond;
		if (KP(viewcalcx.firstcolumn) == not_found_neg1 && is_seen) {
			KP(viewcalcx.firstcolumn) = col;
			KP(viewcalcx.leftdrawoffset) =  - KP(r_scrollport.left);
		}
		if (KP(viewcalcx.lastcolumn) == not_found_neg1 && is_last) {
			KP(viewcalcx.lastcolumn) = col;
			break;
		}
		cume = cbeyond;
	} // for

	if (KP(viewcalcx.lastcolumn) == not_found_neg1)
		KP(viewcalcx.lastcolumn) = KP(columncount) - 1;
	KP(viewcalcx.pageamount) = 0;// ?

	KP(viewcalcx.show_scrollbar) = true;
}

// Set the r_scrollport.left amount of scroll
void ViewCalc_Set_KX (ktrl k, rect_s pr_interior, int scrolldown, int listcount)
{
	int rowheight_saved = KP(viewcalcx.rowheight);

	//Struct_VC_Memset0 (k.viewcalcx);
//	size_t sz= sizeof(k->viewcalcx);
	memset (&k->viewcalcx, 0, sizeof(k->viewcalcx));

	KP(viewcalcx.rowheight) = rowheight_saved;

	if (listcount == 0)
		return; // return vc;

	KP(viewcalcx.num_visiblef) = pr_interior.width / (float)KP(viewcalcx.rowheight);

	//#pragma message ("What if first and last row are cutoff?")
	KP(viewcalcx.num_visible_partials) = ceil(KP(viewcalcx.num_visiblef));

	// Validate all of these
	//vc.is_first_row_cutoff	= (scrolldown modulo vc.rowheight) != 0; // engine
	KP(viewcalcx.is_first_row_cutoff)	= floormod(scrolldown, KP(viewcalcx.rowheight)) != 0;
	KP(viewcalcx.first_vis_row)			= ifloor (scrolldown / KP(viewcalcx.rowheight));
	KP(viewcalcx.first_full_row)		= KP(viewcalcx.first_vis_row) + KP(viewcalcx.is_first_row_cutoff);

	// Debatable because not clamped to list.
	KP(viewcalcx.y_beyond)			= scrolldown + pr_interior.width;

	// Debatable ... if the adjusted last_partial_row becomes "culled", this is wrong
	//vc.is_cutoff_bottom	= (vc.y_beyond modulo vc.rowheight) != 0; // 59 + 1 = 60 mod 20 = 0 // engine
	KP(viewcalcx.is_cutoff_bottom)	= floormod(KP(viewcalcx.y_beyond), KP(viewcalcx.rowheight)) != 0; // 59 + 1 = 60 mod 20 = 0

	// These get adjusted in a moment ..
	KP(viewcalcx.last_full_row)		= ifloor ( (KP(viewcalcx.y_beyond) / KP(viewcalcx.rowheight)) - 1 ); // minus 1 because zero based
	KP(viewcalcx.last_partial_row)	= KP(viewcalcx.last_full_row) + KP(viewcalcx.is_cutoff_bottom);

	if (KP(viewcalcx.last_full_row) >= listcount - 1)	KP(viewcalcx.last_full_row) = listcount - 1; // CLAMP for integral
	if (KP(viewcalcx.last_partial_row) >= listcount - 1)	KP(viewcalcx.last_partial_row) = listcount - 1; // Just in case this can happen Nov 26 2018

	KP(viewcalcx.rowcount) = range_length(KP(viewcalcx.first_vis_row), KP(viewcalcx.last_partial_row));
	KP(viewcalcx.pageamount) = floor(KP(viewcalcx.num_visiblef)) - 1;
	vclamp_lo (KP(viewcalcx.pageamount), 1);

	// The max scroll allowed requires 1 item showing.
	if (listcount <= floor(KP(viewcalcx.num_visiblef)) )
		KP(viewcalcx.viewportmax) = 0; // no scroll is allowed because we see it all
	else {
		//(int)vc.num_visiblef
		// 200 items
		// 20.5 rows visible
		// 200 - 20 = 180
		//            181 199
		KP(viewcalcx.viewportmax) = floor ( (listcount - floor(KP(viewcalcx.num_visiblef)) ) * KP(viewcalcx.rowheight) );
	}

	KP(viewcalcx.show_scrollbar) = listcount > floor(KP(viewcalcx.num_visiblef));
}

// Set the r_scrollport.top amount of scroll
void ViewCalc_Set_KY (ktrl k, rect_s pr_interior, int scrolldown, int listcount)
{
	int rowheight_saved = KP(viewcalc.rowheight);

	//Struct_VC_Memset0 (k.viewcalc);
//	size_t sz= sizeof(k->viewcalc);
	memset (&k->viewcalc, 0, sizeof(k->viewcalc));

	KP(viewcalc.rowheight) = rowheight_saved;

	if (listcount == 0)
		return; // return vc;

	KP(viewcalc.num_visiblef) = pr_interior.height / (float)KP(viewcalc.rowheight);

	//#pragma message ("What if first and last row are cutoff?")
	KP(viewcalc.num_visible_partials) = ceil(KP(viewcalc.num_visiblef));

	// Validate all of these
	//vc.is_first_row_cutoff	= (scrolldown modulo vc.rowheight) != 0; // engine
	KP(viewcalc.is_first_row_cutoff)	= floormod(scrolldown, KP(viewcalc.rowheight)) != 0;
	KP(viewcalc.first_vis_row)			= ifloor (scrolldown / KP(viewcalc.rowheight));
	KP(viewcalc.first_full_row)			= KP(viewcalc.first_vis_row) + KP(viewcalc.is_first_row_cutoff);

	// Debatable because not clamped to list.
	KP(viewcalc.y_beyond)			= scrolldown + pr_interior.height;

	// Debatable ... if the adjusted last_partial_row becomes "culled", this is wrong
	//vc.is_cutoff_bottom	= (vc.y_beyond modulo vc.rowheight) != 0; // 59 + 1 = 60 mod 20 = 0 // engine
	KP(viewcalc.is_cutoff_bottom)	= floormod(KP(viewcalc.y_beyond), KP(viewcalc.rowheight)) != 0; // 59 + 1 = 60 mod 20 = 0

	// These get adjusted in a moment ..
	KP(viewcalc.last_full_row)		= ifloor ( (KP(viewcalc.y_beyond) / KP(viewcalc.rowheight)) - 1 ); // minus 1 because zero based
	KP(viewcalc.last_partial_row)	= KP(viewcalc.last_full_row) + KP(viewcalc.is_cutoff_bottom);

	if (KP(viewcalc.last_full_row) >= listcount - 1)	KP(viewcalc.last_full_row) = listcount - 1; // CLAMP for integral
	if (KP(viewcalc.last_partial_row) >= listcount - 1)	KP(viewcalc.last_partial_row) = listcount - 1; // Just in case this can happen Nov 26 2018

	KP(viewcalc.rowcount) = range_length(KP(viewcalc.first_vis_row), KP(viewcalc.last_partial_row));
	KP(viewcalc.pageamount) = floor(KP(viewcalc.num_visiblef)) - 1;
	vclamp_lo (KP(viewcalc.pageamount), 1);

	// The max scroll allowed requires 1 item showing.
	if (listcount <= floor(KP(viewcalc.num_visiblef)) )
		KP(viewcalc.viewportmax) = 0; // no scroll is allowed because we see it all
	else {
		//(int)vc.num_visiblef
		// 200 items
		// 20.5 rows visible
		// 200 - 20 = 180
		//            181 199
		KP(viewcalc.viewportmax) = floor ( (listcount - floor(KP(viewcalc.num_visiblef)) ) * KP(viewcalc.rowheight) );
	}

	if (KP(is_noscrollbars))
		KP(viewcalc.show_scrollbar) = false;
	else
		KP(viewcalc.show_scrollbar) = listcount > floor(KP(viewcalc.num_visiblef));
}

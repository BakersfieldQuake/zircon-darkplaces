// k_textbox_mouse.c.h

RELATED_ (Form_MouseButtonAction)

void TextBox_MouseSelection_Try_Extend (ktrl k, int x, int y)
{
	if (strlen(KP(text_a)) == 0)
		return;

	VarMyFont(myfont);
	int net_x		= x - (KP(txta.r_textarea.left) - KP(r_scrollport.left));
	vclamp_lo (net_x, LENGTH_0); // Draw_String acts weird with negative draw width
	int charnum	= Draw_StringIntoCountAt(myfont, KP(text_a), KP(fontsize), net_x, roundtype_yes_1);
	int newsellen	= charnum - KP(txta.cursor_pos);

	if (newsellen == KP(txta.cursor_length))
		return; // Same length

	TextBox_Cursor_Set_Refresh (k, KP(txta.cursor_pos), newsellen);
}

// Returns true if any range could be selected.
qbool TextBox_Get_Word_Range (ktrl k, int x, int y, int *pbefore, int *pafter)
{
	int slen = strlen(KP(text_a));
	if (slen == 0)
		return false; // No can do

	VarMyFont(myfont);
	int net_x		= x - (KP(txta.r_textarea.left) - KP(r_scrollport.left));
	vclamp_lo (net_x, LENGTH_0); // Draw_String acts weird with negative draw width
	int ch_index_hit	= Draw_StringIntoCountAt(myfont, KP(text_a), KP(fontsize), net_x, roundtype_none_0);
	//printvarf (ch_index_hit);
	int hitwhat = string_ascii_at(KP(text_a),ch_index_hit);
	if (hitwhat == CHAR_SPACE_32) {
		//zpr ("Hit a space");
		return false; // We hit a space.
	}

	// Flood left for space
	int startpos;
	for (startpos = ch_index_hit - 1; startpos >=0; startpos --) {
		int ch = string_ascii_at(KP(text_a),startpos);
		if (ch == CHAR_SPACE_32) {
			//zprf ("Hit space at %d", startpos);
			startpos = startpos + ONE_CHAR_1;
			break; // DONE
		}
	} // for
	if (startpos == -1)
		startpos = 0;
	
	// Flood right for space
	int endpos;
	for (endpos = ch_index_hit + 1; endpos < slen; endpos ++) {
		int ch = string_ascii_at(KP(text_a),endpos);
		if (ch == CHAR_SPACE_32) {
			endpos = endpos - ONE_CHAR_1;
			break; // DONE
		}
	} // for

	if (endpos >= slen)
		endpos = slen - ONE_CHAR_1;

	SET___ (*pbefore) = startpos;
	SET___ (*pafter) = endpos;
	
	return true;
}

void TextBox_MouseSelection_Try_Set (ktrl k, int x, int y)
{
	if (strlen(KP(text_a)) == 0)
		return;

	VarMyFont(myfont);

	int net_x		= x - (KP(txta.r_textarea.left) - KP(r_scrollport.left));
	vclamp_lo (net_x, LENGTH_0); // Draw_String acts weird with negative draw width
	int charnum	= Draw_StringIntoCountAt(myfont, KP(text_a), KP(fontsize), net_x, roundtype_yes_1);

	if (charnum == KP(txta.cursor_pos) && KP(txta.cursor_length) == 0) {
		return; // Same cursorpos and same no selection we would set.
	}

	TextBox_Cursor_Set_Refresh (k, charnum, LENGTH_0);
}

RELATED_ (O_ListBox_MouseButtonAction Form_Mouse_Move)
ktrl O_TextBox_MouseMove (ktrl k, int x, int y)
{
	// Baker: If we are down, don't test for rect collision in r_interior / r_textarea
	// to allow more leeway with mouse collision.
	//unused int isshifted = KM_SHIFT;

	//if (RECT_HIT(k->txta.r_textarea, x,y) == false) return k;

	KP(ui.mousepointer) = mousepointer_text_beam_1;

	int is_down = (KFP(frm.kmousedown) == k);

	if (is_down)
		TextBox_MouseSelection_Try_Extend (k, x, y);


	//int isshifted = KM_SHIFT;
	//if (is_down) {
	//	if (RECT_HIT(k->txta.r_textarea, x,y)) {
	//		int net_x = x - (k->txta.r_textarea.left - k->r_scrollport.left);
	//		if (shiftonly) {
	//		default:			Class_TextBox_MouseMove_Selection_Try_Extend_ (k, x, y);  // SHIFTED: Extend selection. No round
	//		case_break false:	Class_TextBox_MouseMove_Selection_Try_Cursor_Set (k, x, y);
	//	}
	//} // is_down
	return k;
}

ktrl O_TextBox_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	if (!is_down) {
		// UP
		int wasdown = (KFP(frm.kmousedown) == k);
		if (wasdown) {
			if (KFP(frm.kmousedown_click_count) >= 2) {
				//zpr ("Triple");
				TextBox_Cursor_Set_Refresh (k, 0, TXT_STRLEN(k));
			}
			else if (KFP(frm.kmousedown_click_count) == 1) {
				//zpr ("Double");
				int before;
				int after;
				if (TextBox_Get_Word_Range (k, x, y, SET___ ADDRESSOF(before), SET___ ADDRESSOF(after))) {
					int range = range_length(before,after);
					//printvarf (before);
					//printvarf (after);
					//printvarf (range);
					TextBox_Cursor_Set_Refresh (k, before, range);
				} else {
					// Hit a space or zero length string -- cannot select
				}
			}
			else
			TextBox_MouseSelection_Try_Extend (k, x, y);
		}
		return k;
	}

	int isshifted = KM_SHIFT;
	// r_textarea is too strict, use r_interior.
	if (RECT_HIT(KP(r_interior), x,y) == false) {
		//zpr ("Didn't hit interior");
		return k;
	}

	if (isshifted) {
		TextBox_MouseSelection_Try_Extend (k, x, y);
	} else {
		TextBox_MouseSelection_Try_Set (k, x, y);
	}	

	return k;
}



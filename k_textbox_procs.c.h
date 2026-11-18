// k_textbox_procs.c.h

int TextBox_Recalc_Translation_Did_Move (ktrl k);

void TextBox_Cursor_Recalc (ktrl k)
{
	VarMyFont(myfont);
	KP(txta._cursor_length_abs) = abs(KP(txta.cursor_length));

	// DON'T READ THIS, KNOW THAT IT IS RIGHT AND MOVE ALONG ...
	KP(txta._cursor_start)  = KP(txta.cursor_pos);
	KP(txta._cursor_beyond) = KP(txta.cursor_pos) + KP(txta.cursor_length);
	if (KP(txta._cursor_start) > KP(txta._cursor_beyond))
		swapi (KPA(txta._cursor_start), KPA(txta._cursor_beyond));

	// CALCULATE HIGHLIGHT TEXT
	//  TEXT BEFORE HIGHLIGHTED TEXT
	//  TEXT AFTER HIGHLIGHTED TEXT

	//PSTRING sleft = &k->text_a[k->txta._cursor_start];
	PSTRING sleft = string_mid(KP(text_a), KP(txta._cursor_start));
	
	int ncount = KP(txta._cursor_length_abs);
	// May 11 2026 - Maybe needs to be float? _sel_left_px
	// There is small cursor irregularity here.
	// May 12 2026 - All the math here for string lengths seems to check out, so it is the draw somehow.
	// Perhaps figure out a way to XOR draw?
	#pragma message ("ENGINE/QUAKEC CURSOR WEIRDNESS .. this might help this calculates the cursor position")
	// The flutter is very minor in QuakeC but still present.
	// Only occurs when text is selected.
	KP(txta._sel_left_px) = Draw_StringNWidth (myfont, KP(txta._cursor_start), KP(text_a), KP(fontsize));
	KP(txta._sel_len_px) = Draw_StringNWidth (myfont, ncount, sleft, KP(fontsize)); // WRONG
	KP(txta._sel_real_px) = KP(txta._sel_left_px);
	if (KP(txta.cursor_length) > 0) // positive means cursor at end
		KP(txta._sel_real_px) += (KP(txta._sel_len_px) - TXT_CURSOR_WIDTH_2);

	// RECALC VIEWPORTAGE
	TextBox_Recalc_Translation_Did_Move (k); // No action required it is just scroll?

	float thinline_cursorpos_px = KP(txta._sel_left_px);
	if (KP(txta.cursor_length) > 0)
		thinline_cursorpos_px += (KP(txta._sel_len_px) - TXT_CURSOR_WIDTH_2);

	RECT_SET (
		KP(txta.r_cursor),
		KP(txta.r_textarea.left) + thinline_cursorpos_px,
		KP(txta.r_textarea.top),
		TXT_CURSOR_WIDTH_2,
		KFONTSIZE(k)
	);

	RECT_SET (
		KP(txta.r_cursor_highlight),
		KP(txta.r_textarea.left) + KP(txta._sel_left_px),
		KP(txta.r_textarea.top),
		KP(txta._sel_len_px),
		KFONTSIZE(k)
	);

	Z_StrDup_Len_Z_Realloc (&k->txta.seg1_a, &k->text_a[0], k->txta._cursor_start);
	KP(txta.seg1_left_px) = KP(txta.r_textarea.left) + 0;

	Z_StrDup_Len_Z_Realloc (&k->txta.seg2_a, &k->text_a[k->txta._cursor_start], k->txta._cursor_length_abs);
	KP(txta.seg2_left_px) = KP(txta.r_textarea.left) + KP(txta._sel_left_px);

	Z_StrDup_Realloc (&k->txta.seg3_a, &k->text_a[k->txta._cursor_beyond]);
	KP(txta.seg3_left_px) = KP(txta.r_textarea.left) + KP(txta._sel_left_px) + KP(txta._sel_len_px);
}

int TextBox_Recalc_Translation_Did_Move (ktrl k)
{
	VarMyFont(myfont);
	int ret = false;
	KP(txta.stringwidthi_px) = Draw_StringWidthInt (myfont, KP(text_a), KP(fontsize));
	KP(r_scrollport.width)	= KP(txta.stringwidthi_px) + TXT_CURSOR_WIDTH_2;
	KP(txta.is_viewported_x) = KP(txta.r_textarea.width) < KP(r_scrollport.width);

	// If we do not have focus or are not viewported, there is no scroll
	if (KP(txta.is_viewported_x) == false || KIsFocused(k) == false) {
		KP(r_scrollport.left) = 0;
		return false;
	}

	int step_amount_0_25_w = ifloor(KP(txta.r_textarea.width) * TXT_SCROLL_HORZ_AMOUNT_0_25);

	// CONCERN:  Characters wider than the textarea.
	if (KP(txta._sel_real_px) - KP(r_scrollport.left) < 0) {
		// Too much scroll for cursor to be visible.  Avoid "leftspace"
		ret = true;
		do {
			KP(r_scrollport.left) -= step_amount_0_25_w; // REDUCE SCROLL step amt
			vclamp_lo (KP(r_scrollport.left), 0);
		} while (KP(txta._sel_real_px) - KP(r_scrollport.left) < 0);
	}

	else if (KP(txta._sel_real_px) - KP(r_scrollport.left) >= KP(txta.r_textarea.width)) {
		// INCREASE SCROLL.  Pull right into view.
		ret = true;
		do {
			KP(r_scrollport.left) += step_amount_0_25_w; // INCREASE SCROLL step amt
			if (KP(r_scrollport.left) >= KP(r_scrollport.width) - KP(txta.r_textarea.width)) {
				KP(r_scrollport.left) = KP(r_scrollport.width) - KP(txta.r_textarea.width); // Less headache inducing I guess. 0 - 199  100 to 199  200 - 100.  100 is ok max scroll
				break;
			}
		} while (KP(txta._sel_real_px) - KP(r_scrollport.left) >= KP(txta.r_textarea.width));
	}

	return ret;
}



void TextBox_Copy_Selection (ktrl k)
{
	PSTRING scopy_z = Z_StrDup_Len_Z(&k->text_a[TXT_SELLEFT(k)], TXT_SELCOUNTABS(k));
	Clipboard_Set_Text (scopy_z); // TextBox Copy Selection action
	ENGINE_ZFREE (scopy_z);
}

// Immediate cursor draw (force_draw_cursor) set in keypress, not here
void TextBox_Cursor_Set_Refresh (ktrl k, int start, int length)
{
	if (start == KP(txta.cursor_pos) && KP(txta.cursor_length) == length)
		goto must_refresh_exit; // Nothing changed.  We are required to refresh.

	KP(txta.cursor_pos)		= start;
	KP(txta.cursor_length)	= length;

//	if (false == in_range_beyond (0, k->txta.cursor_pos, (TXT_STRLEN(k) + 1) )) {
//		int j = 5;
//	}

//	if (false == in_range_beyond (0, TXT_CURSOR_AT(k), (TXT_STRLEN(k) + 1))) {
//		int j = 5;
//	}

must_refresh_exit:
	O_TextBox_Refresh (k);
}

void TextBox_SetTextMaster_Eventf (ktrl k, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	//strset (k.text_a,  s);
	//if (k.eve.OnChange) {
	//	RELATED_ (TxtServerFiler_OnChange)
	//	k.eve.OnChange(k);
	//}
	Z_StrDup_Realloc (&k->text_a, text);
	if (k->eve.onchange_a && k->eve.onchange_a[0]) {
		exofn_t onclick_fn = Event_Find_Function(k->eve.onchange_a);
		if (onclick_fn) {
			char *s_error_za = onclick_fn (k);
			if (s_error_za) {
				Con_PrintLinef ("ERROR: %s: " QUOTED_S, k->eve.onchange_a, s_error_za);
				Z_FreeNull_ (s_error_za);
			} // if error
		} else {
			// NOT found
			c_assert_msg_ (0, "Object_Event_OnChange not found!");
		}
	} // if text set

	VA_EXPAND_ALLOC_FREE (text);
}

//void TextBox_SetTextMaster_Event (ktrl k, string s)
//{
//	strset (k.text_a,  s);
//	if (k.eve.OnChange) {
//		RELATED_ (TxtServerFiler_OnChange)
//		k.eve.OnChange(k);
//	}
//}

// Used by keydown to emit characters.  How does this work with unicode? QC = The same
// A: Baker March 4 2026 - this WILL NOT work with unicode in Engine because of %c.
// Q: What calls this?  Answer: Any keypress.
void TextBox_Edit_Insert_Char_At (ktrl k, int ch_unicode_insert, int sel_left)
{
#if 1 // Engine -- %c is unicode poisonous.
	if (false == in_range_beyond(1, ch_unicode_insert, CHAR_TILDE_126))
		return;
#endif

	int slen = TXT_STRLEN(k);
//	int slen_after_cursor = slen - sel_left;
	PSTRING zbef = Z_StrDup_Len_Z (k->text_a, sel_left);
	PSTRING zaft = Z_StrDup_Len_Z (&k->text_a[sel_left], slen - sel_left);

	//Z_StrDupf_Realloc (&k->text_a, "%s%c%s",
	//	zbef,
	//	ch_unicode_insert,
	//	zaft
	//); 
	//TextBox_SetTextMaster_Event (k, strcat(zbef, chr2str(ch_unicode_insert), zaft)); // strset (k.text_a,  zbef, chr2str(unicode_insert), zaft);
	TextBox_SetTextMaster_Eventf (k, "%s%c%s", zbef, ch_unicode_insert, zaft);

	ENGINE_ZFREE (zbef);
	ENGINE_ZFREE (zaft);
}

qbool have_any_unicode (ccs *s)
{
	int slen = strlen(s);
	for (int n = 0; n < slen; n ++) {
		int ch = s[n];
		if (false == in_range (0, ch, CHAR_TILDE_126))
			return true;
	}
	return false;
}

// Returns new cursor
int TextBox_Edit_Insert_String_At (ktrl k, PSTRING s_insert, int sel_left)
{
#if 1 // Unicode blocker
	if (have_any_unicode(s_insert))
		return sel_left;
#endif

	int slen = TXT_STRLEN(k);
	PSTRING zbef = Z_StrDup_Len_Z (k->text_a, sel_left);
	PSTRING zaft = Z_StrDup_Len_Z (&k->text_a[sel_left], slen - sel_left);

	int newcursor = sel_left + istrlen(s_insert);
	//Z_StrDupf_Realloc (&k->text_a, "%s%s%s",
	//	zbef,
	//	s_insert,
	//	zaft
	//); TextBox_SetTextMaster_Event (k, strcat(zbef, s_insert, zaft));  // strset (k.text_a,  zbef, s_insert, zaft);
	TextBox_SetTextMaster_Eventf (k, "%s%s%s", zbef, s_insert, zaft);

	ENGINE_ZFREE (zbef);
	ENGINE_ZFREE (zaft);
	return newcursor;
}

void TextBox_Edit_Delete_At_NumChars (ktrl k, int sel_left, int sel_count)
{
	int slen = TXT_STRLEN(k);
	int sel_right = sel_left + sel_count;
	PSTRING zbef = Z_StrDup_Len_Z (k->text_a, LENGTH___ sel_left);
	PSTRING zaft = Z_StrDup_Len_Z (&k->text_a[sel_right], slen-sel_right);

	//Z_StrDupf_Realloc (&k->text_a, "%s%s",
	//	zbef,
	//	zaft
	//);
	TextBox_SetTextMaster_Eventf (k, "%s%s", zbef, zaft);

	ENGINE_ZFREE (zbef);
	ENGINE_ZFREE (zaft);
}

//int KS_Query_FindPos_Word_Prev_At (const ksubstr *ks, int sel_start)
// Baker: Totally not unicode friendly in engine? QC is fine, right?
int String_Find_Previous_Word_At (PSTRING s, int selstart)
{
	int chx;

	if (selstart == 0)
		return 0;

 	//int ch = s[selstart];
	int ch = string_charcode_at (s, selstart);

	// Bob Mary Paul
	int pos_space = 0;
	if (!isdigit(ch) || !isalpha(ch)) {
		pos_space = selstart;
		goto find_alpha;
	}

	// Find space
	pos_space = 0;
	for (int pos = selstart - 1; pos >= 0; pos --) {
		/*int*/ chx = string_charcode_at(s, pos); // s[pos];
		if (chx != CHAR_SPACE_32)
			continue;

		pos_space = pos;
		break;
	} // for

find_alpha:
	if (!pos_space)
		return 0;

	// Find alpha
	int pos_alpha = 0;
	for (int pos = pos_space - 1; pos >= 0; pos --) {
		/*int*/ chx = string_charcode_at(s, pos); // s[pos];
		if (!isdigit(chx) && !isalpha(chx))
			continue;

		pos_alpha = pos;
		break;
	} // for

	// Find space
	pos_space = 0;
	for (int pos = pos_alpha - 1; pos >= 0; pos --) {
		/*int*/ chx = string_charcode_at(s, pos); // s[pos];
		if (chx != CHAR_SPACE_32)
			continue;

		pos_space = pos;
		break;
	} // for

	if (!pos_space)
		return 0;

	return pos_space + ONE_CHAR_1; // WHY?  Because we don't want the space, we want the alpha
}

// In alpha -> find space, find alpha
// In space -> find alpha
// Bob Mary Todd
int String_Find_Next_Word_At (PSTRING s, int selstart)
{
	int chx;

	int slen = istrlen(s);
	if (selstart == slen)
		return slen;

	int ch = string_charcode_at(s, selstart); // s[selstart];

//	int newpos = not_found_neg1;

	// Bob Mary Paul
	int pos_space = slen;
	if (!isdigit(ch) && !isalpha(ch)) {
		// If not in alpha
		pos_space = selstart;
		goto find_alpha;
	}

	// Find space
	pos_space = slen;
	for (int pos = selstart + 1; pos < slen; pos ++) {
		/*int*/ chx = string_charcode_at (s, pos); // s[pos];
		if (chx != CHAR_SPACE_32)
			continue;

		pos_space = pos;
		break;
	} // for

find_alpha:
	if (pos_space == slen)
		return slen;

	// Find alpha
	int pos_alpha = slen;
	for (int pos = pos_space + 1; pos < slen; pos ++) {
		/*int*/ chx = string_charcode_at (s, pos); // s[pos];
		if (!isdigit(chx) && !isalpha(chx))
			continue;

		pos_alpha = pos;
		break;
	} // for

	if (pos_alpha == slen)
		return slen;

	return pos_alpha;
}

void TextBox_Delete_Selection_Refresh (ktrl k)
{
	if (KP(txta.cursor_length)) {
		TextBox_Edit_Delete_At_NumChars (k, TXT_SELLEFT(k), TXT_SELCOUNTABS(k));
		TextBox_Cursor_Set_Refresh (k, TXT_SELLEFT(k), LENGTH_0);
	}
}



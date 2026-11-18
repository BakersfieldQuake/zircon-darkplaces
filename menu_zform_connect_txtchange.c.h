// menu_zform_connect_txtchange.c.h


// onchange:"DevTabSelectOnChange"
RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)
CALLERS_ (TextBox_SetTextMaster_Eventf TextBox_Edit_Insert_Char_At TextBox_Edit_Insert_String_At)

CALLERS_ (DevInit -> DevTabSelectOnChange CALLS US DIRECTLY -> us)
EXO___ char *DevTxtFilterOnChange (ktrl k)
{
	// How does this work with TAB change?
	// A: We will find out.
	// We should use this to repopulate the REAL LIST from listaccum
	Form_Get (f,k);
	qbool bcan_be_text_filtered_have_data = (int)ezdev->f->userdata; // Different var name for search
	qbool already_filled_list_for_no_data = (int)ezdev->f->userdata2;

	if (bcan_be_text_filtered_have_data == false && already_filled_list_for_no_data) {
		return NULL; // Refuse to let anything happen after already populated list.
	}

	// NOT A NODATA DENY 2ND FILTER SCENARIO

	ezdev->f->userdata2 = true; // already_filled_list_for_no_data
	stringlistfreecontents (&ezdev->kList1->list_strings_a); //Object_Method_Clear
	//int oldlistindex = ezdev->kList1->selectedindex; // How does this help since we don't have an idx row?
	ezdev->kList1->selectedindex = 0;
	ezdev->kList1->selectedlength = 0;
	ezdev->kList1->sortcolumndescending = false;
	ezdev->kList1->sortcolumnindex = 0;
	ezdev->kList1->r_scrollport.left = ezdev->kList1->r_scrollport.top = 0;
	Form_QueueRefresh (f);

	// OK ...

	int numcolumns = ezdev->kList1->columncount;
	char *k_text_a_not_null = k->text_a ? k->text_a : "";
	char *txt_new_filter;
	va_super (txt_new_filter0, MAX_QPATHX2_256, "%s", k_text_a_not_null);
	txt_new_filter = txt_new_filter0;

	qbool is_quoted = String_Is_Quoted(txt_new_filter,strlen(txt_new_filter));
	if (is_quoted) {
		String_Edit_DeQuote(txt_new_filter);
	}

	// Treat leading quote
	if (txt_new_filter[0] == CHAR_DQUOTE_34) {
		va_super (txt_new_filter1, MAX_QPATHX2_256, "%s", &k->text_a[1]);
		txt_new_filter = txt_new_filter1;
		is_quoted = true;
	}

	va_super (txt_trimmed, MAX_QPATHX2_256, "%s", txt_new_filter);
	String_Edit_Trim(txt_trimmed); // Remove lead/trail whitespace.
	qbool is_multiword = false;
	if (false == is_quoted) {
		int numspaces = String_Find_Count_Whole_String(txt_trimmed, SPACE);
		if (numspaces > 0)
			is_multiword = true;
	}

	// ZERO LENGTH SCENARIO
	if (bcan_be_text_filtered_have_data == false || NULL == txt_new_filter || istrlen(txt_new_filter) == 0) {
		stringlistappendlist (&ezdev->kList1->list_strings_a, &ezdev->listaccum);
		goto finished_filling_list;
	}

	if (is_quoted == (int)true || is_multiword == (int)false) {
		// These should always match KList because it fills them.
		// We cannot "no data" fillups yet.
		if (false == is_quoted)
			txt_new_filter = txt_trimmed; // Use trimmed text unless quoted.

		int numrows = ezdev->listaccum.numstrings / numcolumns;
		for (int row = 0; row < numrows; row ++) {
			qbool shall_keep_row = false;
			for (int col = 0; col < numcolumns; col ++) {
				int sidx = row * numcolumns + col;
				ccs *sxy = ezdev->listaccum.strings[sidx];
				if (String_Contains_Caseless(sxy, txt_new_filter)) {
					shall_keep_row = true;
					break;
				}
			} // col

			if (false == shall_keep_row)
				continue;

			// Keep - copy all the row
			for (int col = 0; col < numcolumns; col ++) {
				int sidx = row * numcolumns + col;
				ccs *sxy = ezdev->listaccum.strings[sidx];
				stringlistappend (&ezdev->kList1->list_strings_a, sxy);
			} // col
		} // sidx

		goto finished_filling_list;
	} // is_quoted

	// We are multiword if we are here
	if (is_multiword) {
		// These should always match KList because it fills them.
		// We cannot "no data" fillups yet.
		//txt_new_filter = txt_trimmed; // Use trimmed text unless quoted.

		stringlist_t list_words = {0};
		stringlist_append_split (&list_words, txt_trimmed, SPACE);

		int numrows = ezdev->listaccum.numstrings / numcolumns;
		for (int row = 0; row < numrows; row ++) {
			qbool shall_discard_row = false;

			//int numwordmatch = 0;
			for (int wordnum = 0; wordnum < list_words.numstrings; wordnum ++) {
				ccs *sxy_word = list_words.strings[wordnum];
				qbool is_word_found = false;
				for (int col = 0; col < numcolumns; col ++) {
					int sidx = row * numcolumns + col;
					ccs *sxy = ezdev->listaccum.strings[sidx];

					if (String_Contains_Caseless(sxy, sxy_word)) {
						is_word_found = true;
						break;
					}
				} // col

				if (is_word_found == false) {
					shall_discard_row = true;
					break;
				}
			} // each word

			if (shall_discard_row)
				continue;

			// Keep - copy all the row
			for (int col = 0; col < numcolumns; col ++) {
				int sidx = row * numcolumns + col;
				ccs *sxy = ezdev->listaccum.strings[sidx];
				stringlistappend (&ezdev->kList1->list_strings_a, sxy);
			} // col
		} // sidx

		stringlistfreecontents (&list_words);
		goto finished_filling_list;
	}

	// UNREACHABLE

finished_filling_list:
    ; // oblig
	int numrowstotal = ezdev->listaccum.numstrings / numcolumns;
	int numrowsshowing = ezdev->kList1->list_strings_a.numstrings / numcolumns;

	if (false == bcan_be_text_filtered_have_data) {
		Object_Property_Set_Fmt	(ezdev->kLabelEntries, "Caption", "%d Lines", numrowsshowing);
	}
	else if (numrowstotal != numrowsshowing) {
		Object_Property_Set_Fmt	(ezdev->kLabelEntries, "Caption", "%d/%d Entries", numrowsshowing, numrowstotal);
	} else {
		Object_Property_Set_Fmt	(ezdev->kLabelEntries, "Caption", "%d Entries", numrowsshowing);
	}
	// Baker:
	// How sort happen?

#if 3926 // March 9 2026
	// DOES NOT WORK ...
	if (ezdev->try_keep_indexes) {
		//ezdev->kTabSelect1->selectedindex = g_old_tab_idx;
		int rowcount;
		rowcount = Lister_ListCountY(ezdev->kList1);
		if (in_range_beyond(0, g_old_listview1_idx, rowcount))
			ezdev->kList1->selectedindex = g_old_listview1_idx;
		else
			ezdev->kList1->selectedindex = 0;

		rowcount = Lister_ListCountY(ezdev->kList2);
		if (in_range_beyond(0, g_old_listview2_idx, rowcount))
			ezdev->kList2->selectedindex = g_old_listview2_idx;
		else
			ezdev->kList2->selectedindex = 0;

		rowcount = Lister_ListCountY(ezdev->kList3);
		if (in_range_beyond(0, g_old_listview3_idx, rowcount))
			ezdev->kList3->selectedindex = g_old_listview3_idx;
		else
			ezdev->kList3->selectedindex = 0;
	}
#endif
	DevListViewOnChange (ezdev->kList1); // Feb 4 2026 - Can we do this?  This is data only, no metrics in the func Feb 10 2026
//#if 3926 // March 9 2026
//	if (ezdev->try_keep_indexes) {
//		ezdev->kTabSelect1->selectedindex = g_old_tab_idx;
//		int rowcount;
//		rowcount = Lister_ListCountY(ezdev->kList1);
//		if (in_range_beyond(0, g_old_listview1_idx, rowcount))
//			ezdev->kList1->selectedindex = g_old_listview1_idx;
//		else
//			ezdev->kList1->selectedindex = 0;
//
//		rowcount = Lister_ListCountY(ezdev->kList2);
//		if (in_range_beyond(0, g_old_listview2_idx, rowcount))
//			ezdev->kList2->selectedindex = g_old_listview2_idx;
//		else
//			ezdev->kList2->selectedindex = 0;
//
//		rowcount = Lister_ListCountY(ezdev->kList3);
//		if (in_range_beyond(0, g_old_listview3_idx, rowcount))
//			ezdev->kList3->selectedindex = g_old_listview3_idx;
//		else
//			ezdev->kList3->selectedindex = 0;
//	}
//#endif

	// We expect this
	// 0. CLEAR == cursor length to 0, selectedindex stays AS IS?
	// 1. Clear LIST1 LIST2 LIST3
	// 2. Refill LIST1
	// 2B. Run the sort if any
	// 2C   Fire the LIST2/LIST3 thing.
	return NULL;
}

RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)

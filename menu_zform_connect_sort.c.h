// menu_zform_connect_sort.c.h

// This will rely on -- is sorted
// And if it is, sets a "sort index" where 0 is no sort and 1 is column #0
// We will need an ascending descending.
// Add a filter word( and filter column).
// onload:DevInit
		//if (k.sortcolumnindex == column)
		//	k.sortcolumndescending = !k.sortcolumndescending; // If already selected toggle ascend/descend
		//else
		//	k.sortcolumnindex = column;

// Don't come here if k->is_sorted == false or no rows (or no columns).
RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)
EXO___ char *DevListViewSort (ktrl k) // returns an error string or NULL
{
	// 1. We create a long string version with the sort key leading. (TAB delimited?  Or some weird character?)
	// 2. All the columns cat "as-is"
	// 3. We sort
	// 4. Then we decode
	// A. Do we try to find selected index? postcat?
	// B. Do we try to make sure that if it is in the new thing that it is visible?
	// Determine if sort column is numeric.
	if (k->is_sorted == false) {
		return Z_StrDup("NOTSORTED");
	}

	if (in_range_beyond(0, k->sortcolumnindex, k->columncount) == false) {
		return Z_StrDupf("k->sortcolumnindex out of range %d", k->sortcolumnindex);
	}



	int numrows = Lister_ListCountY(k);
	int numcols = k->columncount;
#if 0
	int listcount = k->list_strings_a.numstrings;
	int selidx = k->selectedindex;
#endif
	int sortindexbase0 = k->sortcolumnindex; //UNPLUS1(k->sortcolumnindex);
	#if 0
	qbool is_numeric = false;
	#endif // 0
	int num_numeric = 0;
	int num_nonnumeric = 0;
	int num_floatingpoint = 0;
	int num_ignored = 0;
	int num_with_tab = 0;
	int row_with_tab = not_found_neg1;
	int col_with_tab = not_found_neg1;
	int maxchars = 0;
#if 0
	qbool is_floating = false;
#endif // 0

	// Determine if sort index is numeric or floating point for formatting purposes
	for (int row = 0; row < numrows; row ++) {
		int	listidx		= rowcol_2idx(k, row, /*column =*/ sortindexbase0);
		char	*s			= k->list_strings_a.numstrings > listidx ? k->list_strings_a.strings[listidx] : NULL;
		if (!s || !s[0]) {
			// No string for column - this is ok.  Rules are fast and loose.
			num_ignored ++;
			num_nonnumeric ++;
			continue;
		}

		int ch = s[0];
		if (isdigit(ch)) {
			num_numeric ++;
			if (String_Contains(s, ".")) {
				num_floatingpoint ++;
			}
		} else {
			num_nonnumeric ++;
		}
		if (String_Contains (s, TAB)) {
			row_with_tab = row;
			col_with_tab = row;
			num_with_tab ++;
		}
		maxchars = strlen(s);
	} // End row ... numeric check of sortcolumnindex

	if (num_with_tab) {
		Con_PrintLinef (CON_ERROR "The data has tabs (instances = %d) in it in at row %d col %d",
			num_with_tab,
			row_with_tab,
			col_with_tab
		);
		return Z_StrDup("TAB_IN_DATA");
	} // num_with_tab

	// Now create the super catter
	#define S_MARKER_X "x"
	stringlist_t list = {0};
	for (int row = 0; row < numrows; row ++) {
		baker_string_s *bsa = BakerString_Create_Malloc ("");
		int	listidx;
		listidx		= rowcol_2idx(k, row, /*column =*/ sortindexbase0);

		char	*s			= k->list_strings_a.numstrings > listidx ? k->list_strings_a.strings[listidx] : "";

		if (num_nonnumeric) {
			// Any non-numeric == alpha treatment
			BakerString_CatCFmt (bsa, "%s", s);
		} else if (num_floatingpoint) {
			// Any floating point = all floating point
			float fval = atoi(s);
			BakerString_CatCFmt (bsa, "%09.2f", fval);
		} else {
			int ival = atoi(s);
			//uint64_t uval64 = STRTOI64_FN (valuebuf, /*end pointer*/ NULL, /*base decimal*/ 10);
			BakerString_CatCFmt (bsa, "%09d", ival);
		}

		// Now add the columns
		for (int col = 0; col < numcols; col ++) {
			int	cidx = rowcol_2idx(k, row, col);
			char	*s2	= k->list_strings_a.numstrings > cidx ? k->list_strings_a.strings[cidx] : "";
			BakerString_CatCFmt (bsa, TAB "%s", s2);
		} // col

		// Add 1 extra column indicating it is the currently selected index.
		ccs *strail = row == k->selectedindex ? S_MARKER_X : "";
		BakerString_CatCFmt (bsa, TAB "%s", strail);

		stringlistappend (&list, bsa->string);

		BakerString_Destroy_And_Null_It (&bsa);
	} // End row ... numeric check of sortcolumnindex

	//stringlistclipboardset (&list);

	if (k->sortcolumndescending)
		stringlistsortreverse (&list, fs_make_unique_false);
	else
		stringlistsort_no_unique (&list);

	//stringlistclipboardset (&list);

	// End of list1 create and sort

	// Now clear the list and resplit and find the new selected index.  Yay!
	int newsel = not_found_neg1;
	// 1. Destroy the master.
	stringlistfreecontents (&k->list_strings_a);

	for (int sortrow = 0; sortrow < list.numstrings; sortrow ++) {
		ccs *srow = list.strings[sortrow];
		stringlist_t listrow = {0};
		stringlist_append_split (&listrow, srow, TAB);
		// We expect 2 extra rows.
		if (listrow.numstrings != k->columncount + 2) {
			Con_PrintLinef (CON_ERROR "split column count is %d, expected %d (%d + 2)", listrow.numstrings, k->columncount + 2, k->columncount);
		}
		ccs *s_star = listrow.strings[k->columncount + 1];
		if (String_Match (s_star, S_MARKER_X)) {
			//Con_PrintLinef ("New selected index is %d", sortrow);
			newsel = sortrow;
		}

		for (int col = 0; col < numcols; col ++) {
			//int	listidx2 = sortrow * (k->columncount + 2 /*precolumn/postcolumn*/) + col + 1 /*after sort key*/ ;
			int	sortcolumnidx = col + 1;

			ccs *s_col = listrow.strings[sortcolumnidx]; // First col is sort key
			stringlistappend (&k->list_strings_a, s_col);
		} // col

		stringlistfreecontents (&listrow);
	} // sortrow

	// 2.
	// END
	stringlistfreecontents (&list);

	return NULL;
}
RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)


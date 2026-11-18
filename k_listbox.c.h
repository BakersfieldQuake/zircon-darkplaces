// k_listbox.c.h

// It is ok for the selected idx to fall out of range if the scrollbar did it.

//
// 0  =====   -8  BOB     8 =======
//    BOB		  ======    MARY
//    MARY        MARY      TODD

RELATED_ (O_ListView_Finalize)

ktrl O_ListBox_Spawn (ktrl k)
{
	return Lister_Spawn(k); // Y scrollbar materializes
}

// k.r_screen has been updated and all draw properties known.
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF)

ktrl O_ListBox_Refresh (ktrl k)
{
	return Lister_FixedY_Refresh (k);
}

void O_ListBox_Draw (ktrl k)
{
	VarMyFont(myfont); // This is a setfont.
	Draw_Rect	(KPA(r_interior), KP(backcolor), alpha_1_0);

	rect_s r_cursor;
	RECT_COPY (r_cursor, KP(viewcalc.r_row_first));	// updated in refresh
	int start_idx	= KP(viewcalc.first_vis_row);
	int count		= KP(viewcalc.rowcount);
	int beyond_idx	= start_idx + count;

	for (int n = start_idx; n < beyond_idx; n ++, r_cursor.top += KP(viewcalc.rowheight)) {
		PSTRING s = SLISTGETINDEX (KP(list_strings_a), n);

		int	is_selected	= (n == KP(selectedindex));
		PCOLOR	forecolor0	= is_selected ? KP(forecolorselected) : KP(forecolor);

		if (is_selected) {
			qbool is_focused = KIsFocused (k);
			Draw_Rect	(ADDRESSOF(r_cursor), is_focused ? KP(backcolorfocusselected) : KP(backcolorselected), alpha_1_0);
		}
		Draw_String	(ADDRESSOF(r_cursor), s, KP(fontsize), forecolor0, alpha_1_0);
	} // for

//	return k;
}

// Coordinates are in vid_conwidth scale (screen for menu canvas)
ktrl O_ListBox_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	return Lister_MouseButtonAction (k, x, y, is_down);
}

// Return NULL if input not processed.
ktrl O_ListBox_KeyDown (ktrl k, int key, int unicode)
{
	return Lister_KeyDown(k, key, unicode);
}

// O_ListBox_ListFromString (e, "Mary,Had,A,Little,LambMary,Had,A,Little,LambMary,Had,A,Little,LambMary,Had,A,Little,LambMary,Had,A,Little,Lamb")
void O_ListBox_ListFromString (ktrl k, PSTRING s)
{
	SLISTRESET (KPA(list_strings_a));
	stringlist_append_split (KPA(list_strings_a), s, COMMA_DELIM);
	//int newcount = SLISTCOUNT (KP(list_strings_a));
	//printvarf (k.list_strings_a);
	//printvarf (newcount);
}

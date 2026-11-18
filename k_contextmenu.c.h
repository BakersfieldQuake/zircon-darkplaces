// k_contextmenu.c.h

// Baker: Jan 29 2025 - Requires a double click to fire one of the event items, otherwise just highlights

// Context menu / 3  (String, StringValue, Function)

RELATED_ (Form_ContextMenu_Spawn Form_Mouse2Down O_ContextMenu_Leaving)
ktrl O_Context_Menu_Exec(ktrl k)
{
	int listidx = k->selectedindex * CM_SERIES_COUNT_4; // CONTEXTOID

	//char *s_show	= k->list_strings_a.strings[listidx + CM_0_TEXT_0];
	//char *s_checked	= k->list_strings_a.strings[listidx + CM_1_CHECKED_1];
	char *s_str		= k->list_strings_a.strings[listidx + CM_2_STRING_ID_2];
	char *s_fn		= k->list_strings_a.strings[listidx + CM_3_FUNCSTRING_3];

	RELATED_ (Form_ContextMenu_Spawn)

	O_ContextMenu_Leaving (k, s_str); // Inside O_Context_Menu_Exec

	RELATED_ (Form_ContextMenu_Spawn)
	//"Form State To Clipboard,0,Print,ObjectCommand,"
	//" ,0,Option2String,Option2FunctionString,"
	//" ,0,Option3String,Option3FunctionString,"
	//"-,0,DASH,DASH,"
	//"Edit Mode,0,EditModeString,EditModeToggle",

	// Baker: March 6 2026 - Empty string means no C function name.
	if (strlen(s_fn)) {
		zexeccmd (k->fctrl, k, listidx, s_fn, s_str);
	}

	return k;
}




RELATED_ (Lister_ListCountY)

ktrl O_ContextMenu_Spawn (ktrl k) 
{
	Form_Get (f,k);
	ToolTips_End_Any		(f, "Deploying menu fly"); // Make sure no hover
	return Lister_Spawn(k); 
}

RELATED_ (Form_ContextMenu_Spawn Form_Mouse2Down)
void O_ContextMenu_Draw (ktrl k)
{
	VarMyFont(myfont);
	Draw_Rect	(&k->r_interior, k->backcolor, alpha_1_0);

	rect_s	r_button	= k->tb.r_button;
	rect_s	r_text		= k->txta.r_textarea;

	Draw_Rect	(&k->tb.r_leftbar, color3_gray_75, alpha_0_5);

	int cap = (k->list_strings_a.numstrings / 4) * 4; // Avoid out of bounds if list screwed.
	for (int nn = 0; nn < cap; nn += CM_SERIES_COUNT_4, r_button.top += k->tb.buttonstep, r_text.top += k->tb.buttonstep) {
		const char *s = k->list_strings_a.strings[nn];
		const char *sc = k->list_strings_a.strings[nn + CM_1_CHECKED_1];
		int ischecked = sc[0] == '1';
		int		trueidx			= nn / CM_SERIES_COUNT_4;
		int		is_selected		= (trueidx == k->selectedindex);
		vec_t	*backcolor		= is_selected ? k->backcolorselected : k->backcolor;
		vec_t	*forecolor		= is_selected ? k->forecolorselected : k->forecolor;

		if (k->is_rounded)	Draw_RectRound	(&r_button, backcolor, alpha_1_0);
		else				Draw_Rect		(&r_button, backcolor, alpha_1_0);

		if (ischecked) {
			rect_s r = { k->tb.r_leftbar.left, r_button.top, k->tb.r_leftbar.width, r_button.height };
			Draw_Rect		(&r, color3_blue, alpha_1_0);
		}
		Draw_String	(&r_text, s, k->fontsize, forecolor, alpha_1_0);
	} // for


//	return k;
}




// Baker: The bounds check here is more complex.
// Devinfo uses full canvas (vid.width)
// CSQC might use full canvas or might not ui_full_canvas.integer
// I think vid_conwidth is safe here.

ktrl O_ContextMenu_Activate_At (ktrl k, int x, int y, ccs *reason)
{
	Form_Get(f,k);
	ktrl kmnu = k;

	int max_x = vid.width;
	int max_y = vid.height;

	// If context menu opened in top half, drop it down.
	// Otherwise drop it up.
	O_ContextMenu_Refresh_Plus_Early (kmnu);

	int tophalf_click = y < vid.height / 2;
	kmnu->cm.relative_rect.left = x;
	kmnu->cm.relative_rect.top = tophalf_click ? y : y - kmnu->cm.relative_rect.height;

	/// Baker: Jan 19 2025 - Context Menu doesn't know its width yet, make it recalc

	if (RECT_RIGHTOF(kmnu->cm.relative_rect) > max_x /* vid.width*/)
		kmnu->cm.relative_rect.left = max_x /*vid.width*/ - kmnu->cm.relative_rect.width;
	else if (kmnu->cm.relative_rect.left < 0)
		kmnu->cm.relative_rect.left = 0;
	if (RECT_BOTTOMOF(kmnu->cm.relative_rect) > max_y /*vid.height*/)
		kmnu->cm.relative_rect.top = max_y/* vid.height*/ - kmnu->cm.relative_rect.height;
	else if (kmnu->cm.relative_rect.top < 0)
		kmnu->cm.relative_rect.top = 0;

	ktrl kold = f->frm.kfocused;

	Oject_Focus_Set (kmnu);

	Oject_Hidden_Set (kmnu, false /*!kmnu->is_hidden*/); // CONTEXTOID SHOW
	f->frm.kpopup = kmnu; // O_ContextMenu_Leaving (START!)

	if (kold && kold->po->enum_id != class_contextmenu) {
		f->frm.kcontextoldfocus = kold; //f->frm.kfocused; // CONTEXTOID Store old focus (SHOW PT 2)
	}

	O_CheckBox_Refresh (kmnu); // Jan 29 2025 -- EMPTY all commented out!
	Form_QueueRefresh (f); // This is the only form refresh available right now!
	return k;
}

CALLERS_ (Form_Mouse2Down Form_CSQC_Mouse2Down /*with shift activates tradition in modelview*/ )
ktrl Form_ContextMenu_EditModeActivate (ktrl f, ccs *reason)
{
	int x = in_windowmouse_x, y = in_windowmouse_y;
	return O_ContextMenu_Activate_At (f->servo.kcontextmenu, x, y, reason);
}

RELATED_ (CSQC_Object_Method EditModeActivate)
// Uses mouse origin.  Q: What would keyboard do?  Different method maybe?
ktrl O_ContextMenu_Popup_Method (ktrl k, ccs *reason)
{
	Form_Get(f,k); // IT IS CORRECT
	int x = in_windowmouse_x, y = in_windowmouse_y;
	O_ContextMenu_Activate_At (k, x, y, reason);
	return k;
}

RELATED_ (O_TabSelect_Refresh_Plus_Early Form_ContextMenu_Spawn)
ktrl O_ContextMenu_Refresh_Plus_Early (ktrl k)
{
	VarMyFont(myfont);
	k->tb.stringwidthmaxpx	= stringlist_maxwidthint(&k->list_strings_a, myfont, k->fontsize);

	// Assume infinite size on finite canvas
	float	descender_pct			= myfont->ft_baker_descend_pct;
	float	descender_reduce_pct	= 1 - bound(0,k->fontdescendpct,1);
	int		descender_reduce_px		= ceil(descender_pct * descender_reduce_pct * k->fontsize);

	k->tb.cellpaddingx_i	= ceil(k->cellpaddingxpct * k->fontsize);
	k->tb.cellpaddingy_i	= ceil(k->cellpaddingypct * k->fontsize);
	k->tb.cellspacing_i		= ceil(k->cellspacingypct * k->fontsize);

	int left_extra = ceil(k->fontsize);

	k->tb.r_button.left		= k->cm.relative_rect.left;
	k->tb.r_button.top		= k->cm.relative_rect.top;
	k->tb.r_button.width	= (k->tb.cellpaddingx_i * 2) + left_extra + k->tb.stringwidthmaxpx;

	if (k->cm.relative_rect.width > k->tb.r_button.width) {
		k->tb.r_button.width = k->cm.relative_rect.width; // Are you sure?
	}

	k->tb.r_button.height	= (k->tb.cellpaddingy_i * 2) + ceil(k->fontsize) - descender_reduce_px;
	k->tb.buttonstep		= k->tb.r_button.height + k->tb.cellspacing_i;

	rect_s r_box = k->tb.r_button;

	// Compute bottom - due to cellspacing stepping is clearest

	int numrows = Lister_ListCountY(k);
	for (int nn = 1; nn < numrows; nn ++, r_box.top += k->tb.buttonstep) {
		// Deliberately empty to calc r_box.top
	}

	k->tb.r_leftbar = k->tb.r_button;
	k->tb.r_leftbar.width = left_extra;
	k->tb.r_leftbar.height = RECT_BOTTOMOF(r_box) - k->tb.r_button.top;

	RECT_SET (k->txta.r_textarea,
		k->cm.relative_rect.left + left_extra + k->tb.cellpaddingx_i,
		k->cm.relative_rect.top + k->tb.cellpaddingy_i,
		k->tb.stringwidthmaxpx,
		ceil(k->fontsize)
	);

autosize_go:
	k->cm.relative_rect.width = r_box.width;
	k->cm.relative_rect.height = RECT_BOTTOMOF(r_box) - k->cm.relative_rect.top;
	return k;
}

static double _lastime;
ktrl O_ContextMenu_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	if (is_down == false)
		return NULL;

	double clickdelta = Host_RealTime() - _lastime;

	int is_doubleclick =  (_lastime && clickdelta < DOUBLE_CLICK_0_5);

	_lastime = host.realtime;

	int newidx = Rect_Hit_Boxes_No_Miss (&k->tb.r_button, x, y, k->tb.buttonstep, k->list_strings_a.numstrings);

	if (newidx == not_found_neg1)
		return k;

	if (newidx != k->selectedindex) {
		k->selectedindex = newidx; 
		Object_Event_OnChange_Recursive (k, shallrecurse_false);
	}

	if (is_doubleclick) {
		ktrl k_gets_focus = k->fctrl->frm.kcontextoldfocus;// kcontextoldfocus gets cleared
		// Baker: Feb 5 2025 - Edit mode plus no control with focus is "at your own risk for now".
		// In fact, edit mode is at your own risk because it zorders and with containers it can
		// be very bad.
		if (k->selectedindex != not_found_neg1)
			O_Context_Menu_Exec (k);

		if (k_gets_focus) {
			Oject_Focus_Set (k_gets_focus);
		} else {
			// Baker: March 6 2026 - This should never happen yet it does ...
			// The prevent crash ...
			// Keep this.  Not sure how it happens, but my laptop trackpad has ambiguous mouse clicking
			// Depending on the % of the pad, it might be left or right and sometimes you never know
			// and that might be causing this?  Anyway ... keep
			Oject_Focus_Set (k->fctrl);
		}
	}

	return k;

}

CALLERS_ (Oject_Focus_Set O_Context_Menu_Exec)

// Focus set might as well be cancel.
ktrl O_ContextMenu_Leaving (ktrl k, ccs *svalue)
{
	Form_Get(f,k);
	// Reply will be dialog name and "DialogCancelled" with string of NULL
	f->frm.kpopup = NULL;
	k->fctrl->frm.kcontextoldfocus = NULL;
	if (k->fctrl->ui.csqc_handle_plus1) {
		if (svalue) {
			// Reply will be dialog name event "DialogCancelled" with value of svalue
			Form_CSQC_Event_EZ (k, "PopupClosed", 0, svalue);
		} else {
			// Reply will be dialog name event "DialogCancelled" with value of svalue
			Form_CSQC_Event_EZ (k, "PopupClosed", 0, EVENT_VALSTR_NULL);
		}
	}

	if (f->eve.onpopupended_a && f->eve.onpopupended_a[0]) {
		//Object_Property_Set (f, "PopUpQuestion", svalue);
		ccs *not_null_value = svalue ? svalue : "";
		Object_Property_Set (f, "PopUpAnswer", not_null_value);
		RELATED_ (DevPopUpEnded)
		exopopupendedfn_t onpopupended_fn = (exopopupendedfn_t)Event_Find_Function(f->eve.onpopupended_a);
		if (onpopupended_fn) {
			onpopupended_fn (f, f->frm.kpopup_initiator);
		} else {
			Con_PrintLinef (CON_RED "Form_PopUpEnded for " QUOTED_S "not found!", f->eve.onpopupended_a);
		}
	}

	return k;
}

// Return NULL if input not processed - it matters May 11 2026
RELATED_ (EditModeActivate O_ContextMenu_Popup_Method O_ContextMenu_KeyDown) //CONTEXTOID
ktrl O_ContextMenu_KeyDown (ktrl k, int key, int unicode)
{
#if 1 // Baker: Jan 16 2025 - This crashes and might always have.  Doesn't seem to have vc filled
	ktrl kreply = Lister_KeyDown(k,key,unicode);

	if (kreply) return kreply; // CONTEXTOID .. NULL on show.  Editmode must already be on.  2X event?
#endif

	// NOT HANDLED
	switch (key) {
	case K_ENTER:
		// The event CANNOT fire here.  Should wait until next frame?
		{
			ktrl k_gets_focus = k->fctrl->frm.kcontextoldfocus;// kcontextoldfocus gets cleared
			if (k->selectedindex != not_found_neg1)
				O_Context_Menu_Exec (k); // O_ContextMenu_Leaving ENTER  CONTEXTOID FIRE <========

			Oject_Focus_Set (k_gets_focus); // CONTEXTOID ... oldfocusneeds set HIDE
		}
		break;

	case K_ESCAPE:
		Oject_Focus_Set (k->fctrl->frm.kcontextoldfocus); // O_ContextMenu_Leaving Escape CONTEXTOID HIDE <========
		return k;

	default:		return NULL; // NO ACTION
	} // sw

	return NULL;
}


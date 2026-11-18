// kz_form.c.h

RELATED_ (Lister_Spawn O_ContextMenu_Draw)

CALLERS_ (Form_Mouse_Move)
// Called by ToolTips_Maybe_Start_One which is actually below us
// int cursor_height = GetSystemMetrics(SM_CYCURSOR);

#define TOOLTIP_MARGIN_BORDER_TOTAL_WIDTH_ADD_6		6
#define TOOLTIP_DISTANCE_18							18

static void ToolTip_Deploy_If_Target_ToolTipText (ktrl f, ktrl k_over, int mouse_x, int mouse_y) // X Y in form coordinates
{
	ktrl ktip = FP(servo.ktooltip);
	ktrl k = FP(servo.ktooltip); // As ktip
	VarMyFont(myfont);

	if (myfont == NULL) {
		// vid_restart doesn't have the font yet.
		// PUNT !
		return;
	}

	ccs *tooltip_raw_text = k_over->tooltip_a;
	if (tooltip_raw_text == NULL) {
		//int j = 5;
		return; // Doesn't have a tooltip
	}

	ccs *tooltip_text = tooltip_raw_text;
	qbool is_valign = false; // VALIGN:
	qbool is_moveup_one_fontsize = false; // UP1ROW:
	if (String_Starts_With_Caseless(tooltip_raw_text, "VALIGN:")) { // LEN 7
		tooltip_text = &tooltip_raw_text[7];
		is_valign = true;
	}

	 
	else if (String_Starts_With_Caseless(tooltip_raw_text, "UP1ROW:")) { // LEN 7 also
		tooltip_text = &tooltip_raw_text[7];
		is_moveup_one_fontsize = true;
	}

	ktip->hover_candidate = k_over;
	Object_Property_Set (ktip, "Caption", tooltip_text);

	int cl_width = vid.width; //f->cm.relative_rect.width;
	int cl_height = vid.height; // f->cm.relative_rect.height;

	// There was multiline text here.

	RELATED_ (O_Label_Refresh_Plus_Early O_ContextMenu_Activate_At)

	// Any attempt to avoid this would just make life harder ...
	int numrows = 1;
	int tooltip_px_height	= k->fontsize * numrows;
	int tooltip_px_width	= Draw_StringWidthInt (myfont, KP(caption_a), KP(fontsize));

	// At this point we don't have multiline draw but the box should reflect intended size
	// We want to display the tooltip safely below the mouse cursor.
	// Otherwise we have the mouse cursor in the way.
	k->cm.relative_rect.left	=	mouse_x;
	k->cm.relative_rect.top		=	mouse_y + TOOLTIP_DISTANCE_18; // Below?

	if (is_moveup_one_fontsize) {
		k->cm.relative_rect.top -= k->fontsize * 5; // UP1ROW
		k->cm.relative_rect.left -= k->fontsize * 5; // UP1ROW
	} else 
	if (is_valign && RECT_BOTTOMOF(k_over->r_screen) > k->cm.relative_rect.top) {
		// Try to not cover the thing we are hovering.
		k->cm.relative_rect.top = RECT_BOTTOMOF(k_over->r_screen) + TOOLTIP_MARGIN_BORDER_TOTAL_WIDTH_ADD_6;
	}


	k->cm.relative_rect.width	=	tooltip_px_width;
	k->cm.relative_rect.height	=	tooltip_px_height;

	k->cm.relative_rect.width	+=	TOOLTIP_MARGIN_BORDER_TOTAL_WIDTH_ADD_6;	// Add some padding to width and height
	k->cm.relative_rect.height	+=	TOOLTIP_MARGIN_BORDER_TOTAL_WIDTH_ADD_6;
	k->cm.relative_rect.left	-=	k->cm.relative_rect.width / 2; // Center it.

	// bottom cutoff?  Prevent from off-screening (if we can?)
	//                              |
	//                              |
	// -----------------------------|  <--- FORM CL HEIGHT 800 - RECT BOTTOM OF TOOLTIP 810
	//

	int beyond_form_bottom_px = RECT_BOTTOMOF(k->cm.relative_rect) - cl_height;

	if (beyond_form_bottom_px > 0) { // It would be at least partially offscreen
		// MOVE THE TOOLTIP TO ABOVE THE MOUSE CURSOR INSTEAD OF BELOW THE MOUSE CURSOR
		k->cm.relative_rect.top = mouse_y - TOOLTIP_DISTANCE_18 - k->cm.relative_rect.height;
		// Let's keep it simple please  NO THIS COLLIDES WITH MOUSE CURSOR INTERFERING WITH READING THE TOOLTIP
		//k->pp.relative_rect.top -= beyond_form_bottom_px; // Pull it back onscreen.  NO FAIL.  Often mouse cursor is over it
		//											and you can't read the tooltip easily and it is annoying as hell
	}

	// right cutoff
	int beyond_form_right_px = RECT_RIGHTOF(k->cm.relative_rect) - cl_width;
	if (beyond_form_right_px > 0) { // Cut off BELOW
		k->cm.relative_rect.left -= beyond_form_right_px;
	}

	// Left cutoff
	if (k->cm.relative_rect.left < 0)	k->cm.relative_rect.left	= 0;
	if (k->cm.relative_rect.top < 0)	k->cm.relative_rect.top		= 0;

	k->hover_when_time_is = 0;
	k->is_hidden = false;

	Form_QueueRefresh (f);
}

CALLERS_ (Form_Mouse_Move)
void ToolTips_Maybe_Start_One (ktrl f, ktrl k_over_or_null, int x, int y, ccs *reason)
{
	oject_s	*ktip = f->servo.ktooltip;

	if (k_over_or_null == NULL) {
		ToolTips_End_Any (f, "Over nothing");
		return;
	}

	ktrl k_over = k_over_or_null; // We are known to not be NULL

	if (k_over == ktip && ktip->is_hidden == false) {
		return; // Showing tip and ktip is over self
	}
	if (k_over == ktip->hover_candidate && ktip->is_hidden == false) {
		return; // Already hovering this
	}

	// Any Z Action prevents a hover from deploying.
	if (f->servo.kcontextmenu->is_hidden == false) {
		return; // A context menu is showing.
	}

	if (ktip->is_hidden == false) {
		ToolTips_End_Any (f, "No longer over the tooltip target");
		// DO NOT RETURN
	}
	ToolTip_Deploy_If_Target_ToolTipText (f, k_over, x, y);
}


void ToolTips_End (ktrl f, ccs *s_reason)
{
	oject_s	*ktip = f->servo.ktooltip;

	ktip->hover_candidate = NULL;
	ktip->hover_when_time_is = 0;

	//EVENT ZENDING WOULD BE HERE // Before visibility is nuked
	ktip->is_hidden = true; //CONTROL_IMMEDIATE_RESIZE_DIRTY (k_tip, "ToolTip Set Hidden"); // Resize because may impact other objects.

	//frm->formx->zo.z_tooltip = NULL;
}

CALLERS_ (O_ContextMenu_Spawn Form_MouseButtonAction Form_Mouse2Down Form_Mouse2Up Form_KeyDown)
void ToolTips_End_Any (ktrl f, ccs *s_reason)
{
	oject_s	*ktip = f->servo.ktooltip;
	if (ktip)
		ToolTips_End (f, s_reason);
}

CALLERS_ (Form_Life)
ktrl Form_ContextMenu_Spawn (ktrl f)
{
	ktrl ktooltip = Object_Create_And_Assign_By_ClassEnum(class_label, &f->servo.ktooltip, f, SERVANT_KCAN_NULL, /*serv owner*/ f);
	{
		KXP(ktooltip, is_hidden) = true;
		KXP(ktooltip, fontsize) = f->fontsize;
		RELATED_ (O_Label_Draw)
		Object_Property_Set (ktooltip, "BackColor", "#FFF");
		Object_Property_Set (ktooltip, "ForeColor", "#000");
	}
	int dd;
	ktrl kdot;
	// Baker: CM_MIDDLE_DOT_4 comes first, covers everything and doesn't draw.
	{
		dd = CM_MIDDLE_DOT_4;
		kdot = Object_Create_And_Assign_By_ClassEnum(class_sizedot, &f->servo.kdots[dd], f,
		SERVANT_KCAN_NULL, /*serv owner*/ f);

		KXP(kdot, direction) = dd;
		KXP(kdot, is_hidden) = true;
	}

	for (dd = 0; dd < CM_DOTCOUNT_COUNT_9; dd ++) {
		if (dd == CM_MIDDLE_DOT_4)
			continue;

		/*ktrl*/ kdot = Object_Create_And_Assign_By_ClassEnum(class_sizedot, &f->servo.kdots[dd], f,
			SERVANT_KCAN_NULL, /*serv owner*/ f);

		KXP(kdot, direction) = dd;
		KXP(kdot, is_hidden) = true;
	}

//kmu_is_zsupreme_or_wont_work_in_edit_mode: CONTEXTOID SPAWN
	ktrl kmnu = Object_Create_And_Assign_By_ClassEnum(class_contextmenu, &f->servo.kcontextmenu, f, SERVANT_KCAN_NULL, /*serv owner*/ f);
#if 1
		RECT_SET (kmnu->cm._relative_rect_orig, 200, 200, 200, 0);
		kmnu->_fontsize_orig = 12;
#endif
		kmnu->cellspacingypct = 0;

		RELATED_ (O_ContextMenu_Draw)
		VectorCopyDestSrc (kmnu->backcolor, color3_gray_875);
		VectorCopyDestSrc (kmnu->backcolorselected, color3_red);
		//RELATED_ (O_Rectangle_Refresh)
		const things_s *po_list = Thing_Find_By_Enum_Id (prop_list);
		Property_Set_By_String_After_Freeing_Is_Ok(kmnu, po_list,
			//CM_0_TEXT_0
			//CM_1_CHECKED_1
			//CM_2_STRING_ID_2
			//CM_3_FUNCSTRING_3
			"Form State To Clipboard,0,Print,ObjectCommand,"
			" ,0,Option2String,Option2FunctionString,"
			" ,0,Option3String,Option3FunctionString,"
			"-,0,DASH,DASH,"
			"Edit Mode,0,EditModeString,EditModeToggle",
			NULL // No trailing comma
		);

		kmnu->is_hidden = true;

	return kmnu;
} // Form_ContextMenu_Spawn


RELATED_ (x = FIELDS_DEF Object_Create_Old)
CALLERS_ (Form_Draw Form_Create_From_String)
// Jan 25 2026 - Finalize functions can run multiple times
// according to the code I see here.
void Form_Finalize_Refresh (ktrl f)
{
	GoogleRobotoFont_Check ();

	// Obsolete: Allow the form to be in the array.
	// Baker: Jan 25 2026 - pretty damn sure we don't have
	// forms in the array.
	NPA__ BakerArray_Erase (f->frm.drawn_list_a);

	// Jan 25 2026 - I think if focus changes
	// due to disabled or hidden control, need to
	// refresh AFTER focus changes
	if (f->frm.kfocused)
		if (f->frm.kfocused->is_disabled || f->frm.kfocused->is_hidden)
			f->frm.kfocused = NULL;

	// If no focus set, set focus to first one that we can
	if (f->frm.kfocused == NULL) {
		for_each_form_all_controls (n, f, k)
			if (Oject_Will_Focus_Currently(k) == false) 
				continue;
#if 1 // Jan 16 2025 - Route all focus through ...
			Oject_Focus_Set (k);
#else
		f->frm.kfocused = k;
#endif

//			if (k == f)
//				int j = 5;
			break;
		for_each_end
	} // find something to focus
	// Seal the form
	if (FP(frm.numrefreshes) == 0)
		Object_Finalize_Recursive (f); // Runs finalize function ONCE

	if (FP(frm.has_metric_run) == false) {
		Object_Metric_Recursive (f); // Refresh the footprint
		FP(frm.has_metric_run) = true;
	}

	Object_Refresh_Recursive (f); // Refresh the footprint
} // Form_Finalize_Refresh

RELATED_ (Object_Destroy_Recursive)
ktrl Form_Life (ktrl f)
{
	if (f == CREATE_NULL) {
		// Create Form
		ktrl form_new = NULL;
		Object_Create_And_Assign_By_ClassEnum (class_form_1, &form_new, FORM_NULL, FORM_KCAN_NULL, SERVANT_TO_NULL);
		form_new->fctrl = form_new;
#if 1
#if 1 // August 8 2025
		if (!devinfomempool) {
			devinfomempool = Mem_AllocPool("devinfo", /*flags*/ 0, /*parent*/ NULL);
		}
#endif
		NPA__ LIFE_ALLO___ form_new->frm.controls_a = (oject_array_s *)Mem_Alloc_SizeOf(devinfomempool, *form_new->frm.controls_a);
		NPA__ LIFE_ALLO___ form_new->frm.drawn_list_a = (oject_array_s *)Mem_Alloc_SizeOf(devinfomempool, *form_new->frm.drawn_list_a);
#else
		NPA__ LIFE_ALLO___ form_new->frm.controls_a = (oject_array_s *)ZAalloc_VoidP_SizeOf(*form_new->frm.controls_a);
		NPA__ LIFE_ALLO___ form_new->frm.drawn_list_a = (oject_array_s *)ZAalloc_VoidP_SizeOf(*form_new->frm.drawn_list_a);
#endif
		// Baker: January 9 2025 - This would be better served being created at the
		// very end so it can inherit the font size.
		Form_ContextMenu_Spawn (form_new);

		RECT_SET (form_new->cm.relative_rect, 0, 0, vid.width, vid.height);
		return form_new;
	}

	// DESTROY ALL CONTROLS (the form is not in its own list, we destroy it last).
	for_each_form_all_controls(n,f,k_needed) // IT IS CORRECT
		f->frm.controls_a->pointers_array[n] = Object_Destroy_Not_Recursive (f->frm.controls_a->pointers_array[n]);
	for_each_end

#ifdef _DEBUG
	extern int num_frees;
	int j = num_frees;
#endif

	LIFE_FREE___ NPA__ Mem_FreeNull_ (f->frm.controls_a->pointers_array); // Baker_Array_Erase
	LIFE_FREE___ NPA__ Mem_FreeNull_ (f->frm.controls_a);
	LIFE_FREE___ NPA__ Mem_FreeNull_ (f->frm.drawn_list_a->pointers_array); // Baker_Array_Erase
	LIFE_FREE___ NPA__ Mem_FreeNull_ (f->frm.drawn_list_a);
	f = Object_Destroy_Not_Recursive (f);

	Vid_Cursor_Set (mousepointer_arrow_default_0);
	return f; // NULL because destroyed
} // Form_Life / Form_Create

ktrl Form_Destroy (ktrl f)
{
	return Form_Life(f); // Destroys it
}

		// What is method in
//#pragma message ("Is form in drawn array?")
ktrl Form_Hit_Drawn_Any (ktrl f, int x, int y)
{
	ktrl kdot4hit = NULL;

	for_each_form_drawn_reverse	(n,f,kdrawn)
		if (RECT_HIT(KXP(kdrawn,r_screen_clipped), x, y) == false) continue;
		// EDITMODE: Even non-mouseable objects can get focus if they are NOT a servant
		// Servant that should not get focus in editmode is like column line or xscroll for a listview
		// Baker: November 1 2025 - this is clear as mud ...
		//          Will Mouse or edit model and not a servant)
		qbool is_eligible = Oject_Will_Mouse(kdrawn) || (FP(frm.editmode) && KXP(kdrawn, servant_owner) == NULL);
		if (is_eligible && kdrawn->is_hidden)
			is_eligible = false;

		// Baker: April 18 2026 - This should not be necessary as should not be in the draw list
		#pragma message ("Why are movers in the draw list?")
		RELATED_ (Form_Draw)
		if (is_eligible && kdrawn->servant_owner && kdrawn->servant_owner->is_hidden)
			is_eligible = false;

		if (!is_eligible)
			continue;

		// ONLY ALLOW DOT4 TO COLLIDE IF THE HIT WOULD OTHERISE HIT THE FOCUSED CONTROL
		if (kdrawn == f->servo.kdots[CM_MIDDLE_DOT_4]) {
			kdot4hit = kdrawn;
			continue;
		}

		// HIT

		if (kdot4hit && kdrawn == f->frm.kfocused) {
			// KDOT 4 WAS HIT
			return kdot4hit;
		}

		return kdrawn; // HIT
	for_each_end // each kdrawn
	return NULL;
}


CALLERS_ (Form_Create_From_String)
ktrl Form_Create (void)
{
	return Form_Life(CREATE_NULL); // Creates it
}

CALLERS_ (O_ScrollBar_Action) // Sets for update when mouse is not moved.
void Form_PressHook_Set (ktrl f, ktrl k, mouse_hook_fn_t fn, timey_dbl_t hookwhen)
{
	FP(frm.kmousepresshook) = k;
	FP(frm.hook_fn) = fn;
	FP(frm.hook_nextthink) = hookwhen;
}

void Form_PressHook_Clear(ktrl f, ktrl k)
{
	if (FP(frm.kmousepresshook) && (FP(frm.kmousepresshook) != k)) {
		//int j = 5;
		return; // A control can only clear its own
	}
	FP(frm.kmousepresshook) = NULL;
	FP(frm.hook_fn) = NULL;
	FP(frm.hook_nextthink) = 0;
}

RELATED_ (Form_Finalize_Refresh)
ktrl Form_Draw (ktrl f)
{
	GoogleRobotoFont_Check ();

refresh_check:
	if (FP(frm.refreshneeded) || FP(frm.refreshdots)) {
		Form_Finalize_Refresh (f);
		FP(frm.numrefreshes) ++;
		FP(frm.refreshneeded) = 0;
		//FP(frm.refreshdots) = 0;  Done in the loop
	}

	// PREDRAW - A hook might do a refresh
	if ((FP(frm.kmousepresshook) != NULL) && (Host_RealTime() >= FP(frm.hook_nextthink))) {
		// ACTIVE HOOK
		mouse_hook_fn_t MyHookFn = FP(frm.hook_fn);
		ktrl k_hook = FP(frm.kmousepresshook);

		// CLEAR THE HOOK.  Function might set it again.
		FP(frm.kmousepresshook) = NULL;
		FP(frm.hook_fn) = NULL;
		FP(frm.hook_nextthink) = 0;

		RELATED_ (O_ScrollBar_MouseHook_Fire)
		MyHookFn (k_hook); // FIRE!
	}

	// ACTUAL DRAW - FORM IS NOT in the list of drawn
	RELATED_ (drawn_list_a)
	for_each_form_control_with_container_as_no_servants (n, f, f, k)
	    if (KP(is_hidden)) continue; // Don't draw hidden.
		Object_Draw_Recursive_Not_Form (k);
	for_each_end

	// Any form servants have container null just like a form does.
	// All servants are zebras (Z-Order on top which means they draw last)
	for_each_form_servant_owner_is_k (n, f, /*servant owner*/ f, kservant)
		if (KXP(kservant, is_hidden)) continue; // Don't draw hidden.
		if (kservant->servant_owner && kservant->servant_owner->is_hidden)
			continue;

		Object_Draw_Recursive_Not_Form (kservant);
	for_each_end

	Draw_Clip_Clear (); // Submits draw buffer

	// HANDLE POSSIBILITY THAT NO REFRESH DOTS DIDN'T CLEAR
	// BECAUSE EDITMODE WAS NOT SET
	if (FP(frm.refreshdots)) {
		FP(frm.refreshdots) = 0;
	}

	return f;
}


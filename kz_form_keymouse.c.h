// form_keymouse.c.h

////////////// SYNC
////////////// SYNC
////////////// SYNC
ktrl Form_MouseUp (ktrl f, int key, int unicode, int x, int y)
{
	ktrl k_mouse = FP(frm.kmousedown);

	if (k_mouse == NULL) {
		// Double click is an extra event
		if (FP(frm.kmousedown_click_count)) {
			// Jan 26 2025 - Can this happen? Sure can otherwise modelview wouldn't work.
			Form_CSQC_Event_With_Mouse_EZ (f, "DoubleClick", 0, NULL, x, y); // MOUSE1 (FORM)
		} else {
			Form_CSQC_Event_With_Mouse_EZ (f, "Click", 0, NULL, x, y); // MOUSE1 (FORM)
		} // DoubleClick and Click events

		return NULL;
	}

	// March 6 2026 - Return is not used for mouse.
	RELATED_ (DevMouseButtonAction)
	if (k_mouse && f->eve.onmousebuttonaction_a && f->eve.onmousebuttonaction_a[0]) {
		//exofn_t onkeypreview_fn = Event_Find_Function(k->eve.onkeypreview_a);
		exomouseactionfn_t onmousebuttonaction_fn = (exomouseactionfn_t)Event_Find_Function(f->eve.onmousebuttonaction_a);
		RELATED_ (DevListViewOnKeyPreview Form_CSQC_Event_EZ DevMouseButtonAction)
		if (onmousebuttonaction_fn) {
			int did_handle = onmousebuttonaction_fn (k_mouse, mouse_index_left_1, x, y, MOUSEUP_0);
			if (did_handle) {
				return k_mouse;
			}
		} else {
			Con_PrintLinef (CON_RED "Form_MouseButtonAction for " QUOTED_S "not found!", k_mouse->eve.onmousebuttonaction_a);
		}
	}

	// MOUSE UP WITH K_MOUSE
	switch (KCLASS(k_mouse)) {
	default:
	case_break class_ext_closer:	O_ExtCloser_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);
	case_break class_ext_mover:		O_ExtMover_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);
	case_break class_imagegrid:		O_ImageGrid_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);
	case_break class_listview:		O_ListView_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);
	case_break class_scrollbar:		O_ScrollBar_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);

	// Sizedot does nothing with mouse up, added for QC consistency
	case_break class_sizedot:		O_SizeDot_MouseButtonAction		(k_mouse, x, y, MOUSEUP_0);
	//case_break class_tabselect:	O_TabSelect_MouseButtonAction	(k_mouse, x, y, MOUSEUP_0);

	// Textbox mouseup QC does selection actions/tripleclick
	case_break class_textbox:		O_TextBox_MouseButtonAction		(k_mouse, x, y, MOUSEUP_0);
	} // sw


	// Assume it was processed, but still do a polite release
	ktrl k = f->frm.kmousedown;

	RECT_SET (k->ui.mouse_down_rect, 0,0,0,0);
	k->ui.mouse_down_thingi = 0;
	k->ui.mouse_down_time = 0;
	k->ui.mouse_down_origin.x = k->ui.mouse_down_origin.y = 0;
	k->ui.mouse_down_thumb_into.x = k->ui.mouse_down_thumb_into.y = 0;	// How many px into thumb are we?
	k->ui.mouse_down_thumb_at_mousedown_px.x = k->ui.mouse_down_thumb_at_mousedown_px.y = 0;	// How many px into thumb are we?
	k->ui.mouse_down_forced_thumb_pos.x = k->ui.mouse_down_forced_thumb_pos.y = 0;	// How many px into thumb are we?

	k->ui.mouse_down_valuei = 0;
	k->ui.is_move_thresh_met = 0;


	k->ui.mousepointer = mousepointer_arrow_default_0;
	Vid_Cursor_Set (k->ui.mousepointer); // Baker: Dec 30 2025 - Why was this commented out?  Eeek!
	// Baker: Assuming it was commented out for a valid reason like entering a form?


	FP(frm.kmousedown) = NULL;

	if (f->frm.kmousedown_click_count) {
		Form_CSQC_Event_With_Mouse_EZ (k, "DoubleClick", 0, NULL, x, y); // MOUSE1 (FORM)
	} else {
		Form_CSQC_Event_With_Mouse_EZ (k, "Click", 0, NULL, x, y); // MOUSE1 (CONTROL)
	} // DoubleClick and Click events

	//Con_PrintLinef ("Mouse release");
	return NULL;
}

CALLERS_ (m_draw ZForm_MouseMove)
ktrl Form_Mouse_Move (ktrl f, int x, int y)
{
	ktrl k_mouse = FP(frm.kmousedown);
	mousepointer_e mpdo;

	if (k_mouse == NULL) {
		// mouse button not pressed!
		ktrl k_over = Form_Hit_Drawn_Any(f, x, y); // Baker: March 5 2026 these can be servants.

		if (k_over && f->eve.onmousemove_a) {
			//exofn_t onkeypreview_fn = Event_Find_Function(k->eve.onkeypreview_a);
			exomouseactionfn_t onmousemove_fn = (exomouseactionfn_t)Event_Find_Function(f->eve.onmousemove_a);
			RELATED_ (DevListViewOnKeyPreview Form_CSQC_Event_EZ DevMouseButtonAction)
			if (onmousemove_fn) {
				int did_handle = onmousemove_fn (k_over, mouse_index_none_0, x, y, MOUSEUP_0);
				if (did_handle) {
					//return k_over;
				}
			} else {
				Con_PrintLinef (CON_RED "Form_MouseMove for " QUOTED_S "not found!", k_mouse->eve.onmousemove_a);
			}
		}

		ToolTips_Maybe_Start_One (f, k_over, x,y, "Mouse move");

		if (k_over) { // MOUSEMOVE WITHOUT MOUSEDOWN
			// Mouse is over disabled control?  Get out ..
			if (KIsDisabled(k_over))
				return NULL;

			switch (KCLASS(k_over)) { // MOUSE_MOVE NO BUTTON
			default:						k_over = NULL;

			// Baker: March 3 2026 - k_over is allow NULL return to neutral mouse pointer?
			// Q: Does any do that?
			// A: Imagegrid (when no buttons), Listview (when no column sizer is hit)
			case_break class_ext_closer:	k_over = O_ExtCloser_MouseMove	(k_over, x, y);
			case_break class_ext_mover:		k_over = O_ExtMover_MouseMove	(k_over, x, y);
			case_break class_imagegrid:		k_over = O_ImageGrid_MouseMove	(k_over, x, y);
			case_break class_listview:		k_over = O_ListView_MouseMove	(k_over, x, y);
			case_break class_sizedot:		k_over = O_SizeDot_MouseMove	(k_over, x, y);

			// Engine: not do mouseup moveover for scrollbars - March 3 2026 -- WHY?
			//case_break class_scrollbar:	k_over = O_ScrollBar_MouseMove(k_over, x, y);
			case_break class_textbox:		k_over = O_TextBox_MouseMove	(k_over, x, y);
			} // sw
		}

		/*mousepointer_e*/ mpdo =
			k_over ?
			KXP(k_over, ui.mousepointer) : mousepointer_arrow_default_0;
		Vid_Cursor_Set (mpdo);

		return NULL;
	}

	// MOUSE DOWN DURING MOVE
mousedown_mousemove:
//zpr ("Mouse move with kmousedown");
	switch (KCLASS(k_mouse)) { // MOUSEDOWN MOUSEMOVE
	default:
	case_break class_ext_closer:	O_ExtCloser_MouseMove	(k_mouse, x, y); // MOUSEMOVE WITH BUTTON DOWN
	case_break class_ext_mover:		O_ExtMover_MouseMove	(k_mouse, x, y);
	case_break class_imagegrid:		O_ImageGrid_MouseMove	(k_mouse, x, y);
	case_break class_listview:		O_ListView_MouseMove	(k_mouse, x, y);
	case_break class_scrollbar:		O_ScrollBar_MouseMove	(k_mouse, x, y);
	case_break class_sizedot:		O_SizeDot_MouseMove		(k_mouse, x, y);
	case_break class_textbox:		O_TextBox_MouseMove		(k_mouse, x, y);
	} // sw

	/*mousepointer_e*/ mpdo =
		k_mouse ?
		KXP(k_mouse, ui.mousepointer) : mousepointer_arrow_default_0;
	Vid_Cursor_Set (mpdo); // Sets mouse cursor - warning empty macro

	return NULL;
}

// Comes from KeyDown (K_MOUSE1)
// Reroutes to MouseUp if down action
// LEFT button comes here, what about right?
CALLERS_ (Form_KeyDown M_ZDevinfoForm_Key when K_MOUSE1)
ktrl Form_MouseButtonAction(ktrl f, int key, int unicode, int is_down)
{
	int x = in_windowmouse_x, y = in_windowmouse_y;

	ToolTips_End_Any (f, "MouseDown / MouseUp"); // A mousedown ends the hover.
	if (is_down == false) {
		// UP
		//zpr ("Rereoute to up ");
		return Form_MouseUp (f, key, unicode, x, y);
	}

	// Baker: Nov 6 2025 - Disabled controls cannot be hit at this time.
	// And this is a problem because a mouse click could hit an underlying control as a result.
	ktrl k = Form_Hit_Drawn_Any(f, x, y); // Never returns form
#if 0
	if (k->po->enum_id == class_ext_mover) {
		int j = 5;
	}
#endif

	//zpr ("Form_MouseButtonAction hit is #", etos(k));

	if (!k)
		return NULL;

	qbool does_focus_event_occur;

	if (FP(frm.editmode))
		// EDIT MODE: Objects that don't normally focus can get focused
		// except NOT servants (scrollbars, sizing hint, etc.)
		// Context menus -- a servant -- don't receive their focus in this manner
		// and disappear on lost focus so that exception doesn't matter here.
		does_focus_event_occur = KIsNotFocused(k) && KIsNonServant(k) && k != f;
	else
		does_focus_event_occur = KIsNotFocused(k) && Oject_Will_Focus_Currently(k);

	// Disabled controls will not receive any events and cannot be "downed"
	// but they do absorb mouse clicks.
	// March 5 2026 - Our handling of disabled controls is probably very incomplete, however they should not receive focus.
	if (KP(is_disabled))
		goto disabled_skip;

	// Lost Focus event - MOUSEDOWN ONLY
	// Baker: Jan 25 2026 - engine had this here
	// so focus is gained before the mousedown is processed
	// by a specific control type.  I think this is correct
	// because a sizedot on the focused control.
	if (does_focus_event_occur) {
		Oject_Focus_Set (k);
	}

	switch (KCLASS(k)) { // ISDOWN ONLY -- ISUP handled above by transfer to Form_MouseUp
	default: //
	case_break class_button:		O_Button_MouseButtonAction		(k, x, y, MOUSEDOWN_1); // MOUSEDOWN
	case_break class_contextmenu:	O_ContextMenu_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_ext_closer:	O_ExtCloser_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_ext_mover:		O_ExtMover_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	//case_break class_gridview:	O_GridView_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_imagegrid:		O_ImageGrid_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_listbox:		O_ListBox_MouseButtonAction		(k, x, y, MOUSEDOWN_1);
	case_break class_listview:		O_ListView_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_scrollbar:		O_ScrollBar_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_sizedot:		O_SizeDot_MouseButtonAction		(k, x, y, MOUSEDOWN_1);
	case_break class_tabselect:		O_TabSelect_MouseButtonAction	(k, x, y, MOUSEDOWN_1);
	case_break class_textbox:		O_TextBox_MouseButtonAction		(k, x, y, MOUSEDOWN_1);
	} // sw

	FP(frm.kmousedown) = k; // EXCLUSIVE SET

	// Check if under 0.5 since -- if so double it.
	if (FP(frm.kmousedown_time) && ((Host_RealTime() - FP(frm.kmousedown_time)) <= DOUBLE_CLICK_0_5) )
		FP(frm.kmousedown_click_count) ++;
	else
		FP(frm.kmousedown_click_count) = 0;
	FP(frm.kmousedown_time) = Host_RealTime();

disabled_skip: // Baker: Jan 25 2026 - if disabled, how did mousedown happen? March 5 2026: A: Disabled still absorbs mouseclicks.
	return k; // If we hit the form that doesn't count.
} // Form_MouseDown

RELATED_ (Form_MouseButtonAction)
ktrl Form_KeyDown (ktrl f, int key, int unicode, int is_down)
{
	ToolTips_End_Any (f, "Form_KeyDown"); // A mousedown ends the hover.

	if (f->eve.onkeypreview_a && f->eve.onkeypreview_a[0]) {
		exokeyfn_t onkeypreview_fn = (exokeyfn_t)Event_Find_Function(f->eve.onkeypreview_a);
		RELATED_ (DevListViewOnKeyPreview Form_CSQC_Event_EZ)
		if (onkeypreview_fn) {
			int did_handle = onkeypreview_fn (f, key, unicode, is_down);
			if (did_handle) {
				return f;
			}
		} else {
			Con_PrintLinef (CON_RED "Form_KeyDown_OnKeyPreview for " QUOTED_S "not found!", f->eve.onkeypreview_a);
		}
	}

	// Disabled controls should not have keyboard focus.
	ktrl k = FP(frm.kfocused);
	if (!k) return NULL;

	// Baker: January 16 2025 - K_TAB does NOT come here goes to Form_Focus_Next
	RELATED_ (Form_Focus_Next)

	if (k->eve.onkeypreview_a && k->eve.onkeypreview_a[0]) {
		exokeyfn_t onkeypreview_fn = (exokeyfn_t)Event_Find_Function(k->eve.onkeypreview_a);
		RELATED_ (DevListViewOnKeyPreview Form_CSQC_Event_EZ)
		if (onkeypreview_fn) {
			int did_handle = onkeypreview_fn (k, key, unicode, is_down);
			if (did_handle) {
				return k;
			}
		} else {
			Con_PrintLinef (CON_RED "Form_KeyDown_OnKeyPreview for " QUOTED_S "not found!", k->eve.onkeypreview_a);
		}
	}

	// Baker: Jan 15 2026 - O_TextBox_KeyUp is empty function
	if (!is_down) {
		switch (KCLASS(k)) { // UP
		default:
		case_break class_textbox:	return O_TextBox_KeyUp(k, key, unicode);
		} // sw
		return NULL;
	}

	if (FP(frm.editmode) && KCLASS(k) != class_contextmenu)
		return O_SizeDot_Proxy_KeyDown(k, key, unicode);

keybind_routes_to_keydown:
	switch (KCLASS(k)) { // IS DOWN
	case class_button:		return O_Button_KeyDown			(k, key, unicode);
	case class_contextmenu:	return O_ContextMenu_KeyDown	(k, key, unicode);
	//case class_gridview:	return O_GridView_KeyDown		(k, key, unicode);
	case class_imagegrid:	return O_ImageGrid_KeyDown		(k, key, unicode);
	case class_listbox:		return O_ListBox_KeyDown		(k, key, unicode);
	case class_listview:	return O_ListView_KeyDown		(k, key, unicode);
	case class_tabselect:	return O_TabSelect_KeyDown		(k, key, unicode);
	case class_textbox:		return O_TextBox_KeyDown		(k, key, unicode);
	} // sw

	return NULL;
}

CALLERS_ (ZForm_MouseMove)


qbool Form_CSQC_Event_With_Mouse_EZ (ktrl k, ccs *s_event_name, float fval, ccs *s_value_string, float x, float y)
{
	prvm_prog_t *prog = CLVM_prog;

	if (!k->fctrl->ui.csqc_handle_plus1)
		return false;
	vec3_t mousepos = {x,y,0};
	VectorCopyDestSrc (PRVM_clientglobalvector(v_forward), mousepos);  // src -> dest
	return Form_CSQC_Event_EZ (k, s_event_name, fval, s_value_string);
}


qbool Form_CSQC_Event_EZ (ktrl k, ccs *s_event_name, float fval, ccs *s_value_string)
{
	if (!k->fctrl->ui.csqc_handle_plus1)
		return false;

	Form_Get(f,k);
	return CL_VM_FormEvent (
		UNPLUS1(f->ui.csqc_handle_plus1),
		f->ui.form_event_qcfunc,
		k->cm.name_a,
		s_event_name,  // OnClick or whatever
		fval,
		s_value_string
	);
}


ktrl Form_CSQC_Mouse2Up (ktrl f)
{
	// For now, disallow mouse2 action if mouse1 is doing something already.
	if (f->frm.kmousedown)
		return NULL;

	int x = in_windowmouse_x, y = in_windowmouse_y;
	ktrl khit = Form_Hit_Drawn_Any(f, x, y);

	Form_CSQC_Event_With_Mouse_EZ (khit ? khit : f, "Click", /*right*/ 1, NULL, x, y); // FOR REAL

	return f;
}


ktrl Form_CSQC_Mouse2Down (ktrl f)
{
	// For now, disallow mouse2 action if mouse1 is doing something already.
	if (f->frm.kmousedown)
		return NULL;

#if 1
	// January 29 - CSQC Forms + Listview
	if (f->ui.csqc_handle_plus1 && KM_SHIFT == false ) {
		// CSQC forms only ... for now ....
		// Treat it mostly like a left click.
		Form_MouseButtonAction (f, /*key*/ 0, /*ascii*/ 0, /*down*/ MOUSEDOWN_1);

		// But RightClick doesn't get mouse focus right now.  Maybe it will later?
		f->frm.kmousedown = NULL;
		return f;
	}
#endif
	if (KM_SHIFT) {
		return Form_ContextMenu_EditModeActivate (f, "Form_CSQC_Mouse2Down with SHIFT"); // CSQC EDITMODE CONTEXTOID SHOW
	}
	return f;
}


ktrl Form_Mouse2Down (ktrl f)
{
	ToolTips_End_Any (f, "Form_Mouse2Down"); // A mousedown ends the hover.

	// For now, disallow mouse2 action if mouse1 is doing something already.
	if (f->frm.kmousedown)
		return NULL;

	if (FP(frm.editmode)) {
		// If edit mode, contextmenu not allowed except for edit mode context menu
		goto disabled_skip;
	}

	// Need collsion
	//RELATED_ (Form_MouseButtonAction)
	int x = in_windowmouse_x, y = in_windowmouse_y;

	// Baker: Nov 6 2025 - Disabled controls cannot be hit at this time.
	// And this is a problem because a mouse click could hit an underlying control as a result.
	ktrl k_hit = Form_Hit_Drawn_Any(f, x, y); // Never returns form

	if (k_hit) {
		qbool does_focus_event_occur = KIsNotFocused(k_hit) && Oject_Will_Focus_Currently(k_hit);

		if (does_focus_event_occur) {
			Oject_Focus_Set (k_hit);
		}

		// Disabled controls mouse collide (they must absorb clicks) but no events must fire for them.
		if (KXP(k_hit, is_disabled))
			goto disabled_skip;

		switch (KCLASS(k_hit)) { // ISDOWN ONLY -- ISUP handled above by transfer to Form_MouseUp
		default: //
		case_break class_listview:
			// Baker: We are oddly fine-tuning the behavior here ... oh well?
			if (k_hit->selectedlength == 0)
				O_ListView_MouseButtonAction2 (k_hit, x, y, MOUSEDOWN_1);
		} // sw

		// Jan 29 2025 List Type controls need to treat this as a click.
		if (k_hit && f->eve.onmousebuttonaction_a && f->eve.onmousebuttonaction_a[0]) {
			//exofn_t onkeypreview_fn = Event_Find_Function(k->eve.onkeypreview_a);
			exomouseactionfn_t onmousebuttonaction_fn = (exomouseactionfn_t)Event_Find_Function(f->eve.onmousebuttonaction_a);
			RELATED_ (DevListViewOnKeyPreview Form_CSQC_Event_EZ DevMouseButtonAction)
			if (onmousebuttonaction_fn) {
				int did_handle = onmousebuttonaction_fn (k_hit, mouse_index_right_2, x, y, MOUSEDOWN_1);
				if (did_handle) {
					return k_hit;
				}
			} else {
				Con_PrintLinef (CON_RED "Form_MouseButtonAction for " QUOTED_S "not found!", k_hit->eve.onmousebuttonaction_a);
			}
		}

	} // k_hit

	// If the input was absorbed by DevListViewOnKeyPreview, we exited earlier and never made it here.
	// March 5 2026 - Disabled controls absorb mouse-clicks, so we need to bypass the event triggers.

disabled_skip:

	if (KM_SHIFT)
		return Form_ContextMenu_EditModeActivate (f, "Form_Mouse2Down"); // DEVINFO

	return f;
}

RELATED_ (O_ListView_MouseButtonAction2)
ktrl Form_Mouse2Up (ktrl f) // DEVINFO
{
	ToolTips_End_Any (f, "Form_Mouse2Up"); // A mousedown ends the hover.

	// For now, disallow mouse2 action if mouse1 is doing something already.
	// Baker: March 5 2026 - Imperfect, but we stand by it.
	if (f->frm.kmousedown || FP(frm.editmode))
		return NULL;

	return f;
}

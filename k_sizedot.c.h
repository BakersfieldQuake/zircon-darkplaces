// k_sizedot.c.h


// k.r_screen has been updated and all draw properties known.
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF)
RELATED_ (EditModeSetDotsObject)
RELATED_ (Object_Refresh_Recursive O_TabSelect_Refresh_Plus_Early)
ktrl O_SizeDot_Refresh (ktrl k)
{
	// Finalize can hit multiple times.
	// We have an origin, now set the position

	if (KP(direction) != alignment_middle_center_4) {
		int dsize = DDOT_SIZE_8 * 2;
		RECT_SET (KP(cm.relative_rect),
			KP(cm.dot_origin.x) - dsize / 2,
			KP(cm.dot_origin.y) - dsize / 2,
			dsize,
			dsize
		);
		rect_s r_area = { RECT_SEND_ISO_CONTRACT(KP(cm.relative_rect), 4) };
		RECT_COPY (KP(tb.r_button), r_area);
	}
	return k;
}

// Draw enters with clip set to r_screen for k
void O_SizeDot_Draw (ktrl k)
{
	if (KP(direction) == CM_MIDDLE_DOT_4)
		return; // Middle dot doesn't draw.  It moves.

	Draw_Rect (KPA(tb.r_button), KP(backcolor), alpha_1_0);
}

RELATED_ (Form_MouseButtonAction O_ListView_ColumnDragContinue EditModeSetDotsObject)
ktrl O_SizeDot_PlacementDragContinue (ktrl k, int x, int y, int is_done)
{
	Form_Get (f,k);
	int	delta_x		= x - KP(ui.mouse_down_origin.x);
	int 	delta_y		= y - KP(ui.mouse_down_origin.y);
	ktrl kshadow = FP(frm.kfocused);

	// Calculate new size of shadowed object based on right bottom for the moment.
	rect_s r_shadow_new;
	RECT_COPY (r_shadow_new, KP(ui.mouse_down_shadow_rect));

	Rect_Directional_Adjust (ADDRESSOF(r_shadow_new), (alignment_e) KP(direction), delta_x, delta_y);
	//Rect_Directional_Bound_Mins (&r_shadow_original, 5, 5, 16, 16);

	RECT_COPY (KXP(kshadow, cm.relative_rect), r_shadow_new); // ADJUSTMENT HERE SIZEOIC

	// Anti-metric it
#ifndef QUAKEC // ENGINE ONLY
	if (f->ui.csqc_handle_plus1 == 0) {
#endif // !QUAKEC
		if (autocvar_magnification != 1) { // no magnix
			FP(frm.has_metric_run) = false; // KICK IT
			RELATED_ (Zircon_UI_ConsizeThink Object_Metric_Recursive)
	
			RECT_COPY (KXP(kshadow, cm._relative_rect_orig), KXP(kshadow, cm.relative_rect));
			float minfac = 1 / autocvar_magnification;
			KXP(kshadow, cm._relative_rect_orig.left) *= minfac;
			KXP(kshadow, cm._relative_rect_orig.top) *= minfac;
			KXP(kshadow, cm._relative_rect_orig.width) *= minfac;
			KXP(kshadow, cm._relative_rect_orig.height) *= minfac;
		}
#ifndef QUAKEC // ENGINE ONLY
	}
#endif // !QUAKEC

	Form_QueueDotsRefresh (f); // f->frm.refreshdots ++

	if (is_done) {
		// O_SizeDot_PlacementDragEnd
		KP(ui.mousepointer) = mousepointer_arrow_default_0;
	} else {
		KP(ui.mousepointer) = cursor_for_dir(KP(direction), isdown_true);
	}
	//Vid_SetWindowTitlef ("Mousepointer %d", k->ui.mousepointer);
	Form_QueueRefresh (f);
	return k;
}

// isdown:  f->frm.kmousedown is SET by Form_MouseButtonAction AFTER we exit
// !isdown: f->frm.kmousedown should be us.
RELATED_ (O_ScrollBar_MouseButtonAction O_ListView_ColumnDragContinue Oject_Focus_Set )

ktrl O_SizeDot_MouseMove (ktrl k, int x, int y)
{
	if (KFP(frm.kmousedown) == k)
		return O_SizeDot_PlacementDragContinue (k, x, y, isdone_false);

	KP(ui.mousepointer) = cursor_for_dir(KP(direction), isdown_false); // like mousepointer_size_both_10
	return k;
}

ktrl O_SizeDot_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	if (is_down == false) {
		// THIS IS MOUSE RELEASE (MOUSEUP WHEN WAS DOWN)
		KP(ui.mousepointer) = mousepointer_arrow_default_0;
		return O_SizeDot_PlacementDragContinue (k,x,y, isdone_true);
	}

	Form_Get (f, k);

	// MOUSEDOWN
	// O_SizeDot_PlacementDragStart

	// We need to zorder is bastard now
	ktrl kshadow = FP(frm.kfocused);
	Object_Method_ZOrder_Front (kshadow);

	KP(ui.mouse_down_time) = Host_RealTime();
	KP(ui.mouse_down_origin.x) = x;
	KP(ui.mouse_down_origin.y) = y;

	RECT_COPY (KP(ui.mouse_down_rect), KP(cm.relative_rect));
	RECT_COPY (KP(ui.mouse_down_shadow_rect), KXP(kshadow, cm.relative_rect));

	// NO HOOK THAT IS A PERPETUAL THUMP WITH NO MOUSEMOVEMENT
	Form_QueueRefresh (f);
	return k;
}

// January 16 2025: Nudge
RELATED_ (O_TextBox_KeyDown)

// K is the control focused here ...
ktrl O_SizeDot_Proxy_KeyDown (ktrl k, int key, int unicode)
{
// k is NOT a sizedot but the button being moved.
	Form_Get(f,k);
	ktrl k_mouse = FP(frm.kmousedown);

 	// Refused.  Pick one or the other, don't do both.
	// Baker: Jan 25 2026 - why?  Explain ...
	// This is a key event.  If the mouse is down on the
	// control, do not let the keyboard do actions.
	if (k_mouse)
		return k; // MOUSEDOWN - DENY ANY KEYBOARD ACTIONS (PICK ONE OR THE OTHER, BOTH == DENIED)

	qbool didact = false;
	qbool uses_magnification = f->ui.csqc_handle_plus1 == 0 && autocvar_magnification != 1;

	int add_left = 0;
	int add_top = 0;
	int add_width = 0;
	int add_height = 0;

	switch (key) {
	default:
	case_break K_ESCAPE:
		return NULL; // We must return NULL to allow exiting the menu.
	case_break K_DELETE:
		// We could set is hidden here and rename to deleted
		//Object_Property_Set_Fmt (k, "IsHidden", "False");
		Object_Property_Set(k, "IsHidden", "True");
		Form_Focus_Next (f, FOCUS_PREV_NEG1);

	case_break '=':
		Object_Method_ZOrder_Front (k);
		Form_QueueDotsRefresh (f); // f->frm.refreshdots ++
		Form_QueueRefresh (f);
	
	case_break '-':
		Object_Method_ZOrder_Back (k);
		Form_QueueDotsRefresh (f); // f->frm.refreshdots ++
		Form_QueueRefresh (f);

	case_break 'c': // CLONE
		if (KM_CTRL) {
			ktrl kcan = k->fctrl == f ? f : k->container;
			ktrl dpk = NULL;

			ktrl knew =
				Object_Create_And_Assign_By_ClassEnum (class_rectangle, &dpk, f,kcan, /*servant to*/ NULL);
			Object_Property_Set(knew, "BackColor", "#f00");
			va_super (s_rect, 32, "%d,%d,%d,%d", RECT_SEND(k->cm.relative_rect));
			Object_Property_Set(knew, "Rect", s_rect);
			Oject_Focus_Set (knew); // Is this right?
		}
	} // sw

	if (KM_CTRL) goto ctrl_only;
	if (KM_SHIFT) goto shift_only;

	switch (key) {
	default:
	case_break K_LEFTARROW:		didact = true; 
								add_left = -1;

	case_break K_RIGHTARROW:	didact = true;
								add_left = 1;

	case_break K_UPARROW:		didact = true;
								add_top = -1;
	case_break K_DOWNARROW:		didact = true; 
								add_top = 1;
	}
	goto processed_key;

shift_only:

	// SHIFTED
	switch (key) {
	default:
	case_break K_LEFTARROW: // SHIFT - Contract RIGHT
		didact = true; 
		add_width = -1;

	case_break K_RIGHTARROW: // SHIFT - Extend RIGHT
		didact = true; 
		add_width = 1;

	case_break K_UPARROW: // SHIFT - Contract DOWN
		didact = true; 
		add_height = -1;

	case_break K_DOWNARROW: // SHIFT - Extend DOWN
		didact = true; 
		add_height = 1;
	}
	goto processed_key;

ctrl_only:

	goto processed_key;

processed_key:

	if (didact) {
		// Anti-metric it
		if (f->ui.csqc_handle_plus1 == 0) { // no magnix
			FP(frm.has_metric_run) = false; // KICK IT
			RELATED_ (Zircon_UI_ConsizeThink)
			
			KP(cm._relative_rect_orig.left) += add_left;
			KP(cm._relative_rect_orig.top) += add_top;
			KP(cm._relative_rect_orig.width) += add_width;
			KP(cm._relative_rect_orig.height) += add_height;
		}

		Form_QueueDotsRefresh (f); // f->frm.refreshdots ++
		Form_QueueRefresh (f);
	}
	return k; // The caller returns this -- MOVE ALONG
}

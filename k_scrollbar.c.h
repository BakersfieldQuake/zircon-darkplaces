// k_scrollbar.c.h

// Baker: Nov 9 2025 - This can be a servant to a listview or other things

RELATED_ (O_ScrollBar_Refresh)

void O_ScrollBar_MouseHook_Fire (ktrl k);
ktrl O_ScrollBar_Refresh (ktrl k);
CALLERS_ (O_ScrollBar_Action /*O_ScrollBar_MouseMove*/ thumb)
void Lister_Scrollbar_Property_Set_Float (ktrl k, float v)
{
	// Baker: Revamp Jan 13 2026
	SET___ KP(value) = CLAMP(0, v, KP(maxvalue));
	KP(cm.in_recursion) ++;
		// ListBox scroll event
		Lister_SetPct (KP(servant_owner), KP(value), k);
	KP(cm.in_recursion) --;
	// We want a refresh.  Caller should provide it.
}

CALLERS_ (O_ListBox_VisRangeCheck_Refresh)
void O_Scrollbar_SetPct (ktrl k, float pct)
{
	if (KP(cm.in_recursion))
		return;

	Lister_Scrollbar_Property_Set_Float (k, pct);

	O_ScrollBar_Refresh (k);
}

// k.r_screen has been updated and all draw properties known.  
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF)
ktrl O_ScrollBar_Refresh (ktrl k)
{
	if (KP(is_horizontal)) {
		RECT_SET (KP(sb.r_top),		KP(cm.relative_rect.left), KP(cm.relative_rect.top), KNOB_SIZE_16, KNOB_SIZE_16);
		RECT_SET (KP(sb.r_track),	KP(cm.relative_rect.left)+ KNOB_SIZE_16, KP(cm.relative_rect.top), KP(cm.relative_rect.width) - KNOB_SIZE_16 * 2, KP(cm.relative_rect.height));
		RECT_SET (KP(sb.r_bottom),	RECT_RIGHTOF(KP(cm.relative_rect))-KNOB_SIZE_16, KP(cm.relative_rect.top), KNOB_SIZE_16, KNOB_SIZE_16 );
	} else {
		RECT_SET (KP(sb.r_top),		KP(cm.relative_rect.left), KP(cm.relative_rect.top), KNOB_SIZE_16, KNOB_SIZE_16);
		RECT_SET (KP(sb.r_track),	KP(cm.relative_rect.left), KP(cm.relative_rect.top) + KNOB_SIZE_16, KP(cm.relative_rect.width), KP(cm.relative_rect.height) - KNOB_SIZE_16 * 2);
		RECT_SET (KP(sb.r_bottom),	KP(cm.relative_rect.left), RECT_BOTTOMOF (KP(cm.relative_rect)) - KNOB_SIZE_16, KNOB_SIZE_16, KNOB_SIZE_16 );
	}

	float thumb_size_pct = 0.50;

	if (KP(is_horizontal)) {
		KP(sb.thumbsize_px)		= ifloor(thumb_size_pct * KP(sb.r_track.width));
		vclamp (KP(sb.thumbsize_px), KNOB_SIZE_16, KP(sb.r_track.width) - 1);
		KP(sb.track_empty_px)	= KP(sb.r_track.width) - KP(sb.thumbsize_px);
	}  else {
		KP(sb.thumbsize_px)		= ifloor(thumb_size_pct * KP(sb.r_track.height));
		vclamp (KP(sb.thumbsize_px), KNOB_SIZE_16, KP(sb.r_track.height) - 1);
		KP(sb.track_empty_px)	= KP(sb.r_track.height) - KP(sb.thumbsize_px);
	}

	KP(sb.num_top_positions) = KP(sb.track_empty_px) + 1;
	// 0 to 49   track height 60  thumb height 10 ..
	// thumb can be at 0 through 9
	// thumb can be at 50 through 59
	// 50 available spots = means 51 positions.

	float pct_down = KP(value) / KP(maxvalue) /*d:1*/; // 0 to 1.0

	int pixels_down = Math_Round(pct_down * KP(sb.track_empty_px));

	if (KP(is_horizontal)) {
		RECT_SET (KP(sb.r_thumb),  KP(cm.relative_rect.left)+ KNOB_SIZE_16 + pixels_down, KP(cm.relative_rect.top), KP(sb.thumbsize_px), KNOB_SIZE_16);
	} else {
		RECT_SET (KP(sb.r_thumb),  KP(cm.relative_rect.left), KP(cm.relative_rect.top) + KNOB_SIZE_16 + pixels_down, KNOB_SIZE_16, KP(sb.thumbsize_px));
	}

	if (KFP(frm.kmousedown) == k && KP(ui.mouse_down_thingi) == SB_ACTION_THUMB_4) {
		// Thumb is mouse controlled
		if (KP(is_horizontal)) {
			KP(sb.r_thumb.left)    = KP(ui.mouse_down_forced_thumb_pos.x);

			// LIMIT TO TRACK
			if (KP(sb.r_thumb.left) < KP(sb.r_track.left))
				KP(sb.r_thumb.left) = KP(sb.r_track.left);
			else if (RECT_RIGHTOF(KP(sb.r_thumb)) > RECT_RIGHTOF(KP(sb.r_track))) {
				KP(sb.r_thumb.left) = RECT_RIGHTOF(KP(sb.r_track)) - KP(sb.r_thumb.width);
			}

		} else {
			KP(sb.r_thumb.top)    = KP(ui.mouse_down_forced_thumb_pos.y);

			// LIMIT TO TRACK
			if (KP(sb.r_thumb.top) < KP(sb.r_track.top))
				KP(sb.r_thumb.top) = KP(sb.r_track.top);
			else if (RECT_BOTTOMOF(KP(sb.r_thumb)) > RECT_BOTTOMOF(KP(sb.r_track))) {
				KP(sb.r_thumb.top) = RECT_BOTTOMOF(KP(sb.r_track)) - KP(sb.r_thumb.height);
			}
			//Con_PrintLinef ("Thumb at y %d", k->mousedown_thumb_pos.y);
		}
	}

	//Con_PrintLinef ("Thumb at y %d", k.mouse_down_thumb_pos.y);

	return NULL;
}

void O_ScrollBar_Draw (ktrl k)
{
	Form_Get (f,k);
	
top:
	switch (FP(appearance)) {
	case appearance_flat_1:
		Draw_Rect	(KPA(sb.r_top), FP(scrollbarcolor), alpha_1_0);
		break;

	default:
		DrawKit_Border_Ex (
			border_Wall_none_0,
			OUTSET_TRUE,
			THIN_FALSE,
			BORDER_TRUE,
			THREED_TRUE,
			TRANSPARENT_FALSE,
			color3_form208,
			color3_black,
			KPA(sb.r_top)
		);
		break;
	}
	DrawKit_Rect_Arrow (KPA(sb.r_top), KP(is_horizontal) ? arrow4_left_0 : arrow4_up_2, /*arrow size*/ 7, color3_black);

bot:
	switch (FP(appearance)) {
	case appearance_flat_1:
		Draw_Rect	(KPA(sb.r_bottom), FP(scrollbarcolor), alpha_1_0);
		break;

	default:
		DrawKit_Border_Ex (
			border_Wall_none_0,
			OUTSET_TRUE,
			THIN_FALSE,
			BORDER_TRUE,
			THREED_TRUE,
			TRANSPARENT_FALSE,
			color3_form208 /*color3_form208*/,
			color3_black,
			KPA(sb.r_bottom)
		);
		break;
	}
	DrawKit_Rect_Arrow (KPA(sb.r_bottom), KP(is_horizontal) ? arrow4_right_1 : arrow4_down_3, /*arrow size*/ 7, color3_black);

track:
	switch (FP(appearance)) {
	case appearance_flat_1:
		Draw_Rect	(KPA(sb.r_track), FP(scrollbarcolor), alpha_1_0);
		break;

	default:
		Draw_Rect	(KPA(sb.r_track), color3_gray_25, alpha_1_0);
		break;
	}

thumb:
	switch (FP(appearance)) {
	case appearance_flat_1:
		Draw_Rect	(KPA(sb.r_thumb), FP(scrollbarthumbcolor), alpha_1_0);
		break;

	default:
		DrawKit_Border_Ex (
			border_Wall_none_0,
			OUTSET_TRUE,
			THIN_FALSE,
			BORDER_TRUE,	// This uses border color
			THREED_TRUE,	// Causes draw
			TRANSPARENT_FALSE, // Supposedly transparent false uses backcolor
			color3_form208,	// backcolor .. drawn for some including 3D
			color3_black,	// bordercolor, used when?
			KPA(sb.r_thumb)
		);
		break;
	} // sw
}

void O_ScrollBar_Action (ktrl k, int action, int is_sethook)
{
	float newvalue;
	switch (action) {
	default: return;
	case_break SB_ACTION_UP_0:		newvalue = KP(value) - 0.05;
	case_break SB_ACTION_DOWN_1:	newvalue = KP(value) + 0.05;
	case_break SB_ACTION_PGUP_2:	newvalue = KP(value) - 0.10;
	case_break SB_ACTION_PGDOWN_3:	newvalue = KP(value) + 0.10;
	case_break SB_ACTION_HOME_10:	newvalue = 0;
	case_break SB_ACTION_END_11:	newvalue = KP(maxvalue);
	} // sw

	Lister_Scrollbar_Property_Set_Float (k, newvalue);
	O_ScrollBar_Refresh (k);

	if (is_sethook && KP(ui.mouse_down_time))
		Form_PressHook_Set (KP(fctrl), k, O_ScrollBar_MouseHook_Fire, Host_RealTime() + SCROLLBAR_HOOK_REFIRE_0_1);
}

CALLERS_ (Form_Draw yikes!)
void O_ScrollBar_MouseHook_Fire (ktrl k)
{
	O_ScrollBar_Action (k, KP(ui.mouse_down_thingi), q_sethook_true);
}

// Return is important, null means mousepointer goes to default.
ktrl O_ScrollBar_MouseMove (ktrl k, int x, int y)
{
	float pct;
	if (KP(ui.mouse_down_thingi) == SB_ACTION_THUMB_4) {
		if (KP(is_horizontal)) {
			// Thumb moved
			int delta_x = x - KP(ui.mouse_down_origin.x); // 130 (now) - 200 (now) means -70 up
			int new_thumb_left = KP(ui.mouse_down_thumb_at_mousedown_px.x) + delta_x;
			vclamp (new_thumb_left, KP(sb.r_track.left), KP(sb.r_track.left) + KP(sb.track_empty_px));
			KP(ui.mouse_down_forced_thumb_pos.x) = new_thumb_left;
			int px_below_track_left = new_thumb_left - KP(sb.r_track.left);
			pct = px_below_track_left / (float)KP(sb.track_empty_px); //(k->sb.num_top_positions - 1);
			Lister_Scrollbar_Property_Set_Float (k, pct);
		} else {
			// Thumb moved
			int delta_y = y - KP(ui.mouse_down_origin.y); // 130 (now) - 200 (now) means -70 up
			int new_thumb_top = KP(ui.mouse_down_thumb_at_mousedown_px.y) + delta_y;
			vclamp (new_thumb_top, KP(sb.r_track.top), KP(sb.r_track.top) + KP(sb.track_empty_px));
			KP(ui.mouse_down_forced_thumb_pos.y) = new_thumb_top;
			int px_below_track_top = new_thumb_top - KP(sb.r_track.top);
			pct = px_below_track_top / (float)KP(sb.track_empty_px); //(k->sb.num_top_positions - 1);
			Lister_Scrollbar_Property_Set_Float (k, pct);
		}
		O_ScrollBar_Refresh (k);
	}
	return NULL;
}

// Coordinates are in vid_conwidth scale (screen for menu canvas)
ktrl O_ScrollBar_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	if (is_down == false) {
		// UP
		if (KFP(frm.kmousedown) == k && KP(ui.mouse_down_thingi) == SB_ACTION_THUMB_4) {
			O_ScrollBar_MouseMove (k, x, y); // Perform final move?
			O_ScrollBar_Refresh (k); // Thumb to final position
		}

		Form_PressHook_Clear (KP(fctrl), k);
		return NULL;
	}

mousedown:
	// This might be able to repeat, don't let it.
	if (KFP(frm.kmousedown) == k && KP(ui.mouse_down_thingi) == SB_ACTION_THUMB_4) {
		zpr ("Ignored thumb mousedown already down");
		return NULL;
	}

	// MOUSE DOWN
	if (RECT_HIT(KP(sb.r_top), x, y)) {
		KP(ui.mouse_down_time) = Host_RealTime();
		KP(ui.mouse_down_thingi) = SB_ACTION_UP_0;
		O_ScrollBar_Action (k, KP(ui.mouse_down_thingi), q_sethook_true);
	}

	else if (RECT_HIT(KP(sb.r_bottom), x, y)) {
		KP(ui.mouse_down_time) = Host_RealTime ();
		KP(ui.mouse_down_thingi) = SB_ACTION_DOWN_1;
		O_ScrollBar_Action (k, KP(ui.mouse_down_thingi), q_sethook_true);
	}

	else if (RECT_HIT(KP(sb.r_thumb), x, y)) {
		// Don't allow repeat!
		if (KP(ui.mouse_down_thingi) != SB_ACTION_THUMB_4) {
			KP(ui.mouse_down_time) = Host_RealTime();
			KP(ui.mouse_down_thingi) = SB_ACTION_THUMB_4; //is_above_thumb ? SB_ACTION_PGUP_2 : SB_ACTION_PGDOWN_3;

			KP(ui.mouse_down_origin.x) = x;
			KP(ui.mouse_down_origin.y) = y;
			KP(ui.mouse_down_thumb_at_mousedown_px.x) = KP(sb.r_thumb.left);
			KP(ui.mouse_down_thumb_at_mousedown_px.y) = KP(sb.r_thumb.top);
			KP(ui.mouse_down_thumb_into.x) = KP(ui.mouse_down_origin.x) - KP(sb.r_thumb.left);
			KP(ui.mouse_down_thumb_into.y) = KP(ui.mouse_down_origin.y) - KP(sb.r_thumb.top);
			KP(ui.mouse_down_forced_thumb_pos) = KP(ui.mouse_down_thumb_at_mousedown_px);
			//Con_PrintLinef ("Mousedown at %d", KP(ui.mouse_down_thumb_at_mousedown_px.y));
		} else {
			//Con_PrintLinef ("Thumb2 ignored");
		}
	}

	else if (RECT_HIT(KP(sb.r_track), x, y)) {
		int is_above_thumb; 
		if (KP(is_horizontal)) {
			is_above_thumb = x <= KP(sb.r_thumb.left);
		} else {
			is_above_thumb = y <= KP(sb.r_thumb.top);
		}
			
		KP(ui.mouse_down_time) = Host_RealTime();
		KP(ui.mouse_down_thingi) = is_above_thumb ? SB_ACTION_PGUP_2 : SB_ACTION_PGDOWN_3;
		O_ScrollBar_Action (k, KP(ui.mouse_down_thingi), q_sethook_true);

	}

	return NULL;
}



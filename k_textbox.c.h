// k_textbox.c.h

RELATED_ (Cursor_Recalc O_TabSelect_Draw, O_Label_Draw O_CheckBox_Draw )

ktrl O_TextBox_Refresh (ktrl k);

void O_TextBox_Finalize(ktrl k)
{
#ifndef QUAKEC // ENGINE ONLY
	//TextBox_SetTextMaster_Event (k, my_text);
	
	if (k->notext_a == NULL)
		k->notext_a = Z_StrDup ("");
	
	if (k->text_a == NULL)				// If we are lacking text, spawn it.
		k->text_a = Z_StrDup ("");

	if (k->have_ext_closer && KP(servo.kcloser) == NULL) {
		RELATED_ (O_ListView_Refresh) // <========== Sizing occurs

		ktrl kext /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_ext_closer, KPA(servo.kcloser),
			KP(fctrl), /*container*/ SERVANT_KCAN_NULL, /*servantto*/ k);
		Object_Property_Set (kext, "ToolTip", "Close");
	}
	if (k->have_ext_mover && KP(servo.kmover) == NULL) {
		ktrl kext /*used*/ =
		Object_Create_And_Assign_By_ClassEnum (class_ext_mover, KPA(servo.kmover),
		KP(fctrl), SERVANT_KCAN_NULL, k);
	}
#endif // !QUAKEC - ENGINE ONLY

	KP(txta._antibackcolor[0]) = bound(0, 1 - KP(backcolor[0]), 1);
	KP(txta._antibackcolor[1]) = bound(0, 1 - KP(backcolor[1]), 1);
	KP(txta._antibackcolor[2]) = bound(0, 1 - KP(backcolor[2]), 1);
	KP(txta._antiforecolor[0]) = bound(0, 1 - KP(forecolor[0]), 1);
	KP(txta._antiforecolor[1]) = bound(0, 1 - KP(forecolor[1]), 1);
	KP(txta._antiforecolor[2]) = bound(0, 1 - KP(forecolor[2]), 1);
}

RELATED_ (Object_Refresh_Recursive)

// k.r_screen has been updated and all draw properties known.
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF O_TextBox_KeyDown)
// Idea size is height = fontsize + 4 + 8 = fontsize + 12
ktrl O_TextBox_Refresh (ktrl k)
{
#ifndef QUAKEC // ENGINE ONLY
	if (KP(servo.kcloser) && KP(servo.kmover)) {
			// PERFORM ADJUSTMENTS
			int sizey = ifloor(KP(fontsize) * 0.8);
//			int inny = ifloor(sizey / 3.0);// ifloor(KP(fontsize) * 0.20);
			k->servo.kcloser->cm.relative_rect.width = sizey;		// PERFORM ADJUSTMENTS
			k->servo.kcloser->cm.relative_rect.left	= RECT_RIGHTOF(k->r_screen) - k->servo.kcloser->cm.relative_rect.width;// - sizey - inny;// * autocvar_magnification;

		k->servo.kmover->cm.relative_rect.width = k->r_screen.width - k->servo.kcloser->cm.relative_rect.width; //ifloor(k->r_screen.width / 2);
		k->servo.kmover->cm.relative_rect.left	= k->r_screen.left;// + ifloor(k->r_screen.width / 4);

		k->servo.kmover->cm.relative_rect.height = ifloor(KP(fontsize) * 0.8);
		k->servo.kmover->cm.relative_rect.top	= k->r_screen.top - k->servo.kmover->cm.relative_rect.height;

			k->servo.kcloser->cm.relative_rect.top	= k->servo.kmover->cm.relative_rect.top;
			k->servo.kcloser->cm.relative_rect.height = k->servo.kmover->cm.relative_rect.height;
	}
	else if (KP(servo.kcloser)) {
		// PERFORM ADJUSTMENTS
		int sizey = ifloor(KP(fontsize) * 0.8);
		int inny = ifloor(sizey / 3.0);// ifloor(KP(fontsize) * 0.20);
		k->servo.kcloser->cm.relative_rect.left	= RECT_RIGHTOF(k->r_screen);// - sizey - inny;// * autocvar_magnification;
		k->servo.kcloser->cm.relative_rect.width = sizey;
		k->servo.kcloser->cm.relative_rect.top	= k->r_screen.top + inny;
		k->servo.kcloser->cm.relative_rect.height = sizey;
	}
	else if (KP(servo.kmover)) {
		// PERFORM ADJUSTMENTS
		k->servo.kmover->cm.relative_rect.left	= k->r_screen.left;// + ifloor(k->r_screen.width / 4);
		k->servo.kmover->cm.relative_rect.width = k->r_screen.width; //ifloor(k->r_screen.width / 2);
		k->servo.kmover->cm.relative_rect.height = ifloor(KP(fontsize) * 0.8);
		k->servo.kmover->cm.relative_rect.top	= k->r_screen.top - k->servo.kmover->cm.relative_rect.height;
	}
#endif // !QUAKEC - ENGINE ONLY

	RECT_SET_CONTRACT (KP(r_interior), KP(r_screen), TXT_BORDER_WIDTH_2);	// Shrink by 4 (excludes border)
	RECT_SET_CONTRACT (KP(txta.r_textarea), KP(r_interior), TXT_MARGIN_4); // Shrink by 2

	TextBox_Cursor_Recalc (k);

	return k;
}

RELATED_ (TextBox_Cursor_Recalc)
// Cliparea cleared after all object draw.  If we were culled, this draw will not occur.
void O_TextBox_Draw (ktrl k)
{
	VarMyFont(myfont);
	DrawKit_Border_Ex ( // This draw fills.
		border_Wall_none_0,	// k.pp.border_walls,
		false,				// k.pp.cc.is_outset_border,
		false,				// k.pp.cc.is_thin_threed_border,
		true,				// k.pp.is_border,
		true,				// k.pp.is_threed,
		false,				// k.pp.is_transparent_background,
		KP(backcolor),		// k.pp._backcolorage.c4,
		color3_black,		// k.pp._bordercolorage.c4,			UNKNOWN
		KPA(r_screen)
	);

	//Draw_Rect			(KPA(txta.r_textarea), KP(backcolor), alpha_1_0);
	Draw_Clip_Set_Rect	(KPA(txta.r_textarea));

	// NO TEXT HINT?  EXAMPLE? Server browser has one.
#ifdef QUAKEC
	if_3 (strlen(KP(text_a)) == 0, strlen(KP(notext_a)), KIsFocused(k) == false) {
#else
	if_5 (KP(text_a), strlen(KP(text_a)) == 0, KP(notext_a), strlen(KP(notext_a)), KIsFocused(k) == false) {
#endif // QUAKEC - ENGINE
		Draw_StringXY		(KP(txta.r_textarea.left), KP(txta.r_textarea.top), KP(notext_a), KP(fontsize), KP(notext_color), alpha_1_0);
		return;
	}

	if (KIsFocused(k)) {
		if (TXT_SELCOUNTABS(k)) {
			Draw_StringXY		(KP(txta.seg1_left_px) - KP(r_scrollport.left), KP(txta.r_textarea.top), KP(txta.seg1_a), KP(fontsize), KP(forecolor), alpha_1_0);
			Draw_Rect_XYWH		(KP(txta.r_cursor_highlight.left) - KP(r_scrollport.left),
									KP(txta.r_cursor_highlight.top),
									KP(txta.r_cursor_highlight.width),
									KP(txta.r_cursor_highlight.height),

									KP(txta._antibackcolor), alpha_1_0);
			Draw_StringXY		(KP(txta.seg2_left_px) - KP(r_scrollport.left), KP(txta.r_textarea.top), KP(txta.seg2_a), KP(fontsize), KP(txta._antiforecolor), alpha_1_0);
			Draw_StringXY		(KP(txta.seg3_left_px) - KP(r_scrollport.left), KP(txta.r_textarea.top), KP(txta.seg3_a), KP(fontsize), KP(forecolor), alpha_1_0);
		}
		else {
			Draw_StringXY		(KP(txta.r_textarea.left) - KP(r_scrollport.left), KP(txta.r_textarea.top), KP(text_a), KP(fontsize), KP(forecolor), alpha_1_0);

		}
		int shall_not_blink = ifloor(Host_RealTime() *  4) & 2; // 4 = con_cursorspeed
		if (shall_not_blink || KP(txta.force_draw_cursor) > 0) {
			PCOLOR cursor_color = KP(txta.cursor_length) != 0 ? KP(backcolor) : color3_black;
			Draw_Rect_XYWH	(KP(txta.r_cursor.left) - KP(r_scrollport.left),
							KP(txta.r_cursor.top),
							KP(txta.r_cursor.width),
							KP(txta.r_cursor.height),
							cursor_color,
							alpha_1_0
							);
			KP(txta.force_draw_cursor) = 0;
		}
	} // focused so draw cursor.
	else {
		// NO FOCUS
		Draw_StringXY		(KP(txta.r_textarea.left) - KP(r_scrollport.left), KP(txta.r_textarea.top), KP(text_a), KP(fontsize), KP(forecolor), alpha_1_0);
	}
}


RELATED_ (M_ZForm_Key Form_KeyDown Z_StrDupf)



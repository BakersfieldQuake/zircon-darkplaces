// k_label.c.h - cannot focus and does not receive keyboard events

// nominally mouse reactive in the sense it that absorbs a mouse click or hover (maybe)
CALLERS_ (Object_Finalize_Recursive manual entry)
void O_Label_Finalize (ktrl k)
{
#ifndef QUAKEC // ENGINE
	// If we are lacking text, spawn it.
	if (k->caption_a == NULL)
		k->caption_a = Z_StrDup ("");
#endif // !QUAKEC
}

// k.r_screen has been updated and all draw properties known.
// 1. Solidify all positioning in screen space.
// 2. Update any final variables.
// 3. Note that focus change can cause a refresh.
RELATED_ (x = FIELDS_DEF)
ktrl O_Label_Refresh_Plus_Early (ktrl k)
{
	VarMyFont(myfont);
	KP(txta.stringwidthi_px) = Draw_StringWidthInt (myfont, KP(caption_a), KP(fontsize));

	KP(txta.r_textarea.width)	= KP(txta.stringwidthi_px);
	KP(txta.r_textarea.height)	= KFONTSIZECEIL(k);

	// Autosize forces minimal footprint.
	if (KP(autosize) || KP(cm.relative_rect.width) == 0 || KP(cm.relative_rect.height) == 0) {
		KP(cm.relative_rect.width) = KP(txta.r_textarea.width), KP(cm.relative_rect.height) = KP(txta.r_textarea.height);
	}

	Label_Rect_Edit_Align (KPA(txta.r_textarea), KPA(r_screen), (alignment_e) KP(alignment));

	return k;
} // O_Label_Refresh_Plus_Early

// screen clip has been set, render ...
void O_Label_Draw (ktrl k)
{
	VarMyFont(myfont);

	// May 11 2026 - Tooltips use this
	if (KP(backalpha)) {
		Draw_Rect	(KPA(r_screen), KP(backcolor), KP(backalpha));
	}
	Draw_String (KPA(txta.r_textarea), KP(caption_a), KP(fontsize), KP(forecolor), alpha_1_0);
} // O_Label_Draw


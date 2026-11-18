// k_ext_mover.c.h

RELATED_ (O_ListView_Refresh O_ListView_Spawn O_ListView_Refresh O_SizeDot_Refresh O_Image_Draw O_ScrollBar_Refresh O_Button_Refresh)

// k.r_screen has been updated and all draw properties known.
void O_ExtMover_Finalize (ktrl k)
{
	// Finalize can hit multiple times.

	// We are using this to detect initialization
	Form_Get (f,k);
	
	// This will not work for QC!
	// It doesn't even work in the engine.
//	if (FP(wasinitialized) != 0)
//		return;
	if (KP(direction) !=0 )
		return;

	// We have not been initialized.  This matters
	// because we let the Listview customize.
	HTMLColorToVector ("#432", k->backcolor);
	k->backalpha = 0.5;
	KP(direction) = alignment_middle_center_4;

	// If a sizer have ListView do alignment_bottom_right_8
}


ktrl O_ExtMover_Refresh (ktrl k)
{
	return k;
}

void O_ExtMover_Draw (ktrl k)
{
	//Draw_Rect	(KPA(r_screen), k->backcolor, k->backalpha);
	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_normal_inset_2);
	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_thin_inset_0);
	
	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_thin_outset_1); // LOOKS GRIPPY
	Draw_Rect	(KPA(r_screen), k->backcolor, k->backalpha);
	DrawKit_Button_Except_Face (KPA(r_screen), drawbut_command_thick_up_6); // LOOKS GRIPPY
}

RELATED_ (Form_MouseButtonAction O_ListView_ColumnDragContinue EditModeSetDotsObject)
ktrl O_ExtMover_PlacementDragContinue (ktrl k, int x, int y, int is_done)
{
	Form_Get (f,k);
	int		delta_x		= x - KP(ui.mouse_down_origin.x);
	int 	delta_y		= y - KP(ui.mouse_down_origin.y);
	ktrl kshadow = k->servant_owner;

	// Calculate new size of shadowed object based on right bottom for the moment.
	rect_s r_shadow_new;
	RECT_COPY (r_shadow_new, KP(ui.mouse_down_shadow_rect));

	Rect_Directional_Adjust (ADDRESSOF(r_shadow_new), (alignment_e) KP(direction), delta_x, delta_y);

	RECT_COPY (KXP(kshadow, cm.relative_rect), r_shadow_new); // ADJUSTMENT HERE SIZEOIC

	Form_QueueDotsRefresh (f); // Sure
	Form_QueueRefreshPlusMetric (f);

	// Anti-metric it
	if (f->ui.csqc_handle_plus1 == 0 && autocvar_magnification != 1) { // no magnix
		FP(frm.has_metric_run) = false; // KICK IT
		RELATED_ (Zircon_UI_ConsizeThink Object_Metric_Recursive)

		RECT_COPY (KXP(kshadow, cm._relative_rect_orig), KXP(kshadow, cm.relative_rect));
		float minfac = 1 / autocvar_magnification;
		KXP(kshadow, cm._relative_rect_orig.left) *= minfac;
		KXP(kshadow, cm._relative_rect_orig.top) *= minfac;
		KXP(kshadow, cm._relative_rect_orig.width) *= minfac;
		KXP(kshadow, cm._relative_rect_orig.height) *= minfac;
	}

	if (is_done) {
		KP(ui.mousepointer) = mousepointer_arrow_default_0; // O_SizeDot_PlacementDragEnd
	} else {
		KP(ui.mousepointer) = cursor_for_dir(KP(direction), isdown_true);
	}
	
	return k;
}

ktrl O_ExtMover_MouseMove (ktrl k, int x, int y)
{
	if (KFP(frm.kmousedown) == k)
		return O_ExtMover_PlacementDragContinue (k, x, y, isdone_false);

	KP(ui.mousepointer) = cursor_for_dir(KP(direction), isdown_false); // like mousepointer_size_both_10
	return k;
}

ktrl O_ExtMover_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	if (is_down == false) {
		// THIS IS MOUSE RELEASE (MOUSEUP WHEN WAS DOWN)
		KP(ui.mousepointer) = mousepointer_arrow_default_0;
		return O_ExtMover_PlacementDragContinue (k,x,y, isdone_true);
	}

	Form_Get (f, k);

	// MOUSEDOWN
	// O_SizeDot_PlacementDragStart

	// We need to zorder is bastard now
	ktrl kshadow = k->servant_owner;
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




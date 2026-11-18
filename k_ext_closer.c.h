// k_ext_closer.c.h

RELATED_ (O_SizeDot_Refresh O_Image_Draw O_ScrollBar_Refresh O_Button_Refresh)

RELATED_ (O_ListView_Spawn O_ListView_Refresh O_SizeDot_Refresh O_Image_Draw O_ScrollBar_Refresh O_Button_Refresh)

// k.r_screen has been updated and all draw properties known.
void O_ExtCloser_Finalize (ktrl k)
{
	HTMLColorToVector ("#432", k->backcolor);
	k->backalpha = 0.5;
	// Default to close.  Remember this function can currently hit twice due to QC needs.
	if (k->tooltip_a == NULL) {
		Object_Property_Set (k, "ToolTip", "Close");
	}
}

void O_ExtCloser_Draw (ktrl k)
{
	//Draw_Rect	(KPA(r_screen), k->backcolor, k->backalpha);
	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_normal_inset_2);
	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_thin_inset_0);

	//DrawKit_Button_Except_Face (KPA(r_screen), drawbut_thin_outset_1); // LOOKS GRIPPY
	Draw_Rect	(KPA(r_screen), k->backcolor, k->backalpha);
	DrawKit_Button_Except_Face (KPA(r_screen), drawbut_command_thick_up_6); // LOOKS GRIPPY
}

ktrl O_ExtCloser_Refresh (ktrl k)
{
	return k;
}

ktrl O_ExtCloser_MouseMove (ktrl k, int x, int y)
{
	//KP(ui.mousepointer) = mousepointer_prohibited_10;
	return k;
}

ktrl O_ExtCloser_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	Form_Get (f, k);
	if (KFP(frm.kmousedown) == k && is_down == false) {
		// RELEASE MEANS HIDE
		// FIRE A CLOSE EVENT

		if (k->servant_owner->eve.beforeclose_a && k->servant_owner->eve.beforeclose_a[0]) {
			exointfn_t beforeclose_fn = (exointfn_t)Event_Find_Function(k->servant_owner->eve.beforeclose_a);
			if (beforeclose_fn) {
				//int ret =
				beforeclose_fn (f);
			} else {
				Con_PrintLinef (CON_RED "Form_Event_Onload for " QUOTED_S "not found!", k->servant_owner->eve.beforeclose_a);
			}
		}

		k->servant_owner->is_hidden = true;
		Form_QueueRefresh (f);
	}

	return NULL;
}


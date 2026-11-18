// oject_ccb_object.c.h

RELATED_ (Object_Create_And_Assign)

// In engine, Form_Hit_Drawn_Any uses Oject_Will_Mouse
// to deny a collision.  QuakeC doesn't (yet?)
// Certain things like rects or polygons or even background
// images might end up being a problem in QC.
qbool Oject_Will_Mouse (ktrl k)
{
	if (KP(is_hidden))
		return false;
#ifdef QUAKEC
	if (KP(ct_will_mouse)) {
#else // ENGINE ...
	if (Have_Flag (k->po->w.oflags, OFCAN_MOUSE_4)) {
#endif // QUAKEC .. ENGINE
		// If not disabled, hidden or such ..
		return true;
	}

	return false;
}

void Object_Metric_Recursive (ktrl k) // 2
{
	Form_Get (f, k);

	if (k == f) {
#ifdef QUAKEC
		RECT_SET (FP(cm.relative_rect), 0, 0, gm.video_consize.x, gm.video_consize.y);
#else // ENGINE ...
		RECT_SET (FP(cm.relative_rect), 0, 0, vid.width, vid.height); // gm.video_consize.x, gm.video_consize.y);
#endif // QUAKEC .. ENGINE
		//goto form_skip;
	}

	// metric_recti_11: metric rect like cm.relative_rect
	else if (RECT_IS_ZERO(KP(cm._relative_rect_orig)) == false) {
		// metric_recti_11
		rect_s rect_val;
		RECT_COPY (rect_val, KP(cm._relative_rect_orig)); // k.cm.relative_rect
		//
		// Metric
		//

		// MAGNIFY
#ifndef QUAKEC // ENGINE
		if (!f->ui.csqc_handle_plus1) {
#endif // ! QUAKE - ENGINE ONLY
			// BOTH COME HERE
			RELATED_ ()
			rect_val.left	= floor(rect_val.left	* autocvar_magnification);
			rect_val.top	= floor(rect_val.top	* autocvar_magnification);
			rect_val.width	= floor(rect_val.width	* autocvar_magnification);
			rect_val.height	= floor(rect_val.height	* autocvar_magnification);

			if (rect_val.left < 0) {
				// NEGATIVE - Become max screen minus
#ifdef QUAKEC
				rect_val.left = gm.video_fullscreen_res.x /*vid width*/ + rect_val.left;
#else
				rect_val.left = vid.width + rect_val.left;
#endif // QUAKEC
			}
			if (rect_val.top < 0) {
				// NEGATIVE - Become max screen minus
				//printvarf (rect_val.top);
				//printvarf (ov.vid_height);
#ifdef QUAKEC
				rect_val.top = gm.video_fullscreen_res.y /*vid height*/ + rect_val.top;
#else
				rect_val.top = vid.height + rect_val.top;
#endif // QUAKEC
				//printvarf (rect_val.top);
			}
#ifndef QUAKEC // ENGINE
		} // no magnix csqc
#endif // ! QUAKE - ENGINE ONLY

		RECT_COPY (KP(cm.relative_rect), rect_val);
	}

	// metric_float_102

	if (KP(_fontsize_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(fontsize) = KP(_fontsize_orig);
		} else {
			KP(fontsize) = KP(_fontsize_orig) * autocvar_magnification;
		}
	} // _fontsize_orig

	if (KP(_imagesize_orig)) { // "devinfo" ButtonExit
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(imagesize) = KP(_imagesize_orig);
		} else {
			KP(imagesize) = KP(_imagesize_orig) * autocvar_magnification;
		}
	} // _imagesize_orig

	if (KP(_imagemargin_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(imagemargin) = KP(_imagemargin_orig);
		} else {
			KP(imagemargin) = KP(_imagemargin_orig) * autocvar_magnification;
		}
	} // _imagemargin_orig

	if (KP(_linewidth_orig)) {
		KP(linewidth) = KP(_linewidth_orig) * autocvar_magnification;
	}

	// mint32list_124  - the idea is that it can magnify
	//case_break prop_columnwidths:

	// metric_integer_119: Metric integer - the idea is that it can magnify
	if (KP(_gridcellheight_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(gridcellheight) = ifloor(KP(_gridcellheight_orig));
		} else {
			KP(gridcellheight) = floor(KP(_gridcellheight_orig) *autocvar_magnification);
		}
	} // _gridcellheight_orig

	if (KP(_gridcellpadding_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(gridcellpadding) = ifloor(KP(_gridcellpadding_orig));
		} else {
			KP(gridcellpadding) = floor(KP(_gridcellpadding_orig) *autocvar_magnification);
		}
	} // _gridcellpadding_orig

	if (KP(_gridcellwidth_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(gridcellwidth) = ifloor(KP(_gridcellwidth_orig));
		} else {
			KP(gridcellwidth) = floor(KP(_gridcellwidth_orig) *autocvar_magnification);
		}
	} // _gridcellwidth_orig

	if (KP(_gridrowheaderwidth_orig)) {
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(gridrowheaderwidth) = ifloor(KP(_gridrowheaderwidth_orig));
		} else {
			KP(gridrowheaderwidth) = floor(KP(_gridrowheaderwidth_orig) *autocvar_magnification);
		}
	} // _gridrowheaderwidth_orig

	// MUST ALWAYS HAVE A ROW HEIGHT IF LISTVIEW - BUT WE DO IT IN REFRESH
	if (KP(_rowheight_orig)) {
		// Baker: floor?  I would have chosen ceil - May 9 2026 - yeah, using ...
		if (f->ui.csqc_handle_plus1) { // no magnix
			KP(rowheight_rt) = iround(KP(_rowheight_orig));
			KP(rowheightpad_rt) = iround((KP(rowheight_rt) - KP(fontsize)) / 2.0);
		} else {
			// Baker: Issue ... rowheight needs to always be >= fontsize and fontsize is float 2.5 x something
			KP(rowheight_rt) = iround(KP(_rowheight_orig) *autocvar_magnification);
			KP(rowheightpad_rt) = iround((KP(rowheight_rt) - KP(fontsize)) / 2.0);
		}
	} // _rowheight_orig

	if (KP(_columnwidths_a_orig.count)) {
		int count = KP(_columnwidths_a_orig.count);
		int32s_freecontents (&k->columnwidths_a);
		for (int idx = 0; idx < count; idx ++) {
			int val = KP(_columnwidths_a_orig.ints[idx]);
			int write_val = val;
			if (f->ui.csqc_handle_plus1 == false) { // no magnix
				write_val = ifloor(val *autocvar_magnification);
			}
			int32s_add1 (&k->columnwidths_a, write_val);
		} // for idx
	} // _columnwidths_a_orig

metric_contained_controls:
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
// May 12 2026 - Should this be commented or not?  Engine has it commented, QuakeC == no
// I am going with refresh it even if hidden
//		if (KXP(kcontained, is_hidden)) continue; // Don't draw hidden.
		Object_Metric_Recursive (kcontained);
	for_each_end

	for_each_form_servant_owner_is_k (n, f, k, kservant)
//		if (KXP(kservant, is_hidden)) continue; // Don't draw hidden.
		Object_Metric_Recursive (kservant);
	for_each_end
} // Object_Metric_Recursive

CALLERS_ (Form_Finalize_Refresh self GLOBALS)
// Forms do come here.  Servants should come here.

void Object_Refresh_Recursive (ktrl k) // 2
{
	Form_Get (f, k);


	// Resist thinking of using console width for magnification.  Blurry.
	// Future: Why is blurry?
	const rect_s r_fullscreen = {0, 0, vid.width, vid.height};
	const rect_s *pr_container_clipped = k->container ? &k->container->r_interior_clipped : &r_fullscreen;

	if (KP(servant_owner)) {
		// Special case: Shared footprint with controller.
		if (k->servant_owner->container) {
			pr_container_clipped = &k->servant_owner->container->r_interior_clipped; // Form servant
		}
		else {
			//pr_container_clipped = &r_fullscreen;
			RECT_COPY (pr_container_clipped, ADDRESSOF(r_fullscreen));
		}
	}

variably_sized:
	// SOLIDIFY VARIABLE SIZED OBJECTS
	// refresh_reason_preautosize_1 - ask size before screen/interior sizes are calculated
	switch (KCLASS(k)) {
	default: // Doesn't have one
	case_break class_contextmenu:	O_ContextMenu_Refresh_Plus_Early(k);
	case_break class_imagegrid:		O_ImageGrid_Refresh_Plus_Early(k);
	case_break class_label:			O_Label_Refresh_Plus_Early(k);
	case_break class_tabselect:		O_TabSelect_Refresh_Plus_Early(k);
	} // sw

	RECT_COPY (KP(r_screen), KP(cm.relative_rect)); // k.r_screen = k.cm.relative_rect;
	if (KP(container)) {
		ktrl k_container = k->container;
		KP(r_screen.left)	+= KXP(k_container, r_interior.left);
		KP(r_screen.top)	+= KXP(k_container, r_interior.top);
	}

	SET___ RECT_COPY (KP(r_interior), KP(r_screen));  // k.r_interior = k.r_screen;

	//
	// Set clip areas
	//
#ifdef QUAKEC // Red outline
	k.draw_anyfocus = Oject_Has_Any_Focus_Set_Color(k);
#endif // QUAKEC
	rect_s r_highlight; 
	RECT_SET_EXPANDED (r_highlight, KP(r_screen), BOXWIDTH_2_3); // This is pixels not font size units :(

	// Calculate the focus area
	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(r_halo),
		SET___ KPA(r_halo_clipped),
		pr_container_clipped,
		RECT_SEND(r_highlight)
	);

	// Calculate the part we show
	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(r_screen),
		SET___ KPA(r_screen_clipped),
		pr_container_clipped,
		RECT_SEND(KP(r_screen))
	);

	// Calculate our interior
	Rect_Calc_Clipped_Unclipped_From_XYWH (
		SET___ KPA(r_interior),
		SET___ KPA(r_interior_clipped),
		pr_container_clipped, // This is allowed to be null
		RECT_SEND(KP(r_interior))
	);

	// Accumulate the draw
	while (1) {
		if_2 (CLIPPING_ON, KP(r_screen_clipped.width) == RECT_WIDTH_CULLED_ZERO)
			break; // CULLED FOOTPRINT

		// Add object to the drawn list
		int idx = BakerArray_NewIndex(k->fctrl->frm.drawn_list_a);
		k->fctrl->frm.drawn_list_a->pointers_array[idx] = k;

		break; // GET OUT
	} // while

	// Some of these solidify positioning of internals (relatively).
	switch (KCLASS(k)) {
	default: // Nothing (some do not do a refresh)
	case_break class_form_1:		// nothing for form
	case_break class_button:		O_Button_Refresh(k);
	case_break class_contextmenu:	O_ContextMenu_Refresh_Plus_Early(k);
	case_break class_checkbox:		O_CheckBox_Refresh(k);
	case_break class_imagegrid:		O_ImageGrid_Refresh_Plus_Early (k);
	case_break class_label:			O_Label_Refresh_Plus_Early(k);
	case_break class_listbox:		O_ListBox_Refresh (k);
	case_break class_listview:		O_ListView_Refresh(k);
	case_break class_polygon:		O_Polygon_Refresh(k);
	case_break class_rectangle:		O_Rectangle_Refresh(k);
	case_break class_scrollbar:		O_ScrollBar_Refresh(k);
	case_break class_tabselect:		O_TabSelect_Refresh_Plus_Early(k);
	case_break class_textbox:		O_TextBox_Refresh(k);
	} // sw

	if (FP(frm.refreshdots) && FP(frm.editmode)) {
		if (k == FP(frm.kfocused)) {
			EditModeSetDotsObject (FP(frm.kfocused));
#ifndef QUAKEC // engine only
			Oject_Set_Infobar (f->frm.kfocused);
#endif // !QUAKEC
			f->frm.refreshdots = 0;
		}
	}

refresh_contained_controls:
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		//if (KXP(kcontained, is_hidden)) continue; // Don't draw hidden.
		Object_Refresh_Recursive (kcontained);
	for_each_end

	for_each_form_servant_owner_is_k (n, f, k, kservant)
		//if (KXP(kservant, is_hidden)) continue; // Don't draw hidden.
		Object_Refresh_Recursive (kservant);
	for_each_end
}

// Baker: Finalize is a chance to set variables after
// all properties have been read.  Even hidden and disabled
// controls need to do this.
// ONLY list those that actually need it.

// Finalize: All fields have been loaded and all values are known (r_screen positioning is unknown here).
// This is a chance for an object to finalize any missing data or default values that have not been set (column widths).
// 1. Even the form comes here.
// 2. Finalize is the first and only place that all final values have been read from file (file load) or memory (hard code).
// 2. r_screen has not been calculated
// 3. Example usage: default the widths of each column
void Object_Finalize_Recursive (ktrl k) // 1
{
	Form_Get(f,k);

	// Baker: Let control type look to see what final values need checked.
	// Baker: Engine: creates empty strings for unset text fields
	switch (KCLASS(k)) {
	default:				//
	case_break class_button:	O_Button_Finalize(k);		// Engine zdups something
	case_break class_ext_closer:O_ExtCloser_Finalize(k);	// Set servant values in also QC friendly way.
	case_break class_ext_mover: O_ExtMover_Finalize(k);		// Set servant values in also QC friendly way.
	case_break class_label:		O_Label_Finalize(k);		// Engine zdups something
	case_break class_listview:	O_ListView_Finalize(k);		// Columnwidth cleanup
	case_break class_textbox:	O_TextBox_Finalize(k);		// calcs "anti-color" for highlighting text
	} // sw

finalize_contained_controls:

	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		//if (KXP(kcontained, is_hidden)) continue; // Don't draw hidden.
		Object_Finalize_Recursive (kcontained);
	for_each_end

	for_each_form_servant_owner_is_k (n, f, k, kservant)
		//if (KXP(kservant, is_hidden)) continue; // Don't draw hidden.
		Object_Finalize_Recursive (kservant);
	for_each_end

	FP(cm.is_finalized) = true;
}

CALLERS_ (Form_Dump)
// Baker: Feb 11 2025 - This accumulates a string list.
// pidx allows sequential numbering.
void Object_Dump_Recursive (ktrl k, stringlist_t *plist, int *pidx, dump_detail_e detail_level, mag_s *magni_opt)
{
	Form_Get(f,k);
	char *s_ot = k->po->name;

	// Name and Rect Dump
	if (detail_level == DUMP_DETAIL_NAMES_ONLY_3) {
		if (KCLASS(k) != class_form_1) {
			char valuebuf[1024];

			qbool ok = UI_Fly (k->cm.name_a, "name", valuebuf, sizeof(valuebuf));
			if (ok == false) {
                Con_PrintLinef ("Couldn't expand variable");
			}
			stringlistappendf (plist, "%-20.20s " CON_BRONZE "%s", valuebuf, s_ot);
		}
		goto list_only_skip;
	}

	if (pidx) {
		stringlistappendf (plist, "%s // %d", s_ot, *pidx);
		(*pidx) ++;
	} else {
		stringlistappendf (plist, "%s", s_ot);
	}
	if (KCLASS(k) == class_form_1) {
		// Form does not print name or rect or container.
	} else {
		Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_name), plist, q_indent_true, magni_opt, detail_level);
		//
		if (detail_level != DUMP_DETAIL_NAMES_ONLY_3) {
			Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_container), plist, q_indent_true, magni_opt, detail_level);
			Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_rect), plist, q_indent_true, magni_opt, detail_level);
		}
	}

	// Property Dump
	//if (detail_level != DUMP_DETAIL_NAMES_ONLY_3) {
		for (things_s *p = &things[0]; p->name; p ++) {
			if (false == in_range (prop_rect + 1, p->enum_id, prop_end))
				continue;

			// Baker: Feb 11 2025 - less spammy for label
			if (isin1 (KCLASS(k), class_label)) {
				if (isin3 (p->enum_id, prop_backalpha, prop_backcolor, prop_forecolorselected)) {
					continue;
				}
			}

			Property_Dump_Maybe (k, p, plist, q_indent_true, magni_opt, detail_level);
		} // for each property
	//}
	// Event Dump
	//for (things_s *p = &things[0]; p->name; p ++) {
	//	if (false == in_range (events_start, p->enum_id, events_end))
	//		continue;

	//	Event_Dump_Maybe (k, p, plist, q_indent_true);
	//} // for each property

	if (detail_level == DUMP_DETAIL_PLUS_CLIPPING_2) {
		stringlistappendf (plist, "  " "r_halo " RECTI_PRINTF, RECT_SEND(k->r_halo));
		stringlistappendf (plist, "  " "r_halo_clipped " RECTI_PRINTF, RECT_SEND(k->r_halo_clipped));
		stringlistappendf (plist, "  " "r_screen " RECTI_PRINTF, RECT_SEND(k->r_screen));
		stringlistappendf (plist, "  " "r_screen_clipped " RECTI_PRINTF, RECT_SEND(k->r_screen_clipped));
		stringlistappendf (plist, "  " "r_interior " RECTI_PRINTF, RECT_SEND(k->r_interior));
		stringlistappendf (plist, "  " "r_interior_clipped " RECTI_PRINTF, RECT_SEND(k->r_interior_clipped));
	}

list_only_skip:

	if (detail_level == DUMP_DETAIL_UNCONTAINED_4 && IsFormTop(k) == false) {
		// Top level only, no recurse into containers.
		// Except for Form, of course otherwise only it prints.
		return;
	}

	if (detail_level == DUMP_DETAIL_NORECURSE_6) {
		// Top level only, no recurse into containers.
		// Except for Form, of course otherwise only it prints.
		return;
	}

	// Dump contained
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		// Servants are internal and do not write to file
		Object_Dump_Recursive (kcontained, plist, pidx, detail_level, magni_opt);
	for_each_end

	if (isin2 (detail_level, DUMP_DETAIL_PLUS_SERVANTS_1, DUMP_DETAIL_PLUS_CLIPPING_2)) {
		for_each_form_servant_owner_is_k (n, f, k, kservant)
			Object_Dump_Recursive (kservant, plist, pidx, detail_level, magni_opt);
		for_each_end
	} // detail plus servants

	// stringlistappendf (plist, "//End");
} // Object_Dump_Recursive

void Object_Dump_Recursive2 (ktrl k, stringlist_t *plist2, int *pidx, dump_detail_e detail_level, mag_s *magni_opt)
{
	Form_Get(f,k);
	char *s_ot = k->po->name;

	// Name and Rect Dump
	if (detail_level == DUMP_DETAIL_NAMES_ONLY_3) {
		if (KCLASS(k) != class_form_1) {
			char namebuf[1024];

			qbool ok = UI_Fly (k->cm.name_a, "name", namebuf, sizeof(namebuf));
			if (ok == false) {
                Con_PrintLinef ("Couldn't expand variable");
			}
			stringlistappendf (plist2, "%s " CON_BRONZE "%s", namebuf, s_ot);
		}
		goto list_only_skip;
	}

#if 0 // May 8
	if (pidx) {
		// May 8 2026 - always null
		stringlistappendf (plist2, "%s // %d", s_ot, *pidx);
		(*pidx) ++;
	} else {
		// May 8 2026 - not for us
		//stringlistappendf (plist2, "%s", s_ot);
	}
#endif

	if (KCLASS(k) == class_form_1) {
		// Form does not print name or rect or container.
	} else {
		Property_Dump_Maybe2 (k, Thing_Find_By_Enum_Id(prop_name), plist2, q_indent_true, magni_opt, detail_level);
		//
		if (detail_level != DUMP_DETAIL_NAMES_ONLY_3) {
			Property_Dump_Maybe2 (k, Thing_Find_By_Enum_Id(prop_container), plist2, q_indent_true, magni_opt, detail_level);
			Property_Dump_Maybe2 (k, Thing_Find_By_Enum_Id(prop_rect), plist2, q_indent_true, magni_opt, detail_level);
		}
	}

	// Property Dump
	//if (detail_level != DUMP_DETAIL_NAMES_ONLY_3) {
		for (things_s *p = &things[0]; p->name; p ++) {
			if (false == in_range (prop_rect + 1, p->enum_id, prop_end))
				continue;

			// Baker: Feb 11 2025 - less spammy for label
			if (isin1 (KCLASS(k), class_label)) {
				if (isin3 (p->enum_id, prop_backalpha, prop_backcolor, prop_forecolorselected)) {
					continue;
				}
			}

			Property_Dump_Maybe2 (k, p, plist2, q_indent_true, magni_opt, detail_level);
		} // for each property
	//}
	// Event Dump
	//for (things_s *p = &things[0]; p->name; p ++) {
	//	if (false == in_range (events_start, p->enum_id, events_end))
	//		continue;

	//	Event_Dump_Maybe (k, p, plist, q_indent_true);
	//} // for each property

	if (detail_level == DUMP_DETAIL_PLUS_CLIPPING_2) {
		stringlistappendf (plist2, "  " "r_halo " RECTI_PRINTF, RECT_SEND(k->r_halo));
		stringlistappendf (plist2, "  " "r_halo_clipped " RECTI_PRINTF, RECT_SEND(k->r_halo_clipped));
		stringlistappendf (plist2, "  " "r_screen " RECTI_PRINTF, RECT_SEND(k->r_screen));
		stringlistappendf (plist2, "  " "r_screen_clipped " RECTI_PRINTF, RECT_SEND(k->r_screen_clipped));
		stringlistappendf (plist2, "  " "r_interior " RECTI_PRINTF, RECT_SEND(k->r_interior));
		stringlistappendf (plist2, "  " "r_interior_clipped " RECTI_PRINTF, RECT_SEND(k->r_interior_clipped));
	}

list_only_skip:

	if (detail_level == DUMP_DETAIL_UNCONTAINED_4 && IsFormTop(k) == false) {
		// Top level only, no recurse into containers.
		// Except for Form, of course otherwise only it prints.
		return;
	}

	if (detail_level == DUMP_DETAIL_NORECURSE_6) {
		// Top level only, no recurse into containers.
		// Except for Form, of course otherwise only it prints.
		return;
	}

	// Dump contained
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		// Servants are internal and do not write to file
		Object_Dump_Recursive2 (kcontained, plist2, pidx, detail_level, magni_opt);
	for_each_end // contained

	if (isin2 (detail_level, DUMP_DETAIL_PLUS_SERVANTS_1, DUMP_DETAIL_PLUS_CLIPPING_2)) {
		for_each_form_servant_owner_is_k (n, f, k, kservant)
			Object_Dump_Recursive2 (kservant, plist2, pidx, detail_level, magni_opt);
		for_each_end
	} // detail plus servants

	// stringlistappendf (plist, "//End");
} // Object_Dump_Recursive


qbool Oject_Will_Focus_Currently (ktrl k)
{
	// Visible.  Enabled.
	if (KP(servant_owner))
		return false; // Servants are mouse interactive only and cannot get focus

	if (KP(is_hidden))
		return false;

	if (KP(is_disabled))
		return false;

#ifdef QUAKEC
	if (KIsFocusableType(k)) // Engine equiv: Have_Flag (k.po->w.oflags, OFCAN_FOCUS_2)
#else // ENGINE ...
	if (Have_Flag (k->po->w.oflags, OFCAN_FOCUS_2))
#endif // QUAKEC
		return true;

	return false;
}

ktrl Oject_Focus_Set (ktrl k)
{
	ktrl f = KP(fctrl);

	if (KIsFocused(k))
		return k; // Nothing to do

	// Examine current focused object
	ktrl kfok = FP(frm.kfocused);
	if (kfok) {
		// About to lose focus
		// CONTEXTMENU exists only when focused ..
		if (Have_Flag (kfok->po->w.oflags, OFCAN_ONLYFOCUS_128)) {
			// Context menu or something that can only exist with focus
			kfok->is_hidden = true; // CONTEXTOID MENU HIDE 2 <=== LOSTFOCUS (LEAVE THIS HERE WE IN FOCUS SET FUNC!)
			if (f->frm.kpopup) {
				// Context Leaving has not been handled yet.
				O_ContextMenu_Leaving (kfok, /*value*/ NULL); // Oject_Focus_Set (AWAY)
			}

		} // Context menu auto-hide
	}

	// SET
	FP(frm.kfocused) = k; // Baker: This is not a contextmenu but oldfocus if applicable.
	if (FP(frm.editmode)) {
		Form_QueueRefresh (f); // Jan 16 2025 - Helps?
		Form_QueueDotsRefresh (f);
		//EditModeSetDotsObject (f);
	}

	return f;
}

ktrl Object_Draw_Recursive_Not_Form (ktrl k)
{
	Form_Get(f,k);

	// OBJECT ENTIRELY CULLED
	if (CLIPPING_ON && KP(r_halo_clipped.width) == RECT_WIDTH_CULLED_ZERO)
		return NULL;  // CULLED HIGHLIGHT

	int have_focus = KIsFocused(k);

	// DRAW HIGHLIGHT =============================================================
	if (have_focus && Have_Flag(k->po->w.oflags, OFCAN_NOHIGHLIGHT_64) == 0 && KIsFocusDrawn(k) ) {
		if (CLIPPING_ON)
			Draw_Clip_Set_Rect (&k->r_halo_clipped);

		Draw_Box (&k->r_halo, BOXWIDTH_2_3, color3_gray_50, alpha_1_0);
	} // anyfocus

	// DRAW CONTROL =============================================================
	if (CLIPPING_ON && KP(r_screen_clipped.width) == RECT_WIDTH_CULLED_ZERO) {
		return NULL; // CULLED FOOTPRINT
	}

	if (CLIPPING_ON) {
		Draw_Clip_Set_Rect (KPA(r_screen_clipped)); // Submits draw buffer as side-effect.
	}

	// DRAW CONTROL
	switch (KCLASS(k)) {
	default:
	case_break class_button:		O_Button_Draw(k);
	case_break class_checkbox:		O_CheckBox_Draw(k);
	case_break class_contextmenu:	O_ContextMenu_Draw(k);
	case_break class_ext_closer:	O_ExtCloser_Draw(k);
	case_break class_ext_mover:		O_ExtMover_Draw(k);

	//case_break class_gridview:	O_GridView_Draw(k);
	case_break class_image:			O_Image_Draw(k);
	case_break class_imagegrid:		O_ImageGrid_Draw(k);
	case_break class_label:			O_Label_Draw(k);
	case_break class_listbox:		O_ListBox_Draw(k);
	case_break class_listview:		O_ListView_Draw(k);
	case_break class_polygon:		O_Polygon_Draw(k);
	case_break class_rectangle:		O_Rectangle_Draw(k);
	case_break class_scrollbar:		O_ScrollBar_Draw(k);
	case_break class_sizedot:		O_SizeDot_Draw (k);
	case_break class_tabselect:		O_TabSelect_Draw(k);
	case_break class_textbox:		O_TextBox_Draw(k);
	} // sw

	// RECURSE CONTAINED
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		if (KXP(kcontained, is_hidden)) continue; // Don't draw hidden.
		Object_Draw_Recursive_Not_Form (kcontained);
	for_each_end

	// ZEBRAZ
	for_each_form_servant_owner_is_k (n, f, /*servant owner*/ k, kservant)
		if (KXP(kservant, is_hidden)) continue; // Don't draw hidden.
		if_2 (KXP(kservant, servant_owner), KXP(kservant, servant_owner->is_hidden))
			continue;
		Object_Draw_Recursive_Not_Form (kservant);
	for_each_end

	if (CLIPPING_ON) {
		Draw_Clip_Clear (); // Submits draw buffer as side-effect.
	}
	return k;
}


static void _RunDefaultString (ktrl k, const char *s_string)
{
	stringlist_t	kdeflist = {0};

	stringlistappend_tokenize_qcquotes (&kdeflist, /*linenum list*/ NULL, s_string);

	// MaxValue:100
	const char **wordray = stringlist_nullterm_add (&kdeflist);
	for (/*nada*/ ; *wordray; *wordray ++) {
		const things_s *thing = Thing_Find (*wordray);


		if (!thing) {
			ccs *s = *wordray;
			Con_PrintLinef (QUOTED_S " Unknown keyword", s);
			break;
		}

		if (!IsProperty(thing)) {
			ccs *s = *wordray;
			Con_PrintLinef (QUOTED_S " is not a property", s);
			break;
		}

		//c_assert_msg_ (IsProperty(thing), "property is messed");


		// Advance past property
		*wordray ++; Word_Check_Fail_Message ("No more words after property");

		c_assert_msg_ (String_Match(*wordray, PROPERTY_COLON_DELIM), "no colon after property is messed");

		// Advance past semi-colon
		*wordray ++; Word_Check_Fail_Message ("No more words after property colon");

		// PROCESS PROPERTY
		ccs *s_value = *wordray;

		qbool is_ok;
		is_ok = Property_Set_By_String_After_Freeing_Is_Ok (k, thing, s_value, NULL);
	} // for words
	stringlistfreecontents (&kdeflist);
}

// Relatively many callers
EXO___ int Object_Property_Set(ktrl k, ccs *s_property_name, ccs *s_propertyvalue_unquoted)
{
	const things_s *po = Thing_Find(s_property_name);
	if (!po)
		return IERR_PROP_NOT_FOUND_1;

	int isok = Property_Set_By_String_After_Freeing_Is_Ok(k, po, s_propertyvalue_unquoted, NULL);
	if (!isok)
		return IERR_PROP_SET_ERROR_2;
	return OK_NOERROR_0;
}

 // text is s_propertyvalue_unquoted
// Relatively few callers
EXO___ int Object_Property_Set_Fmt(ktrl k, ccs *s_property_name, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	int ret = Object_Property_Set(k, s_property_name, text);
	VA_EXPAND_ALLOC_FREE (text);

	return ret;
}

//CALLERS_ ()
//EXO___ int Object_Set_Float(ktrl k, const things_s *p, float fval)
//{
//	const things_s *p = Thing_Find(s_property_name);
//	if (!p) return IERR_PROP_NOT_FOUND_1;
//	qbool isok = Object_Property_Set_Float(k, p, fval);
//	if (!isok) return IERR_PROP_SET_ERROR_2;
//	return OK_NOERROR_0;
//}

void Object_Class_Default_Values (ktrl k, const things_s *po)
{
	if (po->sdefault == NULL || po->sdefault[0] == CHAR_NULL_0)
		return; // No defaults

	_RunDefaultString (k, po->sdefault);
}

void Object_Global_Default_Values (ktrl k, const things_s *po)
{
	if (KCLASS(k) != class_form_1) {
		const things_s *poform = k->fctrl->po;
		_RunDefaultString (k, poform->sdefault);
	}
}

// Occurs in parser when a new object is about to be created or after last parse.
CALLERS_ (Object_Parse_Done)
ktrl Object_CheckNameCollide (ktrl kfind)
{
	Form_Get(f,kfind);
	for_each_form_all_controls(n,f,k) // ALL CONTROLS!
		if (k == kfind)
			continue; // It's us!
		if (String_Match (KP(cm.name_a), KXP(kfind, cm.name_a))) {
			return k;
		}
	for_each_end
	return NULL;
} // Object_CheckNameCollide

ktrl Object_Parse_Done (ktrl k)
{
	// WARN ABOUT NAME COLLISION, but do nothing ...
	ktrl k_name_collide = Object_CheckNameCollide (k);

	if (k_name_collide) {
		zprf (CON_MAGENTA "Form_Create: Duplicate name detected " QUOTED_S, KP(cm.name_a));
	}

	// We don't do anything right now.
	return k;
}

// Does a form come here? YES -> Form_Life
CALLERS_ (Form_Life)
// Object_Create_And_Assign / Object_CreateEx
ktrl Object_Create_And_Assign (ktrl *pk, ktrl f, ktrl container, ktrl servant_to, const things_s *po)
{
	if (*pk) {
		c_assert_msg_ (0, "Object_Create_And_Assign object already exists!");
	}

	LIFE_ALLO___ ktrl k = (ktrl )ZAlloc_VoidP_SizeOf (oject_s);

//	if (po->enum_id == class_form_1)
//		int j = 5;

	k->po				= po;
	k->fctrl			= f;
	k->container		= container;
	k->servant_owner	= servant_to;
	k->cm.creation_idx  = (po->enum_id == class_form_1) ? 0 :f->cm.creation_idx ++; // k is not a form here.

	// DEFAULT a NAME
	const things_s *po_name = Thing_Find_By_Enum_Id (prop_name);

	if (KCLASS(k) == class_form_1) {
		Property_Set_By_String_After_Freeing_Is_Ok (k, po_name, "Form1", NULL);
	} else {
		char *sz = Mem_strdupf(devinfomempool, "%s%d", k->po->name, k->cm.creation_idx);
		Property_Set_By_String_After_Freeing_Is_Ok (k, po_name, sz, NULL);
		Z_FreeNull_ (sz);
	}

	// FORM
	if (po->enum_id != class_form_1) {
	// FORM EXCLUDED: ADD K TO FORM COLLECTION
//collection_add_not_form:
		int idx = BakerArray_NewIndex(f->frm.controls_a);
		f->frm.controls_a->pointers_array[idx] = k;
		//k->cm.controlindex = idx; NO!  It can change during run-time.
	} else {
		//k->cm.controlindex = not_found_neg1;
	}

	Object_Global_Default_Values (k, po); // Runs the control type default string
	if (KCLASS(k) != class_form_1) {
		RELATED_ (THEME_DEFAULTS)
inherit_from_modified_form_value:
		VectorCopyDestSrc  (k->forecolor, f->forecolor); // Black
		VectorCopyDestSrc  (k->backcolor, f->backcolor); // White
		VectorCopyDestSrc  (k->backcolorselected, f->backcolorselected); // Black
		VectorCopyDestSrc  (k->forecolorselected, f->forecolorselected); // White
		k->fontsize = f->fontsize;// 24; // Default 24 for now
	}

	Object_Class_Default_Values (k, po); // Object type

//EXAMPLE: IH	{"BackColor",			prop_backcolor,			vtrgb_21,		member_offsetof (form1, backcolor),		},

spawn_servants:

	switch (KCLASS(k)) {
	default: // Nothing
	//case class_form_1:	Form_ContextMenu_Spawn (k);	break; // NO!  TOO EARLY NO control array yet!
	case_break class_imagegrid:	O_ImageGrid_Spawn(k);
	case_break class_listbox:	O_ListBox_Spawn (k);
	case_break class_listview:	O_ListView_Spawn (k);
	} // sw

	(*pk) = k; // return k;
	return k;
} // Object_Create_And_Assign / Object_CreateEx

ktrl Form_Find_Name (ktrl f, PSTRING s_name_wanted)
{
	if (String_Match (FP(cm.name_a), s_name_wanted))
		return f; // Check if it is the form first.
	for_each_form_all_controls (n,f,k)
		//if (KP(cm.name_a)) {
			if (String_Match(KP(cm.name_a), s_name_wanted))
				return k;
		//} else {
		//	c_assert_msg_ (0, "Object have no name");
		//}
	for_each_end
	return NULL;
}

// Jan 24 2026 - kdots come here
ktrl Object_Create_And_Assign_By_ClassEnum (int class_id, ktrl *pk, ktrl f, ktrl container, ktrl servant_to)
{
	const things_s *po = Thing_Find_By_Enum_Id (class_id);
	return Object_Create_And_Assign (pk, f, container, servant_to, po);
}

// FORM does not come here to draw
CALLERS_ (Form_Draw)

RELATED_ (Label_Rect_Edit_Align)

void Oject_Set_Infobar (ktrl k)
{
	Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Object Name: %s @ %d, %d .. %d, %d (%d x %d)\" silent",
		k->cm.name_a,
		k->r_screen.left,
		k->r_screen.top,
		k->r_screen.left + k->r_screen.width,
		k->r_screen.top + k->r_screen.height,
		k->r_screen.width,
		k->r_screen.height
	);
}


CALLERS_ (Form_Dump_Single_Line)
void Object_Dump_Less_Recursive (ktrl k, stringlist_t *pmaster, mag_s *magni_opt)
{
	dump_detail_e detail_level = DUMP_DETAIL_SINGLE_LINE_5;
	qbool shall_indent = q_indent_false;
	Form_Get(f,k);
	char *s_ot = k->po->name;

	stringlist_t list = {0};

	stringlistappendf (&list, "%s", s_ot); // Object type

	if (KCLASS(k) != class_form_1) {
		// Form does not print name or rect
		Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_name), &list, shall_indent, magni_opt, detail_level);

		if (detail_level != DUMP_DETAIL_NAMES_ONLY_3) {
			Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_container), &list, shall_indent, magni_opt, detail_level);
			Property_Dump_Maybe (k, Thing_Find_By_Enum_Id(prop_rect), &list, shall_indent, magni_opt, detail_level);
		}
	}

	// Property Dump - this prints properties after name, container, rect ...
	for (things_s *p = &things[0]; p->name; p ++) {
		if (false == in_range (prop_rect + 1, p->enum_id, prop_end))
			continue;

		// Baker: Feb 11 2025 - less spammy for label
		if (isin1 (KCLASS(k), class_label) &&
			isin3 (p->enum_id, prop_backalpha, prop_backcolor, prop_forecolorselected))
				continue;

		Property_Dump_Maybe (k, p, &list, shall_indent, magni_opt, detail_level);
	} // for each property

	char *sline_za = stringlist_join_spaced_zalloc (&list);
	stringlistfreecontents (&list);

	stringlistappend (pmaster, sline_za);
	Mem_FreeNull_ (sline_za);

	// Dump contained but not servants, which are internal and do write to file.
	for_each_form_control_with_container_as_no_servants (n, f, k, kcontained)
		Object_Dump_Less_Recursive (kcontained, pmaster, magni_opt);
	for_each_end

}




// Does not touch form control array
CALLERS_ (Form_Life)

// Q: Does a form come here? Yes. Last.
ktrl Object_Destroy_Not_Recursive (ktrl k)
{
	// Find each property.
	for (things_s *p = &things[0]; p->name; p ++) {
		if (IsProperty (p) == false) continue;
		Property_Destroy (k, p);
	}

	NPA__ int32s_freecontents (&k->columnwidths_a);
	NPA__ int32s_freecontents (&k->stringwidths_rt_a);
	
	Mem_FreeNull_ (k->txta.seg1_a);
	Mem_FreeNull_ (k->txta.seg2_a);
	Mem_FreeNull_ (k->txta.seg3_a);


	// zebra_servants OBJECTS get destroyed with form.
	// We need to destroy the array.
	//Mem_FreeNull_ (k->servo.zebra_servants);
	NPA__ LIFE_FREE___ /*oject_s*/ Mem_FreeNull_ (k);
	return k;
}





CALLERS_ (O_ContextMenu_Activate_At)
ktrl Oject_Hidden_Set (ktrl k, int newvalue)
{
	if (newvalue == (int)true && KIsFocused (k)) {
		Form_Focus_Next (k->fctrl, DIR_FORWARD_1);
		if (KIsFocused (k)) {
#if 1
			Oject_Focus_Set (k->fctrl); // Route all focus through ...
#else
			k->fctrl->frm.kfocused = k->fctrl; // Set focus to form if all else fails
#endif
		}
	}

	k->is_hidden = newvalue;
	return k;
}

CALLERS_ (Form_Finalize_Refresh related to Oject_Focus_Set sForm_Get_Focused_Idx sFind_Next_Prev_Focus_Control)
CALLERS_ (Form_MouseButtonAction)




// kz_form_misc.c.h

CALLERS_ (/*Object_Parse_Done*/  Form_Create_From_String /*Finalize Refresh*/)
RELATED_ (Object_Event_OnChange_Recursive)
void Form_Event_Onload (ktrl f)
{
	if (f->eve.onload_a && f->eve.onload_a[0]) {
		exofn_t onload_fn = Event_Find_Function(f->eve.onload_a);
		if (onload_fn) {
			char *s_error_za = onload_fn (f);
			if (s_error_za) {
				Con_PrintLinef ("ERROR: %s: " QUOTED_S, f->eve.onload_a, s_error_za);
				Z_FreeNull_ (s_error_za);
			}
		} else {

			Con_PrintLinef (CON_RED "Form_Event_Onload for " QUOTED_S "not found!", f->eve.onload_a);
		}
	}

	Object_Event_OnChange_Recursive (f, shallrecurse_true); // Form ONLOAD
} // Form_Event_Onload


void Form_Print_Zones(ktrl f)
{
	for (int j = 0; j < f->frm.drawn_list_a->numitems; j ++) {
		ktrl k = f->frm.drawn_list_a->pointers_array[j];

		Con_PrintLinef ("%4d " S_FMT_LEFT_PAD_20 " r = " RECTI_4PRINTF " clipped r = " RECTI_4PRINTF,
			j, k->po->name, RECT_SEND(k->cm.relative_rect), RECT_SEND(k->r_screen_clipped));
	} // for
}

void Form_Dump (ktrl f, stringlist_t *plist, dump_detail_e detail_level, mag_s *magni_opt)
{
	int idx = 0;
	Object_Dump_Recursive (f, plist, &idx, detail_level, magni_opt);
	extern cvar_t scr_xtranslate; if (scr_xtranslate.string[0]) { Cvar_SetQuick (&scr_xtranslate, ""); }
}

void Form_Dump2 (ktrl f, stringlist_t *plist, dump_detail_e detail_level, mag_s *magni_opt)
{
	int idx = 0;
	Object_Dump_Recursive2 (f, plist, &idx, detail_level, magni_opt);
	extern cvar_t scr_xtranslate; if (scr_xtranslate.string[0]) { Cvar_SetQuick (&scr_xtranslate, ""); }
}

CALLERS_ (Form_Single_Line_To_String_Zalloc _zcmdObjectCommand)
void Form_Dump_Single_Line (ktrl f, stringlist_t *plist, dump_detail_e detail_level, mag_s *magni_opt)
{

	Object_Dump_Less_Recursive (f, plist, magni_opt);
	extern cvar_t scr_xtranslate; if (scr_xtranslate.string[0]) { Cvar_SetQuick (&scr_xtranslate, ""); }
}


void Form_Draw_Dump (ktrl f, stringlist_t *plist)
{
	for (int j = 0; j < f->frm.drawn_list_a->numitems; j ++) {
		ktrl kdrawn = f->frm.drawn_list_a->pointers_array[j];
		stringlistappendf (plist, "%4d: " S_FMT_LEFT_PAD_20 " " RECTI_4PRINTF ,
			j,
			kdrawn->cm.name_a,
			RECT_SEND (kdrawn->r_screen_clipped)
		);
	}
}

// This does ALL controls
CALLERS_ (Form_Focus_Next exclusive)
static int sForm_Get_Focused_Idx (ktrl f, ktrl kfocused, int *p_num_can_focus, ktrl *pk_last)
{
	int kfocus_idx = not_found_neg1;

	for_each_form_all_controls(n,f,k) // ALL CONTROLS!
		if (Oject_Will_Focus_Currently(k) == false) continue;

		*pk_last = k;
		(*p_num_can_focus) ++;
		if (k == kfocused)
			kfocus_idx = n; // Found it.
	for_each_end

	return kfocus_idx;
} // sForm_Get_Focused_Idx


static ktrl sFind_Next_Prev_Focus_Control(ktrl f, ktrl k_oldfocus, int focusidxstart, int dir)
{
	int iters = 0;
	for (int n = focusidxstart + dir; iters < f->frm.controls_a->numitems; n+= dir, iters ++) {
		if (dir > 0 && n >=  f->frm.controls_a->numitems)
			n = 0; // Loop high
		else if (dir < 0 && n < 0)
			n = f->frm.controls_a->numitems - 1; // Loop low

		ktrl k = f->frm.controls_a->pointers_array[n];

		if (Oject_Will_Focus_Currently (k) == false || k == k_oldfocus)
			continue;

		// Found next or prev control that can focus
		return k;
	} // for

	return NULL;
}



void Form_Focus_Next (ktrl f, int dir)
{ c_assert_ (isin2(dir, -1, 1));

	ktrl k_oldfocus = f->frm.kfocused;

	// find our index so we can cycle through starting there.
	ktrl k_last = NULL;
	int numcanfocus = 0, focusidxstart = sForm_Get_Focused_Idx(f, k_oldfocus, &numcanfocus, &k_last);

	if (numcanfocus <= 1) {
		// Focus change impossible if 1 or less sub object
		// However, with form editing this needs handled.
		if (k_last == NULL) {
			Oject_Focus_Set (f);
			return;
		}
		Oject_Focus_Set (k_last);
		return;
	}

	ktrl k_newfocus = sFind_Next_Prev_Focus_Control(f, k_oldfocus, focusidxstart, dir);

	// Lost Focus event
	Oject_Focus_Set (k_newfocus);
}

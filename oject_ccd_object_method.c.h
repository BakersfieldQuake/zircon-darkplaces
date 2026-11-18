// oject_ccd_object_method.c.h

CALLERS_(Object_Method_ZOrder_Front)
int Object_Get_ControlIndex (ktrl kfind)
{
	Form_Get(f,kfind);
	for_each_form_all_controls(n,f,kx) // ALL CONTROLS!
		if (kx == kfind)
			return n;
	for_each_end

	return not_found_neg1;
}

// Jan 23 2025 - BTW this ignores containers.
// Makes an object the first object in the control list, draws first so gets covered
// by any other object drawn in front of it.
// This ignore containers so it sucks.
// This causes the control index to change so it sucks.
// Deal with it for now.

// QC: Walk the string buffer, find the index.
// QC: Rewrite the strings.
// ZORDER FRONT MEANS DRAW LAST
void Object_Method_ZOrder_Front (ktrl kzlast)
{
	Form_Get (f, kzlast);
	int fidx = Object_Get_ControlIndex(kzlast); // Index in f.frm.controls_a
	int listcount = f->frm.controls_a->numitems;
	if (fidx == listcount -1)
		return; // Already last control in the zorder.

	f->frm.controls_a->pointers_array[fidx] = NULL;
	// So we nulled out #20 or #0
	//0 = 1, 1 = 0
	//1 = 2, 2 = 0
	//for count-1 to 1

	// Shift all down from fidx to end.  We could memcpy to do it better.
	// WATCHOUT must do --> f->frm.controls_a->numitems - 1 or we write out of bounds
	// because array[count - 1 + 1] is beyond limit
	for (int jidx = fidx; jidx < (listcount - 1); jidx ++) {
		//ktrl kthis  = f->frm.controls_a->pointers_array[jidx + 0];
		//c_assert_msg_ (kthis == NULL, "Object_Method_ZOrder_Front this shouldn't be null");
		ktrl kafter = f->frm.controls_a->pointers_array[jidx + 1];
		f->frm.controls_a->pointers_array[jidx + 0] = kafter;
		f->frm.controls_a->pointers_array[jidx + 1] = NULL;
	} // for

	f->frm.controls_a->pointers_array[f->frm.controls_a->numitems - 1] = kzlast;
} // Object_Method_ZOrder_Front


void Object_Method_ZOrder_Back (ktrl kzlast)
{
	Form_Get (f, kzlast);
	int fidx = Object_Get_ControlIndex(kzlast);
//	int fcount = f->frm.controls_a->numitems;

	if (fidx == 0) // fcount -1)
		return; // Already first // Already last

	// We are #18 .. We need to become #0
	// If there are 20
	// 0 to 17 move up +1
	// We become #0

	f->frm.controls_a->pointers_array[fidx] = NULL;
	// So we nulled out #20 or #0
	//0 = 1, 1 = 0
	//1 = 2, 2 = 0
	//for count-1 to 1

	// Shift all down from fidx to end.  We could memcpy to do it better.
	// WATCHOUT must do --> f->frm.controls_a->numitems - 1 or we write out of bounds
	// because array[count - 1 + 1] is beyond limit
	for (int jidx = fidx - 1; jidx >= 0; jidx --) {
		//ktrl kthis  = f->frm.controls_a->pointers_array[jidx + 0];
		//c_assert_msg_ (kthis == NULL, "Object_Method_ZOrder_Front this shouldn't be null");
		ktrl kbefore = f->frm.controls_a->pointers_array[jidx];
		f->frm.controls_a->pointers_array[jidx + 0] = NULL;
		f->frm.controls_a->pointers_array[jidx + 1] = kbefore;
	} // for

	f->frm.controls_a->pointers_array[0] = kzlast;
} // Object_Method_ZOrder_Front





CALLERS_ (Oject_Focus_Set Form_ContextMenu_Spawn)
void EditModeSetDotsObject (ktrl k)
{
	Form_Get (f, k);

	if (KCLASS(k) == class_contextmenu)
		return; // SILENT DENY RIGHT NOW.

	for (int dd = 0; dd < CM_DOTCOUNT_COUNT_9; dd ++) {
		ktrl kdot = FP(servo.kdots[dd]);
		XY_Set_Dot (KXPA(kdot, cm.dot_origin), KPA(r_screen), (alignment_e) KXP(kdot, direction));
		O_SizeDot_Refresh (kdot);
		KXP(kdot, is_hidden) = false;
	}

	// CM_MIDDLE_DOT_4 .. overlays entire thing, own cursor.  Moves
	ktrl kmid = FP(servo.kdots[CM_MIDDLE_DOT_4]);
	RECT_COPY (KXP(kmid, cm.relative_rect), KP(r_screen));
	KXP(kmid, is_hidden) = false;
	O_SizeDot_Refresh (kmid);

	//Form_QueueRefresh (f); // need to refresh drawn objects list
}

int EditModeSet (ktrl f, int newval)
{
	int dd;
	if (newval == FP(frm.editmode))
		return 0; // no change

	FP(frm.editmode) = newval;
	if (FP(frm.editmode)) {
		RELATED_ (CM_DOTCOUNT_COUNT_9)
		//unused ktrl kfoc = f->frm.kfocused; // contextmenu disappears first, kfocus should be ok

		// ContextMenu is DENIED!  Should be ok.
		for (dd = 0; dd < CM_DOTCOUNT_COUNT_9; dd ++) {
			ktrl kdot = FP(servo.kdots[dd]);
			KXP (kdot, is_hidden) = false;
		}

		Form_QueueDotsRefresh (f);
	} else {
		// f->frm.editmode == 0
		for (dd = 0; dd < CM_DOTCOUNT_COUNT_9; dd ++) {
			ktrl kdot = FP(servo.kdots[dd]);
			KXP(kdot, is_hidden) = true;
		}
	}

	Form_QueueRefresh (f);
	return 0;
}

typedef struct {
	ccs		*name;
	exofn_t	fn;
	int		is_quakec;
} method_entry_s;

#define DOENTRY(x) { STRINGIFY(x), /*x*/ NULL }
method_entry_s method_entries[] = {
	//{ "DevInit",	DevInit },
	//{ "DevOnIndex", DevOnIndex },
	{"", NULL }, // Method 0 unavailable.
	DOENTRY(DevInit),
	//DOENTRY(DevTabSelectOnChange),
	//DOENTRY(DevListViewSort),
};
#undef DOENTRY

int Method_Find_Index (ccs *s_method)
{
	for (int j = 1; j < (int)ARRAY_COUNT(method_entries); j ++) {
		method_entry_s *zi = &method_entries[j];
		if (String_Match_Caseless(zi->name, s_method)) {
			return j; //zi->fn;// (f, k, listidx, s);
		}
	}
	//Con_PrintLinef ("zexeccmd: command function " QUOTED_S " not found", s_fn);
	return not_found_neg1;
}

void Object_Method_Hide (ktrl k)
{
}

void Object_Method_Show (ktrl k)
{
}

void Object_Method_Enable (ktrl k)
{
}

void Object_Method_Disable (ktrl k)
{
}

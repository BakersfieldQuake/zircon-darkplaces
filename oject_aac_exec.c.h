// oject_aac_exec.c.h


char *Form_To_String_Zalloc (ktrl f, mag_s *magni_opt)
{
	stringlist_t lines = {0};
	Form_Dump (f, &lines, DUMP_DETAIL_SAVE_FILE_0, magni_opt); // Jan 9 2025 - Corrected?

	char *s_ret_zlloc = stringlist_join_lines_zalloc(&lines);
	stringlistfreecontents (&lines);
	return s_ret_zlloc;
}

char *Form_Single_Line_To_String_Zalloc (ktrl f, mag_s *magni_opt)
{
	stringlist_t lines = {0};
	Form_Dump_Single_Line (f, &lines, DUMP_DETAIL_SAVE_FILE_0, magni_opt); // Jan 9 2025 - Corrected?

	char *s_ret_zlloc = stringlist_join_lines_zalloc(&lines);
	stringlistfreecontents (&lines);
	return s_ret_zlloc;
}

void Magnify_From_Video (mag_s *mag)
{
#if 1
	int iscale_x = vid.width / 640;
	int iscale_y = vid.height / 360;
	
	int iscale = iscale_x < iscale_y ? iscale_x : iscale_y;
#else
	int iscale = vid_magnification_factor.integer;
#endif
	if (iscale == 0) {
		iscale = 1; // I don't think this can happen
		Con_PrintLinef (CON_RED "vid_magnification_factor was 0!");
	}

	int excess_x = vid.width - (640 * iscale);
	int excess_y = vid.height - (360 * iscale);
	mag->magnifier = iscale;
	mag->offset_xx = excess_x / 2;
	mag->offset_yy = excess_y / 2;

	if (mag->offset_xx < 0)
		mag->offset_xx = 0;

	if (mag->offset_yy < 0)
		mag->offset_yy = 0;

	//Con_PrintVarInt (mag->magnifier);
	//Con_PrintVarInt (mag->offset_xx);
	//Con_PrintVarInt (mag->offset_yy);
}


static int _zcmdObjectCommand (ktrl f, ktrl k, int listidx, ccs *s)
{
	Con_Clear_f (cmd_local);
	Con_PrintLinef ("Object command!");
	//if (String_Match_Caseless (s, "print")) {
	//	int j = 5;
	//}
	//Form_Dump (f, DUMP_DETAIL_SAVE_FILE_0);
//		stringlist_t lines = {0};
		//Form_Dump (form1, &lines, DUMP_DETAIL_SAVE_FILE_0);
		
		char *s_zalloc;
		if (ui_export_magnification.integer /*d:0*/) { // Feb 12 2025 - Yes this really is ZERO
			// 1920 - (640 * 3)
			mag_s magni_option = { 1, 0, 0};
			Magnify_From_Video (&magni_option);
			s_zalloc = Form_To_String_Zalloc (f, &magni_option);
		} else {
			
			s_zalloc = Form_Single_Line_To_String_Zalloc (f, magni_opt_null);
		}

//#if 0
//		stringlistprint_with_title (&lines, va32("Form Dump detail level (0 to 2) = %d", DUMP_DETAIL_SAVE_FILE_0), Con_PrintLinef);
//#else
		
		Clipboard_Set_Text (s_zalloc); // EditMode: "Form State To Clipboard" clicked
		Z_FreeNull_ (s_zalloc);

		

		//Form_Print_Zones (form1);
		//Form_Draw_Dump (form1, &lines);
		//stringlistprint_with_title (&lines, va32("Drawn list = %d", form1->frm.drawn_list_a->numitems), Con_PrintLinef);
		//stringlistfreecontents (&lines);



	return 0;
}

static int _zcmdEditModeToggle (ktrl f, ktrl k, int listidx, ccs *s)
{
	//char *s_show	= k->list_strings_a.strings[listidx + CM_0_TEXT_0];
	char *s_checked	= k->list_strings_a.strings[listidx + CM_1_CHECKED_1];
	//char *s_str		= k->list_strings_a.strings[listidx + CM_2_STRING_ID_2];
	//unusedchar *s_fn		= k->list_strings_a.strings[listidx + CM_3_FUNCSTRING_3];

	// As much as we hate this ...
	int oldval = s_checked[0] == '1';
	int	newval = !oldval;

	Z_StrDupf_Realloc(&k->list_strings_a.strings[listidx + CM_1_CHECKED_1], "%d", newval ? 1 : 0);

	EditModeSet (f, newval);

	//Con_PrintLinef ("EditMode set to %d", newval);
	return 0;
}

typedef int (*zexecfn_t) (ktrl f, ktrl k, int listidx, ccs *s);
typedef struct {
	ccs			*name;
	zexecfn_t	fn;
} zexecitem_s;

CALLERS_ (O_Context_Menu_Exec)

RELATED_ (Form_ContextMenu_Spawn)
zexecitem_s zexecitems[] = {
	{"EditModeToggle",	_zcmdEditModeToggle	},
	{"ObjectCommand",	_zcmdObjectCommand	},
};

static int zexeccmd (ktrl f, ktrl k, int listidx, ccs *s_fn, ccs *s)
{
	for (int j = 0; j < (int)ARRAY_COUNT(zexecitems); j ++) {
		zexecitem_s *zi = &zexecitems[j];
		if (String_Match_Caseless(zi->name, s_fn)) {
			return zi->fn (f, k, listidx, s);
		}
	}
	Con_PrintLinef ("zexeccmd: command function " QUOTED_S " not found", s_fn);
	return 0;
}



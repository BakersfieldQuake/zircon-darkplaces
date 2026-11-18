// menu_zform_connect_tabchange.c.h

RELATED_ (Form_Event_Onload DevInit)

#define DOFILE(SFILEHERE) \
		numcolumns = 2; \
		c_strlcpy (ezdev->folderopentarget, SFILEHERE); \
		Object_Property_Set (ezdev->kList1, "ColumnWidths", "35,400");	/*connect_metric2*/ \
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Line,Text"); \
		if (!stringlistappendfilelines_did_load(&ezdev->listaccum, SFILEHERE, /*base1?*/ true, /*numcol?*/ true)) { \
			stringlistappend (&ezdev->listaccum, ""); \
			stringlistappendf (&ezdev->listaccum, "%s not found", SFILEHERE); \
			can_be_text_filtered_have_data = false; \
		} // Ender

RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)


EXO___ char *DevGridParticlesOnChange (ktrl k)
{
	RELATED_ (O_ImageGrid_Draw)
	//int row = k->selectedindex % k->gridcolumns;
	//int col = k->selectedindex % k->gridcolumns;
	//int row_idx = row * KP(gridcolumns);
	//int cell_idx = row_idx + KP(viewcalcx.first_vis_row);

	//ktrl kLabel = ezdev->kLabelTexNum;
	//int cell_idx = k->s
	Object_Property_Set_Fmt (ezdev->kLabelTexNum, "Caption", "Tex %d", k->selectedindex);

	RELATED_ (O_Label_Refresh_Plus_Early)
	//ezdev->kLabelTexNum->cm._relative_rect_orig.width = 0; // See if will kick
	//Form_QueueRefreshPlusMetric (k->fctrl);
	Form_QueueRefresh (k->fctrl);
	return NULL;
}

void PRVM_Globals_Query2 (prvm_prog_t *prog)
{
	char vabuf[1024];
	char valuebuf[MAX_INPUTLINE_16384]; // QuakeC values written to this.
	ccs *s_stringlist_suffix = sv_changelevel_preserve_globals_stringlist_suffix.string;
	//qbool is_stringlist = (s_stringlist_suffix[0] && String_Ends_With (key, s_stringlist_suffix));

	for (int idx = 0; idx < prog->numglobaldefs; idx ++) {
		mdef_t *global = &prog->globaldefs[idx];

		int type = global->type;

		// Baker: Type can be zero -- examples .. "self"
		ccs *key = PRVM_GetString(prog, global->s_name);

		// Baker: January 5 2025 - "devinfo" -> globals is showing self so what is this skipping?
		if (false == Have_Flag (global->type, DEF_SAVEGLOBAL))
			continue; // self, other, ...

		int vartype = Flag_Remove(type, DEF_SAVEGLOBAL);

		// Baker: What are the types that are rejected here? .. vector  cause xyz is wriiten
		// filter _x _y _z
		if (false == isin4 (vartype, ev_string_1, ev_float_2, ev_vector_3, ev_entity_4))
			continue;

		// Any of the following suffix we ignore, covered by vector.
		if (vartype == ev_float_2 && String_Ends_With_Caseless_3 (key, "_x", "_y", "_z"))
			continue;

		// Baker: prog->statestring provides the current action in case of a crash
		prog->statestring = va(vabuf, sizeof(vabuf), "PRVM_ED_WriteGlobals, name=%s", key);
		PRVM_UglyValueString (prog, (etype_t)vartype, (prvm_eval_t *)&prog->globals.fp[global->ofs], valuebuf, sizeof(valuebuf));
		prog->statestring = NULL;

		// Baker: January 5 2025 NASTY
		//int count = ezdev->listaccum.numstrings / 4 /*numcolumns*/;
		// April 16 2026 - expanded
		int count = ezdev->listaccum.numstrings / ezdev->kList1->columncount; // 5

		ccs *vartype_str = VarType_For_EV (vartype);

		stringlistappendf	(&ezdev->listaccum, "%d", count); // index0

		ccs *value = valuebuf;
		// Index,Global Variable,Value,VarType,Extra
		if (false == isin3(vartype, ev_string, ev_float, ev_entity)) {
			//stringlistappendf	(&ezdev->listaccum, CON_RED "%s" CON_WHITE, vartype_str); // ?
			stringlistappendf	(&ezdev->listaccum, "%s", key);  // key1
			stringlistappend	(&ezdev->listaccum, value); // value2
			stringlistappendf	(&ezdev->listaccum, "%s" CON_YELLOW  " (will NOT save to savegame)" CON_WHITE, vartype_str); // vartype3
			stringlistappend	(&ezdev->listaccum, ""); // extra4 Can't be a stringbuffer because not a float
		} else {
			stringlistappend	(&ezdev->listaccum, key); // key1
			stringlistappend	(&ezdev->listaccum, value); // value2
			stringlistappend	(&ezdev->listaccum, vartype_str); // strtype3
#if 0
			if (String_Contains (key, "_list")) {
				int j = 5;
			}
#endif
			qbool is_stringlist = (s_stringlist_suffix[0] && String_Ends_With (key, s_stringlist_suffix));
			if (is_stringlist) {
				int bufnum = atoi (value);
				if (value == 0)
					stringlistappendf (&ezdev->listaccum, "list (uninit)"); // extra4
				else {
					prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, bufnum);
					if (!stringbuffer) {
						stringlistappendf (&ezdev->listaccum, "list (invalid)"); // extra4
					}
					else {
						char sample[512] = {0};
						for (int n = 0; n < 5 && n < stringbuffer->num_strings; n ++) {
							if (n > 0)
								c_strlcat (sample, "," SPACE);
							ccs *sxy = stringbuffer_get_index (stringbuffer, n);
							c_strlcatf (sample, QUOTED_S, sxy);
						}
						if (stringbuffer->num_strings && stringbuffer->num_strings >= 5) {
							c_strlcat (sample, ", ...");
						}
						int list_count = stringbuffer->num_strings;
						stringlistappendf (&ezdev->listaccum, "list (%d ) %s", list_count, sample); // extra4
					}
				}
			} else {
			  stringlistappend	(&ezdev->listaccum, ""); // extra4 Not a stringbuffer according to suffix rules
			}

		}

		extern cvar_t _scr_devinfo_globalscap;
		if (_scr_devinfo_globalscap.integer && count >= _scr_devinfo_globalscap.integer) {
			stringlistappendf	(&ezdev->listaccum, "%d", _scr_devinfo_globalscap.integer);
			stringlistappendf	(&ezdev->listaccum, "[_scr_devinfo_globalscap MAX HIT AT %d]", _scr_devinfo_globalscap.integer);
			stringlistappend	(&ezdev->listaccum, "---");
			stringlistappend	(&ezdev->listaccum, "---");
			stringlistappend	(&ezdev->listaccum, "");

			return; // STOP!!!!!!!!!!!
		}

	} // for
}


RELATED_ (OnChange calls us too on Form_Create)
EXO___ char *DevTabSelectOnChange (ktrl kTabSelect1)
{
	char vabuf[1024];
	stringlistfreecontents (&ezdev->listaccum);

	Form_Get (f, kTabSelect1);
	int tab_idx = kTabSelect1->selectedindex;

	Form_QueueRefreshPlusMetric (f);

	Object_Property_Set (ezdev->f, "OnMouseButtonAction", "DevMouseButtonAction"); // SET LISTVIEW1 OnMouseButtonAction
	Object_Property_Set (ezdev->f, "OnMouseMove", ""); // Set to nothing.
	Object_Property_Set (ezdev->kList1, "OnChange", ""); // Set to nothing.
	Object_Property_Set (ezdev->kList2, "OnChange", ""); // Set to nothing.

	//if (ezdev->kList3->servo.kcloser)
	//	Object_Property_Set (ezdev->kList3->servo.kcloser, "ToolTip", "VALIGN:(Ctrl+W) Close");

//#pragma message ("FIXME DevListViewOnKeyPreview is music specific, make it default nothing and set per tabindex")

	RELATED_ (DevInit ==> DevFormOnKeyPreview)
	RELATED_ (DevMouseButtonAction)

	// KLIST
	stringlistfreecontents (&ezdev->kList1->list_strings_a); //Object_Method_Clear
	stringlistfreecontents (&ezdev->kList2->list_strings_a); //Object_Method_Clear
	stringlistfreecontents (&ezdev->kList3->list_strings_a); //Object_Method_Clear
	ezdev->folderopentarget[0] = CHAR_NULL_0;

	ezdev->kList1->selectedindex = 0;
	ezdev->kList1->selectedlength = 0;
	ezdev->kList1->sortcolumnindex = 0;
	ezdev->kList1->sortcolumndescending = false;
	ezdev->kList1->r_scrollport.left = ezdev->kList1->r_scrollport.top = 0;

	ezdev->kList1->cm._relative_rect_orig.width = 500; // connect_metric
	ezdev->kList1->columncount = 2;
	Object_Property_Set (ezdev->kList1, "ColumnWidths", "50,150");
	Lister_SetPctHorzListView (ezdev->kList1, 0, ezdev->kList1->servo.kscrollx);
	if (ezdev->kList1->servo.kscrollx->value) {
		ezdev->kList1->servo.kscrollx->value = 0;
	}
	Lister_SetPct (ezdev->kList1, 0, ezdev->kList1->servo.kscrolly);
	if (ezdev->kList1->servo.kscrolly->value) {
		ezdev->kList1->servo.kscrolly->value = 0;
	}
	Object_Property_Set (ezdev->kList1, "BackColor", "#005AE6"); //

	// KLIST2
	//Object_Property_Set		(ezdev->kList2, "FontSize", "9");
	ezdev->kList2->selectedindex = 0;
	ezdev->kList2->selectedlength = 0;
	ezdev->kList2->sortcolumnindex = 0;
	ezdev->kList2->sortcolumndescending = false;
	ezdev->kList2->r_scrollport.left = ezdev->kList2->r_scrollport.top = 0;
	Object_Property_Set	(ezdev->kList2, "ColumnWidths", "50,107"); // connect_metric2
	Object_Property_Set	(ezdev->kList2, "ColumnHeaders", "Key,Value");
	Object_Property_Set (ezdev->kList2, "IsHidden", "True"); // Set to nothing.

	// KLIST3
	ezdev->kList3->selectedindex = 0;
	ezdev->kList3->selectedlength = 0;
	ezdev->kList3->sortcolumnindex = 0;
	ezdev->kList3->sortcolumndescending = false;
	ezdev->kList3->r_scrollport.left = ezdev->kList3->r_scrollport.top = 0;
	Object_Property_Set (ezdev->kList3, "ToolTip", "");
	Object_Property_Set (ezdev->kList3, "IsHidden", "True"); // Hide List3
	Object_Property_Set (ezdev->kImageTexture, "IsHidden", "True");
	//Object_Property_Set (ezdev->kImageParticleFont, "IsHidden", "True");
	//Object_Property_Set (ezdev->kImageParticleFontCursor, "IsHidden", "True");
	Object_Property_Set (ezdev->kGridParticles, "IsHidden", "True");
	Object_Property_Set (ezdev->kLabelTexNum, "IsHidden", "True");

	Object_Property_Set (ezdev->kImageParticleFont, "IsHidden", "True");
	Object_Property_Set (ezdev->kRectColor, "IsHidden", "True");

	Object_Property_Set (ezdev->kButtonShowList3, "IsHidden", "True"); // Assume List3 is not available
	Object_Property_Set (ezdev->kImageTexture, "IsQuake1BSPTexture", "False"); // Not Q1 BSP

#if 000000
	for (int idx = 0; idx < 16; idx ++) {
		RELATED_ (DevInit)
		va_super (sr, 128, "LabelParticleRow%d", idx);
		ktrl krow = Form_Find_Name (f, sr);
		va_super (sc, 128, "LabelParticleCol%d", idx);
		ktrl kcol = Form_Find_Name (f, sc);
		krow->is_hidden = true;
		kcol->is_hidden = true;
	}
#endif

	RELATED_ (DevListViewOnChange)

	// REFRESH LabelTitle.
	va_super (s_lbltitle, 128, "[%d] %s", tab_idx, ezdev->kTabSelect1->list_strings_a.strings[tab_idx]);
	Z_StrDup_Realloc (&ezdev->kLabelTitle->caption_a, s_lbltitle);
	Form_QueueRefreshPlusMetric (f);

	// Set No Data
	int numcolumns = 2;
	Object_Property_Set	(ezdev->kList1, "ColumnWidths", "75,200");
	Object_Property_Set	(ezdev->kList1, "ColumnHeaders", "No Data");

//REOL:

	qbool can_be_text_filtered_have_data = false; // "NO DATA == cannot be filtered"
	switch (tab_idx) {
	default: // Nada

	case_break lw_entities_0: // Server Entities
		if (false == SVVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No server progs loaded (map not running)");
			break;
		}

		numcolumns = 5; can_be_text_filtered_have_data = true;
		ezdev->kList1->cm._relative_rect_orig.width = 326; // connect_metric
		Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange"); // SET LISTVIEW1 ONCHANGE
		Object_Property_Set (ezdev->kList2, "IsHidden", "False"); // Make visible
		Object_Property_Set (ezdev->kList1, "ColumnWidths","30,76,51,70,88"); // connect_metric
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Index,Classname,TargetName,Model (No Path),Origin (Rounded)");

		{ RELATED_ (DevListViewOnChange VM_buf_create)
			prvm_prog_t *prog = SVVM_prog;
			RELATED_ (PRVM_ED_PrintNum)
			for_each_edict_in_prog(edict_num, ed)
			//for (int edict_num = 0; edict_num < prog->num_edicts; edict_num ++) {
				//prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);
				//if (ed->free) continue;

				stringlistappendf	(&ezdev->listaccum, "%d", edict_num);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "classname", vabuf);
				stringlistappend	(&ezdev->listaccum, vabuf);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "targetname", vabuf);
				stringlistappend	(&ezdev->listaccum, vabuf);
				// fix

				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "model", vabuf);
				ccs *modelname_no_path = File_URL_SkipPath(vabuf);
				stringlistappend	(&ezdev->listaccum, modelname_no_path);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "origin", vabuf);
				if (String_Match(vabuf, "0 0 0")) {
					stringlistappend	(&ezdev->listaccum, "");
				} else {
					vec3_t vorg = {0};
					Math_atov3 (vabuf, vorg);
					stringlistappendf	(&ezdev->listaccum, "%d %d %d", (int)vorg[0], (int)vorg[1], (int)vorg[2]);
				}

			for_each_end // each entity
		} // End block

	case_break lw_models_1: // Models
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}

		RELATED_ (DevListViewOnChange)
		numcolumns = 4; can_be_text_filtered_have_data = true;

		ezdev->kList1->cm._relative_rect_orig.width = 326; // connect_metric
		Object_Property_Set		(ezdev->kList1, "OnChange", "DevListViewOnChange"); // SET LISTVIEW1 ONCHANGE
		Object_Property_Set		(ezdev->kList2, "OnChange", "DevList2ViewOnChange"); // HOVER COL2
		Object_Property_Set		(ezdev->kList2, "IsHidden", "False"); // Make visible
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "50,154," "108,0"); //300"); // FOLDERENABLED MODELS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "ModelIndex,Model," "Pack,Real Full Unix Path");

		if (PLAYING_MAP_LOADED) {
			for (int modelindex = 1; modelindex < MAX_MODELS_8192 && cl.model_name[modelindex][0]; modelindex ++) {
				ccs *modelname = &cl.model_name[modelindex][0];
				model_t *mod = cl.model_precache[modelindex];
				if (!mod)
					break; // DONE.

				stringlistappendf (&ezdev->listaccum, "%d", modelindex);
				stringlistappendf (&ezdev->listaccum, "%s", modelname);

				// Need to accomodate "*" models with ""
				ccs *s_which_file = FS_Which_Or_Null(modelname);
				if (s_which_file == NULL) {
					stringlistappend	(&ezdev->listaccum, "");
					stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
					continue;
				}
				va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
				File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

				if (String_Ends_With(s_which_file_unix, "/")) {
					// It is a directory
					stringlistappend	(&ezdev->listaccum,  "" );
					stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, modelname);
				} else {
					stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
					stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
				}
			} // for
		} // Map loaded

	case_break lw_sounds_2: // CL Sounds (SOUNDINDEX) - not sure how CSQC works with this
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}

		numcolumns = 4; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "50,154," "208,0"); //"108,0");// ,300"); // SOUNDS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "SoundIndex,Sound," "Pack,Real Full Unix Path");

		for (int soundindex = 1; soundindex < MAX_SOUNDS_4096 && cl.sound_name[soundindex][0]; soundindex ++) {
			ccs *soundname = cl.sound_name[soundindex];
			stringlistappendf (&ezdev->listaccum, "%d", soundindex);
			stringlistappendf (&ezdev->listaccum, "%s", soundname);

			// Prefix with "sound/" folder
			// This is normal for Quake 1, Nexuiz, Xonotic and even for maps target Quake III Arena.
			qbool starts_with_sound = String_Starts_With (soundname, "sound/");
			va_super (s_sounds_soundname, MAX_QPATH_128, "%s%s", starts_with_sound ? "" : "sound/", soundname);

			ccs *s_which_file = FS_Which_Or_Null(s_sounds_soundname);

			if (!s_which_file) {
				// DOES NOT EXIST
				stringlistappend	(&ezdev->listaccum, ""); // PACK
				stringlistappend	(&ezdev->listaccum, ""); // REALPATH UNIX FOR CONTAINER (But does not exist)
				continue;
			}

			va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
			File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

			if (String_Ends_With(s_which_file_unix, "/")) {
				// It is a directory
				stringlistappend	(&ezdev->listaccum,  "" );
				stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, s_sounds_soundname);
			} else {
				stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
				stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
			}
		} // for

	case_break lw_textures_3: // Textures
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}

		numcolumns = 4;  can_be_text_filtered_have_data = true;
		Object_Property_Set	(ezdev->kList1, "OnChange", "DevListViewOnChange");
		Object_Property_Set	(ezdev->kList1, "ColumnWidths", "50,154," "108,0");//300"); // TEXTURES
		Object_Property_Set	(ezdev->kList1, "ColumnHeaders", "Index,World Texture," "Pack,Real Full Unix Path");
		Object_Property_Set (ezdev->kImageTexture, "IsHidden", "False");

		// Baker: Half-Life and Q2 ... dunno
		//qbool is_q1bsp_cannot_path = cl.worldmodel->type == mod_brushq1;
		if (/*is_q1bsp_cannot_path*/ cl.worldmodel->type == mod_brushq1)
			Object_Property_Set (ezdev->kImageTexture, "IsQuake1BSPTexture", "True"); // Not Q1 BSP

		for (int texture_idx = 0; texture_idx < r_refdef.scene.worldmodel->num_textures; texture_idx ++) {
			model_t		*m = r_refdef.scene.worldmodel;
			texture_t	*t = &m->data_textures[texture_idx];
			ccs *texturename = t->name;
			stringlistappendf (&ezdev->listaccum, "%d", texture_idx);
			stringlistappendf (&ezdev->listaccum, "%s", texturename);

			if (/*is_q1bsp_cannot_path*/ cl.worldmodel->type == mod_brushq1) {
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
				continue;
			}

			// Baker: We have to try .tga then .png then .jpeg then .jpg then .pcx then .lmp
			ccs *fmt_string = Image_Locate_Found_Format_String(texturename);

			if (NULL == fmt_string) {
				// Does not exist
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
				continue;
			}

			va_super (s_image_with_extension, MAX_OSPATH_EX_1024, fmt_string, texturename);

			ccs *s_which_file = FS_Which_Or_Null(s_image_with_extension);

			if (!s_which_file)
				continue;

			va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
			File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

			if (String_Ends_With(s_which_file_unix, "/")) {
				// It is a directory
				stringlistappend	(&ezdev->listaccum,  "" );
				stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, s_image_with_extension);
			} else {
				stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
				stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
			}


#if 0
			qbool did_find = false;
			for (int image_format_index = 0; image_format_index < NUM_IMAGE_FORMATS_6; image_format_index ++) {
				ccs *fmt_string = Image_Extension_Format_String_Number_0_to_5(image_format_index);

				va_super (s_image_with_extension, MAX_OSPATH_EX_1024, fmt_string, texturename);

				ccs *s_which_file = FS_Which_Or_Null(s_image_with_extension);

				if (!s_which_file)
					continue;

				did_find = true;

				va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
				File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

				if (String_Ends_With(s_which_file_unix, "/")) {
					// It is a directory
					stringlistappend	(&ezdev->listaccum,  "" );
					stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, texturename);
				} else {
					stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
					stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
				}

				break;
			} // for
			if (false == did_find) {
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
				continue;
			}

#endif

		} // for

	case_break lw_shaders_4:
		numcolumns = 6; can_be_text_filtered_have_data = true;

		RELATED_ (DevListViewOnChange)

		// New: index shader scriptname pakfile
		Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange");
		Object_Property_Set	(ezdev->kList1, "ColumnWidths", "30,189,19,113,124,0"); // SHADERS
		Object_Property_Set	(ezdev->kList1, "ColumnHeaders", "#,Shader,Len,Scriptfile,pk3,fulltext");

		Object_Property_Set (ezdev->kList3, "IsHidden", "False"); // Set to nothing.

		RELATED_ (ShaderText_Atomize Mod_Shaders2_Query)
// plist_shaders - column0 - shadername
//                 column1 - scripts/theshader.name
//                 column2 - the entire shader text

		for (int shader64_row_idx = 0; shader64_row_idx < ezdev->list_shaders64.numstrings; shader64_row_idx += 3) {
			int shader64num_calc = shader64_row_idx / 3;
			ccs *shadername = ezdev->list_shaders64.strings[shader64_row_idx + 0];
			int slen = istrlen(shadername);
			if (slen == 0) {
				// Our de-duplication marked for deletion .. ignore ..
				continue;
			}
			ccs *shaderfile = ezdev->list_shaders64.strings[shader64_row_idx + 1];
			ccs *shaderblob = ezdev->list_shaders64.strings[shader64_row_idx + 2];
			char *pk3name_za = NULL;

			//ccs *scripts_name_dot_shader = NULL;
			for (int filenum = 0; filenum < ezdev->list_scripts_dot_shader.numstrings; filenum ++) {
				ccs *thisfile_plus_pk3 = ezdev->list_scripts_dot_shader.strings[filenum];
				if (false == String_Starts_With(thisfile_plus_pk3, shaderfile))
					continue;  // Not this one.

				pk3name_za = String_Instance_Base1_ZAlloc (thisfile_plus_pk3, CHAR_TAB_9, /*2nd instance*/ 2, /*reply*/ NULL);
				break; // Found it get out!
			} // for

			stringlistappendf	(&ezdev->listaccum, "%d", shader64num_calc); // 1

			// 01234567
			// scripts/
			ccs *shaderfile_no_scripts_prefix = strlen(shaderfile) >= 8 ? &shaderfile[8] : shaderfile;
			stringlistappend 	(&ezdev->listaccum, shadername); // 2
			stringlistappendf	(&ezdev->listaccum, "%d", slen); // 3
			stringlistappend	(&ezdev->listaccum, shaderfile_no_scripts_prefix); // 4
			stringlistappend	(&ezdev->listaccum, pk3name_za ? pk3name_za : ""); // 5
			stringlistappend	(&ezdev->listaccum, shaderblob); // 5

			Mem_FreeNull_ (pk3name_za);
		} // shader64num

	case_break lw_globals_5:
		if (false == SVVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No server progs loaded (map not running)");
			break;
		}

		numcolumns = 5;  can_be_text_filtered_have_data = true; // Baker: Janary 5 2025 hardcoded in EZDev_Globals_Feed_Shall_Stop_Fn /*numcolumns*/;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "50,100,100,100,160");
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "Index,Global Variable,Value,VarType,Extra");

		PRVM_Globals_Query2 (SVVM_prog);
		break;

	case_break lw_fields_6:
		if (false == SVVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No server progs loaded (map not running)");
			break;
		}

		numcolumns = 3; can_be_text_filtered_have_data = true;
		Object_Property_Set	(ezdev->kList1, "ColumnWidths", "50,100,165");
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "Index,FieldName,VarType");
		PRVM_Fields_Query (SVVM_prog, EZDev_Fields_Feed_Shall_Stop_Fn);

	case_break lw_map_7:
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}

		numcolumns = 2; can_be_text_filtered_have_data = false; // This is a map summary
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "150,340"); // INFO
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", ",");

		Map_Append_Pairs (&ezdev->listaccum);

	case_break lw_environment_8:
		numcolumns = 2; can_be_text_filtered_have_data = false; // This is an environment summary

		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "150,340"); // INFO
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", ",");

		// path
		// basedir

		stringlistappend	(&ezdev->listaccum,  "Base game:");
		stringlistappendf	(&ezdev->listaccum,  "%s", gamedirname1);

		stringlistappend2	(&ezdev->listaccum,  "", "");

		stringlistappend2	(&ezdev->listaccum,  "Command Line:", cmdline.string);
		stringlistappend2	(&ezdev->listaccum,  "", "");
		{
			ccs *cwd_ta = FS_Getcwd_No_Trailing_Slash_Unsafe_TempAlloc ();
			stringlistappend2	(&ezdev->listaccum,  "Current directory:", cwd_ta);
			stringlistappend2	(&ezdev->listaccum,  "", "");
			Mem_FreeNull_ (cwd_ta);
		}
		stringlistappend2	(&ezdev->listaccum,  "fs_basedir:", fs_basedir);
		stringlistappend2	(&ezdev->listaccum,  "", "");

		stringlistappend2	(&ezdev->listaccum,  "fs_gamedir:", fs_gamedir);
		stringlistappend2	(&ezdev->listaccum,  "", "");

		stringlistappend2	(&ezdev->listaccum,  "fs_userdir:", fs_userdir);
		stringlistappend2	(&ezdev->listaccum,  "", "");
		{
			char *exe_za = FS_File_Binary_URL_Unsafe_ZAlloc ();
			stringlistappend2	(&ezdev->listaccum,  "exe:", exe_za);
			stringlistappend2	(&ezdev->listaccum,  "", "");
			Mem_FreeNull_ (exe_za);
		}

		va_super (s_argc, 8, "%d", sys.argc);
		stringlistappend2	(&ezdev->listaccum,  "Sys Args:", s_argc);

		for (int n = 0; n < sys.argc; n ++) {
			va_super (s_num, 8, "%d", n);
			stringlistappend2	(&ezdev->listaccum,  s_num, sys.argv[n]);
		} // for

		stringlistappend	(&ezdev->listaccum,  "");
		stringlistappend	(&ezdev->listaccum,  "");

		stringlistappend	(&ezdev->listaccum,  "Path:");
		stringlistappend	(&ezdev->listaccum,  "");
		FS_Path_Feed (&ezdev->listaccum);

		break;

	case_break lw_csqc_ents_9:
		if (false == CLVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No client progs loaded");
			break;
		}
		numcolumns = 5; can_be_text_filtered_have_data = true;
		ezdev->kList1->cm._relative_rect_orig.width = 326; // connect_metric
		Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange"); // SET LISTVIEW1 ONCHANGE
		Object_Property_Set (ezdev->kList2, "IsHidden", "False"); // Make visible
		Object_Property_Set (ezdev->kList1, "ColumnWidths","30,76,51,70,88"); // connect_metric
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Index,Classname,TargetName,Model (No Path),Origin (Rounded)");

		{ RELATED_ (DevListViewOnChange VM_buf_create)
			prvm_prog_t *prog = CLVM_prog;
			RELATED_ (PRVM_ED_PrintNum)
			for (int edict_num = 0; edict_num < prog->num_edicts; edict_num ++) {
				prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);
				if (ed->free) continue;

				stringlistappendf	(&ezdev->listaccum, "%d", edict_num);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "classname", vabuf);
				stringlistappend	(&ezdev->listaccum, vabuf);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "targetname", vabuf);
				stringlistappend	(&ezdev->listaccum, vabuf);
				// fix

				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "model", vabuf);
				ccs *modelname_no_path = File_URL_SkipPath(vabuf);
				stringlistappend	(&ezdev->listaccum, modelname_no_path);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "origin", vabuf);
				if (String_Match(vabuf, "0 0 0")) {
					stringlistappend	(&ezdev->listaccum, "");
				} else {
					vec3_t vorg = {0};
					Math_atov3 (vabuf, vorg);
					stringlistappendf	(&ezdev->listaccum, "%d %d %d", (int)vorg[0], (int)vorg[1], (int)vorg[2]);
				}

			} // each entity
		}


	case_break lw_csqc_globals_10:
		if (false == CLVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No client progs loaded");
			break;
		}

		numcolumns = 5; can_be_text_filtered_have_data = true; // Change in EZDev_Globals_Feed_Shall_Stop_Fn too!
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "50,100,100,165"); // GLOBALS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "Index,Global Variable,Value,VarType");

		PRVM_Globals_Query2 (CLVM_prog);

	case_break lw_csqc_fields_11:
		if (false == CLVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No client progs loaded");
			break;
		}

		numcolumns = 3; can_be_text_filtered_have_data = true;
		Object_Property_Set	(ezdev->kList1, "ColumnWidths", "50,100,165");
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Index,FieldName,VarType");

		PRVM_Fields_Query (CLVM_prog, EZDev_Fields_Feed_Shall_Stop_Fn);

	case_break lw_csqc_stringlist_12: // Add to list accum
		if (false == CLVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No client progs loaded");
			break;
		}
		{ RELATED_ (DevListViewOnChange VM_buf_create)
			prvm_prog_t *prog = CLVM_prog;
			numcolumns = 2; can_be_text_filtered_have_data = true;
			ezdev->kList1->cm._relative_rect_orig.width = 320; // connect_metric
			Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange"); // Set to nothing.
			Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Buffer #,NumStrings");
			Object_Property_Set (ezdev->kList1, "ColumnWidths", "50,150"); // connect_me

			Object_Property_Set (ezdev->kList2, "IsHidden", "False"); // VISIBLE
			Object_Property_Set	(ezdev->kList2, "ColumnHeaders", "Index,String");
			Object_Property_Set	(ezdev->kList2, "ColumnWidths", "50,550"); // CSQC STRINGLIST

			if (prog->loaded) {
				int numarrays = (int)prog->stringbuffersarray.maxarrays;
				for (int idx = 0; idx < numarrays; idx ++) {
					stringlistappendf (&ezdev->listaccum, "%d", idx);
					prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, idx);

					if (!stringbuffer)	stringlistappend	(&ezdev->listaccum, "(empty)");
					else				stringlistappendf	(&ezdev->listaccum, "%d", stringbuffer->num_strings);
				} // each idx
			} // IF CLVM
		}

	case_break lw_ssqc_stringlist_13:
		if (false == SVVM_prog->loaded) {
			stringlistappend2 (&ezdev->listaccum, "", "No server progs loaded (map not running)");
			break;
		}

		numcolumns = 2; can_be_text_filtered_have_data = true;
		ezdev->kList1->cm._relative_rect_orig.width = 320; // connect_metric
		Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange"); // Set to nothing.
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Buffer #,NumStrings");
		Object_Property_Set (ezdev->kList1, "ColumnWidths", "50,150"); // SS STRINGLIST

		Object_Property_Set (ezdev->kList2, "IsHidden", "False"); // VISIBLE
		Object_Property_Set	(ezdev->kList2, "ColumnHeaders", "Index,String");
		Object_Property_Set	(ezdev->kList2, "ColumnWidths", "50,550"); // connect_metric

		if (SVVM_prog->loaded) {
			prvm_prog_t *prog = SVVM_prog;
			int numarrays = (int)prog->stringbuffersarray.maxarrays;
			for (int idx = 0; idx < numarrays; idx ++) {
				stringlistappendf (&ezdev->listaccum, "%d", idx);
				prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, idx);

				if (!stringbuffer)	stringlistappend (&ezdev->listaccum, "(empty)");
				else				stringlistappendf (&ezdev->listaccum, "%d", stringbuffer->num_strings);
			} // each idx
		} // IF CLVM

	case_break lw_quake_rc_14:			can_be_text_filtered_have_data = true; DOFILE("quake.rc");
	case_break lw_default_cfg_15:		can_be_text_filtered_have_data = true; DOFILE("default.cfg");
	case_break lw_autoexec_cfg_16:		can_be_text_filtered_have_data = true; DOFILE("autoexec.cfg");
	case_break lw_config_cfg_17:		can_be_text_filtered_have_data = true; DOFILE("config.cfg");

	case_break lw_map_ents_18: // OnChange (2)
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}

		numcolumns = 2; can_be_text_filtered_have_data = true;

		Object_Property_Set (ezdev->kList1, "ColumnWidths", "35,300");
		Object_Property_Set (ezdev->kList1, "ColumnHeaders", "Line,Text");
		{
			va_super (entname, MAX_QPATH_128, "%s.ent", cl.worldnamenoextension);  // DOT_ENT (2)
			char *entities_alloc_ta = FS_LoadFileQuick(entname);
			if (entities_alloc_ta) {
				int slenplus1 = istrlen(entities_alloc_ta) + ONE_CHAR_1;
				String_Edit_Replace_Memory_Constant (entities_alloc_ta, slenplus1, "\r", "");
				stringlistappend_split_delimiter_linenumbers (&ezdev->listaccum, entities_alloc_ta, NEWLINE);
				c_strlcpy (ezdev->folderopentarget, entname);
				Mem_FreeNull_ (entities_alloc_ta);
			} else {
				c_strlcpy (ezdev->folderopentarget, cl.worldname);
				stringlistappend_split_delimiter_linenumbers (&ezdev->listaccum, cl.worldmodel->brush.entities, NEWLINE);
			}
		}

	case_break lw_rtlights_19:
		if (false == PLAYING_MAP_LOADED) {
			stringlistappend2 (&ezdev->listaccum, "", "Map not running");
			break;
		}
		{
			can_be_text_filtered_have_data = true; // Error towards too much freedom to filter.
			va_super (rtlightsfilename, MAX_QPATH_128, "maps/%s.rtlights", cl.worldbasename);
			DOFILE(rtlightsfilename);
		}

	case_break lw_music_20:

		numcolumns = 4; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,189," "108,0");//330"); // MUSIC
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Track," "Pack,Real Full Unix Path");

		{
			fssearch_t *search;

			search = FS_Search ("music/*.ogg", fs_caseless_true, fs_quiet_true, fs_pakfile_null, fs_gamedironly_false);

			if (search) {
				for (int file_idx = 0; file_idx < search->numfilenames; file_idx ++) {
					ccs *trackname = search->filenames[file_idx];
					int rownum = ezdev->listaccum.numstrings / numcolumns;

					stringlistappendf	(&ezdev->listaccum, "%d", rownum);
					stringlistappend	(&ezdev->listaccum, trackname);

					ccs *s_which_file = FS_Which_Or_Null(trackname);
					if (s_which_file == NULL) {
						stringlistappend	(&ezdev->listaccum, "");
						stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
						continue;
					}

					va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
					File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

					if (String_Ends_With(s_which_file_unix, "/")) {
						// It is a directory
						stringlistappend	(&ezdev->listaccum,  "" );
						stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, trackname);
					} else {
						stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
						stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
					}
				} // for
				FS_FreeSearch(search);
			} // search

			search = FS_Search ("sound/cdtracks/*.ogg", fs_caseless_true, fs_quiet_true, fs_pakfile_null, fs_gamedironly_false);

			if (search) {
				for (int file_idx = 0; file_idx < search->numfilenames; file_idx ++) {
					ccs *trackname = search->filenames[file_idx];
					int rownum = ezdev->listaccum.numstrings / numcolumns;

					stringlistappendf	(&ezdev->listaccum, "%d", rownum);
					stringlistappend	(&ezdev->listaccum, trackname);

					ccs *s_which_file = FS_Which_Or_Null(trackname);
					if (s_which_file == NULL) {
						stringlistappend	(&ezdev->listaccum, "");
						stringlistappend	(&ezdev->listaccum, ""); // Does not exist anywhere.
						continue;
					}

					va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
					File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

					if (String_Ends_With(s_which_file_unix, "/")) {
						// It is a directory
						stringlistappend	(&ezdev->listaccum,  "" );
						stringlistappendf	(&ezdev->listaccum, "%s%s", /*folder*/ s_which_file_unix, trackname);
					} else {
						stringlistappend	(&ezdev->listaccum, File_URL_SkipPath(s_which_file_unix));
						stringlistappend	(&ezdev->listaccum, /*pak*/ s_which_file_unix);
					}
				} // for
				FS_FreeSearch(search);
			} // search
		}

	case_break lw_particlefont_21:

		//numcolumns = 5;  can_be_text_filtered_have_data = false;
		can_be_text_filtered_have_data = true; // We want this to be true so CTRL+O works
		stringlistappend2 (&ezdev->listaccum, "", ""); // Append a no data entry

		RELATED_ (DevGridParticles OnChangeDevInit O_ImageGrid_Draw)
		Object_Property_Set (ezdev->kTxtFilter, "IsHidden", "True");
		Object_Property_Set (ezdev->kGridParticles, "IsHidden", "False");
		Object_Property_Set (ezdev->kLabelTexNum, "IsHidden", "False");
		Object_Property_Set (ezdev->kList1, "BackColor", "#111"); // #005AE6

		RELATED_ (DevMouseMoveAction ToolTip_Deploy_If_Target_ToolTipText)
		Object_Property_Set (ezdev->f, "OnMouseMove", "DevMouseMoveAction");


	case_break lw_effectinfo_list_22:
		RELATED_ (lw_shaders_4)
		numcolumns = 2; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,250");
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Effect Name");
		{
			int effectinfo_list_stringlist_columns_2 (stringlist_t *plist);
			effectinfo_list_stringlist_columns_2 (&ezdev->listaccum);
		}

		// New: index shader scriptname pakfile
		Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange");
		Object_Property_Set (ezdev->kList3, "IsHidden", "False"); // Set to nothing.
		c_strlcpy(ezdev->folderopentarget,"effectinfo.txt");

	case_break lw_effectinfo_txt_23:

		can_be_text_filtered_have_data = true; DOFILE("effectinfo.txt");
		if (can_be_text_filtered_have_data) {
			RELATED_ (DevListViewOnChange)
			Object_Property_Set (ezdev->kList1, "OnChange", "DevListViewOnChange");
			Object_Property_Set (ezdev->kImageParticleFont, "IsHidden", "True");
			Object_Property_Set (ezdev->kRectColor, "IsHidden", "True");

		}
#if 0
		for (int effectnum = 0; effectnum < MAX_PARTICLETEXTURES_256; effectnum ++) {
			float s0 = -999, t0= -999, s1 = -999, t1 = -999;
			qbool ok = ParticleFont_Get_Index_Was_Ok(effectnum, &s0, &t0, &s1, &t1);
			if (!ok) {
				// Happens? Yes with 128 size particle font (Intermap)
				stringlistappendf	(&ezdev->listaccum, "%d", effectnum);
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
			} else {
				stringlistappendf	(&ezdev->listaccum, "%d", effectnum);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", s0);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", t0);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", s1);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", t1);
			}
		} // for
#endif

	case_break lw_shaderlist_24:
		numcolumns = 3; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,189,200"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Filename,pk3");

		for (int filenum = 0; filenum < ezdev->list_scripts_dot_shader.numstrings; filenum ++) {
			ccs *thisfile_plus_pk3 = ezdev->list_scripts_dot_shader.strings[filenum];

			ccs *scriptfile_za	= String_Instance_Base1_ZAlloc (thisfile_plus_pk3, CHAR_TAB_9, /*2nd instance*/ 1, /*reply*/ NULL);
			ccs *pk3name_za		= String_Instance_Base1_ZAlloc (thisfile_plus_pk3, CHAR_TAB_9, /*2nd instance*/ 2, /*reply*/ NULL);
			stringlistappendf	(&ezdev->listaccum, "%d", filenum);
			stringlistappend	(&ezdev->listaccum, scriptfile_za);
			stringlistappend	(&ezdev->listaccum, pk3name_za);
			Mem_FreeNull_ (scriptfile_za);
			Mem_FreeNull_ (pk3name_za);
		} // for

	case_break lw_shaderlist_warnings_25:
		numcolumns = 2; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,400"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Line");

		for (int linenum = 0; linenum < ezdev->list_shaderwarnings.numstrings; linenum ++) {
			ccs *this_line = ezdev->list_shaderwarnings.strings[linenum];
			stringlistappendf	(&ezdev->listaccum, "%d", linenum);
			stringlistappend	(&ezdev->listaccum, this_line);
		} // for

	case_break lw_constants_26:
		numcolumns = 4; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,50,200,150"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "Row,Category,Constant,Value");
		{
			int constants_fill_stringlist_columns_4 (stringlist_t *plist, int index);
			for (int n = 0; n < 10; n ++) {
				constants_fill_stringlist_columns_4 (&ezdev->listaccum, n);
			} // for
		}

	case_break lw_quakec_builtins_27:
		numcolumns = 2; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,250"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Function");
		{
			int sv_builtins_stringlist_columns_2 (stringlist_t *plist);
			sv_builtins_stringlist_columns_2 (&ezdev->listaccum);
		}

	case_break lw_csqc_builtins_28:
		numcolumns = 2; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,250"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Function");
		{
			int csqc_builtins_stringlist_columns_2 (stringlist_t *plist);
			csqc_builtins_stringlist_columns_2 (&ezdev->listaccum);
		}

	case_break lw_menu_builtins_29:
		numcolumns = 2; can_be_text_filtered_have_data = true;
		Object_Property_Set		(ezdev->kList1, "ColumnWidths", "30,250"); // SHADERS
		Object_Property_Set		(ezdev->kList1, "ColumnHeaders", "#,Function");
		{
			int menuqc_builtins_stringlist_columns_2 (stringlist_t *plist);
			menuqc_builtins_stringlist_columns_2 (&ezdev->listaccum);
		}

	case_break lw_zircfeatures_30:		can_be_text_filtered_have_data = true; DOFILE("engine/zircon_features.txt");
	case_break lw_zircdev_31:			can_be_text_filtered_have_data = true; DOFILE("engine/zircon_developer.txt");
	case_break lw_zircpreproces_32:		can_be_text_filtered_have_data = true; DOFILE("engine/zircon_preprocessor.txt");
	} // sw

	// Determine what buttons to hide
	if (1) {
		// Reversed logic since "is hidden" is opposite of is visible
		ezdev->kButtonMarker->is_hidden = !(isin3(tab_idx, lw_entities_0, lw_textures_3, lw_csqc_ents_9) && can_be_text_filtered_have_data);
		ezdev->kButtonMarkerClear->is_hidden = !(isin3(tab_idx, lw_entities_0, lw_textures_3, lw_csqc_ents_9) && can_be_text_filtered_have_data);

		int can_folder = 0;
		can_folder |= isin5(tab_idx, lw_models_1, lw_sounds_2, lw_textures_3, lw_shaders_4, lw_map_7);
		can_folder |= isin4(tab_idx, lw_quake_rc_14, lw_default_cfg_15, lw_autoexec_cfg_16, lw_config_cfg_17);
		can_folder |= isin4(tab_idx, lw_map_ents_18, lw_rtlights_19, lw_music_20, lw_particlefont_21);
		can_folder |= isin3(tab_idx, lw_effectinfo_list_22, lw_effectinfo_txt_23, lw_shaderlist_24);
		can_folder |= isin1(tab_idx, lw_shaderlist_warnings_25);
		qbool is_folder_available = can_folder && can_be_text_filtered_have_data;
		ezdev->kButtonFolder->is_hidden = (is_folder_available == false);
// lw_quake_rc_14, lw_default_cfg_15, lw_autoexec_cfg_16,lw_config_cfg_17,lw_map_ents_18,lw_rtlights_19, lw_particlefont_21, lw_effectinfo_list_22, lw_effectinfo_txt_23
		ezdev->kButtonPlaySound->is_hidden = !(isin2(tab_idx, lw_sounds_2, lw_music_20) && can_be_text_filtered_have_data);
		ezdev->kButtonStopSound->is_hidden = !(isin2(tab_idx, lw_sounds_2, lw_music_20) && can_be_text_filtered_have_data);
	}

	// PROCESS numcolumns and LIST ACCUM
	Object_Property_Set_Fmt	(ezdev->kList1, "ColumnCount", "%d", numcolumns);

	ezdev->f->userdata = (int)can_be_text_filtered_have_data;
	ezdev->f->userdata2 = 0; // have we populated the data once (deny "no data" changes from hitting again)
	if (can_be_text_filtered_have_data)
		Object_Property_Set (ezdev->kTxtFilter, "BackColor", "#fff"); // Set to nothing.
	else
		Object_Property_Set (ezdev->kTxtFilter, "BackColor", "#fcc"); // Set to nothing.

	// Baker: If can't be filtered, no point in sorting right?
	ezdev->kList1->is_sorted = can_be_text_filtered_have_data;

	DevTxtFilterOnChange (ezdev->kTxtFilter); // This propogates it now.


	return OK_NOERROR_NULL;
}


RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)

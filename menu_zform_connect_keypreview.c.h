// menu_zform_connect_keypreview.c.h



RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange)
RELATED_ (Form_Event_Onload onkeypreview_a)
// OnKeyPreview - I think the idea here is we return true (1) if we handled the event

// Occurs before hidden is toggled.
EXO___ int DevTxtFilterBeforeClose (ktrl k)
{
	// k is the textbox
	if (k->is_hidden == false) {
		// We will be closed -- so TALL the List2
		ezdev->kList2->cm._relative_rect_orig.height = 320; // Make it tall
		//Object_Property_Set (ezdev->kList2, "Rect", "447,37,165,320");
	} else {
		// We will be shown
		ezdev->kList2->cm._relative_rect_orig.height = 298; // Make it short
		//Object_Property_Set (ezdev->kList2, "Rect", "447,37,165,298");
	}
	Form_QueueRefreshPlusMetric (k->fctrl); // KICK IT
	return false; // I assume we would return false to cancel?
}

int Folder_Fail_File_With_Return_Value (ccs *filename, ccs *failuredescribeitem, int return_value)
{
	if (!failuredescribeitem)
		failuredescribeitem = filename;
	Cbuf_AddTextLinef (cmd_local, "play %s", sv_sound_watersplash.string); // FAILURE SOUND
	Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Folder open %s impossible, no file exists\" silent", failuredescribeitem);
	return return_value;
}


int Folder_Open_File_With_Return_Value (ccs *filename, ccs *failuredescribeitem, int return_value)
{
	ccs *s_which_file = FS_Which_Or_Null(filename);

	if (!s_which_file) {
		return Folder_Fail_File_With_Return_Value (filename, failuredescribeitem, return_value);
	}

	// CONSTRUCT REAL PATH
	va_super (s_which_file_unix, MAX_OSPATH_EX_1024, "%s", s_which_file);
	File_URL_Edit_SlashesForward_Like_Unix (s_which_file_unix);

	ccs *realpath_unix = NULL;
	char vabuf[MAX_OSPATH_EX_1024];

	if (String_Ends_With(s_which_file_unix, "/")) {
		c_dpsnprintf2 (vabuf, "%s%s", /*folder*/ s_which_file_unix, filename);
		realpath_unix = vabuf;
	} else {
		realpath_unix = s_which_file_unix;
	}

	RELATED_ (SCR_Folder_f)
	// Nothing right?  We can handle it now.
	Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Folder open %s\" silent", realpath_unix);
	Cbuf_AddTextLinef (cmd_local, "folder realpath \"%s\"", realpath_unix); // quoted in case spaces in filename
	return return_value;
}

int Folder_Open_RealPathFile_Existing (ccs *realpath_unix_existing, int return_value)
{
	RELATED_ (SCR_Folder_f)
	// Nothing right?  We can handle it now.
	Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Folder open %s\" silent", realpath_unix_existing);
	Cbuf_AddTextLinef (cmd_local, "folder realpath \"%s\"", realpath_unix_existing); // quoted in case spaces in filename
	return return_value;
}

int KeyFunctionCentral (ktrl f, int is_km_none, int is_km_ctrl, int is_km_ctrl_shift, int scancode)
{
	// ONLY ALLOW THE RIGHT KEYS COMBOS TO GET HERE
	int tab_idx = ezdev->kTabSelect1->selectedindex;

	if (is_km_ctrl && scancode == 'x') { // Ctrl+X Close devinfo
		menu_state_reenter = 0; // ZERO FOOL!
		KeyDest_Set (key_game); // key_dest = key_game;
		menu_state_set_nova (m_none);
		return /*absorbed*/ true; // ?
	}

	if (is_km_ctrl && scancode == 'h') { // Ctrl+H hide text filter and resize is ALWAYS available
		// BEFORE SHOWLIST3 is fired.
		if (ezdev->kTxtFilter->is_hidden == false) {
			// We will be closed -- so TALL the List2
			ezdev->kList2->cm._relative_rect_orig.height = 320; // Make it tall
			//Object_Property_Set (ezdev->kList2, "Rect", "447,37,165,320");
		} else {
			// We will be shown
			ezdev->kList2->cm._relative_rect_orig.height = 298; // Make it short
			//Object_Property_Set (ezdev->kList2, "Rect", "447,37,165,298");
		}
		Form_QueueRefreshPlusMetric (f); // KICK IT
		ezdev->kTxtFilter->is_hidden = !ezdev->kTxtFilter->is_hidden;
		return /*absorbed*/ true; // ?
	}

	if (is_km_ctrl && scancode == 'f') { // Ctrl+F is always available
		if (ezdev->f->frm.kfocused != ezdev->kTxtFilter) {
			// Because key could repeat ...
			if (ezdev->kTxtFilter->is_hidden) {
				// We will be shown
				ezdev->kList2->cm._relative_rect_orig.height = 298; // Make it short
				Form_QueueRefreshPlusMetric (f); // KICK IT
				ezdev->kTxtFilter->is_hidden = false;
			}
			Oject_Focus_Set (ezdev->kTxtFilter); // Queue refresh or no?
		} else {
			// Pressing CTRL+F with focus clears it.
			// Ok this triggers an update but how?
			O_TextBox_SetText (ezdev->kTxtFilter, "");
		}
		return /*absorbed*/ true; // ?
	}

	if (is_km_ctrl && scancode == 'o' && isin1(tab_idx, lw_particlefont_21) && ezdev->kButtonFolder->is_hidden == false) { // Ctrl+O FOLDER OPEN?
		// LIVE: We have to try .tga then .png then .jpeg then .jpg then .pcx then .lmp
		ccs *texturename = "particles/particlefont";
		ccs *fmt_string = Image_Locate_Found_Format_String(texturename);
		if (NULL == fmt_string)
			return Folder_Fail_File_With_Return_Value (texturename, /*describe*/ NULL, /*absorbed*/ true);

		va_super (s_image_with_extension, MAX_OSPATH_EX_1024, fmt_string, texturename);
		return Folder_Open_File_With_Return_Value (s_image_with_extension, /*describe*/ NULL, /*absorbed*/ true);
	}

	int row = ezdev->kList1->selectedindex;
	int numcolumns = ezdev->kList1->columncount;
	int row_idx = row * numcolumns;
	int row_idx_col0 = row_idx + /*col*/ 0;
	qbool is_empty_data = (false == in_range_beyond(0,row_idx_col0,ezdev->kList1->list_strings_a.numstrings));

	// No selectable row for target action
	if (is_empty_data) {
		return /*absorbed*/ true; // ?
	}

	// If no data none of these should do anything.
	qbool bcan_be_text_filtered_have_data = (int)ezdev->f->userdata; // Different var name for search
	if (bcan_be_text_filtered_have_data == false)
		return /*absorbed handled_false*/ true;

		//can_folder |= isin4(tab_idx, lw_quake_rc_14, lw_default_cfg_15, lw_autoexec_cfg_16, lw_config_cfg_17);
		//can_folder |= isin4(tab_idx, lw_map_ents_18, lw_rtlights_19, lw_music_20, lw_particlefont_21);
		//can_folder |= isin3(tab_idx, lw_effectinfo_list_22, lw_effectinfo_txt_23, lw_shaderlist_24);
		//can_folder |= isin1(tab_idx, lw_shaderlist_warnings_25);
	if (is_km_ctrl && scancode == 'o' &&
			(isin4(tab_idx, lw_quake_rc_14, lw_default_cfg_15, lw_autoexec_cfg_16, lw_config_cfg_17)
			|| isin4(tab_idx, lw_map_ents_18, lw_rtlights_19, lw_effectinfo_list_22, lw_effectinfo_txt_23) )

			&& ezdev->kButtonFolder->is_hidden == false) { // Ctrl+O FOLDER OPEN?
		return Folder_Open_File_With_Return_Value (ezdev->folderopentarget, /*describe*/ NULL, /*absorbed*/ true);
	}

	//if (is_km_ctrl && scancode == 'w' && ezdev->kList3->is_hidden == (int)true && isin2(tab_idx, lw_shaders_4, lw_effectinfo_list_22) && ezdev->kButtonShowList3->is_hidden == (int)false) {
	//	ezdev->kList3->is_hidden = false;
	//	ezdev->kButtonShowList3->is_hidden = true;

	//	return /*absorbed*/ true; // ?
	//}

	if (is_km_ctrl && scancode == 'o' && isin1(tab_idx, lw_shaders_4) && ezdev->kButtonFolder->is_hidden == false) { // Ctrl+O FOLDER OPEN?
		// Unlike the others, we have to live try.
		ccs *scriptfile = ezdev->kList1->list_strings_a.strings[row_idx_col0 + 3];
		ccs *pk3name = ezdev->kList1->list_strings_a.strings[row_idx_col0 + 4];
		va_super (scripts_scriptfile, MAX_QPATH_128, "scripts/%s", scriptfile);
		ccs *filename = scripts_scriptfile;

		if (pk3name[0])
			filename = pk3name;

		ccs *describe	= ezdev->kList1->list_strings_a.strings[row_idx_col0 + 1];
		return Folder_Open_File_With_Return_Value (filename, describe, /*absorbed*/ true);
	}

	if (is_km_ctrl && scancode == 'o' && isin4(tab_idx, lw_models_1, lw_sounds_2, lw_textures_3, lw_music_20) && ezdev->kButtonFolder->is_hidden == false) { // Ctrl+O FOLDER OPEN?
		ccs *realpath_unix	= ezdev->kList1->list_strings_a.strings[row_idx_col0 + 3];
		if (realpath_unix[0] == CHAR_NULL_0) {
			return Folder_Fail_File_With_Return_Value (NULL, "No file associated", /*absorbed*/ true);
		}
		return Folder_Open_RealPathFile_Existing (realpath_unix, /*absorbed*/ true);
	} // models/sounds/textures/music + CTRL O - open folder

	if (is_km_ctrl && scancode == 'w' && isin2(tab_idx, lw_shaders_4, lw_effectinfo_list_22)) {
		ezdev->kList3->is_hidden = !ezdev->kList3->is_hidden;
		ezdev->kButtonShowList3->is_hidden = !ezdev->kButtonShowList3->is_hidden;

		return /*absorbed*/ true; // ?
	}

	if (is_km_ctrl && scancode == 'm' && isin1(tab_idx, lw_textures_3) && ezdev->kButtonMarker->is_hidden == false) { // Ctrl+M DOWN
		model_t		*m				= r_refdef.scene.worldmodel;
		if (!m || !m->surfmesh.num_triangles) {
			return /*absorbed*/ true; // Somehow no world to search
		}

		// FIND TEXTURE
		ccs			*s_textureindex = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		int			textureindex	= atoi(s_textureindex);

		texture_t	*t				= &m->data_textures[textureindex];
		ccs			*texturename	= t->name;

		char animatedname[1024]; // For finding animated textures

		//ccs		*swanted = Cmd_Argv (cmd, 1);
		//ccs		*swanted1 = NULL; // 2nd check

		qbool is_q1_animated_texture = texturename[0] == '+';

		if (is_q1_animated_texture && texturename[1] && texturename[2]) {
			c_dpsnprintf1 (animatedname, "+0%s", &texturename[2]);
			texturename = animatedname; // REROUTE
		}

		qbool Mod_TextureFindPos_Did_Find_Is_Ok (model_t *model, ccs *swanted, vec3_t org_reply, int *p_submodel);

		vec3_t org_found = {0,0,0};
		int submodel_num = 0;
		ccs *texturename_short = File_URL_SkipPath(texturename);

		qbool did_find_ok = Mod_TextureFindPos_Did_Find_Is_Ok(m, texturename, org_found, &submodel_num);
		if (!did_find_ok) {
			Cbuf_AddTextLine	(cmd_local, "tool_marker 0");
			Cbuf_AddTextLinef	(cmd_local, "infobar 3 \"No instances of texture %s were found\" silent", texturename_short);
			return /*absorbed*/ true;
		}

		//va_super (texturename_short_submodel, MAX_QPATHX2_256, "%s Submodel %d", texturename_short
		if (submodel_num == 0) {
			Cbuf_AddTextLinef (cmd_local, "tool_marker " VECTOR3_G " " QUOTED_S, VECTOR3_SEND(org_found), texturename);
			Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Set texture marker at " VECTOR3_G " for %s\" silent", VECTOR3_SEND(org_found), texturename_short);
		}
		else {
			va_super (desc, 512, "%s Submodel %s%d", texturename, "*", submodel_num);
			Cbuf_AddTextLinef (cmd_local, "tool_marker " VECTOR3_G " " QUOTED_S, VECTOR3_SEND(org_found), desc);
			Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Set texture marker at " VECTOR3_G " for %s submodel %s%d\" silent", VECTOR3_SEND(org_found), texturename_short, "*", submodel_num);
		}

		return /*absorbed*/ true;
	} // Ctrl+M

	if (is_km_ctrl && scancode == 'm' && isin2(tab_idx, lw_entities_0, lw_csqc_ents_9) && ezdev->kButtonMarker->is_hidden == false) { // Ctrl+M DOWN
		ccs *s_edict_num = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		int edict_num = atoi(s_edict_num);
		if (isin2(edict_num, 0, 1) && tab_idx == lw_entities_0) {
			Cbuf_AddTextLinef (cmd_local, "play %s", sv_sound_watersplash.string); // FAILURE SOUND
			Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Cannot set server marker on world #0 or player #1\" silent");
			return /*absorbed*/ true;
		} // world or player

		if (isin1(edict_num, 0) && tab_idx == lw_csqc_ents_9) {
			Cbuf_AddTextLinef (cmd_local, "play %s", sv_sound_watersplash.string); // FAILURE SOUND
			Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Cannot set csqc marker on world #0\" silent");
			return /*absorbed*/ true;
		} // world or player

		Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Set marker on edict #%s\" silent", s_edict_num);
		if (tab_idx == lw_csqc_ents_9)	Cbuf_AddTextLinef (cmd_local, "tool_marker client %s", s_edict_num);
		else							Cbuf_AddTextLinef (cmd_local, "tool_marker %s", s_edict_num);

		return /*absorbed*/ true;
	} // Ctrl+M

	if (is_km_ctrl_shift && scancode == 'm' && isin2(tab_idx, lw_entities_0, lw_csqc_ents_9) && ezdev->kButtonMarkerClear->is_hidden == false) { // Ctrl+Shift+M DOWN
		//ccs *s_edict_num = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		Cbuf_AddTextLine	(cmd_local, "infobar 3 \"Marker cleared\" silent");
		Cbuf_AddTextLine	(cmd_local, "tool_marker 0");
		return /*absorbed*/ true;
	} // Ctrl+Shift+M

	if (is_km_none && scancode == K_ESCAPE && isin2(tab_idx, lw_sounds_2, lw_music_20) && ezdev->kButtonStopSound->is_hidden == false) { // ESCAPE STOP SOUND DOWN
		Cbuf_AddTextLine	(cmd_local, "infobar 3 \"Stopping sound\" silent");
		Cbuf_AddTextLine	(cmd_local, "stopsound");
		return /*absorbed*/ true;
	} // music + ENTER

	if (is_km_none && scancode == K_ENTER && isin2(tab_idx, lw_sounds_2, lw_music_20) && ezdev->kButtonPlaySound->is_hidden == false) { // ENTER + sound file
		ccs *musictrackname = ezdev->kList1->list_strings_a.strings[row_idx_col0 + 1];
		Cbuf_AddTextLinef	(cmd_local, "infobar 3 \"Playing %s\" silent", musictrackname);
		Cbuf_AddTextLinef	(cmd_local, "stopsound; play %s", musictrackname);
		return /*absorbed*/ true;
	} // music + ENTER



	return /*absorbed*/ false;
}

EXO___ int DevFormOnKeyPreview (ktrl f, int scancode, int unicode, int is_down)
{
	// We are only acting on key downs - they might repeat if held.
	if (is_down == false)
		return /*absorbed handled_false*/ false;

	//Form_Get(f,k);
	// If no data none of these should do anything.
	qbool bcan_be_text_filtered_have_data = (int)ezdev->f->userdata; // Different var name for search
	//int val = KM_NONE;
	if (KM_NONE && in_range ('A', scancode, 'z') && bcan_be_text_filtered_have_data &&
		isin3 (f->frm.kfocused, ezdev->kList1, ezdev->kList2, ezdev->kList3)) {
		ktrl k = f->frm.kfocused;
		int numcolumns = k->columncount;
//		int startidx = k->selectedindex;
		int rowcount = Lister_ListCountY(k);
		int startrow = k->selectedindex;
		if (false == in_range_beyond (0, startrow, rowcount)) {
			startrow = 0;
		}
		int iters = 0;
		for (int myrow = startrow + 1; iters < rowcount; myrow ++, iters ++) {
			if (myrow >= rowcount)
				myrow = 0;
			int cell_idx = myrow * numcolumns + 1; // Column 1 is column2
			ccs *sxy = k->list_strings_a.strings[cell_idx];
			int firstcharlower = tolower(sxy[0]);
			if (firstcharlower == scancode) {
				// what else?
				O_ListView_SelectedIndex_Set (k, myrow);
				if (k->eve.onchange_a && k->eve.onchange_a[0]) {
					exofn_t onclick_fn = Event_Find_Function(k->eve.onchange_a);
					if (onclick_fn) {
						char *s_error_za = onclick_fn (k);
						if (s_error_za) {
							Con_PrintLinef ("ERROR: %s: " QUOTED_S, k->eve.onchange_a, s_error_za);
							Z_FreeNull_ (s_error_za);
						} // if error
					}
				}
				break;
			}
		}

		return /*handled_false*/ false;
	}

	qbool is_considered = false;

	//if (KM_CTRL_SHIFT && scancode == 'x') {  // Baker: Mar 9 2026 - Something blocks or eats CTRL+SHIFT+X
	if (KM_CTRL && scancode == 'x') is_considered = true;		// Ctrl+X			Close "devinfo"
	if (KM_CTRL && scancode == 'h') is_considered = true;		// Ctrl+H			Hide find box
	if (KM_CTRL && scancode == 'f') is_considered = true;		// Ctrl+F			Activate find box
	if (is_considered) goto always; // These always occur

	if (bcan_be_text_filtered_have_data == false)
		return /*absorbed handled_false*/ false;

	if (KM_CTRL && scancode == 'w') is_considered = true;		// Ctrl+W			Window Show
	if (KM_CTRL && scancode == 'm') is_considered = true;		// Ctrl+M			MARKER
	if (KM_CTRL_SHIFT && scancode == 'm') is_considered = true;	// Ctrl+Shift+M		MARKER CLEAR
	if (KM_NONE && scancode == K_ESCAPE) is_considered = true;	// ESCAPE			for stopsound
	if (KM_NONE && scancode == K_ENTER) is_considered = true;	// ENTER			Playsound
	if (KM_CTRL && scancode == 'o') is_considered = true;		// Ctrl+O			Open Folder.

	// Not one of the special keys
	if (is_considered == false)
		return /*absorbed handled_false*/ false;

always:
	return KeyFunctionCentral (f, KM_NONE, KM_CTRL, KM_CTRL_SHIFT, scancode);

	return /*handled_false*/ false;
}

EXO___ int DevListViewOnKeyPreview (ktrl k, int scancode, int unicode, int is_down)
{
//	// We are only acting on key downs - they might repeat if held.
//	if (is_down == false)
//		return /*absorbed handled_false*/ false;
//
//	qbool is_considered = false;
//	if (KM_CTRL && scancode == 'm') is_considered = true;		// Ctrl+M			MARKER
//	if (KM_CTRL_SHIFT && scancode == 'm') is_considered = true;	// Ctrl+Shift+M		MARKER CLEAR
//	if (KM_NONE && scancode == K_ESCAPE) is_considered = true;	// ESCAPE			for stopsound
//	if (KM_NONE && scancode == K_ENTER) is_considered = true;	// ENTER			Playsound
//	if (KM_CTRL && scancode == 'o') is_considered = true;		// Ctrl+O			Open Folder.
//
//	// Not one of the special keys
//	if (is_considered == false)
//		return /*absorbed handled_false*/ false;
//
//	KeyC
//
//	int tab_idx = ezdev->kTabSelect1->selectedindex;
//	int row = ezdev->kList1->selectedindex;
//	int numcolumns = ezdev->kList1->columncount;
//	int row_idx = row * numcolumns;
//	int row_idx_col0 = row_idx + /*col*/ 0;
//	qbool is_empty_data = (false == in_range_beyond(0,row_idx_col0,ezdev->kList1->list_strings_a.numstrings));
//
//	// No selectable row for target action
//	if (is_empty_data) {
//		return /*absorbed*/ false; // ?
//	}
//
//	if (KM_CTRL && scancode == 'm' && isin2(tab_idx, lw_entities_0, lw_csqc_ents_9)) { // Ctrl+M DOWN
//		ccs *s_edict_num = ezdev->kList1->list_strings_a.strings[row_idx_col0];
//		int edict_num = atoi(s_edict_num);
//		if (isin2(edict_num, 0, 1)) {
//			Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Cannot set marker on world #0 or player #1\" silent");
//			return /*absorbed*/ true;
//		} // world or player
//
//		Cbuf_AddTextLinef (cmd_local, "infobar 3 \"Set marker on edict #%s\" silent", s_edict_num);
//		if (tab_idx == lw_csqc_ents_9)	Cbuf_AddTextLinef (cmd_local, "tool_marker client %s", s_edict_num);
//		else							Cbuf_AddTextLinef (cmd_local, "tool_marker %s", s_edict_num);
//
//		return /*absorbed*/ true;
//	} // Ctrl+M
//
//	if (KM_CTRL_SHIFT && scancode == 'm' && isin2(tab_idx, lw_entities_0, lw_csqc_ents_9)) { // Ctrl+Shift+M DOWN
//		ccs *s_edict_num = ezdev->kList1->list_strings_a.strings[row_idx_col0];
//
//		Cbuf_AddTextLinef (cmd_local, "tool_marker 0", s_edict_num);
//		return /*absorbed*/ true;
//	} // Ctrl+Shift+M
//
//	if (KM_NONE && scancode == K_ESCAPE && isin1(tab_idx, lw_music_20)) { // ESCAPE STOP SOUND DOWN
//		Cbuf_AddTextLinef (cmd_local, "stopsound");
//		return /*absorbed*/ true;
//	} // music + ENTER
//
//	if (KM_NONE && scancode == K_ENTER && isin1(tab_idx, lw_music_20)) { // ENTER + sound file
//		ccs *musictrackname = ezdev->kList1->list_strings_a.strings[row_idx_col0 + 1];
//		Cbuf_AddTextLinef (cmd_local, "stopsound; play %s", musictrackname);
//		return /*absorbed*/ true;
//	} // music + ENTER
//
//	if (KM_CTRL && scancode == 'o' && isin1(tab_idx, lw_music_20)) { // Ctrl+O FOLDER OPEN?
//		ccs *musictrackname = ezdev->kList1->list_strings_a.strings[row_idx_col0 + 1];
//		ccs *pk3name		= ezdev->kList1->list_strings_a.strings[row_idx_col0 + 2];
//		qbool is_pak		= strlen(pk3name) == 0;
//		//if (false == FS_IsRealFile(s_filename)) {
//		//	Cbuf_AddTextLinef (cmd_local, "echo file %s is in a pak; play %s", s_filename, sv_sound_watersplash.string);
//		//} else {
//		va_super (s_path_nofilename, MAX_QPATH_128, "%s", is_pak ? pk3name : musictrackname);
//#ifdef _WIN32
//		// Nothing right?  We can handle it now.
//#else
//		File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (s_path_nofilename);
//#endif
//		Cbuf_AddTextLinef (cmd_local, "folder %s", s_path_nofilename);
//		return /*absorbed*/ true;
//	} // music + "f" key

	return /*handled_false*/ false;
}
//
////
#define UI_String(x) (x ? x : "");
EXO___ int DevPopUpEnded (ktrl f, ktrl k_initiator)
{
	ktrl k = f->frm.kpopup_initiator;
	//PSTRING question	= UI_String(f->popup_question_a);	// Does not have to be filled
	PSTRING answer		= UI_String(f->popup_answer_a);	// Always filled
	int slen = istrlen(answer);
	//Object_Property_Set (f->frm.kpopup_initiator/*ezdev->kList1*/, "ToolTip", answer);

	if (slen == 0) {
		// Cancelled.
		return true;
	}

	if (k == ezdev->kTxtFilter) {
		if (String_Match (answer, "Clear")) {
			RELATED_ (O_TextBox_KeyDown O_TextBox_Draw)
			//Object_Property_Set (ezdev->kTxtFilter, "Text", "");

			// Baker: March 6 2026 more complex than it should be.
			// Setting string via property set will turn "" to NULL and then we get engine crash in
			// O_TextBox_Draw and cursor functions.
			// Property free will dealloc if not null so even "" strings get cleared.
			// The above "problem" is to avoid setting all possible string properties to "" which requires a pointless allocation
			// QuakeC does not have this problem
			// The engine code for Zircon UI also does not support unicode

			Z_StrDup_Realloc (&k->text_a, ""); // Avoid Object_Property_Set nulling
			KP(txta.cursor_pos) = 0;
			KP(txta.cursor_length) = 0;
			KP(r_scrollport.left) = 0;
			KP(r_scrollport.top) = 0;
			DevTxtFilterOnChange (ezdev->kTxtFilter);
			return true;
		}

	}

	// ContextMenu1
	//List:"Explore here ...,0,ExploreHere,,Copy Path ...,0,CopyPath,X,-,0,-,X,View Text,0,ViewText,X,-,0,-,X,Filename5,0,-,X"
	return true;
}

// PopUp Context Menu
RELATED_ (O_ExtCloser_MouseButtonAction Form_Mouse_Move)
EXO___ int DevMouseMoveAction (ktrl k, int mouseindex, int x, int y, int is_down)
{
	RELATED_ (O_ImageGrid_MouseButtonAction)
	while /*if*/ (k == ezdev->kGridParticles) {
		int newcol = ViewCalc_ColumnCollide (k, x, y);
		int newrow = ViewCalc_RowCollide (k, x, y);

		if (newcol == not_found_neg1 || newrow == not_found_neg1) {
			// Moving over a row or column header might do this
			break;
		}

		int newidx = newrow * KP(gridcolumns) + newcol;

		Object_Property_Set_Fmt (k, "ToolTip", "UP1ROW:Tex %d", newidx);
		ToolTip_Deploy_If_Target_ToolTipText (k->fctrl, k, x, y);
		//ToolTip_Deploy_If_Target_ToolTipText (f, k_over, x, y);

		// Figure out what we are over and set tooltip
		break;
	} // while if

	return /*absorbed */ false;
}

EXO___ int DevMouseButtonAction (ktrl k, int mouseindex, int x, int y, int is_down)
{
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonExit) {
		KeyFunctionCentral (k->fctrl, 0, 1, 0, 'x'); // "EMULATE" CTRL+X -- returns true
		return false; // ALLOW CLOSE TO OCCUR
	}

	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kList3->servo.kcloser) {
		// Q: Does this occur before or after the closer fires?
		// A: BEFORE
		RELATED_ (Form_MouseUp)
		//int is_hidden = ezdev->kList3->is_hidden;
		ezdev->kButtonShowList3->is_hidden = false;
		return false; // ALLOW CLOSE TO OCCUR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonShowList3) {
		// SHOW LIST3
		// BEFORE SHOWLIST3 is fired.
		//DevTxtFilterBeforeClose (ezdev->kButtonShowList3); // WHY?
		ezdev->kList3->is_hidden = false;
		ezdev->kButtonShowList3->is_hidden = true;

		// J
		RELATED_ (ToolTips_End_Any DevInit)
		//
		//if (f->servo.ktooltip->is_hidden == false)
		//	f->servo.ktooltip->is_hidden = true;
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonShowText && ezdev->kButtonShowText->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 1, 0, 'h'); // "EMULATE" Ctrl+H -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonMarker && ezdev->kButtonMarker->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 1, 0, 'm'); // "EMULATE" Ctrl+M -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonMarkerClear && ezdev->kButtonMarkerClear->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 0, 1, 'm'); // "EMULATE" Ctrl+Shift+M -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonPlaySound && ezdev->kButtonPlaySound->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 0, 0, K_ENTER); // "EMULATE" ENTER -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonStopSound && ezdev->kButtonStopSound->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 0, 0, K_ESCAPE); // "EMULATE" ESCAPE -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex == mouse_index_left_1 && is_down == false && k == ezdev->kButtonFolder && ezdev->kButtonFolder->is_hidden == false) {
		KeyFunctionCentral (k->fctrl, 0, 1, 0, 'o'); // "EMULATE" Ctrl+O -- returns true
		return false; // ALLOW NORMAL BEHAVIOR
	}
	if (mouseindex != mouse_index_right_2)	return /*handled false*/ false;

	//
	// MOUSE2 context menu ....
	//

	ccs *cmd_string = NULL;

	//#define	CM_0_TEXT_0							0		// ContextMenu	"-" is special
	//#define	CM_1_CHECKED_1						1		// Checked .. for value use string
	//#define	CM_2_STRING_ID_2					2		// String id
	//#define	CM_3_FUNCSTRING_3					3
	if (k == ezdev->kTxtFilter)		cmd_string = "Clear,0,Clear,,"	",,,";
	else if (k == ezdev->kList1)	cmd_string = "Copy,0,Copy,,"	",,,";

	if (cmd_string == NULL)	return /*handled false*/ false;

	Object_Property_Set (ezdev->kTxtFilter, "List", "Copy,0,Copy,," ",,,");
	RELATED_ (Form_Life) // A context menu is like a listbox
	Form_Get (f,k);

	RELATED_ (Form_Mouse2Down O_ContextMenu_Draw)
	f->frm.kpopup_initiator = k;
	Object_Property_Set			(f, "PopUpQuestion", f->frm.kpopup_initiator->cm.name_a);
	Object_Property_Set			(f, "PopUpQuestion", f->frm.kpopup_initiator->cm.name_a);

	O_ContextMenu_Popup_Method	(ezdev->kContextMenuX, /*reason*/ "DevMouseButtonAction");
	Object_Property_Set			(ezdev->kContextMenuX, "List", cmd_string);
	return /*handled_true*/ true;
}




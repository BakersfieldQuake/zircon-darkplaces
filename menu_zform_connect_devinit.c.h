// menu_zform_connect_devinit.c.h

// Baker: This is OnLoad and occurs after the form was loaded.

//void Mod_Shaders2_Query (stringlist_t *plist_accum); // RETIRED
RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)
extern cvar_t _scr_devinfo_reenter;

EXO___ char *DevInit (ktrl f)
{
	if (ezdev->listaccum.numstrings)
		stringlistfreecontents (&ezdev->listaccum);

	if (ezdev->list_scripts_dot_shader.numstrings)
		stringlistfreecontents (&ezdev->list_scripts_dot_shader);

	if (ezdev->list_shaders64.numstrings)
		stringlistfreecontents (&ezdev->list_shaders64);

	int preserve_try = ezdev->try_keep_indexes;
	memset (&_ezdev, 0, sizeof(_ezdev));
	ezdev->try_keep_indexes = preserve_try;

	RELATED_ ( Mod_Shaders2_Query)
	ShaderText_Atomize (&ezdev->list_scripts_dot_shader, &ezdev->list_shaders64);

	// Now audit it.

	// plist_shaders - column0 - shadername
	//                 column1 - scripts/theshader.name
	//                 column2 - the entire shader text

	stringlistsortcolumns_alpha (&ezdev->list_shaders64, LW_SHADER64_COLUMNS_3, /*sortcol:*/ 0);

	stringlistfreecontents (&ezdev->list_shaderwarnings);

	for (int row_idx = 0; row_idx < ezdev->list_shaders64.numstrings; row_idx += LW_SHADER64_COLUMNS_3) {
		ccs *sxy		= ezdev->list_shaders64.strings[row_idx + 0];
		ccs *sxy_file	= ezdev->list_shaders64.strings[row_idx + 1];
		ccs *sxy_script	= ezdev->list_shaders64.strings[row_idx + 2];
		int dups = 0;
		for (int row_idx2 = row_idx + LW_SHADER64_COLUMNS_3; row_idx2 < ezdev->list_shaders64.numstrings; row_idx2 += LW_SHADER64_COLUMNS_3) {
			ccs *sxy2			= ezdev->list_shaders64.strings[row_idx2 + 0];
			ccs *sxy_file2		= ezdev->list_shaders64.strings[row_idx2 + 1];
			ccs *sxy_script2	= ezdev->list_shaders64.strings[row_idx2 + 2];
			if (String_Match(sxy, sxy2)) {
#ifdef _DEBUG
				if (String_Match(sxy_file, sxy_file2)) {
					int j = 5;
				}
#endif // _DEBUG
				if (dups == 0) {
					stringlistappendf (&ezdev->list_shaderwarnings, "%s: %s has duplicates", sxy, sxy_file);
				}
				qbool is_identical = String_Match(sxy_script, sxy_script2);
				//if (is_identical == false) {
					stringlistappendf (&ezdev->list_shaderwarnings, "#%02d %s: %s has identical text = %d", dups + 1, sxy, sxy_file2, (int)is_identical);
				//}
				stringlistreplaceindex (&ezdev->list_shaders64, row_idx2 + 0, "");
				dups ++;
			}
			else
				break;
		} // for
		if (dups > 0)
			stringlistappendf (&ezdev->list_shaderwarnings, "%s has %d dups", sxy, dups);
	} // for

	//3 column sort

	// plist_shaders - column0 - shadername
	//                 column1 - scripts/theshader.name
	//                 column2 - the entire shader text

	// plist_scripts   column0 - scripts/theshader.name  |pk3

	// FIRST - Add the [PIPE]pak3
	for (int sidx = 0; sidx < ezdev->list_scripts_dot_shader.numstrings; sidx ++) {
		ccs *sxy = ezdev->list_scripts_dot_shader.strings[sidx];

		// Find out the pk3 that this is in, if any
		char *realpath_pak_za = FS_RealFilePath_Pak_Or_Null_Z_Alloc(sxy);
		ccs *filenamepk3 = realpath_pak_za ? realpath_pak_za : "";

		stringlistreplaceindexf (&ezdev->list_scripts_dot_shader, sidx, "%s%c%s", sxy, CHAR_TAB_9, filenamepk3);

		Mem_FreeNull_ (realpath_pak_za);
	}

	ezdev->f	 = f;
	ezdev->kTabSelect1 = Form_Find_Name (f, "TabSelect1");
	ezdev->kList1 = Form_Find_Name (f, "List1");
	ezdev->kList2 = Form_Find_Name (f, "List2");
	ezdev->kList3 = Form_Find_Name (f, "List3");
	
	ezdev->kButtonShowList3 = Form_Find_Name (f, "ButtonShowList3");
	ezdev->kButtonShowText = Form_Find_Name (f, "ButtonShowText");
	ezdev->kButtonExit = Form_Find_Name (f, "ButtonExit");

	ezdev->kImageParticleFont = Form_Find_Name (f, "ImageParticleFont");
	ezdev->kRectColor = Form_Find_Name (f, "RectColor");

	// If not re-entrant then don't show
	if (0 == _scr_devinfo_reenter.integer)
		ezdev->kButtonExit->is_hidden = true;

	ezdev->kButtonMarker = Form_Find_Name (f, "ButtonMarker");
		Object_Property_Set (ezdev->kButtonMarker, "ToolTip", "VALIGN:(Ctrl+M) Mark this entity/texture in 3D.");
	ezdev->kButtonMarkerClear = Form_Find_Name (f, "ButtonMarkerClear");
		Object_Property_Set (ezdev->kButtonMarkerClear, "ToolTip", "VALIGN:(Ctrl+Shift+M) Clear marked entity/texture \"tool_marker 0\".");
	ezdev->kButtonFolder = Form_Find_Name (f, "ButtonFolder");
		Object_Property_Set (ezdev->kButtonFolder, "ToolTip", "VALIGN:(Ctrl+O) Open \"folder\" and highlight file");
	ezdev->kButtonPlaySound = Form_Find_Name (f, "ButtonPlaySound");
		Object_Property_Set (ezdev->kButtonPlaySound, "ToolTip", "VALIGN:(Enter) \"Play\" this sound");
	ezdev->kButtonStopSound = Form_Find_Name (f, "ButtonStopSound");
		Object_Property_Set (ezdev->kButtonStopSound, "ToolTip", "VALIGN:(ESC) \"Stopsound\"");

	ezdev->kLabelEntries = Form_Find_Name (f, "LabelEntries");
	ezdev->kLabelTitle = Form_Find_Name (f, "LabelTitle");
	ezdev->kImageTexture = Form_Find_Name (f, "ImageTexture");

	//ezdev->kImageParticleFont = Form_Find_Name (f, "ImageParticleFont");
	//ezdev->kImageParticleFontCursor = Form_Find_Name (f, "ImageParticleFontCursor");

	ezdev->kGridParticles = Form_Find_Name (f, "GridParticles");
	ezdev->kLabelTexNum = Form_Find_Name (f, "LabelTexNum");
	
	ezdev->particle_cols_rows = 8;
	if (FS_FileExists("particles/particlefont.txt")) {
		// Now we need to read the file to memory and count the rows.
		char *s_za = FS_LoadFileQuick("particles/particlefont.txt");
		if (s_za) {
			stringlist_t list = {0};
			
			stringlistappend_split_lines_cr_scrub(&list, s_za);
			if (list.numstrings > 120) { // Using Arcane Dimensions and the cutoff more or less
				ezdev->particle_cols_rows = 16;
				
			}
			else {
				ezdev->particle_cols_rows = 8;
			}
			stringlistfreecontents (&list);
		}
		Mem_Free (s_za);
	}

	if (ezdev->particle_cols_rows == 16)
		Object_Property_Set (ezdev->kGridParticles, "RowHeaders", "0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240");
	else
		Object_Property_Set (ezdev->kGridParticles, "RowHeaders", "0,8,16,24,32,40,48,56");

	ezdev->kGridParticles->gridautosizecolumns = ezdev->particle_cols_rows;
	ezdev->kGridParticles->gridautosizerows = ezdev->particle_cols_rows;
	ezdev->kGridParticles->gridrows = ezdev->particle_cols_rows;
	ezdev->kGridParticles->gridcolumns = ezdev->particle_cols_rows;
	ezdev->kGridParticles->_gridcellwidth_orig = ezdev->particle_cols_rows == 16 ? 17 : 34;
	ezdev->kGridParticles->_gridcellheight_orig = ezdev->particle_cols_rows == 16 ? 17 : 34;


	//ezdev->particle_cell_size = ezdev->kImageParticleFont->cm._relative_rect_orig.width / ezdev->particle_cols_rows;
	//ezdev->particle_left = ezdev->kImageParticleFont->cm._relative_rect_orig.left;
	//ezdev->particle_top = ezdev->kImageParticleFont->cm._relative_rect_orig.top;

	ezdev->kTxtFilter = Form_Find_Name (f, "TxtFilter");
	RELATED_ (DevMouseButtonAction DevPopUpEnded)
	ezdev->kContextMenuX = Form_Find_Name (f, "ContextMenuX");

	RELATED_ (DevPopUpEnded)
	Object_Property_Set (ezdev->f, "OnPopUpEnded", "DevPopUpEnded");

	Object_Property_Set (ezdev->f, "OnKeyPreview", "DevFormOnKeyPreview"); // SET LISTVIEW1 OnKeyPreview

	Object_Property_Set (ezdev->kContextMenuX, "BackColor", "#000");
	Object_Property_Set (ezdev->kContextMenuX, "ForeColor", "#fff");
	Object_Property_Set (ezdev->kContextMenuX, "FontSize", "8");
#ifdef _DEBUG
	if (!ezdev->kTabSelect1 || !ezdev->kList1 || !ezdev->kList2 || !ezdev->kList3 || !ezdev->kButtonShowList3 ||
		!ezdev->kLabelEntries || !ezdev->kLabelTitle || !ezdev->kTxtFilter || !ezdev->kButtonShowText) {
		int j = 5; // This would be bad, detect it.
	}
#endif // _DEBUG

	{
		f->servo.ktooltip->is_hidden = true;
		f->servo.ktooltip->fontsize = f->fontsize;
		RELATED_ (O_Label_Draw ToolTip_Deploy)
		Object_Property_Set (f->servo.ktooltip, "BackColor", "#ccf");
		Object_Property_Set (f->servo.ktooltip, "ForeColor", "#000");
		Object_Property_Set (f->servo.ktooltip, "FontSize", "8");
		Object_Property_Set (f->servo.ktooltip, "BackAlpha", "1");

		Object_Property_Set (ezdev->kButtonExit, "ToolTip", "VALIGN:(Ctrl+X) Close \"devinfo\"");
		Object_Property_Set (ezdev->kButtonShowText, "ToolTip", "VALIGN:(Ctrl+H) Toggle text filter box");
		Object_Property_Set (ezdev->kButtonShowList3, "ToolTip", "VALIGN:(Ctrl+W) Show extra detail window");
		Object_Property_Set (ezdev->kTxtFilter, "ToolTip", "VALIGN:(Ctrl+F) Required words (caseless): shaders search shadertext also.  Quoted for literal");
	}

	// TABSELECT
	RELATED_ (lw_music_20)
	Object_Property_Set (ezdev->kTabSelect1, "List",
			"Entities,Models,Sounds,Textures,Shaders,"
			"Globals,Fields,Map,Environment,"
			"CSQC Ents,CSQC Globals,CSQC Fields,CSQC Stringlists,SSQC Stringlists,quake.rc,"
			"default.cfg,autoexec.cfg,config.cfg,Map .Entities,rtlights,/music Folder,"
			"particles/particlefont,effectinfo list,effectinfo.txt,"
			"Shaderlist,Shaderlist Duplicates,Constants,QuakeC Builtins,CSQC Builtins,Menu QC Builtins,Zircon Manual,Zircon Developer,Map Pre-Processor"
		);

	// Q: Mar 4 2026 - does this trigger event?
	// A: Does not look like it.  Events are user-input driven.
	// A: THIS WORKS but why does it work?
	// A: Form_Create_From_String ..
	//       OnLoad ==> DevInit occurs (set of index)
	//          OnChange Recursive ==> TabSelect fires OnChange
	Object_Property_Set (ezdev->kTxtFilter, "OnChange", "DevTxtFilterOnChange"); // Wire it up

#if 3926
	if (g_s_devinfo_argnum && g_s_devinfo_argnum[0]) {
		int argnum = atoi(g_s_devinfo_argnum);
		// Validate the range
		if (in_range_beyond(0, argnum, lw_obs_count_33))
			Object_Property_Set (ezdev->kTabSelect1, "SelectedIndex", g_s_devinfo_argnum);

		//ezdev->kTabSelect1->selectedindex = g_old_tab_idx;
	} else {
		int argnum = g_old_tab_idx;
		if (in_range_beyond(0, argnum, lw_obs_count_33))
			ezdev->kTabSelect1->selectedindex = argnum;//g_old_tab_idx;
//			Object_Property_Set (ezdev->kTabSelect1, "SelectedIndex", g_s_devinfo_argnum);

//		ezdev->kTabSelect1->selectedindex = g_old_tab_idx;

	}
#endif // 3926

	RELATED_ (ezdev->try_keep_indexes)
	g_s_devinfo_argnum = NULL;

	DevTabSelectOnChange (ezdev->kTabSelect1);
	return OK_NOERROR_NULL;
}

RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)

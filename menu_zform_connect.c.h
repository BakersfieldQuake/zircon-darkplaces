// menu_zform_connect.c.h



RELATED_ (Object_Class_Default_Values)
// Return code is OK_NULL for ok otherwise a zalloced string that is freed on use.
//void R_WorldTextures_Query (feed_fn_t myfeed_shall_stop);
//void CL_Models_Query (feed_fn_t myfeed_shall_stop);
//void CL_Sounds_Query (feed_fn_t myfeed_shall_stop);

void ShaderText_Atomize (stringlist_t *plist_scripts_dot_shader, stringlist_t *plist_shaders64);

typedef enum {
	lw_entities_0				= 0,
	lw_models_1					= 1,
	lw_sounds_2					= 2,
	lw_textures_3				= 3,
	lw_shaders_4				= 4,
	lw_globals_5				= 5,
	lw_fields_6					= 6,
	lw_map_7					= 7,
	lw_environment_8			= 8,
	lw_csqc_ents_9				= 9,
	lw_csqc_globals_10			= 10,
	lw_csqc_fields_11			= 11,
	lw_csqc_stringlist_12		= 12,
	lw_ssqc_stringlist_13		= 13,
	lw_quake_rc_14				= 14,
	lw_default_cfg_15			= 15,
	lw_autoexec_cfg_16			= 16,
	lw_config_cfg_17			= 17,
	lw_map_ents_18				= 18,
	lw_rtlights_19				= 19,
	lw_music_20 				= 20,
	lw_particlefont_21			= 21,
	lw_effectinfo_list_22		= 22,
	lw_effectinfo_txt_23		= 23,
	lw_shaderlist_24 			= 24,
	lw_shaderlist_warnings_25	= 25,
	lw_constants_26				= 26,
	lw_quakec_builtins_27		= 27,
	lw_csqc_builtins_28			= 28,
	lw_menu_builtins_29			= 29,
	lw_zircfeatures_30			= 30,
	lw_zircdev_31				= 31,
	lw_zircpreproces_32 		= 32,
	
	lw_obs_count_33				= 33,
} lw_e; // list_what



typedef struct {
	oject_ref_s		*f;
	oject_ref_s		*kTabSelect1; // The Tabselect
	oject_ref_s		*kList1;
	oject_ref_s		*kList2;
	oject_ref_s		*kList3; // Detail
	oject_ref_s		*kButtonExit;
	oject_ref_s		*kButtonShowList3;
	oject_ref_s		*kButtonShowText;

	oject_ref_s		*kImageParticleFont;
	oject_ref_s		*kRectColor;

	oject_ref_s		*kButtonMarker;
	oject_ref_s		*kButtonMarkerClear;
	oject_ref_s		*kButtonFolder;
	oject_ref_s		*kButtonPlaySound;
	oject_ref_s		*kButtonStopSound;

	oject_ref_s		*kImageTexture;
	//oject_ref_s	*kImageParticleFontCursor;
	oject_ref_s		*kLabelTexNum; // TexNum
	//oject_ref_s	*kImageParticleFont;
	oject_ref_s		*kGridParticles;

	int				particle_cols_rows;
	//float			particle_cell_size;
	//float			particle_left;
	//float			particle_top;

	oject_ref_s		*kTxtFilter;
	oject_ref_s		*kLabelEntries;
	oject_ref_s		*kLabelTitle;
	oject_ref_s		*kContextMenuX;

	stringlist_t	listaccum;				// This now hangs around for filtering purposes.

	RELATED_ (ShaderText_Atomize)
	stringlist_t	list_scripts_dot_shader; // 1 col

#define LW_SHADER64_COLUMNS_3 3
	stringlist_t	list_shaders64; // 3 colum

	stringlist_t	list_shaderwarnings;

	qbool			try_keep_indexes;
// _scr_devinfo_globalscap cvar replaced the field of similar name
	char folderopentarget[MAX_QPATH_128];
} ezdev_s;

ezdev_s _ezdev, *ezdev = &_ezdev;

ccs *g_s_devinfo_argnum;
int g_old_tab_idx = 0; //ezdev->kTabSelect1->selectedindex;
int g_old_listview1_idx =0; //ezdev->kList1->selectedindex;
int g_old_listview2_idx =0; //ezdev->kList2->selectedindex;
int g_old_listview3_idx =0; //ezdev->kList3->selectedindex;

RELATED_ ( Mod_Shaders2_Query)

// Baker: THIS IS PART OF DEVINFO.
void Map_Append_Pairs (stringlist_t *plist)
{
		stringlistappend2	(&ezdev->listaccum, "Map File", cl.worldname);
		stringlistappend2	(&ezdev->listaccum, "Title (message)", cl.worldmessage);
		stringlistappendf2	(&ezdev->listaccum, QUOTED_STR("sky"), "%s", g_skyname);
		stringlistappendf2	(&ezdev->listaccum, QUOTED_STR("fog"), "%f %f %f %f %f %f %f %f %f", r_refdef.fog_density, r_refdef.fog_red, r_refdef.fog_green, r_refdef.fog_blue, r_refdef.fog_alpha, r_refdef.fog_start, r_refdef.fog_end, r_refdef.fog_height, r_refdef.fog_fadedepth);
		stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

		if (sv.worldmodel) {
			ccs *stype = halflifebsp.integer ? "Half-Life 1" : sv_mapformat_is_quake2.integer ? "Quake 2" : sv_mapformat_is_quake3.integer ? "Quake 3" : String_Ends_With_Caseless (cl.worldname, ".obj") ? ".OBJ" : "Quake 1";
			stringlistappend2	(&ezdev->listaccum, "Server map format", stype);
			stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2
		}

va_super (entname, MAX_QPATH_128, "maps/%s.ent", cl.worldbasename); // DOT_ENT (1)
char *entities_a = (char *)FS_LoadFile(entname, tempmempool, fs_quiet_true, fs_size_ptr_null);
char *entities = AorB (entities_a, cl.worldmodel->brush.entities);

#if 1
		//fs_offset_t filesize = FS_FileSizeEx_Safe(cl.worldname);
		qfile_t *f = FS_OpenVirtualFile(cl.worldname, fs_quiet_true);
		if (f) {
			RELATED_ (SV_Map_f)
			fs_offset_t filesize = FS_FileSize (f);
			double filesize_dbl = (double)filesize / (1024 * 1024.0); // MB
			stringlistappendf2	(&ezdev->listaccum, "File size", "%3.1f MB", filesize_dbl);
		} else {
			stringlistappendf2	(&ezdev->listaccum, "File size", "(unable)");
		}
		FS_CloseNULL_ (f); // We don't need it open

		char maps_timestring[64];
		qbool datetime_ok = FS_File_TimeString_Fill_MM_DD_YY_HH_MM_AM_Safe (cl.worldname, maps_timestring, sizeof(maps_timestring));
		if (datetime_ok) {
			stringlistappendf2	(&ezdev->listaccum, "Map date", "%s", maps_timestring);

			double maptime1 = FS_FileTime_Since_1970_Safe(cl.worldname);

			va_super (s_dotmap, MAX_OSPATH_EX_1024, "%s", cl.worldnamenoextension);
			c_strlcat (s_dotmap, ".map");

			double maptime0 = FS_FileTime_Since_1970_Safe(s_dotmap);

			if (maptime0 && maptime1) {
				double deltatime = maptime1 - maptime0;
				int iminutes = (int64_t)( deltatime / 60);
				int iseconds = (int)(deltatime - iminutes * 60);
				stringlistappendf2	(&ezdev->listaccum, "Compile Time", "%d minutes %d seconds (%g seconds)", iminutes, iseconds, deltatime);
			} else {
				stringlistappend2	(&ezdev->listaccum, "Compile Time", "(no data for calculation because no .map)");
			}

		} else {
			stringlistappend2 	(&ezdev->listaccum, "Map date", "(no date)");
			stringlistappend2	(&ezdev->listaccum, "Compile Time", "(no data for calculation)");
		}

		stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2
#endif

		stringlistappendf2	(&ezdev->listaccum, "Supports wateralpha?", "%s", cl.worldmodel->brush.supportwateralpha ? "Yes" : "No");
		//stringlistappendf2	(&ezdev->listaccum, "Has visibility data?", "%s", cl.worldmodel->brush.num_pvsclusters != 0 ? "Yes" : "No");
		stringlistappendf2	(&ezdev->listaccum, "Has visibility data?", "%s", cl.worldmodel->brush.is_vised ? "Yes" : "No");
		stringlistappendf2	(&ezdev->listaccum, "Has light data?", "%s", r_refdef.scene.worldmodel->lit ? "Yes" : "No");

		va_super (levelshotname_jpg, MAX_QPATHX2_256, "levelshots/%s.jpg", cl.worldbasename);
		stringlistappendf2	(&ezdev->listaccum, "Has levelshots/mapname.tga?", "%s", cl.levelshotsname[0] ? cl.levelshotsname : "(No)");

		va_super (srt, MAX_QPATH_128, "maps/%s.rtlights", cl.worldbasename);
		int is_rts = FS_FileExists (srt);

		stringlistappendf2	(&ezdev->listaccum, "Using .rtlights?", "%s", is_rts ? srt : "(No)");


		//unused? int is_ents = FS_FileExists (entname);

		if (cl.cdtrackname[0]) {
			stringlistappendf2	(&ezdev->listaccum, "Background music?", "%s", cl.cdtrackname);
		} else {
			if (cl.cdtrack) {
				va_super (trackname, MAX_QPATH_128, "music/track%02u.ogg", cl.cdtrack);
				stringlistappendf2	(&ezdev->listaccum, "Background music?", "(No .. it would be %s)", trackname);
			} else {
				stringlistappend2	(&ezdev->listaccum, "Background music?", "(No and map specifies sounds 0 for none)");
			}
		}

		stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2


		stringlistappendf2	(&ezdev->listaccum, "Worldspawn keys:", "[from %s]", entities_a ? entname : cl.worldname);

		stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

		// appends key, value to list
		String_Worldspawn_Values_stringlistappend (&ezdev->listaccum, entities);
// KEEP
	Mem_FreeNull_ (entities_a);

		stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

		prvm_prog_t *prog = SVVM_prog;
		if (prog) {
			int enable_intermap = 0;
			int v_enable_intermap_offset = PRVM_ED_FindGlobalOffset(prog, "enable_intermap");
			if (v_enable_intermap_offset >= 0) {
				enable_intermap = PRVM_GLOBALFIELDFLOAT(v_enable_intermap_offset);
			} // if

			stringlistappendf2	(&ezdev->listaccum, "QC enable_intermap", "%d", enable_intermap);
			stringlistappendf2	(&ezdev->listaccum, "Intermap # Save map states", "# SIVS = %d", sv_intermap_siv_list.numstrings);
			stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

			if (enable_intermap) {
				stringlistappendf2	(&ezdev->listaccum, "+ QC time", "%f", sv.time);
				stringlistappendf2	(&ezdev->listaccum, "- QC surplustime", "%f", sv.intermap_surplustime);
				stringlistappendf2	(&ezdev->listaccum, "+ QC totaltimeatstart (of map)", "%f", sv.intermap_totaltimeatstart);

				stringlistappend2	(&ezdev->listaccum, "", "================");
				stringlistappendf2	(&ezdev->listaccum, "All levels time", "%f", (sv.time - sv.intermap_surplustime) + sv.intermap_totaltimeatstart );

				stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

				//Con_PrintLinef ("enable_intermap  = %d", have_intermap);
				stringlistappendf2	(&ezdev->listaccum, "QC startspot",  QUOTED_S, sv.intermap_startspot );
				stringlistappendf2	(&ezdev->listaccum, "QC startorigin",  VECTOR3_5d1F, VECTOR3_SEND(sv.intermap_startorigin) );

				stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

				stringlistappendf2	(&ezdev->listaccum, "sv.was_intermap_loaded_from_siv", "%d", sv.was_intermap_loaded_from_siv);
				stringlistappendf2	(&ezdev->listaccum, "total time at last map exit", "%f", sv.intermap_totaltimeatlastexit);

				stringlistappend2	(&ezdev->listaccum, "", ""); // DIV2

				if (sv_intermap_siv_list.numstrings) {
					stringlistappend2	(&ezdev->listaccum, "SIVS:", "");
					for (int idx = 0 ; idx < sv_intermap_siv_list.numstrings; idx += 2) {
						// .SIV data - Intermap saved game
						const char *sxy_map = sv_intermap_siv_list.strings[idx + 0];
						const char *sxy_siv = sv_intermap_siv_list.strings[idx + 1];
						stringlistappendf2	(&ezdev->listaccum, sxy_map, "idx %3d strlen %d", idx, istrlen(sxy_siv) );
					} // for
				}
				else {
					stringlistappend2	(&ezdev->listaccum, "No SIVS", "");
				}
			} // enable_intermap
		} // prog


}




//qbool EZDev_Sounds_Feed_Shall_Stop_Fn (int idx, ccs *key, ccs *value, ccs *a, ccs *b, ccs *c,
//							   int64_t n0, int64_t n1, int64_t n2)
//{
//	//va_super (sline, 1024, S_FMT_LEFT_PAD_20 " " S_FMT_LEFT_PAD_40, key, value);
//	if (key[0] == CHAR_NULL_0)
//		return false;
//
//	// 3 columns .. modelindex,modelname as key,n0,numtri
//	int modelindex = n0;
//	//unused ... int numtriangles = n1;
//	stringlistappendf (&ezdev->listaccum, "%d", modelindex);
//	stringlistappendf (&ezdev->listaccum, "%s", key);
//	//stringlistappendf (&ezdev->listaccum, "%d", numtriangles);
//	//stringlistappendf (&ezdev->listaccum, "%s", a); // width
//	//stringlistappendf (&ezdev->listaccum, "%s", b); // depth
//	//stringlistappendf (&ezdev->listaccum, "%s", c); // height
//
//	return false; // Shall stop
//}

RELATED_ (PRVM_Fields_Query)
qbool EZDev_Fields_Feed_Shall_Stop_Fn (int idx, ccs *key, ccs *value, ccs *a, ccs *b, ccs *c,
							   int64_t n0, int64_t n1, int64_t n2)
{
	if (key[0] == CHAR_NULL_0)
		return false;

	//CREATE TABLE CUSTOMERS ( ID INT NOT NULL, NAME VARCHAR (20) NOT NULL, AGE INT NOT NULL, ADDRESS CHAR (25), SALARY DECIMAL (18, 2), PRIMARY KEY (ID) );
	// classname, origin, model?
	ccs *vartype_str = VarType_For_EV (n0);

	//va_super (sline, 1024,
	// S_FMT_LEFT_PAD_20 " %4d t: %4d" S_FMT_LEFT_PAD_40,
	//	key, idx, (int)n1, vartype_str);
	//stringlistappend (&mz->list, idx);
	stringlistappendf (&ezdev->listaccum, "%d", idx);
	if (strlen(key) > 1 && key[strlen(key) - 2] == '_') {
		stringlistappendf	(&ezdev->listaccum, "%s", key); // ?
		stringlistappendf	(&ezdev->listaccum, "%s" CON_YELLOW  " (will NOT save to savegame)" CON_WHITE, vartype_str);
	} else {
		stringlistappend (&ezdev->listaccum, key); // ?
		stringlistappend (&ezdev->listaccum, vartype_str);
	}
	//stringlistappendf (&ezdev->listaccum, "%d", n1); // ?


	// What is our plan to get only the fields we want here?
	// Like column query ... hmmm ...
	// SQL let's you pick field

	return false; // Shall stop
}

RELATED_ (PRVM_Fields_Query (SVVM_prog, ZDev_Globals_Feed_Shall_Stop_Fn))
qbool EZDev_Globals_Feed_Shall_Stop_Fn (int idx, ccs *key, ccs *value, ccs *a, ccs *b, ccs *c,
							   int64_t n0, int64_t n1, int64_t n2)
{
	int is_more_detail = true;
	if (is_more_detail == false) {
		if (n0 == ev_string_1 && strlen(value) == 0)
			return false; // Empty string
		if (value[0] == '0' && value[1] == CHAR_NULL_0)
			return false; // Value of 0
	}

	// Baker: January 5 2025 NASTY
	int count = ezdev->listaccum.numstrings / 4 /*numcolumns*/;
	//va_super (sline, 1024, S_FMT_LEFT_PAD_20 " " S_FMT_LEFT_PAD_40, key, value);

	ccs *vartype_str = VarType_For_EV (n0);

	stringlistappendf	(&ezdev->listaccum, "%d", count);

	//if (strlen(key) > 1 && key[strlen(key) - 2] == '_') {
	if (false == isin3(n0, ev_string, ev_float, ev_entity)) {
		//stringlistappendf	(&ezdev->listaccum, CON_RED "%s" CON_WHITE, vartype_str); // ?
		stringlistappendf	(&ezdev->listaccum, "%s", key);
		stringlistappend	(&ezdev->listaccum, value);
		stringlistappendf	(&ezdev->listaccum, "%s" CON_YELLOW  " (will NOT save to savegame)" CON_WHITE, vartype_str);
	} else {
		stringlistappend	(&ezdev->listaccum, key);
		stringlistappend	(&ezdev->listaccum, value);
		stringlistappend	(&ezdev->listaccum, vartype_str);
	}

	extern cvar_t _scr_devinfo_globalscap;
	if (_scr_devinfo_globalscap.integer && count >= _scr_devinfo_globalscap.integer) {
		stringlistappendf	(&ezdev->listaccum, "%d", _scr_devinfo_globalscap.integer);
		stringlistappendf	(&ezdev->listaccum, "[_scr_devinfo_globalscap MAX HIT AT %d]", _scr_devinfo_globalscap.integer);
		stringlistappend	(&ezdev->listaccum, "---");
		stringlistappend	(&ezdev->listaccum, "---");

		return true; // STOP!!!!!!!!!!!
	}
	return false; // Shall stop
}


//RELATED_ (PRVM_Entities_Query_Fieldname ZDev_Entities_Feed_Shall_Stop_Fn)

//// Tricky: We will need to request:
//// origin
//// targetname
//// model
//qbool EZDev_Entities_Feed_Shall_Stop_Fn (prvm_prog_t *prog, int idx, ccs *key, ccs *value, ccs *a, ccs *b, ccs *c,
//							   int64_t n0, int64_t n1, int64_t n2)
//{
//	//unused ... ccs *classname = key;
//	// value is value
//#if 0
//	if (classname[0] == CHAR_NULL_0)
//		return false;
//#endif
//
//	int edict_num = idx;
//	//ccs *vartype_str = VarType_For_EV (n0);
//
//	//va_super (sline, 1024, " %4d " S_FMT_LEFT_PAD_20, idx, /*key*/ value);
//	//int modelindex = n0;
//	stringlistappendf (&ezdev->listaccum, "%d", idx);
//	//stringlistappend (&ezdev->listaccum, value);
//
//	char *sxz = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "classname");
//	char *s0z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "origin");
//	char *s1z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "model");
//	char *s2z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "frame");
//	char *s3z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "targetname");
//	char *s4z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "target");
//	char *s5z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "noise1");
//	char *s6z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "noise2");
//	char *s7z = PRVM_Entities_Query_EdictNum_ZAlloc (prog, edict_num, "noise3");
//
////idx cla mod fm,  ori, tn,  n1, n2, n3
////150,300,250,100, 180, 180  180,180,180
//
//	stringlistappend (&ezdev->listaccum, sxz ? sxz : ""); // Classname
//	stringlistappend (&ezdev->listaccum, s0z ? s0z : ""); // Origin
//	stringlistappend (&ezdev->listaccum, s1z ? s1z : ""); // Targetname
//	stringlistappend (&ezdev->listaccum, s2z ? s2z : ""); // Model
//	stringlistappend (&ezdev->listaccum, s3z ? s3z : ""); // Origin
//	stringlistappend (&ezdev->listaccum, s4z ? s4z : ""); // Targetname
//	stringlistappend (&ezdev->listaccum, s5z ? s5z : ""); // Noise1
//	stringlistappend (&ezdev->listaccum, s6z ? s6z : ""); // Noise2
//	stringlistappend (&ezdev->listaccum, s7z ? s7z : ""); // Noise3
//
//
//	Z_FreeNull_ (sxz);
//	Z_FreeNull_ (s0z);
//	Z_FreeNull_ (s1z);
//	Z_FreeNull_ (s2z);
//	Z_FreeNull_ (s3z);
//	Z_FreeNull_ (s4z);
//	Z_FreeNull_ (s5z);
//	Z_FreeNull_ (s6z);
//	Z_FreeNull_ (s7z);
//
//	return false; // Shall stop
//}

//RELATED_ (R_WorldTextures_Query)
//qbool EZDev_WorldTextures_Feed_Shall_Stop_Fn (int idx, ccs *key, ccs *value, ccs *a, ccs *b, ccs *c,
//							   int64_t n0, int64_t n1, int64_t n2)
//{
//	//va_super (sline, 1024, S_FMT_LEFT_PAD_20 " " S_FMT_LEFT_PAD_40, key, value);
//	if (key[0] != CHAR_NULL_0) {
//		int ouridx = n0;
//		stringlistappendf (&ezdev->listaccum, "%d", ouridx);
//		stringlistappendf (&ezdev->listaccum, "%s", key);
//	}
//	return false; // Shall stop
//}




#include "menu_zform_connect_tabchange.c.h"
#include "menu_zform_connect_sort.c.h"
#include "menu_zform_connect_itemchange.c.h"
#include "menu_zform_connect_devinit.c.h"
#include "menu_zform_connect_txtchange.c.h"
#include "menu_zform_connect_keypreview.c.h"


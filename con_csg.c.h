// con_csg.c.h

#pragma once

//#define BoxesOverlap(a,b,c,d) ((a)[0] <= (d)[0] && (b)[0] >= (c)[0] && (a)[1] <= (d)[1] && (b)[1] >= (c)[1] && (a)[2] <= (d)[2] && (b)[2] >= (c)[2])
//#define BoxInsideBox(a,b,c,d) ((a)[0] >= (c)[0] && (b)[0] <= (d)[0] && (a)[1] >= (c)[1] && (b)[1] <= (d)[1] && (a)[2] >= (c)[2] && (b)[2] <= (d)[2])
//#define TriangleBBoxOverlapsBox(a,b,c,d,e) (min((a)[0], min((b)[0], (c)[0])) < (e)[0] && max((a)[0], max((b)[0], (c)[0])) > (d)[0] && min((a)[1], min((b)[1], (c)[1])) < (e)[1] && max((a)[1], max((b)[1], (c)[1])) > (d)[1] && min((a)[2], min((b)[2], (c)[2])) < (e)[2] && max((a)[2], max((b)[2], (c)[2])) > (d)[2])
//
//// Baker: BoxesOverlap2D
//#define BoxesOverlap2D(aamins,bamaxs,cbmins,dbmaxs)
//	((aamins)[0] <= (dbmaxs)[0] &&
//	(bamaxs)[0] >= (cbmins)[0] &&
//	(aamins)[1] <= (dbmaxs)[1] && (bamaxs)[1] >= (cbmins)[1]) // Ender

//
//#define TriangleNormal(a,b,c,n) (
//	(n)[0] = ((a)[1] - (b)[1]) * ((c)[2] - (b)[2]) - ((a)[2] - (b)[2]) * ((c)[1] - (b)[1]),
//	(n)[1] = ((a)[2] - (b)[2]) * ((c)[0] - (b)[0]) - ((a)[0] - (b)[0]) * ((c)[2] - (b)[2]),
//	(n)[2] = ((a)[0] - (b)[0]) * ((c)[1] - (b)[1]) - ((a)[1] - (b)[1]) * ((c)[0] - (b)[0])
//	)


RELATED_ (csgax_includes BoxesOverlap2D Boxes_Collide_3D BoxesOverlap)
RELATED_ (map_carve_cubeoid_against_any_bulk)
RELATED_ (CSG_GLOBAL map_metric_brush map_metric_entity_sub )
RELATED_ (brush_fill_face_points)

RELATED_ (Con_csg_f)

#include "csg_local_defs.h"

qbool GetLongExplanation_was_found (ccs *toolcommand, char *long_explanation, size_t sizeof_long_explanation)
{
	#define Q3MAP_PRE(MYSTRING, MYFUNC, EXAMPLE, DESCRIBE, WORLD_OR_ENTITY_TYPE, APPLIES_TO) // Nada
	#define Q3MAP_PRX(MYSTRING, MYFUNC, EXAMPLE, DESCRIBE, WORLD_OR_ENTITY_TYPE, APPLIES_TO) // Nada
	#define DEF_(csg_console_name, func, description, long_explain) \
		csg_console_name NEWLINE long_explain NEWLINE // 3
	ccs *csgtool_list =
		#include "csg_table.h"
	;

	stringlist_t list = {0};
	stringlistappend_split_lines_cr_scrub (&list, csgtool_list);
	int idx = stringlist_find_index (&list, toolcommand);

	if (idx != not_found_neg1) {
		ccs *long_explain = list.strings[idx + 1];
		dp_strlcpy (long_explanation, long_explain, sizeof_long_explanation);
	}

	stringlistfreecontents (&list);

	if (idx == not_found_neg1)
		return false; // Not found
	return true;
}
 //   }
	//// else if (String_Match (toolcommand, CSG_ADJACENTS)) {
 //   //    int num_done = csqc_adjacents_wip (&list_map1);
 //   //    CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
 //   //    bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    //}
#if 0

    } else if (String_Match (toolcommand, CSG_COLLISION_BODY)) {
        int num_done = entitylist_perform_collision_body (&list_map1, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_CUBEIT)) {
        int num_done = entitylist_perform_cubeit (&list_map1);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_DECAL_AUTOMATIC)) {
        int num_done = csge_decal_automatic_droptofloor (&list_map1, CSG_CMD__AUTOMATIC_DROPTOFLOOR, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_DECAL_DROP_DISTANCE)) {
        int num_done = csge_decal_drop_distance(&list_map1, CSG_CMD__DROP_DISTANCE, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_CARVE)) {
        int num_done = csgm_carve (&list_map1, CSG_INCLUDES /*yep*/, MAPPATH_REALPATH_NULL, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_CARVE2)) {
        int num_done = csgm_carve2 (&list_map1, CSG_INCLUDES /*yep*/, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_INCLUDES)) {
        int num_done = csge_includes (&list_map1, CSG_INCLUDES, MAPPATH_REALPATH_NULL, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_INCLUDE_SHEET_GEN)) {
        int num_done = csgc_includes_sheet (); // Yep
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;
        Con_PrintLinef ("include_sheet_gen shall not write a map ... skipping");
        goto fail_exit;


    } else if (String_Match (toolcommand, CSG_PINCOMMONTERRAIN)) {
        int num_done = entitylist_perform_pincommonterrain_num_done (&list_map1);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_REMOVENONDOMINANTTEXTUREBRUSHES)) {
        int num_done = csqc_delete_world_brushes_with_nondominant_texture (&list_map1);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

    } else if (String_Match (toolcommand, CSG_SKY_ENCASE_DECAL)) {
        int num_done = csgw_sky_encase (&list_map1, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);

        /*int*/ num_done = csgw_sky_decal_wip (&list_map1, timestring_newval);
        CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
        bop_ok = true;    Con_PrintLinef ("Num changes = %d", num_done);


		/*int*/ num_done = csge_decal_automatic_droptofloor (&list_map1, "_automatic_droptofloor", timestring_newval);
		CMD_FAILEXIT_IF2_ (num_done == 0, "Operation made num changes == %d", num_done);
		bop_ok = true;	Con_PrintLinef ("Num changes = %d", num_done);

#endif // 0

#define IF_THEN_RUN_0(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
        num_done = FUNCTION (&list_map1); \
		goto done; \
	} // Ender

#define IF_THEN_RUN_X(STRING, FUNCTION) \
if (String_Match (toolcommand, STRING)) { \
    num_done = FUNCTION (&list_map1, filename); \
	goto done; \
} // Ender

#define IF_THEN_RUN_1(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
		CMD_FAILEXIT_IF2_ (cmd_argc < 4, "%s: requires 1 argument", toolcommand); \
        ccs *texturename_or_whatitis = toolarg1; \
		num_done = FUNCTION (&list_map1, texturename_or_whatitis); \
		goto done; \
	} // Ender


#define IF_THEN_RUN_2(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
		CMD_FAILEXIT_IF2_ (cmd_argc < 5, "%s: requires 2 arguments", toolcommand); \
        ccs *key = toolarg1; \
		ccs *val = toolarg2; \
		num_done = FUNCTION (&list_map1, key, val); \
		goto done; \
	} // Ender

#define IF_THEN_RUN_30(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
		CMD_FAILEXIT_IF2_ (cmd_argc < 3, "%s: requires 0 arguments", toolcommand); \
        ccs *key = toolarg1; \
		ccs *val = toolarg2; \
		ccs *amt = toolarg3; \
		num_done = FUNCTION (&list_map1, key, val, amt); \
		goto done; \
	} // Ender

// 2 args, but 0 is required
#define IF_THEN_RN_20(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
		CMD_FAILEXIT_IF2_ (cmd_argc < 3, "%s: requires 0 arguments", toolcommand); \
        ccs *key = toolarg1; \
		ccs *val = toolarg2; \
		num_done = FUNCTION (&list_map1, key, val); \
		goto done; \
	} // Ender

// 1 args, but 0 is required
#define IF_THEN_RN_10(STRING, FUNCTION) \
	if (String_Match (toolcommand, STRING)) { \
		CMD_FAILEXIT_IF2_ (cmd_argc < 3, "%s: requires 0 arguments", toolcommand); \
        ccs *texturename_or_whatitis = toolarg1; \
		num_done = FUNCTION (&list_map1, texturename_or_whatitis); \
		goto done; \
	} // Ender



RELATED_ (CSG_Perform)

void Con_csg_f (cmd_state_t *cmd)
{
	qbool is_out_version = String_Match (cmd_argv_command_0, "csgout");
	if (cmd_argc == CMD_NO_ARGS_ARGC_1) {
		#define DEF_(csg_console_name, func, description, long_explain) csg_console_name NEWLINE description NEWLINE // 3
		#define Q3MAP_PRX Q3MAP_PRE // Nada
		#define Q3MAP_PRE(MYSTRING,MYFUNC,EXAMPLE,DESCRIBE,WORLD_OR_ENTITY_TYPE,APPLIES_TO) // Nada

			ccs *csgtool_list =
				#include "csg_table.h"
				;

		{ zpr (NEWLINE "Console options:");
			stringlist_t list = {0};
			stringlistappend_split_lines_cr_scrub (&list, csgtool_list);
			stringlistsortcolumns_alpha (&list, /*numcolumns*/ 2, /*sortcolumn*/ 0);
			for_each_stringlist2_string (n, list, console_name, description)
				int real_index = (int)n/2;
				zpr ("%2d: %-22.22s %s", real_index, console_name, description);
			for_each_end
			stringlistfreecontents (&list);
		}

		zpr (NEWLINE "usage: %s <map> <command> [arg1] [arg2]", cmd_argv_command_0);
		zpr ("ex   : %s maps/mymap.map rewrite", cmd_argv_command_0);
		zpr ("ex   : %s maps/mymap.map brushdeletetexture walls/brick12", cmd_argv_command_0);
		zpr ("ex   : %s maps/mymap.map set_worldspawn_key \"rewrite\" \"1\"", cmd_argv_command_0);
		zpr ("ex   : %s " CON_BRONZE "keys // list .map pre-processor entity keys", cmd_argv_command_0);
		zpr ("ex   : %s " CON_BRONZE "keyshort // concise version of keys", cmd_argv_command_0);

		zpr ("csg will rewrite a .map replacing it with modification.");
		zpr ("csgout is same as csg except appends _out to the filename instead of overwriting file.");
		zpr ("===");
		zpr ("csg only supports Valve 220 Q3 map format .map data.  Open a .map in J.A.C.K. and then export the .map and it will be in a format that Zircon pre-processor understands.");
		return;
	}

	qbool is_short = String_Match (cmd_argv_parm1, "keyshort");
	qbool is_keys = String_Match (cmd_argv_parm1, "keys") || is_short;

	if (cmd_argc == 2 && is_keys) {
		// Become folder command
		int num_columns_3 = 3;
		#define DEF_(csg_console_name, func, description, long_explain) // Nada
		#define Q3MAP_PRX Q3MAP_PRE
		#define Q3MAP_PRE(MYSTRING, MYFUNC, EXAMPLE, DESCRIBE, WORLD_OR_ENTITY_TYPE, APPLIES_TO) \
			MYSTRING NEWLINE EXAMPLE NEWLINE DESCRIBE NEWLINE // 3
				ccs *csgcmdline_list =
					#include "csg_table.h"
					;

		{ zpr ("-csg pre-processor keys:");
			stringlist_t list = {0};
			stringlistappend_split_lines_cr_scrub (&list, csgcmdline_list);
			stringlistsortcolumns_alpha (&list, num_columns_3, /*sortcolumn*/ 0);
			for_each_stringlist3_string (n, list, mystring, example, description)
				int real_index = (int)n/3;

				zpr (CON_BRONZE "%2d: " CON_WHITE" %s " "(" CON_BRONZE  "%s" CON_WHITE")", real_index, mystring, example);
				if (is_short == false)
					zpr ("%s", description);
			for_each_end
			stringlistfreecontents (&list);
		}

		return;
	}

	if (cmd_argc == 2) {
		// Become folder command
		void SCR_Folder_f(cmd_state_t *cmd);
		SCR_Folder_f (cmd);
		return;
	}

	CMD_USAGE_NUM_ARGS_DESCRIBE_ (2, "<maps/model.map> <brushfacer or ...> ", "Performs CSG tool re-write function on specified map");

	ccs		*filename1		= cmd_argv_parm1;	// maps/mymap.map or maps/mymap
	ccs		*toolcommand	= cmd_argv_parm2;	// "brushfacer" or "rewrite"
	ccs		*toolarg1		= cmd_argv_parm3;	// <texturename or parm function needs>
	ccs		*toolarg2		= cmd_argv_parm4;	// ..
	ccs		*toolarg3		= cmd_argv_parm5;	// ..

	// maps/model.map or maps/model
	va_super (filename,  MAX_OSPATH_EX_1024, "%s", filename1);

	// maps/model => maps/model.map
	if (String_Ends_With_Caseless (filename,  ".map") == false)	c_strlcat (filename,  ".map");

	// Clean requires these are always initialized
	char *sin_za = NULL;
	map_t list_map1 = {0};
	qbool shall_write = true; // Default YES
	ccs *no_write_reason = "*NOT SET*";
	int num_done = 0;
	double time_start___0 = Sys_DirtyTime (); // For amount of time report
	double time_read_ov_1 = 0;
	double time_op_over_2 = 0;
	double time_write_d_3 = 0;

	char long_explanation[2048] = {0};
	qbool was_found = GetLongExplanation_was_found (toolcommand, long_explanation, sizeof(long_explanation));

	CMD_FAILEXIT_IF2_ (was_found == false, "Tool " QUOTED_S " was not found", toolcommand);

	// Make sure exists
	qbool bfilenameexists = FS_File_Exists(filename);

	CMD_FAILEXIT_IF2_ (bfilenameexists == false, "Filename %s does not exist to read", filename)

	Vid_SetWindowTitlef ("Reading %s", filename);
	sin_za = FS_LoadFileQuick (filename);
	CMD_FAILEXIT_IF2_ (!sin_za, "Couldn't %s", filename)

	int isok = map_load_from_string (&list_map1, sin_za);
	Mem_FreeNull_ (sin_za); // Done with this.

	CMD_FAILEXIT_IF2_ (!isok, "Couldn't parse map %s", filename)

	time_read_ov_1 = Sys_DirtyTime();

operations:

	Vid_SetWindowTitlef ("Operation: %s", toolcommand);
	if (String_Match (toolcommand, CSG_REWRITE)) {
		num_done = 1;
		goto done;
	}

    if (String_Match (toolcommand, CSG_INCLUDE_SHEET_GEN)) {
        num_done = csgc_includes_sheet ();
		//zpr ("Include sheet makes no changes");
		shall_write = false; no_write_reason = "include sheet generator only";
		goto done;
	}

	if (String_Match (toolcommand, CSG_INFODUMP)) {
        num_done = csgc_infodump (&list_map1);
		//zpr ("infodump sheet makes no changes");
		shall_write = false; no_write_reason = "infodump does not write";
		goto done;
	}

	IF_THEN_RUN_X (CSG_CSG_RUN,						csgc_csg_run) // TESTED - needs to send filename

	IF_THEN_RUN_0 (CSG_BRUSH_DELETE_IF_NONDOM,		csgc_brush_delete_if_nondom)  // TESTED
	IF_THEN_RUN_1 (CSG_BRUSH_DELETE_IF_TEXTURE,		csgc_brush_delete_if_texture)  // TESTED
	IF_THEN_RUN_1 (CSG_BRUSHFACER_ON_TEXTURE,		csgc_brushfacer_specific_texture)  // TESTED
	IF_THEN_RUN_1 (CSG_BRUSHUNFACER_ON_CLASSNAME,	csgc_brushunfacer_specific_classname) // TESTED
	IF_THEN_RUN_0 (CSG_BRUSHFACER,					csgc_brushfacer) // TESTED
	IF_THEN_RUN_0 (CSG_BRUSHUNFACER,				csgc_brushunfacer) // TESTED
	//IF_THEN_RUN_X (CSG_CARVE_INCLUDES,			csgc_carve_includes) //
	IF_THEN_RUN_0 (CSG_CAULKWORLD,					csgc_caulk_non_common_non_sky_containing_textures) // TESTED
	IF_THEN_RUN_0 (CSG_CLIPBOARD_ADD_ENTS,			csgc_add_point_entities_on_clipboard) // TESTED

	IF_THEN_RUN_0 (CSG_DETAIL_AUTO,					csgc_detail_auto) // TESTED
	IF_THEN_RUN_0 (CSG_DETAIL_WEDGE_SPLIT_AUTO,		csgc_detail_wedge_split_auto) // TESTED
	IF_THEN_RUN_0 (CSG_FLAT_TRIM,					csgc_flat_trim)	// TESTED
	IF_THEN_RN_10 (CSG_FLAT_GROUND_RETEXTURE,		csgc_flat_ground_retexture) // 1 optional, 0 required
	IF_THEN_RN_10 (CSG_FLOOR_EVERYTHING,			csgc_floor_every_brush_to_specified_z) // 1 optional, 0 required
	IF_THEN_RUN_0 (CSG_GROUND_CARVE,				csgc_ground_carve) // 1 optional, 0 required
	IF_THEN_RUN_0 (CSG_INFO_PLAYER_START_DROP_CENTER,csgc_info_player_start_drop_center) // 1 optional, 0 required

	IF_THEN_RUN_0 (CSG_PRINT_LOWEST_GROUND,			csgc_print_lowest_ground)
	IF_THEN_RUN_2 (CSG_PRINTFACES,					csgc_printfaces)
	IF_THEN_RUN_0 (CSG_PRINTTEXTURES,				csgc_printtextures)
	IF_THEN_RUN_2 (CSG_PRINTUNIT,					csgc_printunit)
	IF_THEN_RUN_0 (CSG_PROCESS_TERRAIN,				csgc_process_terrain)
	IF_THEN_RUN_0 (CSG_QUAKE1_TO_QUAKE3,			csgc_q13)	// TESTED
	IF_THEN_RUN_30 (CSG_REPLACETEXTUREADJACENT,		csgc_replacetextureadjacent)
	IF_THEN_RUN_0 (CSG_RIDGE_VALLEY,				csgc_ridge_valley)
	IF_THEN_RUN_0 (CSG_R_LISTMAPSHADERS,			csgc_q13)	// TESTED
	IF_THEN_RUN_30 (CSG_ROTATEUNIT,					csgc_rotateunit)
	IF_THEN_RN_20 (CSG_SKY_ENCASE,					csgc_sky_encase) // 2 optional, 0 required // TESTED
	IF_THEN_RUN_0 (CSG_WELD,						csgc_weld)	// TESTED
	IF_THEN_RUN_1 (CSG_WORLDSPAWN_KEY_DELETE,		csgc_worldspawn_key_delete) // TESTED
	IF_THEN_RUN_0 (CSG_WORLDSPAWN_KEYS_PRINT,		csgc_worldspawn_keys_print) // TESTED
	IF_THEN_RUN_2 (CSG_WORLDSPAWN_KEY_SET,			csgc_worldspawn_key_set) // TESTED
	
	RELATED_ (CSG_TABLE)

unknown:
	CMD_FAILEXIT_IF2_ (1, "Tool " QUOTED_S " is unknown", toolcommand);

done:

	zpr ("===================================");
	if (long_explanation[0]) {
		zpr ("%s", long_explanation);
		zpr ("===================================");
	}
	zpr ("%s: num_done = %d", toolcommand, num_done);

	time_op_over_2 = Sys_DirtyTime ();

	// Write the file
	va_super (filename_out,  MAX_OSPATH_EX_1024, "%s", filename);
	if (is_out_version) {
		File_URL_Edit_Remove_Extension (filename_out); // May 24 2025: Should be unicode safe.  All unicode >= char 128 = no collision.
		c_strlcat (filename_out, "_out");
		c_strlcat (filename_out, ".map");
	}

	// NO although csg_run does reload upon num_done_here > 0, it does not write.
	//if (shall_write && String_Match (toolcommand, CSG_CSG_RUN))
	//	shall_write	= false;
	RELATED_ (csgc_csg_run)

	qbool did_write = false;
	if (num_done > 0 && shall_write) {
		Vid_SetWindowTitlef ("Writing %s", filename_out);
		baker_string_s *bsout = map_save_to_string_bsalloc (&list_map1);
		va_copy_32 (str_bytes, String_Num_To_Thousands_Sbuf (bsout->length) );
		float size_mb = bsout->length / (1024.0 * 1024.0);

		CMD_FAILEXIT_IF_ (!bsout, "Couldn't export entitylist to text") // Can't happen, right?

		// RE-WRITE
		int isok2 = FS_WriteFileStringQuick (filename_out, bsout->string);
		BakerString_Destroy_And_Null_It (&bsout);
		CMD_FAILEXIT_IF2_ (!isok2, "Failed to write %s", filename_out) // Can't happen, right?
		zpr ("Wrote %s %s bytes (%.1f MB)", filename_out, str_bytes, size_mb);
		did_write = true;
	} else {
		if (num_done == 0)
			no_write_reason = "no changes were made";
		zpr ("Not writing .map = %s", no_write_reason);
	}
	time_write_d_3 = Sys_DirtyTime ();
	double time_read =  time_read_ov_1 - time_start___0;
	double time_oper =  time_op_over_2 - time_read_ov_1;
	double time_writ =  time_write_d_3 - time_op_over_2;
	double time_tota =  time_write_d_3 - time_start___0;

	zpr ("Read .map time: %9.3f  %s", time_read, filename);
	zpr ("Operation time: %9.3f", time_oper);
	zpr ("Save .map time: %9.3f  %s", time_writ, did_write ? filename_out : "(Not applicable)");
	zpr ("                ===========");
	zpr ("Total time    : %9.3f ===> %s", time_tota, did_write ? filename_out : "(no write occurred)");

fail_exit:

	Mem_FreeNull_ (sin_za); // Done with this.
	map_freecontents	(&list_map1);

	Vid_SetWindowTitle (gamename);
}


void SCR_precache_test_f (cmd_state_t *cmd)
{
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (1, "<model>", "Test precache of model");
	ccs *modelname = cmd_argv_parm1;
	int idx = SV_ModelIndex(modelname, PRECACHE_MODEL_CMD_MODE_2);
	zpr ("%s is %d", modelname, idx);
	if (idx) {
		model_t *m = sv.models[idx];
		zpr ("Size mins " V3G " maxs " V3G, VS(m->normalmins), VS(m->normalmaxs));
		va_super (textline, 256, "%s mins " V3G " maxs " V3G, modelname, VS(m->normalmins), VS(m->normalmaxs));
		ccs *filename = "temp_modelinfo.tmp";
		qbool ok = FS_WriteFileStringQuick (filename, textline);
		zpr ("%s %s", ok ? "Wrote" : "Couldn't write", filename);
	} // If worked
}



RELATED_ (Con_csg_f)


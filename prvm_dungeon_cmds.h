// prvm_dungeon_cmds.h

#include "prvm_dungeon_ent_cmds.h"
#include "prvm_dungeon_cmds_halls.h"
//#include "prvm_dungeon_cmds.h"

// Returns PLUS1(index)
RELATED_ (VM_search_begin VM_callfunction VM_isfunction VM_getentityfieldstring)
RELATED_ (PRVM_Crash)



#if 1
void VM_map_assemble (prvm_prog_t *prog)
{
//	VM_SAFEPARMCOUNTRANGE(1, 2, VM_map_assemble);
//
//	PRVM_G_FLOAT(OFS_RETURN) = -1; // Default?
//
//	ccs *filename			= PRVM_G_STRING(OFS_PARM0);			// maps/mymap.map
//	int blocks_buffernum	= (int)PRVM_G_FLOAT(OFS_PARM1);		// a list like /maps/dungeon_maker/blocks/room.map
//	int indexes_buffernum	= (int)PRVM_G_FLOAT(OFS_PARM2);		// a list of known blocks.  
//																// If this is received the above is numbers instead, like Halls
//	
//	vec3_t vcolumns_rows; VectorCopyDestSrc (vcolumns_rows, PRVM_G_VECTOR(OFS_PARM3));
//	int numcolumns	= vcolumns_rows[0];
//	int numrows		= vcolumns_rows[1];
//
//	vec3_t vblocksize; VectorCopyDestSrc (vblocksize, PRVM_G_VECTOR(OFS_PARM4));
//	//int width		= vcolumns_rows[0];	// 512	X dimension
//	//int depth		= vcolumns_rows[1]; // 512	Y dimension
//	//int tallness	= vcolumns_rows[2]; // 320	Z dimension
//
//	// TO BE DETERMINED.  Some of this will be cvar controlled.
//	int flags		= (int)PRVM_G_FLOAT(OFS_PARM5);
//
//	#pragma message ("Do some sort of validation later")
//
//	RELATED_ (stringbuffer_append VM_maze_generate)
//
//	// OPEN THE SHELL
//	// ISSUE: THIS DEPENDS ON CALCS FROM OPENING THE SHELL PREVIOUSLY!	
//	
//	// COMPILE SETUP
//	
//	map_t list_hmap = {0};
//	qbool isok = map_save_as (&list_hmap, filename);
//	map_freecontents	(&list_map1);
//	
//	// CREATE SOLIDITY FOR FLOORS / CELLING CULL
//	stringlist_t plist_solidity = {0};
//	for (int row = 0; row < numrows; row ++) {
//		int coord_row = (numrows - 1) - row; // Flip the row so 0,0 // 20x13 // 20-
//		for (int col = 0; col < numcols; col ++) {
//			int	cell_idx	= col + row * numcols;
//			string	filename = FileName_For_Cell_Index(cell_idx);
//			stringlist_add(plist_solidity, filename == NULL ? SPACE : "#");
//		}
//	}
//	int blocksize_512 = 512;
//
//	#define CAULKEMPTY_NEG1 -1 // Force caulking empty floor instead of removing.
//
//	World_Emit_Shell_To_Hmap (dun.compile.hmap, 
//		[blocksize_512 * dun.num_columns, blocksize_512 * dun.num_rows, CEIL_HEIGHT_384], 
//		[dun.num_columns, dun.num_rows, TESS_SIZE_512 * CAULKEMPTY_NEG1], plist_solidity, 
//		placemat_mins, placemat_maxs);
//
//
//	// Just in case ..
//	dun.placemat_mins = placemat_mins;
//	dun.placemat_maxs = placemat_maxs;
//
//	stringlist_destroy (plist_solidity);
//
//	dun.compile.is_error = false;
//	dun.compile.last_stage = MAPSTAGE_NONE_0; // CLEAR IT.
//	dun.compile.realtimestart = Host_RealTime ();
//	printvarf (dun.compile.realtimestart);
//	dun.compile.realtimeend = 0;
//
//	string basename = "your_map";
//	strset	(dun.compile.base_mapname_a, basename);
//	strsetf	(dun.compile.filename_map_a, "maps/%s.map", basename);
//	strsetf (dun.compile.filename_bsp_a, "maps/%s.bsp", basename);
//
//	// END COMPILE SETUP
//		
//	int num_additions = 0;
//	qbool first_room = true;
//
//	// START REMOVE HERE
//	Remove_All_Starts (dun.compile.hmap); // Kill all the info_player_starts in the shell.
//
//	for (int row = 0; row < numrows; row ++) {
//		int coord_row = (numrows - 1) - row; // Flip the row so 0,0 // 20x13 // 20-
//		for (int col = 0; col < numcols; col ++) {
//			int	cell_idx	= col + row * numcols;
//			string	block_filename = FileName_For_Cell_Index(cell_idx);
//			if (block_filename == NULL) continue;
//
//			// Requires map block has 0,0,0 based and is 256 x 256 size.
//			vector add_amount;
//			add_amount.x = col			* dun.placemat_blocksize_512 + (dun.placemat_blocksize_512 / 2);
//			add_amount.y = coord_row	* dun.placemat_blocksize_512 + (dun.placemat_blocksize_512 / 2);
//
//			// Mins is the Southwest corner of the placemat
//			vector translate_amount = [dun.placemat_mins.x + add_amount.x, dun.placemat_mins.y + add_amount.y, 0 ];
//
//			int hblock = map_load (block_filename);
//
//				Remove_All_Entities_Of_Classname (hblock, "func_group_floor");
//				Remove_All_Entities_Of_Classname (hblock, "info_player_start");
//
//				qbool is_room = is_room_image(block_filename);
//				// Need to figure out what doors do and don't exist and remove them.
//				if (is_room) {
//					if (first_room) {
//						vector org = translate_amount;
//						org.x = 0;//dun.placemat_blocksize_512 / 2;
//						org.y = 0;//dun.placemat_blocksize_512 / 2;
//						org.z = 64;
//
//						int myentnum = map_entity_add (hblock, "classname", "info_player_start");// = #169; // float entnum = map_entity_add(h, "classname", "shambler");
//						map_entity_key_set_valuef (hblock, myentnum, "origin", "%g %g %g", org.x,  org.y, org.z);
//						map_entity_key_set_valuef (hblock, myentnum, "_info", "row %d col %d", row, col);
//						first_room = false;
//					}
//					// Kill unnecessary doors + reveal those walls
//					// Reveal needed doors + kill unneeded walls
//					Block_Remove_Kill_Doors (hblock, row, col);
//				}
//
//				//this needs to go, make unadaptable intersections
//				//qbool is_inter = is_intersection(block_filename);
//
//				//if (is_inter) {
//				//	// Figure out what to remove for the intersection
//				//	Block_Intersection_Adapt (hblock, row, col);
//				//}
//
//				// This checks component_odds roll items
//				// If failed, removes component_children also
//				Block_Remove_Odds_Roll (hblock, cell_idx);
//				
//				// Give targetnames in .map non-colliding names.
//				// So take "door1" and renaming it "_block2_door1" hitting "target" and "targetname"
//				string targetname_prefix = sprintf ("_block%d_", cell_idx);
//				ret = map_prefix_fields_values(hblock, "target,targetname", targetname_prefix); error_if (ISERROR_LESSTHANZERO(ret), "Error prefixing targetnames ret = ", ftos(ret));
//
//				ret = map_move (hblock, translate_amount);			error_if (ISERROR_LESSTHANZERO(ret), "Error map_move ret = ", ftos(ret));
//
//				ret = map_append_map (dun.compile.hmap, hblock);	error_if (ISERROR_LESSTHANZERO(ret), "Error map_append_map ret = ", ftos(ret));
//
//			ret = map_close (hblock);
//				
//			error_if (ISERROR_LESSTHANZERO(ret), "Error map_close ret = ", ftos(ret));
//
//			num_additions ++;
//
//			//ret = map_save_as (dun.compile.hmap, sprintf("maps/cells_%04d.map", cell_idx));
//		} // col
//	} // row
//
//	//printvarf (num_additions);
//
//	ret = map_save_as (dun.compile.hmap, dun.compile.filename_map_a);
//	error_if (ISERROR_LESSTHANZERO(ret), "Error map_save_as ", dun.compile.filename_map_a, " ret = ", ftos(ret));
//
////	RELATED_ (DungeonMaker_Compile_More_Think Map_Compile_Complete_Callback)
////	dun.compile.last_stage = MAPSTAGE_CSG_1; // IT WILL GO TO #1 CSG then proceed ...
//
////	NextThink_Schedule_Event (DungeonMaker_Compile_More_Think, time + 0.1);
//	

}
#endif // 1

// float map_load (string filename) = #150;
// float map_create () = #150;
// Returns 1 or greater on success
// allowing 0 value uninitialized variables to never contain a valid handle.
// Example:
// float h = map_load ("maps/proto/#dshell.map");
// Q: Does map_create create entity #0 (world?)
// A: March 25 2026 - No
// 	int hmap = map_create ();
//	int entnum = map_entity_add(hmap, "classname", "worldspawn");
// int flor_entnum = map_add_entity_with_classname (hm, "func_group_floor");
void VM_map_load (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(0, 1, VM_map_load); // 0: filename  Optionals: None

	int handle;
	for (handle = 0; handle < PRVM_MAX_OPEN_DOT_MAPS_8; handle ++)
		if (prog->open_dot_maps[handle] == 0)
			break;

	if (handle >= PRVM_MAX_OPEN_DOT_MAPS_8) {
		PRVM_G_FLOAT(OFS_RETURN) = -2; VM_WarningLinef (prog, "VM_map_load: %s ran out of map handles (%d)", prog->name, PRVM_MAX_OPEN_DOT_MAPS_8);
		return;
	}

	// Baker: Construct the entity_t
	map_t *plist = (map_t *)Mem_Alloc(prog->progs_mempool, sizeof(map_t));
	
	if (prog->argc == 0) {
		// Map_create
		prog->open_dot_maps[handle] = plist;
		PRVM_G_FLOAT(OFS_RETURN) = PLUS1(handle);
		return;
	}
	
	ccs *filename = PRVM_G_STRING(OFS_PARM0);  
	VM_CheckEmptyString(prog, filename); // Baker: Host error if first character is whitespace or NULL char 0

	int errorcode = map_load_or_errorcode (plist, filename);
	// returns SUCCESS_OK_0, -1 (file not found) or -3 (couldn't parse)

	if (errorcode) {
		PRVM_G_FLOAT(OFS_RETURN) = errorcode; // File not found or couldn't parse. -1 or -3

		map_freecontents (plist); // free data
		Mem_FreeNull_ (plist); // free the struct
		return;
	}

	prog->open_dot_maps[handle] = plist;
	PRVM_G_FLOAT(OFS_RETURN) = PLUS1(handle);
}

// #157 float map_close (float hmap) = #157; // Return value is true or -4 for invalid handle
void VM_map_close (prvm_prog_t *prog)
{
	RELATED_ (VM_search_end)

	VM_SAFEPARMCOUNT(1, VM_map_close);

	// Baker: Deliberately no boiler macro
	int _hmap = (int)PRVM_G_FLOAT(OFS_PARM0);
	int handle = UNPLUS1(_hmap);

	if (in_range_beyond (0, handle, PRVM_MAX_OPEN_DOT_MAPS_8) == false) {
		VM_WarningLinef (prog, "VM_map_close: invalid handle %d used in %s", handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	if (prog->open_dot_maps[handle] == NULL) {
		VM_WarningLinef (prog, "VM_map_close: no such handle %d in %s", handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	map_freecontents (prog->open_dot_maps[handle]);
	Mem_FreeNull_ (prog->open_dot_maps[handle]);

	PRVM_G_FLOAT(OFS_PARM0) = 0; // Does this work?

	PRVM_G_FLOAT(OFS_RETURN) = 1;
}

// #153 float map_save_as (float hmap, string filename) = #153;
// true = success, false = failure, -4 invalid handle, -1 refused if does not end with .map
// float isok = map_save_as (h, "maps/my_map.map") = #153;
void VM_map_save_as (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 2, VM_map_save_as); // hmap filename // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	ccs *filename = PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, filename);

	if (FS_CheckNastyPath(filename, fs_isgamedir_false)) {
		VM_WarningLinef (prog, "%s: nasty filename rejected " QUOTED_S, __func__, filename);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	if (String_Starts_With (filename, "maps/") == false) {
		VM_WarningLinef (prog, "%s: refused filename " QUOTED_S " must start with maps/ %s", __func__, filename, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	if (String_Ends_With (filename, ".map") == false) {
		VM_WarningLinef (prog, "%s: refused filename " QUOTED_S " must end with .map %s", __func__, filename, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	qbool isok = map_save_as (pe, filename);

	PRVM_G_FLOAT(OFS_RETURN) = isok ? 1 : 0 ; // Return true on success
}

// #154 float map_clipboard_set (float hmap) = #154; // Returns true on success, -4 on invalid handle
// map_clipboard_set (h);
void VM_map_clipboard_set (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(1, 1, VM_map_clipboard_set); // hmap // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	baker_string_s *bsa = map_save_to_string_bsalloc (pe);
	if (!bsa) { // Virtually impossible?
		VM_WarningLinef (prog, "%s: could not allocate string handle %d in %s", __func__, handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -6;
		return;
	}

	Clipboard_Set_Text (bsa->string); // CSQC: map_clipboard_set

	BakerString_Destroy_And_Null_It (&bsa);

	PRVM_G_FLOAT(OFS_RETURN) = 1; // Return true on success
}


// #156 float map_prefix_fields_values (float hmap, string fieldscomma, string prefix) = #156; // returns number changed.
// float numchanges = map_prefix_fields_values(hmap, "target,targetname", "block1_");
void VM_map_prefix_fields_values (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_prefix_fields_values); // hmap commanames prefix // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	ccs *comma_names	= PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, comma_names);
	ccs *prefix			= PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, prefix);

	stringlist_t list_comma_names = {0};
	stringlist_append_split (&list_comma_names, comma_names, ",");

	int num_changed = csg_vm_prefix_targetnames_list (pe, prefix, &list_comma_names);

	stringlistfreecontents (&list_comma_names);

	PRVM_G_FLOAT(OFS_RETURN) = num_changed;
}


// #158 float map_entity_count (float hmap) = #158;
// Return number of entities in the map.  Returns < 0 on failure.
// float numentities = map_entity_count (h);
void VM_map_entity_count (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNT(1, VM_map_entity_count);

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	int num_entities_in_map = pe->count;

	PRVM_G_FLOAT(OFS_RETURN) = num_entities_in_map;
}


// #159 /*entnum*/ float map_find_entity (float hmap, string key, string value, [float afterindex]) = #159; // Returns entity number, 0 is world, failure returns -1
// Returns -4 on handle failure, returns -1 if not found.
// float entnum = map_find_entity (h, "targetname", "mat1");
void VM_map_find_entity (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 4, VM_map_find_entity); // hmap, key, value // opt: afterindex

	HMAP_HANDLE_BOILER;

	ccs *keyname = PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, keyname);
	ccs *keyvalue = PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, keyvalue);
	int startindex = WORLD_0;

	if (prog->argc >= 4) { // h targetname mat1 0
		startindex = (int)PRVM_G_FLOAT(OFS_PARM3) + 1;
	}

	map_t *pe = prog->open_dot_maps[handle];

	int entnum = map_find_key_string_start_beyond (pe, keyname, keyvalue, startindex, pe->count);

	PRVM_G_FLOAT(OFS_RETURN) = entnum;
}


// #152 float map_append_map (float hmapdest, float hmapsrc) = #152;
// Returns <=0 on failure, true on success
void VM_map_append_map (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 2, VM_map_append_map); // dst src // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe_dest = prog->open_dot_maps[handle];

	// Baker: Deliberately no boiler macro
	int _hmap2 = (int)PRVM_G_FLOAT(OFS_PARM1);
	int handle2 = UNPLUS1(_hmap2);

	if (in_range_beyond (0, handle2, PRVM_MAX_OPEN_DOT_MAPS_8) == false) {
		VM_WarningLinef (prog, "%s: invalid handle2 %d used in %s", __func__, handle2, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	if (prog->open_dot_maps[handle2] == NULL) {
		VM_WarningLinef (prog, "%s: no such handle2 %d in %s", __func__, handle2, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	if (handle == handle2) {
		VM_WarningLinef (prog, "%s: Dest handle == source handle %d in %s", __func__, handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -5; // Dest and source are the same.
		return;
	}

	map_t *pe_src = prog->open_dot_maps[handle2];

	// Add entity 0 brushes TO MAP1
	// Add entity ALL brushes and epairs to MAP1
	map_append_map	(pe_dest, pe_src);
	//map_world_brushes_patches_copy	(pe_dest, pe_src);	// no keys, just brushes.
	//map_afterworld_copy				(pe_dest, pe_src);	// brushes and keys.

	PRVM_G_FLOAT(OFS_RETURN) = 1; // true = success
}

RELATED_ (VM_uri_get VM_bufstr_get VM_callfunction VM_isfunction)
RELATED_ (FS_Shell_Blocking_f)
RELATED_ (FS_Dungeon_f)


// #151 float map_move (float hmap, vector move) = #151;
// float isok = map_move (h, '200 200 200')
// The move is relative, not absolute.  It will add +200 +200 +200 to all entities.
// First the brushes (and any patches are moved) and then any "origin" are updated.
// At this time, this operation does not "fail" aside from invalid handle -4
void VM_map_move (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 2, VM_map_move); // hmap, vec // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];
	vec3_t move_amount; VectorCopyDestSrc (move_amount, PRVM_G_VECTOR(OFS_PARM1));

	if (!move_amount[0] && !move_amount[1] && !move_amount[2])
		goto quick_exit; // Zero move

	map_move_epairs_origin	(pe, move_amount);
	map_move_all_brushes_patches	(pe, move_amount); // Has texture scaling.

quick_exit:
	PRVM_G_FLOAT(OFS_RETURN) = 1; // true = success
}


// #155 float map_compile_with_callback (string filename,
//			void(float hmap, float stage, float status) callback) = #155;
// Ex: map_compile_with_callback ("maps/mymap.map", Map_Compile_Complete_Callback
// Example is ...
//cvar_t q3map2_csg_compiler = {CF_SERVER, "q3map2_csg_compiler", "", "If emptystring, engine .exe is the compiler [Zircon]"};
//cvar_t q3map2_compiler = {CF_SERVER, "q3map2_compiler", "q3map2/q3map2.exe", "If emptystring, engine .exe is the compiler [Zircon]"};
//cvar_t q3map2_csg_switches = {CF_SERVER, "q3map2_csg_switches", "-csg", "q3map2 switches for bsp phase [Zircon]"};
//cvar_t q3map2_bsp_switches = {CF_SERVER, "q3map2_bsp_switches", "-game nexuiz -meta -v -mv 1000000 -mi 6000000 -samplesize 8", "q3map2 switches for bsp phase [Zircon]"};
//cvar_t q3map2_vis_switches = {CF_SERVER, "q3map2_vis_switches", "-game nexuiz -vis -saveprt -fast", "q3map2 switches for vis phase [Zircon]"};
//cvar_t q3map2_lit_switches = {CF_SERVER, "q3map2_lit_switches", "-game nexuiz -light -v -deluxe -faster -cheapgrid -filter -gamma 0.5", "q3map2 switches for light phase [Zircon]"};

//./dungeon/q3map2/q3map2.x86_64 -fs_game "dungeon"-meta -v
// dungeon\q3map2\q3map2_64_light_auto.exe -fs_game dungeon -game nexuiz -meta -v -mv 1000000 -mi 6000000 -samplesize 8 "dungeon/maps/my_map"

RELATED_ (FS_Dungeon_f)

	//// 6. cmdline - fake + unused
	//va_super (x_cmdline, MAX_OSPATH_EX_1024, "%s %s \"%s\"", x_compiler, x_switches, x_map_noext);
	//Con_PrintVarString (x_cmdline);

#define MAPSTAGE_CSG_1 1
#define MAPSTAGE_BSP_2 2
#define MAPSTAGE_VIS_3 3
#define MAPSTAGE_LIT_4 4

void VM_map_compile_with_callback (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(4, 4, VM_map_compile_with_callback); // h, file, stage, callback

	HMAP_HANDLE_BOILER; // handle == hmap
	ccs *filename = PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, filename);
	int stage = (int)PRVM_G_FLOAT(OFS_PARM2);

	if (in_range (MAPSTAGE_CSG_1, stage, MAPSTAGE_LIT_4) == false) {
		VM_WarningLinef (prog, "%s: invalid stage %d (expected 1 to 4) in %s", __func__, stage, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -7;
		return;
	}

	func_t mycallbackQCfunction = PRVM_G_INT(OFS_PARM3);

	map_t *pe = prog->open_dot_maps[handle];

	// 1. gamedir ("dungeon")
	ccs *x_gamedir = fs_numgamedirs ? fs_gamedirs[fs_numgamedirs - 1] : gamedirname1; // Con_PrintVarString (x_gamedir);

	// 2. full path to gamedir ("c:\quak\dungeon_lite\") with trailing slash	
	va_super (fp_basedir, MAX_OSPATH_EX_1024, "%s", fs_basedir /*has trailing slash*/);
	File_URL_Edit_SlashesForward_Like_Unix (fp_basedir);
	
	// 2. full path to gamedir ("c:\quak\dungeon_lite\") with trailing slash
	va_super (fp_gamedir, MAX_OSPATH_EX_1024, "%s%s/", fs_basedir /*has trailing slash*/, x_gamedir);
	File_URL_Edit_SlashesForward_Like_Unix (fp_gamedir); // Con_PrintVarString (fp_gamedir);

	stringlist_t list = {0};
	va_super (s_drive, 128, "%s", fp_gamedir);
	char *colonpos = strstr (s_drive, ":");
	if (colonpos) {
		colonpos = 0;
	}

	stringlistappend	(&list, s_drive);			// c:

	va_super (s_cd, 128, "cd " QUOTED_S, fp_gamedir);

	File_URL_Edit_SlashesBack_Like_Windows (s_cd);
	stringlistappendf	(&list, "cd");			// cd
	stringlistappend	(&list, s_cd);			// cd "c:\galaxy\inventory"

	// 2. compile binary ("dungeon/q3map2/q3map2.x86_64" or "dungeon/q3map2/q3map2.exe")
	// a. q3map2_compiler.string = "q3map2/q3map2.exe"
	char fp_compiler[MAX_OSPATH_EX_1024];
	switch (stage) {
	case MAPSTAGE_CSG_1:	c_dpsnprintf2 (fp_compiler, "%s%s", fp_gamedir, q3map2_csg_compiler.string); break;
	default:				c_dpsnprintf2 (fp_compiler, "%s%s", fp_gamedir, q3map2_compiler.string); break;
	}
//	Con_PrintVarString (fp_compiler);

	// 3. map
	// filename = "maps/mymap.map"
	// dungeon/maps/my_map

	va_super (fp_map_noext, MAX_OSPATH_EX_1024, "%s%s", fp_gamedir, filename); // maps/my_map
	File_URL_Edit_Remove_Extension (fp_map_noext); // Con_PrintVarString (fp_map_noext);

//
//c:
//cd c:\galaxy\inventory
//c:\galaxy\inventory\q3map2\q3map2_64_light_auto.exe -fs_basepath "C:/galaxy/" -fs_game inventory -game nexuiz -meta -v -mv 1000000 -mi 6000000 -samplesize 8 "C:/galaxy/inventory/maps/your_map" > c:\galaxy\inventory\maps\your_map_bsp.log

	char x_switches[MAX_OSPATH_EX_1024];
	// 4. switches
	ccs *s_stage = "";
	switch (stage) {
	case MAPSTAGE_CSG_1: s_stage = "csg"; c_dpsnprintf3 (x_switches, "-fs_basepath \"%s\" -fs_game %s %s", fp_basedir, x_gamedir, q3map2_csg_compiler.string); break;
	case MAPSTAGE_BSP_2: s_stage = "bsp"; c_dpsnprintf3 (x_switches, "-fs_basepath \"%s\" -fs_game %s %s", fp_basedir, x_gamedir, q3map2_bsp_switches.string); break;
	case MAPSTAGE_VIS_3: s_stage = "vis"; c_dpsnprintf3 (x_switches, "-fs_basepath \"%s\" -fs_game %s %s", fp_basedir, x_gamedir, q3map2_vis_switches.string); break;
	case MAPSTAGE_LIT_4: s_stage = "lit"; c_dpsnprintf3 (x_switches, "-fs_basepath \"%s\" -fs_game %s %s", fp_basedir, x_gamedir, q3map2_lit_switches.string); break;
	} // -fs_basepath "C:/galaxy/"
	//	Con_PrintVarString (x_switches);

	// 5. x_switches_full includes "maps/my_map"
	va_super (x_switches_full, MAX_OSPATH_EX_1024, "%s \"%s\"", x_switches, fp_map_noext); //Con_PrintVarString (x_switches_full);

	va_super (s_output, MAX_OSPATH_EX_1024, "%s_%s.log", fp_map_noext, s_stage);
	
	File_URL_Edit_SlashesBack_Like_Windows (s_output);
	stringlistappendf	(&list, QUOTED_S " %s > " QUOTED_S, fp_compiler, x_switches_full, s_output);			// cd "c:\galaxy\inventory"

	//REM Capture the exit code of the last command in a variable
	//SET LastExitCode=%ERRORLEVEL%
	//REM Use EXIT /B to exit the script with that specific code
	//EXIT /B %LastExitCode%

	stringlistappend	(&list, "REM Capture the exit code of the last command in a variable");
	stringlistappend	(&list, "SET LastExitCode=%ERRORLEVEL%");
	stringlistappend	(&list, "REM Use EXIT /B to exit the script with that specific code");
	stringlistappend	(&list, "EXIT /B %LastExitCode%");

	char *s = stringlist_join_lines_zalloc(&list);
	//Clipboard_Set_Text (s);
	
	FS_WriteFileStringQuick ("phase.bat", s);

	stringlistfreecontents (&list);

	// Verify that process is not running, etc.
	RELATED_ (VM_callfunction CLVM_ExecuteProgram PRVM_ExecuteProgram PRVM_EnterFunction OP_CALL0 SV_CheckContentsTransition next_stage_process)

	//prog->ExecuteProgram(prog, mycallback, "Callback undefined");
	pe->func_t_stored_int = mycallbackQCfunction;

	if (prog->process_syshandle) {
		VM_WarningLinef (prog, "%s: BUSY process already running for %d in %s", __func__, prog->process_index, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -5; // Dest and source are the same.
		return;
	}

	RELATED_ (FS_Shell_Blocking_f)

//	Con_PrintLinef ("%s %s", fp_compiler, x_switches_full);

	va_super (s_phase, MAX_OSPATH_EX_1024, "%s" "phase.bat", fp_gamedir);
	File_URL_Edit_SlashesForward_Like_Unix (s_phase);

#ifdef _WIN32
	prog->process_syshandle = System_Process_Create (s_phase, "" /*x_switches_full*/, fp_basedir);
#endif

// CREATE THE WHOLE FUCKER HERE
// cmd.exe
// System_Process_Create_As_BatFile_Make_Log
	RELATED_ (System_Process_Create_As_BatFile_Make_Log)

	if (!prog->process_syshandle) {
		VM_WarningLinef (prog, "%s: Unable to start process %s in %s", __func__, fp_compiler, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -6; // Dest and source are the same.
		return;
	}

	Con_DPrintLinef ("Process started with handle %p", prog->process_syshandle);
	
	prog->process_hmap = handle;
	prog->process_callback_qcfunc = mycallbackQCfunction;
	prog->process_stage = stage;
	


	PRVM_G_FLOAT(OFS_RETURN) = 1; // PROCESS STARTED OK.  THE CALLBACK GETS THE EXIT CODE.
}

// Who is going to call this?
// Client?  Server? How?
// StartFrame
// Client Disconnect what happens?
RELATED_ (SV_Frame SV_Physics)
RELATED_ (CL_UpdateScreen_SCR_DrawScreen CL_VM_UpdateView CL_Frame)
RELATED_ (MP_Draw)

// Returns 1 if just completed. 0 is not running. -1 if running.
#define STILL_RUNNING_NEG1		-1
#define NOT_RUNNING_0			0
#define JUST_COMPLETED_1		1
#define UNKNOWN_ERROR_NEG2		-2
// float map_compile_status_think (void) = #168; // March 30 2026 - returns 0 not running, -1 still running, 1 just completed
void VM_map_compile_status_think (prvm_prog_t *prog)
{
#ifdef _WIN32 // Only Windows supported at the moment
	if (!prog->process_syshandle) {
		PRVM_G_FLOAT(OFS_RETURN) = NOT_RUNNING_0; // Not running
		return;
	}
	
	int exit_code;
	int running = System_Process_Is_Still_Running_Neg1_Error (prog->process_syshandle, &exit_code);
	if (running == (int)true) {
		PRVM_G_FLOAT(OFS_RETURN) = STILL_RUNNING_NEG1; // Not running
		return;
	}
	
	if (running == not_found_neg1) {
		VM_WarningLinef (prog, "%s: Process not found -1 in %s", __func__, prog->name);
		prog->process_syshandle = NULL;
		PRVM_G_FLOAT(OFS_RETURN) = UNKNOWN_ERROR_NEG2; // Did not find the process.
		return;
	} 
	
	Con_DPrintLinef ("%s: Process completed with exit code %d in %s", __func__, exit_code, prog->name);

	// We do this now.
	prog->process_syshandle = NULL;
	
	// CALLBACK
	{
		// QuakeC: The callback looks like: void Map_Compile_Complete_Callback (float hmap, float stage, float status);
		PRVM_G_FLOAT(OFS_PARM0) = PLUS1(prog->process_hmap); // Need to +1 it.
		PRVM_G_FLOAT(OFS_PARM1) = prog->process_stage;
		PRVM_G_FLOAT(OFS_PARM2) = exit_code;

		prog->ExecuteProgram(prog, prog->process_callback_qcfunc, "Callback undefined");
	}
	
	PRVM_G_FLOAT(OFS_RETURN) = JUST_COMPLETED_1; // Not running
#else
	PRVM_G_FLOAT(OFS_RETURN) = -2; // Not supported
#endif
}


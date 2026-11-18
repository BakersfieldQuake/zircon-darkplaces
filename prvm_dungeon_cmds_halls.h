// prvm_dungeon_cmds_halls.h


RELATED_ (x = VM_map_load)

qbool block_names_cell_has_contents(stringlist_t *plist_blocknames, int row, int col, int numrows, int numcolumns)
{
	if (false == in_range_beyond (0, row, numrows))
		return 0; //HALL_CH_EMPTY_SPACE_32;
	if (false == in_range_beyond (0, col, numcolumns))
		return 0; //HALL_CH_EMPTY_SPACE_32;

	int		cell_idx	= row * numcolumns + col;
	if (false == in_range_beyond(0,cell_idx,plist_blocknames->numstrings))
		return 0; // BAD!

	ccs	*blockname = plist_blocknames->strings[cell_idx];// &rooms_string[cell_idx];

	qbool has_contents = blockname && blockname[0] != CHAR_NULL_0;
	return has_contents;
}

nsew_e room_names_passage_nsew_bits (stringlist_t *plist_blocknames, int row, int col, int numrows, int numcolumns)
{
//	if (row == 7 && col == 13) {
//		int j = 5;
//	}
	// Look at block north, south, east, west
	// If image does not contain room
	int bits = nsew_none_0;

	int ch_left		= block_names_cell_has_contents(plist_blocknames, row, col - 1, numrows, numcolumns);
	int ch_right	= block_names_cell_has_contents(plist_blocknames, row, col + 1, numrows, numcolumns);
	int ch_up		= block_names_cell_has_contents(plist_blocknames, row - 1, col, numrows, numcolumns);
	int ch_down		= block_names_cell_has_contents(plist_blocknames, row + 1, col, numrows, numcolumns);

	if (false == CH_IS_EMPTY(ch_up))		bits += nsew_north_1;
	if (false == CH_IS_EMPTY(ch_down))		bits += nsew_south_2;
	if (false == CH_IS_EMPTY(ch_left))		bits += nsew_west_8;
	if (false == CH_IS_EMPTY(ch_right))		bits += nsew_east_4;

	return (nsew_e)bits;
}

void Block_Remove_Component_Children (map_t *bm, ccs *s_component_children_removename)
{
	for_each_entity_undeleted(entnum,bm,dent)
		ccs *s_component_name = map_get (bm, entnum, "component_name");
		if (!s_component_name) continue; // Nameless
		if (false == String_Match (s_component_name, s_component_children_removename)) continue; // No match

		dent->is_considered_deleted = true; //int ok = map_entity_delete (hblock, entnum, DELETE_ACTION_DELETE_1);
	for_each_end
}

void Block_Remove_Odds_Roll (map_t *bm, int cell_idx)
{
	for_each_entity_undeleted(entnum,bm,dent)
		ccs *s_component_odds = map_get (bm, entnum, "component_odds");
		if (!s_component_odds) continue; // Does not have odds

		// This has odds
		float odds		= atof(s_component_odds);
		float my_roll	= lhrandom(0, 1); // random(); // 0 to 1 excluding 0 and 1
		if (my_roll <= odds)
			continue; // ROLL SUCCESS -- IT STAYS!

		// ROLL FAILED
		ccs *s_component_children = map_get (bm, entnum, "component_children");
		if (s_component_children)
			Block_Remove_Component_Children (bm, s_component_children);

		dent->is_considered_deleted = true; // //int ok = map_entity_delete (hblock, entnum, DELETE_ACTION_DELETE_1);
	for_each_end
}

// Door remove uses this IF EQUALS == EXACT FULL STRING MATCH
void Block_Remove_Component (map_t *bm, ccs *s_remove)
{
	int num_removed = 0;
	for_each_entity_undeleted(entnum,bm,dent)
		ccs	*s_component_bits = map_get (bm, entnum, "component_bits");
		if (!s_component_bits || false == String_Match(s_component_bits, s_remove))
			continue;

		dent->is_considered_deleted = true;
		num_removed ++;
	for_each_end
}

void Block_Names_Remove_Room_Doors_Walls(map_t *plist_block, stringlist_t *plist_blocknames, int row, int col, int numrows, int numcolumns)
{
	nsew_e door_bits = room_names_passage_nsew_bits (plist_blocknames, row, col, numrows, numcolumns);

	if (Have_Flag(door_bits, nsew_north_1))		Block_Remove_Component (plist_block, "-n"); // Has north door - Remove "-n"
	else										Block_Remove_Component (plist_block, "n");	// WALL NORTH - Remove "n"

	if (Have_Flag(door_bits, nsew_south_2))		Block_Remove_Component (plist_block, "-s"); //
	else										Block_Remove_Component (plist_block, "s");	//

	if (Have_Flag(door_bits, nsew_east_4))		Block_Remove_Component (plist_block, "-e"); //
	else										Block_Remove_Component (plist_block, "e");	//

	if (Have_Flag(door_bits, nsew_west_8))		Block_Remove_Component (plist_block, "-w"); //
	else										Block_Remove_Component (plist_block, "w");	//
}

// returns floor entnum right now
int Tile_Emit_Floor_Ceiling (map_t *hm, ccs *texturename_floor, ccs *texturename_ceil,
							  vec3_t blockmins, vec3_t blocksize, float floor_thickness_256_0, int flags)
{
	//int ceil_entnum = map_add_entity_with_classname (hm, "func_group_ceiling");


	//vector floor_mins	= { open_mins.x, open_mins.y, open_mins.z - thickness_256};
	vec3_t floor_mins	= { blockmins[0], blockmins[1], blockmins[2] - floor_thickness_256_0}; // Deepest
	vec3_t floor_maxs	= { blockmins[0] + blocksize[0], blockmins[1] + blocksize[1], blockmins[2] - 0 }; // Floor level

	vec3_t ceil_mins	= { floor_mins[0], floor_mins[1], floor_maxs[2] + blocksize[2] }; // Ceiling level
	vec3_t ceil_maxs	= { floor_maxs[0], floor_maxs[1], floor_maxs[2] + blocksize[2] + floor_thickness_256_0}; // Above ceiling

//	vec3_t texture_scale = { 1/8.0, 1/8.0, 0}; // Shell texture scale. 0.125

	// ADD FLOOR SEPARATE FOR GREAT LIGHTMAPS.
	int flor_entnum = 0;
	if (flags & 1) { // FLOOR
		flor_entnum = map_add (hm, "func_group_floor");
		//map_set (hm, flor_entnum, "lightmapscale", "0.5"); // "lightmapscale" "_lightmapscale" "_ls"
		map_add_cubeoid (hm, flor_entnum, floor_mins, floor_maxs, texturename_floor, 1/8.0 /*0.125*/, TEXSIDE_CEIL_16);
	}
	if (flags & 2) // CEIL
		map_add_cubeoid (hm, WORLD_0, ceil_mins, ceil_maxs, texturename_ceil, 1/8.0 /*0.125*/, TEXSIDE_FLOOR_32);
	return flor_entnum;
}

// How can we make this shell vs. non-shell?

// We still need to construct the shell.
// Or be able to supply the shell?
RELATED_ (SCR_mazegen3_f VM_worldkey)


// RELATED:
RELATED_ (Block_Names_Remove_Room_Doors_Walls)
RELATED_ (Block_Remove_Odds_Roll)

int write_map_from_blocks (ccs *filename_mapname, ccs *mymap_txt_blob, stringlist_t *plist_blocknames, int numrows, int numcolumns,
					  vec3_t blocksize, vec3_t vcentral /*0,0,0*/, int action_flags, ccs *floortex)
{
// TODO: Eventually we would want to place triggers.

	//int errorcode = SUCCESS_OK_0;
	map_t list_hmap = {0};
	map_t *hm = &list_hmap;

	map_add		(hm, "worldspawn"); // WORLD_0
	map_set		(hm, WORLD_0, "_keeplights","1");
	map_set		(hm, WORLD_0, "_blocksize","2048 2048 2048"); // Visibility divisions
	map_set		(hm, WORLD_0, "mapversion","220");
	map_set_fmt	(hm, WORLD_0, "message", "Dungeon Level %d", 1);

	char *worldgrid_compressed_za = base64zip_zalloc(mymap_txt_blob); // Baker: Getting about 90% compression 9927 => compress 680 => both 908
	//
	// THE ACTION / CLEANUP
	//
	//
	int worldgrid_compressed_slen = strlen(worldgrid_compressed_za);
	int num_segments = ceil(worldgrid_compressed_slen / 400.0); // Chunks of 400
	for (int segnum = 0; segnum < num_segments; segnum ++) {
		int seg_start = segnum * 400;

		int seg_beyond = seg_start + 400;
		if (seg_beyond > worldgrid_compressed_slen)
			seg_beyond = worldgrid_compressed_slen;
		int seg_len = seg_beyond - seg_start;

		char *seg_za = Z_StrDup_Len_Z (&worldgrid_compressed_za[seg_start], seg_len);
		va_super (keyname, 512, WORLDGRID_ZIP_BASE64 "%d", segnum);
		//map_set		(hm, WORLD_0, WORLDGRID_ZIP_BASE64, worldgrid_compressed_za);
		map_set		(hm, WORLD_0, keyname, seg_za);
		Mem_FreeNull_ (seg_za);
	}

	map_set_fmt	(hm, WORLD_0, WORLDGRID_BLOCKSIZE, "%d %d %d", (int)blocksize[0], (int)blocksize[1], (int)blocksize[2]);

	Mem_FreeNull_ (worldgrid_compressed_za);
	//
	// END WORLDSPAWN
	qbool has_player_start_been_placed = false;

	vec3_t space_required = { blocksize[0] * numcolumns /*= 9600*/, blocksize[1] * numrows, blocksize[2] };

	vec3_t placemat_mins = {-space_required[0]/2.0, -space_required[1]/2.0, 0 };
	vec3_t placemat_maxs = { space_required[0]/2.0,  space_required[1]/2.0, blocksize[2] };

	// Translate to destination if not 0,0,0
	VectorAdd (placemat_mins, vcentral, placemat_mins);
	VectorAdd (placemat_maxs, vcentral, placemat_maxs);

	// SHELL?  Not now ..
	int room_number = 0;
	for (int row = 0; row < numrows; row ++) {
		int flipped_row = (numrows - 1) - row; // Flip the row so 0,0 // 20x13 // 20-
		for (int col = 0; col < numcolumns; col ++) {
			int cell_idx = row * numcolumns + col;

			if (false == in_range_beyond(0, cell_idx, plist_blocknames->numstrings)) continue; // Paranoid bounds check ...
			ccs *filename_block = plist_blocknames->strings[cell_idx];

			if (filename_block[0] == CHAR_NULL_0)
				continue; // ""

			map_t list_block = {0};
			map_t *bx = &list_block;

			int errorcode = map_load_or_errorcode(bx, filename_block);

			if (errorcode != SUCCESS_OK_0) {
				Con_PrintLinef (CON_WARN "Failed to parse .map %s with errorcode %d .. ignoring ..", filename_block, errorcode);
				continue;
			}

			ccs	*worldhints = map_get (bx, ENTITY_WORLD_0, "worldhints");

			qbool is_room = worldhints && String_Contains_Caseless(worldhints, "[room]");
			qbool is_ceil = true;
			qbool is_floor = true;
			int floor_flags = is_ceil * 2 + is_floor;

			// TRANSLATE
			// Requires map block has 0,0,0 based and is 256 x 256 size.
			vec3_t blockmins	= {col * blocksize[0], flipped_row * blocksize[1], 0 };
			vec3_t blockcenter  = {blockmins[0] + (blocksize[0] / 2.0), blockmins[1] + (blocksize[1] / 2.0), 0 }; // Translate amount

			// Add The placemat mins in (those are negative usually)
			VectorAdd (blockmins,	placemat_mins, blockmins);
			VectorAdd (blockcenter,  placemat_mins, blockcenter);

			// This emits to world!
			int florentnum = Tile_Emit_Floor_Ceiling (hm, floortex, floortex, blockmins, blocksize, /*thick*/ 32, floor_flags);
			map_set_fmt (hm, florentnum, "comment", "row %d col %d", row, col);

			// Mins is the Southwest corner of the placemat
			if (1 || action_flags) { // We could customize this
				csgc_classname_delete (bx, "func_group_floor"); // Remove room floors
				if (is_room == false)
					csgc_classname_delete (bx, "info_player_start");
			}

			// WOWZERS - conditional doors.  For rooms.
			// Need to figure out what doors do and don't exist and remove them.
			if (is_room) {
				if (has_player_start_been_placed == false) {
					int ent_start = map_add (bx, "info_player_start");
					map_set		(bx, ent_start, "origin", "0 0 64"); // Center of tile
					map_set_fmt	(bx, ent_start, "comment", "row %d col %d", row, col);
					has_player_start_been_placed = true;
				}

				// Kill unnecessary doors + reveal those walls
				// Reveal needed doors + kill unneeded walls
				Block_Names_Remove_Room_Doors_Walls (bx, plist_blocknames, row, col, numrows, numcolumns);
			} // End of room specific

			// This checks component_odds roll items.  If failed, removes component_children also.
			Block_Remove_Odds_Roll (bx, cell_idx);

			// Give targetnames in .map non-colliding names -- "door1" and renaming it "_block2_door1"
#ifdef _DEBUG
			int num_changed = /*used*/
#endif
			//map_prefix_standard_targetnamesf (bx, "_block%d_", cell_idx); // "target" and "targetname"
			map_prefix_dungeon_targetnamesf (bx, "_block%d_", cell_idx); // "target" and "targetname" and "door_outline"

			// ROOM PART 2 - Late to avoid targetname change
			if (is_room) {
				// ROOM NAVIGATION
				int info_room_entnum = map_find(bx, "classname", "info_room");
				if (info_room_entnum != not_found_neg1) {
					// ADDRESS info_room "info_room"
					// "targetname" "room_20"
					// "colrow0" "" // MAKE THIS A QC FIELD

					va_super (room_name, 128, "room_%d", room_number);
					/*qbool isok0 = */map_set (bx, info_room_entnum, "targetname", room_name);
					/*qbool isok1 = */map_set_fmt (bx, info_room_entnum, "colrow0", "%d %d", col, row);

					//"room_targetname" "room_1" /////////////////////// ADD      MAKE THIS A QC FIELD

					#define for_each_entity_undeleted_with_key_value(NN,HM,DENT,KEY,VALUE)\
						for (int NN = 0; NN < HM->count; NN ++) { \
							entityx_s	*DENT = &HM->entity[NN]; \
							if (DENT->is_considered_deleted) continue; \
							ccs *s = map_get (HM, NN, KEY); \
							if (!s || false == String_Match(s, VALUE)) \
								continue; /* Wrong classname */ // Ender

					// func_door_rotating
					for_each_entity_undeleted_with_key_value(entnum,bx,dent,"classname","func_door_rotating")
						// WE HAVE A VALUE MATCH
						map_set (bx, entnum, "room_targetname", room_name); //"room_targetname" "room_1"
					for_each_end

					// ADDRESS info_player_coop starts
					// ADDRESS

					// info_player_coop
					// "room_targetname" "room_1" //////////////////////////////////// ADD
					// "component_bits" "s"
					// "targetname" "room_1_south"
					for_each_entity_undeleted_with_key_value(entnum,bx,dent,"classname","info_player_start")
						// WE HAVE A VALUE MATCH
						map_set (bx, entnum, "room_targetname", room_name); //"room_targetname" "room_1"

						// SET TARGETNAME

						ccs *component_bits = map_get (bx, entnum, "component_bits");
						if (component_bits) {
							va_super (coop_targetname, 128, "room_%d_%s", room_number, component_bits);
							map_set (bx, entnum, "targetname", coop_targetname); //"room_targetname" "room_1"
						}
					for_each_end
				}
				room_number ++;
			} // End of room specific

			map_move		(bx, blockcenter /*translate_amount*/);

			map_append_map	(hm, bx);
		} // col
	} // row

	qbool isok = map_save_as (hm, filename_mapname);
	map_freecontents (hm);
	return isok ? SUCCESS_OK_0 : -1;
}

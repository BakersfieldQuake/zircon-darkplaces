// csg_carve_cubeoid.c.h


RELATED_ (csgwx_floorage brush_add_cubeoid map_add_cubeoid)

static int Cubeoid_Carve_Num_Created (
	map_t *plist,
	entityx_s *d_ent,
	//const brush_s *r_brush_world,
	int r_brushnum,
    const vec3_t brmins,
    const vec3_t brmaxs,
    const vec3_t carvemins,
    const vec3_t carvemaxs)
{
    vec3_t overlapmins;
    vec3_t overlapmaxs;
    vec3_t mins;
    vec3_t maxs;
    int num_created = 0;

    /*
     * Find intersection between brush and carve.
     */
    overlapmins[0] = max(brmins[0], carvemins[0]);
    overlapmins[1] = max(brmins[1], carvemins[1]);
    overlapmins[2] = max(brmins[2], carvemins[2]);

    overlapmaxs[0] = min(brmaxs[0], carvemaxs[0]);
    overlapmaxs[1] = min(brmaxs[1], carvemaxs[1]);
    overlapmaxs[2] = min(brmaxs[2], carvemaxs[2]);

    // Check for no intersection
    if (overlapmins[0] >= overlapmaxs[0] ||
        overlapmins[1] >= overlapmaxs[1] ||
        overlapmins[2] >= overlapmaxs[2])
    {
        //printf("No intersection - original cubeoid remains:\n");
		zpr ("This is an error because we should have checked first");
        return -1;
    }

    // WEST
    if (brmins[0] < overlapmins[0]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);
		maxs[0] = overlapmins[0]; // Baker: Clipping X reducing to overlapmins
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_WEST_0); // WEST
		num_created ++;
    }

    // EAST
	RELATED_ (FACE_EAST_1)
    if (overlapmaxs[0] < brmaxs[0]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);
        mins[0] = overlapmaxs[0]; // Baker: Clipping X increasing mins to overlapmins
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_EAST_1); // EAST
		num_created ++;
    }

	// SOUTH
    if (brmins[1] < overlapmins[1]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);
        mins[0] = overlapmins[0]; // Baker: Raising mins of X
        maxs[0] = overlapmaxs[0]; // Baker: Reducing maxs of X
        maxs[1] = overlapmins[1]; // Baker: Reducing Y max to overlap mins
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_SOUTH_2); // FRONT
		num_created ++;
    }

	// NORTH
    if (overlapmaxs[1] < brmaxs[1]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);
        mins[0] = overlapmins[0]; // Baker: Raising mins of X
        maxs[0] = overlapmaxs[0]; // Baker: Reducing maxs of X
		mins[1] = overlapmaxs[1]; // Baker: Increasing Y mins to overlap maxs
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_NORTH_3); // BACK
		num_created ++;
    }

    // FLOOR
    if (brmins[2] < overlapmins[2]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);

        mins[0] = overlapmins[0];
        maxs[0] = overlapmaxs[0];

		mins[1] = overlapmins[1];
        maxs[1] = overlapmaxs[1];
        maxs[2] = overlapmins[2];
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_FLOOR_4); // FLOOR
		num_created ++;
    }

    // CEIL
    if (overlapmaxs[2] < brmaxs[2]) {
		VectorCopyDestSrc (mins, brmins);
		VectorCopyDestSrc (maxs, brmaxs);
        mins[0] = overlapmins[0];
        mins[1] = overlapmins[1];
        mins[2] = overlapmaxs[2];

        maxs[0] = overlapmaxs[0];
        maxs[1] = overlapmaxs[1];
		brush_add_as_cubeoid_texturing_like (plist, d_ent, r_brushnum, mins, maxs, FACE_CEIL_5); // CEILING
		num_created ++;
    }

    return num_created;
}

RELATED_ (csgax_frost)
void csg_get_gamedir_realpath_plus_slash (ccs *mappath_realpath_full, char *valuebuf, size_t valuebuf_sizeof)
{
	va_copy_16384 (mygamedir_realpath, mappath_realpath_full);
	File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (mygamedir_realpath); // Strip mymap.map
	File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (mygamedir_realpath); // Strip maps/mymap.map (Now in gamedir)
	dp_strlcpy (valuebuf, mygamedir_realpath, valuebuf_sizeof);
	dp_strlcat (valuebuf, "/", valuebuf_sizeof);
}

// #includes/library.tga ==> maps/#includes/library.map
char *sinza_from_include_tga (ccs *include_slash_map_dot_tga, ccs *mappath_realpath_full, char *realpath_mapout, size_t realpath_mapout_sizeof)
{
	char *sinza = NULL;

	ccs *map_dot_tga = File_URL_SkipPath(include_slash_map_dot_tga); // includes/library.tga ==> library.tga
	va_copy_1024 (map_no_ext, map_dot_tga);
	File_URL_Edit_Remove_Extension (map_no_ext);
	if (mappath_realpath_full == NULL) {
		// console command
		va_super (filename, MAX_OSPATH_EX_1024, "maps/#includes/%s.map", map_no_ext);
		sinza = FS_LoadFileQuick (filename);
		dp_strlcpy (realpath_mapout, filename, realpath_mapout_sizeof);
	} else {
		// csg tool
		char gamedirpath[MAX_OSPATH_EX_1024];
		// mappath_realpath_full is apparently gamedir? C:/galaxy/zircon/
		csg_get_gamedir_realpath_plus_slash (mappath_realpath_full, gamedirpath, sizeof(gamedirpath));
		va_super (filename, MAX_OSPATH_EX_1024, "%s" "maps/#includes/%s.map", gamedirpath, map_no_ext);
		//c_strlcatf (filename, "maps/#includes/%s.map", map_no_ext);
		sinza = FS_LoadFileString_Unsafe_ZAlloc (filename);
		dp_strlcpy (realpath_mapout, filename, realpath_mapout_sizeof);
	}
	return sinza;
}

//map_carve_cubeoid (plist, &list_map1);
RELATED_ (map_carve_super)
CALLERS_ (csgax_includes)
void map_carve_cubeoid (map_t *plist, map_t *carver)
{
	int num_made = 0;
	// Do each carver carver that has a brush volume
	for_each_entity_with_brushes (rentnum, carver, r_ent)
		for_each_brush_not_a_patch (rbrushnum, r_ent, r_brush_carve)
			// DOES NOT HAVE TO BE CUBEOID -- will use CUBEOID BOUNDS
			Vector3DeclareCopyEx (carve_mins, r_brush_carve->bxmins);
			Vector3DeclareCopyEx (carve_maxs, r_brush_carve->bxmaxs);
			//float *carve_mins = r_brush_carve->bxmins;
			//float *carve_maxs = r_brush_carve->bxmaxs;

// CARVE THE WORLD
			for_each_entity_with_brushes (entnum, plist, d_ent)
				for_each_brush_cubeoid_undeleted (d_brushnum, d_ent, d_brushno)
#ifdef _DEBUG
					if (d_brushno->is_cubeoid == false) {
						int j = 5;
					}
					if (d_brushno->brushrowlist.count != 6) {
						int j = 5;
					}
#endif
					// THIS CREATES MORE BRUSHES AT END but they should not collide
					qbool do_collide = Boxes_Collide_Epsilon_3D (d_brushno->bxmins, d_brushno->bxmaxs,
						carve_mins, carve_maxs);
					if (do_collide == false)
						continue; // NO COLLISION

					// We usually expect non-zero?
					// What if exact size?
					zpr ("Colliding carve %d:%d with world %d:%d", rentnum, rbrushnum, entnum, d_brushnum);
#ifdef _DEBUG
					DebugPrintLinef ("Colliding carve %d:%d with world %d:%d", rentnum, rbrushnum, entnum, d_brushnum);
#endif
					Vector3DeclareCopyEx (d_brush_mins, d_brushno->bxmins);
					Vector3DeclareCopyEx (d_brush_maxs, d_brushno->bxmaxs);
					d_brushno->is_considered_deleted = true;
					int num_made_here =
						Cubeoid_Carve_Num_Created (plist, d_ent, d_brushnum,
							d_brush_mins, d_brush_maxs,
							carve_mins, carve_maxs
						);
#ifdef _DEBUG
					DebugPrintLinef ("num_made_here = %d", num_made_here);
#endif
					num_made += num_made_here;


				for_each_end // for_each_brush_cubeoid
				// Let's assume a whole func_group or entity does not get consumed.
				//d_ent->is_considered_deleted = true;
			for_each_end // for_each_entity_with_brushes
// CARVE

		for_each_end // for_each_brush_not_a_patch
	for_each_end // entities with brushes

}



//map_carve_cubeoid (plist, &list_map1);
CALLERS_ (csgc_carve_includes) //
// SOLID
int map_carve_cubeoid_single (map_t *hm, int carve_entnum, int carve_brushnum, const vec3_t carve_mins, const vec3_t carve_maxs)
{
	int num_made = 0;

	for_each_entity_2 (d_entnum, d_ent, hm)
		if (d_ent->has_brushes == false)
			continue;
		if (d_ent->has_include_texture)
			continue; // DO NOT CARVE AGAINST CARVERS

		for_each_brush_2 (d_brushnum_carve_target, d_brush_carve_target, hm, d_entnum)
			if (d_brush_carve_target->is_cubeoid == false)
				continue;

			// Cubeoid Carve Go!
			Vector3DeclareCopyEx (brmins, d_brush_carve_target->bxmins);
			Vector3DeclareCopyEx (brmaxs, d_brush_carve_target->bxmaxs);

			// THIS CREATES MORE BRUSHES AT END but they should not collide
			vec3_t overlapmins, overlapmaxs;
			overlapmins[0] = max(brmins[0], carve_mins[0]);
			overlapmins[1] = max(brmins[1], carve_mins[1]);
			overlapmins[2] = max(brmins[2], carve_mins[2]);

			overlapmaxs[0] = min(brmaxs[0], carve_maxs[0]);
			overlapmaxs[1] = min(brmaxs[1], carve_maxs[1]);
			overlapmaxs[2] = min(brmaxs[2], carve_maxs[2]);

			// Check for no intersection
			if (OVERLAP_IS_NONE(overlapmins))
				continue; // No intersection

			d_brush_carve_target->is_considered_deleted = true;

			DebugPrintLinef ("Collsion against entnum %d brushnum %d mins "
									V3G " maxs " V3G,
				 d_entnum, d_brushnum_carve_target, VS(brmins), VS(brmaxs));

			DebugPrintLinef ("Overlap mins " V3G " maxs " V3G, VS(overlapmins),
				VS(overlapmaxs));

			RELATED_ (map_load_from_string)

			RELATED_ (map_carve_cubeoid_against_any) // MAGICAL BEAST
#if 0
			int num_made = map_carve_cubeoid_against_any (hm, d_entnum, d_brushnum_carve_target, r_carve_brush);
#else
			int num_made_here =
				Cubeoid_Carve_Num_Created (hm, d_ent, d_brushnum_carve_target,
					brmins, brmaxs,
					carve_mins, carve_maxs
				);
#endif

#ifdef _DEBUG
			DebugPrintLinef ("num_made_here = %d", num_made_here);
#endif
			num_made += num_made_here;
			//break;
		for_each_end // for_each_brush_cubeoid
	for_each_end // entities with brushes

	//int my_num_made = num_made;
	return num_made;
}

int csgm_carve (map_t *plist, /*"#includes/"*/ ccs *MYKEY, ccs *mappath_realpath_full)
{
	CSGM_START_

phase_1: //

	for_each_entity_original_count_undeleted (carve_entnum, plist, d_ent)
		//qbool entity_has_include_texture = false; // Remove a func_group
		for_each_brush_original_count (carve_brushnum, d_ent, d_brush)
			char include_texture[Q3PATHLENGTH_64] = {0};

			for_each_brush_face (facenum, d_brush, d_face)
				if (false == String_Starts_With (d_face->pbrtexture, MYKEY /*"#includes/"*/))
					continue;
				c_strlcpy (include_texture, d_face->pbrtexture);
				break;
			for_each_end // for_each_brush_face

			// If not an include texture continue ...
			if (include_texture[0] == CHAR_NULL_0)
				continue;

active_include:

			// Delete the brush.
			d_brush->is_considered_deleted = true;

			// if single brush entity that isn't world, consider that deleted too.
			if (carve_entnum > WORLD_0 && d_ent->brushlist.count == 1) {
				d_ent->is_considered_deleted = true;
			}

			// NOW PERFORM THE CARVE
			Vector3DeclareCopyEx (carve_brmins, d_brush->bxmins);
			Vector3DeclareCopyEx (carve_brmaxs, d_brush->bxmaxs);
			vec3_t carve_size; VectorSubtract (carve_brmaxs, carve_brmins, carve_size);
			zpr ("%04d: Carving " VECTOR3_5d1F " to " VECTOR3_5d1F " (size " VECTOR3_G " )",
                _num_done, VECTOR3_SEND(carve_brmins), VECTOR3_SEND(carve_brmaxs), VECTOR3_SEND(carve_size));

			DebugPrintLinef ("%04d: Carving " VECTOR3_5d1F " to " VECTOR3_5d1F " (size " VECTOR3_G " )",
                _num_done, VECTOR3_SEND(carve_brmins), VECTOR3_SEND(carve_brmaxs), VECTOR3_SEND(carve_size));

			//map_carve_cubeoid (plist, &list_map1);
			map_carve_cubeoid_single (plist, carve_entnum, carve_brushnum, carve_brmins, carve_brmaxs);
			//break;

			// NO map_append_map_with_move (plist, &list_map1, move_amount);
			CSGM_NUM_DONE_PLUS_

		for_each_end // for_each_brush_or_patch

	for_each_end // each entity

	CSGM_END_
}


int csgmsub_find_delete_includes (map_t *hm, ccs *MYKEY, ccs *mappath_realpath_full, int32list_s *plist_entnums, stringlist_t *plist2)
{
	int num_done = 0;
	for_each_entity_afterworld_original_2 (carve_entnum, d_ent, hm)
		if ( false == entity_has_include_texture (d_ent, MYKEY, d_ent->texturename_work, sizeof(d_ent->texturename_work) ))
			continue; // NO INCLUDE TEXTURE

		// ENTITY ACCEPTED
		ccs *include_texture = d_ent->texturename_work;
		d_ent->is_considered_deleted = true;
		int32s_add1 (plist_entnums, carve_entnum);
		num_done ++;


		// IS BUILDING A DUP?
		qbool added = stringlistappend_unique_didAdd(plist2, d_ent->texturename_work);
		if (added == false)
			continue;

		// BUILDING IS NOT A DuP
		char filename_realpath[MAX_OSPATH_EX_1024];
		char *sinza = sinza_from_include_tga(include_texture, mappath_realpath_full, filename_realpath, sizeof(filename_realpath) );

		// We must replace carriage returns and convert to newlines for LIST COLUMN SORT which uses CR13 chars.
		int count = 0;
		String_Edit_Replace_Char (sinza, CHAR_CARRIAGE_RETURN_13, CHAR_NEWLINE_10, &count);

		stringlistappend (plist2, sinza);
		Mem_FreeNull_ (sinza);

	for_each_end // each entity
	return num_done;
}

// m
qbool map_move_prefix_targetnames (map_t *plist, const vec3_t move_amount, int prefixnum, stringlist_t *list_targetname_fields)
{

	// Move it into position
	map_move_epairs_origin		(plist, move_amount);
	map_move_all_brushes_patches			(plist, move_amount);

#pragma message ("TODO: rotate")
#pragma message ("TODO: targetnames")
	map_metric_again (plist); // Update the new bounds of everything.

	return true;
}


// output to maps/#includes_sheet.map
// 256 space between
int csgc_includes_sheet (void)
{
	int cursor_x = 0;
	const int x_spacing_256 = 256;
	map_t list_hmap = {0};
	map_t *hm = &list_hmap;

	char includes_sheet_map_filename[] = "maps/#includes_sheet.map";
	map_add_with_title_keeplights (hm, "Includes Sheet");

	// Add a ground level brush
	vec3_t floor_mins = {-1024, -1024, -1024};
	vec3_t floor_maxs = { 16384, 16384, 0};
	map_add_cubeoid (hm, WORLD_0, floor_mins, floor_maxs, MAPS_SH_FANG_GRASS, /*texscale*/ 1.0, TEXSIDE_ALL_63);

	// Find all the maps/#includes/
	stringlist_t list_include_filenames = {0};
	stringlistappend_from_dir_pattern (&list_include_filenames, "maps/#includes", ".map", q_strip_exten_false);

	for_each_stringlist_string (n, list_include_filenames, this_map_filename)
		map_t this_map = {0};
			// armundos
			ccs *trail_filename_dot = File_URL_SkipPath (this_map_filename);
			va_super (texturename, Q3PATHLENGTH_64, "#includes/%s", trail_filename_dot);
			File_URL_Edit_Remove_Extension (texturename);

			// Load map
			int map_load_or_errorcode (map_t *hm, ccs *filename);
			map_load_or_errorcode (&this_map, this_map_filename);

			//float descent = this_map.bbentsmins[2];  // Like -140
			float fake_height = this_map.bbentsdims[2] + this_map.bbentsmins[2];// 1000 + -140
			int entnum = map_add (hm, "func_group_building");
			map_set_vec3_g (hm, entnum, "_actual_size", this_map.bbentsdims);
			map_set_vec3_g (hm, entnum, "_ascent_only_size", this_map.bbentsdims);
			map_set_float (hm, entnum, "_descent", this_map.bbentsmins[2]); // like -140

			//Vector3DeclareSet (boxmins, cursor_x, /*y*/ 0, descent);
			Vector3DeclareSet		(boxmins, cursor_x, /*y*/ 0, 0);
			Vector3DeclareCopyEx	(boxmaxs, boxmins);
			boxmaxs[0] += this_map.bbentsdims[0];
			boxmaxs[1] += this_map.bbentsdims[1];
			boxmaxs[2] += fake_height; //this_map.bbentsdims[0];
			//vec3_t boxmaxs; VectorAdd(boxmins, this_map.bbentsdims, boxmaxs);
			cursor_x += this_map.bbentsdims[0] + x_spacing_256;

			vec3_t _ascent_only_size; VectorSubtractSize (boxmaxs, boxmins, _ascent_only_size);
			map_set_vec3_g (hm, entnum, "_ascent_only_size", _ascent_only_size);

			map_add_cubeoid (hm, entnum, boxmins, boxmaxs, texturename, /*texturescale*/ 1.0, TEXSIDE_ALL_63);
			zpr ("Added %4d: %s => %s @ " VECTOR3_G, n, this_map_filename, texturename, VECTOR3_SEND(boxmins));
		map_freecontents (&this_map);
	for_each_end // each file
	//stringlist_condump_with_linenums (&list_include_filenames);
	stringlistfreecontents (&list_include_filenames);

	qbool isok = map_save_as (hm, includes_sheet_map_filename);
	if (!isok) {
        zpr ("Couldn't write includes sheet");
	} else {
		zpr (CON_CYAN "Wrote %s", includes_sheet_map_filename);
	}
	map_freecontents (hm);

	return 1;
}



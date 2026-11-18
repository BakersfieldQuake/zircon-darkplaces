// csg_perform_weld.c.h


// 1. All triangles with a normal of x get put into a group.
// 2. Search for triangles with (???) and turn them into
// 3. Triangle brushes with same Z min and same Z max and every point x,y,z, the x y or z matches a bbox corner.
// 4. Same bbox.
void MergeBounds(const vec3_t mins1, const vec3_t maxs1,
                 const vec3_t mins2, const vec3_t maxs2,
                 vec3_t out_mins, vec3_t out_maxs)
{
    int i;
    for (i = 0; i < 3; i++) {
        /* Find the absolute minimum for the low corners */
        out_mins[i] = (mins1[i] < mins2[i]) ? mins1[i] : mins2[i];

        /* Find the absolute maximum for the high corners */
        out_maxs[i] = (maxs1[i] > maxs2[i]) ? maxs1[i] : maxs2[i];
    }
}

CALLERS_ (entity_perform_weld_num_added)
int brush_find_yz_match(entityx_s *r_ent, const vec3_t bbmins, const vec3_t bbmaxs, int start_idx, int beyond_idx, ccs *texturename)
{
	// p_ent_world->brushlist.count
	//for (int brush_idx = start_idx; brush_idx < beyond_idx /*r_ent->brushlist.count*/; brush_idx ++) {
	for_each_entity_brush_start_beyond (brushnum, r_brush, r_ent, start_idx, beyond_idx)
		if (! (r_brush->bxmins[1] == bbmins[1] ))	continue; // Not same Y min
		if (! (r_brush->bxmins[2] == bbmins[2] ))	continue; // Not same Z min
		if (! (r_brush->bxmaxs[1] == bbmaxs[1] ))	continue; // Not same Y max
		if (! (r_brush->bxmaxs[2] == bbmaxs[2] ))	continue; // Not same Z max

		ccs *non_common_texturename = brush_texture_first_non_common (r_brush, NULL);
		if (non_common_texturename == NULL) continue; // No texture
		if (String_Match (non_common_texturename, texturename) == false)
			continue; // Wrong texture
		// full match
		return brushnum;
	} // next brush.
	return not_found_neg1;
}

CALLERS_ (weld)
int brush_find_triangular_with_xyz_match(const entityx_s *r_ent, const vec3_t bbmins, const vec3_t bbmaxs, int ignore_brushnum, int hard_limit)
{
	for_each_entity_brush_start_beyond (brushnum, r_brush, r_ent, /*start*/ 0, hard_limit)
		if (brushnum == ignore_brushnum)											continue;
		if (BRUSH_IS_5_SIDED_TRI(r_brush) == false /*triangle + top + bottom is 5*/)continue;

		if (! (r_brush->bxmins[0] == bbmins[0] )) continue; // Not same X min
		if (! (r_brush->bxmins[1] == bbmins[1] )) continue; // Not same Y min
		if (! (r_brush->bxmins[2] == bbmins[2] )) continue; // Not same Z min
		if (! (r_brush->bxmaxs[0] == bbmaxs[0] )) continue; // Not same X max
		if (! (r_brush->bxmaxs[1] == bbmaxs[1] )) continue; // Not same Y max
		if (! (r_brush->bxmaxs[2] == bbmaxs[2] )) continue; // Not same Z max

		// Full match
		return brushnum;
	} // next brush.
	return not_found_neg1;
}

//

int map_merge_rect_rows_metric_new_brush (map_t *hm, int entnum, ccs *texturename)
{
	CSGM_START_
	ENT_DEFINE (d_ent, hm, entnum);
	// Create a list of cubeoid brushes with this texture on the ceil face.
	// Create a list of south Y.  Unique it and work each.
	floatlist_s z_list = {0};
	floatlist_s y_list = {0};

	for_each_entity_brush (brushnum, r_brush, d_ent)
		if (r_brush->is_cubeoid == false) continue;
		int ceil_index = r_brush->cubeoid_index[FACE_CEIL_5];
		brushrow_s *face_ceil = &r_brush->brushrowlist.brushrow[ceil_index];
		ccs *ceil_texture = face_ceil->pbrtexture;
		if (String_Match (ceil_texture, texturename) == false) continue;
		//floats_add1 (&y_list, face_ceil->bfmins[1]);
		floats_add1 (&y_list, r_brush->bxmins[1]); // Y Min
		floats_add1 (&z_list, r_brush->bxmaxs[2]); // Z Max (surface)
	for_each_end // brush

	zpr (texturename);
	floats_sort_unique (&y_list);
	floats_sort_unique (&z_list);

	floats_dump (&y_list);
	floats_dump (&z_list);

	// SOUTH
	for_each_floatlist (n, z_list, z_ceil)
		for_each_floatlist (n, y_list, y_row)
			Vid_SetWindowTitlef ("D. Merging rect rows Y = %.1f Z = %.1f", y_row, z_ceil);

			int hard_limit = d_ent->brushlist.count;
			for_each_entity_brush_start_beyond (orig_src_brushnum, d_brush1, d_ent, 0, hard_limit)
				// We may replace the brush and the brush number.
				int src_brushnum = orig_src_brushnum;

				if (d_brush1->is_cubeoid == false) continue; // Not cubeoid
				if (d_brush1->bxmaxs[2] != z_ceil) continue; // Wrong z
				if (d_brush1->bxmins[1] != y_row) continue; // Wrong y

				// Now try to merge this with other brushes.
				for_each_entity_brush_start_beyond (brushnum2, d_brush2, d_ent, src_brushnum + 1, hard_limit)
					if (d_brush2->is_cubeoid == false) continue; // Not cubeoid
					//if (brushnum2 == src_brushnum) continue; // This is us.  CAN'T HAPPEN NOW.
					if (d_brush2->bxmaxs[2] != z_ceil) continue; // Wrong z
					if (d_brush2->bxmins[1] != y_row) continue; // Wrong y

					// Insist on same Z height and same Y height
					if (d_brush2->bxdims[2] != d_brush2->bxdims[2]) continue; // Wrong Z height
					if (d_brush2->bxdims[1] != d_brush2->bxdims[1]) continue; // Wrong Y height

					if (false == brush_xy_touches_brush(d_brush2, d_brush1)) continue; // They don't touch

					vec3_t newmins, newmaxs;
					MergeBounds (d_brush1->bxmins, d_brush1->bxmaxs, d_brush2->bxmins, d_brush2->bxmaxs, newmins, newmaxs);

					// MUST DELETE NOW!  New brush can trash pointer if reallocated.
					d_brush1->is_considered_deleted = true;
					d_brush2->is_considered_deleted = true;

					// Replace the faces.
					int newbrushnum = brush_add_as_cubeoid_texturing_like (hm, d_ent, src_brushnum, newmins, newmaxs, FACE_ADJACENT_ANY_18);

					BRUSH_DEFINE (d_brush3, hm, entnum, newbrushnum);
					if (0) {
						map_metric_brush (hm, entnum, newbrushnum);
					}

					// BEGIN THE CRAZY
					d_brush1 = d_brush3;
					src_brushnum = newbrushnum;
				for_each_end // brush
			for_each_end // brush
		for_each_end // Y min (SOUTH)
	for_each_end // Z max (CEIL)

	floats_freecontents (&y_list);
	floats_freecontents (&z_list);
	CSGM_END_FUNC_
}

CALLERS_ (csgc_weld)
int csgc_weld_entity_merge_2_square_triangles_to_rect_metric_new_brush (map_t *hm, int entnum, ccs *texturename)
{
	// 1. 5 faces - triangle + top + bottom.
	// 2. Every x, y, z matches a bbox corner x,y,z coordate.
	// 3. Create a string index 7d ... space 6d zmins space 6d zmaxs. so string + 8 is z identification.
	// 4. If bboxes are SAME for any 2 triangles.  Merge them.
	ENT_DEFINE (d_ent, hm, entnum);
	CSGM_START_
	stringlist_t list_row_yz_min_maxes = {0};

	// STAGE 1: Identify Y and Z box levels and create a list of Y Low Z Low cubes to work.
	for_each_brush_or_patch (brushnum, d_ent, d_brush)
		// ONLY TRIANGULAR BRUSHES.
		if (BRUSH_IS_5_SIDED_TRI(d_brush) == false)	continue;

		// We want brushes with only a single flat top
		brushrow_s *flat_face = brush_get_face_facing_up_flat_only(d_brush);
		if (flat_face == NULL)
			continue; // Not a flat top

		// Texture name must be the same
		if (false == String_Match (flat_face->pbrtexture, texturename))
			continue;

		// Nov 13 2026 - mins and maxs seems to vary.
		// How can a brush mins and maxs vary so much.
		Vector3DeclareCopyEx (brmins, d_brush->bxmins); Vector_SnapToInteger_NoNegZero_If_003 (brmins);
		Vector3DeclareCopyEx (brmaxs, d_brush->bxmaxs); Vector_SnapToInteger_NoNegZero_If_003 (brmaxs);
		va_super (s, 256, "%g %g %g %g", brmins[1], brmaxs[1], brmins[2], brmaxs[2]);

		qbool added  = stringlistappend_unique_didAdd (&list_row_yz_min_maxes, s);
#if 0 // def _DEBUG		
		if (added) {
			if (in_range(63, list_row_yz_min_maxes.numstrings, 66)) {
				// ISSUE ONLY WHEN NEGATIVE?
				printvari (brushnum);
				printvarv (d_brush->bxmins);
				printvarv (d_brush->bxmaxs);
				int j = 5;
			}
		}
#endif // _DEBUG
	for_each_end // for_each_brush_or_patch

	// These are very different starting at 64
	zpr ("Dumping ent %d list_row_yz_min_maxes ...", entnum);
	stringlist_condump_with_linenums (&list_row_yz_min_maxes);

	// Work every list item.
	int hard_limit = d_ent->brushlist.count;
	for_each_stringlist_string (n, list_row_yz_min_maxes, sxy)
		vec4_t v4split = {0};
		Math_atov4 (sxy, v4split);
		vec3_t bbmins = { 0, v4split[0], v4split[2]};
		vec3_t bbmaxs = { 0, v4split[1], v4split[3]};

		int start_idx = 0;
		int after_idx = d_ent->brushlist.count;
		while (1) {
			// Now find a brush matching this one.
			int brush_idx1 = brush_find_yz_match(d_ent, bbmins, bbmaxs, start_idx, after_idx, texturename);

			//
			if (brush_idx1 == not_found_neg1)
				break; // NO MORE TO DO

			start_idx = brush_idx1 + 1; // For next iter
			brush_s *d_brush1 = &d_ent->brushlist.brush[brush_idx1];
			if (!d_brush1) {
				Con_PrintLinef ("Couldn't find brush yz matching mins: " VECTOR3_G " maxs: " VECTOR3_G,
					VECTOR3_SEND(bbmins),VECTOR3_SEND(bbmaxs) );
				continue; // Impossible, right?
			}

			// It might not be a flat one, check eligibility
			brushrow_s *flat_face = brush_get_face_facing_up_flat_only(d_brush1);

			if (!flat_face)
				continue; // NOT ELIGIBILE BECAUSE NOT FLAT

			int brush_idx2 = brush_find_triangular_with_xyz_match(d_ent, d_brush1->bxmins, d_brush1->bxmaxs, brush_idx1, hard_limit);

			// Our brush references are dangerous because page array!  On realloc, they fail.
			if (brush_idx2 == not_found_neg1) {
				// We don't always have a winner.
				continue;
			}

			// Kill both
			brush_s *d_brush2 = &d_ent->brushlist.brush[brush_idx2];
			d_brush1->is_considered_deleted = true;
			d_brush2->is_considered_deleted = true;

			Vector3DeclareCopyEx (bxmins, d_brush1->bxmins);
			Vector3DeclareCopyEx (bxmaxs, d_brush1->bxmaxs);

			////////////////int newidx = map_add_brush_cubeoid_with_texture (d_ent, /*d_brush1->*/bxmins, /*d_brush1->*/bxmaxs, COMMON_UP1);
			//int newidx = map_add_brush_cubeoid_with_texture (d_ent, /*d_brush1->*/bxmins, /*d_brush1->*/bxmaxs, texturename);

			////////////int newidx = map_add_brush_cubeoid_with_texture (d_ent, /*d_brush1->*/bxmins, /*d_brush1->*/bxmaxs, texturename);

			int newidx = brush_add_as_cubeoid_texturing_like_2_weld (hm, d_ent, brush_idx1, brush_idx2, bxmins, bxmaxs, FACE_ADJACENT_ANY_18);

			if (newidx != not_found_neg1) {
				//brush_s *n_brush = &d_ent->brushlist.brush[newidx];
				int entnum = ENT_GET_NUM (hm, d_ent);
				map_metric_brush (hm, entnum, newidx);
			}

			CSGM_NUM_DONE_PLUS_

		} // while 1 brush search loop
// END WHILE 1
	for_each_end // Each string

	stringlistfreecontents (&list_row_yz_min_maxes);


#if 0 // I think something here is broken
	// Now unite any xH1 == xL2  Y match Z match new brushes, making oldies as delete.
	for_each_entity_brush (brush_idx1, d_brush1, d_ent)
		if (d_brush1->brushrowlist.count != BRUSH_IS_6_SIDED(d_brush1))	continue;	// NOT CUBEOID - probably can't happen
		// Assume this brush makes sense.
		// Now find a brush next to it.  And repeat.

		vec3_t bxmins, bxmaxs;
		VectorCopyDestSrc (bxmins, d_brush1->bxmins);
		VectorCopyDestSrc (bxmaxs, d_brush1->bxmaxs);

		int start_idx = hard_limit + 1;
		int beyond_idx = d_ent->brushlist.count;
		int num_brush_combines = 0;
		int first_brush_num = not_found_neg1;
		while (1) {
			int brush_idx2 = brush_find_cuboid_with_xyz_rightof (d_ent, bxmins, bxmaxs, start_idx, beyond_idx, brush_idx1);
			if (brush_idx2 == not_found_neg1)
				break;

			start_idx = brush_idx2;

			// Need to expand.
			brush_s *d_brush2 = &d_ent->brushlist.brush[brush_idx2];
			d_brush1->is_considered_deleted = true;
			d_brush2->is_considered_deleted = true;
			bxmaxs[0] = d_brush2->bxmaxs[0]; // EXTEND

			if (first_brush_num == not_found_neg1)
				first_brush_num = brush_idx2;
			else {
				int j = 5;

			}
			num_brush_combines ++;

			zpr ("brushidx %d num_combines %d: Extended a rect: "
				"new bbox = " VECTOR3_G " maxs " VECTOR3_G,
				brush_idx1,
				num_brush_combines,
				VECTOR3_SEND(bxmins), VECTOR3_SEND(bxmaxs)
			);

			// RESET - do not presume an order.
			start_idx = hard_limit + 1;
		} // while.

		if (num_brush_combines) {
			//int brushnum0 = BRUSH_GET_NUM (hm, entnum, d_brush);
			int new_brush_idx =
				//map_add_brush_cubeoid_with_texture (d_ent, /*d_brush1->*/bxmins, /*d_brush1->*/bxmaxs, COMMON_CAULK);
				brush_add_as_cubeoid_texturing_like_2_weld (hm, d_ent, brush_idx1, first_brush_num, bxmins, bxmaxs, FACE_ADJACENT_ANY_18);

			//brush_s *n_brush_cube = &d_ent->brushlist.brush[newidx2];
			//brush_s *n_brush_cube = BRUSH_GET (hm, e  &d_ent->brushlist.brush[newidx2];
			BRUSH_DEFINE (n_brush_cube, hm, entnum, new_brush_idx);

			//int d_entnum = ENT_GET_NUM(hm, );// ARRAY_INDEX(d_ent, hm->entity);
			brush_metric (n_brush_cube);
			brushrow_s *flat_face = brush_get_face_facing_up_flat_only(n_brush_cube);
			if (flat_face) {
				Mem_FreeNull_ (flat_face->pbrtexture);
				flat_face->pbrtexture = Z_StrDup ("common/up2");
			}
		} // num_brush_combines
		// Delete brush
	for_each_end // for_each_brush_or_patch
#endif  //
	CSGM_END_FUNC_
}

int csgc_weld (map_t *hm)
{
	CSGM_START_
	stringlist_t list_textures = {0};

	Vid_SetWindowTitlef ("A. Identifying textures ...");
	for_each_entity_const_2 (entnum, r_ent, hm)
		stringlist_append_entity_textures (&list_textures, r_ent);
	for_each_end // Each string

	stringlistsort_unique (&list_textures);
	stringlist_condump_with_linenums (&list_textures);

	Vid_SetWindowTitlef ("B. Merging 2 triangles to a rect ...");

	for_each_stringlist_string (n, list_textures, this_texturename)
		for_each_entity_2 (entnum, d_ent, hm)
			//zpr ("Texture %d of %d for Entity #%d ...", n, list_textures.numstrings, entnum);
			int num_done_here = csgc_weld_entity_merge_2_square_triangles_to_rect_metric_new_brush (hm, entnum, this_texturename);

			if (num_done_here)
				CSGM_NUM_DONE_PLUS_EQ (num_done_here);
		for_each_end // for_each_entity
	for_each_end // Each string


	//Vid_SetWindowTitlef ("C. Reload");
	// The before and after varies (map reload vs. not) --- but it is just texture axis s and t
	// It could be the order that brushes or faces are worked somehow.
	// hard_limit does not access origin_brush_count
// BEFORE:
//( -640 928 -168 ) ( -640 928 -212 ) ( -736 928 -168 )  +0skullswithc [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0
//( -736 960 -168 ) ( -736 960 -212 ) ( -640 960 -168 )  +0skullswithc [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0
//( -736 960 -212 ) ( -736 928 -212 ) ( -640 960 -212 )  +0skullswithc [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 553648128 16512 0
//( -640 928 -168 ) ( -736 928 -168 ) ( -640 960 -168 )  +0skullswithc [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 553648128 16512 0
// AFTER:                                                                      DIFF          DIFF
// So the S and T texture axis varies but why?
//( -640 928 -168 ) ( -640 928 -212 ) ( -736 928 -168 )  +0skullswithc [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0
//( -736 960 -168 ) ( -736 960 -212 ) ( -640 960 -168 )  +0skullswithc [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0
//( -736 960 -212 ) ( -736 928 -212 ) ( -640 960 -212 )  +0skullswithc [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0
//( -640 928 -168 ) ( -736 928 -168 ) ( -640 960 -168 )  +0skullswithc [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 553648128 16512 0

	//map_reload (hm);
	//map_metric_reset

	Vid_SetWindowTitlef ("D. Merging rect rows");

	for_each_stringlist_string (n, list_textures, this_texturename)
		for_each_entity_2 (entnum, d_ent, hm)
			//zpr ("Texture %d of %d for Entity #%d ...", n, list_textures.numstrings, entnum);
			int num_done_here = map_merge_rect_rows_metric_new_brush (hm, entnum, this_texturename);

			if (num_done_here)
				CSGM_NUM_DONE_PLUS_EQ (num_done_here);
		for_each_end // for_each_entity
	for_each_end // Each string

	stringlistfreecontents (&list_textures);

	Vid_SetWindowTitle (gamename); // RESET
	CSGM_END_FUNC_
}





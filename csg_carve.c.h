// csg_carve.c.h

RELATED_ (map_carve_cubeoid_against_any_bulk)

#if 0
brush_s *GetFrontBrush (map_t *hm, int entnum, brush_s **p_front, int *p_front_num)
{
	brush_s *br = *p_front;
	if (br)
		return br;
	// MAKE ONE
	int new_brushnum = map_add_brush (hm, entnum);

	BRUSH_DEFINE (d_brush, hm, entnum, new_brushnum);
	(*p_front) = d_brush;
	(*p_front_num) = new_brushnum;
	return d_brush;
}
#endif

#if 0
#define EPSILON_0_01 0.01f
// --- True 3D Brush-by-Plane Clipper with Face Filtering ---
void SplitBrushByPlane(map_t *hm, int entnum, int brushnum,
   const brush_s *in_brush, const face_plane_t *carve_plane, int *p_front_num, int *p_back_num)
{
//    front->num_faces = 0;
//    back->num_faces = 0;
	brush_s *front_brush = NULL;

    // 1. Filter existing faces: evaluate each face relative to the splitting plane
	for_each_brush_face (facenum, in_brush, this_face)
    //for (int i = 0; i < in_brush->num_faces; i++) {
        //q3face_t face = in_brush->faces[i];

        // Test face points against the plane
        vec_t d0 = DotProduct(carve_plane->normal, this_face->a) - carve_plane->dist;
        vec_t d1 = DotProduct(carve_plane->normal, this_face->b) - carve_plane->dist;
        vec_t d2 = DotProduct(carve_plane->normal, this_face->c) - carve_plane->dist;

        bool is_front = (d0 >= -EPSILON_0_01 || d1 >= -EPSILON_0_01 || d2 >= -EPSILON_0_01);
        bool is_back  = (d0 <= EPSILON_0_01  || d1 <= EPSILON_0_01  || d2 <= EPSILON_0_01);

        // If face is primarily on the front side, keep it in the front brush
        if (is_front && !is_back) {
            //if (front->num_faces < MAX_FACES_PER_BRUSH) {
			GetFrontBrush (hm, entnum, &front_brush, p_front_num);
			brushrow_s *new_face = brush_add_face (front_brush);
			face_copy_data (new_face, this_face); // KEEP
			map_metric_brush (hm, entnum, *p_front_num);
        }
        // If face is primarily on the back side, keep it in the back brush
        else if (is_back && !is_front) {
			// We don't need back faces for carve.  FUTURE - Split brush?
			if (p_back_num) {
				//if (back->num_faces < MAX_FACES_PER_BRUSH) {
					//back->faces[back->num_faces++] = face;
				//}
			}
        }
        // If the face spans the plane, it gets clipped/split into both fragments
        else {
			GetFrontBrush (hm, entnum, &front_brush, p_front_num);
			brushrow_s *new_face = brush_add_face (front_brush);
			face_copy_data (new_face, this_face); // KEEP
			map_metric_brush (hm, entnum, *p_front_num);
            //if (front->num_faces < MAX_FACES_PER_BRUSH) {
            //    front->faces[front->num_faces++] = face;
            //}
			if (p_back_num) {
				//if (back->num_faces < MAX_FACES_PER_BRUSH) {
					//back->faces[back->num_faces++] = face;
				//}
			}
        }
    for_each_end // for_each_brush_face

    // 2. Add the new cutting plane as a face to close off the front fragment

    //if (front->num_faces < MAX_FACES_PER_BRUSH) {
	if (1) {
		brushrow_s *new_face = brush_add_face (front_brush);
		new_face->face_plane = *carve_plane;
		face_texture_set (new_face, COMMON_CAULK);

        //q3face_t *f = &front->faces[front->num_faces++]; // ADDING FACE
        //f->plane = *plane;
        //strncpy(f->shader, "common/caulk", sizeof(f->shader)-1);
        //PointsFromPlane(&f->face_plane, f->p0, f->p1, f->p2);
		PointsFromPlane(&new_face->face_plane, new_face->a, new_face->b, new_face->c);
		map_metric_brush (hm, entnum, *p_front_num);
		RELATED_ (brush_fill_face_points)
		//brush_and_face_metric (front_brush);
		// Baker: Texture matrix?  Sort of doesn't matter that much for caulk.
    }

    // 3. Add the inverted cutting plane to close off the back fragment
    face_plane_t inv_plane = *carve_plane;
	PlaneReverse (&inv_plane);
    //inv_plane.normal[0] = -plane->normal[0];
    //inv_plane.normal[1] = -plane->normal[1];
    //inv_plane.normal[2] = -plane->normal[2];
    //inv_plane.dist = -plane->dist;

    //if (back->num_faces < MAX_FACES_PER_BRUSH) {
	if (p_back_num) {
		//brushrow_s *new_face = brush_add_face (front_brush);
  //      q3face_t *f = &back->faces[back->num_faces++];
  //      f->plane = inv_plane;
  //      strncpy(f->shader, "common/caulk", sizeof(f->shader)-1);
  //      PointsFromPlane(&f->plane, f->p0, f->p1, f->p2);
		// map_brush_and_face_metric (new_face);
		//map_brush_and_face_metric (bavk_brush);
    //}
	}
}
#endif // 0

#if 0
#define IsBrushValidFaces4Plus(BR) (BR->brushrowlist.count >= 4)
//bool IsBrushValid(const q3brush_t *brush) {
//    if (brush->num_faces < BRUSH_MIN_FACES_4) return false;
//    return true;
//}

#define NO_BACKNUM_NULL NULL

int map_carve_brush_v_brush (map_t *hm, int entnum, int brushnum, const brush_s *cutter_brush)
{
	// map_carve_super says there is box overlap so result is somewhat likely.
	// But not guaranteed.
	int num_done = 0;

	//ENT_DEFINE (results_world, &hm_carve_results, WORLD_0);

	// Add brush to our world.
	//BRUSH_DEFINE_CONST (target_brush, hm, entnum, brushnum);
	// brush_add_copying_brush_or_patch (results_world, target_brush);

    // Iterate through each face of the cuboid cutter
	// The working group changes each time.
	//
	int32list_s brush_ilist = {0}, *plist_brushset = &brush_ilist;
	int32s_add1 (plist_brushset, brushnum);
	static int iters = 0;
	int numfronts =0;
	int numdels =0;
	for_each_brush_face (facenum, cutter_brush, cutter_face) // FOR EACH FACE OF CUTTER
        face_plane_t mcutter_plane = cutter_face->face_plane;

        // Invert plane to point outward (carving away the interior)
        face_plane_t mcarve_plane = mcutter_plane;
		PlaneReverse (&mcarve_plane);

		int32list_s brush_ilist2 = {0}, *plist_newset = &brush_ilist2;
work_our_group:
		// As each face cuts, brushes are created.
		for_each_int32listp (n, plist_brushset, sxy_brushnum)
			iters++;
			BRUSH_DEFINE (this_brush, hm, entnum, sxy_brushnum);

			//int front_num = map_add_brush (&hm_carve_results, WORLD_0);
			//int back_num = map_add_brush (&hm_carve_results, WORLD_0);
			//
			//BRUSH_DEFINE (back_brush, &hm_carve_results, WORLD_0, brush, back_num);

			int front_num = not_found_neg1;

			// What happens if this does nothing?
            SplitBrushByPlane (hm, entnum, sxy_brushnum, this_brush, &mcarve_plane, &front_num, NO_BACKNUM_NULL);

			if (front_num != not_found_neg1) {
				BRUSH_DEFINE (front_brush, hm, WORLD_0, front_num);
				// Keep exterior fragments ('front') for the next cut plane
				if (IsBrushValidFaces4Plus(front_brush)) {
					//next_set[next_count++] = front; // ADDING BRUSH
	                int32s_add1 (plist_newset, front_num);
					numfronts ++;
				} else {
					front_brush->is_considered_deleted = true; // Invalid
					numdels ++;
				}
			}
			// Interior fragments ('back') are dropped/deleted here

        for_each_end // for_each_brush_face

		// REPLACE THE BRUSHES WITH THE NEW ONES
		int32s_freecontents (plist_brushset);
        for_each_int32listp (n, plist_newset, sxy_brushnum)
			int32s_add1 (plist_brushset, sxy_brushnum);
        for_each_end // for_each_brush_face
        if (plist_newset->count == 0) break;

	for_each_end // for_each_brush_face

	int32s_freecontents (plist_brushset);
	debugprintvari (numfronts);
	debugprintvari (numdels);
	debugprintvari (iters);
	return num_done;
}
#endif

#if 0 // This was a failure
RELATED_ (map_carve_cubeoid)
CALLERS_ (csgax_includes)
int map_carve_super (map_t *hm, const brush_s *cutter_brush_in)
{
	int num_made = 0;
	const brush_s cutter_brush_copy = (*cutter_brush_in);
	const brush_s *r_cutter_brush = &cutter_brush_copy;

	for_each_entity_2 (d_entnum, r_ent, hm)
		// Yes this will work the new brushes.  It will probably be fine.
		for_each_brush_original_2 (d_brushnum_target, d_brush_target, hm, d_entnum)
			debugprintvari (d_brushnum_target);
			Vector3DeclareCopyEx (brmins, d_brush_target->bxmins);
			Vector3DeclareCopyEx (brmaxs, d_brush_target->bxmaxs);
			Vector3DeclareCopyEx (brsize, d_brush_target->bxdims);

			qbool collide = Boxes_Collide_Epsilon_3D(brmins, brmaxs, r_cutter_brush->bxmins, r_cutter_brush->bxmaxs);

			if (collide == false) {
				continue;
			}

			d_brush_target->is_considered_deleted = true;
			//int num_made_here = map_carve_cubeoid_against_any (hm, d_entnum, d_brushnum_target, &r_carve_brush_copy);

			DebugPrintLinef ("Cutter vs. ent %d brush %d", d_entnum, d_brushnum_target);
			int num_made_here = map_carve_brush_v_brush (hm, d_entnum, d_brushnum_target, r_cutter_brush);
			//num_made += num_made_here;
			num_made ++;
			//goto fail_out_wtf;
		for_each_end // for_each_brush_or_patch
	for_each_end // after_world


//fail_out_wtf:
	return num_made;

}
#endif // 0


// Only comes here if terrain wedge
void brush_terrain_wedge_split (map_t *hm, int entnum, int brushnum)
{
 //   ENT_DEFINE (ent, hm, entnum);

    int brushnum_upper = map_add_brush (hm, entnum);
    int brushnum_lower = map_add_brush (hm, entnum);

    BRUSH_DEFINE (original, hm, entnum, brushnum);
	original->is_considered_deleted = true;

    int face_index_top = original->top_slanted_index;
    int face_index_bottom = original->cubeoid_index[FACE_FLOOR_4];

    FACE_DEFINE (top_face, hm, entnum, brushnum, face_index_top);

    float split_z = top_face->bfmins[2];

    float xmin = top_face->bfmins[0];
    float xmax = top_face->bfmaxs[0];
    float ymin = top_face->bfmins[1];
    float ymax = top_face->bfmaxs[1];

    /*
     * UPPER BRUSH
     *
     * Copy all faces except the original floor.
     */

    BRUSH_DEFINE (upper, hm, entnum, brushnum_upper);

    for_each_brush_face_2 (n, old_face, hm, entnum, brushnum)

        brushrow_s *new_face = brush_add_face (upper);
        face_copy_data (new_face, old_face);

		if (n == face_index_bottom) {
			//VectorSet (new_face->a, xmin, ymin, split_z);
			//VectorSet (new_face->b, xmin, ymax, split_z);
			//VectorSet (new_face->c, xmax, ymin, split_z);
VectorSet (new_face->a, xmin, ymin, split_z);
VectorSet (new_face->b, xmax, ymin, split_z);
VectorSet (new_face->c, xmin, ymax, split_z);
			face_texture_set (new_face, COMMON_CAULK);
		}

		int content_flags = new_face->faceparms[FT_SURFACEFLAGS_3];
		Flag_Add_To (content_flags, CONTENTS_DETAIL);
		Flag_Remove_From (content_flags, CONTENTS_STRUCTURAL);
		new_face->faceparms[FT_SURFACEFLAGS_3] = content_flags;

    for_each_end // brush face

    /*
     * LOWER BRUSH
     *
     * Copy all original faces.
     * Replace the sloped top with a horizontal top.
     */

    BRUSH_DEFINE (lower, hm, entnum, brushnum_lower);

    for_each_brush_face_2 (n, old_face, hm, entnum, brushnum)
        brushrow_s *new_face = brush_add_face (lower);
        face_copy_data (new_face, old_face);

		// This changes the top of the lower brush to a flat one
        if (n == face_index_top) {
            //VectorSet (new_face->a, xmin, ymin, split_z);
            //VectorSet (new_face->b, xmax, ymin, split_z);
            //VectorSet (new_face->c, xmin, ymax, split_z);
VectorSet (new_face->a, xmin, ymin, split_z);
VectorSet (new_face->b, xmin, ymax, split_z);
VectorSet (new_face->c, xmax, ymin, split_z);
			face_texture_set (new_face, COMMON_CAULK);
        }
    for_each_end // brush face

	map_metric_brush_maybe_badfaces(hm, entnum, brushnum_upper, /*strip bad faces*/ false);
	//map_metric_brush (hm, entnum, brushnum_upper);
	map_metric_brush_cubeoid (hm, entnum, brushnum_upper);
	map_metric_brush_maybe_badfaces(hm, entnum, brushnum_lower, /*strip bad faces*/ false);
	//map_metric_brush (hm, entnum, brushnum_lower);
	// TEXSIDE_SOUTHWEST_58 or TEXSIDE_NORTHWEST_57
	map_metric_brush_cubeoid (hm, entnum, brushnum_lower); // It makes 58 and 57
}

// Merge two complementary flat triangle brushes into a cubeoid.
// Returns the new brush number, or -1 if they cannot be merged.
// Nov 10 2026 - I use a different method than this.
RELATED_ (csgc_detail_wedge_split_auto)
static int brush_terrain_triangles_merge_used (map_t *hm, int entnum, int brushnum_a, int brushnum_b)
{
 //   ENT_DEFINE (ent, hm, entnum);

    BRUSH_DEFINE (brush_a, hm, entnum, brushnum_a);
    BRUSH_DEFINE (brush_b, hm, entnum, brushnum_b);

    brush_a->is_considered_deleted = true;
    brush_b->is_considered_deleted = true;

    // Save the cardinal face indexes before adding another brush.
    int face_index_a[FACE_MAX_6];
    int face_index_b[FACE_MAX_6];

    for (int n = 0; n < FACE_MAX_6; n++) {
        face_index_a[n] = brush_a->cubeoid_index[n];
        face_index_b[n] = brush_b->cubeoid_index[n];
    }

    // Determine which brush supplies each of the six cubeoid faces.
    int face_source[FACE_MAX_6];

    for (int n = 0; n < FACE_MAX_6; n++) {
        face_source[n] = -1;

        if (face_index_a[n] != FACE_INVALID_NEG_1)
            face_source[n] = brushnum_a;
        else if (face_index_b[n] != FACE_INVALID_NEG_1)
            face_source[n] = brushnum_b;

        // Both triangles together must provide all six faces.
        if (face_source[n] == -1)
            return -1;
    }

    // Create the merged brush.
    int brushnum_merged = map_add_brush (hm, entnum);

    BRUSH_DEFINE (merged, hm, entnum, brushnum_merged);

    // Copy the six cardinal faces, excluding the diagonal faces.
    for (int n = 0; n < FACE_MAX_6; n++) {
        int face_index;

        if (face_source[n] == brushnum_a) {
            face_index = face_index_a[n];

            FACE_DEFINE (source_face, hm, entnum, brushnum_a, face_index);

            brushrow_s *new_face = brush_add_face (merged);
            face_copy_data (new_face, source_face);
        }
        else {
            face_index = face_index_b[n];

            FACE_DEFINE (source_face, hm, entnum, brushnum_b, face_index);

            brushrow_s *new_face = brush_add_face (merged);
            face_copy_data (new_face, source_face);
        }
    }

	// THE FACE CHECKER HATES SOMETHING AND SAYS BAD BRUSH
//	int facesc = merged->brushrowlist.count;
//	if (BRUSH_IS_6_SIDED(merged) == false) {
//		int j = 5;
//	}

	map_metric_brush (hm, entnum, brushnum_merged);
//	int facesc1 = merged->brushrowlist.count;
//	if (facesc1 != facesc) {
//		int j = 5;
//	}
	map_metric_brush_cubeoid (hm, entnum, brushnum_merged);
//	int facesc2 = merged->brushrowlist.count;
//	if (facesc2 != facesc1) {
//		int j = 5;
//	}

    return brushnum_merged;
}


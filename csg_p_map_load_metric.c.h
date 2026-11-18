// csg_p_map_load_metric.c.h

RELATED_ (map_metric_brush)

// Epsilon version
//void face_plane_t_calc(brushrow_s *d_face)
//{
//    vec3_t v1, v2;
//    vec3_t normal;
//
//    VectorSubtract(d_face->b, d_face->a, v1);
//    VectorSubtract(d_face->c, d_face->a, v2);
//
//    CrossProduct(v1, v2, normal);
//
//    // Detect axis-aligned planes.
//    #define FACE_AXIS_EPSILON 0.001f
//
//    if (fabsf(d_face->a[0] - d_face->b[0]) <= FACE_AXIS_EPSILON &&
//        fabsf(d_face->a[0] - d_face->c[0]) <= FACE_AXIS_EPSILON)
//    {
//        // X plane.
//        d_face->face_plane.normal[0] = (normal[0] < 0.0f) ? -1.0f : 1.0f;
//        d_face->face_plane.normal[1] = 0.0f;
//        d_face->face_plane.normal[2] = 0.0f;
//    }
//    else if (fabsf(d_face->a[1] - d_face->b[1]) <= FACE_AXIS_EPSILON &&
//             fabsf(d_face->a[1] - d_face->c[1]) <= FACE_AXIS_EPSILON)
//    {
//        // Y plane.
//        d_face->face_plane.normal[0] = 0.0f;
//        d_face->face_plane.normal[1] = (normal[1] < 0.0f) ? -1.0f : 1.0f;
//        d_face->face_plane.normal[2] = 0.0f;
//    }
//    else if (fabsf(d_face->a[2] - d_face->b[2]) <= FACE_AXIS_EPSILON &&
//             fabsf(d_face->a[2] - d_face->c[2]) <= FACE_AXIS_EPSILON)
//    {
//        // Z plane.
//        d_face->face_plane.normal[0] = 0.0f;
//        d_face->face_plane.normal[1] = 0.0f;
//        d_face->face_plane.normal[2] = (normal[2] < 0.0f) ? -1.0f : 1.0f;
//    }
//    else
//    {
//        // General plane.
//        VectorCopy(normal, d_face->face_plane.normal);
//        VectorNormalize(d_face->face_plane.normal);
//    }
//
//    d_face->face_plane.dist = DotProduct(
//        d_face->a, d_face->face_plane.normal);
//
//    d_face->is_face_plane_set = true;
//}

void face_plane_t_calc(brushrow_s *d_face)
{
    vec3_t v1, v2;
    vec3_t normal;

#ifdef _DEBUG
	qbool is_flat = d_face->a[2] == d_face->b[2] && d_face->b[2] == d_face->c[2];
	qbool is_caulk = FACE_IS_CAULK(d_face); // DEBUG CHECK

	if (is_flat && !is_caulk) {
		int j = 5;
	}
#endif

    VectorSubtract(d_face->b, d_face->a, v1);
    VectorSubtract(d_face->c, d_face->a, v2);

    CrossProduct(v1, v2, normal);

    // Detect axis-aligned planes.
    if (d_face->a[0] == d_face->b[0] &&
        d_face->a[0] == d_face->c[0])
    {
        // X plane.
        d_face->face_plane.normal[0] = (normal[0] < 0.0f) ? -1.0f : 1.0f;
        d_face->face_plane.normal[1] = 0.0f;
        d_face->face_plane.normal[2] = 0.0f;
    }
    else if (d_face->a[1] == d_face->b[1] &&
             d_face->a[1] == d_face->c[1])
    {
        // Y plane.
        d_face->face_plane.normal[0] = 0.0f;
        d_face->face_plane.normal[1] = (normal[1] < 0.0f) ? -1.0f : 1.0f;
        d_face->face_plane.normal[2] = 0.0f;
    }
    else if (d_face->a[2] == d_face->b[2] &&
             d_face->a[2] == d_face->c[2])
    {
        // Z plane.
        d_face->face_plane.normal[0] = 0.0f;
        d_face->face_plane.normal[1] = 0.0f;
        d_face->face_plane.normal[2] = (normal[2] < 0.0f) ? -1.0f : 1.0f;
    }
    else
    {
        // General plane.
        VectorCopy(normal, d_face->face_plane.normal);
        VectorNormalize(d_face->face_plane.normal);
    }

    d_face->face_plane.dist = DotProduct(d_face->a, d_face->face_plane.normal);
    d_face->is_face_plane_set = true;
}

void face_plane_t_calc_trig (brushrow_s *d_face)
{
    vec3_t v1, v2;

    VectorSubtract(d_face->b, d_face->a, v1);
    VectorSubtract(d_face->c, d_face->a, v2);

    CrossProduct(v1, v2, d_face->face_plane.normal);
	//CrossProduct(v2, v1, d_face->face_plane.normal); // Seems to be the winding we need -- NO just negate it otherwise could mess with carve
    VectorNormalize(d_face->face_plane.normal);

    d_face->face_plane.dist = DotProduct(d_face->a, d_face->face_plane.normal);
	d_face->is_face_plane_set = true;
}

// WARNING --- MAYBE NO CALLERS?
RELATED_ (map_metric_entity_sub) // ACTUALLY USED.

void map_metric_brush_maybe_badfaces(map_t *hm, int entnum, int brushnum, qbool do_badfaces)
{
	//ENT_DEFINE (d_ent, hm, entnum);
	BRUSH_DEFINE (d_brush, hm, entnum, brushnum);

	bounds_init_9 (d_brush->bxmins, d_brush->bxmaxs);

	if (d_brush->is_a_patch) {
		// PATCH - Translate all vertexes
		for_each_patch_row (rownum, d_brush, d_patchrow)
			// FIGURING BBOX OF PATCH
			bounds_init_9 (d_patchrow->bpmins,  d_patchrow->bpmaxs);
			for_each_patch_column (columnnum, d_brush, d_patchrow)
				float *pvec = &d_patchrow->flots[columnnum * 5 + 0];
				bounds_expand	(pvec, d_patchrow->bpmins, d_patchrow->bpmaxs);

				// FINALIZE FACE DIMENSIONS
				VectorSubtract	(d_patchrow->bpmaxs, d_patchrow->bpmins, d_patchrow->bpdims);
				//VectorCenter	(d_patchrow->bpmins, d_patchrow->bpmaxs, d_patchrow->bpcenter);

				// EXPANDING BRUSH METRIX
				bounds_expand	(d_patchrow->bpmins, d_brush->bxmins, d_brush->bxmaxs);
				bounds_expand	(d_patchrow->bpmaxs, d_brush->bxmins, d_brush->bxmaxs);
			for_each_end // patch_column
		for_each_end // patch_row

		goto brush_finalize_because_i_am_patch;
	} // IF PATCH

	// REAL BRUSH

	RELATED_ (map_metric_brush)
	if (d_brush->is_a_patch == false) {
		RELATED_ (_cat_brush ) // SAVE BRUSH
		qbool is_bad = brush_fill_face_points_needs_rebuilt (d_brush);

		if (is_bad && do_badfaces) {
			brush_rebuild_fill_face_points (hm, entnum, brushnum);
			/*qbool is_bad2 =*/ brush_fill_face_points_needs_rebuilt (d_brush);
		}
	}

	for_each_brush_face_even_deleted_2 (facenum, d_face, hm, entnum, brushnum)
#ifdef _DEBUG
		int magic = entnum + brushnum + facenum;
#endif // _DEBUG

		if (d_face->is_considered_deleted)
			continue;

		// FIGURING BBOX OF ACTUAL FACE
		bounds_init_9 (d_face->bfmins, d_face->bfmaxs);

        // Loop through all actual winding points of the face polygon
        for (int i = 0; i < d_face->num_points; i++) {
            bounds_expand(d_face->points[i], d_face->bfmins, d_face->bfmaxs);
        }

		// NO FOOL!  You cannot use face points they represent an infinite plane
		//bounds_expand	(d_face->a, d_face->bfmins, d_face->bfmaxs);
		//bounds_expand	(d_face->b, d_face->bfmins, d_face->bfmaxs);
		//bounds_expand	(d_face->c, d_face->bfmins, d_face->bfmaxs);

		// FIX STUPID NUMBERS
		if (_csg_snaptointeger.integer) {
			Vector_SnapToInteger_If_003 (d_face->bfmaxs);
			Vector_SnapToInteger_If_003	(d_face->bfmins);
		}



		// FINALIZE FACE DIMENSIONS
		VectorSubtract	(d_face->bfmaxs, d_face->bfmins, d_face->bfdims);

		// EXPANDING BRUSH METRIX
		bounds_expand	(d_face->bfmins, d_brush->bxmins, d_brush->bxmaxs);
		bounds_expand	(d_face->bfmaxs, d_brush->bxmins, d_brush->bxmaxs);
	for_each_end // brush row

brush_finalize_because_i_am_patch:
	// FINALIZE BRUSH DIMENSIONS
	VectorSubtract	(d_brush->bxmaxs, d_brush->bxmins, d_brush->bxdims);
	VectorCenter	(d_brush->bxmaxs, d_brush->bxmins, d_brush->bxcenter);
}

// October 23, 2026 - this is now used!
void map_metric_brush(map_t *hm, int entnum, int brushnum)
{
	map_metric_brush_maybe_badfaces (hm, entnum, brushnum, /*kill bad faces*/ true);
}

CALLERS_ (map_metric_again)
RELATED_ (map_move)
qbool map_metric_entity_sub (map_t *hm, int entnum) // EVEN WORLD COMES HERE
{
	qbool did_find_any_ent_brushes = false;

	ENT_DEFINE (d_ent, hm, entnum);
	// Baker: September 2 2026 - Weakness: non-func_group entities.  Those technically shouldn't count.
	// Baker: September 13 2026 - I think that is correct.
	// Baker: October 20 2026 - I believe this is all fine.
	if (!d_ent->brushlist.count)
		goto no_brushes;

	bounds_init_9 (d_ent->bemins, d_ent->bemaxs);

	// How does an origin brush impact this?  It doesn't.  Right?
	for_each_brush_or_patch_2 (brushnum, d_brush, hm, entnum)
		if (d_brush->brushrowlist.count == 0 && d_brush->patchrowlist.count == 0) {
			//int j = 5;
			continue; // Can this happen?  Should be impossible.
		}

		if (did_find_any_ent_brushes == false)
			did_find_any_ent_brushes = true;

		map_metric_brush (hm, entnum, brushnum);
		if (1) {
			bounds_expand	(d_brush->bxmins, d_ent->bemins, d_ent->bemaxs);
			bounds_expand	(d_brush->bxmaxs, d_ent->bemins, d_ent->bemaxs);
		}
	for_each_end // brush or patch

	if (did_find_any_ent_brushes) {
		VectorSubtract		(d_ent->bemaxs, d_ent->bemins, d_ent->bedims);
		VectorCenter		(d_ent->bemaxs, d_ent->bemins, d_ent->becenter);
		d_ent->has_brushes = true;
	} else {
no_brushes:
		// ZERO
		VectorClear (d_ent->bemins);
		VectorClear (d_ent->bemaxs);
		VectorClear (d_ent->bedims);
		VectorClear (d_ent->becenter);
		d_ent->has_brushes = false;
	}
	return did_find_any_ent_brushes;
}

CALLERS_ (map_metric_again)
static void map_metric_again_reset (map_t *hm)
{
	VectorClear	(hm->bbentsmins);
	VectorClear	(hm->bbentsmaxs);
	VectorClear	(hm->bbentsdims);

	for_each_entity_2 (entnum, d_ent, hm)
		VectorClear (d_ent->bemins);
		VectorClear (d_ent->bemaxs);
		VectorClear (d_ent->bedims);
		d_ent->has_brushes = false;

		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			VectorClear (d_brush->bxmins);
			VectorClear (d_brush->bxmaxs);
			VectorClear (d_brush->bxdims);
			VectorClear (d_brush->bxcenter);

			for_each_brush_face_2 (facenum, d_face, hm, entnum, brushnum)
				VectorClear (d_face->bfmins);
				VectorClear (d_face->bfmaxs);
				VectorClear (d_face->bfdims);
			for_each_end // face
		for_each_end // brush

		for_each_patch2 (brushnum, d_patch, hm, entnum)
			VectorClear (d_patch->bxmins);
			VectorClear (d_patch->bxmaxs);
			VectorClear (d_patch->bxdims);
			VectorClear (d_patch->bxcenter);
			for_each_patch_row (rownum, d_patch, d_patchrow)
				VectorClear (d_patchrow->bpmins);
				VectorClear (d_patchrow->bpmaxs);
				//for_each_patch_column (columnnum, d_brush, d_patchrow)
				//for_each_end // patch_column
			for_each_end // patch_row

		for_each_end // for_each_patch2

	for_each_end // entity
}

CALLERS_ (map_load_from_string)
void map_metric_all (map_t *hm)
{
	int num_processed = 0;

	ENT_DEFINE (d_world, hm, WORLD_0);

	qbool was_processed = map_metric_entity_sub (hm, WORLD_0); // Returns false if no brushes.
	d_world->is_world_or_func_group = true;

	// We could be dealing with something weird like all func groups.
	if (was_processed)
		num_processed ++;

	Vector3DeclareCopyEx (bbemins, d_world->bemins);
	Vector3DeclareCopyEx (bbemaxs, d_world->bemaxs);

	qbool did_any_ents = false;
	//for_each_entity_after_world_undeleted (entnum, plist, d_ent)
	for_each_entity_afterworld_2 (entnum, d_ent, hm)
		ccs *classname = map_get_classname (hm, entnum);
		if (String_Starts_With_Nullproof(classname, "func_group"))
			d_ent->is_world_or_func_group = true;

		qbool was_processed2 = map_metric_entity_sub (hm, entnum);
		if (!was_processed2)
			continue;

		// EXPANDING ENTITY METRIC
		bounds_expand	(d_ent->bemins, bbemins, bbemaxs); // Yes, bbemaxs is what expands
		bounds_expand	(d_ent->bemaxs, bbemins, bbemaxs);

		did_any_ents = true;
		num_processed ++;
	for_each_end // for_each_entity  entities that are "func_wall"

	if (1 /*did_any_ents*/) {
		// Could be all world and point entities -- no func_group and no walls/doors/etc.
		VectorCopyDestSrc	(hm->bbentsmins, bbemins);
		VectorCopyDestSrc	(hm->bbentsmaxs, bbemaxs);
		VectorSubtract		(hm->bbentsmaxs, hm->bbentsmins, hm->bbentsdims); // Size
		// Entitites do not have "center", only brushes
	} // if 1
}



RELATED_ (map_metric_cubeoid_mark_FULL_METRIC_AVAIL map_metric_all map_metric_reset)
void map_metric_again (map_t *hm)
{
	RELATED_ (_entity_s is_world_or_func_group)
	map_metric_again_reset	(hm);
	map_metric_all			(hm);
//	map_metric_cubeoid_mark_FULL_METRIC_AVAIL (hm);
}


//void brush_and_face_metric (brush_s *d_brush)
//{
//	bounds_init_9 (d_brush->bxmins, d_brush->bxmaxs);
//
//	// REAL BRUSH
//	for_each_brush_face (facenum, d_brush, d_face)
//		// FIGURING BBOX OF ACTUAL FACE
//		bounds_init_9 (d_face->bfmins, d_face->bfmaxs);
//
//		bounds_expand	(d_face->a, d_face->bfmins, d_face->bfmaxs);
//		bounds_expand	(d_face->b, d_face->bfmins, d_face->bfmaxs);
//		bounds_expand	(d_face->c, d_face->bfmins, d_face->bfmaxs);
//
//		// FINALIZE FACE DIMENSIONS
//		VectorSubtract	(d_face->bfmaxs, d_face->bfmins, d_face->bfdims);
//
//		// EXPANDING BRUSH METRIX
//		bounds_expand	(d_face->bfmins, d_brush->bxmins, d_brush->bxmaxs);
//		bounds_expand	(d_face->bfmaxs, d_brush->bxmins, d_brush->bxmaxs);
//	for_each_end // brush row
//
//	// FINALIZE BRUSH DIMENSIONS
//	VectorSubtract		(d_brush->bxmaxs, d_brush->bxmins, d_brush->bxdims);
//}

//void map_brush_and_face_metric (map_t *hm, int entnum, int brushnum)
//{
//	BRUSH_DEFINE (d_brush, hm, entnum, brushnum);
//	brush_and_face_metric (d_brush);
//}

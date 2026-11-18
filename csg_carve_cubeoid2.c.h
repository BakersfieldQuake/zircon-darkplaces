// csg_carve_cubeoid2.c.h


// Cubeoid vs. everything
// Carve a cubeoid out of a convex target brush.
// Returns the number of new brushes created.
// The original target brush is NOT deleted here.
// Baker: Do not call if no box collision or brushes not qualified.
// Baker: Caller's responsibility to delete target
#define ONE_EXTRA_PLANE		1
#define THREE_EXTRA_PLANES	3
#define FIVE_EXTRA_PLANES	5
// Baker: This function shall only be called if there is a cubeoid collision
// and there are brushes.
// This appears be 2 brushes (hm + entnum + brushnum) is carve target
// carve_brush is a copy of the brush.
// (I wrote this a month ago and forgot the internals)..
// Caller should be delete.
// This should NOT be called if there no overlap



// Find the intersection point of three planes.
//
// Solves:
//     n1 dot p = d1
//     n2 dot p = d2
//     n3 dot p = d3
// Returns false when the planes do not have
// a unique intersection.

qbool Carve_Three_Planes_Intersection (const face_plane_t *p1, const face_plane_t *p2, const face_plane_t *p3,
    vec3_t result)
{

    float a1 = p1->normal[0];
    float b1 = p1->normal[1];
    float c1 = p1->normal[2];

    float a2 = p2->normal[0];
    float b2 = p2->normal[1];
    float c2 = p2->normal[2];

    float a3 = p3->normal[0];
    float b3 = p3->normal[1];
    float c3 = p3->normal[2];

    float d1 = p1->dist;
    float d2 = p2->dist;
    float d3 = p3->dist;

	// Baker: a non-zero determinant guarantees that the three plane normals are linearly independent,
	// meaning the planes intersect at a single, unique 3D point.

    float det =	a1 * (b2 * c3 - c2 * b3) -
				b1 * (a2 * c3 - c2 * a3) +
				c1 * (a2 * b3 - b2 * a3);

	// Baker: "epsilon" check
    if (det > -0.0001 && det < 0.0001)
        return false;

	// Baker: A valid intersection point exists, now calculate the points.

    float detx =	d1 * (b2 * c3 - c2 * b3) -
					b1 * (d2 * c3 - c2 * d3) +
					c1 * (d2 * b3 - b2 * d3);

    float dety =	a1 * (d2 * c3 - c2 * d3) -
					d1 * (a2 * c3 - c2 * a3) +
					c1 * (a2 * d3 - d2 * a3);

    float detz =	a1 * (b2 * d3 - d2 * b3) -
					b1 * (a2 * d3 - d2 * a3) +
					d1 * (a2 * b3 - b2 * a3);

    result[0] = detx / det;	// X
    result[1] = dety / det;	// Y
    result[2] = detz / det;	// Z

    return true;
}




RELATED_ (PlaneDiff)


// Get one plane from the combined plane set.
// The first target_count planes come directly from the target brush faces.
// The remaining planes come from extra_planes.

void Carve_Get_Plane (map_t *hm, int entnum, int brushnum, int brush_num_faces,
   const face_plane_t *cubeoid_plane_set, int plane_index, face_plane_t *plane)
{
    if (plane_index < brush_num_faces) {
        //brushrow_s *face = &target_brush->brushrowlist.brushrow[plane_index];
		FACE_DEFINE (target_face, hm, entnum, brushnum, /*facenum =*/ plane_index);
        (*plane) = target_face->face_plane; // Baker: struct copy
		return;
    }

	int cubeoid_plane_index = plane_index - brush_num_faces;
	(*plane) = cubeoid_plane_set[cubeoid_plane_index];  // Baker: struct copy
}


// Test whether the combined target brush and cubeoid planes
// enclose a real 3D volume.
// The target brush faces are read directly from their cached face_plane values.
qbool Carve_Planes_Have_Volume (map_t *hm, int entnum, int brushnum, const face_plane_t *cubeoid_plane_set,
								int cubeoid_plane_set_count)
{
    face_plane_t p1;
    face_plane_t p2;
    face_plane_t p3;

	//BRUSH_DEFINE_CONST (target_brush, hm, entnum, brushnum);
    const int brush_num_faces = FACE_COUNT(hm, entnum, brushnum);
	//int num_valid_faces = 0;

    int numplanes = brush_num_faces + cubeoid_plane_set_count;

	// Baker: At least 4 planes are required to enclose a 3D volume.
	// Four planes can form a tetrahedron (triangular pyramid).
	if (numplanes < BRUSH_MIN_FACES_4)
        return false;

	// Opposing coincident planes leave no 3D volume.
	for (int i = 0; i < numplanes - 1; i++) {
		face_plane_t p1;
		Carve_Get_Plane(hm, entnum, brushnum,
			brush_num_faces, cubeoid_plane_set, i, &p1);

		for (int j = i + 1; j < numplanes; j++) {
			face_plane_t p2;
			Carve_Get_Plane(hm, entnum, brushnum,
				brush_num_faces, cubeoid_plane_set, j, &p2);

			float normal_dot =
				p1.normal[0] * p2.normal[0] +
				p1.normal[1] * p2.normal[1] +
				p1.normal[2] * p2.normal[2];

			if (normal_dot < -0.9999f &&
				fabsf(p1.dist + p2.dist) < CSG_EPSILON_0_01)
				return false;
		}
	}

	// Test every combination of three planes.
    for (int i = 0; i < numplanes - 2; i++) {
        Carve_Get_Plane (hm, entnum, brushnum, brush_num_faces, cubeoid_plane_set, i, &p1);

        for (int j = i + 1; j < numplanes - 1; j++) {
			vec3_t point;
            Carve_Get_Plane (hm, entnum, brushnum, brush_num_faces, cubeoid_plane_set, j, &p2);

            for (int k = j + 1; k < numplanes; k++) {
                Carve_Get_Plane (hm, entnum, brushnum, brush_num_faces, cubeoid_plane_set, k, &p3);

                if (!Carve_Three_Planes_Intersection(&p1, &p2, &p3, point))
                    continue;

                // Check the point against every plane.
				int m = 0;
                for (/*int m = 0*/; m < numplanes; m++) {
					face_plane_t test_plane;
                    Carve_Get_Plane (hm, entnum, brushnum, brush_num_faces, cubeoid_plane_set, m, &test_plane);

                    //float value = Carve_Plane_Point_Value(&test_plane,point);
					float distance = PlanePointDistance(point, &test_plane);

                    if (distance < -CSG_EPSILON_0_01 /*-0.01*/)
                        break;
                } // for m

                if (m != numplanes)
                    continue;

                // The vertex itself will normally be on three planes. Make sure the intersection
				// actually has interior volume.
                qbool has_positive_clearance = false;

                for (int m = 0; m < numplanes; m++) {
					face_plane_t test_plane;
                    Carve_Get_Plane (hm, entnum, brushnum, brush_num_faces, cubeoid_plane_set, m, &test_plane);

                    //float value = Carve_Plane_Point_Value(&test_plane, point);
					float distance = PlanePointDistance(point, &test_plane);

                    if (distance > CSG_EPSILON_0_01 /*0.01*/) {
                        has_positive_clearance = true;
                        break;
                    }
                } // m

				if (has_positive_clearance) {
                    return true;
				}
            } // k
        } // j
    } // i

    return false;
}

// Add a cubeoid face to a result brush.
// reverse == false: copy the face and its plane exactly.
// reverse == true: reverse the winding and plane.
static void Carve_Add_Cubeoid_Face (map_t *hm, int entnum, int result_brushnum, const brushrow_s *r_face_cubeoid, qbool reverse)
{
	int new_facenum = map_add_face (hm, entnum, result_brushnum);
	FACE_DEFINE (d_face_new, hm, entnum, result_brushnum, new_facenum);

	face_copy_data (d_face_new, r_face_cubeoid);

    if (reverse) {
		// Baker: NOTICE A and B are swapped, then plane is recalculated.
        VectorCopyDestSrc(d_face_new->a, r_face_cubeoid->a);
        VectorCopyDestSrc(d_face_new->b, r_face_cubeoid->c);
        VectorCopyDestSrc(d_face_new->c, r_face_cubeoid->b);

        d_face_new->face_plane = r_face_cubeoid->face_plane;

        PlaneReverse(&d_face_new->face_plane);


	} else {

		//d_face_new->pbrtexture = Z_StrDup(r_face_cubeoid->pbrtexture);
	}

	// Baker: The carve cubeioid is always caulk
	d_face_new->pbrtexture = Z_StrDup(COMMON_CAULK);

    Vector4Set(
        d_face_new->tex_axis_s,
        r_face_cubeoid->tex_axis_s[0],
        r_face_cubeoid->tex_axis_s[1],
        r_face_cubeoid->tex_axis_s[2],
        r_face_cubeoid->tex_axis_s[3]);

    Vector4Set(
        d_face_new->tex_axis_t,
        r_face_cubeoid->tex_axis_t[0],
        r_face_cubeoid->tex_axis_t[1],
        r_face_cubeoid->tex_axis_t[2],
        r_face_cubeoid->tex_axis_t[3]);

    d_face_new->trail_count = r_face_cubeoid->trail_count;
    d_face_new->nsewcf_bits = r_face_cubeoid->nsewcf_bits;
}

qbool DuplicatePlaneCheck_Maybe (map_t *hm, int entnum, int brushnum, qbool shall_delete)
{
	qbool hits = false;
	BRUSH_DEFINE (brush, hm, entnum, brushnum);
    for_each_brush_face_2 (facenum, face, hm, entnum, brushnum)

        for (int facenum2 = facenum + 1; facenum2 < brush->brushrowlist.count; facenum2++) {
            brushrow_s *face2 = &brush->brushrowlist.brushrow[facenum2];
			if (face2->is_considered_deleted)
				continue;

			qbool match = VectorCompare(face->face_plane.normal, face2->face_plane.normal) &&
                face->face_plane.dist == face2->face_plane.dist;

            if (match == false)
				continue;

            //zpr ("Duplicate cubeoid plane: Entity %d Brush %d Faces %d and %d (%s and %s)",
            //    entnum, brushnum, facenum, facenum2, face->pbrtexture, face2->pbrtexture);

			//zpr ("Describe: %s", brush->describe);

			if (String_Match (face->pbrtexture, face2->pbrtexture) && shall_delete) {
				face2->is_considered_deleted = true;
				DebugPrintLinef ("Removing: face2 %d", facenum2);
			}

			hits = true;
        } // Forward planes

    for_each_end // face
	return hits;
}

qbool brush_has_duplicate_planes_delete_them (map_t *hm, int entnum, int brushnum)
{
	//return DuplicatePlaneCheck_Maybe (hm, entnum, brushnum);
	return DuplicatePlaneCheck_Maybe (hm, entnum, brushnum, /*delete?*/ true);
}

qbool brush_has_duplicate_planes (map_t *hm, int entnum, int brushnum)
{
	return DuplicatePlaneCheck_Maybe (hm, entnum, brushnum, /*delete?*/ false);
}

static brush_s *Carve_Create_Result_Brush (map_t *hm, int entnum, int brushnum, const brush_s *carve_brush,
    const int *cubeoid_faces_set, const qbool *reverse_faces_set, int num_cubeoid_faces)
{
	int result_brushnum = map_add_brush(hm, entnum); // PART OF THE ENT

    // Copy every original target face
	for_each_brush_face_2 (source_facenum, source_face, hm, entnum, brushnum)
		int result_facenum = map_add_face(hm, entnum, result_brushnum);
		FACE_DEFINE (result_face, hm, entnum, result_brushnum, result_facenum);
        face_copy_data (result_face, source_face);
    for_each_end // face

    // Add the cubeoid boundary faces.
    for (int n = 0; n < num_cubeoid_faces; n ++) {
#ifdef _DEBUG
		int cubeoid_facenum = cubeoid_faces_set[n];
#endif // DEBUG
        brushrow_s *cubeoid_face = &carve_brush->brushrowlist.brushrow[cubeoid_faces_set[n]];

		qbool shall_reverse = reverse_faces_set[n];
        Carve_Add_Cubeoid_Face(hm, entnum, result_brushnum, cubeoid_face, shall_reverse);
    }

    // Calculate the resulting bounds.
	BRUSH_DEFINE (result_brush, hm, entnum, result_brushnum);

#if 0 // DEBUG
	qbool any_duplicate_planes =  brush_has_duplicate_planes_delete_them(hm, entnum, result_brushnum); // DELETE TOO!
	static int dups = 0;
	if (any_duplicate_planes) {
		//printvari (dups);
		dups ++;
	}

	qbool any_dups2 = brush_has_duplicate_planes (hm, entnum, result_brushnum);
	if (any_dups2) {
		int j = 5; // Better never hit
	}

	if (_csg_detail.value && any_duplicate_planes) {
		//map_clipboard_set_brush (hm, entnum, result_brushnum);

		//int j = 5;
	}
#endif // DEBUG

	RELATED_ (map_metric_again)
	if (result_brush->is_a_patch == false) {
		RELATED_ (_cat_brush ) // SAVE BRUSH
		qbool is_bad = brush_fill_face_points_needs_rebuilt (result_brush);
		//qbool is_bad = any_duplicate_planes || brush_fill_face_points_needs_rebuilt (result_brush);

		if (is_bad) {
			RELATED_ (brush_s)
			brush_rebuild_fill_face_points (hm, entnum, result_brushnum);
			/*qbool is_bad2 =*/ brush_fill_face_points_needs_rebuilt (result_brush);
//			if (is_bad2) {
//				// Shouldn't happen
//				int j = 5;
//			}
		} // is bad
	} // not patch

	// WARNING: Calls brush_fill_face_points_needs_rebuilt
	map_metric_brush (hm, entnum, result_brushnum); //
	//map_brush_cubeoid_remetric (hm, entnum, result_brushnum);
		
	

#if 0 //def _DEBUG
	map_clipboard_set_brush (hm, entnum, result_brushnum);
	qbool dup = brush_has_duplicate_planes (hm, entnum, result_brushnum);
	if (dup) {
		int j = 5;
	}
	if (result_brush->bxmins[0] == -2432.0f &&
		result_brush->bxmins[1] == 712.348f &&
		result_brush->bxmins[2] == 0.0f &&
		result_brush->bxmaxs[0] == -2281.41f &&
		result_brush->bxmaxs[1] == 801.391f &&
		result_brush->bxmaxs[2] == 176.0f)
	{
		// This is the result_brush.
		int j =5;
	}
	if (entnum == 0 && result_brushnum == 561) {
		map_clipboard_set_brush (hm, entnum, result_brushnum);
		int j = 5;
	}
#endif // DEBUG
    return result_brush;
}

void brush_cutter_describe (brush_s *br, int cutter_unit, int cutter_entnum, int cutter_brushnum, int entnum, int brushnum, ccs *direction)
{
	br->scutter_unit = cutter_unit;
	br->scutter_entnum = cutter_entnum;
	br->scutter_brushnum = cutter_brushnum;
	c_dpsnprintf6 (br->describe,
		"cutter unit %d ent %d bnum %d carve from ent %d brush %d dir = %s",
		cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum,
		direction/*"West"*/);
}

int map_carve_cubeoid_against_any (map_t *hm, int entnum, int brushnum, const brush_s *carve_brush, int cutter_unit, int cutter_entnum, int cutter_brushnum)
{
	face_plane_t cubeoid_planes[6]; // Fill by cubeoid struct copy -- carve planes
	// The carve brush is a cubeoid, so its six cached face planes can be used directly.
	for (int facenum = 0; facenum < 6; facenum ++) {
		brushrow_s *face = &carve_brush->brushrowlist.brushrow[facenum]; // Can't FACE_GET the carve_brush
		cubeoid_planes[facenum] = face->face_plane; // Baker: struct copy
	}

	face_plane_t result_planes[5];
	int cubeoid_faces_set[5];
	qbool reverse_faces_set[5];

	int result_count = 0;

	//  WEST PIECE - Target brush + outside of west plane.
	result_planes[0] = cubeoid_planes[FACE_WEST_0];

	PlaneReverse(&result_planes[0]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, ONE_EXTRA_PLANE)) {
		cubeoid_faces_set[0] = FACE_WEST_0;
		reverse_faces_set[0] = true;
		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, ONE_EXTRA_PLANE);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "West");
		//c_dpsnprintf5 (br->describe, "cutter %d %d carve from ent %d brush %d dir = %s", cutter_entnum, cutter_brushnum, entnum, brushnum, "West");
		result_count++;

#ifdef _DEBUG
		//map_to_clipboard (hm);
		//return result_count;
#endif // _DEBUG

	} // WEST

	// EAST PIECE - Target brush + outside of east plane.
	result_planes[0] = cubeoid_planes[FACE_EAST_1];

	PlaneReverse(&result_planes[0]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, ONE_EXTRA_PLANE)) {
		cubeoid_faces_set[0] = FACE_EAST_1;
		reverse_faces_set[0] = true;
		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, ONE_EXTRA_PLANE);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "East");
		result_count++;
#ifdef _DEBUG
		//map_to_clipboard (hm);
		//return result_count;
#endif // _DEBUG
	} // EAST

	// SOUTH PIECE - Inside west/east slab + outside of south plane.
	result_planes[0] = cubeoid_planes[FACE_WEST_0];
	result_planes[1] = cubeoid_planes[FACE_EAST_1];
	result_planes[2] = cubeoid_planes[FACE_SOUTH_2];

	PlaneReverse(&result_planes[2]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, THREE_EXTRA_PLANES)) {
		cubeoid_faces_set[0] = FACE_WEST_0;
		cubeoid_faces_set[1] = FACE_EAST_1;
		cubeoid_faces_set[2] = FACE_SOUTH_2;

		reverse_faces_set[0] = false;
		reverse_faces_set[1] = false;
		reverse_faces_set[2] = true;

		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, THREE_EXTRA_PLANES);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "South");

		result_count++;

#if 0 // def _DEBUG
		int bn = BRUSH_GET_NUM(hm, entnum, br);
		map_clipboard_set_brush (hm, entnum, bn);
		//return result_count;
#endif // _DEBUG
	} // SOUTH


	// NORTH PIECE - Inside west/east slab + outside of north plane.
	result_planes[0] = cubeoid_planes[FACE_WEST_0];
	result_planes[1] = cubeoid_planes[FACE_EAST_1];
	result_planes[2] = cubeoid_planes[FACE_NORTH_3];

	PlaneReverse(&result_planes[2]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, THREE_EXTRA_PLANES)) {
		cubeoid_faces_set[0] = FACE_WEST_0;
		cubeoid_faces_set[1] = FACE_EAST_1;
		cubeoid_faces_set[2] = FACE_NORTH_3;

		reverse_faces_set[0] = false;
		reverse_faces_set[1] = false;
		reverse_faces_set[2] = true;

		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, THREE_EXTRA_PLANES);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "North");
		result_count++;
#if 0 // def _DEBUG
		int bn = BRUSH_GET_NUM(hm, entnum, br);
		map_clipboard_set_brush (hm, entnum, bn);
		//return result_count;
#endif // _DEBUG
	} // NORTH


	// BELOW PIECE Inside west/east/north/south slab + outside of ceiling plane.
	result_planes[0] = cubeoid_planes[FACE_WEST_0];
	result_planes[1] = cubeoid_planes[FACE_EAST_1];
	result_planes[2] = cubeoid_planes[FACE_SOUTH_2];
	result_planes[3] = cubeoid_planes[FACE_NORTH_3];
	//result_planes[4] = cubeoid_planes[FACE_CEIL_5];
	result_planes[4] = cubeoid_planes[FACE_FLOOR_4];

	PlaneReverse(&result_planes[4]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, FIVE_EXTRA_PLANES)) {
		cubeoid_faces_set[0] = FACE_WEST_0;
		cubeoid_faces_set[1] = FACE_EAST_1;
		cubeoid_faces_set[2] = FACE_SOUTH_2;
		cubeoid_faces_set[3] = FACE_NORTH_3;
		cubeoid_faces_set[4] = FACE_FLOOR_4; // CHANGED

		reverse_faces_set[0] = false;
		reverse_faces_set[1] = false;
		reverse_faces_set[2] = false;
		reverse_faces_set[3] = false;
		reverse_faces_set[4] = true;

		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, FIVE_EXTRA_PLANES);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "Below");
		result_count++;
#ifdef _DEBUG
		//map_to_clipboard (hm);
		//return result_count;
#endif // _DEBUG
	} // BELOW

	// ABOVE PIECE - Inside west/east/north/south slab + outside of floor plane.
	result_planes[0] = cubeoid_planes[FACE_WEST_0];
	result_planes[1] = cubeoid_planes[FACE_EAST_1];
	result_planes[2] = cubeoid_planes[FACE_SOUTH_2];
	result_planes[3] = cubeoid_planes[FACE_NORTH_3];
	result_planes[4] = cubeoid_planes[FACE_CEIL_5];

	PlaneReverse(&result_planes[4]);

	if (Carve_Planes_Have_Volume(hm, entnum, brushnum, result_planes, FIVE_EXTRA_PLANES)) {
		cubeoid_faces_set[0] = FACE_WEST_0;
		cubeoid_faces_set[1] = FACE_EAST_1;
		cubeoid_faces_set[2] = FACE_SOUTH_2;
		cubeoid_faces_set[3] = FACE_NORTH_3;
		cubeoid_faces_set[4] = FACE_CEIL_5;

		reverse_faces_set[0] = false;
		reverse_faces_set[1] = false;
		reverse_faces_set[2] = false;
		reverse_faces_set[3] = false;
		reverse_faces_set[4] = true; //true; TEST

		brush_s *br = Carve_Create_Result_Brush(hm, entnum, brushnum, carve_brush, cubeoid_faces_set, reverse_faces_set, FIVE_EXTRA_PLANES);
		brush_cutter_describe (br, cutter_unit, cutter_entnum, cutter_brushnum, entnum, brushnum, "Above");
		result_count++;
#ifdef _DEBUG
		//map_to_clipboard (hm);
		return result_count;
#endif // _DEBUG
	} // CEIL

	return result_count;
}

// Plan?  common/carve
CALLERS_ (csgax_includes)
int map_carve_cubeoid_against_any_bulk (map_t *hm, const brush_s *carve_brush_in, int cutter_unit, int cutter_entnum, int cutter_brushnum)
{
	int num_carve_attempts = 0;
	int num_brushes_made = 0;
	int num_fully_clipped_away = 0;
	static int irregulars;
	const brush_s r_carve_brush_copy = (*carve_brush_in);

	for_each_entity_2 (d_entnum, r_ent, hm)
		// Yes this will work the new brushes.  It will probably be fine.
		for_each_brush_2 (d_brushnum_target, d_brush_target, hm, d_entnum)

			Vector3DeclareCopyEx (brmins, d_brush_target->bxmins);
			Vector3DeclareCopyEx (brmaxs, d_brush_target->bxmaxs);
			Vector3DeclareCopyEx (brsize, d_brush_target->bxdims);

			// THIS CREATES MORE BRUSHES AT END but they should not collide
			vec3_t overlapmins, overlapmaxs;
			overlapmins[0] = max(brmins[0], r_carve_brush_copy.bxmins[0]);
			overlapmins[1] = max(brmins[1], r_carve_brush_copy.bxmins[1]);
			overlapmins[2] = max(brmins[2], r_carve_brush_copy.bxmins[2]);

			overlapmaxs[0] = min(brmaxs[0], r_carve_brush_copy.bxmaxs[0]);
			overlapmaxs[1] = min(brmaxs[1], r_carve_brush_copy.bxmaxs[1]);
			overlapmaxs[2] = min(brmaxs[2], r_carve_brush_copy.bxmaxs[2]);

			// Check for no intersection
			if (OVERLAP_IS_NONE(overlapmins))
				continue; // No intersection

			vec3_t overlapdims;

			// 2. Calculate the size/dimensions vector (max - min)
			VectorSubtract(overlapmaxs, overlapmins, overlapdims);

			// Optional: Safety check if they actually overlap
			// If any dimension is negative, they aren't actually colliding/overlapping
			if (overlapdims[0] < 0 || overlapdims[1] < 0 || overlapdims[2] < 0) {
				// No valid overlap volume
				VectorClear(overlapdims); // or handle as zero size
			}

			// This is an extra double check that is probably not necessary
			qbool collide = Boxes_Collide_Epsilon_3D (brmins, brmaxs,
				r_carve_brush_copy.bxmins, r_carve_brush_copy.bxmaxs);


			if (collide == false)
				continue; // HITS - October 23 2026

			//qbool target_fully_inside =
			//	(brmins[0] >= r_carve_brush_copy.bxmins[0] && brmaxs[0] <= r_carve_brush_copy.bxmaxs[0]) &&
			//	(brmins[1] >= r_carve_brush_copy.bxmins[1] && brmaxs[1] <= r_carve_brush_copy.bxmaxs[1]) &&
			//	(brmins[2] >= r_carve_brush_copy.bxmins[2] && brmaxs[2] <= r_carve_brush_copy.bxmaxs[2]);

#define CSG_EPSILON  0.03125
qbool target_fully_inside =
    (brmins[0] >= r_carve_brush_copy.bxmins[0] - CSG_EPSILON && brmaxs[0] <= r_carve_brush_copy.bxmaxs[0] + CSG_EPSILON) &&
    (brmins[1] >= r_carve_brush_copy.bxmins[1] - CSG_EPSILON && brmaxs[1] <= r_carve_brush_copy.bxmaxs[1] + CSG_EPSILON) &&
    (brmins[2] >= r_carve_brush_copy.bxmins[2] - CSG_EPSILON && brmaxs[2] <= r_carve_brush_copy.bxmaxs[2] + CSG_EPSILON);

			dzpr ("== cutter mins/maxs " V3G "/" V3G " target mins/maxs" V3G "/" V3G,
				VS(brmins),VS(brmaxs),VS(r_carve_brush_copy.bxmins),VS(r_carve_brush_copy.bxmaxs) );

			dzpr ("Cutter size: " V3G  " Target size:" V3G,
				VS(r_carve_brush_copy.bxdims),
				VS(brsize) );

#if 0 // def _DEBUG
			map_clipboard_set_brush (hm, d_entnum, d_brushnum_target);
#endif

#ifdef _DEBUG
			static int carve_iters;
			carve_iters ++;
#endif
			//           1         2       3  4          5  7
			dzpr ("iter %d Cutter unit %d: ent %d:%d against %d:%d %s overlap " V3G,
				carve_iters,
				cutter_unit,
				cutter_entnum,
				cutter_brushnum,
				d_entnum,
				d_brushnum_target,
				d_brush_target->describe,
					VS(overlapdims)
				);

			if (target_fully_inside) {
				// Completely engulfed by the cutter: no split carve results will be generated.
				d_brush_target->is_considered_deleted = true;
				//map_clipboard_set_brush (hm, d_entnum, d_brushnum_target);
				//num_brushes_made ++; // Need to have a non-zero result or map won't write.
				num_carve_attempts ++;
				num_fully_clipped_away ++;
				//num_made = 0;
				//continue;
				continue;
			}

			if (d_brush_target->scutter_unit == cutter_unit &&
				d_brush_target->scutter_entnum == cutter_entnum &&
				d_brush_target->scutter_brushnum == cutter_brushnum) {
				// FUCK IT. NO DOUBLE COLLIDE.  JUST SKIP
				// October 24 2026
				irregulars ++;
				d_brush_target->irregular = true;
				zpr ("Carve re-collision ignored for cutter unit %d : ent %d : brush %d",
					cutter_unit, cutter_entnum, cutter_brushnum
				);
				continue;
			}

			//if (cutter_entnum == 0 && cutter_brushnum == 184
			//	&&
			//	d_entnum == 3 && d_brushnum_target == 787) {
			//		//map_clipboard_set_brush (hm, d_entnum, d_brushnum_target);

			//		int j = 5;
			//}


			static int seconds_iters = 0;
			if (*d_brush_target->describe) {
				dzpr ("seconds_iters %d", seconds_iters);
				seconds_iters ++;

				int cutter_old_entnum = -1, cutter_old_brushnum = -1;
				qbool read_it = false;
				int res = sscanf(d_brush_target->describe, "cutter %d %d", &cutter_old_entnum, &cutter_old_brushnum);
				if (res == 2) {
					// Successfully parsed cutter_entnum and cutter_brushnum
					read_it = true;
					if (read_it && cutter_entnum == cutter_old_entnum && cutter_old_brushnum == cutter_brushnum) {
						// Self carving fail
						//int j = 444;
					}
				}

				//int j = 5;
			}
			d_brush_target->is_considered_deleted = true;
			//int num_created = 0;
			// num_made_here is bullshit.
			// It could have been deleted during metrics.
#ifdef _DEBUG
            int old_brush_count = BRUSH_COUNT (hm, d_entnum);
#endif // _DEBUG
			num_carve_attempts += 1;
			int num_made_here = map_carve_cubeoid_against_any (hm, d_entnum, d_brushnum_target, &r_carve_brush_copy, cutter_unit, cutter_entnum, cutter_brushnum);

			BRUSH_DEFINE_CONST (d_brush_target2, hm, d_entnum, d_brushnum_target);

#if 0 // def _DEBUG
            int new_brush_count = BRUSH_COUNT (hm, d_entnum);
			BRUSH_DEFINE_CONST (d_brush_made0, hm, d_entnum, (old_brush_count + 0) );
			map_clipboard_set_brush (hm, d_entnum, (old_brush_count + 0));
			BRUSH_DEFINE_CONST (d_brush_made1, hm, d_entnum, (old_brush_count + 1) );
			map_clipboard_set_brush (hm, d_entnum, (old_brush_count + 1));
			BRUSH_DEFINE_CONST (d_brush_made2, hm, d_entnum, (old_brush_count + 2) );
			map_clipboard_set_brush (hm, d_entnum, (old_brush_count + 2));
#endif // _DEBUG
			//if (new_brush_count > old_brush_count)
			//	map_clipboard_set_brush (hm, d_entnum, (new_brush_count - 1));
			//if (cutter_entnum == 0 && cutter_brushnum == 184
			//	&&
			//	d_entnum == 3 && d_brushnum_target == 787) {
			//		BRUSH_DEFINE_CONST (d_brush_target2, hm, d_entnum, (new_brush_count - 1) );
			//		map_clipboard_set_brush (hm, d_entnum, (new_brush_count - 1));


			//		int j = 5;
			//}
			if (num_made_here) {
				num_brushes_made += num_made_here;
			}


			if (num_made_here == 0) {
				debugprintvari (num_fully_clipped_away);
				num_fully_clipped_away ++;
			}

			// Baker: We are trying to get 1 brush collision to work correctly
			// before trying to do all of them
			//return 1;

		for_each_end // for_each_brush_or_patch
	for_each_end // after_world
#ifdef _DEBUG
		{
			int j = num_fully_clipped_away;
			int k = num_brushes_made; // May get deleted if invalid planes.
			int jj = num_carve_attempts;
			if (irregulars) {
				int pp88 = irregulars;
			}
		}
#endif // _DEBUG
	return num_carve_attempts; //num_made;
}

#if 0
int map_carve_cubeoid_against_any_bulk_test (map_t *hm, const brush_s *carve_brush_in, int cutter_instance, int cutter_entnum, int cutter_brushnum)
{
	int num_made = 0;
	const brush_s r_carve_brush_copy = (*carve_brush_in);

	for_each_entity_2 (d_entnum, r_ent, hm)
		// Yes this will work the new brushes.  It will probably be fine.
		for_each_brush_2 (d_brushnum_target, d_brush_target, hm, d_entnum)

			Vector3DeclareCopyEx (brmins, d_brush_target->bxmins);
			Vector3DeclareCopyEx (brmaxs, d_brush_target->bxmaxs);
			Vector3DeclareCopyEx (brsize, d_brush_target->bxdims);

			// THIS CREATES MORE BRUSHES AT END but they should not collide
			vec3_t overlapmins, overlapmaxs;
			overlapmins[0] = max(brmins[0], r_carve_brush_copy.bxmins[0]);
			overlapmins[1] = max(brmins[1], r_carve_brush_copy.bxmins[1]);
			overlapmins[2] = max(brmins[2], r_carve_brush_copy.bxmins[2]);

			overlapmaxs[0] = min(brmaxs[0], r_carve_brush_copy.bxmaxs[0]);
			overlapmaxs[1] = min(brmaxs[1], r_carve_brush_copy.bxmaxs[1]);
			overlapmaxs[2] = min(brmaxs[2], r_carve_brush_copy.bxmaxs[2]);

			// Check for no intersection
			if (OVERLAP_IS_NONE(overlapmins))
				continue; // No intersection

			vec3_t overlapdims;

			// 2. Calculate the size/dimensions vector (max - min)
			VectorSubtract(overlapmaxs, overlapmins, overlapdims);

			// Optional: Safety check if they actually overlap
			// If any dimension is negative, they aren't actually colliding/overlapping
			if (overlapdims[0] < 0 || overlapdims[1] < 0 || overlapdims[2] < 0) {
				// No valid overlap volume
				VectorClear(overlapdims); // or handle as zero size
			}

			// This is an extra double check that is probably not necessary
			qbool collide = Boxes_Collide_Epsilon_3D (brmins, brmaxs,
				r_carve_brush_copy.bxmins, r_carve_brush_copy.bxmaxs);


			if (collide == false)
				continue;

			dzpr ("cutter mins/maxs " V3G "/" V3G " target mins/maxs" V3G "/" V3G,
				VS(brmins),VS(brmaxs),VS(r_carve_brush_copy.bxmins),VS(r_carve_brush_copy.bxmaxs) );

			dzpr ("Cutter size: " V3G  " Target size:" V3G,
				VS(r_carve_brush_copy.bxdims),
				VS(brsize) );


#ifdef _DEBUG
			static int carve_iters;
			carve_iters ++;
#endif
			//           1         2       3  4          5  7
			dzpr ("iter %d Cutter %d: ent %d:%d against %d:%d %s overlap " V3G,
				carve_iters,
				cutter_instance,
				cutter_entnum,
				cutter_brushnum,
				d_entnum,
				d_brushnum_target,
				d_brush_target->describe,
					VS(overlapdims)
				);


		for_each_end // for_each_brush_or_patch
	for_each_end // after_world

	return num_made;
}
#endif
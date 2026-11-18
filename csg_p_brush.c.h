// csg_p_brush.c.h

RELATED_ (BRUSH_MAIN BRUSH_DIRECTION)

#pragma once

const vec3_t flat_ground_top_0_0_NEG1 = {0,0,-1};
const vec3_t flat_ground_top_0_0_1_rev = {0,0,1};


RELATED_ (map_load_from_string)

CALLERS_ (brush_or_patch_tranlate_with_texturelock_was_processed exclusive)

// NOTE: > instead of >= ... equals is a TOUCH HIT
qbool BOX_XY_Touch (const vec3_t aL, const vec3_t aH, const vec3_t bL, const vec3_t bH)
{
	//if (a->left >= RECT_RIGHTOF(*target))  return false; // a_x beyond width (cull)
	//if (a->top  >= RECT_BOTTOMOF(*target)) return false; // a_y beyond height (cull)
	if (aL[0] > bH[0]) return false;
	if (aL[1] > bH[1]) return false;
	//if (RECT_RIGHTOF(*a)  <= target->left) return false; // x2 doesn't reach 0 (cull)
	//if (RECT_BOTTOMOF(*a) <= target->top)  return false; // x2 doesn't reach 0 (cull)
	if (aH[0] < bL[0]) return false;
	if (aH[1] < bL[1]) return false;
	return true; // Touch!
}

qbool BOX_XY_Collide (const vec3_t aL, const vec3_t aH, const vec3_t bL, const vec3_t bH)
{
	//if (a->left >= RECT_RIGHTOF(*target))  return false; // a_x beyond width (cull)
	//if (a->top  >= RECT_BOTTOMOF(*target)) return false; // a_y beyond height (cull)
	if (aL[0] >= bH[0]) return false;
	if (aL[1] >= bH[1]) return false;
	//if (RECT_RIGHTOF(*a)  <= target->left) return false; // x2 doesn't reach 0 (cull)
	//if (RECT_BOTTOMOF(*a) <= target->top)  return false; // x2 doesn't reach 0 (cull)
	if (aH[0] <= bL[0]) return false;
	if (aH[1] <= bL[1]) return false;
	return true; // Collision!
}

//qbool Boxes_Collide_3D (
//    const vec3_t brmins,
//    const vec3_t brmaxs,
//    const vec3_t carvemins,
//    const vec3_t carvemaxs)
//{
//    if (brmaxs[0] <= carvemins[0] ||
//        brmins[0] >= carvemaxs[0] ||
//        brmaxs[1] <= carvemins[1] ||
//        brmins[1] >= carvemaxs[1] ||
//        brmaxs[2] <= carvemins[2] ||
//        brmins[2] >= carvemaxs[2])
//    {
//        return false;
//    }
//
//    return true;
//}

qbool Boxes_Collide_3D (
    const vec3_t brmins,
    const vec3_t brmaxs,
    const vec3_t brmins2,
    const vec3_t brmaxs2)
{
    if (brmaxs[0] <= brmins2[0] ||
        brmins[0] >= brmaxs2[0] ||
        brmaxs[1] <= brmins2[1] ||
        brmins[1] >= brmaxs2[1] ||
        brmaxs[2] <= brmins2[2] ||
        brmins[2] >= brmaxs2[2])
    {
        return false;
    }

    return true;
}

qbool Boxes_Touch_3D (
    const vec3_t brmins,
    const vec3_t brmaxs,
    const vec3_t brmins2,
    const vec3_t brmaxs2)
{
    if (brmaxs[0] < brmins2[0] ||
        brmins[0] > brmaxs2[0] ||
        brmaxs[1] < brmins2[1] ||
        brmins[1] > brmaxs2[1] ||
        brmaxs[2] < brmins2[2] ||
        brmins[2] > brmaxs2[2])
    {
        return false;
    }

    return true;
}

#define BOX_EPSILON 0.03125f // Standard Quake-friendly tolerance (1/32 unit)

qbool Boxes_Collide_Epsilon_3D (
    const vec3_t brmins,
    const vec3_t brmaxs,
    const vec3_t carvemins,
    const vec3_t carvemaxs)
{

    // Require a meaningful overlap, not just a floating-point touch
    if (brmaxs[0] <= carvemins[0] + BOX_EPSILON ||
        brmins[0] >= carvemaxs[0] - BOX_EPSILON ||
        brmaxs[1] <= carvemins[1] + BOX_EPSILON ||
        brmins[1] >= carvemaxs[1] - BOX_EPSILON ||
        brmaxs[2] <= carvemins[2] + BOX_EPSILON ||
        brmins[2] >= carvemaxs[2] - BOX_EPSILON)
    {
        return false; // They are separated or only touching by noise
    }

    return true;
}

qbool brush_xy_touches_brush (const brush_s *a, const brush_s *b)
{
	qbool does_touch = BOX_XY_Touch(a->bxmins, a->bxmaxs, b->bxmins, b->bxmaxs);
	return does_touch;
}


static void brush_or_patch_tranlate_patch_translate(const brush_s *p_brush, const vec3_t vadd)
{
	// PATCH - Translate all vertexes
	for_each_patch_row (rownum, p_brush, d_patchrow)
		for_each_patch_column (columnnum, p_brush, d_patchrow)
			float *pvec = &d_patchrow->flots[columnnum * 5 + 0];
			VectorAdd (vadd, pvec, pvec); // Translate
		for_each_end // col
	for_each_end // patchrow
}

void brush_or_patch_tranlate_with_texturelock_was_processed (const brush_s *d_brush, const vec3_t vadd)
{
	if (d_brush->is_a_patch) {
		brush_or_patch_tranlate_patch_translate (d_brush, vadd);
	}

	// REAL BRUSH
	for_each_brush_face (facenum, d_brush, d_face)
		VectorAdd (d_face->a, vadd, d_face->a); // Translate
		VectorAdd (d_face->b, vadd, d_face->b); // Translate
		VectorAdd (d_face->c, vadd, d_face->c); // Translate

		// OCTOBER 3 2024 - TEXTURELOCK
		double xscale  = d_face->faceparms[FT_USCALE_1];
		double yscale  = d_face->faceparms[FT_VSCALE_2];
		double xshift = (d_face->tex_axis_s[X1_UX_0] * -vadd[0] + d_face->tex_axis_s[X1_UY_1] * -vadd[1] + d_face->tex_axis_s[X1_UZ_2] * -vadd[2]) / xscale;
		double yshift = (d_face->tex_axis_t[X2_VX_0] * -vadd[0] + d_face->tex_axis_t[X2_VY_1] * -vadd[1] + d_face->tex_axis_t[X2_VZ_2] * -vadd[2]) / yscale;
		double newxshift = d_face->tex_axis_s[X1_UOFFSET_3] + xshift;
		double newyshift = d_face->tex_axis_t[X2_VOFFSET_3] + yshift;
		d_face->tex_axis_s[X1_UOFFSET_3] = newxshift;
		d_face->tex_axis_t[X2_VOFFSET_3] = newyshift;
	for_each_end // each brush face
}

// An entity can have multiple brushes.
// Returns true on success
// Better be a brush.


// No Interaction with patches, returns false
RELATED_ (superior is brush_metric)
// Baker: August 14 2026 -- one living caller
CALLERS_ (dead entitylist_perform_layout_from_boxes) // _layout_to_boxes supposedly elder_quad_shrine.map
qbool brush_get_bbox_was_processed (brush_s *d_brush, vec3_t brmins, vec3_t brmaxs)
{
	if (d_brush->is_a_patch)
		return false; // PATCH, SKIP

	qbool was_ok = false;
	bounds_init_9 (brmins, brmaxs);

	// REAL BRUSH
	for_each_brush_face (facenum, d_brush, d_face)
		// EXPANDING BRUSH METRIX
		bounds_expand (d_face->a, brmins, brmaxs);
		bounds_expand (d_face->b, brmins, brmaxs);
		bounds_expand (d_face->c, brmins, brmaxs);

		// FIGURING BBOX OF ACTUAL FACE
		bounds_init_9 (d_face->bfmins, d_face->bfmaxs);

		bounds_expand (d_face->a, d_face->bfmins, d_face->bfmaxs);
		bounds_expand (d_face->b, d_face->bfmins, d_face->bfmaxs);
		bounds_expand (d_face->c, d_face->bfmins, d_face->bfmaxs);

		if (!was_ok) was_ok = true;
	for_each_end // brush row

	return was_ok;
}





// weld and infodump call this
ccs *brush_texture_first_non_common (brush_s *r_brush, int *p_facenumreply)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (FACE_IS_COMMON (r_face)) //if (String_Starts_With (r_face->pbrtexture, COMMON_SLASH))
			continue;
		NOT_MISSING_ASSIGN (p_facenumreply, facenum);
		return r_face->pbrtexture;
	for_each_end // face
	return NULL;
}

/**
 * Calculates the bounding box volume of a single brush using its pre-calculated dimensions.
 */
int64_t brush_volume_cubic_units(const brush_s *b) {
    if (!b) return 0.0f;

    // Multiply Width * Length * Height
    int64_t volume = (int64_t)b->bxdims[0] * (int64_t)b->bxdims[1] * (int64_t)b->bxdims[2];

    // Use absolute value in case dimensions were calculated with flipped bounds
    return volume;
}

CALLERS_ (csqc_delete_world_brushes_with_nondominant_texture)


// USED -- MOVE ALONG September 19 2026 ...
int brush_facenum_first_with_texture (brush_s *r_brush, ccs *texturename)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (String_Match (r_face->pbrtexture, texturename))
			return facenum;
	for_each_end // for_each_brush_face
	return not_found_neg1;
}

qbool brush_has_texturename (brush_s *r_brush, ccs *texturename)
{
	int facenum = brush_facenum_first_with_texture(r_brush, texturename);
	if (facenum == not_found_neg1)
		return false;
	return true;
}

int brush_texturename_starts_with_facenum (brush_s *r_brush, ccs *texturename_prefix)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (String_Starts_With (r_face->pbrtexture, texturename_prefix))
			return true;
	for_each_end // for_each_brush_face
	return false;
}

CALLERS_ (weld nothing else)


brushrow_s *brush_add_face (brush_s *d_brush)
{
	brushrow_s *br = _LIFE_brushface_alloc (&d_brush->brushrowlist);
	return br;
}

void brush_add_copying_brush_or_patch(entityx_s *d_ent, const brush_s *r_brush)
{
	 // Create
	brush_s *copy_brush = _LIFE_brush_add (&d_ent->brushlist);
	copy_brush->is_a_patch = r_brush->is_a_patch;

	// PATCH
	if (r_brush->is_a_patch) {
		// If patch, copy patch stuff ...
		copy_brush->patch_rows = r_brush->patch_rows;
		copy_brush->patch_cols = r_brush->patch_cols;
		copy_brush->other3[0] = r_brush->other3[0];
		copy_brush->other3[1] = r_brush->other3[1];
		copy_brush->other3[2] = r_brush->other3[2];
		copy_brush->patch_texture = Z_StrDup (r_brush->patch_texture);

		for_each_const_patch_row (rownum, r_brush, r_patchrow)
			patchrow_s *copy_patchrow = patchrowlist_add (&copy_brush->patchrowlist);

			// 5 x number of columns - mem copy all the numbers
			size_t size_to_copy = sizeof(copy_patchrow->flots); // Should be about 1200
			memcpy (copy_patchrow->flots, r_patchrow->flots, size_to_copy);
		for_each_end // patch_row
		return;
	}

	// REAL BRUSH
	for_each_const_brush_face (facenum, r_brush, r_face)
		brushrow_s *copy_face = _LIFE_brushface_alloc(&copy_brush->brushrowlist);

		memcpy (copy_face, r_face, sizeof(*r_face));
		copy_face->pbrtexture = Z_StrDup (copy_face->pbrtexture);
	for_each_end // for_each_brush_face
}

void map_add_brush_or_patch_copy(map_t *hm_dst, int dst_entnum, const map_t *hm_src, int src_entnum, int src_brushnum)
{
	ENT_DEFINE			(d_ent, hm_dst, dst_entnum);
	BRUSH_DEFINE_CONST	(r_brush, hm_src, src_entnum, src_brushnum);

	brush_add_copying_brush_or_patch (d_ent, r_brush);
}


// CHATGPT


#define BRUSH_FACE_POINT_EPSILON_0_01		0.01f
#define BRUSH_FACE_DET_EPSILON_0_0000001	0.000001f

static int IntersectThreePlanes(
    const brushrow_s *a,
    const brushrow_s *b,
    const brushrow_s *c,
    vec3_t point)
{
    vec3_t cross_bc;
    vec3_t cross_ca;
    vec3_t cross_ab;
    float det;
    int axis;

    CrossProduct(
        b->face_plane.normal,
        c->face_plane.normal,
        cross_bc);

    CrossProduct(
        c->face_plane.normal,
        a->face_plane.normal,
        cross_ca);

    CrossProduct(
        a->face_plane.normal,
        b->face_plane.normal,
        cross_ab);

    det = DotProduct(a->face_plane.normal, cross_bc);

    if (fabsf(det) < BRUSH_FACE_DET_EPSILON_0_0000001)
        return 0;

    for (axis = 0; axis < 3; axis++)
    {
        point[axis] =
            (a->face_plane.dist * cross_bc[axis]
           + b->face_plane.dist * cross_ca[axis]
           + c->face_plane.dist * cross_ab[axis]) / det;
    }

    return 1;
}

/*
 * Return 1 if the point is inside all planes with one
 * consistent normal orientation, otherwise return 0.
 */
static int PointInside(const brush_s *brush, const vec3_t point)
{
    int all_positive = 1;
    int all_negative = 1;

	for_each_brush_face (facenum, brush, face)
        float d = PlanePointDistance(point, &face->face_plane);

        if (d < -BRUSH_FACE_POINT_EPSILON_0_01)
            all_positive = 0;

        if (d > BRUSH_FACE_POINT_EPSILON_0_01)
            all_negative = 0;
    for_each_end // face

    return all_positive || all_negative;
}

static void BrushFace_AddPoint(brushrow_s *face, const vec3_t point)
{
    int i;
    vec3_t delta;

    for (i = 0; i < face->num_points; i++) {
        delta[0] = face->points[i][0] - point[0];
        delta[1] = face->points[i][1] - point[1];
        delta[2] = face->points[i][2] - point[2];

        if (DotProduct(delta, delta)
            < BRUSH_FACE_POINT_EPSILON_0_01 *
              BRUSH_FACE_POINT_EPSILON_0_01)
        {
            return;
        }
    } // face points

    if (face->num_points >= BRUSH_FACE_MAX_POINTS_64) {
        zpr ("Brush face exceeded %d points", BRUSH_FACE_MAX_POINTS_64);
        return;
    }

    face->points[face->num_points][0] = point[0];
    face->points[face->num_points][1] = point[1];
    face->points[face->num_points][2] = point[2];

    face->num_points++;
}

/*
 * Calculate the vertices for every face of one convex brush.
 * faces[] contains pointers to all brush faces.
 */
qbool brush_fill_face_points_needs_rebuilt (brush_s *brush)
{
    int i, j, k, f;
    vec3_t point;

	if (brush->is_considered_deleted) {
		//static int baddies_here;
		//baddies_here ++;
		return false;
	}

	if (brush->is_a_patch) {
		zpr ("Patch!  Do not come here!");
		return false;
	}

	brush->has_bad_faces = false;
	int numfaces = FACE_COUNT_PTR (brush);

	if (!numfaces || numfaces < BRUSH_MIN_FACES_4) {
		// DEBUG info really ... 
		// zpr ("Brush has less than 4 faces shall be deleted. (Carve can generate these)");
		brush->is_considered_deleted = true;
		brush->has_bad_faces = -1;
        return false;
	}

	for_each_brush_face (facenum, brush, face)
		face_plane_t_calc (face);
        face->num_points = 0;
	for_each_end // for_each_brush_face

    for (i = 0; i < numfaces - 2; i++) {
		FACE_DEFINE_PTR (face_i, brush, i);
        for (j = i + 1; j < numfaces - 1; j++) {
			FACE_DEFINE_PTR (face_j, brush, j);
            for (k = j + 1; k < numfaces; k++) {
				FACE_DEFINE_PTR (face_k, brush, k);
                if (!IntersectThreePlanes(face_i, face_j, face_k, point)) {
                    continue;
                }

                if (!PointInside(brush, point)) {
                    continue;
                }

                for (f = 0; f < numfaces; f++) {
					FACE_DEFINE_PTR (this_face, brush, f);
                    if (fabsf(PlanePointDistance(point, &this_face->face_plane))
						<= BRUSH_FACE_POINT_EPSILON_0_01)
                    {
                        BrushFace_AddPoint(this_face, point);
                    }
                }
            }
        }
    }


    for_each_brush_face (facenum, brush, face)
		FACE_DEFINE_PTR (this_face, brush, facenum);
		if (this_face->num_points < FACE_MIN_POINTS_3) {
			//zpr ("Face %d with less than 3 points", facenum); HAPPENS!!!
			face->is_considered_deleted = true;// October 20 2026 SUPERNOVA
			brush->has_bad_faces ++;
		}
    for_each_end // for_each_brush_face

//START
	// Detect duplicate axis-aligned planes.
	for (i = 0; i < numfaces - 1; i++) {
		FACE_DEFINE_PTR(face_i, brush, i);

		if (face_i->face_plane.normal[0] != 0.0f &&
			face_i->face_plane.normal[1] != 0.0f &&
			face_i->face_plane.normal[2] != 0.0f)
			continue;

		for (j = i + 1; j < numfaces; j++) {
			FACE_DEFINE_PTR(face_j, brush, j);

			//qbool is_same =
			//	face_i->face_plane.normal[0] == face_j->face_plane.normal[0] &&
			//	face_i->face_plane.normal[1] == face_j->face_plane.normal[1] &&
			//	face_i->face_plane.normal[2] == face_j->face_plane.normal[2] &&
			//	face_i->face_plane.dist == face_j->face_plane.dist;

#define PLANE_EPSILON 0.001f

			//qbool catch1 = (face_i->a[0] == -2432.0f &&
			//	face_i->a[1] == 623.304f &&
			//	face_i->a[2] == -64.0f &&
			//	face_i->b[0] == -2432.0f &&
			//	face_i->b[1] == 712.348f &&
			//	face_i->b[2] == -64.0f &&
			//	face_i->c[0] == -2432.0f &&
			//	face_i->c[1] == 623.304f &&
			//	face_i->c[2] == 0.0f);

			//qbool catch2 = (face_j->a[0] == -2432.0f &&
			//	face_j->a[1] == -512.0f &&
			//	face_j->a[2] == 0.0f &&
			//	face_j->b[0] == -2432.0f &&
			//	face_j->b[1] == -512.0f &&
			//	face_j->b[2] == -256.0f &&
			//	face_j->c[0] == -2432.0f &&
			//	face_j->c[1] == 1024.0f &&
			//	face_j->c[2] == 0.0f);

			//if (catch1 && catch2) {
			//	int j = 5;
			//}

		qbool is_same =
			fabsf(face_i->face_plane.normal[0] - face_j->face_plane.normal[0]) <= PLANE_EPSILON &&
			fabsf(face_i->face_plane.normal[1] - face_j->face_plane.normal[1]) <= PLANE_EPSILON &&
			fabsf(face_i->face_plane.normal[2] - face_j->face_plane.normal[2]) <= PLANE_EPSILON &&
			fabsf(face_i->face_plane.dist - face_j->face_plane.dist) <= PLANE_EPSILON;

			if (is_same && String_Match(face_i->pbrtexture, face_j->pbrtexture)) {
				// DUP
				face_j->is_considered_deleted = true;
				brush->has_bad_faces++;
			}
		} // j
	} // i
//

	int num_faces_here = FACE_COUNT_PTR (brush);
	int net_faces = num_faces_here - brush->has_bad_faces;

	if (brush->has_bad_faces) {
		static int iters2;
		iters2 ++;
		//int j = 5;
		//if (net_faces < 6 && num_faces_here >=6) {
		//	int j = 5; // Odd!
		//}
	}
	if (net_faces == 0) {
		static int baddies_here;
		baddies_here ++;
		brush->is_considered_deleted = true;
		brush->has_bad_faces = -2;
	} else if (net_faces < BRUSH_MIN_FACES_4) {
		static int baddies_here;
		baddies_here ++;
		brush->is_considered_deleted = true;
		brush->has_bad_faces = -3;
	}
	return brush->has_bad_faces;
}

void brush_rebuild_fill_face_points (map_t *hm, int entnum, int brushnum)
{
	BRUSH_DEFINE (brush, hm, entnum, brushnum);
	if (brush->has_bad_faces == false)
		return;

	brush_s deadbrush_copy = *brush;
	//size_t test = sizeof(*brush);
	#define MemSet0(ptr) memset((ptr), 0, sizeof(*(ptr)))
	MemSet0 (brush);

	for_each_brush_face (n, (&deadbrush_copy), dead_face)
		if (dead_face->is_considered_deleted)
			continue;
		brushrow_s	*new_face = _LIFE_brushface_alloc (&brush->brushrowlist);
		face_copy_data (new_face, dead_face);
	for_each_end

	brush_free (&deadbrush_copy);
}

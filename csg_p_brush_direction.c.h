// csg_p_brush_direction.c.h

#define BRUSH_DIRECTION
#pragma once

// For a 6 sided brush, identifies quadrants
// No active callers - Sep 28 2026
int brush_is_adjacent (brush_s *b1, brushrow_s *face1, brush_s *b2)
{
	//  The top should share a mins/maxs
	if (BRUSH_IS_5_SIDED_TRI(b1) && BRUSH_IS_5_SIDED_TRI(b2)) {
		if (Vector3Compare(b1->bxmins, b2->bxmins) && Vector3Compare(b1->bxmaxs, b2->bxmaxs)) {
			return FACE_TRIANGLE_FRIEND_17; //  quadrant_triangle_friend_7;
		}
	}

	qbool is_adjacent = false;
	for_each_brush_face (a, b1, pbr1)
		for_each_brush_face (b, b2, pbr2)
			if (Vector3Compare(pbr1->bfmins, pbr2->bfmins) &&  Vector3Compare(pbr1->bfmins, pbr2->bfmins)) {
				is_adjacent = true;
				return FACE_ADJACENT_ANY_18;
			}
		for_each_end // for_each_brush_face // b 
	for_each_end // for_each_brush_face // a

	return FACE_INVALID_NEG_1;
}

// This is shared min/max for a face
// And we are hitting diagonal adjacents + triangle friends.
// No active callers Sep 28 2026
qbool brush_is_adjacent_with_brush_with_diags (brush_s *pb1, brush_s *pb2)
{
	//  The top should share a mins/maxs
	qbool is_adjacent = false;
	for_each_brush_face (a, pb1, pbr1)
		if (face_is_up_or_down(pbr1))
			continue; // We only want sides.

		for_each_brush_face (b, pb2, pbr2)
			if (face_is_up_or_down(pbr2))
				continue; // We only want sides.

			if (Vector3Compare(pbr1->bfmins, pbr2->bfmins) &&  Vector3Compare(pbr1->bfmaxs, pbr2->bfmaxs)) {
				is_adjacent = true;
				//num_different_adjacents ++;
			}
		for_each_end // for_each_brush_face // b 
	for_each_end // for_each_brush_face // a

	return is_adjacent;
}


// WENSDU
qbool brush_has_a_north_face (const brush_s *r_brush)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (r_brush->bxmaxs[1] == r_face->bfmins[1]) // This face is northmost
			if (r_face->bfdims[1] == 0) // This face is X flat |------- |
				return true;
	for_each_end // for_each_brush_face
	return false;
}

qbool brush_has_a_south_face (const brush_s *r_brush)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (r_brush->bxmins[1] == r_face->bfmaxs[1]) // This face is southmost
			if (r_face->bfdims[1] == 0) // This face is X flat |------- |
				return true;
	for_each_end // for_each_brush_face
	return false;
}

qbool brush_has_a_west_face (const brush_s *r_brush)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (r_brush->bxmins[0] == r_face->bfmaxs[0]) // This face is westmost
			if (r_face->bfdims[0] == 0) // This face is Y flat |------- |
				return true;
	for_each_end // for_each_brush_face
	return false;
}

qbool brush_has_a_east_face (const brush_s *r_brush)
{
	for_each_brush_face (facenum, r_brush, r_face)
		if (r_brush->bxmaxs[0] == r_face->bfmins[0]) // This face is westmost
			if (r_face->bfdims[0] == 0) // This face is Y flat |------- |
				return true;
	for_each_end // for_each_brush_face
	return false;
}

int brush_top_face_index_maybe (const brush_s *brush)
{
	if (brush->is_cubeoid) 
		return brush->cubeoid_index[FACE_CEIL_5];
	if (brush->is_flat_half_cubeoid) 
		return brush->cubeoid_index[FACE_CEIL_5];
	if (brush->is_terrain_wedge) 
		return brush->top_slanted_index;

	return not_found_neg1;
}

// Any same face neighbor.
FACE_INT_e brush_is_adjacent_with_face_match(brush_s *a, brush_s *b)
{
	//  The top should share a mins/maxs
	if (Vector3Compare (a->bxmins, b->bxmins) && Vector3Compare (a->bxmaxs, b->bxmaxs))
		return FACE_TRIANGLE_FRIEND_17;

	if (a->bxmins[1] == b->bxmins[1] && a->bxmaxs[1] == b->bxmaxs[1]) {
		// SAME Y mins/maxs
		if (a->bxmins[0] == b->bxmaxs[0]) {
			if (!brush_has_a_east_face (b))
				return FACE_INVALID_NEG_1; // Diag
			return FACE_WEST_0; // b is west of a
		}
		if (a->bxmaxs[0] == b->bxmins[0]) {
			if (!brush_has_a_west_face (b))
				return FACE_INVALID_NEG_1; // Diag

			return FACE_EAST_1; // b is east of a
		}

		return FACE_INVALID_NEG_1;
	}

	if (a->bxmins[0] == b->bxmins[0] && a->bxmaxs[0] == b->bxmaxs[0]) {
		// SAME X mins/maxs
		if (a->bxmins[1] == b->bxmaxs[1]) {
			if (!brush_has_a_north_face (b))
				return FACE_INVALID_NEG_1; // Diag

			return FACE_SOUTH_2; // b is south of a
		}
		if (a->bxmaxs[1] == b->bxmins[1]) {
			if (!brush_has_a_south_face (b))
				return FACE_INVALID_NEG_1; // Diag

			return FACE_NORTH_3; // b is north of a
		}
		return FACE_INVALID_NEG_1;
	}

	// How to eliminate the diag matches?
	// The wanted ones have a top face with 2 points of commonality if truly adjacent.
	return FACE_INVALID_NEG_1;
}

// Something is not quite right with this one
// Nov 11 2026 - Don't know what this is, but no one uses it.
#if 0 // Not even used in older pre-processor
static FACE_INT_e brush_is_adjacent_with_face_match_diag(brush_s *a, brush_s *b)
{
	//  The top should share a mins/maxs
	if (Vector3Compare (a->bxmins, b->bxmins) && Vector3Compare (a->bxmaxs, b->bxmaxs))
		return FACE_TRIANGLE_FRIEND_17;

	if (a->bxmins[1] == b->bxmins[1] && a->bxmaxs[1] == b->bxmaxs[1]) {
		// SAME Y mins/maxs
		if (a->bxmins[0] == b->bxmaxs[0]) {
			if (!brush_has_a_east_face (b))
				return FACE_DIAGONAL_ADJACENT_ANY_19; // Diag
			return FACE_WEST_0; // b is west of a
		}
		if (a->bxmaxs[0] == b->bxmins[0]) {
			if (!brush_has_a_west_face (b))
				return FACE_DIAGONAL_ADJACENT_ANY_19; // Diag

			return FACE_EAST_1; // b is east of a
		}

		return FACE_INVALID_NEG_1;
	}

	if (a->bxmins[0] == b->bxmins[0] && a->bxmaxs[0] == b->bxmaxs[0]) {
		// SAME X mins/maxs
		if (a->bxmins[1] == b->bxmaxs[1]) {
			if (!brush_has_a_north_face (b))
				return FACE_DIAGONAL_ADJACENT_ANY_19; // Diag

			return FACE_SOUTH_2; // b is south of a
		}
		if (a->bxmaxs[1] == b->bxmins[1]) {
			if (!brush_has_a_south_face (b))
				return FACE_DIAGONAL_ADJACENT_ANY_19; // Diag

			return FACE_NORTH_3; // b is north of a
		}
		return FACE_INVALID_NEG_1;
	}

	// How to eliminate the diag matches?
	// The wanted ones have a top face with 2 points of commonality if truly adjacent.

	return FACE_INVALID_NEG_1;
}
#endif // 0


CALLERS_ (WELD) // num_upwards_faces == 1 and num_flat_faces == 2 -- sounds like surface triangle?
brushrow_s *brush_get_face_facing_up_flat_only (brush_s *r_brush)
{
	brushrow_s *candidate = NULL;
	int num_upwards_faces = 0;
	int num_flat_faces = 0;

	for_each_brush_face (facenum, r_brush, r_face)
		qbool is_flat = face_is_flat(r_face);
		qbool is_up = face_is_up(r_face);
		
		if (is_flat) 
			num_flat_faces ++;

		if (is_up) {
			// This one is facing up.
			num_upwards_faces ++;
			candidate = r_face;
		}
	 for_each_end // for_each_brush_face

	if (num_upwards_faces == 1 && num_flat_faces == 2)
		return candidate;
	return NULL; // Multiple faces that are facing up.
}

int brush_find_cuboid_with_xyz_rightof(entityx_s *r_ent, const vec3_t bbmins, const vec3_t bbmaxs, int start_idx, int beyond_idx, int ignore_idx)
{
	for (int brush_idx = start_idx; brush_idx < beyond_idx; brush_idx ++) {
		brush_s *r_brush = &r_ent->brushlist.brush[brush_idx];

		if (brush_idx == ignore_idx)												continue; // IGNORE
		if (r_brush->is_considered_deleted)											continue;
		if (r_brush->brushrowlist.count != 6 /*rect 4 sides + top + bottom is 6*/)	continue;
		if (r_brush->is_a_patch)													continue;

		// We are assuming these are flat right now ...

		if (! (r_brush->bxmins[0] == bbmaxs[0] )) continue; // RIGHTOF MATCH!

		if (! (r_brush->bxmins[1] == bbmins[1] )) continue; // Not same Y min
		if (! (r_brush->bxmins[2] == bbmins[2] )) continue; // Not same Z min

		if (! (r_brush->bxmaxs[1] == bbmaxs[1] )) continue; // Not same Y max
		if (! (r_brush->bxmaxs[2] == bbmaxs[2] )) continue; // Not same Z max

		// full match
		return brush_idx;
	} // next brush.
	return not_found_neg1;
}

// Only returns triangular and rectangular brushes with a single upside
// Requires a flat bottom. -- Nov 12 2026 - but a flat top could trick this code.
// Requires real brush.
brushrow_s *brush_get_face_single_upward_tri_or_quad (brush_s *r_brush)
{
	if (!isin2 (r_brush->brushrowlist.count, 5, 6)) // Baker: this should excluded patches too.
		return NULL; // tri (3+ 2) or quad (4 + 2) are only eligible brush shapes

	brushrow_s *candidate = NULL;

	int num_upwards_faces = 0;
	int num_flat_faces = 0;

	for_each_brush_face (facenum, r_brush, r_face)
		// IDEA from Nov 13 2026 - didn't use
		//r_brush->is_terrain_wedge
		//if (false == VectorCompare (r_face->face_plane.normal, flat_ground_top_0_0_NEG1)) {
		//	continue;
		//}

		vec3_t pba;		VectorSubtract (r_face->b, r_face->a, pba);
		vec3_t pca;		VectorSubtract (r_face->c, r_face->a, pca);
		vec3_t cross;	CrossProduct (pba, pca, cross);

		qbool is_flatty = (r_face->a[2] == r_face->b[2] && r_face->b[2] == r_face->c[2]);

		if (is_flatty) num_flat_faces ++;

		if (cross[2] < 0) {
			// This one is facing up.
			num_upwards_faces ++;
			candidate = r_face;
		}
	for_each_end // for_each_brush_face

	if (num_upwards_faces == 1 && num_flat_faces >= 1)
		return candidate;
	return NULL; // Multiple faces that are facing up.

}


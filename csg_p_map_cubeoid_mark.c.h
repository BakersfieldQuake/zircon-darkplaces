// csg_p_map_cubeoid_mark.c.h

qbool _map_metric_triangle_5_sided_we_have_full_metric_available (brush_s *d_brush)
{
	if (BRUSH_IS_5_SIDED_TRI(d_brush) == false)
		return false;

	int num_flat_x = 0, num_flat_y = 0, num_flat_z = 0;


	d_brush->sloped_face_index = not_found_neg1;
	d_brush->top_slanted_index = not_found_neg1;


	for_each_brush_face (facenum, d_brush, d_face)
		// WEIRD CASES THE FACE MINS/MAXS ARE NOT EQUAL BRUSH MINS/MAX DUE
		// TO FLOATING POINT EPSILON -- DO NOT DO EQUALS AGAINST BRUSH
		if (d_face->bfdims[0] == 0)	{
			// EAST FACE SEEMS TO HAVE NORMAL OF -1,0,0
			// WEST FACE SEEMS TO HAVE NORMAL OF 1,0,0
			vec3_t normal_east = {-1,0,0};
			vec3_t normal_west = { 1,0,0};
			if (VectorCompare (d_face->face_plane.normal, normal_west)) {
				// Default west
				d_brush->cubeoid_index[FACE_WEST_0] = facenum; // We take west for now.
				d_brush->triangle_face_bits |= TEXSIDE_WEST_8; // MAYBE!
			} else if (VectorCompare (d_face->face_plane.normal, normal_east)) {
				// Examine
				//brushrow_s *face_w = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_WEST_0]];
				d_brush->cubeoid_index[FACE_EAST_1] = facenum; // WE ARE EAST because we aren't lower
				d_brush->triangle_face_bits |= TEXSIDE_EAST_4;
			} else {
				//int j = 5; // HITS? ===  NO IF I RECALL
			}
			//if (num_flat_x == 0) {
			//	// Default west
			//	d_brush->cubeoid_index[FACE_WEST_0] = facenum; // We take west for now.
			//	d_brush->triangle_face_bits |= TEXSIDE_WEST_8; // MAYBE!
			//} else {
			//	// Examine
			//	brushrow_s *face_w = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_WEST_0]];
			//	d_brush->triangle_face_bits |= TEXSIDE_EAST_4;
			//	if (d_face->bfmins[0] < face_w->bfmins[0]) {
			//		// This one is more west, TAKE WEST
			//		int losing_idx = d_brush->cubeoid_index[FACE_WEST_0];
			//		d_brush->cubeoid_index[FACE_WEST_0] = facenum;
			//		d_brush->cubeoid_index[FACE_EAST_1] = losing_idx;
			//	} else {
			//		// TAKE EAST
			//		d_brush->cubeoid_index[FACE_EAST_1] = facenum; // WE ARE EAST because we aren't lower
			//	}
			//}
			num_flat_x ++;
		} else if (d_face->bfdims[1] == 0)	{
			if (num_flat_y == 0) {
				// Default south
				d_brush->cubeoid_index[FACE_SOUTH_2] = facenum; // We take south for now.
				d_brush->triangle_face_bits |= TEXSIDE_SOUTH_2; // MAYBE!
			} else {
				// Examine
				brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_SOUTH_2]];
				d_brush->triangle_face_bits |= TEXSIDE_NORTH_1; // MAYBE!
				if (d_face->bfmins[1] < face_s->bfmins[1]) {
					// This one is more south, TAKE SOUTH
					int losing_idx = d_brush->cubeoid_index[FACE_SOUTH_2];
					d_brush->cubeoid_index[FACE_SOUTH_2] = facenum;
					d_brush->cubeoid_index[FACE_NORTH_3] = losing_idx;
				} else {
					// TAKE NORTH
					d_brush->cubeoid_index[FACE_NORTH_3] = facenum; // WE ARE NORTH because we aren't lower
				}
			}
			num_flat_y ++;
		} else if (d_face->bfdims[2] == 0)	{
			if (num_flat_z == 0) {
				// Default floor
				d_brush->cubeoid_index[FACE_FLOOR_4] = facenum; // We take floor for now.
				d_brush->triangle_face_bits |= TEXSIDE_FLOOR_32; // MAYBE!
			} else {
				// Examine
				brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_FLOOR_4]];
				d_brush->triangle_face_bits |= TEXSIDE_CEIL_16; // MAYBE!
				if (d_face->bfmins[2] < face_s->bfmins[2]) {
					// This one is more lower, TAKE FLOOR
					int losing_idx = d_brush->cubeoid_index[FACE_FLOOR_4];
					d_brush->cubeoid_index[FACE_FLOOR_4] = facenum;
					d_brush->cubeoid_index[FACE_CEIL_5] = losing_idx;
				} else {
					// TAKE CEIL
					d_brush->cubeoid_index[FACE_CEIL_5] = facenum; // WE ARE CEIL because we aren't lower
				}
			}
			num_flat_z ++;
		} else {
			// MONEY TIME
			if (d_brush->sloped_face_index == not_found_neg1) {
				d_brush->sloped_face_index = facenum; // Check the normal

			} else {
				// Can't have 2
				// Nov 5 2026 - this can be an angled top
				if (d_face->face_plane.normal[2] == 0)
					return false; // Vertical
				else {
					// Possibly a sloped ceiling.
				}

			}
		}
	for_each_end // for_each_brush_face

	if (d_brush->sloped_face_index == not_found_neg1)
		return false;

	//d_brush->top_slanted_index = not_found_neg1;
	// Find the "top"
//	int facey = -1;
//	ccs *texture = brush_texture_first_non_common (d_brush, &facey);
//	FACE_DEFINE_PTR (face0, d_brush, 0);
//	FACE_DEFINE_PTR (face1, d_brush, 1);
//	FACE_DEFINE_PTR (face2, d_brush, 2);
//	FACE_DEFINE_PTR (face3, d_brush, 3);
//	FACE_DEFINE_PTR (face4, d_brush, 4);
//	FACE_DEFINE_PTR (topface, d_brush, facey);
	for_each_brush_face (facenum, d_brush, d_face)
		int is_flat_top = VectorCompare(d_face->face_plane.normal, flat_ground_top_0_0_NEG1);
		int is_flat_toprev = VectorCompare(d_face->face_plane.normal, flat_ground_top_0_0_1_rev);

		if (FACE_IS_COMMON (d_face)) continue;

		if (d_face->face_plane.normal[2] == 0)
			continue; // vertical, right?
		if (is_flat_top)
			continue; // No flats
		if (is_flat_toprev)
			continue;
//		if (String_Contains (d_face->pbrtexture, "flat")) {
//			int j = 5;
//		}
		if (d_face->bfdims[2] == 0)
			continue; // Can't be z flat.

			// This face is sloped
		if (in_range_between (-1.0, d_face->face_plane.normal[2], 0)) {
			d_brush->top_slanted_index = facenum;
			d_brush->is_terrain_wedge = true;
			break;
		}
		if (in_range_between (0, d_face->face_plane.normal[2], 1)) {
			d_brush->top_slanted_index = facenum;
			d_brush->is_terrain_wedge = true;
			break;
		}
	for_each_end

	qbool is_tri_we_want =
		num_flat_z == 2	// Flat Z
		&& (num_flat_x + num_flat_y) == 2; // And 2 other flat sides .. the final side is odd.

	if (is_tri_we_want && d_brush->sloped_face_index != not_found_neg1) {
		// GOT ONE!  A FLAT TOP TRIANGLE.  NOT SLOPED
	} else {
		return false; // Some other type of slope that is not a "top".
	}

	// NOW ANOTHER WEIRD PART.
	brushrow_s *face_w = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_WEST_0]];
	brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_SOUTH_2]];
	brushrow_s *face_sloped = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[d_brush->sloped_face_index]];

	vec3_t centersloped;
	VectorCenter (face_sloped->bfmins, face_sloped->bfmaxs, centersloped);

	if ((d_brush->triangle_face_bits & (TEXSIDE_EAST_4 | TEXSIDE_WEST_8)) == TEXSIDE_WEST_8) {
		// THE "WEST" MIGHT BE EAST INSTEAD
		vec3_t centerwest;
		VectorCenter (face_w->bfmins, face_w->bfmaxs, centerwest);
		if (centerwest[0] > centersloped[0]) {
			// We are EAST NOT WEST!
			Flag_Remove_From (d_brush->triangle_face_bits, TEXSIDE_WEST_8);
			Flag_Add_To (d_brush->triangle_face_bits, TEXSIDE_EAST_4);
			d_brush->cubeoid_index[FACE_EAST_1] = d_brush->cubeoid_index[FACE_WEST_0];
			// HITS
		} else {
			// WEST IS CORRECT
			//int j = 5; // HITS
		}
	}
	if ((d_brush->triangle_face_bits & (TEXSIDE_NORTH_1 | TEXSIDE_SOUTH_2)) == TEXSIDE_SOUTH_2) {
		vec3_t centersouth;
		VectorCenter (face_s->bfmins, face_s->bfmaxs, centersouth);
		if (centersouth[1] > centersloped[1]) {
			// We are NORTH NOT SOUTH!
			Flag_Remove_From (d_brush->triangle_face_bits, TEXSIDE_SOUTH_2);
			Flag_Add_To (d_brush->triangle_face_bits, TEXSIDE_NORTH_1);
			d_brush->cubeoid_index[FACE_NORTH_3] = d_brush->cubeoid_index[FACE_SOUTH_2];
		} else {
			// SOUTH IS CORRECT
			//int j = 5;
		}
	} else {
		//int j = 5;
		// Unexpected!
	}

	RELATED_ (TEXSIDE_NORTHWEST_57 TEXSIDE_SOUTHEAST_54)
	//int j = d_brush->triangle_face_bits;

	return true;
}


RELATED_ (VM_map_entity_brush_add_cubeoid)
CALLERS_ (map_metric_cubeoid_mark_FULL_METRIC_AVAIL)
// FULL METRICS AVAILABLE!
qbool _map_metric_cubeoid_mark_brush_check_sub(brush_s *d_brush, int *p_noncubeoid_6_sider)
{
	if (BRUSH_IS_6_SIDED(d_brush) == false)
		return false;

	int num_flat_x = 0, num_flat_y = 0, num_flat_z = 0;

	for_each_brush_face (facenum, d_brush, d_face)
		// WEIRD CASES THE FACE MINS/MAXS ARE NOT EQUAL BRUSH MINS/MAX DUE
		// TO FLOATING POINT EPSILON -- DO NOT DO EQUALS AGAINST BRUSH
		if (d_face->bfdims[0] == 0)	{
			if (num_flat_x == 0) {
				// Default west
				RELATED_ (TEXSIDE_NORTHWEST_57  TEXSIDE_SOUTHWEST_58)
				d_brush->cubeoid_index[FACE_WEST_0] = facenum; // We take west for now.
				//d_brush->triangle_face_bits |= TEXSIDE_WEST_8; // MAYBE!
			} else {
				// Examine
				brushrow_s *face_w = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_WEST_0]];
				//d_brush->triangle_face_bits |= TEXSIDE_EAST_4;
				if (d_face->bfmins[0] < face_w->bfmins[0]) {
					// This one is more west, TAKE WEST
					int losing_idx = d_brush->cubeoid_index[FACE_WEST_0];
					d_brush->cubeoid_index[FACE_WEST_0] = facenum;
					d_brush->cubeoid_index[FACE_EAST_1] = losing_idx;
				} else {
					// TAKE EAST
					d_brush->cubeoid_index[FACE_EAST_1] = facenum; // WE ARE EAST because we aren't lower
				}
			}
			num_flat_x ++;
		} else if (d_face->bfdims[1] == 0)	{
			if (num_flat_y == 0) {
				// Default south
				d_brush->cubeoid_index[FACE_SOUTH_2] = facenum; // We take south for now.
			} else {
				// Examine
				brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_SOUTH_2]];
				if (d_face->bfmins[1] < face_s->bfmins[1]) {
					// This one is more south, TAKE SOUTH
					int losing_idx = d_brush->cubeoid_index[FACE_SOUTH_2];
					d_brush->cubeoid_index[FACE_SOUTH_2] = facenum;
					d_brush->cubeoid_index[FACE_NORTH_3] = losing_idx;
				} else {
					// TAKE NORTH
					d_brush->cubeoid_index[FACE_NORTH_3] = facenum; // WE ARE NORTH because we aren't lower
				}
			}
			num_flat_y ++;
		} else if (d_face->bfdims[2] == 0)	{
			if (num_flat_z == 0) {
				// Default floor
				d_brush->cubeoid_index[FACE_FLOOR_4] = facenum; // We take floor for now.
			} else {
				// Examine
				brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_FLOOR_4]];
				if (d_face->bfmins[2] < face_s->bfmins[2]) {
					// This one is more lower, TAKE FLOOR
					int losing_idx = d_brush->cubeoid_index[FACE_FLOOR_4];
					d_brush->cubeoid_index[FACE_FLOOR_4] = facenum;
					d_brush->cubeoid_index[FACE_CEIL_5] = losing_idx;
				} else {
					// TAKE CEIL
					d_brush->cubeoid_index[FACE_CEIL_5] = facenum; // WE ARE CEIL because we aren't lower
				}
			}
			num_flat_z ++;
		} else {
			//is_disqualified = true;
			if (p_noncubeoid_6_sider) {
				(*p_noncubeoid_6_sider) ++;
			}
			return false;
		}
	for_each_end // for_each_brush_face

	if (num_flat_x != 2 || num_flat_y != 2 || num_flat_z != 2) {
		if (p_noncubeoid_6_sider) {
			(*p_noncubeoid_6_sider) ++;
		}
		return false;
	}

	brushrow_s *face_w = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_WEST_0]];
	brushrow_s *face_e = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_EAST_1]];
	brushrow_s *face_s = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_SOUTH_2]];
	brushrow_s *face_n = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_NORTH_3]];
	brushrow_s *face_f = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_FLOOR_4]];
	brushrow_s *face_c = &d_brush->brushrowlist.brushrow[d_brush->cubeoid_index[FACE_CEIL_5]];

	if ( (face_w->bfdims[0] == 0 && face_w->bfmins[0] <= face_e->bfmins[0]) == false) { zpr ("Failed w");  }
	if ( (face_e->bfdims[0] == 0 && face_w->bfmins[0] <= face_e->bfmins[0]) == false) { zpr ("Failed e");  }
	if ( (face_s->bfdims[1] == 0 && face_n->bfmins[1] >= face_s->bfmins[1]) == false) {
		zpr ("Failed s");
	}
	if ( (face_n->bfdims[1] == 0 && face_n->bfmins[1] >= face_s->bfmins[1]) == false) { zpr ("Failed n");  }
	if ( (face_c->bfdims[2] == 0 && face_c->bfmins[2] >= face_f->bfmins[2]) == false) { zpr ("Failed c");  }
	if ( (face_f->bfdims[2] == 0 && face_c->bfmins[2] >= face_f->bfmins[2]) == false) { zpr ("Failed f");  }
	// TEXSIDE_SOUTHWEST_58
	return true;
}

// Must have run map_metric_brush
RELATED_ (map_metric_brush)
// This is where it normally happens
RELATED_ (map_metric_cubeoid_mark_FULL_METRIC_AVAIL)
void map_brush_cubeoid_remetric (map_t *hm, int entnum, int brushnum)
{
	BRUSH_DEFINE (brush, hm, entnum, brushnum);
	brush->is_cubeoid = false; // remetric clear
	qbool is_cubeoid = _map_metric_cubeoid_mark_brush_check_sub (brush, NULL);
	if (is_cubeoid) {
		brush->is_cubeoid = true; // SET
	}
}

void map_metric_brush_cubeoid (map_t *hm, int entnum, int brushnum)
{
	BRUSH_DEFINE (brush, hm, entnum, brushnum);
	brush->is_cubeoid = false;
	brush->is_terrain_wedge = false;
	brush->is_flat_half_cubeoid = false;
	qbool is_cubeoid = _map_metric_cubeoid_mark_brush_check_sub (brush, &hm->num_non_cubeoid_6_facer);
	if (is_cubeoid) {
		brush->is_cubeoid = true;
		hm->num_cubeoids ++;
	} else {
		// Maybe 5 sided triangle
//		if (entnum == 3 && brushnum == 0) {
//			int j=5;
//		}
		qbool is_tri_5 = _map_metric_triangle_5_sided_we_have_full_metric_available (brush);
		if (is_tri_5) {
			brush->is_flat_half_cubeoid = true;
		}
	}
}

// Mark brushes
CALLERS_ (map_load_from_string then we are called AFTER map_metric_all) // Means we have full metric available.
void map_metric_cubeoid_mark_FULL_METRIC_AVAIL (map_t *hm)
{
	for_each_entity_2 (entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)

		map_metric_brush_cubeoid (hm, entnum, brushnum);
//		//for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
//#ifdef _DEBUG
//			if (brushnum == 66) {
//				int k = 5;
//			}
//#endif // _DEBUG
//			qbool is_cubeoid = _map_metric_cubeoid_mark_brush_check_sub (d_brush, &hm->num_non_cubeoid_6_facer);
//			if (is_cubeoid) {
//				d_brush->is_cubeoid = true;
//				hm->num_cubeoids ++;
//			} else {
//				// Maybe 5 sided triangle
//				if (entnum == 3 && brushnum == 0) {
//					int j=5;
//				}
//				qbool is_tri_5 = _map_metric_triangle_5_sided_we_have_full_metric_available (d_brush);
//				if (is_tri_5) {
//					d_brush->is_flat_half_cubeoid = true;
//				}
//			}
		for_each_end // brush
	for_each_end // for_each_entity  entities that are "func_wall"
}

// csg_perform_nodes.c.h


// Find the next node for this one
static int FindNextTrackFaceEntNum (map_t *plist, int oentnum)
{
	map_get_vec3_dest_macro_DEFINE_AS_ (plist, oentnum, origin, master_origin);
	map_get_vec3_dest_macro_DEFINE_AS_ (plist, oentnum, forward, master_forward);

    int best = not_found_neg1;
    float bestDistance = 999999999.0f;

	// Look through the nodes
	for_each_entity_undeleted_startnum_with_classname(entnum, plist, dent, plist->original_ent_count, "node")
		// Now we need to find the next and previous.
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, origin);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, angles);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, forward);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, up);

		vec3_t origin_delta;
		VectorSubtract(origin, master_origin, origin_delta);

		float dist = VectorLength(origin_delta);

		if (dist <= 0.0f)
			continue;

		VectorNormalize(origin_delta);

		float alignment = DotProduct(origin_delta, master_forward);

		/*
		 * 1.0 = exactly forward
		 * 0.0 = 90 degrees to the side
		 * -1.0 = directly behind
		 */
		if (alignment < 0.7f)
			continue;

		/*
		 * Among nodes reasonably close to our forward direction,
		 * pick the closest one.
		 */
		if (dist < bestDistance) {
			bestDistance = dist;
			best = entnum;
		}
	for_each_end // each entity

	return best;
}

void GetFaceCenter(const vec3_t p1, const vec3_t p2, const vec3_t p3, vec3_t center)
{
    center[0] = (p2[0] + p3[0]) * 0.5f;
    center[1] = (p2[1] + p3[1]) * 0.5f;
    center[2] = (p2[2] + p3[2]) * 0.5f;
}

void Face4_SurfaceNormal(const vec3_t p1, const vec3_t p2, const vec3_t p3, vec3_t normal_out)
{
    // Two edges of the brush face
    vec3_t a = {
        p2[0] - p1[0],
        p2[1] - p1[1],
        p2[2] - p1[2]
    };

    vec3_t b = {
        p3[0] - p1[0],
        p3[1] - p1[1],
        p3[2] - p1[2]
    };

    // Cross product: A x B
    vec3_t normal = {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };

    // Normalize
    float length = sqrtf(
        normal[0] * normal[0] +
        normal[1] * normal[1] +
        normal[2] * normal[2]
    );

    if (length > 0.0f)
    {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
    }

	VectorCopyDestSrc (normal_out, normal);
}

void yaw_to_vector(float yaw, /*modify*/ vec3_t forward) {
    float radians = yaw * (M_PI / 180.0f);
    forward[0] = cosf(radians);
    forward[1] = sinf(radians);
    forward[2] = 0.0f;
}



void TextureForward_From_S (const brushrow_s *dface, /*modify*/ vec3_t forward)
{
    forward[0] = dface->tex_axis_s[0];// s[0];
    forward[1] = dface->tex_axis_s[1];
    forward[2] = dface->tex_axis_s[2];

    float length = sqrtf(
        forward[0] * forward[0] +
        forward[1] * forward[1] +
        forward[2] * forward[2]
    );

    if (length > 0.0f)
    {
        forward[0] /= length;
        forward[1] /= length;
        forward[2] /= length;
    }
}

//vec3_t Face4_SurfaceNormal(void) { vec3_t p1 = { 168.144f, 152.527f, 12.5f }; vec3_t p2 = { 184.144f, 180.240f, 12.5f }; vec3_t p3 = { 362.810f, 40.1362f, 12.5f }; // Two edges of the brush face vec3_t a = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z }; vec3_t b = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z }; // Cross product: A x B vec3_t normal = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; // Normalize float length = sqrtf( normal.x * normal.x + normal.y * normal.y + normal.z * normal.z ); if (length > 0.0f) { normal.x /= length; normal.y /= length; normal.z /= length; } return normal; }
// "_nodes_from_texture" "texturecan/wood_0047d_track texturecan/wood_0047d"

// Baker: In the future, we might have to have func_group ids to distinguish track sets.
// We could work it by track id in the func_groups.
// Wave into it.
// track_id should be a real field otherwise QuakeC cannot search it.
static int _csgw_nodes_build_this_track_sub (map_t *plist, ccs *tx_find0, ccs *tx_replace1, ccs *track_id_wanted)
{
	CSGM_START_
	int track_id_num = atoi(track_id_wanted);

	for_each_entity_original_count_undeleted (entnum, plist, d_ent)
		ccs *track_id = map_get (plist, entnum, "track_id"); // FUTURE
		if (false == String_Match_Nullproof (track_id, track_id_wanted)) continue;

		for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
			for_each_brush_face (facenum, d_brush, d_face)
				// DO EVERY VERTEX.EVEN CAULK
				//stringlistappend_face_vertex_unique (&list_vertex_strings, r_face);
				if (!d_face->pbrtexture) continue;
				if (!String_Match (d_face->pbrtexture, tx_find0)) continue;

				// Calc this one.
				// [ -0.5 -0.866025 0 -905.881 ]
				// ( 114.811 60.1509 12.5 ) ( 130.811 87.8638 12.5 ) ( 309.476 -52.2393 12.5 )
				//      texturecan/wood_0047d_track [ -0.5 -0.866025 0 -905.881 ] [ -0.866025 0.5 0 -436.487 ] 0 0.0171875 0.0125 134217728 0 0
				// vec4_t tex_axis_s;		// [ 0 1 0 0 ] // [ Ux Uy Uz Uoffset ]
				// Apparently all we need is the x information.

				float sx = d_face->tex_axis_s[0]; //  -0.5f;
				float sy = d_face->tex_axis_s[1]; //  -0.866025f;

				// Get texture S-axis angle in radians.
				float radians = atan2f(sy, sx);

				// Convert radians to degrees.
				//angle = angle * (180.0f / (float)M_PI);
				float angle = RAD2DEG(radians);
				// The brace runs opposite to the texture S axis.
				angle += 180.0f;

				// Normalize to 0..360 degrees.
				math_angleswrap_float (&angle);

				vec3_t center= {0};
				GetFaceCenter (d_face->a, d_face->b, d_face->c, center);
				vec3_t up = {0};
				Face4_SurfaceNormal (d_face->a, d_face->b, d_face->c, up);
				VectorNegate (up, up);

				vec3_t angles = {0};
				angles[1] = angle;

				// We need to get the forward
				vec3_t forward = {0,0,0};
				TextureForward_From_S (d_face, forward); // Get forward vector from texture

				VectorNegate (forward, forward);

				vec3_t forward_quake_angles = {0};
				AnglesFromVectors(forward_quake_angles, forward, up, q_flip_pitch_true);

				RELATED_ (SCR_sigmoid_f)

				//printf("Brace angle = %.3f degrees\n", angle);
				face_texture_set (d_face, tx_replace1);

				zpr ("Entity %4d Brush %4d Face %4d: angle is %3.3f", entnum, brushnum, facenum, angle);

				int entnum = map_add_entity_empty (plist);

				// Baker: LIFO
				map_set					(plist, entnum, "_nextnode", "");
				map_set_vec3_lossless	(plist, entnum, "up", up);
				map_set_vec3_lossless	(plist, entnum, "forward", forward); // Forward vector
				map_set_vec3_lossless	(plist, entnum, "angles", forward_quake_angles);
				map_set_vec3_lossless	(plist, entnum, "origin", center);
				map_set_fmt				(plist, entnum, "_targetname", "primnode%d_%d", track_id_num, _num_done);
				map_set					(plist, entnum, "prevnode", "");
				map_set					(plist, entnum, "nextnode", "");
				map_set					(plist, entnum, "targetname", "");
				map_set					(plist, entnum, "classname", "node"); // "info_player_start"

				CSGM_NUM_DONE_PLUS_
			for_each_end // next face
		for_each_end // next brush
	for_each_end // each entity

	for_each_entity_classname_startnum_2 (entnum, dent, plist, plist->original_ent_count, "node")
		// Now we need to find the next and previous.
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, origin);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, angles);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, forward);
		map_get_vec3_dest_macro_DEFINE_ (plist, entnum, up);
		int best_next = FindNextTrackFaceEntNum (plist, entnum);

		if (best_next == not_found_neg1) {
			zpr ("Best next not found for %d", entnum);
			continue;
		}
		ccs *sorigin = map_get(plist, entnum, "origin");
		ccs *name = map_get (plist, best_next, "_targetname");
		if (name) {
			map_set (plist, entnum, "_nextnode", name);
			zpr ("Forward look %d at to %d  at %s", entnum, best_next, sorigin);
		}
	for_each_end // each entity

	int lowy_entnum = map_find_low_y (plist, "node");
	ccs *starg = map_get (plist, lowy_entnum, "_targetname");
	ccs *snexty = map_get (plist, lowy_entnum, "_nextnode");
	ccs *sorg = map_get (plist, lowy_entnum, "origin");
	zpr ("Entity %d is start with _targetname " QUOTED_S " and _nextnode = " QUOTED_S " at %s",
		lowy_entnum, starg, snexty, sorg);

	int cur_entnum = lowy_entnum;
	int iters = 0;
	// Run the loop
	while (1) {
		map_set_fmt (plist, cur_entnum,  "targetname", "node%d_%d", track_id_num, iters + 0);
		ccs *targetname = map_get (plist, cur_entnum, "targetname");
		zpr ("Chaining entity # %d %s iters = %d", cur_entnum, targetname, iters);
		ccs *snext = map_get (plist, cur_entnum, "_nextnode");
		if (!snext) {
			zpr ("Entity %d has no _nextnode ending the chain.  End loop", cur_entnum);
			break;
		}

		ccs *scheck = map_get (plist, cur_entnum, "nextnode");
		if (scheck && scheck[0] != CHAR_NULL_0) {
			zpr ("Entity %d has nextnode set to " QUOTED_S " ending the chain.  End loop", cur_entnum, scheck);
			break;
		}

		int next_entnum = map_find(plist, "_targetname", snext);

		if (next_entnum == not_found_neg1) {
			zpr ("Entity %d cannot _targetname " QUOTED_S".  End loop.", cur_entnum, snext);
			break;
		}

		ccs *next_targetname = map_get (plist, next_entnum, "targetname");
		if (next_targetname && next_targetname[0])
			map_set_fmt (plist, cur_entnum, "nextnode", next_targetname); // Already named.
		else
			map_set_fmt (plist, cur_entnum, "nextnode", "node%d_%d", track_id_num, iters + 1);

		ccs *nextnode = map_get (plist, cur_entnum, "nextnode");
		map_set_fmt (plist, cur_entnum, "target", nextnode);

		zpr ("Chaining entity # %d (%s) to entity #%d (%s)", cur_entnum, targetname, next_entnum, nextnode);

		map_set_fmt (plist, next_entnum, "prevnode", "node%d_%d", track_id_num, iters + 0);
		cur_entnum = next_entnum;
		iters ++;
		if (cur_entnum == lowy_entnum) {
			zpr ("Hit starting entity %d again, End loop.", cur_entnum);
			break;
		}
	} // while

delete_useless_temp_keys:
; // obig
	int our_spawnflags_3 = SPAWNFLAGS_PATH_CORNER_FLY_POLE_1 + SPAWNFLAGS_PATH_CORNER_TRAIN_TURN_2;

	for_each_entity_undeleted_startnum_with_classname(entnum, plist, dent, plist->original_ent_count,"node")
		map_set			(plist, entnum, "classname", "path_corner");
		map_set_int32	(plist, entnum, "spawnflags", our_spawnflags_3); // FLYPOLE 1 + TRAIN TURN 2

		map_key_delete (plist, entnum, "_targetname");
		map_key_delete (plist, entnum, "_nextnode");
		ccs *snext = map_get(plist,entnum, "nextnode"); if (snext && snext[0] == CHAR_NULL_0) map_key_delete (plist, entnum, "nextnode");
		ccs *sprev = map_get(plist,entnum, "prevnode"); if (sprev && sprev[0] == CHAR_NULL_0) map_key_delete (plist, entnum, "prevnode");
	for_each_end

	CSGM_END_RETURN_ONLY_
}


// WORLD KEY:	_nodes_from_texture
// ENTITY KEY:	"track_id"
// CSG_CMW__NODES_FROM_TEXTURE
int csgw_nodes_from_texture (map_t *hm, ccs *MYKEY)
{
	CSGM_START_
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0 (_nodes_from_texture, MYKEY); // _nodes_from_texture

	char tx_find0[Q3PATHLENGTH_64], tx_replace1[Q3PATHLENGTH_64];
	space_split2 (_nodes_from_texture, tx_find0, tx_replace1);

	// Stamp early for simplicity for this one.
	CSGM_KEY_DELETE_WORLD_ (csgw_nodes_from_texture)

	stringlist_t list_track_ids = {0};

	// Accumulate a list of track_id values
	for_each_entity_with_key_original_2 (entnum, r_ent, hm, track_id, TRACK_ID)
		stringlistappend_unique_didAdd (&list_track_ids, track_id);
	for_each_end // each entity

	stringlistsort_unique (&list_track_ids);

	if (list_track_ids.numstrings == 0) {
		zpr (CON_RED "No track ids were found!");
	}

	for_each_stringlist_string (sidx, list_track_ids, this_track_id)
		int num_done_here = _csgw_nodes_build_this_track_sub (hm, tx_find0, tx_replace1, this_track_id);
		CSGM_NUM_DONE_PLUS_EQ (num_done_here);
	for_each_end // each entity
	stringlistfreecontents (&list_track_ids);

	CSGM_END_
}


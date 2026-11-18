// csg_p_map.c.h


// ONLY TRANSLATING THE ORIGIN KEY.
RELATED_ (map_entity_translate_with_metric map_afterworld_copy)
//void entity_translate_with_metric (entityx_s *d_ent, vec3_t move_amount_add)
//{
//
//	int num_moved = 0;
//
//#pragma message ("Won't this get weird if there is an origin brush?")
//#pragma message ("Does aafter have a rotating door anywhere?  Yes the trailers.")
//#pragma message ("Those seem ok but what function does it use?")
//
//	// Translate entity brushes.
//	for_each_brush_or_patch (brushnum, d_ent, d_brush)
//		brush_or_patch_tranlate_with_texturelock_was_processed (d_brush, move_amount_add);
//		num_moved ++;
//	for_each_end // for_each_brush_or_patch
//
//	RELATED_ (map_move_epairs_origin)
//	vec3_t origin;
//	qbool has_origin = entity_get_vec3 (d_ent, "origin", origin);
//
//	// ADJUST ORIGIN KEY IF IT HAS ONE
//	if (has_origin) {
//		VectorAdd (move_amount_add, origin, origin);
//		entity_set_vec3_lossess (d_ent, "origin", origin);
//	}
//
//	map_metric_entity_sub (hm, d_ent);
//}
//

// SIGMOID ONLY
void map_rotate_brush_fix_yaw (map_t *hm, const vec3_t move0, const vec3_t rotate0)
{
	// BEWARE ... 
	// Ok not sure why rotate on X.  But Q3MAP2 .map is not same as Quake.
	Vector3DeclareCopyEx(move, move0);
	Vector3DeclareCopyEx(rotate, rotate0);
	rotate[0] = 360 - rotate[0]; // Inverse pitch
	for_each_entity_2 (entnum, ent, hm)
		map_rotate_entity (hm, entnum, move, rotate, NULL);
	for_each_end // for_each_entity
}

//RELATED_ (brushface_process_translate_rotate brushface_process_translate_rotate2) // Deeper
//void map_brushes_translate_rotate_2_unused (map_t *hm, const vec3_t move, const vec3_t rotate)
//{
//	for_each_entity_2 (entnum, ent, hm)
//		map_rotate_entity (hm, entnum, move, rotate);
//	for_each_end // for_each_entity
//}



void map_entity_translate_with_metric (map_t *hm, int entnum, vec3_t move_amount_add)
{
	ENT_DEFINE (d_ent, hm, entnum);
	
	int num_moved = 0;

	#pragma message ("Won't this get weird if there is an origin brush?")
	#pragma message ("Does aafter have a rotating door anywhere?  Yes the trailers.")
	#pragma message ("Those seem ok but what function does it use?")

	// Translate entity brushes.
	for_each_brush_or_patch_2 (brushnum, d_brush, hm, entnum)
		brush_or_patch_tranlate_with_texturelock_was_processed (d_brush, move_amount_add);
		num_moved ++;
	for_each_end // for_each_brush_or_patch

	RELATED_ (map_move_epairs_origin)
	vec3_t origin;
	qbool has_origin = entity_get_vec3 (d_ent, "origin", origin);

	// ADJUST ORIGIN KEY IF IT HAS ONE
	if (has_origin) {
		VectorAdd (move_amount_add, origin, origin);
		entity_set_vec3_lossess (d_ent, "origin", origin);
	}

	map_metric_entity_sub (hm, entnum);
}

CALLERS_ (MazeGen only mid-2025)
entityx_s *entitylist_add_pasting_this_ent (map_t *plist, const entityx_s *r_ent /*paste source*/) // June 8 2025
{
	entityx_s	*copyent = entitylist_add(plist);	// D = DEST

	VectorCopyDestSrc (copyent->bemins, r_ent->bemins);
	VectorCopyDestSrc (copyent->bemaxs, r_ent->bemaxs);
	VectorCopyDestSrc (copyent->bedims, r_ent->bedims);

	for_each_key_val_pair (keyidx, r_ent, key, val)
		stringlistappend (&copyent->pairslist, key); // KEY
		stringlistappend (&copyent->pairslist, val); // VALUE
	for_each_end // key val pairs

	for_each_const_brush (brushnum, r_ent, r_brush)					// READ BRUSH FROM SRC
		brush_s *copybrush = _LIFE_brush_add (&copyent->brushlist); // CREATE BRUSH FOR DEST

		copybrush->is_a_patch = r_brush->is_a_patch;

		VectorCopyDestSrc (copybrush->bxmins, r_brush->bxmins);
		VectorCopyDestSrc (copybrush->bxmaxs, r_brush->bxmaxs);
		VectorCopyDestSrc (copybrush->bxdims, r_brush->bxdims);

		if (r_brush->is_a_patch) {
			// PATCH
			copybrush->patch_texture = Z_StrDup (r_brush->patch_texture);
			copybrush->patch_rows = r_brush->patch_rows;
			copybrush->patch_cols = r_brush->patch_cols;
			copybrush->other3[0] = r_brush->other3[0];
			copybrush->other3[1] = r_brush->other3[1];
			copybrush->other3[2] = r_brush->other3[2];

			for_each_patch_row (rownum, r_brush, r_patchrow)
				patchrow_s *copypatch = patchrowlist_add (&copybrush->patchrowlist);

				// 5 x number of columns - mem copy all the numbers
				size_t size_to_copy = sizeof(copypatch->flots); // Should be about 1200 bytes
				memcpy (copypatch->flots, r_patchrow->flots, size_to_copy);
			for_each_end // patch_row
		} else {
			// REAL BRUSH
			for_each_const_brush_face (facenum, r_brush, r_face)
				brushrow_s *copyface = _LIFE_brushface_alloc(&copybrush->brushrowlist);

				memcpy (copyface, r_face, sizeof(*r_face));
				copyface->pbrtexture = Z_StrDup (r_face->pbrtexture);

				VectorCopyDestSrc (copyface->bfmins, r_face->bfmins);
				VectorCopyDestSrc (copyface->bfmaxs, r_face->bfmaxs);
				VectorCopyDestSrc (copyface->bfdims, r_face->bfdims);
			for_each_end // for_each_brush_face
		} // if brush
	for_each_end // for_each_brush_or_patch
	return copyent;
}

// Baker: The entity here is a func_group like "func_group_wall_north".
// We expect a box.
// MAZEGEN ALPHA BLENDED BRUSHES
void entity_add_alpha_brushes (entityx_s *d_ent, vec3_t worldmins, vec3_t worldmaxs, vec3_t worldsize)
{
	for (int corner = 0; corner < 8; corner ++) { // Do all 8 corners.
		qbool xhi = (corner & 1) ? true : false;
		qbool yhi = (corner & 2) ? true : false;
		qbool zhi = (corner & 4) ? true : false;

		vec3_t brmins, brmaxs, point3d;
		point3d[0] = xhi ? d_ent->bemaxs[0] : d_ent->bemins[0];
		point3d[1] = yhi ? d_ent->bemaxs[1] : d_ent->bemins[1];
		point3d[2] = zhi ? d_ent->bemaxs[2] : d_ent->bemins[2];

		VectorCopyDestSrc (brmins, point3d);
		VectorCopyDestSrc (brmaxs, point3d);

		for (int d = 0; d < 3; d ++) {
			brmins[d] -= 48;
			brmaxs[d] += 48;
		} // for

		float f = (point3d[0] - worldmins[0])/worldsize[0];
		f = bound (0, f, 1);
		va_super (texturename, 128, "common/alpha_%1.2f", f);
		String_Edit_Replace (texturename, sizeof(texturename), ".", "_");
		brush_add_cubeoid_with_texture (d_ent, brmins, brmaxs, texturename);
		// %1.3f 0
	}

}

CALLERS_ (csge_keep_percent) // This entity failed a DICE ROLL
qbool entitylist_remove_odds_entity (map_t *plist, entityx_s *e)
{
	// REMOVE!  ROLL FAILED!
	e->is_considered_deleted = true;

	ccs *s_odds_children_branch_name = entity_get (e, "_odds_children_branch_name");

	if (s_odds_children_branch_name) {
		for_each_entity_original_count_undeleted_startnum(entnum2, plist, edelete, AFTER_WORLD_1)
			ccs *odds_branch_name = entity_get (edelete, "_odds_branch_name");
			if (false == String_Match_Nullproof (odds_branch_name, s_odds_children_branch_name))
				continue; // NO NAME MATCH - SO NOT A CHILD

			// CHILD OF DELETED ENTITY -- REMOVE IT
			edelete->is_considered_deleted = true;
		for_each_end // children of branch
	} // if branch name
	return true;
}



RELATED_ (csgc_rotateunit)


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float _face_get_yaw_rotation(const brushrow_s *myface) {
    // tex_axis_s is a vec4_t structured as: [Ux, Uy, Uz, Uoffset]
    // For a top face (aligned with the XY plane), the S texture axis 
    // direction projected onto the XY plane gives the rotation/yaw.
    double x = myface->tex_axis_s[0];
    double y = myface->tex_axis_s[1];

    // Compute angle using atan2 and convert from radians to degrees
    double angle_deg = atan2(y, x) * (180.0 / M_PI);

    // Normalize to [0, 360) range if desired
    if (angle_deg < 0.0) {
        angle_deg += 360.0;
    }

    return (float)angle_deg;
}

RELATED_ (map_brushes_translate_rotate)
qbool map_brush_get_rotation_is_ok (map_t *hm, int entnum, int brushnum, float *yaw_degrees_out)
{
	if (in_range_beyond(0, entnum, hm->count) == false) {
		zpr ("Entity %d not in range, max is %d", entnum, hm->count);
		return false;
	} // out range

	int entbrushcount = BRUSH_COUNT (hm, entnum);

	if (brushnum != not_found_neg1 && in_range_beyond(0, brushnum, entbrushcount) == false) {
		zpr ("Brushnum %d:%d not in range, max is %d", entnum, brushnum, entbrushcount);
		return false;
	} // out range

	if (brushnum == not_found_neg1) {
		// ENTITY
		if (entbrushcount == 0) {
			zpr ("Entity %d has no brushes", entnum);
			return false;
		}
		brushnum = 0;

	}

	BRUSH_DEFINE_CONST (brush, hm, entnum, brushnum);
	if (brush->is_cubeoid == false) {
		zpr ("Brush %d:%d is not cubeoid", entnum, brushnum);
		return false;
	}

	FACE_DEFINE_CONST (face, hm, entnum, brushnum, FACE_CEIL_5);
	(*yaw_degrees_out) = _face_get_yaw_rotation (face);
	return true;
}
















RELATED_ (String_Worldspawn_Value_For_Key_Sbuf)

RELATED_ (VM_CL_project)



CALLERS_ (csgc_delete_world_brushes_with_nondominant_texture, entitylist_perform_weld_num_done)
void stringlist_append_entity_textures (stringlist_t *plist, const entityx_s *r_ent)
{
	for_each_brush_or_patch (brushnum, r_ent, d_brush)
		for_each_brush_face (facenum, d_brush, d_face)
			stringlistappend (plist, d_face->pbrtexture);
		for_each_end // face
	for_each_end // brush
}

numchanged_int map_fieldname_prefix (map_t *hm, ccs *fieldname, ccs *prefix) // door1 --> $PREFIX_door1
{
	int num_changed = 0;
	for_each_entity_undeleted (entnum, hm, d_ent)
		for_each_key_val_pair (keyidx, d_ent, key, val)
			if (false == String_Match (key, fieldname))
				continue;

			Z_StrDupf_Realloc (&d_ent->pairslist.strings[keyidx + 1], "%s%s", prefix, val);
			num_changed ++;
		for_each_end // key val pairs
	for_each_end // for_each_entity
	return num_changed;
}


ccs *map_get (const map_t *hm, int entnum, ccs *keyname)
{
	return entity_get(ITOE(hm, entnum), keyname); // SEPT12026PASS
}

qbool map_get_vec3 (const map_t *hm, int entnum, ccs *key, vec3_t vout)
{
	VectorClear (vout);

	ccs *sval = entity_get(ITOE(hm, entnum), key); // SEPT12026PASS
	if (sval) {
		Math_atov3 (sval, vout);
		return true;
	}

	return false;
}

entnum_int map_entity_find_start_after (map_t *hm, ccs *key, ccs *value, int start_after)
{
	for_each_entity_undeleted_start_after(entnum,hm,dent,start_after)
		ccs *this_value = map_get (hm, entnum, key);
		if (!this_value) continue; // Does not have a classname
		if (false == String_Match (this_value, value)) continue; // No match

		return entnum; // FOUND
	for_each_end
	return not_found_neg1;
}

CALLERS_ (csgw_nodes_build_this_track)
int map_entity_find_start_after_lowy (map_t *hm, ccs *find_classname, int start_after)
{
	//float best_x = 999999;
	float best_y = 999999;
	int best_idx = not_found_neg1;
	for_each_entity_undeleted_start_after(entnum,hm,dent,start_after)
		ccs *classname = map_get (hm, entnum, "classname");
		if (!classname || false == String_Match (classname, find_classname/* "node"*/))
			continue;
		// Now we need to find the next and previous.
		map_get_vec3_dest_macro_DEFINE_ (hm, entnum, origin);
		if (origin[1] < best_y) {
			best_y = origin[1];
			best_idx = entnum;
		}
	for_each_end
	return best_idx;
}

void map_move (map_t *hm, vec3_t move_amount)
{
	map_move_epairs_origin	(hm, move_amount);
	map_move_all_brushes_patches	(hm, move_amount); // Has texture scaling.
}

CALLERS_ (VM_map_append_map map_get)
static void _map_afterworld_copy (map_t *hm, map_t *plist_paste_from)
{
	// ADD PASTE ENTS TO PLIST
	for_each_entity_const_afterworld_2 (entnum, r_ent, plist_paste_from)
	//for_each_entity_after_world_undeleted (entnum, plist_paste_from, r_ent)
		entityx_s	*copy_entity = entitylist_add(hm);	// D = DEST

		for_each_key_val_pair (keyidx, r_ent, key, val)
			stringlistappend (&copy_entity->pairslist, key); // KEY
			stringlistappend (&copy_entity->pairslist, val); // VALUE
		for_each_end // key val pairs

		for_each_brush_or_patch_const_2 (brushnum, r_brush, plist_paste_from, entnum)
		//for_each_const_brush (brushnum, r_ent, r_brush)
			brush_s *copy_brush = _LIFE_brush_add (&copy_entity->brushlist); // CREATE BRUSH FOR DEST

			copy_brush->is_a_patch = r_brush->is_a_patch;

			if (r_brush->is_a_patch) {
				// If patch, copy patch stuff ...
				copy_brush->patch_texture = Z_StrDup (r_brush->patch_texture);
				copy_brush->patch_rows = r_brush->patch_rows;
				copy_brush->patch_cols = r_brush->patch_cols;
				copy_brush->other3[0] = r_brush->other3[0];
				copy_brush->other3[1] = r_brush->other3[1];
				copy_brush->other3[2] = r_brush->other3[2];
			}

			if (r_brush->is_a_patch == false) {
				// REAL BRUSH
				for_each_const_brush_face (facenum, r_brush, r_face)
					brushrow_s *copy_face = _LIFE_brushface_alloc(&copy_brush->brushrowlist);

					memcpy (copy_face, r_face, sizeof(*r_face));
					copy_face->pbrtexture = Z_StrDup (r_face->pbrtexture);
				for_each_end // for_each_brush_face
			} else {
				// PATCH
				for_each_const_patch_row (rownum, r_brush, r_patchrow)
					patchrow_s *copy_patchrow = patchrowlist_add (&copy_brush->patchrowlist);

					// 5 x number of columns - mem copy all the numbers
					size_t size_to_copy = sizeof(copy_patchrow->flots); // Should be about 1200
					memcpy (copy_patchrow->flots, r_patchrow->flots, size_to_copy);
				}
			} // if brush
		for_each_end // for_each_brush_or_patch
	} // entities in .map
}

void map_append_map (map_t *dst, map_t *src)
{
	map_world_brushes_patches_copy	(dst, src);	// no keys, just brushes.
	_map_afterworld_copy			(dst, src);	// brushes and keys.
}

void map_append_map_with_move (map_t *dst_map, map_t *src_map, vec3_t translate_move)
{
	// Do we have a rotating door problem?  translate the origin and the brush, that's double.
	// The only way we can check is looking for common/origin texture name

	// If origin brush entity (per entity basis)
	// Do not translate the brushes.

	map_move_epairs_origin		(src_map, translate_move);
	map_move_all_brushes_patches		(src_map, translate_move);
	map_append_map				(dst_map, src_map);
}

// same classname and same targetname == nth
int map_get_nthness_classnametargetname(map_t *plist, int my_entnum)
{
	entityx_s *efind = &plist->entity[my_entnum];

	int nthness = 0;

	ccs *efind_classname = map_get (plist, my_entnum, "classname");
	if (!efind_classname) return not_found_neg1;

	ccs *efind_targetname = map_get (plist, my_entnum, "targetname");
	if (!efind_targetname) return not_found_neg1;

	for_each_entity_original_even_deleted_ones_startnum (entnum, plist, e, AFTER_WORLD_1)
		// REQUIRE CLASSNAME AND TARGETNAME MATCH
		if (false == map_get_keyval_does_match (plist, entnum, "classname", efind_classname)) continue;
		if (false == map_get_keyval_does_match (plist, entnum, "targetname", efind_targetname)) continue;

		nthness ++; // Number of ones that are same classname and targetname

		if (e == efind) {
			Con_PrintLinef ("Entity %d %s %s: nthness == %d", my_entnum, efind_classname, efind_targetname, nthness);
			return nthness;
		} // e == efind
	for_each_end // original entities even undeleted in .map

	Con_PrintLinef ("Entity %d %s %s: nthness == " CON_RED "NOT FOUND", my_entnum, efind_classname, efind_targetname);
	return not_found_neg1;
}


CALLERS_ (VM_map_texture_replace)
numchanged_int map_texture_replace (map_t *hm, ccs *findtexturename, ccs *newtexturename)
{
	CSGM_START_
	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_or_patch_2 (brushnum, d_brush, hm, entnum)
			for_each_face_with_texture_2 (facenum, d_face, hm, entnum, brushnum, findtexturename)
				face_texture_set (d_face, newtexturename);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity

	CSGM_END_RETURN_ONLY_
}



CALLERS_ (csgw_rename_GARUX) // exclusive

int map_afterworld_classname_rename_all (map_t *hm, ccs *oldclassname, ccs *newclassname)
{
	CSGM_START_
	for_each_entity_classname_afterworld_original_2 (entnum, d_ent, hm, oldclassname)
		map_set (hm, entnum, "classname", newclassname);
		CSGM_NUM_DONE_PLUS_
	for_each_end // each entity after world

	CSGM_END_FUNC_
}

int map_prefix_dungeon_targetnames (map_t *hm, ccs *prefix)
{
	// target and targetname
	int num_changed0 = map_fieldname_prefix(hm, "target", prefix);
	int num_changed1 = map_fieldname_prefix(hm, "targetname", prefix);
	int num_changed2 = map_fieldname_prefix(hm, "door_outline", prefix);
	int num_changed = num_changed0 + num_changed1 + num_changed2;
	return num_changed;
}

int map_prefix_dungeon_targetnamesf (map_t *hm, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt); // Create the supplied prefix like "_block%d_", cell_idx
	int num_changed = map_prefix_dungeon_targetnames (hm, /*prefix*/ text);
	VA_EXPAND_ALLOC_FREE (text);
	return num_changed;
}

RELATED_ (Generate_Dungeon_Map_File VM_map_prefix_fields_values map_include)
int map_prefix_standard_targetnames (map_t *hm, ccs *prefix)
{
	// target and targetname
	int num_changed0 = map_fieldname_prefix(hm, "target", prefix);
	int num_changed1 = map_fieldname_prefix(hm, "targetname", prefix);
	int num_changed = num_changed0 + num_changed1;
	return num_changed;
}

// "target" and "targetname" are prefixed.
int map_prefix_standard_targetnamesf (map_t *hm, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt); // Create the supplied prefix like "_block%d_", cell_idx
	int num_changed = map_prefix_standard_targetnames (hm, /*prefix*/ text);
	VA_EXPAND_ALLOC_FREE (text);
	return num_changed;
} // NEW



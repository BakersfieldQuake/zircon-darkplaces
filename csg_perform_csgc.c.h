// csg_perform_csgc.c.h

// Baker: This never has brushes.
// Baker: Our missing here is adding the entities to the map, nothing more ...
RELATED_ (VM_map_load)
CALLERS_ (csqc_add_point_entities_on_clipboard) // Sept 27 2026
static int _entitylist_parse_string_as_map_then_concat_ents (map_t *pe_dest, ccs *clipboard_string_of_entities)
{
	int ret_is_ok = false;
	map_t list_concat = {0};

	qbool is_parse_ok = map_load_from_string (&list_concat, clipboard_string_of_entities);
	CMD_FAILEXIT_IF_ (!is_parse_ok, "Couldn't parse supplied text")

	// CONCAT
	map_paste_point_entities (pe_dest, &list_concat);

	ret_is_ok = true;

fail_exit:
	map_freecontents (&list_concat); // free data

	return ret_is_ok;
}

void sub_print_keys_quick (entityx_s *r_ent)
{
	// Print the world
	for_each_key_val_pair (keyidx, r_ent, key, val)
		if (key[0] == CHAR_NULL_0) continue; // Do not print "deleted" keys
		zpr (S_FMT_LEFT_PAD_25 " " S_FMT_LEFT_PAD_25, key, val);
	for_each_end // key val pairs
}


// Adds entities from the clipboard to the map
int csgc_add_point_entities_on_clipboard (map_t *hm)
{
	int ret_is_ok = false;
	ccs *clip_za = Clipboard_Get_Text_ZAlloc();

	CMD_FAILEXIT_IF_ (!clip_za || strlen(clip_za) == 0, "No text on clipboard")

	ret_is_ok = _entitylist_parse_string_as_map_then_concat_ents (hm, clip_za);

fail_exit:
	Mem_FreeNull_ (clip_za);

	return ret_is_ok;
}



// Baker: July 5 2026 - Removes the brushes with the most dominant texture in the .map.  No command line csg version.
// Baker: This was used in 2025 to remove brushes from aafter and probably children which eliminated
// a lot of lightmap storage
int csgc_brush_delete_if_nondom (map_t *hm)
{
	CSGM_START_

	char most_common_texturename[Q3PATHLENGTH_64] = {0};
	{
		stringlist_t list_textures = {0};
		stringlist_append_entity_textures (&list_textures, ITOE(hm, WORLD_0));
		ccs *most_common = stringlist_find_most_common_index_ignoring_this (&list_textures, "common/caulk");
		dp_strlcpy (most_common_texturename, most_common, sizeof(most_common_texturename));
		stringlistfreecontents (&list_textures);
	}

	zpr ("World: Most common texture: %s", most_common_texturename);

	for_each_brush_2 (brushnum, d_brush, hm, WORLD_0)
		if (false == brush_has_texturename(d_brush, most_common_texturename))
			continue; // Doesn't have this texture

		// HAS TEXTURE -- DELETE THE BRUSH
		d_brush->is_considered_deleted = true;
		CSGM_NUM_DONE_PLUS_
	for_each_end // next brush

	CSGM_END_FUNC_ // return
}

int csgc_brush_delete_if_texture (map_t *hm, ccs *texturename)
{
	CSGM_START_
	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			if (false == brush_has_texturename(d_brush, texturename))
				continue; // Doesn't have this texture

			// HAS TEXTURE -- DELETE THE BRUSH
			d_brush->is_considered_deleted = true;
			CSGM_NUM_DONE_PLUS_
		for_each_end // next brush
	for_each_end // next entity
	CSGM_END_FUNC_ // return
}

// All upwards facing brushes get texture "common/up"
int csgc_caulk_everything (map_t *hm)
{
	CSGM_START_

	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face (facenum, d_brush, d_face)
				face_texture_set (d_face, COMMON_CAULK);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // after_world

	CSGM_END_FUNC_
}

int csgc_brushfacer_ex (map_t *hm, ccs *texturename, float *p_z_required)
{
	CSGM_START_

	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face (facenum, d_brush, d_face)
				qbool is_up_face = face_is_up (d_face);
				if (is_up_face == false) continue; // Skip non-up faces

				if (p_z_required) {
					int z_required = (*p_z_required);
					if (false == VectorCompare (d_face->face_plane.normal, flat_ground_top_0_0_NEG1))
						continue; // We require flat ground surface faces
					// ONLY
					if (d_face->bfmaxs[2] != z_required)
						continue; // Wrong z.
				}

				// UP
				face_texture_set (d_face, texturename);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // after_world

	CSGM_END_FUNC_
}

int csgc_brushfacer (map_t *hm)
{
	return csgc_brushfacer_ex (hm, COMMON_UP, NULL);
}

// All upwards facing brushes get texture "textures/up"
// texturecan/wood_0047d
int csgc_brushfacer_specific_texture (map_t *hm, ccs *texturename)
{
	CSGM_START_
	for_each_entity_2(entnum,d_ent,hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_face_with_texture_2 (facenum, d_face, hm, entnum, brushnum, texturename)
				if (face_is_up (d_face) == false) continue; // WALL OR UPSIDE DOWN == NO

				// UP FACE WITH TEXTURE MATCH ==> SET "common/up"
				face_texture_set (d_face, COMMON_UP);
				CSGM_NUM_DONE_PLUS_
			for_each_end // next face
		for_each_end // next brush
	for_each_end // next entity

	CSGM_END_FUNC_
}




int csgc_brushunfacer (map_t *hm)
{
	CSGM_START_

	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face (facenum, d_brush, d_face)
				if (face_is_up (d_face)) continue; // Skip up faces

				// DOWN FACING
				face_texture_set (d_face, COMMON_CAULK);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // after_world

	CSGM_END_FUNC_
}

// SUPPLY A CLASSNAME LIKE "func_group"
// YOUR_CLASSNAME: CAULK all Faces That Are NOT UP -- DOWN AND VERTICAL -- OPPOSITE SET
// VERTICAL IS VERY IMPORTANT -- Think of those invisible terrain triangles that are vertical
int csgc_brushunfacer_specific_classname (map_t *hm, ccs *classname)
{
	CSGM_START_

	for_each_entity_classname_2 (entnum, d_ent, hm, classname)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face_2 (facenum, d_face, hm, entnum, brushnum)
				if (face_is_up (d_face)) continue; // UP faces are ignored

				face_texture_set (d_face, COMMON_CAULK);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity

	CSGM_END_FUNC_
}




// Not sky
int csgc_caulk_non_common_non_sky_containing_textures (map_t *plist)
{
	CSGM_START_
	for_each_entity_only_world (worldentnum, plist, d_ent)
		// We don't want to do light emitting ones ... but how common is that?
		for_each_brush_or_patch (brushnum, d_ent, d_brush)
			for_each_brush_face (facenum, d_brush, d_face)
				if (FACE_IS_COMMON (d_face))
					continue; // Don't do common

				if (String_Contains(d_face->pbrtexture, COMMON_SKY_LIAR)) // "sky"
					continue; // Don't do sky

				face_texture_set (d_face, COMMON_CAULK);
				CSGM_NUM_DONE_PLUS_
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // only world
	CSGM_END_FUNC_
}

// Remove all instances of a classname like "func_group_floor" -- mazegen etc.
int csgc_classname_delete (map_t *hm, ccs *classname)
{
	CSGM_START_
	if (classname[0] == CHAR_NULL_0) {
		zpr ("No classname was provided.  Ignored.");
		return 0;
	}

	for_each_entity_classname_2 (entnum, d_ent, hm, classname)
		d_ent->is_considered_deleted = true;
		CSGM_NUM_DONE_PLUS_
	for_each_end // for_each_entity
	CSGM_END_FUNC_
}


RELATED_ (CSG_DELETE_WORLDSPAWN_KEY)
int csgc_worldspawn_key_delete (map_t *hm, ccs *key)
{
	CSGM_START_

	ENT_DEFINE (d_world, hm, WORLD_0);

	map_key_delete (hm, WORLD_0, key); // NOT CSG CMDLINE -- move along
	CSGM_NUM_DONE_PLUS_

	sub_print_keys_quick (d_world);

	CSGM_END_FUNC_
}


int csgc_csg_run (map_t *hm, ccs *map_filename)
{
	CSGM_START_
	int n = 0;
	int num_done_here = 0;

	char *map_realpath_za = FS_Get_RealFile_Realpath_Safe_ZAlloc (map_filename);

		#define DEF_(csg_console_name, func, description, long_explain) // Nada
		#define Q3MAP_PRX(MYSTRING,MYFUNC,EXAMPLE,DESCRIBE,WORLD_OR_ENTITY_TYPE,APPLIES_TO) \
			n ++; \
			if (num_done_here) { \
				/*outstanding changes*/ \
				map_reload (hm); \
				num_done_here = 0; \
				zpr ("Reloaded."); \
			} \
			num_done_here = MYFUNC(hm, MYSTRING, map_realpath_za); \
			zpr ("%4d: %-30.30s num_done_here = %d", \
				n, MYSTRING, num_done_here); \
			if (num_done_here) { \
				_num_done += num_done_here; \
			} // Ender

		#define Q3MAP_PRE(MYSTRING,MYFUNC,EXAMPLE,DESCRIBE,WORLD_OR_ENTITY_TYPE,APPLIES_TO) \
			n ++; \
			if (num_done_here) { \
				/*outstanding changes*/ \
				map_reload (hm); \
				num_done_here = 0; \
				zpr ("Reloaded."); \
			} \
			num_done_here = MYFUNC(hm, MYSTRING); \
			zpr ("%4d: %-30.30s num_done_here = %d", \
				n, MYSTRING, num_done_here); \
			if (num_done_here) { \
				_num_done += num_done_here; \
			} // Ender

		#include "csg_table.h"

	Mem_FreeNull_ (map_realpath_za);
	CSGM_END_FUNC_
}


// Baker: July 5 2026 - Removes the brushes with the most dominant texture in the .map.  No command line csg version.
// Baker: This was used in 2025 to remove brushes from aafter and probably children which eliminated
// a lot of lightmap storage

void map_texture_dominant_non_common (map_t *hm, char *sbuf, size_t sizeof_sbuf)
{
	ENT_DEFINE (d_world, hm, WORLD_0);

	stringlist_t list_world_textures = {0};
	stringlist_append_entity_textures (&list_world_textures, d_world);
	ccs *most_common_texturename =
		stringlist_find_most_common_index_ignoring_this (&list_world_textures, COMMON_CAULK);

	if (most_common_texturename)
		dp_strlcpy (sbuf, most_common_texturename, sizeof_sbuf);

	stringlistfreecontents (&list_world_textures);
}

int csgc_delete_world_brushes_with_nondominant_texture (map_t *hm)
{
	CSGM_START_
	//ENT_DEFINE (d_world, hm, WORLD_0);

	//stringlist_t list_world_textures = {0};
	//stringlist_append_entity_textures (&list_world_textures, d_world);

	//ccs *most_common_texturename =
	//	stringlist_find_most_common_index_ignoring_this (&list_world_textures, COMMON_CAULK);

	char most_common_texturename[Q3PATHLENGTH_64] = {0};
	map_texture_dominant_non_common (hm, most_common_texturename, sizeof(most_common_texturename));

	zpr ("Dominant texture is %s", most_common_texturename);
	zpr ("Removing brushes from world not using dominant texture ...");

	for_each_brush_2 (brushnum, d_brush, hm, WORLD_0)
		if (brush_has_texturename (d_brush, most_common_texturename)) continue; // Keep dominants

		d_brush->is_considered_deleted = true;
		CSGM_NUM_DONE_PLUS_
	for_each_end // for_each_brush_or_patch

	//stringlistfreecontents (&list_world_textures);
	CSGM_END_FUNC_
}

// Baker: This works fine, but the visibility times while 70% better are still WAY too long.
int csgc_detail_wedge_split_auto (map_t *hm)
{
	CSGM_START_
	int merges = 0;
	int num_considered = 0;
	int num_half_cube = 0;
	int num_all_carve = 0;
	int num_ne = 0;
	int num_sw = 0;
	int num_se_54 = 0;
	int num_nw_57 = 0;
	int num_other = 0;

	for_each_entity_2 (entnum, ent, hm)
		ccs *_no_detail_auto = map_get (hm, entnum, _NO_DETAIL_AUTO);
		if (_no_detail_auto)
			continue; // Any value

		for_each_brush_original_2 (brushnum, brush, hm, entnum)
			if (brush->is_terrain_wedge == false)
				continue;

			brush_terrain_wedge_split (hm, entnum, brushnum);
			CSGM_NUM_DONE_PLUS_
		for_each_end // for_each_brush_2


		for_each_brush_added_2 (brushnum, brush1, hm, entnum)
			num_considered ++;
			if (brush1->is_flat_half_cubeoid == false)
				continue;

			num_half_cube ++;

			ccs *tex = brush_texture_first_non_common(brush1, NULL);

			if (tex)
				continue; // I shouldn't have one.

			num_all_carve ++;
			// NE ... SW
			if (brush1->triangle_face_bits == TEXSIDE_NORTHEAST_53)
				num_ne ++;
			else if (brush1->triangle_face_bits == TEXSIDE_SOUTHWEST_58)
				num_sw ++;
			else if (brush1->triangle_face_bits == TEXSIDE_SOUTHEAST_54)
				num_se_54 ++;
			else if (brush1->triangle_face_bits == TEXSIDE_NORTHWEST_57)
				num_nw_57 ++;
			else
				num_other ++;

			//if (brush1->triangle_face_bits != TEXSIDE_NORTHEAST_53)
			//	continue;


			for_each_brush_added_2 (brushnum2, brush2, hm, entnum)
				if (brushnum == brushnum2)
					continue;
				if (VectorCompare (brush1->bxmins, brush2->bxmins) == false) continue;
				if (VectorCompare (brush1->bxmaxs, brush2->bxmaxs) == false) continue;
				if (brush2->is_flat_half_cubeoid == false)
					continue;

				//if (brush2->triangle_face_bits != TEXSIDE_SOUTHWEST_58)
				//	continue;

				// Make one brush from these 2.
				// NW              SE
				// W
				// E x
				// S x
				// N
				// F
				// C

				// Bastardize brush1 a little?

				// Add to NE ...
				// 1. South
				// 2. West
				// 3. Deleted the sloped face.
				// Remember these are likely to be all caulk.
				vec3_t brmins; VectorCopyDestSrc (brmins, brush1->bxmins);
				vec3_t brmaxs; VectorCopyDestSrc (brmaxs, brush1->bxmaxs);
				brush1->is_considered_deleted = true;
				brush2->is_considered_deleted = true;

				// THIS CAN BREAK A BRUSH DUE TO ALLOC
				brush_add_cubeoid_with_texture (ent, brmins, brmaxs, COMMON_CAULK);


				// brush_add_as_cubeoid_texturing_like_2_weld
				// brush_terrain_triangles_merge (hm, entnum, brushnum, brushnum2);

				merges ++;
				break;
			for_each_end // for_each_brush_2
		for_each_end // for_each_brush_2
	for_each_end // entity
#ifdef _DEBUG
	int j = merges + num_considered + num_half_cube + num_all_carve + num_ne + num_sw;
#endif// _DEBUG
	CSGM_END_
}


// Non-cubeoid brushes that don't touch world bounding box get CONTENTFLAGS_DETAIL
// To exclude ... _no_detail_auto key on a func_group
int csgc_detail_auto (map_t *hm)
{
	CSGM_START_
		qbool first = false;

	{
		zpr ("Before:");
		int num_detail_faces = 0;
		int num_structural_faces = 0;
		int num_structural_mark = 0;
		for_each_entity_brush_face_3 (hm, entnum, ent, brushnum, brush, facenum, face)
			if (Have_Flag ( 
				(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_DETAIL) )
				num_detail_faces ++;
			else
				num_structural_faces ++;

			if (Have_Flag ( 
				(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_STRUCTURAL) )
				num_structural_mark ++;
		for_each_end_3
		zpr ("num_detail_faces   : %s", comma64(num_detail_faces));
		zpr ("num_structural     : %s", comma64(num_structural_faces));
		zpr ("* explicit         : %s", comma64(num_structural_mark));
	}

	for_each_entity_2 (entnum, ent, hm)
		ccs *_no_detail_auto = map_get (hm, entnum, _NO_DETAIL_AUTO);
		if (_no_detail_auto)
			continue; // Any value

		for_each_brush_2 (brushnum, brush, hm, entnum)
			qbool shall_detail = true;
			qbool touch_west = brush->bxmins[0] <= hm->bbentsmins[0];
			qbool touch_east = brush->bxmaxs[0] >= hm->bbentsmaxs[0];
			qbool touch_south = brush->bxmins[1] <= hm->bbentsmins[1];
			qbool touch_north = brush->bxmaxs[1] >= hm->bbentsmaxs[1];
			qbool touch_floor = brush->bxmins[2] <= hm->bbentsmins[2];
			//qbool touch_ceil = brush->bxmaxs[2] >= hm->bbentsmaxs[2];
//			if (entnum > 0) {
//				int j = 5;
//			}
			if (touch_west || touch_east || touch_south ||
				touch_north || touch_floor/* || touch_ceil*/) {
				// Touching edge of world
				shall_detail = false;
			} else if (brush->is_cubeoid) {
				shall_detail = false;
			} else if (brush->is_terrain_wedge) {
				// Cube brushes are interior usually.
				// We will do the half-cubeoid triangle ones too
				shall_detail = true;
			} else if (brush->is_flat_half_cubeoid) {
				// Cube brushes are interior usually.
				// We will do the half-cubeoid triangle ones too
				shall_detail = false;
			} else {
				// int j = 5;
			}

			if (shall_detail == false)
				continue;

			if (first == false) {
				zpr ("First is ent %d brush %d", entnum, brushnum);
				first = true;
			}
			for_each_brush_face_2 (facenum, face, hm, entnum, brushnum)
				int content_flags = face->faceparms[FT_SURFACEFLAGS_3];
				Flag_Add_To (content_flags, CONTENTS_DETAIL);
				Flag_Remove_From (content_flags, CONTENTS_STRUCTURAL);
				face->faceparms[FT_SURFACEFLAGS_3] = content_flags;
				face->trail_count = 6;
			for_each_end // face

			CSGM_NUM_DONE_PLUS_
		for_each_end // for_each_brush_2
	for_each_end // entity

	zpr ("Number brushes made detail: %d", _num_done);
	{
		RELATED_ (map_save_to_string_bsalloc)
		zpr ("After:");
		int num_detail_faces = 0;
		int num_structural_faces = 0;
		int num_structural_mark = 0;
		for_each_entity_brush_face_3 (hm, entnum, ent, brushnum, brush, facenum, face)
			if (Have_Flag ( 
				(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_DETAIL) )
				num_detail_faces ++;
			else
				num_structural_faces ++;

			if (Have_Flag ( 
				(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_STRUCTURAL) )
				num_structural_mark ++;
		for_each_end_3
		zpr ("num_detail_faces   : %s", comma64(num_detail_faces));
		zpr ("num_structural     : %s", comma64(num_structural_faces));
		zpr ("* explicit         : %s", comma64(num_structural_mark));

	}

	CSGM_END_
}


// Baker: July 5 2026 - no CSG command line version.
// Looks like it replaces flat textures with common/up

// Z is 0
int csgc_flat_ground_retexture (map_t *hm, ccs *z_val_ground_level_string)
{
	CSGM_START_
	int z = atoi(z_val_ground_level_string);

	zpr ("Ground level = %d", z);

	for_each_entity_2 (entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			brushrow_s *d_face = brush_get_face_facing_up_flat_only (d_brush);

			if (!d_face)					continue;	// No flat face.
			if (d_face->bfmaxs[2] != z)		continue;	// Wrong floor level

			face_texture_set (d_face, COMMON_UP);
			CSGM_NUM_DONE_PLUS_
		for_each_end // for_each_brush_2
	for_each_end // for_each_entity
	CSGM_END_FUNC_
}

void _face_basename_get(brushrow_s *face, char *sbuf, size_t sbuf_sizeof)
{
	va_copy_path_64_Q3 (basename, face->pbrtexture);
	if (!String_Edit_Remove_Trailing_Text (basename, "_noflat"))
		if (!String_Edit_Remove_Trailing_Text (basename, "_flat"))
			if (!String_Edit_Remove_Trailing_Text (basename, "_flat_trim"))
				String_Edit_Remove_Trailing_Text (basename, "_noflat_trim");

	dp_strlcpy (sbuf, basename, sbuf_sizeof);
}

RELATED_(csgc_flat_trim)
qbool brushes_touch (brush_s *a, brush_s *b) {	
	qbool they_touch = Boxes_Touch_3D (a->bxmins, a->bxmaxs, b->bxmins, b->bxmaxs);
	return they_touch;
}


qbool planes_are_mostly_opposite_slope(vec3_t normal_a, vec3_t normal_b)
{
    vec3_t slope_a, slope_b;

    slope_a[0] = normal_a[0];
    slope_a[1] = normal_a[1];
    slope_a[2] = 0.0f;

    slope_b[0] = normal_b[0];
    slope_b[1] = normal_b[1];
    slope_b[2] = 0.0f;

    float len_a = VectorLength(slope_a);
    float len_b = VectorLength(slope_b);

	// // Both faces must have some horizontal slope.
    if (len_a < 0.001f || len_b < 0.001f)
        return false;

    float dot = DotProduct(slope_a, slope_b) / (len_a * len_b);

	// Slope directions differ by more than 135 degrees.
    return (dot < -0.7071f); // cos(135 degres) == 0.707
}

qbool faces_are_mostly_opposite_slope(brushrow_s *face1, brushrow_s *face2)
{
	return planes_are_mostly_opposite_slope(face1->face_plane.normal, face2->face_plane.normal);
}

int csgc_ridge_valley (map_t *hm)
{
	CSGM_START_
	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
#ifdef _DEBUG
		if (entnum == 4) {
			int j = 5;
		}
#endif // _DEBUG
	
		// ALREADY PROCESSED?  Not considered again
//		if (brush->ridgebits) continue; 

		// Terrain wedges only
		// Non-terrain wedges .. do not set ridge bits
		// Because while we don't mark them, we look at them later if adjacent
		if (brush->is_terrain_wedge == false)
			continue;

		//brush->ridgebits = RIDGE_NONE_1;

		brushrow_s *ground_face_sloped_1 = brush_get_face_single_upward_tri_or_quad(brush);

		

		// No flat top or wrong shape are not considered (this includes ceilings)
		if (!ground_face_sloped_1) continue; 

		// Flat top brushes are not considered
		qbool is_flat = face_is_flat(ground_face_sloped_1);
		if (is_flat) continue;

		float slope1 = plane_z_slope_degrees (ground_face_sloped_1->face_plane.normal);

		ccs *texturename = brush_texture_first_non_common (brush, NULL);
		if (!texturename)
			continue;

		// SLOPED! what kind of slope (positive or negative?)
		Vector3DeclareCopyEx (ground_slope_1, ground_face_sloped_1->face_plane.normal);

		

		int num_adjace = 0;
		for_each_entity_brush_2 (hm, entnum2, ent2, brushnum2, brush2)
#ifdef _DEBUG
			if (entnum == 6) {
				int j = 5;
			}
#endif // _DEBUG

			if (brush2 == brush)
				continue; // No check versus self.

			// Have to share same bottom right now ... 
			if (brush2->bxmins[2] != brush2->bxmins[2])
				continue;

			qbool they_touch = brushes_touch (brush, brush2);
			if (!they_touch)
				continue; // Not bbox touching

			ccs *texturename2 = brush_texture_first_non_common (brush2, NULL);
			if (!texturename2)
				continue;

			// These brushes touch for real.
			// Not quite known if same top plane.
			RELATED_ (csgc_weld_entity_merge_2_square_triangles_to_rect_metric_new_brush csgc_weld)
			brushrow_s *face_adjace_2 = brush_get_face_single_upward_tri_or_quad(brush2);

			qbool is_flat = face_is_flat(face_adjace_2);

			float slope2 = plane_z_slope_degrees (face_adjace_2->face_plane.normal);



			// RIGHT NOW, ground is not considered.
			if (is_flat) {
				if (face_adjace_2->bfmaxs[2] == ground_face_sloped_1->bfmaxs[2]) {
					Flag_Add_To (brush->ridgebits, RIDGE_PEAK_1);
					//Flag_Add_To (brush->ridgebits, RIDGE_LOWER_ONE_4);
				}
				if (face_adjace_2->bfmins[2] == ground_face_sloped_1->bfmins[2]) {
					//Flag_Add_To (brush->ridgebits, RIDGE_HIGHER_ONE_8);
					Flag_Add_To (brush->ridgebits, RIDGE_VALLEY_2);
				}

				continue;

			}


			RELATED_ (flat_ground_top_0_0_NEG1)
			qbool are_opposite_slope = faces_are_mostly_opposite_slope(ground_face_sloped_1, face_adjace_2);

			if (are_opposite_slope == false) {
				// Same direction more or less
				// Check for continuation down or up to disqualify
				if (face_adjace_2->bfmaxs[2] == ground_face_sloped_1->bfmins[2]) {
					Flag_Add_To (brush->ridgebits, RIDGE_LOWER_ONE_4);
				}
				if (face_adjace_2->bfmins[2] == ground_face_sloped_1->bfmaxs[2]) {
					Flag_Add_To (brush->ridgebits, RIDGE_HIGHER_ONE_8);
				}

				continue;
			}
			// If these 2 faces share a bottom it is a V
			// If these 2 faces share a top it is ^
			qbool is_ridge = ground_face_sloped_1->bfmaxs[2] == face_adjace_2->bfmaxs[2];
			qbool is_valley = ground_face_sloped_1->bfmins[2] == face_adjace_2->bfmins[2];


			if (is_ridge) {
				Flag_Add_To (brush->ridgebits, RIDGE_PEAK_1);
				Flag_Add_To (brush2->ridgebits, RIDGE_PEAK_1);
				zpr ("ent %d brush %d is a ridge", entnum, brushnum);
				zpr ("ent %d brush %d is a ridge", entnum, brushnum2);
			}
			if (is_valley) {
				Flag_Add_To (brush->ridgebits, RIDGE_VALLEY_2);
				Flag_Add_To (brush2->ridgebits, RIDGE_VALLEY_2);
				zpr ("ent %d brush %d is a valley", entnum, brushnum);
				zpr ("ent %d brush %d is a valley", entnum, brushnum2);
			}
			// No valley if a sloped brush of same is lower than you.
			if (is_ridge && is_valley) {
				int j = 5; // Betttee not happen -- happens now!
			}
			if (!is_ridge && !is_valley) {
				int j = 5; // Can this happen? YES
			}
			
		for_each_end_2 // for_each_entity_brush
	for_each_end_2 // for_each_entity_brush

#if 1 // This disqualifies so many.
	// Need valley and ridge disqualification .. if there is adjacent face brush with our mins as max
	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
		if (brush->is_terrain_wedge == false)
			continue;

		if (Have_Flag (brush->ridgebits, RIDGE_VALLEY_2) && Have_Flag (brush->ridgebits, RIDGE_LOWER_ONE_4)) {
			// Can't be valley if a samey one goes lower.  Right?
			Flag_Remove_From (brush->ridgebits, RIDGE_VALLEY_2);
			continue;
		}

		if (Have_Flag (brush->ridgebits, RIDGE_PEAK_1) && Have_Flag (brush->ridgebits, RIDGE_HIGHER_ONE_8)) {
			// Can't be valley if a samey one goes lower.  Right?
			Flag_Remove_From (brush->ridgebits, RIDGE_PEAK_1);
			continue;
		}

		// VALLEY
	for_each_end_2 // for_each_entity_brush
#endif

	// APPEND PHASE
	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
		if (brush->is_terrain_wedge == false)
			continue;

		if (brush->ridgebits == RIDGE_NONE_0)
			continue;

		brushrow_s *ground_face_sloped_1 = brush_get_face_single_upward_tri_or_quad(brush);

		if (Have_Flag (brush->ridgebits, RIDGE_PEAK_1)) {
			face_texture_set_cat (ground_face_sloped_1, ground_face_sloped_1->pbrtexture, "_ridge");
			CSGM_NUM_DONE_PLUS_
		}

		if (Have_Flag (brush->ridgebits, RIDGE_VALLEY_2)) {
			face_texture_set_cat (ground_face_sloped_1, ground_face_sloped_1->pbrtexture, "_valley");
			CSGM_NUM_DONE_PLUS_
		}

		
	for_each_end_2 // for_each_entity_brush
		zpr ("Num ridge valley appends = %d", _num_done); 
	CSGM_END_
}

// This compares textures with same name
// It strips the _noflat or _flat extension ignoring it.
int csgc_flat_trim (map_t *hm)
{
	CSGM_START_
	stringlist_t list = {0};
	int num_done = 0;
	for_each_entity_2 (entnum, ent, hm)
		for_each_brush_2 (brushnum, brush, hm, entnum)
			//brush_s *a = &d_ent->brushlist.brush[brushnum_a];
			brushrow_s *face = brush_get_face_single_upward_tri_or_quad(brush);
			if (!face)
				continue; // Not eligible due to some reason.  Wrong shape or something.

			if (FACE_IS_COMMON (face)) continue; // flat_trim

			qbool is_flat = face_is_flat(face);
			stringlistappend_unique_didAdd (&list, face->pbrtexture);

			// Construct the basename
			char basename[Q3PATHLENGTH_64];
			_face_basename_get (face, basename, sizeof(basename)); // Strip _noflat ...

			int flat_neighbors = 0; int nonflat_neighbors = 0;
			for_each_brush_2 (brushnum2, brush2, hm, entnum)
				if (brush2 == brush) continue; // Don't do self.
				brushrow_s *face2 = brush_get_face_single_upward_tri_or_quad(brush2);

				if (!face)
					continue; // Ineligible!

				if (FACE_IS_COMMON (face2)) continue; // flat trim face2

				if (!String_Starts_With(face2->pbrtexture, basename))
					continue; // Not same texture class

				qbool they_touch = face_bbox_touch_face (face, face2);
				if (!they_touch)
					continue; // Not bbox touching
				qbool is_flat_neighbor = face_is_flat(face2);
				if (is_flat_neighbor)
					flat_neighbors ++;
				else nonflat_neighbors ++;
			for_each_end // for brush neighbors

			if (is_flat == false && flat_neighbors == 0)
				continue; // nonflat next to nonflats

			// DECIDE ON 3 CATEGORIES.
			ccs *extension;
			if (is_flat && nonflat_neighbors == 0)				extension = "_flat";
			else if (is_flat && nonflat_neighbors)				extension = "_flat_trim";
			else if (is_flat == false && flat_neighbors == 0)	extension = "_noflat";
			else												extension = "_noflat_trim";

			face_texture_set_cat (face, basename, extension);

			num_done ++;
		} // brush
	for_each_end // for_each_entity
	zpr ("List of texturenames replaced:");
	stringlist_condump_with_linenums (&list);
	stringlistfreecontents (&list);
	return num_done;

	CSGM_END_
}


// Not patches
int csgc_floor_every_brush_to_specified_z (map_t *hm, ccs *zfloorstring)
{
	CSGM_START_
	float floor_level_wanted = atof(zfloorstring);
	printvarf (floor_level_wanted);

	// WORLD: MOVE DOWN THE BRUSHES.
	for_each_entity_only_world (entnum, hm, d_ent)
		// No measured brushes? keep moving
		if (d_ent->has_brushes == false) continue;

		// Move EACH brush separately
		for_each_brush_or_patch (brushnum_a, d_ent, d_brush)
			 // 0 - 500 = -500
			//vec3_t move_amount_add = {0, 0 , floor_level_wanted - d_brush->bxmaxs[2] };
			vec3_t move_amount_add = {0, 0 , floor_level_wanted - d_brush->bxmins[2] };
			if (move_amount_add[2] == 0) continue; // No move needed

			// This does texturelock too.
			brush_or_patch_tranlate_with_texturelock_was_processed (d_brush, move_amount_add);
			CSGM_NUM_DONE_PLUS_
		for_each_end // for_each_brush_or_patch
	for_each_end // only world

	// ENTITIES: MOVE DOWN THE ENTITY.
	for_each_entity_after_world (entnum, hm, d_ent)
		// No measured brushes? keep moving
		if (d_ent->has_brushes == false) continue;

		//vec3_t move_amount_add = {0, 0 , floor_level_wanted - d_ent->bemaxs[2]};
		vec3_t move_amount_add = {0, 0 , floor_level_wanted - d_ent->bemins[2]};
		if (move_amount_add[2] == 0) continue; // No move needed

		// Translate entity brushes.
		for_each_brush_or_patch (brushnum_a, d_ent, d_brush)
			brush_or_patch_tranlate_with_texturelock_was_processed (d_brush, move_amount_add);
			CSGM_NUM_DONE_PLUS_ // It was processed.
		for_each_end // for_each_brush_or_patch
	for_each_end // after_world

	CSGM_END_FUNC_
}

int RoundDownTo16(float value)
{
    return (int)(floor(value / 16.0f) * 16.0f);
}

RELATED_ (csgc_print_lowest_ground)
#define CARVE_ADJUST_16 16
int csgc_ground_carve (map_t *hm)
{
	CSGM_START_
	int lowest_entnum = not_found_neg1;
	int lowest_brushnum = not_found_neg1;
	RELATED_ (csgc_print_lowest_ground)
	float lowest_ground = map_lowest_ground_not_caulk (hm, &lowest_entnum, &lowest_brushnum);
	float carve_point = RoundDownTo16 (lowest_ground);

	carve_point -= CARVE_ADJUST_16;
	zpr ("Lowest ground is %g: ent %d brush %d", lowest_ground, lowest_entnum, lowest_brushnum);

	Vector3DeclareCopyEx (cutter_mins, hm->bbentsmins);
	Vector3DeclareCopyEx (cutter_maxs, hm->bbentsmaxs);

	printvarv (hm->bbentsmins);
	printvarv (hm->bbentsmaxs);

	cutter_maxs[2] = carve_point;

	printvarv (cutter_mins);
	printvarv (cutter_maxs);

	int cutter_unit = 0;
	int cutter_entnum = 0;
	int cutter_brushnum = map_add_cubeoid (hm, WORLD_0, cutter_mins, cutter_maxs, COMMON_CARVE, /*texscale*/ 1.0, TEXSIDE_NONE_0);

	BRUSH_DEFINE (cutter_brush, hm, WORLD_0, cutter_brushnum);

	cutter_brush->is_considered_deleted = true;
	int num_made_here = map_carve_cubeoid_against_any_bulk (hm, cutter_brush, cutter_unit, cutter_entnum,
		cutter_brushnum);

	// Refresh stale pointer.
	BRUSH_DEFINE (cutter_brush2, hm, WORLD_0, cutter_brushnum);
	cutter_brush2->is_considered_deleted = false;

	CSGM_NUM_DONE_PLUS_
	CSGM_END_
}


int csgc_info_player_start_drop_center (map_t *hm)
{
	vec3_t drop_center_sky;
	VectorCenter (hm->bbentsmins, hm->bbentsmaxs, drop_center_sky);
	drop_center_sky[2] = SKY_HEIGHT_8192;

	// Drop it

	vec3_t collide_spot;
	qbool did_hit = map_traceline_drop_ok (hm, 	drop_center_sky, /*ignore ent*/ not_found_neg1, collide_spot);

	if (!did_hit) {
		zpr ("sky drop failed from " V3G, VS(collide_spot));
	}

	collide_spot[2] += 32; // Move up

	printvarv (collide_spot);

	int entnum = map_add (hm, "info_player_start");
	map_set_vec3_g (hm, entnum, "origin", collide_spot);

	return 1;
}

int csgc_infodump (map_t *hm)
{
	CSGM_START_
	// Print all the textures
	// # entities
	// # list of entity types
	// # brushes (in entire map)
	// # faces (in entire map)
	// unique textures and their count
	stringlist_t list_textures = {0};
	stringlist_t list_classnames = {0};

	//ENT_DEFINE (r_world, hm, WORLD_0);

	ccs *worldspawn_message = map_get(hm, WORLD_0, "message");

	zpr ("Map title: %s", worldspawn_message ? worldspawn_message : "<NO TITLE>");


	int num_patches = 0, num_brushes = 0, num_faces = 0, num_non_common_faces = 0;
	for_each_entity_2(entnum, r_ent, hm)
		ccs *classname = map_get (hm, entnum, "classname");

		if (classname)
			stringlistappend (&list_classnames, classname);

		for_each_brush_2 (brushnum, r_brush, hm, entnum)
			// REAL BRUSH
			for_each_brush_face_2 (facenum, r_face, hm, entnum, brushnum)
				stringlistappend (&list_textures, r_face->pbrtexture);
				if (FACE_IS_COMMON (r_face)) {
					// Nada
				} else {
					num_non_common_faces ++;
				}
				num_faces ++;
			for_each_end // for_each_brush_face
			num_brushes ++;
		for_each_end // for_each_brush_or_patch

		for_each_patch2 (brushnum, r_patch, hm, entnum)
			if (r_patch->is_a_patch) {
				stringlistappend_unique_didAdd (&list_textures, r_patch->patch_texture);
				num_patches ++;
				continue;
			}
		for_each_end // patch_row
	for_each_end // for_each_entity

	zpr ("Entities: %d", hm->count);
	for_each_entity_2(entnum, r_ent, hm)
		ccs *classname = map_get (hm, entnum, "classname");
		if (Vector3_IsZeros(r_ent->bedims)) {
			zpr ("%4d: %-22.22s", entnum, classname);
		} else {
			zpr ("%4d: %-22.22s %d brushes size " V3G, entnum, classname, r_ent->brushlist.count, VS(r_ent->bedims));
		}
	for_each_end // for_each_entity
	zpr ("=============");

	zpr ("Classnames: %d", list_classnames.numstrings);
	stringlist_condump_autocount_uniques (&list_classnames);
	zpr ("=============");
	zpr ("Textures: %d", list_textures.numstrings);
	stringlist_condump_autocount_uniques (&list_textures);
	zpr ("=============");

	ccs *most_common_texturename = stringlist_find_most_common_index_ignoring_this (&list_textures, COMMON_CAULK);

	// ORIGIN IN WORLD CHECK
	int origin_in_world = 0;
	//  Should check func_group too.
	for_each_entity_2(entnum, ent, hm)
		qbool shall_check = (entnum == WORLD_0) || entity_is_func_group_prefixed (ent);
		if (shall_check == false)
			continue; // Not world or func_group

		for_each_brush_2 (brushnum, brush, hm, entnum)
			if (brush_has_texturename (brush, COMMON_ORIGIN) == false)
				continue;

			zpr ("origin_in_world ent %d brush %d", entnum, brushnum);
			origin_in_world++;
			break; // One warning per brush
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity

	// DUP PLANES
	int num_duplicate_planes = 0;
	for_each_entity_2(entnum, r_ent, hm)
		for_each_brush_2 (brushnum, r_brush, hm, entnum)
			qbool dup = brush_has_duplicate_planes (hm, entnum, brushnum);

			if (dup) {
				zpr ("Duplicate brush planes %d %d", entnum, brushnum);
				num_duplicate_planes ++;
			}
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity

	int num_detail_faces = 0;
	int num_structural_faces = 0;
	int num_structural_mark = 0;
	for_each_entity_brush_face_3 (hm, entnum, ent, brushnum, brush, facenum, face)
		if (Have_Flag ( 
			(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_DETAIL) )
			num_detail_faces ++;
		else
			num_structural_faces ++;

		if (Have_Flag ( 
			(int)face->faceparms[FT_SURFACEFLAGS_3], CONTENTS_STRUCTURAL) )
			num_structural_mark ++;


	for_each_end_3
	

	zpr ("size               : "V3G, VS(hm->bbentsdims));
	zpr ("num_entities       : %s", comma64(hm->count));
	zpr ("num_patches        : %s", comma64(num_patches));
	zpr ("num_brush          : %s", comma64(num_brushes));
	zpr ("num_detail_faces   : %s", comma64(num_detail_faces));
	zpr ("num_structural     : %s", comma64(num_structural_faces));
	zpr ("* explicit         : %s", comma64(num_structural_mark));
	zpr ("num_total_faces    : %s", comma64(num_faces));
	zpr ("non_common_faces   : %s (not caulk, etc.)", comma64(num_non_common_faces));
	zpr ("num_cubeoids       : %s", comma64(hm->num_cubeoids));
	zpr ("num_non_6_cubeoids : %s (6 faces, but not cubeoid)", comma64(hm->num_non_cubeoid_6_facer));
	zpr ("num_textures       : %s", comma64(list_textures.numstrings));
	zpr ("num_classnames     : %s", comma64(list_classnames.numstrings));
	zpr ("most used texture  : %s", most_common_texturename);
	zpr ("duplicate_planes   : %s", comma64(num_duplicate_planes));
	zpr ("origin_in_world    : %s", comma64(origin_in_world));

	CSGM_NUM_DONE_PLUS_
	CSGM_END_FUNC_
}

float map_lowest_ground_not_caulk (map_t *hm, int *plowest_entnum, int *plowest_brushnum)
{
	float lowest_ground = 999999;
	int lowest_entnum = not_found_neg1;
	int lowest_brushnum = not_found_neg1;
	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
		int top_index = brush_top_face_index_maybe (brush);

		if (top_index == not_found_neg1)
			continue; // Not a shape we support

		FACE_DEFINE_CONST (face, hm, entnum, brushnum, top_index);

		if (FACE_IS_COMMON (face)) continue;

		//float high = face->bfmaxs[2];
		// Nov 12 2026 - This is a sloped or flat face, not a brush.
		//
		float high = face->bfmins[2];
		if (high < lowest_ground) {
			lowest_ground = high;
			lowest_entnum = entnum;
			lowest_brushnum = brushnum;
		}

	for_each_end_2 // for_each_entity_brush
	NOT_MISSING_ASSIGN (plowest_entnum, lowest_entnum);
	NOT_MISSING_ASSIGN (plowest_brushnum, lowest_brushnum);
	return lowest_ground;
}


// csgc_print_lowest_ground returns 0
int csgc_print_lowest_ground (map_t *hm)
{
	int lowest_entnum = not_found_neg1;
	int lowest_brushnum = not_found_neg1;
	float lowest_ground = map_lowest_ground_not_caulk (hm, &lowest_entnum, &lowest_brushnum);

	zpr ("Lowest ground is %g: ent %d brush %d", lowest_ground, lowest_entnum, lowest_brushnum);
	return 0;
}

RELATED_ (Mod_LoadTextureFromQ3Shader)
RELATED_ (int csgc_printtextures (map_t *hm))

float plane_z_slope_degrees(vec3_t normal)
{
    vec3_t horizontal;
    float horizontal_length;
    float normal_z_abs;

    horizontal[0] = normal[0];
    horizontal[1] = normal[1];
    horizontal[2] = 0.0f;

    horizontal_length = VectorLength(horizontal);
    normal_z_abs = fabsf(normal[2]);

    return RAD2DEG(atan2f(horizontal_length, normal_z_abs));
}

int csgc_printfaces (map_t *hm, ccs *_entnum, ccs *_brushnum)
{
	int entnum = strlen(_entnum) ? atoi(_entnum) : not_found_neg1;
	int brushnum = strlen(_brushnum) ? atoi(_brushnum) : not_found_neg1;

	if (in_range_beyond(0, entnum, hm->count)== false) {
		zpr ("Entity %d not in range, max is %d", entnum, hm->count);
		return 0;
	}

	if (brushnum != not_found_neg1) {
		// SPECIFIC BRUSH
		int entbrushcount = BRUSH_COUNT (hm, entnum);
		if (in_range_beyond(0, brushnum, entbrushcount)== false) {
			zpr ("Brushnum %d:%d not in range, max is %d", entnum, brushnum, entbrushcount);
			return 0;
		}

		BRUSH_DEFINE (brush, hm, entnum, brushnum);
		RELATED_ (TEXSIDE_NORTHEAST_53 TEXSIDE_SOUTHWEST_58)
		zpr ("Sides %d Is wedge? %s  cubeoid? %s half cubeoid tri? %s  FaceBits %d", brush->brushrowlist.count, brush->is_terrain_wedge ? "yes" : "no", brush->is_cubeoid ? "yes" : "no", brush->is_flat_half_cubeoid ? "yes" : "no", brush->triangle_face_bits);
		zpr ("bxmins " V3G " bxmaxs " V3G, VS(brush->bxmins), VS(brush->bxmaxs));
		for_each_brush_face_2 (facenum, face, hm, entnum, brushnum)
			ccs *face_dir = face_direction (brush, facenum, face);

			//zpr ("Candidate brush ent %d brush %d face %d (%s) maxs" V3G " normal" V3G,
			//	entnum, brushnum, facenum,
			//	face_dir,
			//	VS(face->bfmaxs),
			//	VS(face->face_plane.normal)
			//	);
			qbool is_slanttop = brush->brushrowlist.count == 5 && brush->top_slanted_index == facenum;
			qbool is_flattop = Have_Flag (brush->triangle_face_bits, TEXSIDE_CEIL_16) && facenum == brush->cubeoid_index[FACE_CEIL_5];

			char slopedegrees[32] = {0};
			if (is_slanttop || is_flattop) {
				float z_slope = plane_z_slope_degrees (face->face_plane.normal);
				c_dpsnprintf1 (slopedegrees, "z slope %g", z_slope);
			}

			zpr ("ent %d brush %d face %d (%s) %s normal " V3G " %s",
				entnum,
				brushnum,
				facenum,
				face_dir,
				face->pbrtexture,
				VS(face->face_plane.normal),
				slopedegrees);
		for_each_end // for_each_entity_brush_face

		RELATED_ (map_clipboard_set_brush)// (hm, entnum, brushnum);

		return 0; // Do not write - this function returns 0
	}

	zpr ("Need brush number");
	//// YES - PUT IT ON CLIPBOARD TOO
	//if (1) {
	//	// ENT
	//	baker_string_s *bs_maptext = BakerString_Create_Malloc_With_MegaBytes (100); // 100 MB
	//	_cat_entnum (bs_maptext, hm, entnum);
	//	Clipboard_Set_Text (bs_maptext->string);
	//	zpr (bs_maptext->string);
	//	BakerString_Destroy_And_Null_It (&bs_maptext); // July 29 2025 - This was missing, fool!
	//	return 0; // Do not write - this function returns 0
	//}

	return 0; // UNREACHABLE DUE TO IF 1
}



int csgc_printunit (map_t *hm, ccs *_entnum, ccs *_brushnum)
{
	int entnum = strlen(_entnum) ? atoi(_entnum) : not_found_neg1;
	int brushnum = strlen(_brushnum) ? atoi(_brushnum) : not_found_neg1;

	if (in_range_beyond(0, entnum, hm->count)== false) {
		zpr ("Entity %d not in range, max is %d", entnum, hm->count);
		return 0;
	}

	if (brushnum != not_found_neg1) {
		int entbrushcount = BRUSH_COUNT (hm, entnum);
		if (in_range_beyond(0, brushnum, entbrushcount)== false) {
			zpr ("Brushnum %d:%d not in range, max is %d", entnum, brushnum, entbrushcount);
			return 0;
		}
		baker_string_s *bs_maptext = BakerString_Create_Malloc_With_MegaBytes (100); // 100 MB
		_cat_brushnum (bs_maptext, hm, entnum, brushnum);
		Clipboard_Set_Text (bs_maptext->string);
		zpr (bs_maptext->string);
		BakerString_Destroy_And_Null_It (&bs_maptext); // July 29 2025 - This was missing, fool!

		RELATED_ (map_clipboard_set_brush)// (hm, entnum, brushnum);

		return 0;
	}

	// YES - PUT IT ON CLIPBOARD TOO
	if (1) {
		// ENT
		baker_string_s *bs_maptext = BakerString_Create_Malloc_With_MegaBytes (100); // 100 MB
		_cat_entnum (bs_maptext, hm, entnum);
		Clipboard_Set_Text (bs_maptext->string);
		zpr (bs_maptext->string);
		BakerString_Destroy_And_Null_It (&bs_maptext); // July 29 2025 - This was missing, fool!
		return 0;
	}

	return 0;
}

// replacetextureadjacent textures/tsh/rocky_noflat the texture to replace
// if it slope and the adjacent texture is flat
int csgc_process_terrain (map_t *hm)
{
	// Start with brush facer.
	zpr ("process_terrain 1: caulk all brushes");
	csgc_caulk_everything (hm);

	zpr ("process_terrain 2: texture upwards faces" MAPS_SH_ROCKY);
	map_reload (hm); // Remetric everything
	csgc_brushfacer_ex (hm, MAPS_SH_ROCKY, NULL);

	float z_0 = 0;
	zpr ("process_terrain 3: texture ground level z == %g to" MAPS_SH_FANG_GRASS, z_0);
	map_reload (hm); // Remetric everything
	csgc_brushfacer_ex (hm, MAPS_SH_FANG_GRASS, &z_0);

	zpr ("process_terrain 4: weld triangles and cubeoids");
	map_reload (hm); // Remetric everything
	csgc_weld (hm);

	zpr ("process_terrain 5: ground carve separate slopes (better visibility)");
	map_reload (hm); // Remetric everything
	csgc_ground_carve (hm);

	zpr ("process_terrain 6: flat and trim detection");
	map_reload (hm); // Remetric everything
	csgc_flat_trim (hm);

	zpr ("process_terrain 7: ridge valley detection");
	map_reload (hm); // Remetric everything
	csgc_ridge_valley (hm);

	zpr ("process_terrain 8: add worldspawn sky and detail_auto keys");
	map_reload (hm); // Remetric everything
	map_set (hm, WORLD_0, CSG_CMW__DETAIL_AUTO, "1");
	map_set (hm, WORLD_0, "_blocksize", "4096 4096 4096");
	map_set (hm, WORLD_0, "_farplanedist", "16384");
	map_set (hm, WORLD_0, "_keeplights", "1");
	map_set (hm, WORLD_0, CSG_CMW__SKY_ENCASE, "8192 skies/desert2_sky");

	zpr ("process_terrain 9: info_player_start at center");
	csgc_info_player_start_drop_center (hm);

	return 1;
}

//int csgc_replacetexture (map_t *hm, ccs *texturename)
//{
//	CSGM_START_
//	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
//		if (brush->is_terrain_wedge == false)
//			continue;
//
//		if (brush->top_slanted_index == not_found_neg1)
//			continue; // Better not happen!
//
//		int facenum = brush->top_slanted_index;
//		FACE_DEFINE (face, hm, entnum, brushnum, facenum);
//
//		if (String_Match (face->pbrtexture, texturenamesloped) == false)
//			continue;
//
//		//RELATED_ (csgc_printfaces)
//		//ccs *face_dir = face_direction (brush, facenum, face);
//
//		//zpr ("Candidate brush ent %d brush %d face %d (%s) maxs" V3G " normal" V3G,
//		//	entnum, brushnum, facenum,
//		//	face_dir,
//		//	VS(face->bfmaxs),
//		//	VS(face->face_plane.normal)
//		//	);
//
//		for_each_entity_brush_2 (hm, entnum2, ent2, brushnum2, brush2)
//			if (brush2 == brush) continue; // It's us
//			if (Have_Flag (brush2->triangle_face_bits, TEXSIDE_CEIL_16) == false)
//				continue; // Not a flat top
//			int ceil_facenum = brush2->cubeoid_index[FACE_CEIL_5];
//			FACE_DEFINE (face2, hm, entnum2, brushnum2, ceil_facenum);
//			if (String_Match (face2->pbrtexture, texturenamegrass) == false)
//				continue; // Not a grass texture
//			if (faces_ptr_bbox_touch (face2, face) == false)
//				continue; // No touch
//
//			// Retexture it?
//			face_texture_set (face, texturenamesloped2);
//			CSGM_NUM_DONE_PLUS_
//			break; // DONE
//		for_each_end_2 // for_each_entity_brush
//	for_each_end_2 // for_each_entity_brush
//	CSGM_END_
//}


int csgc_replacetextureadjacent (map_t *hm, ccs *texturenamesloped, ccs *texturenamegrass, ccs *texturenamesloped2)
{
	//for_each_entity_brush_face_3 (hm, entnum, brushnum, facenum)
	//	zpr ("ent %d brush %d face %d", entnum, brushnum, facenum);
	//for_each_end_3 // for_each_entity_brush_face
	CSGM_START_
	for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)

		if (brush->is_terrain_wedge == false)
			continue;

		if (brush->top_slanted_index == not_found_neg1)
			continue; // Better not happen!

		int facenum = brush->top_slanted_index;
		FACE_DEFINE (face, hm, entnum, brushnum, facenum);

		if (String_Match (face->pbrtexture, texturenamesloped) == false)
			continue;

		//RELATED_ (csgc_printfaces)
		//ccs *face_dir = face_direction (brush, facenum, face);

		//zpr ("Candidate brush ent %d brush %d face %d (%s) maxs" V3G " normal" V3G,
		//	entnum, brushnum, facenum,
		//	face_dir,
		//	VS(face->bfmaxs),
		//	VS(face->face_plane.normal)
		//	);

		for_each_entity_brush_2 (hm, entnum2, ent2, brushnum2, brush2)
			if (brush2 == brush) continue; // It's us
			if (Have_Flag (brush2->triangle_face_bits, TEXSIDE_CEIL_16) == false)
				continue; // Not a flat top
			int ceil_facenum = brush2->cubeoid_index[FACE_CEIL_5];
			FACE_DEFINE (face2, hm, entnum2, brushnum2, ceil_facenum);
			if (String_Match (face2->pbrtexture, texturenamegrass) == false)
				continue; // Not a grass texture
			if (faces_ptr_bbox_touch (face2, face) == false)
				continue; // No touch

			// Retexture it?
			face_texture_set (face, texturenamesloped2);
			CSGM_NUM_DONE_PLUS_
			break; // DONE
		for_each_end_2 // for_each_entity_brush
	for_each_end_2 // for_each_entity_brush
	CSGM_END_
}

// DO NOTHING EXCEPT DELETE _rewrite key .  Tests read and write.
int csgc_rewrite (map_t *hm)
{
	int num_done = 1;
	return num_done;
}


// -1 means none.
int csgc_rotateunit (map_t *hm, ccs *_entnum, ccs *_brushnum, ccs *_yawdegrees)
{
	int yaw_rotation_requested = atoi(_yawdegrees);
	vec3_t move = {0};
	Vector3DeclareSet (rotate, 0, yaw_rotation_requested, /*roll*/ 0);
	rotate[0] = 360 - rotate[0]; // Inverse pitch

	printvari (yaw_rotation_requested);
	int entnum = strlen(_entnum) ? atoi(_entnum) : not_found_neg1;
	int brushnum = strlen(_brushnum) ? atoi(_brushnum) : not_found_neg1;

	if (entnum == not_found_neg1) {
		zpr ("Rotate whole map");
		if (yaw_rotation_requested == 0) {
			zpr ("No rotation specified");
			return 0;
		}
		map_rotate_map (hm, move, rotate);
		goto bypass; // And then return 1
	} else if (in_range_beyond(0, entnum, hm->count) == false) {
		zpr ("Entity %d not in range, max is %d", entnum, hm->count);
		return 0;
	} // out range

	int entbrushcount = BRUSH_COUNT (hm, entnum);

	if (brushnum != not_found_neg1 && in_range_beyond(0, brushnum, entbrushcount) == false) {
		zpr ("Brushnum %d:%d not in range, max is %d", entnum, brushnum, entbrushcount);
		return 0;
	} // out range


	float current_rotation = -1;

	// If we are rotating, this is just useless information
	qbool ok = map_brush_get_rotation_is_ok (hm, entnum, brushnum, &current_rotation); // accepts brushnum -1 and converts to FACE_CEIL_5
	printvari (ok);
	printvarf (current_rotation);
	if (brushnum == not_found_neg1) {
		//map_rotate_entity (hm, entnum, move, rotate);
		ENT_DEFINE_CONST (ent, hm, entnum);
		printvarv (ent->bemins);
		printvarv (ent->bemaxs);
		printvarv (ent->bedims);
		printvarv (ent->becenter);
	}
	else {
		BRUSH_DEFINE_CONST (brush, hm, entnum, brushnum);
		printvarv (brush->bxmins);
		printvarv (brush->bxmaxs);
		printvarv (brush->bxdims);
		printvarv (brush->bxcenter);
	}

//	float current_rotation = something ();
	if (yaw_rotation_requested == 0)
		return 0;


	RELATED_ (csgc_rotateunit)
//	map_rotate_patch_no_trigonometry (hm, entnum, brushnum, move, yaw_rotation_requested);
//	return 1;

	if (brushnum == not_found_neg1) {
		map_rotate_entity (hm, entnum, move, rotate, /*no map transform*/ NULL);
	}
	else {
		map_rotate_brush (hm, entnum, brushnum, move, rotate);
	}
bypass:
	return 1;
}

// csgw_sky_encase - yes this is really a console command
int csgc_sky_encase (map_t *hm, ccs *skyabsolutetopzstring, ccs *skytexturename)
{
	CSGM_START_
	ENT_DEFINE (d_world, hm, WORLD_0);
	float wall_thickness_16_0 = 16.0;
	float skyabsolutetop = atof(skyabsolutetopzstring);
	if (skyabsolutetop == 0) {
		zpr ("No sky height defaulting top == %d", SKY_HEIGHT_8192);
		skyabsolutetop = SKY_HEIGHT_8192;
	}
	if (!*skytexturename) {
		skytexturename = "skies/desert2_sky";
		zpr ("No sky texture specified defaulting %s", skytexturename);
		skyabsolutetop = SKY_HEIGHT_8192;
	}

	// Ok now do 6 sider.  AVOIDING PAGE ARRAY ISSUES.
	Vector3DeclareCopyEx (brmins, hm->bbentsmins);
	Vector3DeclareCopyEx (brmaxs, hm->bbentsmaxs);
	float low_z = 0;

	// If floor is lower than zero
	if (brmins[2] < 0)
		low_z = brmins[2]; // -400 ==>  FLOOR SIDE OF ENCASE

	printvarf (skyabsolutetop);
	printvars (skytexturename);
	zpr ("Floor level = %g", low_z);

	vec3_t brmins_side, brmaxs_side;
	// W
	VectorSet (brmaxs_side, brmins[0] - 0, brmaxs[1], skyabsolutetop); // MAX
	VectorSet (brmins_side, brmins[0] - wall_thickness_16_0, brmins[1], low_z );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_EAST_1, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);
	// E
	VectorSet (brmaxs_side, brmaxs[0] + wall_thickness_16_0, brmaxs[1], skyabsolutetop); // MAX
	VectorSet (brmins_side, brmaxs[0] + 0, brmins[1], low_z );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_WEST_0, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);
	// S
	VectorSet (brmaxs_side, brmaxs[0], brmins[1] - 0, skyabsolutetop);
	VectorSet (brmins_side, brmins[0], brmins[1] - wall_thickness_16_0, low_z );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_NORTH_3, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);
	// N
	VectorSet (brmaxs_side, brmaxs[0], brmaxs[1] + wall_thickness_16_0, skyabsolutetop);
	VectorSet (brmins_side, brmins[0], brmaxs[1] + 0, low_z );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_SOUTH_2, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);
	// UP
	VectorSet (brmaxs_side, brmaxs[0] , brmaxs[1], skyabsolutetop + wall_thickness_16_0); // MAX
	VectorSet (brmins_side, brmins[0] , brmins[1], skyabsolutetop );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_FLOOR_4, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);
	// DOWN
	VectorSet (brmaxs_side, brmaxs[0] , brmaxs[1], low_z ); // MAX
	VectorSet (brmins_side, brmins[0] , brmins[1], low_z - wall_thickness_16_0 );
	entity_brushs_add_cubeoid_quadrant (d_world, FACE_CEIL_5, skytexturename, brmins_side, brmaxs_side, TESS_SIZE_HERE_16384, COMMON_CAULK);

	CSGM_NUM_DONE_PLUS_
	CSGM_END_FUNC_
}


int csgc_worldspawn_keys_print (map_t *hm)
{
	CSGM_START_

	ENT_DEFINE (d_world, hm, WORLD_0);

	sub_print_keys_quick (d_world);

	CSGM_NUM_DONE_PLUS_

	CSGM_END_FUNC_
}




RELATED_ (CSG_SET_WORLDSPAWN_KEY)
int csgc_worldspawn_key_set (map_t *hm, ccs *key, ccs *value)
{
	CSGM_START_

	ENT_DEFINE (d_world, hm, WORLD_0);

	map_set (hm, WORLD_0, key, value);
	CSGM_NUM_DONE_PLUS_

	sub_print_keys_quick (d_world);

	CSGM_END_FUNC_
}

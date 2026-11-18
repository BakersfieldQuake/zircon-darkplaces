// csg_p_entity.c.h


RELATED_ (VM_map_entity_get_mins_maxs) // September 15 2026 - Yes, used.  Also GenerateDungeon
// Baker: Does not include patches
qbool entity_expand_bounds_did_brushes (entityx_s *d_ent, vec3_t brmins, vec3_t brmaxs)
{
	qbool did_any_brushes = false;

	for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
		if (d_brush->brushrowlist.count && did_any_brushes == false)
			did_any_brushes = true;

		// REAL BRUSH
		for_each_brush_face (facenum, d_brush, d_face)
			bounds_expand (d_face->a, brmins, brmaxs);
			bounds_expand (d_face->b, brmins, brmaxs);
			bounds_expand (d_face->c, brmins, brmaxs);
		 for_each_end // for_each_brush_face
	for_each_end // for_each_brush_or_patch

	return did_any_brushes;
}

CALLERS_ (csgw_sky_encase csgw_floorage)

// FACE_CEIL_5 is called by floorage, but all the faces are called by sky encase
int entity_brushs_add_cubeoid_quadrant (entityx_s *p_ent, FACE_INT_e quadrant_wanted, ccs *texturename, const vec3_t brmins, const vec3_t brmaxs, 
	float tessSize, ccs *caulktex)
{
	int num_done = 0;
	for (float x0 = brmins[0]; x0 < brmaxs[0]; x0 += tessSize) {
		if (x0 >= brmaxs[0]) 
			continue;
		float x1 = x0 + tessSize; if (x1 > brmaxs[0]) x1 = brmaxs[0];
		for (float y0 = brmins[1]; y0 < brmaxs[1]; y0 += tessSize) {
			if (y0 >= brmaxs[1]) 
				continue;
			float y1 = y0 + tessSize; if (y1 > brmaxs[1]) y1 = brmaxs[1];
			for (float z0 = brmins[2]; z0 < brmaxs[2]; z0 += tessSize) {
				if (z0 >= brmaxs[2]) 
					continue;
				float z1 = z0 + tessSize; if (z1 > brmaxs[2]) z1 = brmaxs[2];
				vec3_t brmins2 = {x0, y0, z0};
				vec3_t brmaxs2 = {x1, y1, z1};

				int brush_idx1 = map_add_brush_cubeoid_with_texture(p_ent, brmins2, brmaxs2, caulktex);
				brush_s *n_brush = &p_ent->brushlist.brush[brush_idx1];

				brushrow_s *n_face = &n_brush->brushrowlist.brushrow[quadrant_wanted];
				face_texture_set (n_face, texturename);
				num_done ++;
			} // z
		} // y
	} // x
	return num_done;
}


// Returns brushnum




CALLERS_ (lightrod) // NO living callers Sep 28 2026 but function is useful
ccs *entity_get_first_texture (entityx_s *r_ent)
{
	for_each_brush_not_a_patch (brushnum, r_ent, r_brush)
		for_each_brush_face (facenum, r_brush, r_face)
			return r_face->pbrtexture;
		for_each_end // for_each_brush_face
	for_each_end // for_each_brush_or_patch
	return NULL;
}

// Returns brushnum --- which can be a patch --- with the texture
static int entity_texturename_brushum (const entityx_s *r_ent, ccs *texturename)
{
	for_each_brush_or_patch (brushnum, r_ent, r_brush)
		if (brush_has_texturename(r_brush, texturename))
			return brushnum;
	for_each_end
	return not_found_neg1;
}

int entity_texturename_starts_with_brushum (entityx_s *r_ent, ccs *texturename)
{
	for_each_brush_or_patch (brushnum, r_ent, r_brush)
		if (brush_texturename_starts_with_facenum(r_brush, texturename) != not_found_neg1)
			return brushnum;
	for_each_end
	return not_found_neg1;
}

qbool entity_has_include_texture (entityx_s *r_ent, ccs *MYKEY, char *texturename_out, size_t texturename_out_sizeof)
{
	// NOT WORLD
	for_each_brush_not_a_patch_undeleted (brushnum, r_ent, r_brush)
		for_each_brush_face (facenum, r_brush, r_face)
			if (false == String_Starts_With (r_face->pbrtexture, MYKEY /*"#includes/"*/))
				continue;
			
			dp_strlcpy (texturename_out, r_face->pbrtexture, texturename_out_sizeof);
			return true;
		for_each_end // for_each_brush_face
	for_each_end // for_each_brush_or_patch

	return false;
}

CALLERS_ (map_add)
entityx_s *entitylist_add (map_t *list)
{
	if (list->count >= list->maxsize) {
		list->maxsize += 1024; // Baker: 4096 is rather large.
		list->entity = (entityx_s *)Mem_Realloc(zonemempool,
			list->entity, list->maxsize * sizeof(entityx_s) );
	}

	entityx_s *pe = &list->entity[list->count ++]; // Incremented count
	return pe;
}

RELATED_ (VM_map_append_map)


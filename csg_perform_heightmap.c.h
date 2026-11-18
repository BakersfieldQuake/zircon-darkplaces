// csg_perform_heightmap.c.h

// ONLY SEND ENTITIES HERE WITH _heightmap value.
CALLERS_ (csge_heightmap csgax_frost)
static int _csge_heightmap_ent_num_added (entityx_s *d_ent, float zlow, float zhigh, float zrange)
{
	CSGM_START_
	qbool is_negative_range = zrange < 0;
	RELATED_ (map_save_as)
	RELATED_ (VECTOR3_G entity_add_alpha_brushes)
	
	stringlist_t list_vertex_strings = {0}; // For entire entity.

	for_each_const_brush_not_a_patch (brushnum, d_ent, r_brush)
		// Now determine quadrants for each row
		for_each_const_brush_face (facenum, r_brush, r_face)
			stringlistappend_face_vertex_unique (&list_vertex_strings, r_face);
		for_each_end // next face
	for_each_end // next brush

	for_each_stringlist_string (n, list_vertex_strings, sxy)
		vec3_t point3d; Math_atov3 (sxy, point3d);
		vec3_t brmins; VectorCopyDestSrc (brmins, point3d);
		vec3_t brmaxs; VectorCopyDestSrc (brmaxs, point3d);

		for (int d = 0; d < 3; d ++) {
			brmins[d] -= 48;
			brmaxs[d] += 48;
		} // for

		float f = (point3d[2] - zlow)/zrange; // (100 - 0) / 384
		if (is_negative_range) {
			f = bound (0, fabs(f), 1); // 384 - 384 // -384   0 - (384) -384
			if (f == 0) f = 0; // Get rid of stupid negative 0 problem.
			//f = 1 - f;
		} else {
			f = bound (0, f, 1);
			if (f == 0) f = 0; // Get rid of stupid negative 0 problem.
		}

		va_super (texturename, 128, "common/alpha_%1.2f", f);
		String_Edit_Replace (texturename, sizeof(texturename), ".", "_");
		brush_add_cubeoid_with_texture (d_ent, brmins, brmaxs, texturename);

		CSGM_NUM_DONE_PLUS_
	for_each_end // next string

	stringlistfreecontents (&list_vertex_strings);
	CSGM_END_RETURN_ONLY_
}

// num_alpha_brushes_added
// _heightmap 0 to 384
// ENTITY:	"_heightmap" "0 to 384" (ENTITY ONLY NO WORLD)
// ACTION:	takes any entity with this
//			1. Create an string array of all non-common/ vertex points in every face.
//          2. Uniques the list.
//			3. Creates an alpha brush (0 to 100) at each unique vertex based on distance from heightmap
// 
//			4. Because we use the faces, we do not care about metric.



int entity_perform_radius_num_alpha_brushes_added (entityx_s *d_ent, vec4_t data)
{
	//qbool is_negative_range = zrange < 0;
	RELATED_ (map_save_as)
	RELATED_ (VECTOR3_G entity_add_alpha_brushes)
	int num_added = 0;
	stringlist_t list_vertex_strings = {0}; // For entire entity.

	float radius = data[0];
	vec3_t radius_origin = {data[1], data[2], data[3]};

	for_each_brush_not_a_patch (brushnum, d_ent, r_brush)
		for_each_brush_face (facenum, r_brush, r_face)
			// DO EVERY VERTEX.EVEN CAULK
			stringlistappend_face_vertex_unique (&list_vertex_strings, r_face);
		for_each_end // next face
	for_each_end // next brush

	for_each_stringlist_string(sidx, list_vertex_strings, sxy)
		vec3_t point3d; Math_atov3 (sxy, point3d);
		vec3_t brmins, brmaxs;
		VectorCopyDestSrc (brmins, point3d);
		VectorCopyDestSrc (brmaxs, point3d);
		

		for (int d = 0; d < 3; d ++) {
			brmins[d] -= 48;
			brmaxs[d] += 48;
		} // for

		vec3_t center;
		VectorCenter (brmins, brmaxs, center);

		vec3_t dist3;
		VectorSubtract	(center, radius_origin, dist3);

		float dist = VectorLength(dist3);
		
		float alpha = 0;
		if (dist >= radius)
			alpha = 0;
		else
			alpha = 1 - (dist / radius);

		alpha = bound (0, fabs(alpha), 1); // 384 - 384 // -384   0 - (384) -384
		if (alpha == 0) alpha = 0; // Get rid of stupid negative 0 problem.

		va_super (texturename, 128, "common/alpha_%1.2f", alpha);
		String_Edit_Replace (texturename, sizeof(texturename), ".", "_");
		brush_add_cubeoid_with_texture (d_ent, brmins, brmaxs, texturename);

		num_added ++;
	for_each_end // for


	stringlistfreecontents (&list_vertex_strings);
	return num_added;
}

// CSG_CME__HEIGHTMAP
int csge_heightmap (map_t *hm, ccs *MYKEY) // ENTITY ONLY NO WORLD
{
	CSGM_START_

	// CSG_CME__HEIGHTMAP
	for_each_entity_with_key_afterworld_original_2 (entnum, d_ent, hm, _heightmap, MYKEY)
		vec3_t vheightmap = {0};
		Math_atov3_ex (_heightmap, vheightmap);
		float range = vheightmap[2] - vheightmap[0]; // Max 384 - mins 0

		int num_alpha_brushes_made =
			_csge_heightmap_ent_num_added (d_ent, vheightmap[0], vheightmap[2], range);
		
		CSGM_KEY_DELETE_ENT_ (entnum, csge_heightmap);
		
		CSGM_NUM_DONE_PLUS_EQ (num_alpha_brushes_made);
	for_each_end // next entity

	CSGM_END_
}

// csg_perform_q13.c.h

// Q3 JACK ( -328 1472 144 ) ( -328 1360 144 ) ( -328 1472 160 ) METAL1_3 [ 0 1 0 0 ] [ 0 0 -1 0 ] -0 1 1
// Q1      ( -328 1352 144 ) ( -328 1472 144 ) ( -336 1472 144 ) METAL1_3 0 0 0 1.000000 1.000000

int map_change_textures_q1 (map_t *hm, ccs *skytexturename, ccs *texturesfolder)
{
	CSGM_START_
	//ENT_DEFINE (d_world, hm, WORLD_0);

	// Add standard Quake 3 worldspawn keys.
	map_set (hm, WORLD_0, "_keeplights", "1");
	RELATED_ (csgc_brushfacer)

	// Lights
	// NO: Add "spawnflags" "1" to all "light" entities. (RADIUS LIGHT)
	// YES: Set "spawnflags" "1" on all light prefixed entities.
	// Baker: This will strip light starts off.  Since DarkPlaces does not really support
	// Q3 dynamic lights, this is the best solution anyway and Q3 dynamic lights are awkward
	// and involve .shader
	for_each_entity_classname_startswith_2 (entnum, this_light, hm, /*prefix:*/ "light")
		#define Q1_LIGHT_STARTS_OFF_1		1
		#define SPAWNFLAGS_RADIUS_LIGHT_1	1
		//int light_spawnflags = entity_get_int32 (this_light, "spawnflags");

		//Flag_Add_To (light_spawnflags, SPAWNFLAGS_RADIUS_LIGHT_1);
		entity_set_int32 (this_light, "spawnflags", SPAWNFLAGS_RADIUS_LIGHT_1); // STOMP VALUE and set RADIUS LIGHT
	for_each_end // for_each_entity

	// Each texture
	// 1. Lower case it.
	// 2. "sky" starts with ==> set the skytexturename
	// 3. Leading star (*) to POUND (#)
	// 4. Prefix with texture folder (if isn't the sky)

	for_each_entity_2 (entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face_2 (facenum, d_face, hm, entnum, brushnum)
				// 1. Lower case the allocated string.
				String_Edit_To_Lower_Num_Done (d_face->pbrtexture);

				// 2. "sky" starts with ==> set the skytexturename
				if (String_Starts_With (d_face->pbrtexture, "sky")) {
					face_texture_set (d_face, skytexturename);
					continue;
				}

				// 2. Leading star (*) to POUND (#)
				if (d_face->pbrtexture[0] == '*')
					d_face->pbrtexture[0] = '#';

				// There are no extensions on textures
				// ( -3392 -696 848 ) ( -3392 -696 0 ) ( -3392 -152 848 ) texturecan/others_0029_facade [ 0 1 0 128 ] [ 0 0 -1 480 ] 0 0.5 0.5

				// 4. Prefix with texture folder
				face_texture_set_fmt (d_face, "%s/%s", texturesfolder, d_face->pbrtexture);
				CSGM_NUM_DONE_PLUS_
			for_each_end // next face
		for_each_end // next brush
	for_each_end // next entity

	CSGM_END_FUNC_
}


int csgc_q13 (map_t *hm)
{
	CSGM_START_
	int num_done_here = map_change_textures_q1 (hm, "skies/purplenebula_sky_low", "id1wad");
	CSGM_NUM_DONE_PLUS_EQ (num_done_here)
	CSGM_END_FUNC_
}

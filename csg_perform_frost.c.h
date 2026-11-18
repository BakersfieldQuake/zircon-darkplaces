// csg_perform_frost.c.h

RELATED_ (csge_heightmap) //  _heightmap
#define frost_algo_none_0		0
#define frost_algo_heightmap_1	1	//"_frost" "heightmap 0 384"
#define frost_algo_radius_2		2	//"_frost" "radius 30 200 200 0" // radius and x y z center 5 things

static int _csgax_frost_phase_sub (map_t *hm, ccs *world_frost)
{
	int num_added = 0;
	int world_algo = frost_algo_none_0;
	float world_frost_range = 0;
	vec3_t world_height_minmax = {0};
	vec4_t world_radius_and_center = {0}; // radius x y z

	if (world_frost) {
		char algoname[128];
		space_split1 (world_frost, algoname);

		if (String_Match(algoname, "heightmap")) {
			world_algo = frost_algo_heightmap_1;
			Math_atov2 (&world_frost[STRINGLEN("heightmap")], world_height_minmax);
			world_frost_range = world_height_minmax[1] - world_height_minmax[0]; // Max 384 - mins 0
		} else if (String_Match(algoname, "radius")) {
			world_algo = frost_algo_radius_2;
			Math_atov4 (&world_frost[STRINGLEN("radius")], world_radius_and_center);
		} else {
			Con_PrintLinef ("Unknown algo " QUOTED_S, algoname);
			return 0; //
		}
	}

	//for_each_entity_classname_original_2 (entnum, d_ent, plist, d_ent)
	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _frost, CSG_CME__FROST) // _frost
	//for_each_entity_original_count_undeleted (entnum, plist, d_ent)
		//ccs *entity_frost = map_get (plist, entnum, "_frost");
		//if (entity_frost && String_Starts_With (entity_frost, TIMESTAMP_WAS_SET_PREFIX_0_SPACE_MINUS))
		//	entity_frost = NULL;

		qbool shall_do_this_entity = world_frost || _frost;

		if (shall_do_this_entity == false)
			continue;

		// How much frost
		int entity_algo = frost_algo_none_0;
		Vector3DeclareCopyEx (entity_height_minmax, world_height_minmax);
		Vector4DeclareCopy (entity_radius_and_center, world_radius_and_center);
		float entity_frost_range = 0;
		if (_frost) {
			char algoname[128];
			space_split1 (_frost, algoname);
			if (String_Match(algoname, "heightmap")) {
				entity_algo = frost_algo_heightmap_1;
				Math_atov2 (&world_frost[STRINGLEN("heightmap")], entity_height_minmax);
				entity_frost_range = entity_height_minmax[1] - entity_height_minmax[0]; // Max 384 - mins 0
			} else if (String_Match(algoname, "radius")) {
				entity_algo = frost_algo_radius_2;
				Math_atov4 (&_frost[STRINGLEN("radius")], entity_radius_and_center);
			} else {
				Con_PrintLinef ("Unknown algo " QUOTED_S, algoname);
				return 0; //
			}
		} else {
			entity_algo = world_algo;
			if (world_algo == frost_algo_heightmap_1) {
				// Inherit from world
				entity_frost_range = entity_height_minmax[1] - entity_height_minmax[0]; // Max 384 - mins 0
			} else if (world_algo == frost_algo_radius_2) {
				// Everyhing is in vec4
			} else {
				Con_PrintLinef ("Unknown world_algo 2 %d", world_algo);
				return 0;
			}
		} // if not a frost entity

		int num_added_here;
		if (entity_algo == frost_algo_heightmap_1)
			num_added_here = _csge_heightmap_ent_num_added (d_ent, entity_height_minmax[0], entity_height_minmax[1], entity_frost_range);
		else
			num_added_here = entity_perform_radius_num_alpha_brushes_added (d_ent, /*vec4*/ entity_radius_and_center);

		num_added += num_added_here;

//		if (entnum != WORLD_0 && entity_frost)
	//		map_set (plist, entnum, "_frost", s0_plus_timestamp);
#pragma message ("DO A DELETE OF KEY HERE")
	for_each_end // for_each_entity_original_count_undeleted

//	if (world_frost)
//		map_set (plist, WORLD_0, "_frost", s0_plus_timestamp);
#pragma message ("DO A DELETE OF KEY HERE")
	return num_added;
}



// Baker: The full map path must be provided.  The current working directory is unpredictable.
// "_frost" "heightmap 0 384" zz_terrain/snow_1.tga
// "_frost_texture" "zz_terrain/snow_1" default is zz_terrain/snow_1.tga

// IDEA ONLY ... NOT IMPLEMENTED
// "_frost_ignore_names" "trailswith _sh,matches liquids,matches skies" // or something.

// This needs map full path to tear in shaders.
// CRAZY special names.
CALLERS_ (CSG_Perform)
// mappath_realpath_full .. "c:/galaxy/zircon/maps/zec_park.map"
int csgax_frost (map_t *hm, ccs *MYKEY, ccs *mappath_realpath_full) // _frost
{
	CSGM_START_
	ccs *world_frost = map_get(hm, WORLD_0, CSG_CME__FROST); // "_frost"

	int beyond_idx = hm->count;
	entnum_int first_entnum_with_frost = map_find_key_string_ANY_start_beyond (hm, CSG_CME__FROST, WORLD_0, beyond_idx, NULL);

	if (first_entnum_with_frost == not_found_neg1)
		return 0; // No one has a _frost key

	stringlist_t list_textures = {0}; //
	stringlist_t list_textures_excluded = {0}; //

	va_copy_16384 (mygamedir, mappath_realpath_full);
	File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (mygamedir); // Strip mymap.map
	File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (mygamedir); // Strip maps/mymap.map (Now in gamedir)

	char frosttexturename[1024] = "zz_terrain/snow_1"; // _frost_texture default

	ccs	*frostkey = map_get (hm, WORLD_0, CSG_CME__FROST_TEXTURE); // "_frost_texture"

	if (frostkey && frostkey[0])
		c_strlcpy (frosttexturename, frostkey);

	for_each_entity_2 (entnum, d_ent, hm) // _frost
	//for_each_entity_undeleted (entnum, hm, d_ent)
		qbool shall_do_this_entity = world_frost ? true : false;
		if (shall_do_this_entity == false) {
			ccs *_frost = map_get (hm, entnum, CSG_CME__FROST); // "_frost"
			//if (entity_frost && String_Starts_With (entity_frost, TIMESTAMP_WAS_SET_PREFIX_0_SPACE_MINUS))
			//	entity_frost = NULL;

			if (_frost) shall_do_this_entity = true;
		}
		// Accumulate list of textures
		for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
			for_each_brush_face (facenum, d_brush, d_face)
				if (!d_face->pbrtexture)
					continue;

filter:
				; // oblig
				qbool shall_exclude = false;
				// decals_sh/concrete_0001_dirtystain2
					 if (String_Contains (d_face->pbrtexture, "_sh/"))	shall_exclude = true;
				else if (String_Starts_With (d_face->pbrtexture, "liquids/"))	shall_exclude = true;
				else if (String_Starts_With (d_face->pbrtexture, "common/"))	shall_exclude = true;
				else if (String_Starts_With (d_face->pbrtexture, "zfx_animated/"))	shall_exclude = true;
				else if (String_Starts_With (d_face->pbrtexture, "fence/"))	shall_exclude = true;
				else if (String_Starts_With (d_face->pbrtexture, "skies/"))	shall_exclude = true;

				if (shall_exclude) {
					stringlistappend_unique_didAdd (&list_textures_excluded, d_face->pbrtexture);
					//Con_PrintLinef ("Texture " QUOTED_S " was excluded due to rules", pbr->pbrtexture);
					continue;
				}

replace:
				stringlistappend_unique_didAdd (&list_textures, d_face->pbrtexture);

				va_super (newtexturename, 1024, "frost_sh/%s", File_URL_SkipPath (d_face->pbrtexture));

				// Old: textures/			azirc0/floor_lite_slate
				// textures/				snow_sh/floor_lite_slate
				face_texture_set (d_face, newtexturename);

				// Add if new
			for_each_end // face
		for_each_end // brush
	for_each_end // entity

	baker_string_s *bsa = BakerString_Create_Malloc ("");
	stringlistsort_unique (&list_textures);

	Con_PrintLinef ("Replaced:");
	stringlist_condump_with_linenums (&list_textures);

	Con_PrintLinef ("Excluded:");
	stringlist_condump_with_linenums (&list_textures_excluded);

emit_your_map_shader:
	//ccs *yourmapname = "yourmap";
	for_each_stringlist_string(n, list_textures, sxy)
		va_super (newtexturename, 1024, "frost_sh/%s", File_URL_SkipPath (sxy /*d_face->pbrtexture*/));

		//BakerString_CatLinef (bsa, "textures/snow_sh/floor_lite_slate", newtexturename);
		BakerString_CatLinef (bsa, "textures/%s", newtexturename);
		BakerString_CatLinef (bsa, "{");
		BakerString_CatLinef (bsa, "	{");
		//BakerString_CatLinef (bsa, "		map textures/azirc0/floor_lite_slate");
		BakerString_CatLinef (bsa, "		map textures/%s", sxy);
		BakerString_CatLinef (bsa, "	}");
		BakerString_CatLinef (bsa, "	{");
		//BakerString_CatLinef (bsa, "		map textures/zz_terrain/snow_1.tga");
		BakerString_CatLinef (bsa, "		map textures/%s.tga", frosttexturename);
		BakerString_CatLinef (bsa, "		blendFunc blend");
		BakerString_CatLinef (bsa, "		alphaGen vertex ");
		BakerString_CatLinef (bsa, "	}");
		BakerString_CatLinef (bsa, "	{");
		BakerString_CatLinef (bsa, "		map $lightmap");
		BakerString_CatLinef (bsa, "	}");
		BakerString_CatLinef (bsa, "}");

		CSGM_NUM_DONE_PLUS_
	for_each_end // Each string

	// Open scripts/shaderlist.txt
	//ccs *your_map_name = "maps/mymap.map";
	va_copy_16384 (your_map_name_no_path_no_ext, File_URL_SkipPath (mappath_realpath_full));
		File_URL_Edit_Remove_Extension (your_map_name_no_path_no_ext);

	va_super  (bareshaderfilename, MAX_OSPATH_EX_1024, "map_%s_frostgen", your_map_name_no_path_no_ext);

	va_super (shaderrealpath, MAX_OSPATH_EX_1024, "%s/scripts/map_%s_frostgen.shader", mygamedir, your_map_name_no_path_no_ext);

	Con_PrintLinef ("map shader file generated for _frost is %s", shaderrealpath);

	// mxl_phong //entry is shaderlist.txt

write_autogen:
    ; // oblig
	//FS_WriteFileStringQuick (myshadername, bsa->string);
	int isok2 = FS_SaveFileString_Is_Ok_Unsafe (shaderrealpath, bsa->string);
	if (!isok2) {
		Con_PrintLinef ("Couldn't write %s", shaderrealpath);
	}

	BakerString_Destroy_And_Null_It (&bsa);

	va_super (scripts_txt_realpath, MAX_OSPATH_EX_1024, "%s/scripts/shaderlist.txt", mygamedir);

	//va_super (realpath, MAX_OSPATH_EX_1024, "%s

	//Con_PrintLinef ("shaderrealpath is %s", shaderrealpath);
	// Now see if the autogen was in the shader

read_scripts:
    ; // oblig
	char *tempza = FS_SysStringFromFile (scripts_txt_realpath) ; // "scripts/shaderlist.txt");
	if (!tempza)
		Con_PrintLinef ("Couldn't open %s", scripts_txt_realpath);

	if (tempza && String_Contains (tempza, bareshaderfilename) == false) {
		baker_string_s *bsa2 = BakerString_Create_Malloc ("");
		BakerString_CatC		(bsa2, tempza);
		BakerString_CatLinef	(bsa2, bareshaderfilename);

		RELATED_ (CSG_CMDLiNE_REWRITE_RELOAD_)
#if 0
		Clipboard_Set_Text (bsa2->string);
#endif

write_scripts:
    ; // oblig
		int isok2 = FS_SaveFileString_Is_Ok_Unsafe (scripts_txt_realpath, bsa2->string);
		if (!isok2) {
			Con_PrintLinef ("Couldn't write %s", scripts_txt_realpath);
		}

		BakerString_Destroy_And_Null_It (&bsa2);
	}
	Mem_FreeNull_ (tempza);

	stringlistfreecontents (&list_textures);
	stringlistfreecontents (&list_textures_excluded);

	_num_done = _csgax_frost_phase_sub (hm, world_frost);
	#pragma message ("DO A DELETE OF KY HERE")
	CSGM_END_RETURN_ONLY_
}


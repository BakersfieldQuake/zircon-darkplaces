// csg_table.h

#define CSG_TABLE 1

// ALPHABETICAL ORDER.  NOT RUNTIME ORDER
RELATED_ (Con_csg_f)

// #define DEF_(csg_console_name, func, description, long_explain) // Nada
// #define Q3MAP_PRE(MYSTRING,MYFUNC,EXAMPLE,DESCRIBE,WORLD_OR_ENTITY_TYPE,APPLIES_TO) // Nada

//	args	csgtool				.map keyword						func							keytype		usage	desc
//  keytype		usage	desc


DEF_ (CSG_BRUSH_DELETE_IF_NONDOM,	csgc_brush_delete_if_nondom,		"non-most common texture world brushes are deleted", "Finds the most dominant world texture that is not specifically common/caulk and deletes all world brushes without that texture.  Does not affect entities.  Used to reduce lightmap storage space for AAFTER map in Shambler Invaders.")
DEF_ (CSG_BRUSH_DELETE_IF_TEXTURE,	csgc_brush_delete_if_texture,		"delete brushes with texture", "")
DEF_ (CSG_BRUSHFACER,				csgc_brushfacer,					"up-faces retexture to common/up", "")
DEF_ (CSG_BRUSHFACER_ON_TEXTURE,	csgc_brushfacer_specific_texture,	"brushfacer on a single texture", "")
DEF_ (CSG_BRUSHUNFACER,				csgc_brushunfacer,					"non up-faces retexture to common/caulk", "")
DEF_ (CSG_BRUSHUNFACER_ON_CLASSNAME,csgc_brushfacer_specific_classname,	"brushunfacer on a single classname", "")
//DEF_ (CSG_CARVE_INCLUDES,			csgc_carve_includes,				"It carves out the includes as textures but they can be func_group too", "") // tz_carve.map

DEF_ (CSG_CAULKWORLD,				csgc_caulk_non_common_non_sky_containing_textures,"texture world brushes to caulk (not entities)", "caulks textures that do not start with \"common/\" so it will not replace common/nodraw, etc.  It also will not replace any texture containing \"sky\" in the name.")
DEF_ (CSG_CLASSNAME_DELETE,			csgc_classname_delete,				"deletes all instances of a classname.", "")
DEF_ (CSG_CLIPBOARD_ADD_ENTS,	csgc_add_point_entities_on_clipboard,	"add point entities to .map from clipboard", "")
DEF_ (CSG_CSG_RUN,					csgc_csg_run,						"run as CSG command line process","")
DEF_ (CSG_DETAIL_AUTO,				csgc_detail_auto,					"automatically make sloped brushes (non-cubeoid) into detail brushes unless they are world bounded, _no_detail_auto key will exclude a func_group","")
DEF_ (CSG_DETAIL_WEDGE_SPLIT_AUTO,	csgc_detail_wedge_split_auto,		"automatically split terrain wedges","")
DEF_ (CSG_FLAT_GROUND_RETEXTURE,	csgc_flat_ground_retexture,			"retextures all faces with flat ground (z=0)", "Retextures flat ground at z 0 (or provided value) all brushes to common/up.  World and all entities done.")
DEF_ (CSG_FLAT_TRIM,				csgc_flat_trim,						"Adds _flat _noflat _trim to textures", "All entities done, but only within same unit.")
DEF_ (CSG_FLOOR_EVERYTHING,			csgc_floor_every_brush_to_specified_z, "Floors all world brushes, then entities to ground level	", "Floors all world brushes -- then all entities by brush move -- to specified ground level.  No entity keys are altered -- that means \"origin\" does not change, for instance.  The flooring process uses texturelock.  Q3 patches are moved too.  If z floor is not specified as an argument, it will be zero.")
DEF_ (CSG_GROUND_CARVE,				csgc_ground_carve,					"Carves at bottom of floor to make a detail brush above and a structural brush below for detail_auto process later on.", "")
DEF_ (CSG_INCLUDE_SHEET_GEN,		csgc_includes_sheet,				"make maps/#includes_sheet.map from maps/#includes/*.map", "")
DEF_ (CSG_INFO_PLAYER_START_DROP_CENTER,csgc_info_player_start_drop_center,	"Drop an info player start from center of map [READ ONLY]", "")
DEF_ (CSG_INFODUMP,					csgc_infodump,						"print information on .map [READ ONLY]", "")
DEF_ (CSG_PRINT_LOWEST_GROUND,		csgc_print_lowest_ground,			"print lowest ground level entity/brushnum [READ ONLY]", "")
DEF_ (CSG_PRINTFACES,				csgc_printfaces,					"print face information for entity/brushnum [READ ONLY]", "")
DEF_ (CSG_PRINTTEXTURES,			csgc_printtextures,					"print textures [READ ONLY]", "")
DEF_ (CSG_PRINTUNIT,				csgc_printunit,						"print entity/brushnum [READ ONLY]", "")
DEF_ (CSG_PROCESS_TERRAIN,			csgc_process_terrain,				"flat texture to grass, other to rocky -- weld", "")
DEF_ (CSG_QUAKE1_TO_QUAKE3,			csgc_q13,							"Converts Q3 J.A.C.K. 220 .map from Quake 1 to Quake3", "Converts Q3 J.A.C.K./Valve 220 .map from Quake 1 to Quake3 by adjusting texturenames and adding light spawnflag 1 for radius lighting.  Also sets _keeplights 1 in worldspawn.  Despite the name, map version J.A.C.K. 220 is supported by TrenchBroom and Radiant")
// replacetextureadjacent textures/tsh/rocky_noflat 
DEF_ (CSG_R_LISTMAPSHADERS,			csgc_r_listmapshaders,				"list map shaders", "")
DEF_ (CSG_REPLACETEXTUREADJACENT,	csgc_replacetextureadjacent,		"Replace adjacent textures, but I don't think it works", "")
DEF_ (CSG_REWRITE,					csgc_rewrite,						"rewrite a .map to validate parser", "")
//  IDEA ONLY DEF_ (CSG_REWRITE_NOTEX,			csgc_rewrite_notex,					"rewrite .map with all textures same", "")
DEF_ (CSG_RIDGE_VALLEY,				csgc_ridge_valley,					"appends _ridge _valley  _somethingelse to texturenames", "")
DEF_ (CSG_ROTATEUNIT,				csgc_rotateunit,					"rotate entity/brushnum by yaw degrees", "")
DEF_ (CSG_SKY_ENCASE,				csgc_sky_encase,					"Encases map in sky cubeoid with specified height", "Encases map in a sky cubeoid with specified height above 0.")
DEF_ (CSG_WELD,						csgc_weld,							"merge same-texture adjacent z-flat triangles into rectangles", "Finds same-z adjacent flat same textured triangles and turns them into rects.  Then merges rects by row where possible.")
DEF_ (CSG_WORLDSPAWN_KEY_DELETE,	csgc_worldspawn_key_delete,			"delete [keyname] [value]", "")
DEF_ (CSG_WORLDSPAWN_KEYS_PRINT,	csgc_worldspawn_keys_print,			"print worldspawn keys", "")
DEF_ (CSG_WORLDSPAWN_KEY_SET,		csgc_worldspawn_key_s,				"set [keyname] [value]", "")

RELATED_ (brush_s)

Q3MAP_PRE (CSG_CMW__CSG_TEST, csgw_csg_test, "_csg_test 1", "World key.  Do nothing but rewrite validating CSG reader/writer", WORLD_KEY, non_world)



// csqw_detail_auto requires csqw_ground_carve
Q3MAP_PRE (CSG_CMW__DETAIL_AUTO,		csgw_detail_auto,				"non-cubeoids become detail brushes unless marked _no_detail_auto", "", WORLD_KEY, non_world)
//Q3MAP_PRE (CSG_CMW__FLAT_NOFLAT_TRIM_DOMINANT,	csgw_flat_noflat_trim_dominant,	"appends _flat _noflat and then _trim to supplied texturename", "", WORLD_KEY, non_world)


//   "#includes/"
Q3MAP_PRX (CSG_CET_INCLUDES, csgax_includes, "#includes/armundos.tga", "Import a map before compile.  The brush is removed.  If part of an entity like a func_group, the entity is removed", tz_carve, any)
//novel:
//	LATER - FOR WATER CURRENTS THAT DONT WORK BECAUSE TRIGGER IS BBOX  #include "fs_baker_csg_clone_trigger.c.h"		// "_clone_trigger" "trigger_push" .. makes entity with same brushes composed of common/trigger and the classname is "trigger_push"
//	LATER -- GARUX STUFF #include "fs_baker_csg_rename.c.h"				// "_rename" "misc_model_torch=misc_model,misc_mdl_entity_kickable=misc_mdl_entity"

//
//serious:
//	// _layout_to_boxes must run before "_clone_classname"
//	NO I DO NOT THINK WE WILL USE #include "fs_baker_csg_layout_to_boxes.c.h"		// "_layout_to_boxes" "1" (MUST RUN BEFORE "_clone_classname")
//
//	// "_layout_to_boxes" may eventually spawn random entities.
//	// How will cabinents and such be renamed?  We have room numbers, we should mydoor ===> myroom_room_1
//
Q3MAP_PRE (CSG_CME__CLONE_CLASSNAME, csge_clone_classname, "_clone_classname light_csqc_emitter;origin_z += 81;spawnflags = 1", "Entity key for deeplair torches.  Actions: light_offset, emitter_offset, origin_z, spawnflags", baker_temple6, non_world)


Q3MAP_PRE (CSG_CMW__RENAME, csgw_rename_GARUX, "_rename misc_model_torch=misc_model,misc_mdl_entity_kickable=misc_mdl_entity", "Comma equals pairs classnames get renamed to a different classname, in part to support non-Baker Q3map2 compilers", ????, non_world)


Q3MAP_PRE (CSG_CME__ORIGINMAKE, csge_originmake, "_originmake 1", "Give a brush model an origin brush the center", zec_rotating_train, any entity)
Q3MAP_PRE (CSG_CME__ATOMIZE, csge_atomize, "_atomize 1", "Make every brush of an entity a separate entity with the same field values, then remove it.  Used to make a sliced up func_wall_breakable into 100 func_wall_breakables", around17, any entity)

//	#include "fs_baker_csg_collision_body.c.h"		// "_collision_body" "models/vehicles/psx/4door_car01.md3 184.5 76.703125 65.21875"
Q3MAP_PRE (CSG_CMW__FLOORAGE, csgw_floorage, "_floorage 1 or _floorage maps_sh/grass", "Magically creates square surfaces ignoring classname func_group_terrain (or anything starting with func_group_terrain) using specified texturename or if numeric then defaults to maps_sh/fang_grass_only", pac3, world)
Q3MAP_PRE (CSG_CME__HEIGHTMAP, csge_heightmap, "_heightmap 0 to 384", "Entity only, no world.  Creates heightmap alpha point brushes for an entity.  The map compiler may sometimes chop up brushes -- seems to be primarily large ones -- causing it to be slightly imperfect sometimes.", zec_heightmap, func_group)

Q3MAP_PRE (CSG_CMW__SKY_ENCASE, csgw_sky_encase, "_sky_encase 1 or _sky_encase 8192 skies/desert2_sky", "Encases map in a sky cubeoid with specified height above 0.  It caulks the outside of the skybox.  Only 1 argument uses the defaults.", wood_bridge20_shear_both_odds, world)
Q3MAP_PRE (CSG_CMW__SKY_DECAL, csgw_sky_decal, "_sky_decal 3 200 -32 textures/decals_sh/dirtystain1 512 512", "Drops decals at z using cubeoid collision.  WIP.", wood_bridge_titl_snow, world)

// Baker: August 10 2026 - frost is irregular and needs to write shader things
Q3MAP_PRE (CSG_CMW__NODES_FROM_TEXTURE, csgw_nodes_from_texture, "_nodes_from_texture texturecan/wood_0047d_track texturecan/wood_0047d", "Generates angled path_corner entities on each surface with specified texture, 1 per brush face. Replaces texture0 with texture1.  Textures must be on func_group because it requires track_id 1 or track_id 18.  Must be looped or crash?", trainway5x, world)
Q3MAP_PRX (CSG_CME__FROST, csgax_frost, "_frost heightmap 0 384", "world key _frost_texture zz_terrain/mytexture specifies frost texture, it defaults to zz_terrain/snow_1.  Hardcoded ignores starts with:liquids/,common/,zfx_animated/,fence/,skies/ or contains _sh/ because pre-processor does not parse shaders.", wood_bridge_titl_snow zec_frost, zec_frost)

				//	 if (String_Contains (d_face->pbrtexture, "_sh/"))	shall_exclude = true;
				//else if (String_Starts_With (d_face->pbrtexture, "liquids/"))	shall_exclude = true;
				//else if (String_Starts_With (d_face->pbrtexture, "common/"))	shall_exclude = true;
				//else if (String_Starts_With (d_face->pbrtexture, "zfx_animated/"))	shall_exclude = true;
				//else if (String_Starts_With (d_face->pbrtexture, "fence/"))	shall_exclude = true;
				//else if (String_Starts_With (d_face->pbrtexture, "skies/"))	shall_exclude = true;


// OLD DECAL ROADS "fs_baker_csg_decals.c.h"
// #include "fs_baker_csg_map_include_prefix_start_end.c.h"	// 

//
// Things that eliminate things should run first.
//
Q3MAP_PRE (CSG_CME__KEEP_EVERY_NTH, csge_keep_every_nth, "_keep_every_nth 3", "Keep every third entity by instance of classname/targetname combination - not random", wood_bridge20_shear_both_odds, any entity)
Q3MAP_PRE (CSG_CME__KEEP_PERCENT, csge_keep_percent, "_keep_percent 0.7", "Keep 70% randomly, remove based on dice roll", wood_bridge20_shear_both_odds, any entity)

Q3MAP_PRE (CSG_CME__ANGLE_JITTER, csge_angle_jitter, "_angle_jitter 25 50 100", "Add random +/- x y z to angles", NONE!, any entity)
Q3MAP_PRE (CSG_CME__LIGHT_RANGE, csge_light_range, "_light_range 50 200 or _light_range alt 50 200", "Sets light to random number in range of 50 to 200, [alt] will instead 50/50 roll and pick one or the other", wood_bridge20_shear_both_odds, wood_bridge)
Q3MAP_PRE (CSG_CME__MODELSCALE_RANGE, csge_modelscale_range, "_modelscale_range 1 3", "Random modelscale 1 to 3.  For misc_model but not locked to it, QuakeC entities use scale not modelscale.", wood_bridge20_shear_both_odds, misc_model)
Q3MAP_PRE (CSG_CME__ORIGIN_JITTER, csge_origin_jitter, "_origin_jitter 25 50 100", "Add random +/- x y z to origins", wood_bridge20_shear_both_odds, any entity)

//c:\galaxy\zircon\maps\#prefabs\wood_bridge\.map
Q3MAP_PRE (CSG_CMW__BRUSH_DELETE_IF_TEXTURE, csgw_brush_delete_if_texture, "_brush_delete_if_texture cc0-textures/concrete_rock_path_global", "Removes all brushes with any texture of that face. World key", ???, world)

Q3MAP_PRE (CSG_CME__AUTOMATIC_DROPTOFLOOR, csge_decal_automatic_droptofloor, "target _automatic_droptofloor", "target _automatic_droptofloor", inferno_5, any entity)

#undef DEF_
#undef Q3MAP_PRE
#undef Q3MAP_PRX

// END


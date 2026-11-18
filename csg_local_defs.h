// csg_local_defs.h

RELATED_ (CSG_GLOBAL CSG_LOCAL CSG_LOCAL_DEFS)

#pragma once


///



#define CSG_INCLUDES						"#includes/"
#define CSG_CARVE                          	"#carve/"
#define CSG_CARVE2                         	"#carve2/"
#define CSG_ADJACENTS                      	"adjacents"
#define CSG_ANGLE_JITTER                   	"angle_jitter"

//DONE
#define CSG_BRUSH_DELETE_IF_TEXTURE			"brush_delete_if_texture"
#define CSG_BRUSHFACER                     	"brushfacer"
#define CSG_BRUSHFACER_ON_TEXTURE			"brushfacer_on_texture"
#define CSG_BRUSHUNFACER                    "brushunfacer"
#define CSG_BRUSHUNFACER_ON_CLASSNAME       "brushunfacer_on_classname"
#define CSG_BRUSH_DELETE_IF_NONDOM			"brush_delete_if_nondom"
//#define CSG_CARVE_INCLUDES				"carve_includes"

#define CSG_CAULKWORLD                     	"caulkworld"
#define CSG_CLASSNAME_DELETE                "classname_delete"
#define CSG_CLIPBOARD_ADD_ENTS				"clipboard_add_ents"
#define CSG_CSG_RUN							"csg_run"
#define CSG_DETAIL_AUTO						"detail_auto"
#define CSG_DETAIL_WEDGE_SPLIT_AUTO			"detail_wedge_split_auto"
#define CSG_FLAT_GROUND_RETEXTURE			"flat_ground_retexture"
#define CSG_FLAT_TRIM						"flat_trim"
#define CSG_FLOOR_EVERYTHING				"floor_everything"
#define CSG_GROUND_CARVE					"ground_carve"
#define CSG_INCLUDE_SHEET_GEN              	"include_sheet_gen"
#define CSG_INFO_PLAYER_START_DROP_CENTER	"info_player_start_drop_center"
#define CSG_INFODUMP						"infodump"
#define CSG_PRINT_LOWEST_GROUND				"print_lowest_ground"
#define CSG_PRINTFACES						"printfaces"
#define CSG_PRINTTEXTURES					"printtextures"
#define CSG_PRINTUNIT						"printunit"
#define CSG_PROCESS_TERRAIN					"process_terrain"
#define CSG_QUAKE1_TO_QUAKE3				"quake1_to_quake3"
#define CSG_R_LISTMAPSHADERS				"r_listmapshaders"
#define CSG_REPLACETEXTUREADJACENT			"replacetextureadjacent"
#define CSG_REWRITE							"rewrite"
#define CSG_RIDGE_VALLEY					"ridge_valley"
#define CSG_ROTATEUNIT						"rotateunit"
#define CSG_WORLDSPAWN_KEY_DELETE			"worldspawn_key_delete"
#define CSG_WORLDSPAWN_KEY_SET				"worldspawn_key_set"
#define CSG_WORLDSPAWN_KEYS_PRINT			"worldspawn_keys_print"
#define CSG_WELD							"weld"
#define CSG_SKY_ENCASE						"sky_encase"




//////////////////////////////////////////////
#define CSG_COLLISION_BODY                 	"collision_body"
#define CSG_CUBEIT                         	"cubeit"
#define CSG_DECAL_AUTOMATIC                	"decal_automatic"
#define CSG_DECAL_DROP_DISTANCE            	"decal_drop_distance"
#define CSG_FLATZEROSETTEXTURE             	"flatzerosettexture"


#define CSG_CONCAT_ENTS_CLIPBOARD          	"concat_ents_clipboard"
#define CSG_WELD							"weld"







#define CSG_SKY_ENCASE_DECAL				"sky_encase_decal"

#define CSG_CET_INCLUDES					"#includes/"				// NON-WORLD TEXTURE NAME - requires func_group or such

// CET ?  TEXTURE NAME -- YES.  SINGLE POINT OF REFERENCE

#define CSG_CME__ANGLE_JITTER				"_angle_jitter"				// NON-WORLD KEY
#define CSG_CME__ATOMIZE					"_atomize"					// NON-WORLD
#define CSG_CME__AUTOMATIC_DROPTOFLOOR		"_automatic_droptofloor"	// DECAL KEY
#define CSG_CMW__BRUSH_DELETE_IF_TEXTURE	"_brush_delete_if_texture"	// WORLD KEY (but scope all)
//#define CSG_CMW__BRUSHFACER				"_brushfacer"				// WORLD KEY (scope world) - intended for terrain
#define CSG_CME__CLONE_CLASSNAME			"_clone_classname"			// NON-WORLD KEY "_clone_classname" "light_csqc_emitter,origin_z 81,spawnflags 1"
#define CSG_CMW__CSG_TEST					"_csg_test"					// WORLD KEY (scope all) - writes with no changes to test CSG
#define CSG_CMW__DETAIL_AUTO				"_detail_auto"				// WORLD KEY ==> _NO_DETAIL_AUTO ent key
#define CSG_CMW__FLOORAGE					"_floorage"					// WORLD KEY (but scope all)
#define CSG_CME__FROST						"_frost"					// ANY ENTITY
#define CSG_CME__HEIGHTMAP					"_heightmap"				// NON-WORLD KEY - YES NON-WORLD
#define CSG_CME__KEEP_PERCENT				"_keep_percent"				// NON-WORLD KEY
#define CSG_CME__KEEP_EVERY_NTH				"_keep_every_nth"			// NON-WORLD KEY
#define CSG_CME__LIGHT_RANGE				"_light_range"				// NON-WORLD KEY
#define CSG_CME__MODELSCALE_RANGE			"_modelscale_range"			// NON-WORLD KEY
#define CSG_CMW__NODES_FROM_TEXTURE			"_nodes_from_texture"		// WORLD KEY (but scope all)
#define CSG_CME__ORIGIN_JITTER				"_origin_jitter"			// NON-WORLD KEY
#define CSG_CME__ORIGINMAKE					"_originmake"				// NON-WORLD
#define CSG_CMW__SKY_ENCASE					"_sky_encase"				// WORLD KEY
#define CSG_CMW__SKY_DECAL					"_sky_decal"				// WORLD KEY
#define CSG_CMW__RENAME						"_rename"					// WORLD KEY (scope all) _rename misc_model_torch=misc_model, ..

// Secondary
#define CSG_CME__FROST_TEXTURE				"_frost_texture"			// ANY ENTITY Defaults to zz_terrain/snow_1
#define CSG_CME__DROP_DISTANCE				"_drop_distance"			// DECAL KEY


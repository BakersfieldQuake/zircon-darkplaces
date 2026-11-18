// csg_global.h

#define CSG_GLOBAL
RELATED_ (CSG_LOCAL)

#include "csg_global_macros.h"

// SEPT 28 2024

// https://developer.valvesoftware.com/wiki/MAP_(file_format)
// ( x1 y1 z1 ) ( x2 y2 z2 ) ( x3 y3 z3 ) path/texturename
//  [ Ux Uy Uz Uoffset ] [ Vx Vy Vz Voffset ] rotation Uscale Vscale SurfaceFlag ContentsFlag Value
//
RELATED_ (map_move_all_brushes_patches)



#define CSG_EPSILON_0_01	0.01


typedef struct {
    vec3_t normal;
    float dist;
} face_plane_t;

RELATED_ (flat_ground_top_0_0_NEG1)

void PlaneReverse (face_plane_t *plane);
void PointsFromPlane(const face_plane_t *plane, vec3_t p0, vec3_t p1, vec3_t p2);
void PlaneFromPoints(face_plane_t *plane, const vec3_t p0, const vec3_t p1, const vec3_t p2);

#define PlanePointDistance(point, plane) (DotProduct((point), (plane)->normal) - (plane)->dist) // Baker

#define BRUSH_FACE_MAX_POINTS_64 64

#define RIDGE_NONE_0		0
#define RIDGE_PEAK_1		1
#define RIDGE_VALLEY_2		2
#define RIDGE_LOWER_ONE_4	4
#define RIDGE_HIGHER_ONE_8	8

typedef struct _brushrow_s {
    vec3_t a;            // ( 480 704 -260 )	// October 18 2026 - Clockwise order says Google.
    vec3_t b;            // ( 480 672 -260 )	// Clockwise when viewed outside the brush.
    vec3_t c;            // ( 480 704 -132 )

    //Z_StrDup_Realloc (&d_face->pbrtexture, newtexturename)
    RELATED_ (face_texture_set)
    char *pbrtexture;    // Z_StrDup_Realloc common/caulk         // https://developer.valvesoftware.com/wiki/MAP_(file_format) ...
    vec4_t tex_axis_s;        // [ 0 1 0 0 ] // [ Ux Uy Uz Uoffset ]    // tex_axis_s
    vec4_t tex_axis_t;        // [ 0 0 -1 0 ] // [ Vx Vy Vz Voffset ] // tex_axis_t
    int trail_count;    // There are 3 or more at the end... supposedly it can go up to 8
//( 32768 -34816 10240 ) ( 32768 -34816 -512 ) ( 32768 34816 10240 ) sky/treefall_nite_sky [ 0 1 0 -0 ]
//        [ 0 0 -1 -0 ] 0 0.5 0.5 // 3 numbers
//( 34816 34816 10240 ) ( 34816 34816 -512 ) ( 34816 -34816 10240 ) common/caulk [ 0 1 0 -0 ] [ 0 0 -1 -0 ]
//                             0 0.5 0.5 0 160 0 // 6 numbers

    // rotation s_scale t_scale surfaceflags contentflags value // value is like Q2 surfacelight
	RELATED_ (FT_SURFACEFLAGS_3)
    double faceparms[8];    // -0 1 1 0 160 0   // rotation Uscale Vscale SurfaceFlag ContentsFlag (<=structural vs. detail) Value
//                                                   SurfaceFlag 134217728 is detail
//                                                   ContentsFlag 128 = slime?  Doesn't make sense for q3?
    vec3_t              bfmins;
    vec3_t              bfmaxs;
    vec3_t              bfdims;
    int                 nsewcf_bits;

    RELATED_ (face_plane_t_calc)
	qbool				is_face_plane_set;

	RELATED_ (flat_ground_top_0_0_NEG1)
    face_plane_t		face_plane;    // face_plane_t_calc calculates

	RELATED_ (brush_fill_face_points)
	int num_points;
    vec3_t points[BRUSH_FACE_MAX_POINTS_64]; // October 18 2026 - hopefully in a certain order.

	qbool is_considered_deleted;
	
	
} __brushrow_s;

RELATED_ (face_bbox_touch_face brush_s)

void face_plane_t_calc(brushrow_s *d_face);

typedef struct _brushrowlist_st {
    int                 maxsize;
    int                 count;
    brushrow_s          *brushrow;    // PAGE
} brushrowlist_t;


typedef struct _patchrow_st {
// ( -126 -192 128 8 0 ) ( -126 -192 384 8 -6 ) ( -126 -192 640 8 -12 )
    float flots[300];

    // JULY 30 2025
    vec3_t              bpmins;
    vec3_t              bpmaxs;
    vec3_t              bpdims;
} patchrow_s;


// list of // ( -126 -192 128 8 0 ) ( -126 -192 384 8 -6 ) ( -126 -192 640 8 -12 )
typedef struct _patchrowlist_st {
    int maxsize;
    int count;
    patchrow_s          *patchrow;    // PAGE
} patchrowlist_t;

typedef struct _brush_st {
    brushrowlist_t      brushrowlist;
    int                 is_a_patch;    // PATCHES ARE BRUSHES BUT NOT BRUSH FACES. So d_ent->brushlist.count considers patches.

    char				*patch_texture;	// PATCH_TEXTURE liquids_lava/lava_blue_LX_1000_alpha_50  bare texture
    float				patch_rows;		// ( 9 3 536870920 16 1000 ) always 5 per column
    float				patch_cols;
    float				other3[3];
    patchrowlist_t      patchrowlist;	// ( -126 -192 128 8 0 ) ( -126 -192 384 8 -6 ) ( -126 -192 640 8 -12 )
//
    vec3_t              bxmins; // Derived from face points
    vec3_t              bxmaxs; // 
    vec3_t              bxdims; // 
    vec3_t              bxcenter;

    qbool               is_considered_deleted;
	int					has_bad_faces;
//
    RELATED_ (entity_perform_adjacents_num_done)
    int                 temp_num_adjacent_diffs;    // KEEP. adjacent texture blending
    int                 temp_num_adjacent_sames;    // KEEP. adjacent texture blending

RELATED_ (brush_has_a_west_face)
    qbool               is_cubeoid; // 6 sides
	qbool               is_terrain_wedge; // 5 sides - generally or never FLAT TOP
	qbool               is_flat_half_cubeoid; // 5 sides - FLAT TOP
    int                 cubeoid_index[FACE_MAX_6];    // facenum
    FACE_INT_e          triangle_face_bits;
	int					sloped_face_index;	// This a half-rect triangle X or Y wedge size with caulk
	int					top_slanted_index; // This the top of a terrain wedge if it has slope (otherwise use ceil)

	int					scutter_unit;
	int					scutter_entnum;
	int					scutter_brushnum;
	qbool				irregular;
	char				describe[128];
	RELATED_ (RIDGE_PEAK_2 RIDGE_VALLEY_3)
	int					ridgebits; // 1, none, 2 peak, valley RIDGE_PEAK_2
} brush_s;

RELATED_ (__brushrow_s brush_has_a_east_face brush_xy_touches_brush)

// /* 5 patchDef2 header numbers:
//        numcolumns, numrows, content flags, lightmap/tessellation LOD (horiz), subdiv/LOD (vert) */

// lightmap/tessellation LOD (horiz), subdiv/LOD (vert)
// The horizontal partitioning/subdivision level or spacing hint used during map
// compilation (lightmap/tessellation density).

// Why it matters: It guides Q3Map2 or the compiler on how finely to chop up the curved patch into flat triangles when generating lightmaps and collision surfaces.

// Control points:
// ( X Y Z S T )

//    patchDef2
//    {
//        liquids_lava/lava_blue_LX_1000_alpha_50
//        ( 9 3 536870920 16 1000 )
//        (
//            (
//                ( -126 -192 128 0 0 )
//                ( -126 -192 384 0 -6 )
//                ( -126 -192 640 0 -12 )
//            )
//            (MORE LIKE THESE)
//            (
//                ( -126 -192 128 8 0 ) ( -126 -192 384 8 -6 ) ( -126 -192 640 8 -12 )
//            )
//        )
//    }
//}

typedef struct _brushlist_st {
    /// maxstrings changes as needed, causing reallocation of strings[] array
    int                 maxsize;
    int                 count;
    brush_s             *brush;        // PAGE
} brushlist_t;

RELATED_ (entity_free)
typedef struct _entityx_s { // p_ent
    stringlist_t        pairslist;
    brushlist_t         brushlist;        // can be a patch!

    RELATED_ (map_metric_all)
    qbool               is_considered_deleted;
    vec3_t              bemins;    RELATED_ (entitylist_metric_world_brushes_with_walls) // will fill it in.
    vec3_t              bemaxs;    // will it do brush rows? Yes
    vec3_t              bedims; // It does not do entities.  That includes func_group.
    vec3_t              becenter;

    int                 original_brush_count; // September 1 2026 - Only used by carve so far on Sept 24 2026
    qbool               is_world_or_func_group; // map_metric_all sets this
    qbool               has_brushes; // map_metric_entity_sub sets this
	qbool				has_include_texture;
    char				texturename_work[Q3PATHLENGTH_64]; // csgmsub_find_delete_includes
} __entityx_s;

RELATED_ (ENTITY_WORLD_0 AFTER_WORLD_1)
typedef struct _entitylist_st {
    /// maxstrings changes as needed, causing reallocation of strings[] array
    int                 maxsize;
    int                 count;
    entityx_s           *entity;                        // p_ent (PAGE)
    int                 stage;                          // stage_csg .. etc.
    int                 func_t_stored_int;              // Dungeon callback for something.



    // Baker: October 15 2025 - this should almost never be used.  It's dangerous.  Per loop variable before is best.
    int /*danger*/      original_ent_count; /*ALWAZSET*/// Avoid searching new entities.
                                            // Note: modelgen's insert at 1 breaks this.
                                            // Could reload the map after that.

    vec3_t              bbentsmins;			// map_load_from_string must run
    vec3_t              bbentsmaxs;
    vec3_t              bbentsdims;

    int                 num_cubeoids;
    int                 num_non_cubeoid_6_facer;

	stringlist_t		list_textures;  // JIT
	int32list_s			list_sizes2;	// JIT width/height
} map_t;

RELATED_ (map_freecontents)

#define entnum_int		int
#define brushnum_int	int
#define numchanged_int	int	// Use sparingly -- only if function is vague
#define facenum_int		int

RELATED_ (SCR_mazegen3_f VM_worldkey worldgrid_get_blocksize worldgrid_get_compressed_and_blocksize)
#define WORLDGRID_ZIP_BASE64		"worldgrid_zip_base64"
#define WORLDGRID_BLOCKSIZE			"worldgrid_blocksize" /// Like "512 512 320"
#define WORLDGRID_COMPRESSED_1		1 // SPECIAL

// map_carve_cubeoid_single ...
#define OVERLAP_IS_NONE(overlapmins) \
    (overlapmins[0] >= overlapmaxs[0] || overlapmins[1] >= overlapmaxs[1] || overlapmins[2] >= overlapmaxs[2])

void CSG_PathWork_Chdir_Linuxize_Slashses_No_Unicode_Support(int csg_argnum, char *mappath_fill, size_t mappath_fill_size); // CSG_Perform calls to Windows ==> Linux command line map name
baker_string_s *CSG_Process_BSAlloc (ccs *datasrc); // Returns a (baker_string_s *) or NULL if no data
void bounds_init_9 (vec3_t your_mins, vec3_t your_maxs);
int csg_vm_prefix_targetnames_list (map_t *plist, ccs *prefix, stringlist_t *plist_prefixes);


//int ParseModelBounds(const char *line, char *modelName, vec3_t mins, vec3_t maxs) {
//    char format[64];
//    int parsedCount;
//
//    /* Ensure variables are declared at the top for strict C89 compliance */
//    parsedCount = sscanf(line, "%63s mins %f %f %f maxs %f %f %f",
//                         modelName,
//                         &mins[0], &mins[1], &mins[2],
//                         &maxs[0], &maxs[1], &maxs[2]);
//
//    /* Returns the number of successfully assigned items (should be 7 if fully matched) */
//    return parsedCount == 7;
//}

// **********
// Map
// **********


#define map_add_with_title_keeplights(plist, map_title) \
    map_add    (hm, "worldspawn"); /*WORLD_0*/ \
    map_set    (hm, WORLD_0, "_keeplights","1"); \
    map_set    (hm, WORLD_0, "_blocksize","2048 2048 2048"); /* Visibility divisions default is 1024 */ \
    map_set    (hm, WORLD_0, "mapversion","220"); \
    map_set    (hm, WORLD_0, "message", map_title)

#define map_add_with_title_keeplights(plist, map_title) \
    map_add    (hm, "worldspawn"); /*WORLD_0*/ \
    map_set    (hm, WORLD_0, "_keeplights","1"); \
    map_set    (hm, WORLD_0, "_blocksize","2048 2048 2048"); /* Visibility divisions default is 1024 */ \
    map_set    (hm, WORLD_0, "mapversion","220"); \
    map_set    (hm, WORLD_0, "message", map_title)

#define map_get_classname(plist,entnum) map_get(plist,entnum,"classname")

#define map_get_vec3_dest_macro_DEFINE_(plist, entnum, key) \
    vec3_t key = {0,0,0}; map_get_vec3 (plist, entnum, STRINGIFY(key), key) // Ender

#define map_get_vec3_dest_macro_DEFINE_AS_(plist, entnum, key, keyas) \
    vec3_t keyas = {0,0,0}; map_get_vec3 (plist, entnum, STRINGIFY(key), keyas) // End

#define map_find(hm,key,value) \
    map_entity_find_start_after(hm, key, value, not_found_neg1) // Ender

// csgw_nodes_build_this_track ...
#define map_find_low_y(hm,classname) \
    map_entity_find_start_after_lowy(hm, classname, not_found_neg1) // Ender



#define map_load_is_ok(HM, FILENAME) \
	(map_load_or_errorcode(HM, FILENAME)!=SUCCESS_OK_0)



void map_texture_dominant_non_common (map_t *hm, char *sbuf, size_t sizeof_sbuf);

void map_freecontents(map_t *hm_destroy_free); // Map destroy
qbool map_reload (map_t *hm);
qbool map_load_from_string (map_t *hm, ccs *txt); // Runs metric - marks cubeoids
int map_load_or_errorcode (map_t *hm, ccs *filename);
qbool map_save_as (map_t *hm, ccs *filename);

baker_string_s *map_save_to_string_bsalloc (map_t *hm);
RELATED_ (VM_map_clipboard_set) // Manually does it.
void map_to_clipboard_debug_only (map_t *hm);
void map_clipboard_set_brush (map_t *hm, int entnum, int brushnum);

void map_move (map_t *hm, vec3_t move_amount); // Whole map
void map_metric_brush(map_t *hm, int entnum, int brushnum);
void map_brush_cubeoid_remetric (map_t *hm, int entnum, int brushnum);
void map_entity_translate_with_metric (map_t *hm, int entnum, vec3_t move_amount_add); // Just entity ..

int map_prefix_standard_targetnames (map_t *hm, ccs *prefix);
int map_prefix_standard_targetnamesf (map_t *hm, ccs *fmt, ...) DP_FUNC_PRINTF(2);
int map_prefix_dungeon_targetnamesf (map_t *hm, ccs *fmt, ...) DP_FUNC_PRINTF(2);
int map_prefix_dungeon_targetnames (map_t *hm, ccs *prefix);

// Baker: Prefix a fieldname like .. "targetname" "door1" ==> "myprefix_" "%s%s", prefix, string ==> "myprefix_door1"
numchanged_int map_fieldname_prefix (map_t *hm, ccs *fieldname, ccs *prefix);

void map_metric_all (map_t *hm); // Called by metric
void map_metric_cubeoid_mark_FULL_METRIC_AVAIL (map_t *hm);
//void map_metric_reset (map_t *hm); // Clears mins/maxes d_ent->has_brushes becames false.
void map_metric_again (map_t *hm); // map_metric_reset then map_metric_all

qbool map_reload (map_t *hm); // makes string, runs map_load_from_string repopulating list

void map_move_epairs_origin (map_t *hm, const vec3_t move_amount);
void map_move_all_brushes_patches (map_t *hm, const vec3_t move_amount);
void map_world_brushes_patches_copy (map_t *hm, map_t *paste);
//void _map_afterworld_copy (map_t *list, map_t *paste); // STATIC NOW
void map_paste_point_entities (map_t *hm, map_t *paste); // Only epairs -- no brushes.

// next is nearly a dup of entity_expand_bounds_did_brushes


numchanged_int map_texture_replace (map_t *hm, ccs *findtexturename, ccs *newtexturename);

RELATED_ (map_rotate_brush_fix_yaw)
void map_append_map_with_move (map_t *dst_map, map_t *src_map, vec3_t translate_move);
void map_append_map (map_t *dst_map, map_t *src_map);

// Baker: August/September 2026 - Seems to work, but untrusted ... for "brushrotate" console command
void map_rotate_brush_fix_yaw (map_t *hm, const vec3_t move, const vec3_t rotate); // sigmoid
//void map_brushes_translate_rotate_2_unused (map_t *hm, const vec3_t move0, const vec3_t rotate0);

qbool map_brush_get_rotation_is_ok (map_t *hm, int _entnum, int _brushnum, float *rotation_out);

void map_set (map_t *hm, int entnum, ccs *keyname, ccs *value);
void map_set_float (map_t *hm, int entnum, ccs *keyname, float fvalue);
void map_set_int32 (map_t *hm, int entnum, ccs *keyname, int ivalue);
void map_set_vec3_lossless (map_t *hm, int entnum, ccs *keyname, const vec3_t v); // LOSSLESS FORMAT "%.9g"
void map_set_vec3_g (map_t *hm, int entnum, ccs *keyname, const vec3_t v); // %g
void map_set_fmt (map_t *hm, int entnum, ccs *keyname, ccs *fmt, ...)    DP_FUNC_PRINTF(4);


// Baker: Deletes by renaming the key to "" which as of August 5 2026, those will no longer write
RELATED_ (map_save_to_string_bsalloc)
void map_key_delete (map_t *hm, int entnum, ccs *keyname); // Renames to "".
ccs *map_get (const map_t *hm, int entnum, ccs *key);
qbool map_get_keyval_does_match (const map_t *hm, int entnum, ccs *key, ccs *match_string); // nthness uses

#define map_has_key(HM,ENTNUM,KEY) (map_get(HM,ENTNUM,KEY)!=NULL) // Sep 28 2026 - No one use though!

float map_lowest_ground_not_caulk (map_t *hm, int *plowest_entnum, int *plowest_brushnum);

int map_get_int32 (const map_t *hm, int entnum, ccs *key);
float map_get_float (const map_t *hm, int entnum, ccs *key);
qbool map_get_vec3 (const map_t *hm, int entnum, ccs *key, vec3_t vout);

// map_find macro uses following ...
entnum_int map_entity_find_start_after (map_t *hm, ccs *key, ccs *value, int start_after);

// Baker: These find an entity with the key with a value of X
entnum_int map_find_key_string (map_t *plist, ccs *key, ccs *val);
entnum_int map_find_key_string_start_beyond (map_t *plist, ccs *key_to_find, ccs *val_to_find, int start_idx, int beyond_idx);
entnum_int map_find_key_string_ANY_start_beyond (map_t *plist, ccs *key_to_find, int start_idx, int beyond_idx, ccs **pVal);
entnum_int map_find_key_string_startswith_start_beyond (map_t *plist, ccs *key_to_find, ccs *val_prefix, int start_idx, int beyond_idx);

// #includes/ .... 
qbool map_find_did_find_brush_texture_starts_with (map_t *hm, ccs *texturename_prefix);

RELATED_ (map_add_with_title_keeplights)

entnum_int map_add (map_t *hm, ccs *classname); // Adds an entity
entnum_int map_add_entity_empty (map_t *hm); // ABOVE map_add is often better.

int map_carve_cubeoid_single (map_t *plist, int carve_entnum, int carve_brushnum, const vec3_t carve_mins, const vec3_t carve_maxs);
//void map_brush_and_face_metric (map_t *hm, int entnum, int brushnum);

// Return true if hit something
qbool map_traceline_drop_ok (const map_t *hm, vec3_t org, int ignore_entnum, vec3_t collide_out); // REALLY exists.

void map_copy_keys(map_t *hm_dst, int dstnum, const map_t *hm_src, int srcnum);
facenum_int map_add_face(map_t *hm, int entnum, int brushnum);

void map_add_brush_or_patch_copy(map_t *hm_dst, int dst_entnum, const map_t *hm_src, int src_entnum, int src_brushnum);
int map_add_cubeoid (map_t *hm, int entnum, vec3_t floor_mins, vec3_t floor_maxs, ccs *texturename, float texture_scale_xy, TEXSIDE_INT_e texturesideflags);


// **********
// Entity
// **********

#define entity_add_brush(HM, ENTNUM) _LIFE_brush_add (&(ITOE(HM, ENTNUM))->brushlist) // Used once in map_add_brush


entityx_s *entitylist_add (map_t *hm); // September 13 2026 - Used a lot.

void entity_add_alpha_brushes (entityx_s *d_ent, vec3_t worldmins, vec3_t worldmaxs, vec3_t worldsize); // mazegen

//int entitylist_add_decal_512_256_with_texturename (map_t *plist, ccs *texturename);
entityx_s *entitylist_add_pasting_this_ent (map_t *plist, const entityx_s *r_ent_paste); // mazgen -- June 8 2025
void entity_translate_with_metric (entityx_s *d_ent, vec3_t move_amount_add); // mazegen -- June 8 2025

qbool entity_expand_bounds_did_brushes (entityx_s *r_ent, vec3_t brmins, vec3_t brmaxs); // dungeon ..

void entity_move_epairs_origin (map_t *hm, int entnum, const vec3_t move_amount);

// DO NOT DELETE
//int entity_texturename_brushum (const entityx_s *r_ent, ccs *texturename);
//#define entity_has_texturename(RENT, TEXTURENAME) (entity_texturename_brushum(RENT, TEXTURENAME) != not_found_neg1)

ccs *entity_get (const entityx_s *r_ent, ccs *keyname);
int entity_get_int32 (const entityx_s *r_ent, ccs *keyname);
float entity_get_float (const entityx_s *r_ent, ccs *keyname);
#define entity_has_key(RENT,KEY) (entity_get(RENT, KEY) != NULL)

qbool entity_is_func_group_prefixed (const entityx_s *r_ent);

qbool entity_get_vec3 (const entityx_s *r_rent, ccs *keyname, vec3_t vout);
// LEAVE .. void entity_key_rename (entityx_s *d_ent, ccs *key_oldname, ccs *key_newname);

void entity_set (entityx_s *d_ent, ccs *keyname, ccs *value_to_set); // 10+ instances left
void entity_set_fmt (entityx_s *d_ent, ccs *keyname, ccs *fmt, ...) DP_FUNC_PRINTF(3);

void entity_set_int32 (entityx_s *d_ent, ccs *keyname, int ivalue);
void entity_set_float (entityx_s *d_ent, ccs *keyname, float fvalue);
void entity_set_vec3_g (entityx_s *d_ent, ccs *keyname, const vec3_t v);
void entity_set_vec3_lossess (entityx_s *d_ent, ccs *keyname, const vec3_t v);



// **********
// Brush, Face & Patch Operations
// **********

void brush_terrain_wedge_split (map_t *hm, int entnum, int brushnum);
int brush_facenum_first_with_texture (brush_s *r_brush, ccs *texturename);
qbool brush_has_texturename (brush_s *r_brush, ccs *texturename);
int brush_add_cubeoid (map_t *hm, entityx_s *d_ent, vec3_t brmins, vec3_t brmaxs, ccs *texturename, float texture_scale_x, float texture_scale_y, TEXSIDE_INT_e wall_side_flags);
qbool brush_add_alpha_brush (map_t *hm, int entnum, const vec3_t point3d, float alpha);
qbool brush_add_alpha_brush_string (map_t *hm, int entnum, ccs *point3d_string, float alpha);

int brush_top_face_index_maybe (const brush_s *brush);

void brush_add_copying_brush_or_patch(entityx_s *d_ent, const brush_s *r_brush);
brushrow_s *_LIFE_brushface_alloc (brushrowlist_t *list);
brushrow_s *brush_add_face (brush_s *d_brush);
//void brush_and_face_metric (brush_s *d_brush); // map_brush_metric

qbool brush_has_duplicate_planes (map_t *hm, int entnum, int brushnum);
qbool brush_has_duplicate_planes_delete_them (map_t *hm, int entnum, int brushnum);

void brushlistfreecontents(brushlist_t *list);
brush_s *_LIFE_brush_add (brushlist_t *list);

qbool brush_add_faces_as_cubeoid_texturing_like (map_t *hm, entityx_s *d_ent, brush_s *d_brush,
	const int r_brushnum, vec3_t brmins, vec3_t brmaxs, FACE_INT_e unused_cubeoid_nsewcf);

//void brush_fill_face_points(brush_s *brush);
qbool brush_fill_face_points_needs_rebuilt (brush_s *brush);
void brush_rebuild_fill_face_points (map_t *hm, int entnum, int brushnum);
void brush_free(brush_s *destroy_free_brush);

ccs *brush_texture_first_non_common (brush_s *r_brush, int *p_facenumreply);

// **********
// PATCH
// **********

void patchrowlistfreecontents (patchrowlist_t *list);
patchrow_s *patchrowlist_add (patchrowlist_t *list);

// **********
// FACE
// **********
qbool face_is_flat (brushrow_s *r_face);
qbool face_is_up (brushrow_s *face);
qbool face_is_up_or_down (brushrow_s *face);

void face_copy_data(brushrow_s *d_face, const brushrow_s *r_face_src);
void face_copy_texturedata (brushrow_s *dest, const brushrow_s *src);
void face_texture_set (brushrow_s *d_face, ccs *s);
void face_texture_set_fmt (brushrow_s *d_face, ccs *fmt, ...) DP_FUNC_PRINTF(2);
void face_texture_append (brushrow_s *d_face, ccs *extension);
void face_texture_set_cat (brushrow_s *d_face, ccs *basename, ccs *extension);

qbool faces_ptr_bbox_touch (const brushrow_s *face1, const brushrow_s *face2);
qbool faces_bbox_touch (map_t *hm, int entnum1, int brushnum1, int facenum1, int entnum2, int brushnum2, int facenum2);

ccs *face_direction(const brush_s *brush, int facenum, const brushrow_s *face);

// **********
// CSG & Perform Pipeline
// **********

qbool map_texture_find_size (map_t *hm, ccs *s, int *w_out, int *h_out);
void map_fill_textures (map_t *hm);

int csgm_carve (map_t *plist, /*"#includes/"*/ ccs *MYKEY, ccs *mappath_realpath_full);
int csgm_carve2 (map_t *plist, /*"#includes/"*/ ccs *MYKEY);

int csgc_add_point_entities_on_clipboard (map_t *hm);
int csgc_brush_delete_if_nondom (map_t *hm);
int csgc_brush_delete_if_texture (map_t *hm, ccs *texturename);
int csgc_brushfacer (map_t *hm);
int csgc_brushfacer_specific_texture (map_t *plist, ccs *texturename);
int csgc_brushunfacer (map_t *hm);
int csgc_brushunfacer_specific_classname (map_t *hm, ccs *classname);
int csgc_caulk_non_common_non_sky_containing_textures (map_t *plist);
int csgc_classname_delete (map_t *hm, ccs *classname);
int csgc_csg_run (map_t *hm, ccs *map_filename);
int csgc_delete_world_brushes_with_nondominant_texture (map_t *plist);
int csgc_detail_auto (map_t *hm);
int csgc_detail_wedge_split_auto (map_t *hm);
int csgc_flat_ground_retexture (map_t *hm, ccs *z_val_ground_level_string);
int csgc_flat_trim (map_t *hm);
int csgc_floor_every_brush_to_specified_z (map_t *hm, ccs *z_val_floorstring);
int csgc_ground_carve (map_t *hm);
int csgc_includes_sheet (void);
int csgc_info_player_start_drop_center (map_t *hm);
int csgc_infodump (map_t *hm);
int csgc_print_lowest_ground (map_t *hm);
int csgc_printfaces (map_t *hm, ccs *_entnum, ccs *_brushnum);
int csgc_printtextures (map_t *hm);
int csgc_printunit (map_t *hm, ccs *_entnum, ccs *_brushnum);
int csgc_process_terrain (map_t *hm);
int csgc_q13 (map_t *hm);
int csgc_rewrite (map_t *hm);
int csgc_ridge_valley (map_t *hm);
int csgc_rotateunit (map_t *hm, ccs *_entnum, ccs *_brushnum, ccs *_yawdegrees);
int csgc_sky_encase (map_t *hm, ccs *skyheightstring, ccs *skytexturename);
int csgc_replacetextureadjacent (map_t *hm, ccs *texturenamesloped, ccs *texturenamegrass, ccs *texturenamesloped2);
int csgc_weld (map_t *hm);
int csgc_worldspawn_key_delete (map_t *hm, ccs *key);
int csgc_worldspawn_key_set (map_t *hm, ccs *key, ccs *value);
int csgc_worldspawn_keys_print (map_t *hm);

int csge_angle_jitter (map_t *plist, ccs *MYKEY);
int csge_atomize (map_t *hm, ccs *MYKEY);
int csgw_brush_delete_if_texture (map_t *hm, ccs *MYKEY);
int csge_clone_classname (map_t *hm, ccs *MYKEY); // DEEP LAIR TORCHES
int csgw_csg_test (map_t *hm, ccs *MYKEY);
int csge_decal_automatic_droptofloor (map_t *hm, ccs *MYKEY);
int csge_decal_drop_distance (map_t *plist, ccs *MYKEY);
int csgw_detail_auto (map_t *hm, ccs *MYKEY);
int csgw_floorage (map_t *hm, ccs *MYKEY);
int csgax_frost (map_t *plist, ccs *MYKEY, ccs *mappath_realpath_full);
int csge_heightmap (map_t *plist, ccs *MYKEY);
int csgax_includes (map_t *plist, ccs *MYKEY, ccs *mappath_realpath_full); // /*"#includes/"*/
int csge_keep_every_nth (map_t *plist, ccs *MYKEY);
int csge_keep_percent (map_t *plist, ccs *MYKEY);
int csge_light_range (map_t *plist, ccs *MYKEY);
int csge_modelscale_range (map_t *plist, ccs *MYKEY);
int csgw_nodes_from_texture (map_t *plist, ccs *MYKEY);
int csge_origin_jitter (map_t *plist, ccs *MYKEY);
int csge_originmake (map_t *plist, ccs *MYKEY);
int csgw_rename_GARUX (map_t *hm, ccs *MYKEY);
int csgw_sky_decal (map_t *plist, ccs *MYKEY);
int csgw_sky_encase (map_t *plist, ccs *MYKEY);

///**/ int csgw_sky_decal_wip (map_t *plist, ccs *MYKEY);

// Baker: all shapes carve was an epic fail
//int map_carve_super (map_t *hm, const brush_s *carve_brush_in);

extern cvar_t _csg_detail;
extern cvar_t _csg_snaptointeger;
extern cvar_t _csg_snaptointeger_negative_zero_is_zero;


qbool Boxes_Touch_3D (
    const vec3_t brmins,
    const vec3_t brmaxs,
    const vec3_t brmins2,
    const vec3_t brmaxs2);


// csg_local.h



RELATED_ (CSG_GLOBAL CSG_LOCAL_DEFS)
#define CSG_LOCAL

#pragma once
#include "csg_local_defs.h"

#define ALPHA_BRUSH_SIZE_HALF_48	48

#define COMMON_UP					"common/up"
#define COMMON_UP1					"common/up1"	// WELD
#define COMMON_ALPHA				"common/alpha"

// if (FACE_IS_COMMON (d_face)) continue;
#define FACE_IS_COMMON(FACE) String_Starts_With ((FACE)->pbrtexture, COMMON_SLASH)
#define FACE_IS_CAULK(FACE) String_Match ((FACE)->pbrtexture, COMMON_CAULK)

#define COMMON_CAULK				"common/caulk"
#define COMMON_CARVE				"common/carve"
#define COMMON_ORIGIN				"common/origin"
#define COMMON_TRIGGER				"common/trigger"
#define COMMON_SLASH				"common/"
#define COMMON_SKY_LIAR				"sky"
#define	FUNC_GROUP_TERRAIN			"func_group_terrain"
#define _NO_DETAIL_AUTO				"_no_detail_auto" // func_group key to avoid detail auto

#define SKY_HEIGHT_8192				8192
#define SKY_HEIGHT_STRING_8192		"8192"


#define MAPS_SH_ROCKY				"maps_sh/rocky"
#define MAPS_SH_FANG_GRASS			"maps_sh/fang_grass"

// UNDERSCORE prefixed keys are deleted for entities EXCEPT for "light" with "keeplights 1"

#define	KEYNAME__TARGETNAME			"_targetname"	// nodes
#define KEYNAME__NEXTNODE			"_nextnode"
#define KEYNAME__PREVNODE			"_prevnode"

#define SPAWNFLAGS_PATH_CORNER_FLY_POLE_1		1		// GALAXY Real tall
#define SPAWNFLAGS_PATH_CORNER_TRAIN_TURN_2		2		// GALAXY Turn to dest

#define TRACK_ID					"track_id"


#define TESSEL_BIG_10000			100000	// For no tesselation - csgw_floorage
#define TESS_SIZE_HERE_16384		16384	// sky_encase


#define BBOX_6						6
#define RECTUS_ACTIVE_1				1
#define RECTUS_DELETED_0			0


#define CSGM_START_							int _num_done = 0;
#define CSGM_END_							/*zpr ("%s processed", MYKEY);*/ return _num_done;
#define CSGM_END_FUNC_						/*zpr ("%s processed", __func__); */return _num_done;
#define CSGM_END_RETURN_ONLY_				return _num_done;
#define CSGM_ENT_STAMP_DONE_				map_key_delete (hm, entnum, MYKEY); _num_done ++; // STAMP
#define CSGM_ENT_STAMP_NO_DONE_				map_key_delete (hm, entnum, MYKEY); // STAMP
#define CSGM_KEY_DELETE_WORLD_(x)			map_key_delete (hm, WORLD_0, MYKEY);
#define CSGM_KEY_DELETE_ENT_(ENTNUM,x)		map_key_delete (hm, ENTNUM, MYKEY);

//CSGM_KEY_DELETE_ENT_ (entnum, csge_);

// CSGM_KEY_DEFINE_WORLD_OR_ABORT_0 (_rename, MYKEY);
#define CSGM_KEY_DEFINE_WORLD_OR_ABORT_0(_VAR, KEYNAME) \
	ccs *_VAR = map_get(hm, WORLD_0, MYKEY); \
	if (_VAR == NULL) return 0; \
	_num_done ++; // Ender ... FORCE PRE-PROCESSOR REWRITE


#define CSGM_NUM_DONE_PLUS_			_num_done ++;
#define CSGM_NUM_DONE_PLUS_EQ(X)	_num_done += (X);

#define TARGETNAMES_EXTRA_NULL		NULL

typedef struct {
	ccs				*text_start;
	ccs				*datasrc;
	int				depth;

	map_t			*plist;
	entityx_s		*p_ent;
	brush_s			*p_brush;
	brushrow_s		*p_brushrow;
	patchrow_s		*p_patchrow;
} map_reader_s;



//
// FOR_EACH
//




// plist->original_ent_count
RELATED_ (for_each_entity_original_count_undeleted)
// for_each_entity_undeleted (entnum, plist, d_ent)
// for_each_end // for_each_entity
#define for_each_entity_undeleted(NN,HM,DENT)\
	for (int NN = 0; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender

// for_each_entity_with_brushes (entnum, plist, d_ent)
// for_each_end // entities with brushes

#define for_each_entity_with_brushes(NN,HM,DENT)\
	for (int NN = 0; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; \
		if (DENT->has_brushes == false) continue; // ender

// for_each_world_func_group (entnum, plist, d_ent)
// for_each_end // for_each_world_func_group

#define for_each_world_func_group(NN,HM,DENT)\
	for (int NN = 0; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; \
		if (DENT->is_world_or_func_group) continue; // Ender

// for_each_brush_cubeoid_undeleted (brushnum, d_ent, d_brush)
// for_each_end // for_each_brush_cubeoid
#define for_each_brush_cubeoid_undeleted(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue; /*PATCH, SKIP*/ \
		if (D_BRUSH->brushrowlist.count == 0) continue; /*NO ROWS*/ \
		if (D_BRUSH->is_cubeoid == false) continue; // Ender


// for_each_entity_even_deleted (entnum, plist, d_ent)
// for_each_end // for_each_entity_even_deleted
#define for_each_entity_even_deleted(NN,HM,DENT)\
	for (int NN = 0; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; // Ender


// for_each_entity_only_world (worldentnum, plist, d_ent)
// for_each_end // only world
#define for_each_entity_only_world(NN,HM,DENT) \
	for (int NN = ENTITY_WORLD_0; NN < AFTER_WORLD_1; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; // Ender

// for_each_entity_after_world (entnum, plist, d_ent)
// for_each_end // after_world
#define for_each_entity_after_world(NN,HM,DENT) \
	for (int NN = AFTER_WORLD_1; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; // Ender

// for_each_key_val_pair (keyidx, d_ent, key, val)
// for_each_end // key val pairs
#define for_each_key_val_pair(KEYIDX, DENT, KEY, VAL) \
		for (int KEYIDX = 0; KEYIDX < DENT->pairslist.numstrings; KEYIDX += 2) { \
			ccs *KEY = DENT->pairslist.strings[KEYIDX + 0]; \
			ccs *VAL = DENT->pairslist.strings[KEYIDX + 1]; // Ender

// for_each_entity_after_world_undeleted (entnum, plist, d_ent)
// for_each_end // each entity after world
#define for_each_entity_after_world_undeleted(NN,HM,DENT)\
	for (int NN = AFTER_WORLD_1; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender

// for_each_entity_after_world_undeleted_original_ent_count (entnum, plist, d_ent) // We add entities
// for_each_end // each entity after world original count
#define for_each_entity_after_world_undeleted_original_ent_count(NN,HM,DENT)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender


// EXAMPLE:
#if 0
	for_each_entity_undeleted(entnum, plist, d_ent) // EXAMPLE
		for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
			for_each_brush_face (facenum, d_brush, d_face)
				// DO EVERY VERTEX.EVEN CAULK
				stringlistappend_face_vertex_unique (&list_vertex_strings, r_face);
			for_each_end // next face
		for_each_end // next brush
	for_each_end // next entity
#endif // 0
//

// for_each_entity_original_count_undeleted (entnum, plist, d_ent)
// for_each_end // for_each_entity_original_count_undeleted
#define for_each_entity_original_count_undeleted(NN,HM,DENT)\
	for (int NN = 0; NN < HM->original_ent_count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender

// for_each_entity_after_world_original_count_undeleted (entnum, plist, e)
// for_each_end // original entities even undeleted in .map
#define for_each_entity_after_world_original_count_undeleted(NN,HM,DENT)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender

#define for_each_entity_undeleted_start_after(NN,HM,DENT,STARTNUM)\
	for (int NN = STARTNUM + 1; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender


#define for_each_entity_start_beyond_undeleted_startnum(NN,HM,DENT,STARTNUM,BEYOND)\
	for (int NN = STARTNUM; NN < BEYOND; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender

#define for_each_entity_undeleted_startnum_with_classname(NN,HM,DENT,STARTNUM,CLASSNAME_WANTED)\
	for (int NN = STARTNUM; NN < HM->count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; \
		ccs *classname = map_get (plist, entnum, "classname"); \
		if (false == String_Match_Nullproof (classname, CLASSNAME_WANTED)) \
			continue; // Ender


// for_each_entity_undeleted_startnum_with_classname(entnum, plist, dent, plist->original_ent_count, "node")
// for_each_end // entity with classname
#define for_each_entity_original_count_undeleted_startnum(NN,HM,DENT,STARTNUM)\
	for (int NN = STARTNUM; NN < HM->original_ent_count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; \
		if (DENT->is_considered_deleted) continue; // Ender


// nthness won't work if we don't count the deleted ones.
// for_each_entity_original_even_deleted_ones_startnum (entnum, plist, e, AFTER_WORLD_1)
#define for_each_entity_original_even_deleted_ones_startnum(NN,HM,DENT,STARTNUM)\
	for (int NN = STARTNUM; NN < HM->original_ent_count; NN ++) { \
		entityx_s	*DENT = &HM->entity[NN]; // Ender


// for_each_brush_not_a_patch (brushnum, d_ent, d_brush)
// for_each_end // for_each_brush_not_a_patch
#define for_each_brush_not_a_patch(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_a_patch) \
			continue; /*PATCH, SKIP*/ \
		if (D_BRUSH->brushrowlist.count == 0) \
			continue; /*NO ROWS*/ // Ender



// for_each_const_brush_not_a_patch (brushnum, r_ent, r_brush)
// for_each_end // for_each_brush_or_patch

#define for_each_const_brush_not_a_patch(YOUR_BRUSHNUM_N, D_ENT, R_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		const brush_s *R_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (R_BRUSH->is_a_patch) \
			continue; /*PATCH, SKIP*/ \
		if (R_BRUSH->brushrowlist.count == 0) \
			continue; /*NO ROWS*/ // Ender

// for_each_brush_or_patch (brushnum, d_ent, d_brush)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_or_patch(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; // Ender

// for_each_brush_original_count --- only used by carve at this time
#define for_each_brush_original_count(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->original_brush_count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; // Ender


// for_each_brush_undeleted (brushnum, d_ent, d_brush)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_undeleted(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_considered_deleted) \
			continue; // Ender

// for_each_const_brush (brushnum, r_ent, r_brush)
// for_each_end // for_each_brush_or_patch

#define for_each_const_brush(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		const brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; // Ender

// for_each_brush_not_a_patch_undeleted (brushnum, d_ent, d_brush)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_not_a_patch_undeleted(YOUR_BRUSHNUM_N, D_ENT, D_BRUSH) \
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_considered_deleted) \
			continue; /*DELETED*/ \
		if (D_BRUSH->is_a_patch) \
			continue; /*PATCH, SKIP*/ \
		if (D_BRUSH->brushrowlist.count == 0) \
			continue; /*NO ROWS*/ // Ender

//for_each_brush_face (facenum, d_brush, d_face)
//	bounds_expand (d_face->a, brmins, brmaxs);
//	bounds_expand (d_face->b, brmins, brmaxs);
//	bounds_expand (d_face->c, brmins, brmaxs);
//for_each_end // next brush


// for_each_brush_face (facenum, d_brush, d_face)
// for_each_end // for_each_brush_face
#define for_each_brush_face(YOUR_FACENUM_N, D_BRUSH, D_FACE)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < D_BRUSH->brushrowlist.count; YOUR_FACENUM_N ++) { \
		brushrow_s *D_FACE = &D_BRUSH->brushrowlist.brushrow[YOUR_FACENUM_N]; \
		if (D_FACE->is_considered_deleted) continue; // Ender

// for_each_const_brush_face (facenum, r_brush, r_face)
// for_each_end // for_each_brush_face
#define for_each_const_brush_face(YOUR_FACENUM_N, D_BRUSH, D_FACE)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < D_BRUSH->brushrowlist.count; YOUR_FACENUM_N ++) { \
		const brushrow_s *D_FACE = &D_BRUSH->brushrowlist.brushrow[YOUR_FACENUM_N]; \
		if (D_FACE->is_considered_deleted) continue; // Ender


// for_each_patch_row (rownum, d_brush, d_patchrow)
// for_each_end // patch_row
#define for_each_patch_row(YOUR_FACENUM_N, D_BRUSH, D_PATCHROW)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < D_BRUSH->patchrowlist.count; YOUR_FACENUM_N ++) { \
		patchrow_s *D_PATCHROW = &D_BRUSH->patchrowlist.patchrow[YOUR_FACENUM_N]; // Ender

// for_each_const_patch_row (rownum, r_brush, r_patchrow)
// for_each_end // patch_row
#define for_each_const_patch_row(YOUR_FACENUM_N, D_BRUSH, D_PATCHROW)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < D_BRUSH->patchrowlist.count; YOUR_FACENUM_N ++) { \
		const patchrow_s *D_PATCHROW = &D_BRUSH->patchrowlist.patchrow[YOUR_FACENUM_N]; // Ender

// for_each_patch_column (columnnum, d_brush, d_patchrow)
// for_each_end // patch_column
#define for_each_patch_column(YOUR_COL_N, D_BRUSH, D_FACE) \
	for (int YOUR_COL_N = 0; YOUR_COL_N < D_BRUSH->patch_cols; YOUR_COL_N ++) {







#define FACE_MIN_POINTS_3 3
#define BRUSH_MIN_FACES_4 4


// Baker: adjacents and weld uses this
#define BRUSH_IS_5_SIDED_TRI(pbrush)	(pbrush->brushrowlist.count == (3 + 2))
#define BRUSH_IS_5_or_6_SIDED(pbrush)	(isin2(pbrush->brushrowlist.count, 5, 6))	// TRI or RECT -- if flat bottom with perpendicular bottom verts as sides
#define BRUSH_IS_6_SIDED(pbrush)		(isin1(pbrush->brushrowlist.count, 6))


// March 24 2026
/* Entity and Brush Getter Helpers */
#define ITOE(HM, ENTNUM)											&((HM)->entity[ENTNUM])

// REVERSE: int entnum = ARRAY_INDEX (d_ent, plist->entity);
//#define ENT_GET(HM, ENTNUM)										&((HM)->entity[ENTNUM])
#define ENT_GET(HM, ENTNUM)											(&((HM)->entity[ENTNUM]))
#define ENT_GET2(HM, ENTNUM)										(&((HM)->entity[ENTNUM]))

#define ENT_GET_NUM(HM, PENT)										ARRAY_INDEX(PENT, (HM)->entity);


#define ENT_COUNT(HM)												((HM)->count)
#define BRUSH_GET(HM, ENTNUM, BRUSHNUM)								(&((HM)->entity[ENTNUM].brushlist.brush[BRUSHNUM]))

#define BRUSH_COUNT(HM, ENTNUM)										((HM)->entity[ENTNUM].brushlist.count)
#define BRUSH_ORIGINAL_COUNT(HM, ENTNUM)							((HM)->entity[ENTNUM].original_brush_count)

#define BRUSH_GET_NUM(HM, ENTNUM, DBRUSH)							ARRAY_INDEX(DBRUSH, (HM)->entity[ENTNUM].brushlist.brush);

#define FACE_COUNT_PTR(BR)											(BR)->brushrowlist.count

#define FACE_COUNT(HM, ENTNUM, BRUSHNUM)							((HM)->entity[ENTNUM].brushlist.brush[BRUSHNUM].brushrowlist.count)
#define FACE_GET(HM, ENTNUM, BRUSHNUM, FACENUM)						&((HM)->entity[ENTNUM].brushlist.brush[BRUSHNUM].brushrowlist.brushrow[FACENUM])
#define FACE_GET_PTR(BRUSH, FACENUM)								&BRUSH->brushrowlist.brushrow[FACENUM]

#define PATCHROW_GET(HM, ENTNUM, BRUSHNUM, PATCHROW)				&((HM)->entity[ENTNUM].brushlist.brush[BRUSHNUM].patchrowlist.patchrow[PATCHROW])

/* Scope-Safe Variable Definition Helpers (C89 Compliant) */
// BRUSH_DEFINE (d_brush, hm, entnum, new_brushnum);
#define BRUSH_DEFINE(P_BRUSH, HM, ENTNUM, BRUSHNUM)					brush_s      *P_BRUSH = BRUSH_GET(HM, ENTNUM, BRUSHNUM)
#define BRUSH_DEFINE_CONST(P_BRUSH, HM, ENTNUM, BRUSHNUM)			const brush_s  *P_BRUSH = BRUSH_GET(HM, ENTNUM, BRUSHNUM)

#define FACE_DEFINE(P_FACE, HM, ENTNUM, BRUSHNUM, FACENUM)			brushrow_s     *P_FACE = FACE_GET(HM, ENTNUM, BRUSHNUM, FACENUM)
#define FACE_DEFINE_CONST(P_FACE, HM, ENTNUM, BRUSHNUM, FACENUM)	const brushrow_s *P_FACE = FACE_GET(HM, ENTNUM, BRUSHNUM, FACENUM)

#define FACE_DEFINE_PTR(P_FACE, BRUSH, FACENUM)						brushrow_s     *P_FACE = FACE_GET_PTR(BRUSH, FACENUM)

#define ENT_DEFINE(P_ENT, HM, ENTNUM)								entityx_s      *P_ENT = ENT_GET(HM, ENTNUM)
#define ENT_DEFINE_CONST(P_ENT, HM, ENTNUM)							const entityx_s *P_ENT = ENT_GET(HM, ENTNUM)




//
// 2
//




#include "csg_local_entity.h"

#include "csg_local_brush_patch.h"
#include "csg_local_brush_patch_face.h"



// for_each_entity_brush (brush_idx1, d_brush1, d_ent)
#define for_each_entity_brush(YOUR_BRUSHNUM_N, D_BRUSH, D_ENT)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < D_ENT->brushlist.count; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue; // Ender


#define for_each_entity_brush_start_beyond(YOUR_BRUSHNUM_N, D_BRUSH, D_ENT, START, BEYOND)\
	for (int YOUR_BRUSHNUM_N = START; YOUR_BRUSHNUM_N < BEYOND; YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = &D_ENT->brushlist.brush[YOUR_BRUSHNUM_N]; \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue; // Ender


RELATED_ (_cat_brushnum_patch)
// Float_SnapToInteger_If_003 ...
// _cat_brushnum_patch is only wild user
// all others use vector3 snap
float Float_SnapToInteger_If_003(float value);
void Vector_SnapToInteger_If_003 (vec3_t snapped);
void Vector_SnapToInteger_NoNegZero_If_003 (vec3_t snapped);

float plane_z_slope_degrees(vec3_t normal);

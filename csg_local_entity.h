// csg_local_entity.h

#define CSG_LOCAL_ENTITY

// for_each_entity_with_key_2 (entnum, d_ent, hm, _heightmap, MYKEY)
// for_each_end // for each entity

#define for_each_entity_with_key_2(NN, P_ENT, HM, KEYVALUEVAR, KEYNAME)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *KEYVALUEVAR = entity_get (P_ENT, KEYNAME); \
		if (KEYVALUEVAR == NULL) continue; // Ender

// for_each_entity_with_key_afterworld_2 (entnum, d_ent, hm, _heightmap, MYKEY)
// for_each_end // for each entity

#define for_each_entity_with_key_afterworld_2(NN, P_ENT, HM, KEYVALUEVAR, KEYNAME)\
	for (int NN = AFTER_WORLD_1; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *KEYVALUEVAR = entity_get (P_ENT, KEYNAME); \
		if (KEYVALUEVAR == NULL) continue; // Ender

#define for_each_entity_classname_startnum_2(NN, P_ENT, HM, STARTNUM, CLASSNAME_WANTED)\
	for (int NN = STARTNUM; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *this_classname = map_get (HM, NN, "classname"); \
		if (false == String_Match_Nullproof (this_classname, CLASSNAME_WANTED)) \
			continue; // Ender


#define for_each_entity_with_key_afterworld_original_2(NN, P_ENT, HM, KEYVALUEVAR, KEYNAME)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *KEYVALUEVAR = entity_get (P_ENT, KEYNAME); \
		if (KEYVALUEVAR == NULL) continue; // Ender

#define for_each_entity_with_key_original_2(NN, P_ENT, HM, KEYVALUEVAR, KEYNAME)\
	for (int NN = WORLD_0; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *KEYVALUEVAR = entity_get (P_ENT, KEYNAME); \
		if (KEYVALUEVAR == NULL) continue; // Ender


#define for_each_entity_classname_original_2(NN, P_ENT, HM, CLASSNAME_WANTED)\
	for (int NN = WORLD_0; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *classname = map_get (HM, NN, "classname"); \
		if (false == String_Match_Nullproof (classname, CLASSNAME_WANTED)) \
			continue; // Ender

// for_each_entity_original_2 (entnum, r_ent, hm)
// for_each_end // after_world
#define for_each_entity_original_2(NN,P_ENT,HM)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

// for_each_entity_afterworld_original_2 (entnum, r_ent, hm)
// for_each_end // after_world
#define for_each_entity_afterworld_original_2(NN,P_ENT,HM)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

// for_each_entity_afterworld_2 (entnum, d_ent, hm)
// for_each_end // after_world
#define for_each_entity_afterworld_2(NN,P_ENT,HM)\
	for (int NN = AFTER_WORLD_1; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

// for_each_entity_afterworld_2 (entnum, d_ent, hm)
// for_each_end // after_world
#define for_each_entity_const_afterworld_2(NN,P_ENT,HM)\
	for (int NN = AFTER_WORLD_1; NN < HM->count; NN ++) { \
		ENT_DEFINE_CONST (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

#define for_each_entity_classname_2(NN, P_ENT, HM, CLASSNAME_WANTED)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *this_classname = map_get (HM, NN, "classname"); \
		if (false == String_Match_Nullproof (this_classname, CLASSNAME_WANTED)) \
			continue; // Ender

#define for_each_entity_classname_startswith_2(NN, P_ENT, HM, CLASSNAME_STARTSWITH)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *this_classname = map_get (HM, NN, "classname"); \
		if (false == String_Starts_With_Nullproof (this_classname, CLASSNAME_STARTSWITH)) \
			continue; // Ender

#define for_each_entity_classname_startswith_afterworld_original_2(NN, P_ENT, HM, CLASSNAME_STARTSWITH)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *this_classname = map_get (HM, NN, "classname"); \
		if (false == String_Starts_With_Nullproof (this_classname, CLASSNAME_STARTSWITH)) \
			continue; // Ender


// for_each_entity_2 (entnum, ent, hm)
// for_each_entity_2 (entnum, d_ent, hm)
// for_each_end // entity
RELATED_ (for_each_entity_with_key_original_2)
#define for_each_entity_2(NN,P_ENT,HM)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

//for_each_entity_brush_2 (hm, entnum, ent, brushnum, brush)
//	zpr ("ent %d brush %d", entnum, brushnum);
//for_each_end_2 // for_each_entity_brush
#define for_each_entity_brush_2(MAP, ENTNUM, D_ENT, BRUSHNUM, D_BRUSH) \
    for_each_entity_2 (ENTNUM, D_ENT, MAP) \
        for_each_brush_2 (BRUSHNUM, D_BRUSH, MAP, ENTNUM)


#define for_each_entity_brush_face_3(MAP, ENTNUM, D_ENT, BRUSHNUM, D_BRUSH, FACENUM, D_FACE) \
    for_each_entity_2 (ENTNUM, D_ENT, MAP) \
        for_each_brush_2 (BRUSHNUM, D_BRUSH, MAP, ENTNUM) \
            for_each_brush_face_2 (FACENUM, D_FACE, MAP, ENTNUM, BRUSHNUM)

// for_each_entity_2 (entnum, d_ent, hm)
// for_each_end // entity
RELATED_ (for_each_entity_with_key_original_2)
#define for_each_entity_const_2(NN,P_ENT,HM)\
	for (int NN = WORLD_0; NN < HM->count; NN ++) { \
		ENT_DEFINE_CONST (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue;

// for_each_entity_start_beyond_2(entnum, plist, r_ent, start_idx, beyond_idx)
#define for_each_entity_start_beyond_2(NN,P_ENT,HM,STARTNUM,BEYOND)\
	for (int NN = STARTNUM; NN < BEYOND; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; // Ender

// for_each_entity_classname_afterworld_original_2 (decal_entnum, d_ent_decal, hm, "_decal")
// for_each_end // after_world

RELATED_ (for_each_entity_with_key_original_2 for_each_entity_classname_2)
#define for_each_entity_classname_afterworld_original_2(NN,P_ENT,HM,CLASSNAME_WANTED)\
	for (int NN = AFTER_WORLD_1; NN < HM->original_ent_count; NN ++) { \
		ENT_DEFINE (P_ENT, HM, NN); \
		if (P_ENT->is_considered_deleted) continue; \
		ccs *classname = map_get (HM, NN, "classname"); \
		if (false == String_Match_Nullproof (classname, CLASSNAME_WANTED)) \
			continue; // Ender


// csg_p_keys.c.h

RELATED_ (VM_map_find_entity)

// Dec 22 2024
static int local_entity_key_idx (const entityx_s *r_ent, ccs *keyname)
{
	for_each_key_val_pair (keyidx, r_ent, key, val)
		if (String_Match (key, keyname))
			return keyidx;
	for_each_end // key val pairs
	return not_found_neg1;
}

// Dec 22 2024
void entity_keyidx_value_rename (entityx_s *d_ent, int keyidx, ccs *newvalue) 
{ 
	if (in_range_beyond(0, keyidx + 1, d_ent->pairslist.numstrings) == false) {
		zpr ("Error keyidx out of bounds");
	}
	Z_StrDup_Realloc (&d_ent->pairslist.strings[keyidx + 1], newvalue); // VALUE
}

void entity_keyidx_value_rename_fmt (entityx_s *d_ent, int keyidx, ccs *fmt, ...) // DP_FUNC_PRINTF(3);
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	entity_keyidx_value_rename (d_ent, keyidx, text);
	VA_EXPAND_ALLOC_FREE (text);
}

// Dec 22 2024
void entity_keyidx_keyname_rename (entityx_s *d_ent, int keyidx, ccs *newkeyname) 
{ 
	if (in_range_beyond(0, keyidx + 0, d_ent->pairslist.numstrings) == false) {
		zpr ("Error keyidx out of bounds");
	}
	Z_StrDup_Realloc (&d_ent->pairslist.strings[keyidx + 0], newkeyname); // VALUE
}

void entity_keyidx_keyname_rename_fmt (entityx_s *d_ent, int keyidx, ccs *fmt, ...) // DP_FUNC_PRINTF(3);
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	entity_keyidx_keyname_rename (d_ent, keyidx, text);
	VA_EXPAND_ALLOC_FREE (text);
}

// Baker: Discouraged -- use map_get if convenient, although map_get calls this internally
CALLERS_ (map_get)
ccs *entity_get (const entityx_s *r_ent, ccs *keyname)
{
	for_each_key_val_pair (keyidx, r_ent, key, val)
		if (String_Match (key, keyname))
			return val;
	for_each_end // key val pairs
	return NULL;
}

CALLERS_ (map_get_int32)

// FAILURE MEANS RETURN 0
int entity_get_int32 (const entityx_s *r_ent, ccs *keyname)
{
	ccs *s = entity_get(r_ent, keyname); // SEPT12026PASS

	if (!s)
		return 0;

	int d = atoi(s);
	return d;
}

qbool entity_is_func_group_prefixed (const entityx_s *r_ent)
{
	ccs *classname = entity_get (r_ent, "classname");
	if (String_Starts_With_Nullproof (classname, "func_group"))
		return true;
	return false;
}

CALLERS_ (map_get_float)
// FAILURE MEANS RETURN 0
float entity_get_float (const entityx_s *r_ent, ccs *keyname)
{
	ccs *s = entity_get(r_ent, keyname); // SEPT12026PASS

	if (!s)
		return 0;

	float f = atof(s);
	return f;
}


qbool entity_get_vec3 (const entityx_s *r_ent, ccs *keyname, vec3_t vout)
{
	VectorClear (vout);

	ccs *sval = entity_get(r_ent, keyname); // SEPT12026PASS
	if (sval) {
		Math_atov3 (sval, vout);
		return true;
	}

	return false;
}

CALLERS_ (map_set_vec3_g)
//void entity_set_vec3_g (entityx_s *d_ent, ccs *keyname, const vec3_t v)
//{
//	va_super (s_num, MAX_QPATH_128, V3G, VS(v));
//	entity_set (d_ent, keyname, s_num);
//}
//
//CALLERS_ (map_set_vec3_lossless)
//void entity_set_vec3_lossess (entityx_s *d_ent, ccs *keyname, const vec3_t v)
//{
//	va_super (s_num, MAX_QPATH_128, VECTOR3_LOSSLESS, VS(v));
//	entity_set (d_ent, keyname, s_num);
//}

void Vector_SnapToInteger_If_003 (vec3_t snapped)
{
    snapped[0] = Float_SnapToInteger_If_003(snapped[0]);
    snapped[1] = Float_SnapToInteger_If_003(snapped[1]);
    snapped[2] = Float_SnapToInteger_If_003(snapped[2]);
}

void Vector_SnapToInteger_NoNegZero_If_003 (vec3_t snapped)
{
    snapped[0] = Float_SnapToInteger_If_003(snapped[0]);
	if (snapped[0] == 0) snapped[0] = 0; // Kill neg zero
    snapped[1] = Float_SnapToInteger_If_003(snapped[1]);
	if (snapped[1] == 0) snapped[1] = 0; // Kill neg zero
    snapped[2] = Float_SnapToInteger_If_003(snapped[2]);
	if (snapped[2] == 0) snapped[2] = 0; // Kill neg zero
}

void entity_set_vec3_g (entityx_s *d_ent, ccs *keyname, const vec3_t v)
{
    vec3_t snapped;

    VectorCopy(v, snapped);

	if (_csg_snaptointeger.integer) {
		Vector_SnapToInteger_If_003 (snapped);
    }

    va_super (s_num, MAX_QPATH_128, V3G, VS(snapped));
    entity_set (d_ent, keyname, s_num);
}

CALLERS_ (map_set_vec3_lossless)
void entity_set_vec3_lossess (entityx_s *d_ent, ccs *keyname, const vec3_t v)
{
    vec3_t snapped;

    VectorCopy(v, snapped);

    if (_csg_snaptointeger.integer) {
		Vector_SnapToInteger_If_003 (snapped);
    }

    va_super (s_num, MAX_QPATH_128, VECTOR3_LOSSLESS, VS(snapped));
    entity_set (d_ent, keyname, s_num);
}

RELATED_ (map_set_fmt)
void entity_set_fmt (entityx_s *d_ent, ccs *keyname, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	entity_set (d_ent, keyname, text);
	VA_EXPAND_ALLOC_FREE (text);
}




static void _entity_key_rename (entityx_s *d_ent, ccs *key_oldname, ccs *key_newname)
{
	int keyidx = local_entity_key_idx (d_ent, key_oldname);
	if (keyidx == not_found_neg1)
		return; // false;
	
	entity_keyidx_keyname_rename (d_ent, keyidx, key_newname);
}


void entity_set (entityx_s *d_ent, ccs *key, ccs *newvalue)
{
	int keyidx = local_entity_key_idx (d_ent, key);
	if (keyidx == not_found_neg1) {
		// ADD THE NON-EXISTENT KEY - ADD PAIR == 2 ENTRIES
		stringlistappend (&d_ent->pairslist, key);
		stringlistappend (&d_ent->pairslist, newvalue);
	} else {
		// CHANGE THE EXISTING KEY
		entity_keyidx_value_rename (d_ent, keyidx, newvalue);
	}
}

CALLERS_ (map_set_int32)
void entity_set_int32 (entityx_s *d_ent, ccs *keyname, int ivalue)
{
	va_super (s_num, MAX_QPATH_128, "%d", ivalue);
	entity_set (d_ent, keyname, s_num);
}

CALLERS_ (map_set_float)
void entity_set_float (entityx_s *d_ent, ccs *keyname, float fvalue)
{
	va_super (s_num, MAX_QPATH_128, "%f", fvalue);
	entity_set (d_ent, keyname, s_num);
}

//void entity_set_vec3_g (entityx_s *d_ent, ccs *keyname, const vec3_t vec)
//{
//	va_super (s_num, MAX_QPATH_128, VECTOR3_LOSSLESS, VECTOR3_SEND(vec) );
//	entity_set (d_ent, key, s_num);
//}
//
//void entity_set_float (entityx_s *d_ent, ccs *keyname, const vec3_t vec)
//{
//	va_super (s_num, MAX_QPATH_128, VECTOR3_LOSSLESS, VECTOR3_SEND(vec) );
//	entity_set (d_ent, key, s_num);
//}


//
// MAP FORM: KEYS
//


void map_set (map_t *hm, int entnum, ccs *key, ccs *value)
{
	entity_set (ITOE(hm, entnum), key, value);
}


void map_key_delete (map_t *hm, int entnum, ccs *keyname)
{
	_entity_key_rename (ITOE(hm, entnum), keyname, "");
}

void map_set_float (map_t *hm, int entnum, ccs *keyname, float fvalue)
{
	entity_set_float (ITOE(hm, entnum), keyname, fvalue);
}

void map_set_int32 (map_t *hm, int entnum, ccs *keyname, int ivalue)
{
	entity_set_int32 (ITOE(hm, entnum), keyname, ivalue);
}

void map_set_vec3_lossless (map_t *hm, int entnum, ccs *keyname, const vec3_t v)
{
	entity_set_vec3_lossess (ITOE(hm, entnum), keyname, v);
}

void map_set_vec3_g (map_t *hm, int entnum, ccs *keyname, const vec3_t v)
{
	entity_set_vec3_g (ITOE(hm, entnum), keyname, v);
}

void map_set_fmt (map_t *hm, int entnum, ccs *keyname, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	entity_set (ITOE(hm, entnum), keyname, text);
	VA_EXPAND_ALLOC_FREE (text);
}

int map_get_int32 (const map_t *hm, int entnum, ccs *keyname)
{
	return entity_get_int32 (ITOE(hm, entnum), keyname); // SEPT12026PASS
}

float map_get_float (const map_t *hm, int entnum, ccs *keyname)
{
	return entity_get_float(ITOE(hm, entnum), keyname); // SEPT12026PASS
}

// nthness uses this map_get_nthness_classnametargetname so does droptofloor
qbool map_get_keyval_does_match (const map_t *hm, int entnum, ccs *keyname, ccs *match_string)
{
	ccs *s = map_get (hm, entnum, keyname);
	if (String_Match_Nullproof (s, match_string))
		return true;
	return false;
}





// Returns entity number or -1
RELATED_ (map_get entity_get)
int map_find_key_string (map_t *hm, ccs *keyname, ccs *findvalue)
{
	return map_find_key_string_start_beyond(hm, keyname, findvalue, WORLD_0, hm->count);
}

qbool map_find_did_find_brush_texture_starts_with (map_t *hm, ccs *texturename_prefix)
{
	for_each_entity_2 (entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			for_each_brush_face_2 (facenum, d_face, hm, entnum, brushnum)
				if (String_Starts_With (d_face->pbrtexture, texturename_prefix))
					return true;
				
			for_each_end // for_each_brush_face
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity

	return false;
}

// This sets the value pointer too!
entnum_int map_find_key_string_ANY_start_beyond (map_t *hm, ccs *key_to_find, int start_idx, int beyond_idx, ccs **pVal)
{
	for_each_entity_start_beyond_2 (entnum, r_ent, hm, start_idx, beyond_idx)
		ccs *val = map_get (hm, entnum, key_to_find);
		if (!val) continue; // ANY STRING WILL DO!

		NOT_MISSING_ASSIGN (pVal, val);
		return entnum;
	for_each_end // entity

	return not_found_neg1;
}

entnum_int map_find_key_string_start_beyond (map_t *hm, ccs *keyname, ccs *findvalue, int start_idx, int beyond_idx) // "entity find entity_find"
{
	// Check each entity in a range
	for_each_entity_start_beyond_2(entnum, r_ent, hm, start_idx, beyond_idx)
		ccs *val = map_get (hm, entnum, keyname);
		if (String_Match_Nullproof (val, findvalue))
			return entnum;
	for_each_end
	return not_found_neg1;
}

CALLERS_ (mazgen)
entnum_int map_find_key_string_startswith_start_beyond (map_t *hm, ccs *keyname, ccs *valueprefix, int start_idx, int beyond_idx) // "entity find entity_find"
{
	//if (val_prefix[0] == CHAR_NULL_0) return not_found_neg1; // Blank shall never be ok
	for_each_entity_start_beyond_2(entnum, r_ent, hm, start_idx, beyond_idx)
		ccs *val = map_get (hm, entnum, keyname);
		if (String_Starts_With_Nullproof (val, valueprefix))
			return entnum;
	for_each_end
	return not_found_neg1;
}



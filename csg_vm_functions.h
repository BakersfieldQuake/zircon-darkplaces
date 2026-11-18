// csg_vm_functions.h

// Baker: This does world also.

CALLERS_ (VM_map_move)
RELATED_	(brush_or_patch_tranlate_with_texturelock_was_processed) // We call this
void map_move_all_brushes_patches (map_t *hm, const vec3_t move_amount)
{
	int num_moved = 0;
	for_each_entity_2 (entnum, d_ent, hm) // EXAMPLE
		for_each_brush_or_patch_2 (brushnum, d_brush, hm, entnum)
			brush_or_patch_tranlate_with_texturelock_was_processed (d_brush, move_amount);
			num_moved ++;
		 for_each_end // for_each_brush_or_patch
	 for_each_end // for_each_entity
}

RELATED_ (Generate_Dungeon_Map_File calls map_append_map_with_move calls us)

RELATED_ (Generate_Dungeon_Map_File) // OLDY

// This is brush paste.  It does not do epairs or entities.
CALLERS_ (VM_map_append_map map_add map_afterworld_copy)
void map_world_brushes_patches_copy (map_t *hm, map_t *paste)
{
	//ENT_DEFINE_CONST	(r_world, paste, WORLD_0); // R = READ
	ENT_DEFINE			(d_world, hm, WORLD_0); // D = DEST

	for_each_brush_or_patch_const_2 (brushnum, r_brush, paste, WORLD_0) // Oct 21 2026 - deleted == ignored
		brush_add_copying_brush_or_patch (d_world, r_brush);
	for_each_end // for_each_brush_or_patch
}

static void _entity_copy_keys(entityx_s *d_ent, const entityx_s *r_ent)
{
	for_each_key_val_pair (keyidx, r_ent, key, val)
		stringlistappend (&d_ent->pairslist, key);
		stringlistappend (&d_ent->pairslist, val);
	for_each_end // key val pairs
}

void map_copy_keys(map_t *hm_dst, int dstnum, const map_t *hm_src, int srcnum)
{
	ENT_DEFINE			(dest_ent, hm_dst, dstnum);
	ENT_DEFINE_CONST	(src_ent, hm_src, srcnum);

	_entity_copy_keys (dest_ent, src_ent);
}

// Another strategy is copying them all like "_atomize"
// Then deleting the unwanted one.  But this is better.
void entity_copy_keys_except(entityx_s *d_ent, const entityx_s *r_ent, ccs *this_keyname_no_copy)
{
	for_each_key_val_pair (keyidx, r_ent, key, val)
		if (String_Match (key, this_keyname_no_copy)) continue; // Don't copy this one!
		stringlistappend (&d_ent->pairslist, key);
		stringlistappend (&d_ent->pairslist, val);
	for_each_end // key val pairs
}


CALLERS_ (VM_map_append_map)

// CONCAT WITH NO BRUSHES - EPAIRS COPY - LIST PASTE HAS NO WORLD
void map_paste_point_entities (map_t *hm, map_t *paste_map)
{
	// ADD PASTE ENTS TO PLIST
	// WORLD IS NOT ENTITY ZERO HERE -- THESE ARE POINT ENTITIES FROM CLIPBOARD
	for_each_entity_2(entnum, paste_ent, paste_map)
		entityx_s	*d_ent = entitylist_add(hm);	// D = DEST

		for_each_key_val_pair (keyidx, paste_ent, key, val)
			stringlistappend (&d_ent->pairslist, key); // KEY
			stringlistappend (&d_ent->pairslist, val); // VALUE
		for_each_end // key val pairs
	for_each_end // entity
}



// BakerString_Destroy_And_Null_It (&bs);

// Baker: April 15 2025 - Debugging command with no active callers.
// This copies the entities from a .map to the clipboard.
CALLERS_ (NONE as June 2 2025)
RELATED_ (VM_map_clipboard_set) // Manually does it.
void map_to_clipboard_debug_only (map_t *hm)
{
#ifdef _DEBUG
	baker_string_s *bsa = map_save_to_string_bsalloc (hm);

	Clipboard_Set_Text (bsa->string);

	//Con_PrintLinef ("Clipboarded it strlen = %d -- done", (int)bsa->length);
	BakerString_Destroy_And_Null_It (&bsa);
#endif // _DEBUG
}

void entity_move_epairs_origin (map_t *hm, int entnum, const vec3_t move_amount)
{
	vec3_t origin;
	if (!map_get_vec3 (hm, entnum, "origin", origin))
		return;

	VectorAdd (origin, move_amount, origin);

	// REWRITE
	map_set_vec3_lossless (hm, entnum, "origin", origin);
}

RELATED_ (VM_map_move)
void map_move_epairs_origin (map_t *hm, const vec3_t move_amount)
{
	for_each_entity_2(entnum, d_ent, hm)
		entity_move_epairs_origin (hm, entnum, move_amount);
	for_each_end // for_each_entity
}

int csg_vm_prefix_targetnames_list (map_t *hm, ccs *prefix, stringlist_t *plist_prefixes)
{
	CSGM_START_
	for_each_entity_2(entnum, d_ent, hm)
		for_each_key_val_pair (keyidx, d_ent, key, val)
			qbool match_any_in_list = (stringlist_find_index (plist_prefixes, key) != not_found_neg1);

			if (!match_any_in_list)
				continue;

			entity_keyidx_value_rename_fmt (d_ent, keyidx, "%s%s", prefix, val);
			CSGM_NUM_DONE_PLUS_
		for_each_end // key val pairs
	for_each_end // for_each_entity
	CSGM_END_FUNC_
}



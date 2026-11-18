// csg_p_map_load.c.h

// is_world_or_func_group is now part of metric
RELATED_ (map_save_to_string_bsalloc map_save_as map_metric_cubeoid_mark_FULL_METRIC_AVAIL)
qbool map_load_from_string (map_t *hm, ccs *txt)
{
	qbool isok = private_map_load_from_string_parsemaptxt (hm, txt);
	if (!isok) {
		// Sept 28 2026 - I suspect a partial parse won't fail to destroy with an error.
		// I can't think of anything free activity that would get tripped up.
		map_freecontents (hm); // Destroy any partial parse
		return false;
	}
	
	map_metric_all			(hm);
	map_metric_cubeoid_mark_FULL_METRIC_AVAIL (hm);

	for_each_entity_2 (entnum, d_ent, hm)
		d_ent->original_brush_count = d_ent->brushlist.count;
	for_each_end // for_each_entity

	return true;
}


// Returns 0 if successful
CALLERS_ (VM_map_load and others)
int map_load_or_errorcode (map_t *hm, ccs *filename) // V2
{
	char *maptext_za = FS_LoadFileQuick (filename);
	if (!maptext_za)
		return not_found_neg1; // File not found

	int isok = map_load_from_string (hm, maptext_za);
	Mem_FreeNull_ (maptext_za);

	int ret = SUCCESS_OK_0;
	if (!isok)
		ret = -3; // Opened, couldn't parse the data.

	return ret;
}


// Exports to string and then loads that string
// Sep 24 2026 - An improvement might be create a new map and do map append map?
// Making sure to copy worldspawn keys?
qbool map_reload (map_t *hm)
{
	baker_string_s *bsa = map_save_to_string_bsalloc (hm);
	map_freecontents (hm);
	qbool ok = map_load_from_string (hm, bsa->string);
	BakerString_Destroy_And_Null_It (&bsa);
	return ok;
}



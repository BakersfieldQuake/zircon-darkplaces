// csg_lifecycle.c.h

//
// brushrowlist -- entry free, list free, list add
//

static void brush_face_free (brushrow_s *face_destroy_free)
{
	Mem_FreeNull_ (face_destroy_free->pbrtexture);
}


CALLERS_ (brush_free)
static void brushrowlistfreecontents (brushrowlist_t *list_brushrows_destroy_free)
{
	for (int j = 0;j < list_brushrows_destroy_free->count; j++) {
		brushrow_s *face_destroy_free = &list_brushrows_destroy_free->brushrow[j];
		brush_face_free (face_destroy_free);
	}
	list_brushrows_destroy_free->count = list_brushrows_destroy_free->maxsize = 0;
	Mem_FreeNull_ (list_brushrows_destroy_free->brushrow);
}

// ADD .... brush_s *p_brushrow = _LIFE_brushface_alloc(&p_brush->brushrowlist);
RELATED_ (brush_face_free stringlistappend) // September 12 2026 - closest stringlist function

int map_add_face(map_t *hm, int entnum, int brushnum)
{
	int facenum = FACE_COUNT(hm, entnum, brushnum);
	BRUSH_DEFINE(br, hm, entnum, brushnum);
	//brushrow_s *d_new_face =
        brush_add_face (br);
	return facenum;
}


brushrow_s *_LIFE_brushface_alloc (brushrowlist_t *list)
{
	if (list->count >= list->maxsize) {
#ifdef _DEBUG
		size_t sizeof_brushrow_s= sizeof(brushrow_s);
		size_t sizeof_alloc = list->maxsize * sizeof(brushrow_s);
#endif
		list->maxsize += 16; // Baker: 4096 is way too big for this.  Page array, right?
		list->brushrow = (brushrow_s *)Mem_Realloc(zonemempool,
			list->brushrow, list->maxsize * sizeof(brushrow_s) );
	}

	brushrow_s *p_brushrow = &list->brushrow[list->count ++]; // Incremented count
	return p_brushrow;
}

//
// brushlist -- entry free, list free, list add
//

RELATED_ (brush_face_free)
void brush_free(brush_s *destroy_free_brush)
{
	brushrowlistfreecontents	(&destroy_free_brush->brushrowlist);

	// As patch
	Mem_FreeNull_ (destroy_free_brush->patch_texture);
	patchrowlistfreecontents	(&destroy_free_brush->patchrowlist);
}

void brushlistfreecontents (brushlist_t *list_brushlist_destroy_free)
{
	// Baker: August 15 2026 - this is a rare pass of a brushlist
	for (int j = 0;j < list_brushlist_destroy_free->count; j++) {
		brush_free (&list_brushlist_destroy_free->brush[j]);
	}
	list_brushlist_destroy_free->count = list_brushlist_destroy_free->maxsize = 0;
	Mem_FreeNull_ (list_brushlist_destroy_free->brush);
}

RELATED_ (stringlistappend map_entity_add_brush)

int map_add_brush (map_t *hm, int entnum)
{
	int brushnum = BRUSH_COUNT(hm, entnum);
	//brush_s *br =
        entity_add_brush (hm, entnum);
	return brushnum;
}

// ADD .... brush_s *p_brush = _LIFE_brush_add(&p_ent->brushlist);
RELATED_ (map_add_brush)
brush_s *_LIFE_brush_add (brushlist_t *list)
{
	if (list->count >= list->maxsize) {
		list->maxsize += 32; // Baker: 4096 is really wasteful
		list->brush = (brush_s *)Mem_Realloc(zonemempool,
			list->brush, list->maxsize * sizeof(brush_s) );
	}

	brush_s *p_brush = &list->brush[list->count ++]; // Incremented count
	return p_brush;
}

//
// patchrowlist -- entry free, list free, list add
//

static void patchrow_free(patchrow_s *p_patchrow)
{
	// We do nothing!
}


void patchrowlistfreecontents (patchrowlist_t *list)
{
	for (int j = 0;j < list->count; j++) {
		patchrow_free (&list->patchrow[j]);
	}
	list->count = list->maxsize = 0;
	Mem_FreeNull_ (list->patchrow);
}

// ADD .... patch_s *p_patchrow = patchrowlist_add(&p_patch->patchrowlist);
patchrow_s *patchrowlist_add (patchrowlist_t *list)
{
	if (list->count >= list->maxsize) {
		list->maxsize += 16; // Baker: 4096 is stupidly wasteful
		list->patchrow = (patchrow_s *)Mem_Realloc(zonemempool,
			list->patchrow, list->maxsize * sizeof(patchrow_s) );
	}

	patchrow_s *p_patchrow = &list->patchrow[list->count ++]; // Incremented count
	return p_patchrow;
}

//
// entitylist -- entry free, list free, list add
//

static void entity_free(entityx_s *destroy_free_this_entity)
{
	stringlistfreecontents	(&destroy_free_this_entity->pairslist);
	brushlistfreecontents	(&destroy_free_this_entity->brushlist);
}


RELATED_ (stringlistappend)

void map_freecontents(map_t *plist_destroy_free)
{
	stringlistfreecontents	(&plist_destroy_free->list_textures);
	int32s_freecontents		(&plist_destroy_free->list_sizes2);

	for_each_entity_even_deleted (entnum, plist_destroy_free, destroy_free_entity)
		RELATED_ (__entityx_s)
		entity_free (destroy_free_entity);
	for_each_end // for_each_entity_even_deleted
	plist_destroy_free->count = plist_destroy_free->maxsize = 0;
	Mem_FreeNull_ (plist_destroy_free->entity);
}


entnum_int map_add (map_t *hm, ccs *classname)
{
	int entnum = map_add_entity_empty (hm); // Can't fail.
	entity_set (ITOE(hm, entnum), "classname", classname); // Can't fail.
	return entnum;
}

// September 14 2026: Set collide_out to origin and try to drop it.
qbool map_traceline_drop_ok (const map_t *hm, vec3_t org, int ignore_entnum, vec3_t collide_out)
{
	VectorCopyDestSrc (collide_out, org);

	int num_collides = 0;
	int entnum_collide = not_found_neg1;
	int brushnum_collide = not_found_neg1;
	char classname_collide[64];

	for_each_entity_2 (entnum, ent, hm)
		if (entnum == ignore_entnum)
			continue;

		// We don't collide against things without brushes.
		if (false == ent->has_brushes) continue;

		// Collide against brushes.
		for_each_brush_2 (brushnum, brush, hm, entnum)
			if (!in_range (brush->bxmins[0], org[0],  brush->bxmaxs[0])) continue; // No X collide
			if (!in_range (brush->bxmins[1], org[1],  brush->bxmaxs[1])) continue; // No Y collide

			// Now .. z max must be lower
			if (brush->bxmaxs[2] > org[2]) continue; // It's above us.

			// HIT - But is it best?  We want the largest one. (Highest thing below us)
			if (num_collides > 0 && brush->bxmaxs[2] <= collide_out[2]) continue; // We aren't the highest

			entnum_collide = entnum;
			brushnum_collide = brushnum;
			collide_out[2] = brush->bxmaxs[2]; // Best collision
			ccs *classname = map_get (hm, entnum, "classname");
			if (classname)
				c_strlcpy (classname_collide, classname);
			else
				c_dpsnprintf1 (classname_collide, "No class ent # %d", entnum);
			num_collides ++;
		for_each_end // for_each_brush_or_patch
	for_each_end // for_each_entity_original_count_undeleted

	if (num_collides)
		Con_PrintLinef ("Collided with entity #%d %s brush %d", entnum_collide, classname_collide, brushnum_collide);
	else
		Con_PrintLinef ("No collision");
	return (num_collides > 0);
}



// Baker: May 31 2025 - FIX_THIS_SHIT
entityx_s *entitylist_add_at_1_shiftup (map_t *list)
{
	if (list->count >= list->maxsize) {
		list->maxsize += 1024; // Baker: 4096 is rather large
		list->entity = (entityx_s *)Mem_Realloc(zonemempool,
			list->entity, list->maxsize * sizeof(entityx_s) );
	}

//	entityx_s *pe = &list->entity[list->count ++]; // Incremented count
//	return pe;

	int newidx = list->count; // live: 480

	// 0
	// 1
	// 99
	// 100 <--- us (newidx)
	// we move 1 to 99 to 2 100 ...
	//Dec 22 2024 unused ... entityx_s *pstructfake = entitylist_add(list);


	// Baker: This is a page array.  These aren't pointers!
	size_t startidx = 1;
	size_t move_num_entries = /*beyond*/ newidx /*100*/ - startidx /*1*/ ; // Result 99 is size of move

	entityx_s *pstart1 = &list->entity[1];
	entityx_s *p2 = &list->entity[2];
	//entityx_s *p479 = &list->entity[newidx - 1];
	//entityx_s *p480 = &list->entity[newidx];
	size_t struct_size = sizeof(*pstart1); // 1 to 479 get moved to 2 thru 480.  This 479 entries.

	//Dec 22 2024 unused ... entityx_s *pdest  = &list->entity[2];
	size_t movesize = struct_size * move_num_entries;
	memmove (p2, pstart1, movesize);
	entityx_s *pe  = &list->entity[1];
	memset (pe, 0, sizeof(*pe));

	list->count ++;
	return pe;
}

//
//
//

RELATED_ (VM_map_entity_add)
RELATED_ (map_add) // <--------------- often better
int map_add_entity_empty (map_t *plist)
{
	int entnum = plist->count;
	/*entityx_s *xt = unused Dec 22 2024*/ entitylist_add (plist);
	return entnum;
}

RELATED_ (_parsemap_readfaces_patchblock_isok)
// October 6 2026 - Some of this is a bit ugly -- MOVE ALONG
entnum_int _csgw_sky_decal_add_decal_512_256_with_texturename (map_t *hm, ccs *texturename0)
{
	// Remove the textures/
	int mylen = STRINGLEN("textures/");
	ccs *texturename = String_Starts_With (texturename0, "textures/") ? &texturename0[mylen] : texturename0;
	
	int decal_entnum = map_add (hm, "_decal"); // Can't fail.
	ENT_DEFINE (d_ent_decal, hm, decal_entnum);

	brush_s *p_brush = _LIFE_brush_add (&d_ent_decal->brushlist); // CREATE BRUSH FOR DEST
	p_brush->is_a_patch = true;
	p_brush->patch_texture = Z_StrDup (texturename);
	p_brush->patch_rows = 3; // atof(com_token); // ( 3 3 0 0 0 )   3 is rows
	p_brush->patch_cols = 3; // p_brush->cols = atof(com_token); //     3				is columns
	p_brush->other3[0] = 536870912; // atof(com_token);
	p_brush->other3[1] = 16384; // atof(com_token);
	p_brush->other3[2] = 0; // atof(com_token);

	int idx, my_col;
	int items_read;
	ccs *s0, *s1, *s2;
	patchrow_s *p_row;

	// ROW 0    
	p_row = patchrowlist_add (&p_brush->patchrowlist);
		// Scan the string for 5 floating-point numbers
		s0 = "-256 256 0.199997 0 -0.000000";
		s1 = "0 256 0.199997 0.500000 0";
		s2 = "256 256 0.199997 1 0.000000";

		my_col = 0; idx = my_col * 5; /* 5 is XYZST*/
		items_read = sscanf(s0, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 1; idx = my_col * 5;
		items_read = sscanf(s1, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 2; idx = my_col * 5;
		items_read = sscanf(s2, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}

	// ROW 1
	p_row = patchrowlist_add (&p_brush->patchrowlist);
		// Scan the string for 5 floating-point numbers
		s0 = "-256 0 0.199997 0 0.500000";
		s1 = "0 0 0.199997 0.500000 0.500000";
		s2 = "256 0 0.199997 1 0.500000";

		my_col = 0; idx = my_col * 5; /* 5 is XYZST*/
		items_read = sscanf(s0, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 1; idx = my_col * 5;
		items_read = sscanf(s1, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 2; idx = my_col * 5;
		items_read = sscanf(s2, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}

	// ROW 2
	p_row = patchrowlist_add (&p_brush->patchrowlist);
		s0 = "-256 -256 0.199997 0 1.000000";
		s1 = "0 -256 0.199997 0.500000 1";
		s2 = "256 -256 0.199997 1.000000 1";

		my_col = 0; idx = my_col * 5; /* 5 is XYZST*/
		items_read = sscanf(s0, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 1; idx = my_col * 5;
		items_read = sscanf(s1, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}
		my_col = 2; idx = my_col * 5;
		items_read = sscanf(s2, "%f %f %f %f %f", &p_row->flots[idx + 0], &p_row->flots[idx + 1], &p_row->flots[idx + 2], &p_row->flots[idx + 3], &p_row->flots[idx + 4]);
	//	if (items_read != 5) {
	//		int j = 5;
	//	}

	return decal_entnum;
}

// END

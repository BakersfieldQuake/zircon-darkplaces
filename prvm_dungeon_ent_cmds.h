// prvm_dungeon_ent_cmds.h

//float map_append_map (float hmapdest, float hmapsrc) = #152;
//VM entity_attributes
// entity_has_originbrush
//
//VM entity_has_originbrush
//VM entity translate
//VM replace texture
//VM entity find texture
//num textures

//void VM_entity_move (prvm_prog_t *prog) // #151 float map_move (float hmap, vector move) = #151;
//{
//	map_move_epairs_origin	(&list_paste, translate_amount);
//	map_move_all_brushes_patches		(&list_paste, translate_amount);
//}


// #160 float map_entity_key_count (float hmap, float entnum) = #159;
// Returns entity number, 0 is world, failure returns -1 on invalid entnum
// Returns -4 on handle failure.
// float numkeys = map_entity_key_count (h, entum)
void VM_map_entity_key_count (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 2, VM_map_entity_key_count); // hmap, entnum // opt:

	HMAP_HANDLE_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);

	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	entityx_s *r_ent = &pe->entity[entnum];
	int num_keys_for_ent = r_ent->pairslist.numstrings / 2; // Number of pairs is numstrings divided by 2.

	PRVM_G_FLOAT(OFS_RETURN) = num_keys_for_ent;
}


// #161 string map_entity_key_for_index (float hmap, float entnum, float keyindex) = #160;
// Returns NULL (world) on failure
// string skeyval = map_entity_key_for_index(h, entnum, /*keyindex*/ 0);

RELATED_ (VM_fgets)
void VM_map_entity_key_for_index (prvm_prog_t *prog)
{
	char string16384[VM_STRINGTEMP_LENGTH_16384];

	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_entity_key_for_index); // hmap, entnum, keyindex // opt:

	// set the return value regardless of any possible errors
	PRVM_G_INT(OFS_RETURN) = OFS_NULL;

	HMAP_HANDLE_STRING_RETURN_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	int keyindex = (int)PRVM_G_FLOAT(OFS_PARM2);

	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		return;
	}

	entityx_s *r_ent = &pe->entity[entnum];
	int num_keys_for_ent = r_ent->pairslist.numstrings / 2; // Number of pairs is numstrings divided by 2.

	if (in_range_beyond (0, keyindex, num_keys_for_ent) == false) {
		VM_WarningLinef (prog, "%s: keyindex %d invalid for entnum %d, num_keys_for_ent is %d in %s", __func__, keyindex, entnum, num_keys_for_ent, prog->name);
		return;
	}

	int index_mult_2 = keyindex * 2;
	ccs *keyname = r_ent->pairslist.strings[index_mult_2 + 0];
	//ccs *keyval = r_ent->pairslist.strings[index_mult_2 + 1];

	dp_strlcpy (string16384, keyname, sizeof(string16384));

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, string16384);
}



// Returns brush number created or -1 on failure.
// #167 float map_entity_brush_add_cubeoid (float hmap, float entnum, vector brushmins, vector brushmaxs,
//	string texture, float texture_flags, optional vector texture_scale) = #167;
void VM_map_entity_brush_add_cubeoid(prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(6, 7, VM_map_entity_brush_add_cubeoid); //
	PRVM_G_FLOAT(OFS_RETURN) = not_found_neg1;

	HMAP_HANDLE_STRING_RETURN_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		return;
	}

	entityx_s *d_ent = &pe->entity[entnum];

	vec3_t bbmins, bbmaxs;
	VectorCopyDestSrc(bbmins, PRVM_G_VECTOR(OFS_PARM2));
	VectorCopyDestSrc(bbmaxs, PRVM_G_VECTOR(OFS_PARM3));
	vec3_t bbsize;
	VectorSubtract (bbmaxs, bbmins, bbsize);

	ccs *texturename = PRVM_G_STRING(OFS_PARM4);	VM_CheckEmptyString(prog, texturename);
	int texture_side_flags = (int)PRVM_G_FLOAT(OFS_PARM5);

	if (bbsize[0] <= 0 || bbsize[1] <= 0 || bbsize[2] <= 0) {
		VM_WarningLinef (CLVM_prog, "%s: size < 0 for a dimension (texflags = %d) " VECTOR3_5d1F,
			__func__,
			texture_side_flags,
			VECTOR3_SEND(bbsize)
		);
		return;
	}

	vec3_t texture_scale;
	texture_scale[0] = 0.125; // 1 / 8 or 8 times magnification
	texture_scale[1] = 0.125; // 1 / 8 or 8 times magnification
	if (prog->argc > 6)
		VectorCopyDestSrc(texture_scale, PRVM_G_VECTOR(OFS_PARM6));
	texture_scale[2] = 0;

	if (texture_scale[0] <= 0 || texture_scale[1] <= 0) {
		VM_WarningLinef (CLVM_prog, "%s: texture_scale <= 0 for a dimension " VECTOR3_5d1F, __func__, VECTOR3_SEND(texture_scale));
		return;
	}

	RELATED_ (brush_list_model_add_wall_isok)

	int brushnum = brush_add_cubeoid (pe, d_ent, bbmins, bbmaxs,
		texturename, texture_scale[0], texture_scale[1], texture_side_flags);

	PRVM_G_FLOAT(OFS_RETURN) = brushnum;
}

void VM_map_entity_keyvalue_for_index (prvm_prog_t *prog)
{
	char string16384[VM_STRINGTEMP_LENGTH_16384];

	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_entity_keyvalue_for_index); // hmap, entnum, keyindex // opt:

	// set the return value regardless of any possible errors
	PRVM_G_INT(OFS_RETURN) = OFS_NULL;

	HMAP_HANDLE_STRING_RETURN_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	int keyindex = (int)PRVM_G_FLOAT(OFS_PARM2);

	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		return;
	}

	entityx_s *r_ent = &pe->entity[entnum];
	int num_keys_for_ent = r_ent->pairslist.numstrings / 2; // Number of pairs is numstrings divided by 2.

	if (in_range_beyond (0, keyindex, num_keys_for_ent) == false) {
		VM_WarningLinef (prog, "%s: keyindex %d invalid for entnum %d, num_keys_for_ent is %d in %s", __func__, keyindex, entnum, num_keys_for_ent, prog->name);
		return;
	}

	int index_mult_2 = keyindex * 2;
	//ccs *keyname = r_ent->pairslist.strings[index_mult_2 + 0];
	ccs *keyvalue = r_ent->pairslist.strings[index_mult_2 + 1];

	dp_strlcpy (string16384, keyvalue, sizeof(string16384));

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, string16384);
}

// #162 string map_entity_get_key_value (float hmap, float entnum, string key) = #162;
// string s = map_entity_get_key_value (h, 1, "classname");
void VM_map_entity_key_get_value (prvm_prog_t *prog)
{
	char string16384[VM_STRINGTEMP_LENGTH_16384];

	// set the return value regardless of any possible errors
	PRVM_G_INT(OFS_RETURN) = OFS_NULL;

	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_entity_key_get_value); // hmap 0, entnum 1, keyname 2 // opt:

	HMAP_HANDLE_STRING_RETURN_BOILER;

	map_t *pe = prog->open_dot_maps[handle];
	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	//entityx_s *r_ent = &pe->entity[entnum];

	ccs *keyname = PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, keyname);
	//ccs *keyvalue = entity_get (r_ent, keyname);
	ccs *keyvalue = map_get (pe, entnum, keyname);

	if (!keyvalue) {
		// Not found, return NULL
		return;
	}

	dp_strlcpy (string16384, keyvalue, sizeof(string16384));
	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, string16384);
}

// delete_action -1 = query if deleted
// delete_action  1 = delete
// delete_action  0 = delete
#define DELETE_ACTION_QUERY_NEG1	-1
#define DELETE_ACTION_DELETE_1		1
// #165 float map_entity_delete (float hmap, float entnum, float delete_action) = #165;
// return value LESS THAN ZERO on failure
// query returns true if deleted, false if status is not deleted
// float delete_ret = map_entity_delete(hmap, 15, DELETE_ACTION_DELETE_1); // Returns 1 on delete ok, less than zero on error
// float delete_ret = map_entity_delete(hmap, 15, DELETE_ACTION_QUERY_NEG1); // Returns 1 if deleted, 0 if not deleted, negative for error
void VM_map_entity_delete(prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_entity_delete); // hmap 0, entnum 1, keyname 2, 3 value // opt:
	HMAP_HANDLE_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	int delaction = (int)PRVM_G_FLOAT(OFS_PARM2);
	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	entityx_s *d_ent = &pe->entity[entnum];
	if (delaction == DELETE_ACTION_QUERY_NEG1) {
		PRVM_G_FLOAT(OFS_RETURN) = d_ent->is_considered_deleted ? 1 : 0;
		return;
	}

	if (delaction == DELETE_ACTION_DELETE_1) {
		d_ent->is_considered_deleted = true;
		PRVM_G_FLOAT(OFS_RETURN) = 1;
		return;
	}

	PRVM_G_FLOAT(OFS_RETURN) = -2; // Don't know what this is
}

// #163 float map_entity_set_key_value (float hmap, float entnum, string key, string value) = #163;
// float isok = map_entity_set_key_value (h, 1, "classname", "ted");
// string keyvalue = map_entity_get_key_value (h, 1, "classname");

// OBSOLETE A LONG TIME: If the key does not exist, fail.  Do NOT add the key.
// September 1 2026 - I would call this map_entity_key_set_string or just map_set
// And we do add the key if it doesn't exist
void VM_map_entity_key_set_value (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(4, 4, VM_map_entity_key_set_value); // hmap 0, entnum 1, keyname 2, 3 value // opt:

	HMAP_HANDLE_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	//entityx_s *r_ent = &pe->entity[entnum];

	ccs *keyname	= PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, keyname);
	ccs *keyvalue	= PRVM_G_STRING(OFS_PARM3);	VM_CheckEmptyString(prog, keyvalue);

	map_set (pe, entnum, keyname, keyvalue); // September 1 2026 - Can't fail -- but what does it do if entity doesn't exist?

	PRVM_G_FLOAT(OFS_RETURN) = 1;
}

// #164 float map_entity_get_mins_maxs (float hmap, float entnum, __inout vector mins_out, __inout vector maxs_out) = #164;
// float wasprocessed = map_entity_get_mins_maxs (h, /*entnum* 1, minsout, maxsout);
void VM_map_entity_get_mins_maxs (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(4, 4, VM_map_entity_key_count); // hmap 0, entnum 1, minsout 2, maxsout 3 // opt:

	HMAP_HANDLE_BOILER;

	int entnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	map_t *pe = prog->open_dot_maps[handle];

	if (in_range_beyond (0, entnum, pe->count) == false) {
		VM_WarningLinef (prog, "%s: entnum %d invalid, numents is %d in %s", __func__, entnum, pe->count, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return;
	}

	entityx_s *r_ent = &pe->entity[entnum];

	vec3_t pmins, pmaxs;
	bounds_init_9 (pmins, pmaxs);

	int did_any = entity_expand_bounds_did_brushes(r_ent, pmins, pmaxs);

	if (did_any == 0) {
        // Did not find any brushes
        PRVM_G_FLOAT(OFS_RETURN) = 0;
        return;
	}
	VectorCopyDestSrc(PRVM_G_VECTOR(OFS_PARM2), pmins);
	VectorCopyDestSrc(PRVM_G_VECTOR(OFS_PARM3), pmaxs);

	PRVM_G_FLOAT(OFS_RETURN) = 1;
}


// m->listsolvesteps // %d %d row col
RELATED_ (SCR_mazegen_f mazegen_s solve_recursive)
void maze_generate (stringlist_t *plist_maze, int cols, int rows, stringlist_t *plist_solve, vec3_t start, vec3_t endout, int numblots);

// Returns 1 on success
// Returns 0 on failure
// #175 float maze_generate (vector dimensions_algo, //0
//							float plist, // 1
//							optional float plist_solve, // 2
//							optional vector startout,  // 3
//							optional vector endout,  // 4
//							optional float genflags) = ##175;

// V2: float halls_generate (vector dimensions_algo, float mazebufnum, float numrooms, float fixedroomsbufnum) = #175;

// maze_generate([mm.numcolumns, mm.numrows], mm.plist_maze, mm.plist_solve, mm.startpos, mm.endpos);

// plist_fixed_coords are forced room locations ... A will be first room
// - and | (minus ascii 45) and pipe ascii (124) will be used for halls
// spaces will be SOLID SPACE
// and rooms will start with "0" ascii 48 and ascend from there.
// For each fixed coord, the location of that will get

RELATED_ (SCR_mazegen2_f)
// V2: float halls_generate (vector dimensions_algo, float mazebufnum, float numrooms, float fixedroomsbufnum) = #175;
#define ALGO_HALLS2	2
void VM_maze_generate (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 6, VM_maze_generate); // hmap "classname", "shambler"  // opt:
	PRVM_G_FLOAT(OFS_RETURN) = 0; // Assume failure

	vec3_t dimensions_algo;
	VectorCopyDestSrc (dimensions_algo, PRVM_G_VECTOR(OFS_PARM0));
	int numcolumns = dimensions_algo[0];
	int numrows = dimensions_algo[1];
	int algo = dimensions_algo[2];

	int mazebufnum = (int)PRVM_G_FLOAT(OFS_PARM1);
	prvm_stringbuffer_t *stringbuffer_maze = stringbuffer_getlist_by_index (prog, mazebufnum);
	if (!stringbuffer_maze) {
		VM_WarningLinef (prog, "VM_maze_generate: mazebufnum invalid buffer %d", mazebufnum);
		return;
	}
	if (stringbuffer_maze->num_strings != 0) {
		VM_WarningLinef (prog, "VM_maze_generate: mazebufnum is not empty, has %d strings", stringbuffer_maze->num_strings);
		return;
	}

	prvm_stringbuffer_t *stringbuffer_solve = NULL;

	if (prog->argc > 2) {
		int solvebufnum = (int)PRVM_G_FLOAT(OFS_PARM2);
		stringbuffer_solve = stringbuffer_getlist_by_index (prog, solvebufnum);
		if (!stringbuffer_maze) {
			VM_WarningLinef (prog, "VM_maze_generate: solvebufnum invalid buffer %d", solvebufnum);
			return;
		}
		if (algo != ALGO_HALLS2 && stringbuffer_solve->num_strings != 0) {
			VM_WarningLinef (prog, "VM_maze_generate: solvebufnum is not empty, has %d strings", stringbuffer_solve->num_strings);
			return;
		}
	} // solve buf

	if (algo == ALGO_HALLS2) { // 0                        1          2                             3
		if (prog->argc != 4) {
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 requires 4 args, received %d", prog->argc);
		}

		//float halls_generate (vector dimensions_algo, float plist, float plistfixedroomsbufnum, float numrooms) = #175;
		int numrooms = (int)PRVM_G_FLOAT(OFS_PARM3);
		if (numrooms <= 0) {
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 numrooms = %d", numrooms);
			return;
		}

		if (numrooms > 110) {
			// We run out of ascii characters
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 numrooms exceeds 110 = %d", numrooms);
			return;
		}

		if (numrooms > (int)((numrows * numcolumns)/3) ) {
			// We run out of ascii characters
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 numrooms too high 100 = %d", numrooms);
			return;
		}

		stringlist_t list_fixed_rooms = {0}; // We will add new rooms to this.

		prvm_stringbuffer_t *stringbuffer_fixed_rooms = stringbuffer_solve;

		if (stringbuffer_maze->num_strings > 0) {
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 stringbuffer_maze->num_strings  (%d) > 0", stringbuffer_maze->num_strings);
			return;
		}

		// Fill list_fixed_rooms if it exists
		if (stringbuffer_fixed_rooms) {
			stringlist_append_from_stringbuffer (&list_fixed_rooms, prog, stringbuffer_fixed_rooms);

			if (stringbuffer_fixed_rooms->num_strings > numrooms) {
				VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 stringbuffer_fixed_rooms->num_strings (%d) > numrooms (%d)", stringbuffer_fixed_rooms->num_strings, numrooms);
				return;
			}

			// VALIDATE THE FIXED ROOMS
			for (int sidx = 0; sidx < list_fixed_rooms.numstrings; sidx ++) {
				char *sxy = list_fixed_rooms.strings[sidx];
				vec3_t vroomcoords = {0};
				Math_atov2 (sxy, vroomcoords);
				int col = vroomcoords[0];
				int row = vroomcoords[1];
				if (false == in_range_beyond(0, col, numcolumns) || false == in_range_beyond(0, row, numrows)) {
					VM_WarningLinef (prog,
						"VM_maze_generate: fixed room %d is at col %d row %d out-of-bounds for numcolumns %d numrows %d",
						sidx, col, row, numcolumns, numrows);
					return;
				}
			} // for
		}

		char *Halls_Generate_Zalloc (int numcolumns, int numrows, int numrooms, stringlist_t *plist_supplied_room_coords, char *room_bytes_optional_400);
		char *s_z = Halls_Generate_Zalloc (numcolumns, numrows, numrooms, &list_fixed_rooms, /*room bytes engine string 400*/ NULL);

		if (strlen(s_z) == 0) {
			VM_WarningLinef (prog, "VM_maze_generate: ALGO_HALLS2 maze generation failed likely due to high room density for size");
			return;
		}

		// BUILD THE LIST_MAZE from s_z
		int slen = (int)strlen(s_z);
		for (int n = 0; n < slen; n ++) {
			int ch = s_z[n];
			stringbuffer_appendf (prog, stringbuffer_maze, "%c", ch);
		}
		Mem_FreeNull_ (s_z);

		// Take list list_fixed_rooms and add to stringbuffer
		if (stringbuffer_fixed_rooms) {
			int numfixedrooms = stringbuffer_fixed_rooms->num_strings;
			for (int sidx = numfixedrooms; sidx <  list_fixed_rooms.numstrings; sidx ++) {
				ccs *sxy = list_fixed_rooms.strings[sidx];
				stringbuffer_append (prog, stringbuffer_fixed_rooms, sxy);
			} // sidx
		}

		stringlistfreecontents (&list_fixed_rooms);
		PRVM_G_FLOAT(OFS_RETURN) = true; // Boolean "is ok" return
		return;
	}

	int mazeflags = 0;
	if (prog->argc > 5) {
		mazeflags = (int)PRVM_G_FLOAT(OFS_PARM5);
	}

	if (numrows < 8) {
		VM_WarningLinef (prog, "VM_maze_generate: rows = %d which is less than 8", numrows);
		return;
	}
	if (numcolumns < 8) {
		VM_WarningLinef (prog, "VM_maze_generate: cols = %d which is less than 8", numcolumns);
		return;
	}

	if (numcolumns > 100) {
		VM_WarningLinef (prog, "VM_maze_generate: cols = %d which is greater than 100", numcolumns);
		return;
	}

	if (numrows > 100) {
		VM_WarningLinef (prog, "VM_maze_generate: rows = %d which is greater than 100", numrows);
		return;
	}

	if (numcolumns != numrows) {
		VM_WarningLinef (prog, "VM_maze_generate: this algorithm requires same rows as cols, rows = %d cols = %d", numrows, numcolumns);
		return;
	}

	vec3_t startout = {0}, endout = {0};
	stringlist_t list_maze = {0};
	stringlist_t list_solve = {0};
	maze_generate (&list_maze, numcolumns, numrows, &list_solve, startout, endout, /*numblots*/ 0);

	stringbuffer_append_stringlist (&list_maze, prog, stringbuffer_maze); // dst src
	stringbuffer_append_stringlist (&list_solve, prog, stringbuffer_solve); // dst src

	if (prog->argc > 3)
		VectorCopyDestSrc(PRVM_G_VECTOR(OFS_PARM3), startout);

	if (prog->argc > 4)
		VectorCopyDestSrc(PRVM_G_VECTOR(OFS_PARM4), endout);

	stringlistfreecontents (&list_maze);
	stringlistfreecontents (&list_solve);

	PRVM_G_FLOAT(OFS_RETURN) = true; // Boolean "is ok" return
}

// #176: TODO
void VM_maze_solve (prvm_prog_t *prog)
{
	PRVM_G_FLOAT(OFS_RETURN) = -1;
}

// April 17 2026 - Not used yet.
// We will see ...
void VM_dungeon_build (prvm_prog_t *prog)
{
	//
	RELATED_ (SV_OnEntityPreSpawnFunction map_prefix_dungeon_targetnamesf)

}

// #169 float map_entity_add (float hmap, string keyname, string keyvalue) = #169;
//int entnum = map_entity_add (h, "classname", "shambler");
void VM_map_entity_add (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_entity_add); // hmap "classname", "shambler"  // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	ccs *keyname	= PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, keyname);
	ccs *keyvalue	= PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, keyvalue);

	int entnum = map_add_entity_empty (pe); // Can't fail.

	entityx_s *r_ent = &pe->entity[entnum];

	/*int isok = */ entity_set (r_ent, keyname, keyvalue); // Can't fail.

	PRVM_G_FLOAT(OFS_RETURN) = entnum;
}

// #171 float map_texture_replace (float hmap, string find, string replace) = #171;
// float numdone = map_texture_replace(h, "texturecan/tiles_0014", "texturecan/tiles_0019");
// Textures do not have extensions in .map files.  Returns number of replacements performed.
void VM_map_texture_replace (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 3, VM_map_texture_replace); // hmap sfind sreplacewith // opt:

	HMAP_HANDLE_BOILER;

	map_t *pe = prog->open_dot_maps[handle];

	ccs *texturefind	= PRVM_G_STRING(OFS_PARM1);	VM_CheckEmptyString(prog, texturefind);
	ccs *texturereplace	= PRVM_G_STRING(OFS_PARM2);	VM_CheckEmptyString(prog, texturereplace);

	int numreplaces = map_texture_replace (pe, texturefind, texturereplace);

	PRVM_G_FLOAT(OFS_RETURN) = numreplaces;
}



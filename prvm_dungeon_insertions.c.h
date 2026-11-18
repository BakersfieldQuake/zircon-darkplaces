// prvm_dungeon_genmap.c.h

#include "csg_local.h"

// Grid is already filled and rows and cols are set.
// 256 256 ->block_size is set.

// Baker: September 13 2026 - the dead "dungeon" command calls this
qbool Generate_Dungeon_Map_File (map_compile_s *p)
{
	int isok = map_load_is_ok (&p->eout_map, p->maps_shell_dot_map);
	if (!isok) {
		Con_PrintLinef ("Unable to load clone %s", p->maps_shell_dot_map);
		goto exitor;
	}

	// Find the mat.  Get the size.
	// Load mat1
	// Load block1
	int eidx_mat1   = map_find_key_string (&p->eout_map, "targetname", "mat1");
	if (eidx_mat1 == not_found_neg1) {
		isok = false;
		Con_PrintLinef ("Error couldn't find the mat1");
		goto exitor;
	}
	//entityx_s *emat1  = EntForNum(p->eout_map, eidx_mat1);
	//entityx_s *emat1  =  ENTITYGET (p->eout_map);
	
	entityx_s *emat1  =  ENT_GET (&p->eout_map, eidx_mat1);
	
	map_set (&p->eout_map, eidx_mat1, "classname", "info_null");

	bounds_init_9 (p->mat_mins, p->mat_maxs);
	/*unused int did_any =*/ entity_expand_bounds_did_brushes(emat1, p->mat_mins, p->mat_maxs);

	// END OF SHELL OPEN

	// BEGIN PASTE BLOCKS
	// Where are we pasting?

	for (int row = 0; row < p->rows; row ++) {
		for (int col = 0; col < p->cols; col ++) {
			int idx = col + row * p->cols;
			int gridval = p->ints_grid.ints[idx];
			map_t eblock = {0};
			if (!gridval)
				continue; // 0 = empty

			// Open the file
			va_super (filename, MAX_QPATH_128, "%s", "maps/proto/#block1.map");
			int isok2 = map_load_is_ok (&eblock, filename);
			if (!isok2)
				goto exitor; // Failed to open file.

			int coord_row = (p->rows - 1) - row; // Flip the row so 0,0 // 20x13 // 20-

			// Requires map block has 0,0,0 based and is 256 x 256 size.
			int add_x = col * p->block_size[0];
			int add_y = coord_row * p->block_size[1];
			vec3_t translate_amount = { p->mat_mins[0] + add_x, p->mat_mins[1] + add_y, 0 };

			va_super (prefix_here, MAX_QPATH_128, "_block%d_", idx);

			csg_vm_prefix_targetnames_list	(&eblock, prefix_here, &p->listnamefields); // _block1_
			map_append_map_with_move	(&p->eout_map, &eblock, translate_amount);

			map_freecontents (&eblock);
		} // cole
	} // row

exitor:
	//map_freecontents	(&p->emap);
	map_save_as (&p->eout_map, p->rp_map_dot_map);

	return isok;
}

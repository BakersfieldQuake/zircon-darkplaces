// mazegen.c.h - NOW SERVER

#include "mazegen_procs.c.h"
#include "mazegen_pathfind_2.c.h"
#include "mazegen_mazegen1.c.h"

static mazegen_s _maze = {0}, *maze = &_maze;

// Feb 20 2026 - never written
//void SCR_maze_path_find_f (cmd_state_t *cmd)
//{
//}

qbool Maze_To_Map_Is_Ok (mazegen_s *m, ccs *maze_model_filename, ccs *filename, qbool is_overwriteok);


// maze_save filename
void SCR_maze_save_f (cmd_state_t *cmd)
{
	ccs *savefilename1 = cmd_argv_parm1;
	qfile_t *f = NULL;

	CMD_USAGE_NUM_ARGS_DESCRIBE_ (1, "<savefilename.txt>", "saves current random maze to .txt")

	CMD_FAILEXIT_IF_ (!maze->maze_za, "No maze active");

	f = FS_OpenRealFile (savefilename1, "wb", fs_quiet_FALSE);
	CMD_FAILEXIT_IF2_ (!f, "Couldn't open file for writing " QUOTED_S, savefilename1);

	FS_PrintLinef (f, "%d", maze->rows);
	FS_PrintLinef (f, "%d", maze->cols);

	for (int row = 0; row < maze->rows; row ++) {
		for (int col = 0; col < maze->cols; col ++) {
			int cell_idx = NCELL (maze, row, col);
			FS_Printf (f, "%c", maze->maze_za[cell_idx]);
        } // col
		FS_Print (f, NEWLINE);
	} // row

	FS_CloseNULL_ (f);

	Con_PrintLinef ("Wrote " QUOTED_S, savefilename1);

fail_exit:
	FS_CloseNULL_ (f);
}

// maze_clear .. clears all maze data
void SCR_maze_clear_f (cmd_state_t *cmd)
{
	_maze_free (&_maze);

	memset (&_maze, 0, sizeof(_maze));

	Con_PrintLinef ("Maze cleared");
}



// June 10 2025: BUSTED PATH SOLVE.
void SCR_maze_load_f (cmd_state_t *cmd) // "test5.txt"
{
//	int argc = cmd_argc;
	ccs *loadfilename1 = cmd_argv_parm1;

	CMD_USAGE_NUM_ARGS_DESCRIBE_ (1, "<loadfilename.txt>", "Loads current random maze from .txt")

	_maze_free (&_maze);

	maze->maze_za = edusporto_loadfile_za(maze, loadfilename1);

	CMD_FAILEXIT_IF2_ (!maze->maze_za, "Couldn't load maze txt file %s", loadfilename1)

	maze->startpos	= Point2D_Eduo_Find_Char(maze, maze->maze_za, MAZE_ENTRANCE_I);
	maze->endpos	= Point2D_Eduo_Find_Char(maze, maze->maze_za, MAZE_EXIT_O);

	print_maze (maze, maze->maze_za, CON_BRONZE);

	Con_PrintLinef ("Above is unaltered maze");

	// Copy the maze
	maze->path_za = (char *)Z_MemDup_Z (maze->maze_za, maze->sizeof_char_ray);

	// Because edu uses those to check if found exit.
	Con_PrintLinef (CON_CYAN "Entrance to maze: [R %d, C %d]", maze->startpos.row, maze->startpos.col);

	print_maze (maze, maze->path_za, CON_GREEN);

	Con_PrintLinef (CON_BRONZE "Above is maze before solve.");

	// PATH OPTIMIZATION
	while (/*ALLOW BREAK FAILURE*/ 1) {
		point2d_s solvepos2 = {-1,-1};
		point2d_s exit_to_maze = {-1,-1};

		size_t sizeofiternums = (maze->rows * maze->cols + ONE_CHAR_1) * sizeof(int);

		Mem_FreeNull_ (maze->iternums_za);
		maze->iternums_za = ZAlloc_Int32 (sizeofiternums);

		solve_recursive(maze, maze->path_za, maze->startpos, &solvepos2);

		if (maze->iters_exit) {
			Con_PrintLinef ("Hit iters limit %d... failed maze_max_iters = %d", maze->iters, maze_max_iters.integer);
			break;
		}

		if (Point2D_Are_Equal(maze->startpos, exit_to_maze)) {
			Con_PrintLinef (CON_RED "There is no answer to this maze. Iters = %d", maze->iters);
			break;
		}

		Con_PrintLinef (CON_GREEN "There is an answer to this maze.");
		print_maze (maze, maze->path_za, CON_BLUE);

		print_maze2 (maze, maze->path_za);

		Con_PrintLinef ("Optimizing path  ...");
		maze_iter_refine (maze);
		print_maze3 (maze, maze->path_za);

		Con_PrintLinef (CON_GREEN "There is an answer to this maze.");
		Con_PrintLinef (CON_GREEN "Exits at [R %d, C %d] - iters %d", solvepos2.row, solvepos2.col, maze->iters);

		break;
	} // block

fail_exit:
	;
}


// maze_to_map modelname filename
void SCR_maze_to_map_f (cmd_state_t *cmd)
{
	//maze_to_map maps/maze_model_hell.map maps/mymap.map overwrite
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (1, "<maps/model.map> <maps/filename.map> [overwrite]", "Writes current maze to .map using model.map as the sample room");

	ccs		*modelname1			= cmd_argv_parm1;
	ccs		*filename1			= cmd_argv_parm2;
	qbool	is_overwrite_mode	= String_Match(cmd_argv_parm3, "overwrite");

	va_super (filename,  MAX_OSPATH_EX_1024, "%s", filename1);
	va_super (modelname, MAX_OSPATH_EX_1024, "%s", modelname1);

	if (String_Ends_With (filename,  ".map") == false)	c_strlcat (filename,  ".map");
	if (String_Ends_With (modelname, ".map") == false)	c_strlcat (modelname, ".map");

	Con_PrintVarString	(modelname);
	Con_PrintVarString	(filename);
	Con_PrintVarInt		(is_overwrite_mode);

	// maps/maze_model_hell.map
	qbool isok = Maze_To_Map_Is_Ok (maze, modelname, filename, is_overwrite_mode);

	if (!isok) return; // An error message printed, just leave.

	Con_PrintLinef ("Wrote %s", filename);
}

// "maps/maze_model_hell.map";
qbool Maze_To_Map_Is_Ok (mazegen_s *m, ccs *maze_model_filename, ccs *filename, qbool is_overwriteok)
{
	qbool bret = false;
	double clock_start = Sys_DirtyTime ();

	map_t list_model = {0};
	map_t list_map1 = {0};

	CMD_FAILEXIT_IF_ (!m->maze_za, "No maze loaded")

	qbool bfilenameexists = FS_File_Exists(filename);

	CMD_FAILEXIT_IF2_ (bfilenameexists && is_overwriteok == false, "Filename %s already exists and allow overwrite is false", filename)

	{
		char *sin_model_za = FS_LoadFileQuick (maze_model_filename);
		CMD_FAILEXIT_IF2_ (!sin_model_za, "Couldn't load maze model %s", maze_model_filename)

		int isok = map_load_from_string (&list_model, sin_model_za);
		Mem_FreeNull_ (sin_model_za); // Done with this.

		CMD_FAILEXIT_IF2_ (!isok, "Couldn't parse maze model map %s", maze_model_filename)
	}

examine_maze_model:
    ;
	int north_wall_entnum = map_find_key_string_start_beyond(&list_model, "classname", "func_group_north_wall", AFTER_WORLD_1, list_model.count);
	int floor_wall_entnum = map_find_key_string_start_beyond(&list_model, "classname", "func_group_floor_wall", AFTER_WORLD_1, list_model.count);
	entityx_s *north = north_wall_entnum == not_found_neg1 ? NULL : &list_model.entity[north_wall_entnum];
	entityx_s *floor = floor_wall_entnum == not_found_neg1 ? NULL : &list_model.entity[floor_wall_entnum];

	float wall_height = north ? north->bedims[2] : 0;
	float tile_size = floor ? floor->bedims[0] : 0;

	CMD_FAILEXIT_IF_ (tile_size == 0,	"Tile size is 0 in the layout model room.")
	CMD_FAILEXIT_IF_ (wall_height == 0,	"Wall height is 0  in the layout model room.")

	Con_PrintVarInt (tile_size);
	Con_PrintVarInt (wall_height);

construct_world:
;
	// CONSTRUCT WORLD
	//entityx_s *d_ent_world = entitylist_add (&list_map1);
	map_set (&list_map1, WORLD_0, "classname", "worldspawn");
	map_set (&list_map1, WORLD_0, "_keeplights", "1");
	map_set (&list_map1, WORLD_0, "worldtype", "0");
	map_set (&list_map1, WORLD_0, "_blocksize", "2048 2048 2048");


	// ROWS - Are ascending, we need them descending
	// COLS - Are ascending
	vec3_t worldsize = {0}, worldmins = {0}, worldmaxs = {0};
	worldsize[0]	= m->rows * tile_size; // 32 * 256 = 8192
	worldsize[1]	= m->cols * tile_size; // 32 * 256 = 8192
	worldsize[2]	= wall_height; // 32 * 256 = 8192

	printvari	(m->rows);
	printvari	(m->cols);
	printvarv	(worldsize);

	worldmins[0]	= - (worldsize[0] / 2.0);
	worldmins[1]	= - (worldsize[1] / 2.0);
	worldmins[2]	= 0;

	worldmaxs[0]	=   (worldsize[0] / 2.0);
	worldmaxs[1]	=   (worldsize[1] / 2.0);
	worldmaxs[2]	= 0;

	printvarv	(worldmins);

	qbool is_solid_colmax2 = is_solid_column (m, m->cols - 2);
	qbool is_solid_rowmax2 = is_solid_row (m, m->rows - 2);
	qbool is_solid_colmin2 = is_solid_column (m, 2);
	qbool is_solid_rowmin2 = is_solid_row (m, 2);

	// Maze generator is stupid and can make extra solid walls
	int rowmin = 1;  if (is_solid_rowmin2) rowmin ++;
	int colmin = 1;  if (is_solid_colmin2) colmin ++;
	int rowmax = m->rows - 1;  if (is_solid_rowmax2) rowmax --;
	int colmax = m->cols - 1;  if (is_solid_colmax2) colmax --;

	printvari (rowmin);
	printvari (colmin);
	printvari (rowmax);
	printvari (colmax);

	vec3_t vcursor;
	vcursor[0] =  worldmins[0];
	vcursor[1] = -worldmins[1] - tile_size; // -4096 ==> 4096
	vcursor[2] = 0;

	qbool did_info_player_start = false;

	static int iters;

	for (int row = rowmin; row <= rowmax; row ++, vcursor[1] += (-tile_size)) {
		vcursor[0] =  worldmins[0];	// Reset
		for (int col = colmin; col < colmax; col ++, vcursor[0] += tile_size) {
			int cell_idx = NCELL (m, row, col);
			int ch = m->maze_za[cell_idx];

			int nsbits;
			// Stops before "I" and "O" just in case?
			if (in_range_beyond('A', ch, 'H')) {
				nsbits = nsew_floor_512 | nsew_ceil_256;
			} else {
				// Solid returns pure 0.
				nsbits = (int)walls_for_cell (m, m->maze_za, row, col, rowmin, rowmax, colmin, colmax);
			}
			//Con_PrintLinef ("RC %3d, %3d bits = %d", row, col, nsbits & 15);

			if (nsbits == nsew_solid_neg1)
				continue; // SOLID

			if (did_info_player_start == false) {
				entityx_s *xs = entitylist_add (&list_map1);
				vec3_t vorg;
				VectorCopyDestSrc (vorg, vcursor);
				vorg[0] += tile_size / 2.0;
				vorg[1] += tile_size / 2.0;
				vorg[2] = 64;
				entity_set (xs, "classname", "info_player_start");
				entity_set_vec3_g (xs, "origin", vorg);

				did_info_player_start = true;
			}

			ccs *direction_moves[] = {"n", "s", "e", "w", "c", "f"};
			int bit_moves[] = {
				nsew_north_1,
				nsew_south_2,
				nsew_east_4,
				nsew_west_8,
				nsew_ceil_256,
				nsew_floor_512,
				//nsew_bits_north_1 + nsew_bits_south_2 + nsew_bits_east_4 + nsew_bits_west_8, // alpha move
			};
			int num_moves = ARRAY_COUNT (bit_moves);
			for (int move_num = 0; move_num < num_moves; move_num ++) {
				int this_bit_move = bit_moves[move_num];
				ccs *direction_move = direction_moves[move_num];
loop_nonworld_entities:
				// Ok ... the nasty.  Have to add alpha brushes as func_group
				// Add to what though ... specific classnames.
				while (Have_Flag (nsbits, this_bit_move)) {
					int start_idx = AFTER_WORLD_1, beyond_idx = list_model.count;

					while (1) { // // Find all entities in model with 'n' - NORTH
						ccs *nsew_keyval = NULL; iters ++;
						int entnum = map_find_key_string_ANY_start_beyond (&list_model, "_nsew", start_idx, beyond_idx, &nsew_keyval);
						if (entnum == not_found_neg1) break; // NO MORE MATCHES
						start_idx = entnum + 1; // Move beyond

						entityx_s *me = &list_model.entity[entnum];
						//ccs *_nsew = entity_get (me, "_nsew");

						if (!String_Contains (nsew_keyval, direction_move))
							continue;

						//ccs *s_chance = entity_get (me, "_chance");
						ccs *s_chance = map_get (&list_model, /*me*/ entnum, "_chance");

						if (s_chance) {
							float chance = atof(s_chance);
							float mynum = lhrandom (0, 1);
							if (mynum > chance) continue; // Failed dice roll.
						}

						//ccs *classname = entity_get (me, "classname");

						// We have an entity.
						entityx_s *xn = entitylist_add_pasting_this_ent (&list_map1, me);
						
// No include so dup ...
#define ENT_GET_NUM(HM, PENT) \
	ARRAY_INDEX(PENT, (HM)->entity); // DUP but whatever ... move along

						int entnum_new = ENT_GET_NUM (&list_map1, xn);
						map_entity_translate_with_metric (&list_map1, entnum_new, vcursor);

						entity_set_fmt (xn, "_rowcol", "RC_%d_%d", row, col);
						
						ccs *_alphawanted = map_get (&list_model, /*me*/ entnum, "_alphawanted");
						if (_alphawanted) {
							// Add alpha brushes to func group at all 8 points.  Too many is fine for now.
							// Calc the alpha how?
							// do all 8 corners adding a brush with appropriate alpha texture
							// redo the common/alpha shaders with a better naming convention.
							// bbmin range to bbmax pct for each corner.
							entity_add_alpha_brushes (xn, worldmins, worldmaxs, worldsize);
						} // Specific classnames

						//ccs *_lightpattern = entity_get (me, "_lightpattern");
						ccs *_lightpattern = map_get (&list_model, /*me*/ entnum, "_lightpattern");
						while (_lightpattern) {
							//entityx_s *w_ent = &list_model.entity[ENTITY_WORLD_0];
							//GETL_WORLD_0 (list_model, w_ent);
							// Override color based on origin.
							int ival = atoi (_lightpattern);
							// Find the lightpattern
							va_super (lightpatternfind, 128, "_lightpattern%d", ival);
							//ccs *wlightpatternval = entity_get (w_ent, lightpatternfind);
							ccs *wlightpatternval = map_get (&list_model, WORLD_0, "lightpatternfind");
							//entity_set_vec3_g (,"_color", lightvector);
							//"_lightpattern" 1 4000 distfrom 0 0 0 0.97 0.21 0.07 0.1 0.8 0.5
							if (!wlightpatternval) {
								Con_PrintLinef ("Cannot locate %s in world", lightpatternfind);
								break;
							}

							// world:
							// "_lightpattern1"
							// Radius where we hit 0.
							//
							//  0     1       2 3 4 5    6    7     8   9  10
							//  4000 distfrom 0 0 0 0.97 0.21 0.07 0.1 0.8 0.5
							stringlist_t list = {0};
							stringlist_append_split (&list, wlightpatternval, SPACE);
							if (list.numstrings != 11) {
								Con_PrintLinef ("World %s does not have 11 args in value", lightpatternfind);
								stringlistfreecontents (&list);
								break;
							}
							float radius = atof(list.strings[0]);
							if (radius <= 0) {
								Con_PrintLinef ("World %s has invalid radius of %g", lightpatternfind, radius);
								stringlistfreecontents (&list);
								break;
							}
							vec3_t vcenter = { atof(list.strings[2]), atof(list.strings[3]), atof(list.strings[4]) };
							vec3_t color0 = { atof(list.strings[5]), atof(list.strings[6]), atof(list.strings[7]) };
							vec3_t color1 = { atof(list.strings[8]), atof(list.strings[9]), atof(list.strings[10]) };
							stringlistfreecontents (&list);

							vec3_t eorg = {0};
							qbool isfound = entity_get_vec3 (xn, "origin", eorg);
							if (!isfound) {
								Con_PrintLinef ("World %s has invalid radius of %g", lightpatternfind, radius);
								break;
							}

							float dist = VectorDistance (eorg, vcenter);
							float pct = dist / radius;
							pct = bound (0, pct, 1);
							vec3_t colorblended;
							colorblended[0] = color0[0] * pct + color1[0] * (1.0-pct);
							colorblended[1] = color0[1] * pct + color1[1] * (1.0-pct);
							colorblended[2] = color0[2] * pct + color1[2] * (1.0-pct);
							entity_set_vec3_g (xn, "_color", colorblended);

							break;
						} // lightpattern while if


					} // while finding north entities.
					break;
				} // north while if
			} // each direction


		} // col

	} // r

	double clock_map_done = Sys_DirtyTime ();

	Con_PrintLinef ("Elapsed: %g", clock_map_done - clock_start);

	{
		baker_string_s *bsout = map_save_to_string_bsalloc (&list_map1);
		ccs *s_size = String_Num_To_Thousands_Sbuf (bsout->length);
		Con_PrintLinef (".Map file bytes = %s", s_size);
		ccs *s_size_mb = String_Num_To_Thousands_Sbuf (bsout->length / (1024 * 1024));
		Con_PrintLinef (".Map file = %s MB", s_size_mb);

		CMD_FAILEXIT_IF_ (!bsout, "Couldn't export entitylist to text") // Can't happen, right?

		// RE-WRITE
		int isok = FS_WriteFileStringQuick (filename, bsout->string);
		BakerString_Destroy_And_Null_It (&bsout);
		CMD_FAILEXIT_IF2_ (!isok, "Failed to write %s", filename) // Can't happen, right?
	}

	double clock_write_done = Sys_DirtyTime ();

	Con_PrintLinef ("Elapsed: %g", clock_write_done - clock_start);

	bret = true; // SUCCESS

fail_exit:
	map_freecontents	(&list_map1);
	map_freecontents	(&list_model);
	return bret;
}







void SCR_mazegen_f (cmd_state_t *cmd) // Seems to work
{
	mazegen_name_rows_cols_blots_blotmin_blotmax  (maze, maze_rows.integer, maze_columns.integer,
		maze_blots.integer /*d:3*/, 5, 7, 5, 7);

	Con_PrintLinef (CON_GREEN "There is an answer to this maze.");

	print_maze (maze, maze->maze_za, CON_BRONZE);
	Con_PrintLinef (CON_CYAN "^^^^ Above: maze_za");
	print_maze (maze, maze->path_za, CON_BRONZE);
	Con_PrintLinef (CON_CYAN "^^^^ Above: path_za RAW");
	print_maze2 (maze, maze->path_za);
	Con_PrintLinef (CON_CYAN "^^^^ Above: path_za amalgam");

	Con_PrintLinef ("Improving solution  ...");
	maze_iter_refine (maze); // Creates lists.
	print_maze3 (maze, maze->path_za);
#if 1
	stringlist_condump_with_linenums (&maze->listsolvesteps);
#endif

	Con_PrintLinef ("Start at %d %d", maze->startpos.col, maze->startpos.row);
	Con_PrintLinef ("End at %d %d", maze->endpos.col, maze->endpos.row);

	Con_PrintLinef (CON_GREEN "There is an answer to this maze.");
	Con_PrintLinef (CON_GREEN "Exits at [R %d, C %d] - iters %d", maze->endpos.row, maze->endpos.col, maze->iters);
}


#include "mazegen_halls.c.h"
void Parse_Blocks_List_From_Text (stringlist_t *plist_blocks, ccs *s_mymap_txt_blob, int *pnumrows, int *pnumcolumns);
void Parse_Blocks_List_From_Text_To_Png (stringlist_t *plist_blockspng, ccs *s_mymap_txt_blob, int *pnumrows, int *pnumcolumns)
{
	Parse_Blocks_List_From_Text (plist_blockspng, s_mymap_txt_blob, pnumrows, pnumcolumns);
	// Now remove the extensions
	for_each_stringlista_string(n, plist_blockspng, dot_map)
		va_copy_16384 (extension_removed, dot_map);
		File_URL_Edit_Remove_Extension (extension_removed);
		stringlistreplaceindex (plist_blockspng, n, extension_removed);
	for_each_end
}

void Parse_Blocks_List_From_Text (stringlist_t *plist_blocks, ccs *s_mymap_txt_blob, int *pnumrows, int *pnumcolumns)
{
	stringlist_t list_lines = {0};
	stringlistappend_split_lines_cr_scrub (&list_lines, s_mymap_txt_blob);
	//stringlist_condump_with_linenums (&list_lines);

	for_each_stringlist_string(linenum,list_lines,sxy)
		va_trim_16384 (sline, sxy);
		if (String_Starts_With (sline, "//"))
			continue; // comment
		// row 0 col 7 maps/dungeon_maker/blocks/room.map
		stringlist_t words = {0};
		stringlist_append_split (&words, sline, SPACE);
		if (!words.numstrings) {
			// Does this happen?  Do we really need to free?
			stringlistfreecontents (&words);
			continue;
		}
		//
		// PROCESS LINE
		//
		ccs *word0 = WORDNUM(words, 0);
		ccs *word1 = WORDNUM(words, 1);
		//ccs *word2 = WORDNUM(words, 2); // Technically not used
		ccs *word3 = WORDNUM(words, 3);
		ccs *word4 = WORDNUM(words, 4);

		if (String_Match (word0, "rows")) {
			if (word1) {
				// We expect this 2nd
				(*pnumrows) = atoi(word1);
				if ((*pnumrows) > 0 && (*pnumcolumns) > 0 && plist_blocks->numstrings == 0) {
					// Create X number of blanks
					int cells = (*pnumrows) * (*pnumcolumns);
					for (int n = 0; n < cells; n ++) {
						stringlistappend (plist_blocks, "");
					}
					// FILL COMPLETE
				}
			}
			else Con_PrintLinef ("rows without count at line %d", linenum);
		} else if (String_Match (word0, "cols")) {
			if (word1) {
				(*pnumcolumns) = atoi(word1);
			}
			else Con_PrintLinef ("cols without count at line %d", linenum);
		} else if (String_Match (word0, "row")) {
			// row 0 col 7 maps/dungeon_maker/blocks/room.map
			if (words.numstrings != 5)
				Con_PrintLinef ("Count != 5 at line %d", linenum);
			else if ((*pnumrows) == 0 || (*pnumcolumns) == 0) {
				Con_PrintLinef ("Data without numrows and numcolumns set at line %d", linenum);
			}
			else {
				int row			= atoi(word1);
				int col			= atoi(word3);
				ccs *blockname  = word4;
				int cell_idx = row * (*pnumcolumns) + col;
				if (in_range_beyond (0, cell_idx, plist_blocks->numstrings)) {
					stringlistreplaceindex (plist_blocks, cell_idx, blockname);
				}
			}
		} else {
			Con_PrintLinef ("Unexpected %s at line %d", word0, linenum);
		}

		stringlistfreecontents (&words);
	for_each_end
	stringlistfreecontents (&list_lines);
	//stringlist_condump_with_linenums (plist_blocks);
}

#include "mazegen_halls_make4.c.h"

// mazegen3 takes data/mymap.txt ... it does not create the halls

RELATED_ (SCR_mazegen2_f Halls_Make QuakeC HALL_ROOM_MAPS_FILENAME_MAP QuakeC Halls_Save_Mymap_Txt VM_maze_generate)
void SCR_mazegen3_f (cmd_state_t *cmd)
{
	// NOTE: This is NO SHELL, but with floor and ceiling.  The ceiling texture is also the floor texture.
	ccs *floortex = "texturecan/tiles_0014";
	ccs *filename = "data/mymap.txt";
	int numrows = 0;
	int numcolumns = 0;
	vec3_t blocksize = {512,512,320};
	vec3_t vcentral = {0,0,0};

	stringlist_t list_blocks = {0};

	char *s_za = FS_LoadFileQuick(filename); // data/mymap.txt
	if (!s_za) {
		Con_PrintLinef (CON_RED "File read from %s failed", filename);
		return;
	}
	RELATED_ (SCR_Screenshot_Get_JPEG_BGRA4_From_Save_File_ZAlloc SV_Savegame_to)

	Parse_Blocks_List_From_Text (&list_blocks, s_za, &numrows, &numcolumns);

#ifdef _DEBUG
	int ret = /*used*/
#endif // _DEBUG
        write_map_from_blocks ("maps/mymap2.map", s_za, &list_blocks, numrows, numcolumns, blocksize, vcentral, /*flags*/ 0, floortex);

	stringlistfreecontents (&list_blocks);
	Mem_FreeNull_ (s_za); // DEL
}

void SCR_mazegen2_f (cmd_state_t *cmd) // Seems to work
{
#if 0
#pragma message ("This must be removed")
	int seed = atoi(cmd_argv_parm1);
	srand(seed);
#endif
	if (cmd_argc > 4) {
		Con_PrintLinef ("usage: mazegen2 <columns> <rows> <numrooms>");
		return;
	}

	int numcolumns = atoi(cmd_argv_parm1); // 2
	int numrows = atoi(cmd_argv_parm2); // 3
	int numrooms = atoi(cmd_argv_parm3); // 4

	if (numcolumns == 0) numcolumns = 25;
	if (numrows == 0) numrows = 16;
	if (numrooms == 0) numrooms = 26;


	// QuakeC does not expect s_a, but rather a stringbuffer of the rooms.  Like #
	stringlist_t list_room_coords = {0};

	RELATED_ (VM_maze_generate)
	char *s_a = Halls_Generate_Zalloc (numcolumns, numrows, numrooms, &list_room_coords, /*rooms 400*/ NULL); // Maze string

	if (s_a == NULL) {
		Con_PrintLinef ("mazegen2: Failed.  Likely due to high room density");
		goto exitor;
	}

	Con_PrintLinef ("mazegen2: printing rooms");
	for (int sidx = 0; sidx < list_room_coords.numstrings; sidx ++) {
		char *sxy		= list_room_coords.strings[sidx];
		//int *pconnect1	= &ilist_room_numconnect.ints[sidx];
		//Con_PrintLinef ("%c %4d: " S_FMT_LEFT_PAD_20 " connects = %d", ASCII_48_ZERO +sidx,sidx, sxy, (*pconnect1));
		Con_PrintLinef ("%c %4d: " S_FMT_LEFT_PAD_20 , HALL_ROOM_CHAR_48_ZERO + sidx, sidx, sxy);
	}

	Con_PrintLinef ("mazegen2: printing maze");
//	int slen = strlen(s_a);
	for (int row = 0; row < numrows; row ++) {
		for (int col = 0; col < numcolumns; col ++) {
			int cell_idx = row * numcolumns + col;
			char sch = s_a[cell_idx];
			//char s[2] = { sch, CHAR_NULL_0 };
			Con_Printf ("%c", sch);
		} // col
		Con_PrintLineEmpty ();
	} // row

exitor:

	Mem_FreeNull_ (s_a);
	stringlistfreecontents  (&list_room_coords);
	//int32s_freecontents		(&ilist_room_numconnect);
}

RELATED_ (VM_maze_generate mazegen_s)

void halls_generate (stringlist_t *plist_maze, stringlist_t *plist_room, int cols, int rows, int numrooms)
{
}

// Sometimes a row is thick.  Sometimes it is not.
// Sometimes a col is thick.  Sometimes it is not.
// +2 and know this algo might thick up a row.
// We will intercept and turn the thick row or column to at symbols @@@.
// Check the end and check the start.
void maze_generate (stringlist_t *plist_maze, int cols, int rows, stringlist_t *plist_solve, vec3_t startout, vec3_t endout, int numblots)
{
	// There is an outer shell wall always.  Sometimes a lead/trail row and/or col is double thick.
	mazegen_name_rows_cols_blots_blotmin_blotmax  (maze, rows, cols, numblots, 5, 7, 5, 7);

	for (int row = 0; row < maze->rows; row ++) {
		//	Con_Printf (CON_BRONZE);
		for (int col = 0; col < maze->cols; col ++) {
			int cell_idx = row * maze->cols + col;
			int ch = maze->path_za[cell_idx];
			stringlistappendf (plist_maze, "%c", ch);
		}
	}

	maze_iter_refine (maze); // Creates lists.
	stringlistappend_list (plist_solve, &maze->listsolvesteps);

	startout[0] = maze->startpos.col;
	startout[1] = maze->startpos.row;
	endout[0] = maze->endpos.col;
	endout[1] = maze->endpos.row;
}

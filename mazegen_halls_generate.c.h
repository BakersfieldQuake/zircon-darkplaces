// mazegen_halls_generate.c.h

int force_get_row_string_zero_bounds(ccs *rooms_string, int row, int col, int numrows, int numcolumns)
{
	if (false == in_range_beyond (0, row, numrows))
		return 0; //HALL_CH_EMPTY_SPACE_32;
	if (false == in_range_beyond (0, col, numcolumns))
		return 0; //HALL_CH_EMPTY_SPACE_32;

	int		cell_idx	= row * numcolumns + col;
	ccs		*sch_look	= &rooms_string[cell_idx];
	int		ch			= *sch_look; //string_ascii(sch_look);
	return ch;
}

static void con_print_string_cells(ccs *rooms_string, int numrows, int numcolumns)
{
	for (int row = 0; row < numrows; row ++) {
		for (int col = 0; col < numcolumns; col ++) {
			int cell_idx = row * numcolumns + col;
			char sch = rooms_string[cell_idx];
			Con_Printf ("%c", sch);
		} // col
		Con_PrintLineEmpty ();
	} // row
}

char *Halls_Generate_NULL_Fail_ZA (int numcolumns, int numrows, int numrooms, stringlist_t *plist_supplied_room_coords, char *room_bytes_optional_400)
{
	int num_fails = 0;
	int numcells = numcolumns * numrows;
	char *rooms_string_zout = Z_StrRepeat(CHAR_SPACE_32, numcells);

	stringlist_t	list_room_coords = {0};			// "%d %d", col, row
	int32list_s		ilist_room_numconnect = {0};

	// Make X fixed rooms
	int numfixedrooms = plist_supplied_room_coords->numstrings;
	for (int n = 0; n < numfixedrooms; n ++) {
		ccs *sxy = plist_supplied_room_coords->strings[n];
		vec3_t vroomcoords = {0};
		Math_atov2 (sxy, vroomcoords);
		int col = vroomcoords[0];
		int row = vroomcoords[1];
		int cell_idx = row * numcolumns + col;

		// OCCUPY and write coordinates in col, row format.
		rooms_string_zout[cell_idx] =  HALL_ROOM_CHAR_48_ZERO + n; // 'O';
		stringlistappendf	(&list_room_coords, "%d %d", col, row);
		int32s_add1			(&ilist_room_numconnect, 0);
	} // for

	// Make 20 random rooms
	int attempts = 0;
	for (int n = numfixedrooms; n < numrooms; n ++) {
		while (1) {
			if (attempts > 50) {
				return NULL;
			}
			attempts ++;

			int col = lhrandom(0, numcolumns);
			int row = lhrandom(0, numrows);

			int ch = force_get_row_string_zero_bounds(rooms_string_zout, row, col, numrows, numcolumns);
			if (CH_IS_ROOM(ch))
				continue; // ALREADY TAKEN

			int ch_left		= force_get_row_string_zero_bounds(rooms_string_zout, row, col - 1, numrows, numcolumns);
			int ch_right	= force_get_row_string_zero_bounds(rooms_string_zout, row, col + 1, numrows, numcolumns);
			int ch_up		= force_get_row_string_zero_bounds(rooms_string_zout, row - 1, col, numrows, numcolumns);
			int ch_down		= force_get_row_string_zero_bounds(rooms_string_zout, row + 1, col, numrows, numcolumns);

			if (CH_IS_ROOM(ch_left) || CH_IS_ROOM(ch_right) || CH_IS_ROOM(ch_up) || CH_IS_ROOM(ch_down))
				continue; // An adjacent cell has a room .. try again ...

			// OCCUPY and write coordinates in col, row format.
			int		cell_idx	= row * numcolumns + col;
			rooms_string_zout[cell_idx] =  HALL_ROOM_CHAR_48_ZERO + n;// 'O';
			stringlistappendf	(&list_room_coords, "%d %d", col, row); // HEAPCORRUPTION HERE?
			int32s_add1			(&ilist_room_numconnect, 0);
			break;
		} // while not adjacent
	} // numrooms

	//
	// CONNECT COMMON COLUMN ROOMS WITH PIPES
	//
	for (int sidx = 0; sidx < list_room_coords.numstrings; sidx ++) {
		char *sxy = list_room_coords.strings[sidx];
		vec3_t vroom;
		Math_atov2 (sxy, vroom);

		int col = vroom[0], row = vroom[1];

		for (int sidx2 = 0; sidx2 < list_room_coords.numstrings; sidx2 ++) {
			char *sxy2 = list_room_coords.strings[sidx2];
			vec3_t vroom2;
			Math_atov2 (sxy2, vroom2);
			int r2col = vroom2[0], r2row = vroom2[1];

			// We want same column rooms
			// They can't be different columns
			if (r2col != col)
				continue;

			// They can't be same row number
			if (r2row == row)
				continue;

			// Let's only do ones where greater
			if (r2row < row)
				continue;

			// PIPE CONNECT THESE 2 ROOMS NORTH SOUTH
			int hallcol = col;
			for (int hallrow = row + 1; hallrow < r2row; hallrow ++) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];

				if (s_hall[0] != CHAR_SPACE_32)
					continue; // Already done

				s_hall[0] = HALL_NORTH_SOUTH_PIPE_CHAR; // '|'
			}

			int *pconnect1 = &ilist_room_numconnect.ints[sidx];   (*pconnect1) ++;
			int *pconnect2 = &ilist_room_numconnect.ints[sidx2];  (*pconnect2) ++;
		} // r2

		//
		// CONNECT COMMON ROW ROOMS WITH EAST WEST HALLS "-"
		//
		for (int sidx2 = 0; sidx2 < list_room_coords.numstrings; sidx2 ++) {
			char *sxy2 = list_room_coords.strings[sidx2];
			vec3_t vroom2;
			Math_atov2 (sxy2, vroom2);
			int r2col = vroom2[0];
			int r2row = vroom2[1];

			// We want same rows rooms
			// They can't be different rows
			if (r2row != row)
				continue;

			// They can't be same col number
			if (r2col == col)
				continue;

			// Let's only do ones where greater
			if (r2col < col)
				continue;

			// CONNECT THEM.
			int hallrow = row;
			for (int hallcol = col + 1; hallcol < r2col; hallcol ++) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];
				if (s_hall[0] != CHAR_SPACE_32)
					continue; // Already done

				s_hall[0] = HALL_EAST_WEST_MINUS_CHAR; // '-';
			}

			int *pconnect1 = &ilist_room_numconnect.ints[sidx];   (*pconnect1) ++;
			int *pconnect2 = &ilist_room_numconnect.ints[sidx2];  (*pconnect2) ++;
		} // r2
	} // sidx

	//
	// UNCONNECTED ROOMS - DRILL A HALL TOWARDS THE CENTER AND SEE IF IT CONNECTS
	// IF IT CONNECTS THEN LAY DOWN THE HALL.
	// If everything fails, it will flag in unsolvable.
	for (int sidx = 0; sidx < list_room_coords.numstrings; sidx ++) {
		char *sxy = list_room_coords.strings[sidx];
		int *pconnect1 = &ilist_room_numconnect.ints[sidx];
		if ( (*pconnect1) > 0)
			continue; // ALREADY CONNECTED.

		vec3_t vroom;
		Math_atov2 (sxy, vroom);
		int col = vroom[0];
		int row = vroom[1];

		int is_horz = lhrandom(0,50) > 25;

		qbool is_to_nowhere_row = false;
		qbool is_to_nowhere_col = false;
		qbool is_positive_col = (col <= (numcolumns/2));
		qbool is_positive_row = (row <= (numrows/2));
		if (is_horz) {
			// Make a column
force_horz:
			// PROBLEM ... HALL TO NOWHERE
            ; // oblig
			int col_move	= is_positive_col ? 1 : -1;
			int col_start	= col + col_move;
			int col_beyond	= is_positive_col ? (numcolumns - 1): 0;

			int hallrow = row;

			// TEST CONNECTION
			is_to_nowhere_col = true;
			for (int hallcol = col_start; is_positive_col ? hallcol < col_beyond : hallcol >=0; hallcol += col_move) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];
				if (s_hall[0] != CHAR_SPACE_32) {
					is_to_nowhere_col = false;
					break; // OCCUPIED
				}
			}
			if (is_to_nowhere_col) {
				// Con_PrintLinef ("Room #%d %c at %d %d cannot be connected with left/right hall", sidx, ASCII_48_ZERO + sidx, col, row);

				if (is_to_nowhere_row)
					goto cannot_connect;
				goto force_vert;
			}

			// CONNECT THEM.
			for (int hallcol = col_start; is_positive_col ? hallcol < col_beyond : hallcol >=0; hallcol += col_move) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];
				if (s_hall[0] != CHAR_SPACE_32)
					break; // Already done

				s_hall[0] = HALL_EAST_WEST_MINUS_CHAR; // '-';
			}
			int *pconnect2 = &ilist_room_numconnect.ints[sidx];   (*pconnect2) ++;
		} else {
			// Make a row
			// Make a column
force_vert:
            ; // oblig
			int row_move	= is_positive_row ? 1 : -1;
			int row_start	= row + row_move;
			int row_beyond	= is_positive_row ? (numrows - 1): 0;
			int hallcol = col;

			// TEST CONNECTION 2
			is_to_nowhere_row = true;
			for (int hallrow = row_start; is_positive_row ? hallrow < row_beyond : hallrow >=0; hallrow += row_move) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];
				if (s_hall[0] != CHAR_SPACE_32) {
					is_to_nowhere_row = false;
					break; // OCCUPIED
				}
			}
			if (is_to_nowhere_row) {
				Con_PrintLinef ("Room #%d %c at %d %d cannot be connected with up/down hall", sidx, HALL_ROOM_CHAR_48_ZERO + sidx, col, row);
				if (is_to_nowhere_col)
					goto cannot_connect;
				goto force_horz;
			}

			// CONNECT THEM.
			for (int hallrow = row_start; is_positive_row ? hallrow < row_beyond : hallrow >=0; hallrow += row_move) {
				int hall_cell_idx = hallrow * numcolumns + hallcol;
				char *s_hall = &rooms_string_zout[hall_cell_idx];
				if (s_hall[0] != CHAR_SPACE_32)
					break; // Already done

				s_hall[0] = HALL_NORTH_SOUTH_PIPE_CHAR; // '|';
			}
			int *pconnect3 = &ilist_room_numconnect.ints[sidx];   (*pconnect3) ++;
		}
cannot_connect:
		if (is_to_nowhere_col && is_to_nowhere_row) {
			// Con_PrintLinef ("Room #%d %c at %d %d cannot be connected posrow? %d poscol %d", sidx, ASCII_48_ZERO + sidx, col, row, (int)is_positive_row, (int)is_positive_col);
		}
	} // r

// FIND ANY IMPOSSIBLE ROUTES

// March 23 2026 - NEW PLAN
// 1. Upgrade strings with T junctions.
// 2. Make a triple sized one carve it out.
	RELATED_ (Baker_Flood_Fill)

	char *s_fillup_za = Z_StrDup(rooms_string_zout); // ENABLES FLOOD FILL

	//
	// MAKE A SPACES AND POUNDS REPRESENTATION FOR FLOOD FILL TEST
	for (int row = 0; row < numrows; row ++) {
		for (int col = 0; col < numcolumns; col ++) {
			int cell_idx = row * numcolumns + col;
			char *ps = &s_fillup_za[cell_idx];
			if (ps[0] != CHAR_SPACE_32)
				ps[0] = CHAR_SPACE_32;
			else
				ps[0] = MAZE_SOLID_POUND_CHAR;
		} // col
	} // row

	// con_print_string_cells (s_fillup_za, numrows, numcolumns);

solve_each_room:
    ; // oblig

// FIND UNCONNECTED ROOMS WITH FLOODFILL
// IF FAIL.  JUST DISCARD THE MAZE.
// Failure happens about 1 in 3 times.

	for (int sidx = 0; sidx < list_room_coords.numstrings; sidx ++) {
		char *sxy = list_room_coords.strings[sidx];
		vec3_t vroom;
		Math_atov2 (sxy, vroom);
		// Now what?
		int col = vroom[0];
		int row = vroom[1];

		char *s_fillup2_za = Z_StrDup(s_fillup_za);
		Baker_Flood_Fill ('X', ' ', col, row, s_fillup2_za, numcolumns, numrows);

#if 0 // April 11 2026
		// PRINT IT
		con_print_string_cells (s_fillup2_za, numrows, numcolumns);
#endif

		for (int sidx2 = 0; sidx2 < list_room_coords.numstrings; sidx2 ++) {
			if (sidx2 == sidx)
				continue;
			char *sxy2 = list_room_coords.strings[sidx2];

			vec3_t vroom2;
			Math_atov2 (sxy2, vroom2);
			int r2col = vroom2[0];
			int r2row = vroom2[1];
			int r2cell_idx = r2row * numcolumns + r2col;
			qbool is_answer = s_fillup2_za[r2cell_idx] == 'X';
			if (is_answer == false) {
				num_fails ++;
#if 0 // No print
				Con_PrintLinef ("Room %d %c cannot find Room2 %d %c", sidx, ASCII_48_ZERO + sidx, sidx2, ASCII_48_ZERO + sidx2);
#endif
			}
		} // r2
		Mem_FreeNull_ (s_fillup2_za);
	} // r
// END ROUTES

	//for (int sidx = 0; sidx < list_room_coords.numstrings; sidx ++) {
	//	char *sxy		= list_room_coords.strings[sidx];
	//	int *pconnect1	= &ilist_room_numconnect.ints[sidx];
	//	Con_PrintLinef ("%c %4d: " S_FMT_LEFT_PAD_20 " connects = %d", HALL_ROOM_CHAR_48_ZERO + sidx, sidx, sxy, (*pconnect1));
	//}
	//con_print_string_cells (rooms_string_zout, numrows, numcolumns);
	//Con_PrintLinef ("Num fails = %d", num_fails);

	Mem_FreeNull_ (s_fillup_za);

	int32s_freecontents		(&ilist_room_numconnect);

	if (num_fails) {
		stringlistfreecontents  (&list_room_coords);
		Mem_FreeNull_ (rooms_string_zout);
		return NULL;
	}

	// SUCCESS!  APPEND THE ROOMS
	for (int idx = numfixedrooms; idx < list_room_coords.numstrings; idx ++) {
		char *sxy = list_room_coords.strings[idx];
		stringlistappend (plist_supplied_room_coords, sxy);
	} // for

	stringlistfreecontents  (&list_room_coords);

	// FINAL PREP - T JUNCTIONS

	char *rooms_string_t_zout = Z_StrDup (rooms_string_zout);

	for (int row = 0; row < numrows; row ++) {
		for (int col = 0; col < numcolumns; col ++) {
			int		cell_idx	= row * numcolumns + col;
			ccs		*sch_look	= &rooms_string_zout[cell_idx];
			int		ch			= *sch_look;

			// HALL ONLY
			//if (CH_IS_NSEW_HALL(ch) == false)
			//	continue; // NOT A HALL

			int bits_halls = nsew_none_0; // Is next to a north south
			switch (ch) {
			case 0:
				continue; // Shouldn't happen
			case HALL_EMPTY_SPACE_CELL_32:
				if (room_bytes_optional_400) {
					//char ch_out = 32;// + NSEW_EX_ROOM_16 + room_halls;
					room_bytes_optional_400[cell_idx] = HALL_EMPTY_SPACE_CELL_32; // ENGINE STRING because byte.  Primary composed of control characters and null.
					RELATED_ (SCR_mazegen4_f)
				}
				continue; // Totally empty
			default:


				if (room_bytes_optional_400) {
					// We need to know the valid exits.
					#define NSEW_EX_ROOM_16	16
					int ch_look;
					int room_halls = nsew_none_0; //nsew_none_0

					ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row - 1, col + 0, numrows, numcolumns);
					if (ch_look && ch_look != HALL_EMPTY_SPACE_CELL_32)
						room_halls += nsew_north_1; // There needs to be a passage north added
					ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 1, col + 0, numrows, numcolumns);
					if (ch_look && ch_look != HALL_EMPTY_SPACE_CELL_32)
						room_halls += nsew_south_2; // There needs to be a passage south added
					ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 0, col - 1, numrows, numcolumns);
					if (ch_look && ch_look != HALL_EMPTY_SPACE_CELL_32)
						room_halls += nsew_west_8; // There needs to be a passage west added
					ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 0, col + 1, numrows, numcolumns);
					if (ch_look && ch_look != HALL_EMPTY_SPACE_CELL_32)
						room_halls += nsew_east_4; // There needs to be a passage east added.
					char ch_out = 32 + NSEW_EX_ROOM_16 + room_halls;
					room_bytes_optional_400[cell_idx] = ch_out; // ENGINE STRING because byte.  Primary composed of control characters and null.
					RELATED_ (SCR_mazegen4_f)
				}
				continue; // NOT A HALL ....

			case_break HALL_NORTH_SOUTH_PIPE_CHAR:	bits_halls = nsew_ns_3;
			case_break HALL_EAST_WEST_MINUS_CHAR:	bits_halls = nsew_ew_12;
			}

			int ch_look;

			ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row - 1, col + 0, numrows, numcolumns);
			if_3 (ch_look > CHAR_SPACE_32, ch == HALL_CH_EW_MINUS, ch_look != HALL_CH_EW_MINUS) {
				bits_halls += nsew_north_1; // There needs to be a passage north added
			}

			ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 1, col + 0, numrows, numcolumns);
			if_3 (ch_look > CHAR_SPACE_32, ch == HALL_CH_EW_MINUS, ch_look != HALL_CH_EW_MINUS) {
				bits_halls += nsew_south_2; // There needs to be a passage south added
			}

			ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 0, col - 1, numrows, numcolumns);
			if_3 (ch_look > CHAR_SPACE_32, ch == HALL_CH_NS_PIPE, ch_look != HALL_CH_NS_PIPE)  {
				bits_halls += nsew_west_8; // There needs to be a passage west added
			}

			ch_look = force_get_row_string_zero_bounds (rooms_string_zout, row + 0, col + 1, numrows, numcolumns);
			if_3 (ch_look > CHAR_SPACE_32, ch == HALL_CH_NS_PIPE, ch_look != HALL_CH_NS_PIPE) {
				bits_halls += nsew_east_4; // There needs to be a passage east added.
			}

			int ch_write = 0;
			switch (bits_halls) {
			default:						//zprf ("Unxpected bits_halls %d", bits_halls);
											//ch_write = HALL_NORTH_SOUTH_MAPS_FILENAME_MAP; // Give it something.

			case_break nsew_ns_3:			ch_write = HALL_NORTH_SOUTH_PIPE_CHAR;
			case_break nsew_ew_12:			ch_write = HALL_EAST_WEST_MINUS_CHAR;
			case_break nsew_no_north_14:	ch_write = HALL_CH_NO_NORTH_DOLLAR;
			case_break nsew_no_south_13:	ch_write = HALL_CH_NO_SOUTH_PERCENT;
			case_break nsew_no_east_11:		ch_write = HALL_CH_NO_EAST_LPAREN;
			case_break nsew_no_west_7:		ch_write = HALL_CH_NO_WEST_RPAREN;
			case_break nsew_all_15:			ch_write = HALL_CH_INTER_PLUS;
			}

			if (ch_write) {
				// OVERRIDE
				char *sch_t = &rooms_string_t_zout[cell_idx];
				(*sch_t) = ch_write;
			}

			if (room_bytes_optional_400) {
				if (bits_halls == 0) {
					// Does this happen?
				}

				room_bytes_optional_400[cell_idx] = 32 + bits_halls; // ENGINE STRING because byte.  Primary composed of control characters and null.
			}

		} // col
	} // row

	Mem_FreeNull_ (rooms_string_zout);
	return rooms_string_t_zout;
}

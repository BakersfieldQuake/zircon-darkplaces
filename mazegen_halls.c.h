// mazegen_halls.c.h
// - Hall
// | Hall
// O Room
// + Intersection
// Destructively modify
#define HALL_NORTH_SOUTH_PIPE_CHAR	'|' // 124
#define HALL_EAST_WEST_MINUS_CHAR	'-'	// minus = 45 ascii
#define	HALL_CH_EW_MINUS			'-'
#define HALL_CH_NS_PIPE				'|'

// An intersection has as many connectors as neighbors dictate.
// All neighbors are forced to connect to an intersection.
// If a neighbor next to an intersection must be promoted, then do it.
#define HALL_INTERSECTION_PLUS_CHAR	'+' // plus = 43 ascii . Does not occur!!!!  March 20 2026
#define HALL_ROOM_CHAR_48_ZERO		48	// Aka ZERO '0' Room starts at 48 zero and goes up 
#define HALL_ROOM_CHAR_MAX_123		123	// Aka ZERO '0' Room starts at 48 zero and goes up 
#define HALL_EMPTY_SPACE_CELL_32	32	// 

#define HALL_CH_INTER_PLUS			'+'
#define HALL_CH_NO_NORTH_DOLLAR		'$'
#define HALL_CH_NO_SOUTH_PERCENT	'%'
#define HALL_CH_NO_EAST_LPAREN		'('
#define HALL_CH_NO_WEST_RPAREN		')'

RELATED_ (nsew_e)

#define HALL_GET_CELL(ss, rowz, colz)			ss[(rowz) * numcolumns + (colz)]
#define HALL_GET_CELL_FORCE(ss, rowz, colz)		hall_get_cell_force_char(ss, rowz, colz, numrows, numcolumns)

#define CH_IS_ROOM(ich)							in_range(HALL_ROOM_CHAR_48_ZERO, ich, HALL_ROOM_CHAR_MAX_123)
#define CH_IS_INTER(ich)						(ich == HALL_INTERSECTION_PLUS_CHAR)
#define CH_IS_EW(ich)							(ich == HALL_EAST_WEST_MINUS_CHAR)
#define CH_IS_NS(ich)							(ich == HALL_NORTH_SOUTH_PIPE_CHAR)
#define CH_IS_NSEW_HALL(ich)					isin2(ich, HALL_EAST_WEST_MINUS_CHAR, HALL_NORTH_SOUTH_PIPE_CHAR)
#define CH_IS_EMPTY(ich)						(isin2 (ich, CHAR_NULL_0, HALL_EMPTY_SPACE_CELL_32))

// Q: This doesn't return bool so explain result
// A: ch_fill is 'X'
// A: ch_target is ' '
// A: The caller checks every room coordinate to make sure they are filled with an X

// Call: Baker_Flood_Fill ('X', ' ', col, row, s_fillup2_za, numcolumns, numrows);
void Baker_Flood_Fill (int ch_fill, int ch_target, int x, int y, char *s_modify, int width, int height)
{
	//origin2d_s  my_queue[WIDTH_80 * WIDTH_80]
//	int			slen = strlen(s_modify);
	int			numcells = width*height;
	int			cell_size = sizeof(point2d_s);
	point2d_s	*my_queue = (point2d_s *)calloc(numcells, cell_size);
	int			queue_count = 0;
//	int			iters = 0;
	int			ox = 0, oy = 0;
	point2d_s	q = {0,0};

	// Assume our pixel is on target number and in range
	my_queue[queue_count].col = x, my_queue[queue_count].row = y;
	queue_count ++;
	//print "fill color = ", ff_tex_num, "targing", ff_targ_num, "at", x, y

	while (queue_count > 0) {
		q.col = my_queue[queue_count - 1].col, q.row = my_queue[queue_count - 1].row;
		queue_count --;

		//set_cell_texture_num q.x, q.row, ff_tex_num
		s_modify[q.row * width + q.col] = ch_fill;

		ox = q.col + 0, oy = q.row - 1; // LOOK UP
		if (in_range_beyond (0, ox, width) && in_range_beyond (0, oy, height) && s_modify[oy * width + ox] == ch_target) {
			my_queue[queue_count].col = ox;
			my_queue[queue_count].row = oy;
			/*print "Added ", queue_count, "x/y = ", my_queue[queue_count].x, my_queue[queue_count].row, "this pix =", boxes[oy * WIDTH_80 + ox].texture_num;*/
			queue_count ++;
		}

		ox = q.col + 0, oy = q.row + 1; // LOOK DOWN
		if (in_range_beyond (0, ox, width) && in_range_beyond (0, oy, height) && s_modify[oy * width + ox] == ch_target) {
			my_queue[queue_count].col = ox;
			my_queue[queue_count].row = oy;
			/*print "Added ", queue_count, "x/y = ", my_queue[queue_count].x, my_queue[queue_count].row, "this pix =", boxes[oy * WIDTH_80 + ox].texture_num;*/
			queue_count ++;
		}

		ox = q.col - 1, oy = q.row + 0; // LOOK LEFT
		if (in_range_beyond (0, ox, width) && in_range_beyond (0, oy, height) && s_modify[oy * width + ox] == ch_target) {
			my_queue[queue_count].col = ox;
			my_queue[queue_count].row = oy;
			/*print "Added ", queue_count, "x/y = ", my_queue[queue_count].x, my_queue[queue_count].row, "this pix =", boxes[oy * WIDTH_80 + ox].texture_num;*/
			queue_count ++;
		}
		ox = q.col + 1, oy = q.row + 0; // LOOK RIGHT
		if (in_range_beyond (0, ox, width) && in_range_beyond (0, oy, height) && s_modify[oy * width + ox] == ch_target) {
			my_queue[queue_count].col = ox;
			my_queue[queue_count].row = oy;
			/*print "Added ", queue_count, "x/y = ", my_queue[queue_count].x, my_queue[queue_count].row, "this pix =", boxes[oy * WIDTH_80 + ox].texture_num;*/
			queue_count ++;
		}
	} // while

	freenull_ (my_queue);
} // FloodFill


#include "mazegen_halls_generate.c.h"

char *Halls_Generate_Zalloc (int numcolumns, int numrows, int numrooms, stringlist_t *plist_supplied_room_coords, char *room_bytes_optional_400)
{
	char *s_a_out = NULL;
	int num_iters = 0;
	while(s_a_out == NULL) {
		if (num_iters > 50) {
			return NULL;
		}
		// ONLY add to plist_supplied_room_coords if successful
		s_a_out = Halls_Generate_NULL_Fail_ZA (numcolumns, numrows, numrooms, plist_supplied_room_coords, room_bytes_optional_400);
		num_iters ++;
	}
#if 0 // NO PRINT
	Con_PrintVarInt (num_iters);
#endif
	return s_a_out;
}


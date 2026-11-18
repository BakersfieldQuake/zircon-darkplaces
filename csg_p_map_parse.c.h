// csg_p_map_parse.c.h


RELATED_ (map_load_from_string) // SUPERCEDED BY THIS MOSTLY

static int _parsemap_readfaces_facerow (map_reader_s *pread, brushrow_s *read_face)
{
#define READMORE if (COM_Parse_Basic(&pread->datasrc) == false) return false /*NOT OK*/

//( 480 704 -260 ) ( 480 672 -260 ) ( 480 704 -132 )
//				common/caulk [ 0 1 0 0 ] [ 0 0 -1 0 ] -0 1 1 0 160 0

	READMORE;	read_face->a[0] = atof(com_token);			//  480
	READMORE;	read_face->a[1] = atof(com_token);			//  704
	READMORE;	read_face->a[2] = atof(com_token);			// -260
	READMORE;	if (com_token[0] != ')') return false;		// )

	READMORE;	if (com_token[0] != '(') return false;		// (
	READMORE;	read_face->b[0] = atof(com_token);				//	480
	READMORE;	read_face->b[1] = atof(com_token);				//	672
	READMORE;	read_face->b[2] = atof(com_token);				// -260
	READMORE;	if (com_token[0] != ')') return false;		// )

	READMORE;	if (com_token[0] != '(') return false;		// (
	READMORE;	read_face->c[0] = atof(com_token);				//	480
	READMORE;	read_face->c[1] = atof(com_token);				//	704
	READMORE;	read_face->c[2] = atof(com_token);				// -132
	READMORE;	if (com_token[0] != ')') return false;		// )

//( 480 704 -260 ) ( 480 672 -260 ) ( 480 704 -132 )
//				common/caulk [ 0 1 0 0 ] [ 0 0 -1 0 ] -0 1 1 0 160 0

// <x_shift> <y_shift>   ... <rotation> <x_scale> <y_scale> <content_flags> <surface_flags> <value>
	READMORE;	read_face->pbrtexture = Z_StrDup(com_token); //	common/caulk
	READMORE;	if (com_token[0] != '[') return false;		// [
	READMORE;	read_face->tex_axis_s[0] = atof(com_token);			//	0
	READMORE;	read_face->tex_axis_s[1] = atof(com_token);			//	1
	READMORE;	read_face->tex_axis_s[2] = atof(com_token);			//	0
	READMORE;	read_face->tex_axis_s[3] = atof(com_token);			//	0

	//if (read_face->tex_axis_s[3] < 0) {
	// Baker: October 26 2026 - WHY DID A NUKE THOSE?
	// Was it messing with frost?
#if 0
	if (String_Match(com_token, "-0")) {


		int is_negative_bitty = signbit(read_face->tex_axis_s[3]);

		int is_zero = read_face->tex_axis_s[3] == 0;
		int is_nega_zero = is_negative_bitty == 1;
		int is_posi_zero = is_negative_bitty == 0;
		int j = 5 ;

		read_face->tex_axis_s[3] = 0;
	}

#endif

	READMORE;	if (com_token[0] != ']') return false;		// ]
	READMORE;	if (com_token[0] != '[') return false;		// [
	READMORE;	read_face->tex_axis_t[0] = atof(com_token);			//	0
	READMORE;	read_face->tex_axis_t[1] = atof(com_token);			//	0
	READMORE;	read_face->tex_axis_t[2] = atof(com_token);			// -1
	READMORE;	read_face->tex_axis_t[3] = atof(com_token);			//	0
	READMORE;	if (com_token[0] != ']') return false;		// ]

//( 32768 -34816 10240 ) ( 32768 -34816 -512 ) ( 32768 34816 10240 ) sky/treefall_nite_sky [ 0 1 0 -0 ]
//		[ 0 0 -1 -0 ] 0 0.5 0.5 // 3 numbers
//( 34816 34816 10240 ) ( 34816 34816 -512 ) ( 34816 -34816 10240 ) common/caulk [ 0 1 0 -0 ] [ 0 0 -1 -0 ]
//		              0 0.5 0.5 0 160 0 // 6 numbers

	READMORE;	read_face->faceparms[0] = atof(com_token);			// -0
	READMORE;	read_face->faceparms[1] = atof(com_token);			//	1
	READMORE;	read_face->faceparms[2] = atof(com_token);			//	1

	read_face->trail_count = 3;
	ccs *rollback;

	rollback = pread->datasrc;
	READMORE;
	if (isin2(com_token[0], '(', '}') == false) {
		read_face->faceparms[3] = atof(com_token);			//	0
		read_face->trail_count ++; // 3 --> 4
	} else {
		pread->datasrc = rollback;
		return true;
	}

	rollback = pread->datasrc;
	READMORE;
	if (isin2(com_token[0], '(', '}') == false) {
		read_face->faceparms[4] = atof(com_token);			//	0
		read_face->trail_count ++; // 3 --> 4
	} else {
		pread->datasrc = rollback;
		return true;
	}

	rollback = pread->datasrc;
	READMORE;
	if (isin2(com_token[0], '(', '}') == false) {
		read_face->faceparms[5] = atof(com_token);			//	0
		read_face->trail_count ++; // 3 --> 4
	} else {
		pread->datasrc = rollback;
		return true;
	}

	return true; // SUCCESS!
}


static int parsemap_readfaces_thru_curly_isok (map_reader_s *pread, brush_s *read_brush)
{
	while (1) {
		// We just read a "("

		brushrow_s	*p_brushrow = _LIFE_brushface_alloc (&read_brush->brushrowlist);
		pread->p_brushrow = p_brushrow;
		int is_ok = _parsemap_readfaces_facerow(pread, p_brushrow);
		if (is_ok == false)
			return false;

		// READ NEXT -- IT IS EITHER "(" NEW ROW OR "}" END OF DATA
		if (COM_Parse_Basic(&pread->datasrc) == false || isin2(com_token[0], '(', '}') == false) {
			Con_PrintLinef ("Expected ) or { got " QUOTED_S " in fn %s", com_token, __func__ );
			return false;
		}

		if (com_token[0] == '}') {
			// NORMAL END OF BRUSH
			pread->depth --;
			// DEPTH should be 1 now
			return true; // SUCCESS!
		}

		// We read a "(" loop back around and add a new row
	}

	// Unreachable!
	return false;
}

		//patchDef2
		//{ <---------------------------------- START
		//	sg_tomb/wall019
		//	( 3 3 0 0 0 )
		//	(
		//		( ( -749.894287 6939.077148 3254.350586 0.000000 0.561562 ) ( -678.014404 6939.077148 3254.350586 0.000000 0.280781 ) ( -606.134460 6939.077148 3254.350586 0.000000 0.000000 ) )
		//		( ( -749.894287 6942.136719 3314.468750 0.235141 0.561562 ) ( -678.014404 6942.136719 3314.468750 0.235141 0.280781 ) ( -606.134460 6942.136719 3314.468750 0.235141 0.000000 ) )
		//		( ( -749.894287 7006.000000 3325.045410 0.488005 0.561562 ) ( -678.014404 7006.000000 3325.045410 0.488005 0.280781 ) ( -606.134460 7006.000000 3325.045410 0.488005 0.000000 ) )
		//	)
		//}
static int _parsemap_readfaces_patchblock_isok (map_reader_s *pread, brush_s *read_brush)
{
	// We just read a "patchDef2"
		//{ <---------------------------------- START
		//	sg_tomb/wall019
		//	( 3 3 0 0 0 )
		//	(
		//		( ( -749.894287 6939.077148 3254.350586 0.000000 0.561562 ) ( -678.014404 6939.077148 3254.350586 0.000000 0.280781 ) ( -606.134460 6939.077148 3254.350586 0.000000 0.000000 ) )
		//		( ( -749.894287 6942.136719 3314.468750 0.235141 0.561562 ) ( -678.014404 6942.136719 3314.468750 0.235141 0.280781 ) ( -606.134460 6942.136719 3314.468750 0.235141 0.000000 ) )
		//		( ( -749.894287 7006.000000 3325.045410 0.488005 0.561562 ) ( -678.014404 7006.000000 3325.045410 0.488005 0.280781 ) ( -606.134460 7006.000000 3325.045410 0.488005 0.000000 ) )
		//	)
		//}
	READMORE; if (com_token[0] != '{') return false;
	READMORE; read_brush->patch_texture = Z_StrDup (com_token);
	READMORE; if (com_token[0] != '(') return false;
	READMORE; read_brush->patch_rows = atof(com_token); // ( 3 3 0 0 0 )   3 is rows
	READMORE; read_brush->patch_cols = atof(com_token); //     3				is columns
	READMORE; read_brush->other3[0] = atof(com_token);
	READMORE; read_brush->other3[1] = atof(com_token);
	READMORE; read_brush->other3[2] = atof(com_token);
	READMORE; if (com_token[0] != ')') return false;
	READMORE; if (com_token[0] != '(') return false;

	// We read a series of rows
	// Read the closing )
	// Read the closing } and decrease depth

	// WE ARE ON A ROW

	for (int row = 0; row < read_brush->patch_rows; row ++) {
		patchrow_s *p_row = patchrowlist_add (&read_brush->patchrowlist);
		READMORE; if (com_token[0] != '(') return false;
		for (int col = 0; col < read_brush->patch_cols; col ++) {
			READMORE; if (com_token[0] != '(') return false;
			// Read 5 numbers
			READMORE; p_row->flots[col * 5 + 0] = atof(com_token);
			READMORE; p_row->flots[col * 5 + 1] = atof(com_token);
			READMORE; p_row->flots[col * 5 + 2] = atof(com_token);
			READMORE; p_row->flots[col * 5 + 3] = atof(com_token);
			READMORE; p_row->flots[col * 5 + 4] = atof(com_token);
			READMORE; if (com_token[0] != ')') return false;
		}
		READMORE; if (com_token[0] != ')') return false;
	}

	READMORE; if (com_token[0] != ')') return false;
	READMORE; if (com_token[0] != '}') return false;
	READMORE; if (com_token[0] != '}') return false;
	pread->depth --;
	return true;
}


static qbool private_map_load_from_string_parsemaptxt (map_t *plist, ccs *txt)
{
	map_reader_s _reader = {0}, *pread = &_reader;
	pread->text_start = pread->datasrc = txt;

	pread->plist = plist;
	int iters = 0;
	while (1) {
		iters ++;
		// Read some datasrc ...
		if (COM_Parse_Basic(&pread->datasrc) == false) {
			if (pread->depth == 0) {
				plist->original_ent_count = plist->count; // May 31 2025
				return true; // End of string data
			}

			return false; // We expected more data because depth is not 0, therefore this is "bad"
		}

		if (com_token[0] == '}') {
			if (pread->depth == 1) { // (depth 1 ===> 0)
				// STATE_CHECKCLASS_0; // End of entity
			}
			pread->depth --;
			continue;
		} else if (com_token[0] == '{') {
			if (pread->depth == 0) {
				// Allocate a new entity. (depth 0 ===> 1)
				pread->p_ent = entitylist_add (pread->plist);
			}
			pread->depth ++;
			continue;
		}

read_brush:
		if (pread->depth >= 2) {
			if (com_token[0] == '(') { // THAT IS A PAREN
				// I suggest reading brush rows and then the "}" decreasing the depth
				// NEW BRUSH
				RELATED_ (entityx_s)
				pread->p_brush = _LIFE_brush_add (&pread->p_ent->brushlist);
				int is_ok = parsemap_readfaces_thru_curly_isok (pread, pread->p_brush);
				if (is_ok == false)
					return false; // Exited
				//pread->depth --;
				continue;
			} else if (String_Match (com_token, "patchDef2")) {
read_patch:
				//patchDef2
				//{ <---------------------------------- START
				//	sg_tomb/wall019
				//	( 3 3 0 0 0 )
				//	(
				//		( ( -749.894287 6939.077148 3254.350586 0.000000 0.561562 ) ( -678.014404 6939.077148 3254.350586 0.000000 0.280781 ) ( -606.134460 6939.077148 3254.350586 0.000000 0.000000 ) )
				//		( ( -749.894287 6942.136719 3314.468750 0.235141 0.561562 ) ( -678.014404 6942.136719 3314.468750 0.235141 0.280781 ) ( -606.134460 6942.136719 3314.468750 0.235141 0.000000 ) )
				//		( ( -749.894287 7006.000000 3325.045410 0.488005 0.561562 ) ( -678.014404 7006.000000 3325.045410 0.488005 0.280781 ) ( -606.134460 7006.000000 3325.045410 0.488005 0.000000 ) )
				//	)
				//}
				RELATED_ (entityx_s)
				//pread->p_patch = patchlist_add (&pread->p_ent->patchlist);

				pread->p_brush = _LIFE_brush_add (&pread->p_ent->brushlist);
				pread->p_brush->is_a_patch = true;
				int is_ok = _parsemap_readfaces_patchblock_isok (pread, pread->p_brush);
				if (is_ok == false)
					return false; // Exited

				continue;
			}
			// We shouldn't get here ...
			return false;
		}

		// DEPTH = 1 and not { or } or patchDef2 therefore
		// we are parsing epairs.
entity_read_key:

		stringlistappend (&pread->p_ent->pairslist, com_token); // KEY

entity_read_value:
		// Read another - Check for error condition
		if (COM_Parse_Basic(&pread->datasrc) == false)
			return false; // End of string data

		stringlistappend (&pread->p_ent->pairslist, com_token); // KEY
	} // While



}




CALLERS_ (VM_map_load)
int entitylist_parse_mapfile_errorcode (map_t *plist, ccs *filename)
{
	char *maptext_za = FS_LoadFileQuick (filename);
	if (!maptext_za)
		return not_found_neg1; // File not found

	int isok = map_load_from_string (plist, maptext_za);

	int ret = SUCCESS_OK_0;
	if (!isok)
		ret = -3; // Opened, couldn't parse the data.

	Mem_FreeNull_ (maptext_za);
	return ret;
}


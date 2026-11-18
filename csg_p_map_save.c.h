// csg_p_map_save.c.h


qbool is_integer_float(float val) {
    // Returns true if the float has no fractional component
    //return val == truncf(val); VISUAL STUDIO 2008 NO HAVE truncf
	return val == (float)(int)val;
}

int is_negative_zero(float value)
{
    union
    {
        float f;
        unsigned int i;
    } u;

    u.f = value;

    return (u.i == 0x80000000);
}

float Float_SnapToInteger_If_003(float value)
{
#define SNAP_EPSILON_1_DIV_256_003 (1/256.0) // (1/256 = 0.00390625\)
    float rounded;

	if (is_negative_zero(value)) {
		return value;
	}

    if (value >= 0.0f)
        rounded = floorf(value + 0.5f);
    else
        rounded = ceilf(value - 0.5f);

    if (fabsf(value - rounded) < SNAP_EPSILON_1_DIV_256_003 )
        return (rounded == 0.0f) ? 0.0f : rounded;
	
	if (_csg_snaptointeger_negative_zero_is_zero.integer)
		return (value == 0.0f) ? 0.0f : value;
	else
		return value;
}


void _cat_brushnum_patch (baker_string_s *bs_maptext, map_t *hm, int entnum, int brushnum)
{
	BRUSH_DEFINE_CONST (r_brush, hm, entnum, brushnum);
	BakerString_CatCFmt (bs_maptext, "patchDef2" NEWLINE);
	BakerString_CatCFmt (bs_maptext, "{" NEWLINE);
	// caves/3i_sand2
	BakerString_CatCFmt (bs_maptext, "%s" NEWLINE, r_brush->patch_texture);
	// ( 9 3 0 0 0 )
	BakerString_CatCFmt (bs_maptext, "( " FLOAT_LOSSLESS_FORMAT " " FLOAT_LOSSLESS_FORMAT " " FLOAT_LOSSLESS_FORMAT " " FLOAT_LOSSLESS_FORMAT " " FLOAT_LOSSLESS_FORMAT " )" NEWLINE, r_brush->patch_rows, r_brush->patch_cols, r_brush->other3[0], r_brush->other3[1], r_brush->other3[2]);
	BakerString_CatCFmt (bs_maptext, "(" NEWLINE);

for_each_const_patch_row (rownum, r_brush, r_patchrow)
            BakerString_CatCFmt (bs_maptext, "( " NEWLINE);
            for_each_patch_column (columnnum, r_brush, r_patchrow)
			// These don't perfectly match J.A.C.K. output which has extra zeroes
			// in the decimal places sometimes.

                float x_val = r_patchrow->flots[columnnum * 5 + 0];
                float y_val = r_patchrow->flots[columnnum * 5 + 1];
                float z_val = r_patchrow->flots[columnnum * 5 + 2];
                float s_val = r_patchrow->flots[columnnum * 5 + 3];
                float t_val = r_patchrow->flots[columnnum * 5 + 4];

                char x_buf[32], y_buf[32], z_buf[32], s_buf[32], t_buf[32];

				if (_csg_snaptointeger.integer) {
					x_val = Float_SnapToInteger_If_003(x_val);
					y_val = Float_SnapToInteger_If_003(y_val);
					z_val = Float_SnapToInteger_If_003(z_val);
					s_val = Float_SnapToInteger_If_003(s_val);
					t_val = Float_SnapToInteger_If_003(t_val);
				}
                // X coordinate
                if (is_integer_float(x_val)) {
                    c_dpsnprintf1 (x_buf, "%g", x_val);
                } else {
                    c_dpsnprintf1 (x_buf, "%.6f", x_val);
                }

                // Y coordinate
                if (is_integer_float(y_val)) {
                    c_dpsnprintf1 (y_buf, "%g", y_val);
                } else {
                    c_dpsnprintf1 (y_buf, "%.6f", y_val);
                }

                // Z coordinate
                if (is_integer_float(z_val)) {
                    c_dpsnprintf1 (z_buf, "%g", z_val);
                } else {
                    c_dpsnprintf1 (z_buf, "%.6f", z_val);
                }

                // S coordinate
                if (is_integer_float(s_val)) {
                    c_dpsnprintf1 (s_buf, "%g", s_val);
                } else {
                    c_dpsnprintf1 (s_buf, "%.6f", s_val);
                }

                // T coordinate
                if (is_integer_float(t_val)) {
                    c_dpsnprintf1 (t_buf, "%g", t_val);
                } else {
                    c_dpsnprintf1 (t_buf, "%.6f", t_val);
                }

                BakerString_CatCFmt (bs_maptext, "( %s %s %s %s %s )" NEWLINE,
                    x_buf, y_buf, z_buf, s_buf, t_buf
                );

            for_each_end // patch_column
            BakerString_CatCFmt (bs_maptext, " )" NEWLINE);
        for_each_end // patch_row

	BakerString_CatCFmt (bs_maptext, ")" NEWLINE);
	BakerString_CatCFmt (bs_maptext, "}" NEWLINE);
}

RELATED_ (Carve_Create_Result_Brush)
void _cat_brushnum (baker_string_s *bs_maptext, map_t *hm, int entnum, int brushnum)
{
	BRUSH_DEFINE_CONST (r_brush, hm, entnum, brushnum);

	//BakerString_CatCFmt (bs_maptext, "{ // Entity %d Brush %d" NEWLINE, entnum, brushnum);

	BakerString_CatCFmt (bs_maptext, "{"); // Entity %d Brush %d", entnum, brushnum);

	if (_csg_detail.value) {
		// EXTRA DETAIL
		BakerString_CatCFmt (bs_maptext, " // Entity %d Brush %d", entnum, brushnum);
		if (r_brush->is_a_patch == false) {
			#ifdef _DEBUG
			if (r_brush->irregular) {
				//int j = 5;
			}
			#endif // _DEBUG
			BakerString_CatCFmt (bs_maptext, " Faces = %d Cubeoid = %s mins '" V3G "' maxs '" V3G "' size [" V3G "]",
				r_brush->brushrowlist.count, r_brush->is_cubeoid ? "Yes" : "No",
				VS(r_brush->bxmins), VS(r_brush->bxmaxs), VS(r_brush->bxdims)
				);
			if (r_brush->describe[0]) {
				BakerString_CatCFmt (bs_maptext, "%s %s" NEWLINE,
				r_brush->irregular ? "[IRREGULAR]" : "",
				r_brush->describe);
			}
			else
				BakerString_CatCFmt (bs_maptext, NEWLINE);

		} else {
			BakerString_CatCFmt (bs_maptext, " Patch" NEWLINE);
		}
	} else {
		// NO DETAIL
		BakerString_CatCFmt (bs_maptext, NEWLINE);
	}

	if (r_brush->is_a_patch) {
		// if patch not a brush
		_cat_brushnum_patch (bs_maptext, hm, entnum, brushnum);
	} else {
		// REAL BRUSH
		for_each_brush_face_2 (facenum, r_face, hm, entnum, brushnum)

		vec3_t a, b, c;

		VectorCopy(r_face->a, a);
		VectorCopy(r_face->b, b);
		VectorCopy(r_face->c, c);

		if (_csg_snaptointeger.integer) {
			Vector_SnapToInteger_If_003 (a);
			Vector_SnapToInteger_If_003 (b);
			Vector_SnapToInteger_If_003 (c);
		}

			BakerString_CatCFmt (bs_maptext,
				"( " VECTOR3_G  " ) "
				"( " VECTOR3_G  " ) "
				"( " VECTOR3_G  " ) "
				" %s "
				"[ " VECTOR4_G " ] "
				"[ " VECTOR4_G " ] "
				VECTOR3_G,
				 VECTOR3_SEND(a), // r_face->a),
				 VECTOR3_SEND(b), // r_face->b),
				 VECTOR3_SEND(c), // r_face->c),
				 r_face->pbrtexture,
				 VECTOR4_SEND(r_face->tex_axis_s),
				 VECTOR4_SEND(r_face->tex_axis_t),
				 VECTOR3_SEND(r_face->faceparms)
				 );

			// Any extras add a space
			for (int j = 3; j < r_face->trail_count; j ++) {
				BakerString_CatCFmt (bs_maptext, " " FLOAT_LOSSLESS_FORMAT, r_face->faceparms[j] );
			}

			if (_csg_detail.value) {
				if (r_face->num_points) {
					RELATED_ (private_map_load_from_string_parsemaptxt)
					BakerString_CatCFmt (bs_maptext, " // num = %d", r_face->num_points);
					for (int pointnum = 0; pointnum < r_face->num_points; pointnum ++) {
						float *this_point = r_face->points[pointnum];
						BakerString_CatCFmt (bs_maptext, " " V3G, VS(this_point));
					}
				}
			} // detail

			BakerString_CatC (bs_maptext, NEWLINE);
		for_each_end // for_each_brush_face
	}

	BakerString_CatCFmt (bs_maptext, "}" NEWLINE);

	//if (r_brush->irregular) {
	//	Clipboard_Set_Text (bs_maptext->string);
	//}
}

void _cat_entnum (baker_string_s *bs_maptext, map_t *hm, int entnum)
{
	ENT_DEFINE_CONST (r_ent, hm, entnum);
	// Print keys

	BakerString_CatCFmt (bs_maptext, "{");
	if (_csg_detail.value) {
		BakerString_CatCFmt (bs_maptext, " // Entity %d", entnum);
	}
	BakerString_CatCFmt (bs_maptext, NEWLINE);

	for_each_key_val_pair (keyidx, r_ent, key, val)
		// August 5 2026 - keys with name "" no longer write.  Considered deleted.
		if (key && key[0] == CHAR_NULL_0) continue; // map_key_delete: This is where it no longer saves.
		BakerString_CatLinef (bs_maptext, QUOTED_S " " QUOTED_S, key, val);
	for_each_end // key val pairs

	//for_each_brush_undeleted (brushnum, r_ent, r_brush)
	for_each_brush_or_patch_2 (brushnum, r_brush, hm, entnum)
		//BakerString_CatCFmt (bs_maptext, "{" NEWLINE); // OLD
		//BakerString_CatCFmt (bs_maptext, "{ // Ent %d Brush %d" NEWLINE, entnum, brushnum);
		_cat_brushnum (bs_maptext, hm, entnum, brushnum);
	for_each_end // for_each_brush_or_patch
	BakerString_CatCFmt (bs_maptext, "}" NEWLINE);
}

baker_string_s *map_save_to_string_bsalloc (map_t *hm)
{
	baker_string_s *bs_maptext = BakerString_Create_Malloc_With_MegaBytes (100); // 100 MB

	//for_each_entity_undeleted (entnum, plist, r_ent)
	for_each_entity_2 (entnum, r_ent, hm)
		_cat_entnum (bs_maptext, hm, entnum);
	for_each_end // for_each_entity
	return bs_maptext;
}

qbool map_save_as (map_t *hm, ccs *filename)
{
	baker_string_s *bsa = map_save_to_string_bsalloc (hm);
	qbool isok = FS_WriteFileQuick (filename, bsa->string, bsa->length);

	BakerString_Destroy_And_Null_It (&bsa); // July 29 2025 - This was missing, fool!

	return isok;
}

RELATED_ (csgc_printunit) // csgc_printunit does not calls because it also prints it.
void map_clipboard_set_brush (map_t *hm, int entnum, int brushnum)
{
	baker_string_s *bs_maptext = BakerString_Create_Malloc (""); // 100 MB
	_cat_brushnum (bs_maptext, hm, entnum, brushnum);
	Clipboard_Set_Text (bs_maptext->string);
	BakerString_Destroy_And_Null_It (&bs_maptext); // July 29 2025 - This was missing, fool!
}

RELATED_ (csgc_printunit) // csgc_printunit does not calls because it also prints it.
void map_clipboard_set_entity (map_t *hm, int entnum)
{
	baker_string_s *bs_maptext = BakerString_Create_Malloc (""); // 100 MB
	_cat_entnum (bs_maptext, hm, entnum);
	Clipboard_Set_Text (bs_maptext->string);
	BakerString_Destroy_And_Null_It (&bs_maptext); // July 29 2025 - This was missing, fool!
}


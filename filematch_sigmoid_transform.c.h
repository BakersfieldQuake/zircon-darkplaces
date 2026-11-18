// filematch_sigmoid_transform.c.h



RELATED_ (SCR_sigmoid_f)



void SCR_brushmoverotate_f (cmd_state_t *cmd)
{
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (3, " <maps/yourmap | clip> <x y z> <pitch yaw roll>", 
		"Transforms and puts back on clipboard ");

	ccs		*filename1		= cmd_argv_parm1;	// maps/mymap.map or maps/mymap
	ccs		*smove			= cmd_argv_parm2;
	ccs		*srotate		= cmd_argv_parm3;
	qbool	is_sigmoid = String_Match (smove, "sigmoid"); // brushmoverotate chunk.map sigmoid ""
	ccs		*snth			= cmd_argv_parm4;
	int nth = atoi(snth);

	vec3_t move, rotate;
	Math_atov3 (smove, move);
	Math_atov3 (srotate, rotate);
	
	printvari(is_sigmoid);
	printvarv(move);
	printvarv(rotate);
	printvari(nth);

	map_t list_dest = {0}; map_add (&list_dest, "worldspawn");

	map_t list_src0 = {0};
	char *sin_za = NULL;
	va_super (filename,  MAX_OSPATH_EX_1024, "%s", filename1);

	if (String_Match (filename1, "clip")) {
		sin_za = Clipboard_Get_Text_ZAlloc();
		CMD_FAILEXIT_IF_ (!sin_za || strlen(sin_za) == 0, "No text on clipboard")
	} else {
		
		if (String_Ends_With (filename,  ".map") == false)	c_strlcat (filename,  ".map");

		qbool bfilenameexists = FS_File_Exists(filename);
		CMD_FAILEXIT_IF2_ (bfilenameexists == false, "Filename %s does not exist to read", filename)

		sin_za = FS_LoadFileQuick (filename);
		CMD_FAILEXIT_IF2_ (!sin_za, "Couldn't %s", filename)
	}

	int isok = map_load_from_string (&list_src0, sin_za);
	Mem_FreeNull_ (sin_za); // Done with this.
	CMD_FAILEXIT_IF2_ (!isok, "Couldn't parse map %s", filename)
	
	void TimeString_Fill_YYYYMMDD_HHMMSSAM (char *timestring, size_t timestring_size);
	char timestring[64]; TimeString_Fill_YYYYMMDD_HHMMSSAM (timestring, sizeof(timestring));
	va_super (timestring_newval, 128, "0 - %s", timestring);  // looks like "0 - 20241005 10:12 AM" //char *snewval_za = Z_StrDupf ("0 - %s", timestring);

		GETL_WORLD_0 (list_src0, p_world); // entityx_s	*p_world = &list_src0.entity[ENTITY_WORLD_0];
		
		printvarv (p_world->bemaxs);
		printvarv (p_world->bemins);
		vec3_t brcenter;
		VectorAdd(p_world->bemaxs, p_world->bemins, brcenter);
		VectorScale(brcenter, 0.5f, brcenter);
		printvarv (brcenter);

		if (Vector3_IsNotZeros(brcenter)) {
			zpr ("Not centered, centering ...");
			vec3_t move_to_center;
			VectorNegate (brcenter, move_to_center);
			printvarv (move_to_center);

			map_move_epairs_origin	(&list_src0, move_to_center);
			map_move_all_brushes_patches		(&list_src0, move_to_center); // Has texture scaling.
		}

// NOW WHAT?

set_new_to_clipboard:
	

	if (is_sigmoid) {
		// floatlist_s sigmoid_float_list; // origin 3, angles 3, forward 3, up 3 -- a 12 pack
		RELATED_ (SCR_sigmoid_f floatlist_s)

		printvari (sigmoid_float_list.count);
		int iters = 0;
		int num_rexture = 0;
		for_each_floatlist_by_step (idx, sigmoid_float_list, 12)
			Vector3DeclareSet (org, sigmoid_float_list.floats[idx + 0], sigmoid_float_list.floats[idx + 1], sigmoid_float_list.floats[idx + 2] );
			Vector3DeclareSet (ang, sigmoid_float_list.floats[idx + 3], sigmoid_float_list.floats[idx + 4], sigmoid_float_list.floats[idx + 5] );
			Vector3DeclareSet (forward, sigmoid_float_list.floats[idx + 6], sigmoid_float_list.floats[idx + 7], sigmoid_float_list.floats[idx + 8] );
			Vector3DeclareSet (up, sigmoid_float_list.floats[idx + 9], sigmoid_float_list.floats[idx + 10], sigmoid_float_list.floats[idx + 11] );
		
			map_t list_temp = {0};

			map_add (&list_temp, "worldspawn");
			map_append_map (&list_temp, &list_src0);

			if (1 && nth) {
				int do_it = (iters % nth) == 0;
				if (do_it) {
					int num_done = csgc_brushfacer_specific_texture (&list_temp, "texturecan/wood_0047d");
					if (num_done)
						num_rexture += num_done;
				} // do it
			} // nth

			map_rotate_brush_fix_yaw (&list_temp, org, ang);
			map_world_brushes_patches_copy (&list_dest, &list_temp);

			map_freecontents (&list_temp);
			iters ++;
		for_each_end

		
		GETL_WORLD_0 (list_dest, p_world); // entityx_s	*p_world = &list_dest.entity[ENTITY_WORLD_0];
		printvari (p_world->brushlist.count);
		
		printvari (num_rexture);
		printvari (iters);
		
	} else {
		map_rotate_brush_fix_yaw (&list_src0, move, rotate);
		map_world_brushes_patches_copy (&list_dest, &list_src0);
	}
	
	baker_string_s *bsa = map_save_to_string_bsalloc (&list_dest);
	Clipboard_Set_Text (bsa->string);

	BakerString_Destroy_And_Null_It (&bsa); // July 29 2025 - This was missing, fool!

	zpr ("Copied to clipboard");

fail_exit:

	Mem_FreeNull_ (sin_za);
	map_freecontents (&list_src0);
	map_freecontents (&list_dest);
}


// sv_loadgame_from.c.h

void SV_Loadgame_from (cmd_state_t *cmd, const char *s_loadgame)
{
	prvm_prog_t *prog = SVVM_prog;
	char mapname[MAX_QPATH_128];
	float time_from_loadgame;
	float f;
	const char *start;
	const char *end;
	const char *t;
	char *text;
	prvm_edict_t *ent;
	int i, k, numbuffers;
	int entnum;
	int version;
	float spawn_parms[NUM_SPAWN_PARMS_16]; // PARM_EXPAND SV_LOADGAME_FROM
	prvm_stringbuffer_t *stringbuffer;

	t = text = (char *)FS_LoadFile (s_loadgame, tempmempool, fs_quiet_FALSE, fs_size_ptr_null);
	if (!text) {
		Con_PrintLinef ("ERROR: couldn't open.");
		return;
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading version");

	// version
	COM_Parse_Basic (&t);
	version = atoi(com_token);
	if (version != SAVEGAME_VERSION_5) {
		Mem_Free(text);
		Con_PrintLinef ("Savegame is version %d, not %d", version, SAVEGAME_VERSION_5);
		return;
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading description");

	//  HERE is where clear should happen server flags
	#pragma message ("SERVERFIELDS CLEAR ALL - BEFORE LOAD GAME")

	// description
	COM_Parse_Basic (&t);

	for (i = 0 ; i < NUM_SPAWN_PARMS_16; i ++) { // PARM_EXPAND SV_LOADGAME_FROM PARSING SOME PARMS
		COM_Parse_Basic (&t);
		spawn_parms[i] = atof(com_token);
	}
	// skill
	COM_Parse_Basic (&t);
// this silliness is so we can load 1.06 save files, which have float skill values
	current_skill = (int)(atof(com_token) + 0.5);
	Cvar_SetValue (&cvars_all, "skill", (float)current_skill);

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading mapname");

intermap_no_description_spawnparms_skill:

	// mapname
	COM_Parse_Basic (&t);
	c_strlcpy (mapname, com_token);

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading time");

	// time
	COM_Parse_Basic (&t);
	time_from_loadgame = atof(com_token);

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: spawning server");

	// Baker: Intermap ... hmmm .. we do not want entities from the bsp
	// Not even entity 0
	//#pragma message ("LOAD GAME SIV")
	vec3_t zero_origin = {0,0,0}; // SIV TODO!!!!!
	SV_SpawnServer (mapname, s_loadgame, q_s_startspot_EmptyString, zero_origin, /*totaltimeatstart*/ 0); // Baker r9067: loadgame precaches "precache at any time models and sounds"

	if (!sv.active) {
		Mem_Free(text);
		Con_PrintLinef ("Couldn't load map");
		return;
	}

	sv.paused = true;		// pause until all clients connect
	sv.loadgame = true;

intermap_no_sv_loadgame_no_sv_paused:

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading light styles");

// load the light styles

	// -1 is the globals
	entnum = -1;

	for (i = 0; i < MAX_LIGHTSTYLES_256; i ++) {
		// light style
		start = t;
		COM_Parse_Basic (&t);
		// if this is a 64 lightstyle savegame produced by Quake, stop now
		// we have to check this because darkplaces may save more than 64
		if (com_token[0] == '{') {
			t = start;
			break;
		}
		c_strlcpy (sv.lightstyles[i], com_token);
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: skipping until globals");

	// now skip everything before the first opening brace
	// (this is for forward compatibility, so that older versions (at
	// least ones with this fix) can load savegames with extra data before the
	// first brace, as might be produced by a later engine version)
	for (;;) {
		start = t;
		if (!COM_ParseToken_Simple(&t, false, false, true))
			break;

		if (com_token[0] == '{') {
			t = start;
			break;
		}
	} // while (1)

	// unlink all entities
	World_UnlinkAll(&sv.world);

// load the edicts out of the savegame file
	end = t;
	for (;;) {
		start = t;
		while (COM_ParseToken_Simple(&t, false, false, true))
			if (String_Match(com_token, "}"))
				break;

		if (!COM_ParseToken_Simple(&start, false, false, true)) {
			// end of file
			break;
		}

		if (String_NOT_Match (com_token, "{")) {
			Mem_Free(text);
			Host_Error_Line ("First token isn't a brace");
		}

		if (entnum == -1) {
			if (developer_entityparsing.integer)
				Con_PrintLinef ("SV_Loadgame_f: loading globals");

			// parse the global vars
			PRVM_ED_ParseGlobals (prog, start);

			// restore the autocvar globals
			Cvar_UpdateAllAutoCvars(prog->console_cmd->cvars);
		}
		else
		{
			// parse an edict
			if (entnum >= MAX_EDICTS_32768) {
				Mem_Free(text);
				Host_Error_Line ("Host_PerformLoadGame: too many edicts in save file (reached MAX_EDICTS_32768 %d)", MAX_EDICTS_32768);
			}
			while (entnum >= prog->max_edicts)
				PRVM_MEM_IncreaseEdicts(prog);
			ent = PRVM_EDICT_NUM(entnum);
			memset(ent->fields.fp, 0, prog->entityfields * sizeof(prvm_vec_t));
			ent->free = false;

			if (developer_entityparsing.integer)
				Con_PrintLinef ("SV_Loadgame_f: loading edict %d", entnum);

			PRVM_ED_ParseEdict (prog, start, ent, q_is_saveload_true); // ITK #4

			// link it into the bsp tree
			if (!ent->free)
				SV_LinkEdict(ent);
		} // ent != -1

		end = t;
		entnum ++;
	} // end while (1)

	prog->num_edicts = entnum;
	sv.time = time_from_loadgame;
	sv.cdtrack_seconds_into = 0; // Baker: May get overwritten below ...

	// Baker: Reset intermap siv list
	stringlistfreecontents (&sv_intermap_siv_list); // .SIV clear "loadgame"

	for (i = 0 ; i < NUM_SPAWN_PARMS_16; i ++) // PARM_EXPAND SV_LOADGAME_FROM PARSING PLAYER 1 PARMS
		svs.clients[0].spawn_parms[i] = spawn_parms[i];

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: skipping until extended data");

	// read extended data if present
	// the extended data is stored inside a /* */ comment block, which the
	// parser intentionally skips, so we have to check for it manually here
	if (end) {
		while (*end == '\r' || *end == '\n')
			end++;
		// /*
		// // DarkPlaces extended savegame
		// sv.lightstyles 0 m
		// sv.lightstyles 63 a
		// sv.model_precache 1 maps/z2m1_somewhere.bsp
		// sv.sound_precache 1 weapons/r_exp3.wav

		if (end[0] == '/' && end[1] == '*' && (end[2] == '\r' || end[2] == '\n')) {
			if (developer_entityparsing.integer)
				Con_PrintLinef ("SV_Loadgame_f: loading extended data");

			Con_PrintLinef ("Loading extended DarkPlaces savegame");
			t = end + 2;
			memset(sv.lightstyles[0], 0, sizeof(sv.lightstyles));
			memset(sv.model_precache[0], 0, sizeof(sv.model_precache));
			memset(sv.sound_precache[0], 0, sizeof(sv.sound_precache));
			BufStr_Flush(prog);

			// t is ptr
			while (COM_ParseToken_Simple(&t, false, false, true)) {
				if (String_Match(com_token, "sv.lightstyles")) {
					COM_Parse_Basic (&t);
					i = atoi(com_token);
					COM_Parse_Basic (&t);
					if (i >= 0 && i < MAX_LIGHTSTYLES_256)
						strlcpy(sv.lightstyles[i], com_token, sizeof(sv.lightstyles[i]));
					else
						Con_PrintLinef ("unsupported lightstyle %d " QUOTED_S, i, com_token);
				}
				else if (String_Match(com_token, "sv.model_precache")) {
					COM_Parse_Basic (&t);
					i = atoi(com_token);
					COM_Parse_Basic (&t);
					if (i >= 0 && i < MAX_MODELS_8192) {
						strlcpy(sv.model_precache[i], com_token, sizeof(sv.model_precache[i]));
						sv.models[i] = Mod_ForName (sv.model_precache[i], true, false, sv.model_precache[i][0] == '*' ? sv.worldname : NULL);
					}
					else
						Con_PrintLinef ("unsupported model %d " QUOTED_S, i, com_token);
				}
				else if (String_Match(com_token, "sv.sound_precache")) {
					COM_Parse_Basic (&t); // index
					i = atoi(com_token);
					COM_Parse_Basic (&t); // com_token is now string
					if (i >= 0 && i < MAX_SOUNDS_4096)
						strlcpy(sv.sound_precache[i], com_token, sizeof(sv.sound_precache[i]));
					else
						Con_PrintLinef ("unsupported sound %d " QUOTED_S, i, com_token);
				}
				else if (String_Match(com_token, "sv.cdtrack_seconds_into")) {
					COM_Parse_Basic (&t); f = atof(com_token);
					sv.cdtrack_seconds_into = f; // z
				}
				else if (String_Match(com_token, "sv.serverflags")) {
					COM_Parse_Basic (&t); // Flags
					i = atoi(com_token);
					svs.serverflags = i; // LOAD GAME FROM - READ sv.serverflags - WHY WE CAN'T JUST READ THE GLOBAL IS UNCLEAR
					// Baker: Feb 15 2026 - No it isn't unclear.  We cannot read the global.
					// The global is the PRESENT value.
					// The svs.serverflags is the value AT THE START OF THE MAP.
				}
				// VFS_PRINTF(f, "sv.startspot %s\n", InfoBuf_ValueForKey(&svs.info, "*startspot")); //startspot, for restarts.
				else if (String_Match(com_token, "sv.startspot")) {
					//COM_Parse_Basic (&t); // Flags
					//i = atoi(com_token);
					COM_Parse_Basic (&t); // Start spot
					c_strlcpy  (sv.intermap_startspot, com_token);
				}
				else if (String_Match(com_token, "sv.startorigin")) {
					COM_Parse_Basic (&t); f = atof(com_token); sv.intermap_startorigin[0] = f; // x
					COM_Parse_Basic (&t); f = atof(com_token); sv.intermap_startorigin[1] = f; // y
					COM_Parse_Basic (&t); f = atof(com_token); sv.intermap_startorigin[2] = f; // z
				}
				else if (String_Match(com_token, "sv.totaltimeatstart")) {
					COM_Parse_Basic (&t); f = atof(com_token);
					sv.intermap_totaltimeatstart = f; // z
				}
				else if (String_Match(com_token, "sv.totaltimeatlastexit")) {
					COM_Parse_Basic (&t); f = atof(com_token);
					sv.intermap_totaltimeatlastexit = f; // z
				}
				else if (String_Match(com_token, "sv.surplustime")) {
					COM_Parse_Basic (&t); f = atof(com_token);
					sv.intermap_surplustime = f; // z
				}
				else if (String_Match(com_token, "sv.intermap_siv")) {
					// sv.intermap_siv (INDEX) %d (STRLEN)%d (STRING)%s
					COM_Parse_Basic (&t); int siv_idx = atoi(com_token);
					COM_Parse_Basic (&t); int siv_strlen = atoi(com_token);

					const char *s_siv = t; // Cursor is on space for base64 blob
					while (*s_siv && ISWHITESPACE(*s_siv))
						s_siv ++;

					if (siv_idx != sv_intermap_siv_list.numstrings)
						Sys_Error ("Siv index %d but sv_intermap_siv_list.numstrings is %d", siv_idx, sv_intermap_siv_list.numstrings);
					// Baker: Ok here is the deal ...
					// com_token size is 16384 ... it can't read this bastard
					//int is_odd = siv_idx & 1;

					int is_odd = Math_IsOdd(siv_idx);
					if (is_odd == false) {
						// READ MAPNAME
						COM_Parse_Basic (&t); // mapname
						stringlistappend (&sv_intermap_siv_list, com_token);
						if (1) {
							Con_DPrintLinef ("Loading extended Zircon world state: %03d %s", siv_idx, com_token /*mapname*/);
						}
						if (siv_idx == 0)
							Con_DPrintLinef ("Loading extended Zircon world states");
					} else {
						// READ SIV BLOB
						COM_Parse_Basic (&t); // advances cursor past

						baker_string_s *k_siv = BakerString_Create_Malloc ("");
						BakerString_Set (k_siv, siv_strlen, s_siv);

						size_t unbase_datasize;
						unsigned char *data_unbase64_malloc = base64_decode_calloc (k_siv->string, &unbase_datasize); // malloc

						// Baker: s_data_uncompressed_alloc is an entity string like a save game
						char *s_data_uncompressed_malloc = string_zlib_decompress_malloc (data_unbase64_malloc, unbase_datasize, SIV_DECOMPRESS_BUFSIZE_16_MB);

						// Baker: s_data_uncompressed_alloc is a save game blob of text
						stringlistappend (&sv_intermap_siv_list, (char *) s_data_uncompressed_malloc);

						freenull_ (s_data_uncompressed_malloc);
						freenull_ (data_unbase64_malloc);
						BakerString_Destroy_And_Null_It (&k_siv);
					}
				} // sv.intermap_siv
				else if (String_Match(com_token, "sv.buffer")) {
					if (COM_ParseToken_Simple(&t, false, false, true)) {
						i = atoi(com_token);
						if (i >= 0) {
							k = STRINGBUFFER_SAVED_1;
							if (COM_ParseToken_Simple(&t, false, false, true))
								k |= atoi(com_token);
							if (!BufStr_FindCreateReplace(prog, i, k, "string"))
								Con_PrintLinef (CON_ERROR "failed to create stringbuffer %d", i);
						}
						else
							Con_PrintLinef ("unsupported stringbuffer index %d " QUOTED_S, i, com_token);
					}
					else
						Con_PrintLinef ("unexpected end of line when parsing sv.buffer (expected buffer index)");
				}
				else if (String_Match(com_token, "sv.bufstr"))
				{
					if (!COM_ParseToken_Simple(&t, false, false, true))
						Con_PrintLinef ("unexpected end of line when parsing sv.bufstr");
					else
					{
						i = atoi(com_token);
						stringbuffer = BufStr_FindCreateReplace(prog, i, STRINGBUFFER_SAVED_1, "string");
						if (stringbuffer)
						{
							if (COM_ParseToken_Simple(&t, false, false, true))
							{
								k = atoi(com_token);
								if (COM_ParseToken_Simple(&t, false, false, true))
									BufStr_Set(prog, stringbuffer, k, com_token);
								else
									Con_PrintLinef ("unexpected end of line when parsing sv.bufstr (expected string)");
							}
							else
								Con_PrintLinef ("unexpected end of line when parsing sv.bufstr (expected strindex)");
						}
						else
							Con_PrintLinef (CON_ERROR "failed to create stringbuffer %d " QUOTED_S, i, com_token);
					}
				} else if (ServerFlags_Cross_Loadgame_Read_SV_dot_fields(com_token)) {
					// Don't think anything would be required here.

				}
				// skip any trailing text or unrecognized commands
				while (COM_ParseToken_Simple(&t, true, false, true) && String_NOT_Match (com_token, "\n"))
					;
			}
		}
	}

	ServerFlags_Cross_AFTER_Loadgame ("SIV Loadgame");
	// Success
	Mem_Free(text); // AXX1 END

	// Baker: We just read extended savegame data ... update these ...
	if (sv.intermap_startspot[0]) {
		int v_startspot_offset = PRVM_ED_FindGlobalOffset(prog, "startspot");
		if (v_startspot_offset >= 0) {
			PRVM_GLOBALFIELDSTRING(v_startspot_offset) = PRVM_SetEngineString(prog, sv.intermap_startspot);
		} // if
		int v_totaltimeatstart_offset = PRVM_ED_FindGlobalOffset(prog, "totaltimeatstart");
		if (v_totaltimeatstart_offset >= 0) {
			//VectorCopy(v, PRVM_GLOBALFIELDVECTOR(prog->globaldefs[var->globaldefindex[i]].ofs));
			PRVM_GLOBALFIELDFLOAT(v_totaltimeatstart_offset) = sv.intermap_totaltimeatstart;
			//PRVM_GLOBALFIELDVECTOR(v_startspot_offset) = PRVM_SetEngineString(prog, sv.intermap_startorigin);
		} // if
		int v_totaltimeatlastexit_offset = PRVM_ED_FindGlobalOffset(prog, "totaltimeatlastexit");
		if ( v_totaltimeatlastexit_offset >= 0) {
			//VectorCopy(v, PRVM_GLOBALFIELDVECTOR(prog->globaldefs[var->globaldefindex[i]].ofs));
			PRVM_GLOBALFIELDFLOAT(v_totaltimeatstart_offset) = sv.intermap_totaltimeatlastexit;
			//PRVM_GLOBALFIELDVECTOR(v_startspot_offset) = PRVM_SetEngineString(prog, sv.intermap_startorigin);
		} // if
		int v_surplustime_offset = PRVM_ED_FindGlobalOffset(prog, "surplustime");
		if ( v_surplustime_offset >= 0) {
			//VectorCopy(v, PRVM_GLOBALFIELDVECTOR(prog->globaldefs[var->globaldefindex[i]].ofs));
			PRVM_GLOBALFIELDFLOAT(v_surplustime_offset) = sv.intermap_surplustime;
			//PRVM_GLOBALFIELDVECTOR(v_startspot_offset) = PRVM_SetEngineString(prog, sv.intermap_startorigin);
		} // if
		int v_startorigin_offset = PRVM_ED_FindGlobalOffset(prog, "startorigin");
		if (v_startorigin_offset >= 0) {
			//VectorCopy(v, PRVM_GLOBALFIELDVECTOR(prog->globaldefs[var->globaldefindex[i]].ofs));
			VectorCopy(sv.intermap_startorigin, PRVM_GLOBALFIELDVECTOR(v_startorigin_offset) );
			//PRVM_GLOBALFIELDVECTOR(v_startspot_offset) = PRVM_SetEngineString(prog, sv.intermap_startorigin);
		} // if
	}

	// remove all temporary flagged string buffers (ones created with BufStr_FindCreateReplace)
	numbuffers = (int)Mem_ExpandableArray_IndexRange(&prog->stringbuffersarray);
	for (i = 0; i < numbuffers; i ++) {
		if ( (stringbuffer = (prvm_stringbuffer_t *)Mem_ExpandableArray_RecordAtIndex(&prog->stringbuffersarray, i)) )
			if (stringbuffer->flags & STRINGBUFFER_TEMP_128)
				BufStr_Del(prog, stringbuffer);
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: finished");

	// make sure we're connected to loopback
	RELATED_ (CL_EstablishConnection_Local)
	if (sv.active && host.hook.ConnectLocal)
		host.hook.ConnectLocal(); // CL_EstablishConnection_Local
}


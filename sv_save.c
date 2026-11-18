/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "quakedef.h"
#include "prvm_cmds.h"

#include "time.h" // sv.unixtime1970

/*
===============================================================================

LOAD / SAVE GAME

===============================================================================
*/



CALLERS_ (SV_Loadgame_from PRVM_Breakpoint SV_Savegame_f VM_changelevel q_is_intermap_true)

qfile_t			*g_f_save;
static baker_string_s	*g_k_save; // temppool <------------

RELATED_ (SV_Savegame_to)
void Flex_Writef (const char *fmt, ...)
{
	if (g_k_save) {
		VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
		BakerString_Cat_No_Collide (g_k_save, text_slen, text);
		VA_EXPAND_ALLOC_FREE (text);
	}
	else {
#if 1
		// Baker varadic expansion ...
		VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
		/*int result =*/ FS_Print (g_f_save, /*text_slen, */text);
		VA_EXPAND_ALLOC_FREE (text);
#else
		// Baker: This rabbit hole crashes on Linux
		// with large strings .. reasons unknown and
		// easier to solve than explore
		int result;
		va_list args;

		va_start (args, fmt);
		result = FS_VPrintf (g_f_save, fmt, args);
		va_end (args);
#endif
	}
}

RELATED_ (SV_Changelevel2_f)

// Baker: We get a siv_string or a save name (name_or_null) - never both
void SV_Savegame_to (prvm_prog_t *prog, char **p_siv_string, ccs *name_or_null, int is_intermap_siv_write, float totaltimeatlastexit_to_write, char **p_fastsave_string)
{
	int		i;

	// first we have to figure out if this can be saved in 64 lightstyles
	// (for Quake compatibility)
	int lightstyles = 64;
	for (i = 64 ; i < MAX_LIGHTSTYLES_256; i ++)
		if (sv.lightstyles[i][0])
			lightstyles = i + 1;

	qbool isserver = prog == SVVM_prog;
	qbool is_autosave = name_or_null && String_Match (AUTOSAVE_NAME_AUTOSAVE0_SAV, name_or_null);

	// Baker: Cleanup
	if (g_k_save) {
		// Baker: This could happen if somehow error occurs that exits function
		BakerString_Destroy_And_Null_It (&g_k_save); // nulls g_k_save
	}

	if (g_f_save) {
		FS_Close (g_f_save); g_f_save = NULL;
	}

	if (p_siv_string) {
		g_k_save = BakerString_Create_Malloc ("");
	} else if (p_fastsave_string) {
		// Start ?
		g_k_save = BakerString_Create_Malloc_With_MegaBytes (2 /*MB*/ );
	} else {
		// Save To File Start
		if (is_autosave == false)
			Con_PrintLinef ("Saving game to %s...", name_or_null);
		g_f_save = FS_OpenRealFile(name_or_null, "wb", fs_quiet_FALSE); //  WRITE-EON savegameto ... DONE
		if (!g_f_save) {
			Con_PrintLinef ("ERROR: couldn't open.");
			return;
		}
	}

	Flex_Writef ("%d" NEWLINE, SAVEGAME_VERSION_5);

	if (is_intermap_siv_write) {
		// Need to save time and what else?
		Flex_Writef ("%s" NEWLINE, sv.name);
		Flex_Writef ("%f" NEWLINE, sv.time); // YES ... but this means the time accum is wrong
		// We need to subtract out totaltimeatlastexit from totaltimeatstart
		Flex_Writef ("%f" NEWLINE, totaltimeatlastexit_to_write); // totaltimeatlastexit_from_loadgame
		Flex_Writef ("%d" NEWLINE, svs.maxclients);
		goto intermap_no_comment_no_players;
	}

	char	comment[SAVEGAME_COMMENT_LENGTH_39+1];
	memset(comment, 0, sizeof(comment));
	if (isserver)
		dpsnprintf (comment, sizeof(comment), "%-21.21s kills:%3d/%3d", PRVM_GetString(prog, PRVM_serveredictstring(prog->edicts, message)), (int)PRVM_serverglobalfloat(killed_monsters), (int)PRVM_serverglobalfloat(total_monsters));
	else
		dpsnprintf (comment, sizeof(comment), "(crash dump of %s progs)", prog->name);
	// convert space to _ to make stdio happy
	// LadyHavoc: convert control characters to _ as well
	for (i = 0 ; i < SAVEGAME_COMMENT_LENGTH_39; i ++)
		if (ISWHITESPACEORCONTROL(comment[i]))
			comment[i] = '_';
	comment[SAVEGAME_COMMENT_LENGTH_39] = '\0';

	Flex_Writef ("%s" NEWLINE, comment);
	if (isserver)
	{
		for (i = 0 ; i < NUM_SPAWN_PARMS_16; i ++) // PARM_EXPAND SV_SAVEGAME_TO writes CL DATA FOR PLAYER 1
			Flex_Writef ("%f" NEWLINE, svs.clients[0].spawn_parms[i]);
		Flex_Writef ("%d" NEWLINE, current_skill);
		Flex_Writef ("%s" NEWLINE, sv.name);
		Flex_Writef ("%f" NEWLINE, sv.time);
	}
	else
	{
		// Baker: This is saving "client" CSQC ... weird
		for (i = 0 ; i < NUM_SPAWN_PARMS_16; i ++) // PARM_EXPAND SV_SAVESPAWNPARMS -- THIS IS CSQC OR MENU DUMP .TXT
			Flex_Writef ("(dummy)" NEWLINE);
		Flex_Writef ("%d" NEWLINE, 0);
		Flex_Writef ("%s" NEWLINE, "(dummy)");
		Flex_Writef ("%f" NEWLINE, host.realtime);
	}

intermap_no_comment_no_players:

	// write the light styles
	for (i = 0; i < lightstyles; i ++) {
		if (isserver && sv.lightstyles[i][0])
			Flex_Writef ("%s" NEWLINE, sv.lightstyles[i]);
		else
			Flex_Writef ("m" NEWLINE); // Baker: This was FS_Print no EFF
	}

	if (is_intermap_siv_write) {
#if 1 // APRIL 15 2026 - Just lets see ...
		// Baker: Intermap does not do globals. All relevant data must be stored in entities alone.
		// March 28 2026 - Cherry picked specific globals now write
		// sv_siv_globals_write_prefix

		// Baker: March 28 2026 - Intermap now does a thin list of globals
		PRVM_ED_WriteGlobals_Intermap (prog, g_f_save, sv_siv_globals_write_prefix.string);
#endif

		// Baker: We need to write the world ...
		int world_zero_0 = 0;
		Flex_Writef ("// edict %d" NEWLINE, world_zero_0);
		PRVM_ED_Write (prog, g_f_save, PRVM_EDICT_NUM(world_zero_0));
		goto intermap_no_globals;
	}

	PRVM_ED_WriteGlobals (prog, g_f_save);

intermap_no_globals:
	if (0 && is_intermap_siv_write) {
		// Write out nothing burgers for 1 to svs.maxclients
		for (i = 1; i < (svs.maxclients + 1); i ++) {
			Flex_Writef ("// edict %d" NEWLINE, i);
			Flex_Writef ("{" NEWLINE);
			Flex_Writef ("}" NEWLINE);
			//Con_Printf ("edict %d...\n", i);
//			PRVM_ED_Write (prog, f, PRVM_EDICT_NUM(i));
		}
	}


	// Baker: Intermap .. skip players, they are not part of persistent gamestate
//	int start_edict = is_intermap_siv_write ? 1 /*(svs.maxclients + 1)*/ : 0;

	// March 28 2026 -- (svs.maxclients + 1) will start at 2.
	//int start_edict = is_intermap_siv_write ? (svs.maxclients + 1) : 0;  March 28 2026 - Doesn't work.
	int start_edict = is_intermap_siv_write ? 1 : 0;

	for (i = start_edict /*0*/; i < prog->num_edicts; i ++) {
		Flex_Writef ("// edict %d" NEWLINE, i);
		//Con_Printf ("edict %d...\n", i);
		PRVM_ED_Write (prog, g_f_save, PRVM_EDICT_NUM(i));
	}

#if 1
	Flex_Writef ("/*" NEWLINE);
	Flex_Writef ("// DarkPlaces extended savegame" NEWLINE);
	// darkplaces extension - extra lightstyles, support for color lightstyles
	for (i = 0 ; i < MAX_LIGHTSTYLES_256; i ++)
		if (isserver && sv.lightstyles[i][0])
			Flex_Writef ("sv.lightstyles %d %s" NEWLINE, i, sv.lightstyles[i]);

	// darkplaces extension - model precaches
	for (i = 1 ; i < MAX_MODELS_8192; i ++)
		if (sv.model_precache[i][0])
			Flex_Writef ("sv.model_precache %d %s" NEWLINE, i, sv.model_precache[i]);

	// darkplaces extension - sound precaches
	for (i = 1 ; i < MAX_SOUNDS_4096 ; i ++)
		if (sv.sound_precache[i][0])
			Flex_Writef ("sv.sound_precache %d %s" NEWLINE, i, sv.sound_precache[i]);

	// Baker: odd is normal for the first write.

	float CDAudio_GetPosition (void);
	float cdtrack_seconds_into = CDAudio_GetPosition();
	if (cdtrack_seconds_into != -1) {
		Flex_Writef ("sv.cdtrack_seconds_into %g" NEWLINE, cdtrack_seconds_into);
	}

	if (is_intermap_siv_write) {
		// Baker: Intermap does not do stringbuffers because it does not do globals.

		// Baker: Intermap does not do server flags because it is entities only
		// Intermap does not do siv list because it is not a save game
		goto intermap_no_siv_write_no_stringbuffers;
	}

	if (Math_IsOdd(sv_intermap_siv_list.numstrings)) {
		Sys_Error (CON_WARN "sv_intermap_siv_list.numstrings is odd = %d", sv_intermap_siv_list.numstrings);
	}

	// Zircon intermap - If we are here we are writing a QUAKE.SAV file, we put intermap .SIV in as fields
	// We ZLIB compress the data then we base64 encode it to make sure it is pure text

	// TYPE           IDX   STRLEN		DATA
	// sv.intermap_siv 0	0			start				// Even is map name
	// sv.intermap_siv 1	850505		DFDKDFLDKFDF_		// Odd is map .siv data (entities only)
	// sv.intermap_siv 2	0			e1m1
	// sv.intermap_siv 3	34343		DFOIDFFDL

	for (i = 0 ; i < sv_intermap_siv_list.numstrings; i += 2) {
		// .SIV data - Intermap saved game
		const char *sxy_map = sv_intermap_siv_list.strings[i + 0];
		const char *sxy_siv = sv_intermap_siv_list.strings[i + 1];
		RELATED_ (FS_Base64ClipboardCompressed_f FS_Base64ClipboardDeCompressed_f)

		size_t data_compressed_size = 0;
		unsigned char *data_compressed = string_zlib_compress_malloc (sxy_siv, &data_compressed_size, SIV_DECOMPRESS_BUFSIZE_16_MB);
		char *s_base64_alloc = base64_encode_calloc (data_compressed, data_compressed_size, q_reply_len_NULL); // malloc
		int base64_slen = (int)strlen(s_base64_alloc);
		Flex_Writef ("sv.intermap_siv %d %d %s" NEWLINE, i + 0, 0, sxy_map);
		Flex_Writef ("sv.intermap_siv %d %d %s" NEWLINE, i + 1, (int)base64_slen, s_base64_alloc);

		free (s_base64_alloc);
		free (data_compressed);
	} // for

	if (sv.intermap_startspot[0])
		Flex_Writef ("sv.startspot %s" NEWLINE, sv.intermap_startspot);

	if (sv.intermap_totaltimeatstart)
		Flex_Writef ("sv.totaltimeatstart %f" NEWLINE, sv.intermap_totaltimeatstart);

	if (sv.intermap_surplustime)
		Flex_Writef ("sv.surplustime %f" NEWLINE, sv.intermap_surplustime);

	if (sv.intermap_totaltimeatlastexit)
		Flex_Writef ("sv.totaltimeatlastexit %f" NEWLINE, sv.intermap_totaltimeatlastexit);

	if (Vector3_IsZeros (sv.intermap_startorigin) == false)
		Flex_Writef ("sv.startorigin " VECTOR3_5d1F NEWLINE, VECTOR3_SEND(sv.intermap_startorigin));

	if (svs.serverflags) // SAVE GAME TO - WRITE sv.serverflags (THE VALUE FROM THE START OF THE LEVEL!)
		Flex_Writef ("sv.serverflags %d" NEWLINE, svs.serverflags); // SAVE GAME TO - WRITE sv.serverflags

	ServerFlags_Cross_Savegame_Write_SV_dot_fields (); // Write svs.svs.server_cross_globals_list and friends

	if (1) {
		time_t unix_seconds_since_1970 = time(NULL);
		double dtime = unix_seconds_since_1970;
		Flex_Writef ("sv.unixtime1970 %f" NEWLINE, dtime);
	}

	if (cl.islocalgame && sv_save_screenshots.integer && Have_JPEG() ) {
		// free (s_base64_alloc);
		if (cls.state == ca_connected && cls.signon == SIGNONS_4 && cl.worldmodel && r_refdef.scene.worldentity) {
			char *s_base64_alloc = Screenshot_To_Jpeg_String_Malloc_512_320 ();
			if (!s_base64_alloc) {
				Con_PrintLinef (CON_WARN "Screenshot_To_Jpeg_String_Malloc_512_320 failed" NEWLINE);
			}
			else {
				//Sys_PrintToTerminal ("Screenshot_To_Jpeg_String_Malloc_512_320 OK!" NEWLINE);
				int base64_slen = (int)strlen(s_base64_alloc);

				//Sys_PrintToTerminal (va32 ("strlen is %d" NEWLINE, base64_slen));

				// Baker: Warning DarkPlaces can only variadic print 16384 at a time!
				Flex_Writef ("sv.screenshot %d %s" NEWLINE, (int)base64_slen, s_base64_alloc);
				//Sys_PrintToTerminal ("About to free" NEWLINE);
				free (s_base64_alloc);
			}
			RELATED_ (SCR_ScreenShot_f  R_Envmap_f SCR_ScreenShot)
		} // if connected with rendering data available.
	}

	// Baker: NOT intermap scenario where intermap is active (real save of intermap)
	//if (
	//	int siv_idx = stringlist_find_index (&sv_siv_virtual_files_list, sv.worldbasename);
	//	if (siv_idx == not_found_neg1) {
	//		// ADD
	//	} else {
	//		// UPDATE / STOMP
	//	}

	// darkplaces extension - save buffers
	int numbuffers = (int)Mem_ExpandableArray_IndexRange(&prog->stringbuffersarray);
	int k;
	char	line[MAX_INPUTLINE_16384];
	char	*s;
	for (i = 0; i < numbuffers; i++) {
		prvm_stringbuffer_t *stringbuffer = (prvm_stringbuffer_t*) Mem_ExpandableArray_RecordAtIndex(&prog->stringbuffersarray, i);
		if (stringbuffer && (stringbuffer->flags & STRINGBUFFER_SAVED_1)) {
			Flex_Writef ("sv.buffer %d %d \"string\"" NEWLINE, i,
				stringbuffer->flags & STRINGBUFFER_QCFLAGS_1);
			for(k = 0; k < stringbuffer->num_strings; k++)
			{
				int jj;
				if (!stringbuffer->strings[k])
					continue;
				// Parse the string a bit to turn special characters
				// (like newline, specifically) into escape codes
				s = stringbuffer->strings[k];
				for (jj = 0;jj < (int)sizeof(line) - 2 && *s;)
				{
					if (*s == '\n')
					{
						line[jj++] = '\\';
						line[jj++] = 'n';
					}
					else if (*s == '\r')
					{
						line[jj++] = '\\';
						line[jj++] = 'r';
					}
					else if (*s == '\\')
					{
						line[jj++] = '\\';
						line[jj++] = '\\';
					}
					else if (*s == '"')
					{
						line[jj++] = '\\';
						line[jj++] = '"';
					}
					else
						line[jj++] = *s;
					s++;
				}
				line[jj] = '\0';
				Flex_Writef ("sv.bufstr %d %d " QUOTED_S NEWLINE, i, k, line);
			}
		}
	}

intermap_no_siv_write_no_stringbuffers:
	Flex_Writef ("*/" NEWLINE);
#endif

	if (p_fastsave_string) {
		(*p_fastsave_string) = Mem_strdup (tempmempool, g_k_save->string);
		BakerString_Destroy_And_Null_It(&g_k_save); // Free it.
	} else if (p_siv_string) {
		(*p_siv_string) = Mem_strdup (tempmempool, g_k_save->string);
		BakerString_Destroy_And_Null_It(&g_k_save); // Free it.
	} else {
		FS_Close (g_f_save); g_f_save = NULL;
	}

	if (is_autosave == false && p_fastsave_string == NULL)
		Con_PrintLinef ("done.");
}

static qbool SV_CanSave(qbool is_autosave)
{
	prvm_prog_t *prog = SVVM_prog;
	if (SV_IsLocalServer() == 1)
	{
		// singleplayer checks
		// FIXME: This only checks if the first player is dead?
		if ((svs.clients[0].active && PRVM_serveredictfloat(svs.clients[0].edict, deadflag)))
		{
			if (is_autosave == false)
				Con_PrintLinef ("Can't savegame with a dead player");
			return false;
		}

		if (host.hook.CL_Intermission && host.hook.CL_Intermission())
		{
			if (is_autosave == false)
				Con_PrintLinef ("Can't save in intermission.");
			return false;
		}
	}
	else {
		if (is_autosave == false)
			Con_PrintLinef (CON_WARN "Warning: saving a multiplayer game may have strange results when restored (to properly resume, all players must join in the same player slots and then the game can be reloaded).");
	}
	return true;
}


RELATED_ (SV_Savegame_to stringlist_t)
void SV_Siv_f (cmd_state_t *cmd)
{
	if (!sv.active) {
		Con_PrintLinef ("Server not active");
		return;
	}

	prvm_prog_t *prog = SVVM_prog;

	if (cmd_argc == 1) {
		// List them
		Con_PrintLinef ("# SIVS = %d", sv_intermap_siv_list.numstrings);
		int have_intermap = 0;
		int v_enable_intermap_offset = PRVM_ED_FindGlobalOffset(prog, "enable_intermap");
		if (v_enable_intermap_offset >= 0) {
			have_intermap = PRVM_GLOBALFIELDFLOAT(v_enable_intermap_offset);
		} // if

		Con_PrintLinef ("svs.serverflags  = %d", svs.serverflags);
		Con_PrintLinef ("sv.time          = %f", sv.time);
		Con_PrintLinef ("sv.surplustime   = %f", sv.intermap_surplustime);
		Con_PrintLinef ("TotalTimeAtStart = %f", sv.intermap_totaltimeatstart);
		Con_PrintLinef ("================");
		Con_PrintLinef ("                 = %f", (sv.time - sv.intermap_surplustime) + sv.intermap_totaltimeatstart );
		Con_PrintLineEmpty ();//Con_PrintLinef ("");
		Con_PrintLinef ("enable_intermap  = %d", have_intermap);
		Con_PrintLinef ("Startspot        = " QUOTED_S, sv.intermap_startspot );
		Con_PrintLinef ("Startorigin      = " VECTOR3_5d1F, VECTOR3_SEND(sv.intermap_startorigin) );
		Con_PrintLineEmpty ();//Con_PrintLinef ("");
		Con_PrintLinef ("sv.was_intermap_loaded_from_siv = %d", sv.was_intermap_loaded_from_siv);
		Con_PrintLinef ("map last exit    = %f", sv.intermap_totaltimeatlastexit);
		Con_PrintLineEmpty ();//Con_PrintLinef ("");

		Con_PrintLinef ("Index  Map  Data_Strlen");
		for (int idx = 0 ; idx < sv_intermap_siv_list.numstrings; idx += 2) {
			// .SIV data - Intermap saved game
			const char *sxy_map = sv_intermap_siv_list.strings[idx + 0];
			const char *sxy_siv = sv_intermap_siv_list.strings[idx + 1];
			Con_PrintLinef ("%03d " S_FMT_RIGHT_PAD_16 " %07d", idx, sxy_map, (int)strlen(sxy_siv) );
		} // for

#ifdef _DEBUG
		stringlist_condump_with_linenums (&svs.server_cross_globals_list);
#endif

		// Do usage.
		if (cmd_argc == 1) {
			Con_PrintLinef ("Usage: %s ", cmd_argv_command_0);
			Con_PrintLinef ("Usage: %s [siv slot #] -- Export a siv slot to clipboard", cmd_argv_command_0);
		}

		return;
	}

	ccs *s_clipboard_num = cmd_argv_parm1;
	int idx = atoi (s_clipboard_num);
	if (in_range_beyond (0, idx, sv_intermap_siv_list.numstrings) == false) {
		Con_PrintLinef ("Requested %d, valid range is 0 to number stored sivs = %d", idx, sv_intermap_siv_list.numstrings - 1);
		return;
	}

	if (Math_IsOdd (idx)) {
		Con_PrintLinef ("Only even indexs allowed, reducing to even value" NEWLINE);
		Flag_Remove_From (idx, 1); // Even-ize it.
	}

	const char *sxy_map = sv_intermap_siv_list.strings[idx + 0];
	const char *sxy_siv = sv_intermap_siv_list.strings[idx + 1];
	size_t siv_strlen = strlen(sxy_siv);

	Clipboard_Set_Text (sxy_siv); // AUTH: siv cmd asking for clipboard copy
	Con_PrintLinef ("SIV idx %d (%s) copied to clipboard %u chars", idx, sxy_map, (int)siv_strlen);
}

/*
===============
SV_Savegame_f
===============
*/
RELATED_ (SV_Savegame_to)
void SV_Savegame_f (cmd_state_t *cmd)
{
	prvm_prog_t *prog = SVVM_prog;
	//double dirtytime0 = Sys_DirtyTime ();


	if (!sv.active) {
		Con_PrintLinef ("Can't save - no server running.");
		return;
	}

	if (Cmd_Argc(cmd) != 2) {
		Con_PrintLinef ("save <savename> : save a game");
		return;
	}

	const char *s_savename = Cmd_Argv(cmd, 1);
	if (String_Contains (s_savename, "..")) {
		Con_PrintLinef ("Relative pathnames are not allowed.");
		return;
	}

	qbool is_autosave = String_Match (AUTOSAVE_NAME_AUTOSAVE0_SAV, s_savename);
	if (SV_CanSave(is_autosave) == false) // Baker: dead player check, intermission check, multiplayer save warning
		return;

	char	name_with_dot_sav[MAX_QPATH_128];
	c_strlcpy (name_with_dot_sav, s_savename);

	FS_DefaultExtension (name_with_dot_sav, ".sav", sizeof(name_with_dot_sav) );

	char *s_sav_zalloc = NULL; // Zalloc string

	SV_Savegame_to (prog, q_siv_savestring_NULL, q_savefile_NULL, q_is_siv_write_false, /*totaltimeatlastexit*/ 0, &s_sav_zalloc);
	//Clipboard_Set_Text (s_sav_zalloc);
	qbool ok = FS_WriteFileStringQuick (name_with_dot_sav, s_sav_zalloc);

	if (ok) {
		if (is_autosave == false) {
			//double dirtytime1 = Sys_DirtyTime ();
			//double elapsed = dirtytime1 - dirtytime0; //cl.warped.warp_realtime_start;
			Con_PrintLinef ("Saved game to %s", name_with_dot_sav); //, elapsed);
		}
	} else {
		Con_PrintLinef ("Failed to write %s.", name_with_dot_sav);
	}

	Mem_FreeNull_ (s_sav_zalloc);
}

/*
===============
SV_Loadgame_f
===============
*/

// Baker: in ... "start e1m1 e1m2" .. the list of maps we have .siv data for sv_siv_map_list
void Intermap_List_Parse (char *s)
{
	// Wants to call this
	// Baker: Parse out sv_siv_map_list using stringlist_from_delim (space delimiter)
}

// Baker: The blob we jam into an intermap save game .. we somehow parse it out to sv_siv_virtual_files
void Intermap_List_Saves_Parse (int index, char *s)
{
	// Wants to call this
	// Baker: Parse out sv_siv_map_list using stringlist_from_delim (space delimiter)
}


RELATED_ (VM_changelevel, SV_Changelevel2_f SV_SpawnServer)
CALLERS_ (SV_Loadgame_f) // exclusive

// Baker: Feb 13 2026 - is_intermap has only 1 caller SV_Loadgame_f and is_intermap is ALWAYS false
CALLEES_ (SV_SpawnServer 70 lines down)
#include "sv_loadgame_from.c.h"

/*
===============
SV_Loadgame_f
===============
*/
void SV_Loadgame_f(cmd_state_t *cmd)
{
	if (Cmd_Argc(cmd) != 2) {
		Con_PrintLinef ("load <savename> : load a game");
		return;
	}

	char filename[MAX_QPATH_128];
	c_strlcpy (filename, Cmd_Argv(cmd, 1));
	FS_DefaultExtension (filename, ".sav", sizeof (filename));

	Con_PrintLinef ("Loading game from %s...", filename);

	Con_CloseConsole_If_Client(); // Baker r1003: close console for map/load/etc.

	RELATED_ (CL_DisconnectEx)
	if (host.hook.Disconnect)
		host.hook.Disconnect (q_is_kicked_false, q_disconnect_message_NULL); // CL_DisconnectEx

// Baker: Make sure maxplayers command propogates
// Old behavior: "maxplayers 4" ==> Loadgame ==> svs.maxclients == 1
// Previously: game inventory; load nicex (where maxplayers is in default.cfg)
//             the maxplayers number would never take effect.
//             With the above command.
	SV_Shutdown();

	qbool is_forced = sv_maxplayers_forced.integer > 0;
	if (svs.maxclients != svs.maxclients_next || is_forced) {
		svs.maxclients = svs.maxclients_next;
		if (is_forced)
			svs.maxclients = svs.maxclients_next = bound(1, sv_maxplayers_forced.integer, MAX_SCOREBOARD_255);

		if (svs.clients)
			Mem_Free(svs.clients);
		svs.clients = (client_t *)Mem_Alloc(sv_mempool, sizeof(client_t) * svs.maxclients);
	}

	if (host.hook.ToggleMenu)
		host.hook.ToggleMenu();

	memset (&cls.demos, 0, sizeof(cls.demos)); cls.demonum = STARTDEMO_STOP_LOOP_NEG1; // stop demo loop in case this fails

	//SV_Loadgame_from (cmd, filename, q_is_siv_write_false);
	SV_Loadgame_from (cmd, filename);
}


RELATED_ (VM_changelevel SV_Changelevel2_f)
// sv.time = time_from_loadgame; line 1264 +/-
CALLERS_ (SV_SpawnServer_Intermap_SIV_Is_Ok)
int SV_Loadgame_Intermap_Do_Ents (const char *s_load_game_contents)
{
	prvm_prog_t *prog = SVVM_prog;

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading version");

	// version
	const char *t = s_load_game_contents; // Baker: This is the cursor

	COM_Parse_Basic (&t);
	int version = atoi(com_token);
	if (version != SAVEGAME_VERSION_5) {
		Con_PrintLinef ("Savegame is version %d, not %d", version, SAVEGAME_VERSION_5);
		//Mem_Free (s_load_game_contents);
		return false; // NOT OK!
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading description");


	RELATED_ (SV_Changelevel2_f SV_Savegame_to SV_Loadgame_f)

	//FS_Printf (f, "%d" NEWLINE, SAVEGAME_VERSION);

	//if (is_intermap) {
	//	// Need to save time and what else?
	//	FS_Printf(f, "%s" NEWLINE, sv.name);
	//	FS_Printf(f, "%f" NEWLINE, sv.time);


	// mapname
	char mapname[MAX_QPATH_128];
	COM_Parse_Basic (&t);
	c_strlcpy (mapname, com_token);

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading time");

	// time
	COM_Parse_Basic (&t);
	float time_from_loadgame = atof(com_token); // sv.time! SV_Loadgame_Intermap_Do_Ents

	// totaltimeatlastexit
	COM_Parse_Basic (&t);
	float totaltimeatlastexit_from_loadgame = atof(com_token); // sv.time!

	// svs.maxclients
	COM_Parse_Basic (&t);
	int svs_maxclients_from_loadgame = atoi(com_token);

	if (svs.maxclients != svs_maxclients_from_loadgame) {
		Con_PrintLinef ("Savegame maxplayers is %d, current maxclients is %d", svs_maxclients_from_loadgame, svs.maxclients);
		//Mem_Free (s_load_game_contents);
		return false; // NOT OK!
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: spawning server");

	// Baker: Intermap ... hmmm .. we do not want entities from the bsp
	// Not even entity 0

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: loading light styles");

// load the light styles

	const char *start;
	int i;
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
	//#pragma message (".siv see if this is ok")
	World_UnlinkAll(&sv.world);

// load the edicts out of the savegame file
#if 1 // MOVE DOWN BAKER
	// -1 is the globals .. we do NOT save any globals
	//int entnum = 0;
	qbool have_globals = false;
	int entnum = 0; // March 28 2026
	if (t[0] == CHAR_NEWLINE_10 && t[1] == '{') {
		have_globals = true;
		entnum = -1;
	}
#endif

	prvm_edict_t *ent;
	const char *end = t;
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
			//Mem_Free(s_load_game_contents);
			Host_Error_Line ("First token isn't a brace");
		}

#if 1 // March 28 2026
		if (have_globals && entnum == -1) {
			if (developer_entityparsing.integer)
				Con_PrintLinef ("SV_Loadgame_f: loading globals");

			// parse the global vars
			PRVM_ED_ParseGlobals (prog, start);
		}
		else
#endif
		{
			// parse an edict
			if (entnum >= MAX_EDICTS_32768) {
				//Mem_Free(s_load_game_contents);
				Host_Error_Line ("Host_PerformLoadGame: too many edicts in save file (reached MAX_EDICTS_32768 %d)", MAX_EDICTS_32768);
			}
			while (entnum >= prog->max_edicts)
				PRVM_MEM_IncreaseEdicts(prog);
			ent = PRVM_EDICT_NUM(entnum);
			memset(ent->fields.fp, 0, prog->entityfields * sizeof(prvm_vec_t));
			ent->free = false;

			if (developer_entityparsing.integer)
				Con_PrintLinef ("SV_Loadgame_f: loading edict %d", entnum);

			PRVM_ED_ParseEdict (prog, start, ent, q_is_saveload_true); // ITK #5

			// link it into the bsp tree
			if (!ent->free) {
				// Baker: On SIV load all star '*' brush models
				// ... 1) precache_model
				// ... 2) setmodel
				// ... 3) setorigin
				// Fixes SIV return rotating doors collision
				// issues on return
				RELATED_ (VM_SV_setmodel)
#ifdef _DEBUG
				ccs *s_classname = PRVM_GetString(prog, PRVM_alledictstring(ent, classname));
				int entitynumber = PRVM_NUM_FOR_EDICT(ent);
#endif // _DEBUG
				ccs *s_modelname = PRVM_GetString(prog, PRVM_alledictstring(ent, model));

				if (s_modelname && s_modelname[0] == '*') {
					VMX_SV_precache_model	(SVVM_prog, s_modelname);
					VMX_SV_setmodel			(SVVM_prog, ent, s_modelname);
					VMX_SV_setorigin		(SVVM_prog, ent);
				}
				SV_LinkEdict(ent);
			} else {
				// happens? (YES - HAPPENS Once at very ending }  Apr 16 2026)
			}
		} // ent != -1

		end = t;

#if 0  // Baker: We do world, then we do svs.maxclients + 1
		// world	0
		// player	1
		//
		if (entnum == 0)
			entnum =
#endif
		entnum ++;
	} // end while (1)

	prog->num_edicts = entnum;
	sv.time = time_from_loadgame; // SV_Loadgame_Intermap_Do_Ents
	sv.intermap_totaltimeatlastexit = totaltimeatlastexit_from_loadgame;
	// totaltimeatlastexit

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: skipping until extended data");

	// read extended data if present
	// the extended data is stored inside a /* */ comment block, which the
	// parser intentionally skips, so we have to check for it manually here

	int k, numbuffers;
	prvm_stringbuffer_t *stringbuffer;

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
					//#pragma message("Intermap SIV load")
				}
				else if (String_Match(com_token, "sv.sound_precache")) {
					COM_Parse_Basic (&t);
					i = atoi(com_token);
					COM_Parse_Basic (&t);
					if (i >= 0 && i < MAX_SOUNDS_4096)
						strlcpy(sv.sound_precache[i], com_token, sizeof(sv.sound_precache[i]));
					else
						Con_PrintLinef ("unsupported sound %d " QUOTED_S, i, com_token);
				}
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
				}
				// skip any trailing text or unrecognized commands
				while (COM_ParseToken_Simple(&t, true, false, true) && String_NOT_Match (com_token, "\n"))
					;
			}
		}
	}
	// Success
	//Mem_Free(s_load_game_contents); // AXX1 END

	// remove all temporary flagged string buffers (ones created with BufStr_FindCreateReplace)
	numbuffers = (int)Mem_ExpandableArray_IndexRange(&prog->stringbuffersarray);
	for (i = 0; i < numbuffers; i ++) {
		if ( (stringbuffer = (prvm_stringbuffer_t *)Mem_ExpandableArray_RecordAtIndex(&prog->stringbuffersarray, i)) )
			if (stringbuffer->flags & STRINGBUFFER_TEMP_128)
				BufStr_Del(prog, stringbuffer);
	}

	if (developer_entityparsing.integer)
		Con_PrintLinef ("SV_Loadgame_f: finished");

#if 0 // Baker: This gets done immediately after function returns in the caller ...
	// make sure we're connected to loopback
	RELATED_ (CL_EstablishConnection_Local)
	if (sv.active && host.hook.ConnectLocal)
		host.hook.ConnectLocal(); // CL_EstablishConnection_Local
#endif
	return true; // OK!
}

RELATED_ (VM_changelevel SV_Changelevel2_f)


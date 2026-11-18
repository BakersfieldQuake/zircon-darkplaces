// sv_cross.c.h

cvar_t sv_siv_globals_write_prefix = {CF_SERVER, "sv_siv_globals_write_prefix", "", "Server globals with this prefix will write to SIV [Zircon]"};
cvar_t sv_changelevel_preserve_globals_prefix = {CF_SERVER, "sv_changelevel_preserve_globals_prefix", "", "Server globals with this prefix will persist across changelevel [Zircon]"};
cvar_t sv_changelevel_preserve_globals_stringlist_suffix = {CF_SERVER, "sv_changelevel_preserve_globals_stringlist_suffix", "", "Server globals with this suffix will be considered stringlists and their contents will persist across changelevel [Zircon]"};
cvar_t sv_savegame_all_variables = {CF_SERVER, "sv_savegame_all_variables", "0", "Save game shall save all globals and fields except for entity float ending with _x _y _z [Zircon]"};

RELATED_ (SV_Shutdown)
cvar_t sv_maxplayers_forced = {CF_SERVER, "sv_maxplayers_forced", "0", "Forces maxplayers to default to this number [Zircon]"};

#include "sv_cross_overhead.c.h"

RELATED_ (x = svs.serverflags svs.server_cross_fields_list)

// We have crossed over a level.  Transfer the globals from storage to progs.
// QuakeC will have to conditionally NOT buf_create/
// STRINGBUFFER_SAVED_1
CALLERS_ (SV_SpawnServer)
void ServerFlags_Cross_Changelevel_Restore (ccs *reason) // READ
{
#ifdef _DEBUG
	Con_PrintLinef ("Cross_Changelevel_Restore reason = %s: %d", reason, svs.server_cross_globals_list.numstrings);
	stringlist_condump_with_linenums (&svs.server_cross_globals_list);
#endif

	prvm_prog_t *prog = SVVM_prog;
	// TAKE THE PERSISTENT DATA STORED IN
	// 1. svs.server_cross_globals_list
	// and set the QuakeC global variable strings
	ccs *s_stringlist_suffix = sv_changelevel_preserve_globals_stringlist_suffix.string;
	for (int idx = 0; idx < svs.server_cross_globals_list.numstrings; idx += 2) {
		// READ
		// READ
		// READ
		// Work the pairs
		ccs *key = svs.server_cross_globals_list.strings[idx + 0];
		ccs *val = svs.server_cross_globals_list.strings[idx + 1];
		RELATED_ (PRVM_ED_anyset_f)

		qbool is_stringlist = (s_stringlist_suffix[0] && String_Ends_With (key, s_stringlist_suffix));

		if (is_stringlist && atoi(val) == 0) {
			// Uninitialized.
			Con_PrintLinef (CON_BRONZE "An uninitialized string buffer for %s", key);
			is_stringlist = false;
		}

		qbool ok;

		if (is_stringlist) {
			int svs_stringlist_index0 = atoi(val);
			int svs_stringlist_index = UNPLUS1(svs_stringlist_index0);
			stringlist_t *plist = &svs.server_cross_globals_strlists[svs_stringlist_index];

			// Create new buffernum
			int bufnum = not_found_neg1;

			RELATED_ (VM_buf_create)
			stringbuffer_create_neverzero (prog, &bufnum); // This is fuct April 15 2026
			stringbuffernum_append_stringlist (plist, prog, bufnum);

			va_super (s_bufnum, 128, "%d", bufnum);
			ok = Set_A_Global_Print_Errors ("server", key, s_bufnum);
		} else {
			ok = Set_A_Global_Print_Errors ("server", key, val);
		}
	} // for

#ifdef _DEBUG
	// Catch exit
	int j = 5;
#endif
}

RELATED_ (lw_ssqc_stringlist_13)



// Clear the list on "map"
CALLERS_ (SV_Map_f ServerFlags_Cross_Changelevel_Before_Save)
void ServerFlags_Cross_Map_Clear (void)
{
	// Erase everything
	// 1. Erase all the string lists.
	// 2. Erase the array of strings
	sCross_Clear ();
}

// Baker: Does not do ev_function right now
CALLERS_ (SV_SaveSpawnparms)
void ServerFlags_Cross_Changelevel_Before_Save (ccs *reason) // WRITE
{
#ifdef _DEBUG
	Con_PrintLinef ("Cross_Changelevel_Before_Save (WRITE) reason = %s: %d", reason, svs.server_cross_globals_list.numstrings);
	stringlist_condump_with_linenums (&svs.server_cross_globals_list);
#endif

	// SV_SaveSpawnparms before: Clear the globals and then populate our svs cross data
	//
	// Find all the globals and accumulate svs.server_cross_fields_list.numstrings pairs
	sCross_Clear ();

	ccs *prefix = sv_changelevel_preserve_globals_prefix.string;

	// If no prefix, get out.
	if (!prefix[0])
		return;

	ccs *s_stringlist_suffix = sv_changelevel_preserve_globals_stringlist_suffix.string;

	RELATED_ (PRVM_ED_EdictGet_Fly_Lite)
	// Iterate

	prvm_prog_t *prog = SVVM_prog;
	// This includes intermediate variables and such ... list can be huge.
	for (int idx = 0; idx < prog->numglobaldefs; idx ++) {
		// WRITE
		// WRITE
		// WRITE
		char valuebuf[VM_STRINGTEMP_LENGTH_16384];
		mdef_t		*global			= &prog->globaldefs[idx];
		int			vartype			= Flag_Remove(global->type, DEF_SAVEGLOBAL);
		ccs			*s_vartype		= VarType_For_EV (vartype);

		// Baker: January 5 2025 - What is this skipping?
		if (false == Have_Flag (global->type, DEF_SAVEGLOBAL))
			continue; // Baker: I think this excludes intermediates, fields and functions

		// Baker: What are the types that are rejected here? .. vector  cause xyz is wriiten
		// filter _x _y _z
		if (false == isin4 (vartype, ev_string_1, ev_float_2, ev_vector_3, ev_entity_4))
			continue;

		ccs *s_globalname = PRVM_GetString(prog, global->s_name);

		if (false == String_Starts_With (s_globalname, prefix))
			continue;

		// Any of the following suffix we ignore, covered by vector.
		if (vartype == ev_float_2 && String_Ends_With_Caseless_3 (s_globalname, "_x", "_y", "_z"))
			continue;

		prvm_eval_t *v				= (prvm_eval_t *) &prog->globals.fp[global->ofs];
		ccs			*s_value		= PRVM_UglyValueString(prog, (etype_t)global->type, v, valuebuf, sizeof(valuebuf));
		qbool		is_stringlist	= String_Ends_With (s_globalname, s_stringlist_suffix);

#if 1 // April 14 2026 - Load game crash.
		if (is_stringlist && atoi(s_value) == 0) {
			// Uninitialized.
			Con_PrintLinef (CON_BRONZE "An uninitialized string buffer for %s", s_globalname);
			is_stringlist = false;
		}
#endif
		if (is_stringlist) {
			int bufnum = atoi(s_value);

			// 1. Grab a svs_stringlist_index
			// 2. append bufcreate data to svs_plist.
			int svs_stringlist_index = svs.num_server_cross_globals_strlists ++;
			stringlist_t *plist = &svs.server_cross_globals_strlists[svs_stringlist_index];

			stringlist_append_from_stringbuffernum (plist, prog, bufnum);

			stringlistappend	(&svs.server_cross_globals_list, s_globalname);
			stringlistappendf	(&svs.server_cross_globals_list, "%d", PLUS1(svs_stringlist_index));
		} else {
			stringlistappend2	(&svs.server_cross_globals_list, s_globalname, s_value);
		}
		Con_PrintLinef ("%4d: " CON_GRAY_50 "%-6.6s" CON_WHITE " %-35.35s %s" /*S_FMT_LEFT_PAD_14 " %s " CON_GRAY_50 "(%s)" CON_WHITE*/,
			idx,
			s_vartype,
			s_globalname,
			s_value
		);
	} // each field

}

CALLERS_ (SV_Loadgame_from)
void ServerFlags_Cross_AFTER_Loadgame(ccs *reason)
{
	Con_PrintLinef ("ServerFlags_Cross_AFTER_Loadgame");
	// Do a ServerFlags_Cross_Changelevel_Before_Save TO FILL OUT DATABANKS FOR "RESTART"
	ServerFlags_Cross_Changelevel_Before_Save (reason);
}


void ServerFlags_Cross_Savegame_Write_SV_dot_fields (void)
{
	// Would need to write sv. fields to preserve svs. data
}

qbool ServerFlags_Cross_Loadgame_Read_SV_dot_fields (ccs *s_loadgame_token) // like sv.bufstr
{
	// Would need to read sv. fields to preserve svs. data
	return false;
}




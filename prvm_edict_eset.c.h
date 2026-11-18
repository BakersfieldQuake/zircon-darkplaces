// prvm_edict_eset.c.h


RELATED_ (PRVM_ED_EdictSet_f PRVM_GlobalSet_f)
RELATED_ (SV_ShowModel_f)

void _InfoBar_Notice_NoConPrint (cvar_t *var, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);

	Cbuf_AddTextLinef (cmd_local, "infobar 3 " QUOTED_S " silent", text);

	VA_EXPAND_ALLOC_FREE (text);
}

void _Cvar_Set_IsNotify_IsClipboard_String_Print_Fn_t (printline_fn_t printline_fn, cvar_t *var, qbool bshallnotify, qbool bshallclipboard, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);

	Cvar_SetQuick (var, text);
	printline_fn ("%s = " QUOTED_S NEWLINE /*WALDO*/, var->name, var->string);

	if (bshallnotify)
		_InfoBar_Notice_NoConPrint (var, "%s = %s", var->name, var->string);

	if (bshallclipboard) {
		Clipboard_Set_Text (text);
		printline_fn ("%s: value set to clipboard", var->name);
	}

	VA_EXPAND_ALLOC_FREE (text);
}


void _Cvar_Set_IsNotify_IsClipboard_String (cvar_t *var, qbool bshallnotify, qbool bshallclipboard, qbool bshallprint, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);

	Cvar_SetQuick (var, text);
	if (bshallprint)
		Con_PrintLinef ("%s = " QUOTED_S, var->name, var->string);

	if (bshallnotify)
		_InfoBar_Notice_NoConPrint (var, "%s = %s", var->name, var->string);

	if (bshallclipboard) {
		Clipboard_Set_Text (text);
		if (bshallprint)
			Con_PrintLinef ("%s: value set to clipboard", var->name);
	}

	VA_EXPAND_ALLOC_FREE (text);
}


// END VALUE / ENTNUM


static void _Any_PRVM_ED_Edict_Remove_f (cmd_state_t *cmd, ccs *s_progname)
{
	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return; // Bad program
	}

	if (cmd_argc < 2 ) {
		// No params - print information
		// ex: prvm_globalset
		Con_PrintLinef ( "%s: remove an edict", cmd_argv_command_0 );
		Con_PrintLinef ( "See also: ent_remove (server only)");
		return;
	}

	int edict_num = atoi(cmd_argv_parm1);

	if (in_range_beyond (/*world = 0*/ 1, edict_num, prog->num_edicts) == false) {
		Con_PrintLinef ("Bad edict number (1 to %d valid)", prog->num_edicts - 1);
		return; // Bad edict number
	}

	prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);

	if (PRVM_NUM_FOR_EDICT(ed) <= prog->reserved_edicts) {
		Con_PrintLinef ("%s: " "tried to remove the null entity or a reserved entity!", cmd_argv_command_0);
	}
	else if (ed->free) {
		Con_PrintLinef ("%s: " "tried to remove an already freed entity!", cmd_argv_command_0);
	}
	else {
		PRVM_ED_Free (prog, ed);
		Con_PrintLinef ("%s: Successfully removed # %d", cmd_argv_command_0, edict_num);
	}

}



static void _Any_PRVM_ED_Edict_Create_f (cmd_state_t *cmd, ccs *s_progname)
{
	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return; // Bad program
	}

	if (cmd_argc < 2 ) {
		// No params - print information
		// ex: prvm_globalset
		Con_PrintLinef ( "%s: create an edict", cmd_argv_command_0 );
		Con_PrintLinef ( "Type " CON_BRONZE "%s go" CON_WHITE " to create an entity", cmd_argv_command_0 );
		Con_PrintLinef ( "See also: ent_create (server only)");
		return;
	}

	//ccs *s_go		= cmd_argv_parm1;

	RELATED_ (VM_CL_spawn VM_remove)
	prvm_edict_t *ed = PRVM_ED_Alloc (prog); // Failure means host error

	int edictnum = PRVM_NUM_FOR_EDICT(ed);

	Con_PrintLinef ("Created edict #%d", edictnum);
	_Cvar_Set_IsNotify_IsClipboard_String (&_ent, q_notifyinfobar_true, q_clipboard_false, q_shall_print_true,
		"%d",
		edictnum
	);

}


void PRVM_ED_Edict_Remove_CSQC_f (cmd_state_t *cmd)
{
	_Any_PRVM_ED_Edict_Remove_f (cmd, "client");
}

void PRVM_ED_Edict_Create_CSQC_f (cmd_state_t *cmd)
{
	_Any_PRVM_ED_Edict_Create_f (cmd, "client");
}

RELATED_ (PRVM_ED_GlobalGet_f) // gset time 24.7278805
static void PRVM_ED_anygset_f(cmd_state_t *cmd, ccs *s_progname) // "gset"
{
	char valuebuf[1024];
	int argc = cmd_argc;				  // 0    1    2
	ccs *s_globalname	= cmd_argv_parm1; // gset time      argc == 2
	ccs *s_value		= cmd_argv_parm2; // gset time 22   argc == 3

	if (argc < 2) {
		// No params - print information
		// ex: eset
		Con_PrintLinef ("usage:" NEWLINE
			"%s <globalname> // print a field value" NEWLINE
			"%s <globalname> <value> // set a field value" NEWLINE, // End of string
			cmd_argv_command_0, cmd_argv_command_0);
		return;
	}

	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return; // Bad program
	}

	mdef_t *global = PRVM_ED_FindGlobal(prog, s_globalname);
	if (!global) {
		Con_PrintLinef ( "No global named " QUOTED_S " in %s!", s_globalname, s_progname);
		goto fail;
	}

	RELATED_ (PRVM_ED_GlobalGet_f) // gset time 24.7278805
	if (cmd_argc == 2) {
		// GET VALUE
		PRVM_ED_Global_Fly (s_progname, s_globalname, valuebuf, sizeof(valuebuf));
		Con_PrintLinef ("%s", valuebuf);
		if (prvm_edictget_autostore.integer) {
			_Cvar_Set_IsNotify_IsClipboard_String (&_val, q_notifyinfobar_true, prvm_edictget_autostore.integer >= 2 /*d:2*/, q_shall_print_true,
				"%s",
				valuebuf
			);
		} // autostore
		return;
	}

	// SET VALUE - Accumulate!
	RELATED_ (PRVM_ED_GlobalGet_f) // gset time 24.7278805
	//prvm_eval_t *v = (prvm_eval_t *) &prog->globals.fp[global->ofs];

	int global_vartype = Flag_Remove(global->type, DEF_SAVEGLOBAL);
	//ccs *global_vartype_str = VarType_For_EV (global_vartype);

	if (global_vartype == ev_float && String_Starts_With (s_value, "=")) {
		// POPULATE VALUEBUF with the new value
		char current_valuebuf[1024];
		double curval = -9999;
		double incval = -9999;
		double dbl_result = -9999;
		Argv_Cumulate (cmd, /*low arg num*/ 2, valuebuf, sizeof(valuebuf) );

		if (String_Starts_With (s_value, "=+")) {
			// +=
			PRVM_ED_Global_Fly (s_progname, s_globalname, current_valuebuf, sizeof(current_valuebuf));
			curval = atof (current_valuebuf);
			incval = atof (&valuebuf[2]); // Skip leading "=+"
			dbl_result = curval + incval;
			zpr ("Value = %g + %g = %g", curval, incval, dbl_result);
			c_dpsnprintf1 (valuebuf, "%g", dbl_result);
			
		} else if (String_Starts_With (s_value, "=-")) {
			// -=
			PRVM_ED_Global_Fly (s_progname, s_globalname, current_valuebuf, sizeof(current_valuebuf));
			curval = atof (current_valuebuf);
			incval = atof (&valuebuf[2]); // Skip leading "=+"
			dbl_result = curval - incval;
			zpr ("Value = %g - %g = %g", curval, incval, dbl_result);
			c_dpsnprintf1 (valuebuf, "%g", dbl_result);
		} else {
			
			//Con_PrintLinef ("Accumulated value string = " QUOTED_S, valuebuf);

			double dbl_result = -9999;
			char errbuf[2048];
			equat_t _eq = {0};

			// Equation contents are freed by processor.  The struct itself obviously not.

			ccs *s_equat = &valuebuf[1]; // Skip leading "="
			int errcode = equat_calc_result (&_eq, s_equat, &dbl_result, errbuf, sizeof(errbuf));

			if (errcode) {
				c_strlcpy (valuebuf, errbuf);
			} else {
				Con_PrintLinef ("EQ: " QUOTED_S " = " CON_BRONZE "%g", valuebuf, dbl_result);
				c_dpsnprintf1 (valuebuf, "%g", dbl_result);
			}
		} // = 
	}

	else if (argc == 3) {
		// 0    1   2
		// ex: gset time 0.4    (argc 3)
		c_strlcpy (valuebuf, s_value);
	} else /*else if (argc >= 5)*/ {
		// Accumulate
		// 0    1  2     3 4 5
		// eset 40 color 2 2 2  (argc 6)
		Argv_Cumulate (cmd, /*low arg num*/ 2, valuebuf, sizeof(valuebuf) );
		Con_PrintLinef ("Accumulated value string = " QUOTED_S, valuebuf);
	}
	RELATED_ (PRVM_ED_GlobalGet_f) // gset time 24.7278805

	/*int isok =*/ Set_A_Global_Print_Errors (s_progname, s_globalname, valuebuf);

	// SET VALUE
fail:
	;


}

int edictnum_findfirst_for_string(prvm_prog_t *prog, ccs *field_like_targetname, ccs *value_to_find)
{
	char valuebuf[MAX_OSPATH_EX_1024];

	for_each_edict_in_prog(edict_num,ed)
		PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, field_like_targetname, valuebuf);

		if (String_Match (valuebuf, value_to_find))
			return edict_num; // Found one

	for_each_end // edict
	return not_found_neg1;
}

RELATED_ (PRVM_ED_EdictSet_f)
static void PRVM_ED_anyset_f(cmd_state_t *cmd, ccs *s_progname) // "eset" "cset"
{
	char valuebuf[1024];
	int argc = cmd_argc;
	ccs *s_edict_num	= cmd_argv_parm1;
	ccs *s_fieldname	= cmd_argv_parm2;
	ccs *s_value		= cmd_argv_parm3;

	if (argc < 2) {
		// No params - print information
		// ex: eset
		Con_PrintLinef ("usage:" NEWLINE
			"%s <edict_num> // print an edict" NEWLINE
			"%s <edict_num> <fieldname> // print a field value" NEWLINE
			"%s <edict_num> <fieldname> <value> // set a field value" NEWLINE
			"%s <nearby> // list edicts nearby with radius 300" NEWLINE
			"%s <nearby> <radius> // list edicts nearby with custom radius " NEWLINE, // End of string
			cmd_argv_command_0, cmd_argv_command_0, cmd_argv_command_0, cmd_argv_command_0, cmd_argv_command_0);
		return;
	}

	prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	if (!prog) {
		Con_PrintLinef ("Program %s not active", s_progname);
		return; // Bad program
	}

	if (String_Match (cmd_argv_parm1, "nearby")) {
		qbool filtering_because_not_menu = true;

#ifdef CONFIG_MENU
		if (prog == MVM_prog) {
			//Con_PrintLinef ("Nearby cannot work with menu");
			filtering_because_not_menu = false; // Enabled, now menu does short summary
		}
#endif // CONFIG_MENU
		int count = 0;
		float radius = (argc == 3) ? atof (cmd_argv_parm2) : 300;

		// Baker: An alternative is vieworg.
		vec3_t cl_player_origin = {0,0,0}, cl_player_angles = {0,0,0};
		if (cls.signon == SIGNONS_4) {
			// What about crash?
			// A: CL_Disconnect_Real occurs and then cls.signon = SIGNON_ZERO
			VectorCopyDestSrc (cl_player_origin, cl.entities[cl.playerentity].state_current.origin);
			VectorCopyDestSrc (cl_player_angles, cl.entities[cl.playerentity].state_current.angles);
		}

		// List ones nearby
		for (int edict_num = AFTER_WORLD_1; edict_num < prog->num_edicts; edict_num ++) {
			prvm_edict_t	*ed = PRVM_EDICT_NUM(edict_num);

			if (ed->free)
				continue; // It's free

			//qbool has_origin = PRVM_ED_EdictGet_Fly_Lite (prog, edict_num, "origin", valuebuf, sizeof(valuebuf));

			vec3_t center;
			char s_classname[1024] = {0};
			char s_targetname[1024] = {0};
			char s_model[1024] = {0};
			char s_origin[1024] = {0};

			PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "classname", s_classname);
			PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "targetname", s_targetname);
			PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "model", s_model);

			ccs *model_skippath = File_URL_SkipPath (s_model);

			char sbuf[1024];
			{
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "absmin", sbuf);
				vec3_t vabsmin; Math_atov3 (sbuf, vabsmin);
				PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "absmax", sbuf);
				vec3_t vabsmax; Math_atov3 (sbuf, vabsmax);
				if (Vector3_IsZeros (vabsmin)) {
					// Get from origin
					PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "origin", sbuf);
					Math_atov3 (sbuf, center);
				}
				else {
					// Get it from center
					VectorAdd (vabsmin, vabsmax, center);
					VectorScale (center, 0.5, center);
				}
			}

			// Now check distance
			float distance = VectorDistance (cl_player_origin, center);

			// Baker: Modified to allow menu to have short summary.
			if (filtering_because_not_menu)
				if (distance > radius)
					continue; // Disqualified due to radius

			// Baker: Now we want to check if in front of.
			qbool is_infront = Math_infront(cl_player_origin, cl_player_angles, center);

			if (filtering_because_not_menu && !is_infront) {
				//Con_PrintLinef ("%d (%s) is near, but not in front of us", edict_num, s_classname);
				continue;
			}

			// Fill in s_origin
			c_dpsnprintf1 (s_origin, VECTOR3_G, VECTOR3_SEND(center));
			count ++;

			if (filtering_because_not_menu)
				Con_PrintLinef ("%3d: %s (targetname: %s model: %s)",
					edict_num,
					s_classname,
					s_targetname,
					model_skippath // s_model
				);
			else
				// MENU
				Con_PrintLinef ("%3d: %s",
					edict_num,
					s_classname
				);
		} // each entity

		if (filtering_because_not_menu)
			Con_PrintLinef ("Results = %d in radius of %g and in front of view", count, radius);
		else
			Con_PrintLinef ("Results = %d", count); // Menu
		return;
	} // END RADIUS PRINT "NEARBY"

	if (argc == 2) {
		// Become "edict"
		// ex: eset 25
		/*int isok =*/ Print_A_Edict_Print_Errors (s_progname, s_edict_num, s_fieldname);
		return;
	}

	int edict_num = atoi (s_edict_num);

	if (String_Starts_With_Alpha(s_edict_num)) {
		// Find an edict with targetname == how?
		edict_num = edictnum_findfirst_for_string (prog, "targetname", s_edict_num /*which is a alpha name*/);
		if (edict_num == not_found_neg1) {
			Con_PrintLinef ("targetname " QUOTED_S " not found", s_edict_num);
			return;
		}
	}

	if (in_range_beyond (0, edict_num, prog->num_edicts) == false) {
		Con_PrintLinef ("Bad edict number (0 to %d valid)", prog->num_edicts - 1);
		return; // Bad edict number
	}

	if (argc == 3) {
		// Become "get"
		// ex: eset 25 targetname
		PRVM_ED_EdictGet_Fly (s_progname, s_edict_num, s_fieldname, valuebuf, sizeof(valuebuf));

		Con_PrintLinef ("%s", valuebuf);
		if (prvm_edictget_autostore.integer) {
			_Cvar_Set_IsNotify_IsClipboard_String (&_val, q_notifyinfobar_true, prvm_edictget_autostore.integer >= 2 /*d:2*/, q_shall_print_true,
				"%s",
				valuebuf
			);
		} // autostore
		return;
	}

	// 0    1  2     3
	// eset 40 scale 0.4    (argc 4)
	// 0    1  2     3 4 5
	// eset 40 color 2 2 2  (argc 6)

	// Value is argc == 4, if we are at 5 or greater cumulate

	if (Get_A_VarType_Prog_Fieldname(s_progname, s_fieldname) == ev_float
		&& String_Starts_With (s_value, "=")) {

		Argv_Cumulate (cmd, /*low arg num*/ 3, valuebuf, sizeof(valuebuf) );
		//Con_PrintLinef ("Accumulated value string = " QUOTED_S, valuebuf);

		double dbl_result = -9999;
		char errbuf[2048];
		equat_t _eq = {0};

		// Equation contents are freed by processor.  The struct itself obviously not.

		ccs *s_equat = &valuebuf[1]; // Skip leading "="
		int errcode = equat_calc_result (&_eq, s_equat, &dbl_result, errbuf, sizeof(errbuf));

		if (errcode) {
			c_strlcpy (valuebuf, errbuf);
		} else {
			Con_PrintLinef ("EQ: " QUOTED_S " = " CON_BRONZE "%g", valuebuf, dbl_result);
			c_dpsnprintf1 (valuebuf, "%g", dbl_result);
		}
	}

	else if (argc == 4) {
		// 0    1  2     3
		// ex: eset 40 scale 0.4    (argc 4)
		c_strlcpy (valuebuf, s_value);
	} else /*else if (argc >= 5)*/ {
		// Accumulate
		// 0    1  2     3 4 5
		// eset 40 color 2 2 2  (argc 6)
		Argv_Cumulate (cmd, /*low arg num*/ 3, valuebuf, sizeof(valuebuf) );
		Con_PrintLinef ("Accumulated value string = " QUOTED_S, valuebuf);
	}

	//prvm_prog_t *prog = PRVM_FriendlyProgFromString(s_progname);
	/*int isok =*/ Edict_Set_String_Print_Errors (prog, s_edict_num, s_fieldname, valuebuf);
}


static void PRVM_ED_Gset_f(cmd_state_t *cmd)
{
	PRVM_ED_anygset_f (cmd, "server");
}

static void PRVM_ED_Eset_f(cmd_state_t *cmd)
{
	PRVM_ED_anyset_f (cmd, "server");
}


static void PRVM_ED_Cset_f(cmd_state_t *cmd)
{
	PRVM_ED_anyset_f (cmd, "client");
}

#ifdef CONFIG_MENU
static void PRVM_ED_Mset_f(cmd_state_t *cmd)
{
	PRVM_ED_anyset_f (cmd, "menu");
}
#endif // CONFIG_MENU


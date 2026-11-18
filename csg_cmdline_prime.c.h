// csg_cmdline_prime.c.h

#include "csg_local.h"

// CSG PROCESS IS HERE
RELATED_ (Sys_Console_Init_HOutput_WIN32)

// Only -csg calls this and -csg does not init anything and then exits.
void CSG_Macro_Perform_Exit_1_Error (ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	Con_PrintLinef ("************ ERROR ************" NEWLINE "%s", text);
	VA_EXPAND_ALLOC_FREE (text);
	exit (2); // September 20 2026 - It's fine.
}

// September 20 2026 - timestring was retired but we keep the code in the comments.
// Gen timestring // "_brushfacer" "0 - 20241005 10:12 AM"
// char timestring[64]; TimeString_Fill_YYYYMMDD_HHMMSSAM (timestring, sizeof(timestring));
// va_super (timestring_newval, 128, "0 - %s", timestring);  // looks like "0 - 20241005 10:12 AM" //char *snewval_za = Z_StrDupf ("0 - %s", timestring); 

// JACK does C:/galaxy/zircon as the startup directory here.
void CSG_Perform_Early_Chdir (int csg_argnum) // csg console command DOES NOT COME HERE
{
	Sys_Console_Init_HOutput_WIN32 (); // Activate HOUTPUT we hope
	ccs *s_cwd_ta = FS_Getcwd_No_Trailing_Slash_Unsafe_TempAlloc ();
	va_copy_1024 (curdir, s_cwd_ta);
	va_copy_1024 (newdir, s_cwd_ta);

	File_URL_Edit_Reduce_To_Parent_Path_No_Trailing_Slash (newdir); // Strip mymap.map
	c_strlcat (newdir, "/");
	zpr ("CSG: Startup working directory %s", curdir);
	zpr ("CSG: New dir %s", newdir);
	zpr ("CSG: Engine date %s", buildstring);

	char mappath[MAX_OSPATH];
	CSG_PathWork_Chdir_Linuxize_Slashses_No_Unicode_Support (csg_argnum, mappath, sizeof(mappath) );
	Con_PrintLinef ("CSG: Processing map %s", mappath);

	cl_available = false;
	cls.state = ca_dedicated;
	cl_csg_run = csg_argnum;
	
#ifdef _DEBUG
	// Baker: Only when debugging would we use the clipboard for csg command line.
	void SDL_CSG_Init (void);
	SDL_CSG_Init (); // Run SDL_Init for clipboard
#endif // _DEBUG

	// Mod_LookupQ3Shader is called when "needed"

	Mem_FreeNull_ (s_cwd_ta);
}

CALLERS_ (FS_InitOnce_Dir)
void CSG_Perform_Exit_0 (int csg_argnum) // csg console command DOES NOT COME HERE
{
#ifdef _WIN32
	Sys_Console_Init_HOutput_WIN32 (); // Activate HOUTPUT we hope
	ccs *s_cwd_ta = FS_Getcwd_No_Trailing_Slash_Unsafe_TempAlloc ();
	Con_PrintLinef ("CSG: Startup working directory %s", s_cwd_ta);
	Mem_FreeNull_ (s_cwd_ta);
	FS_Path_f (cmd_local);
#endif

	// July 20 2026 -- mappath 0x0313a944 "c:/galaxy/zircon/maps/zec_park.map"	char [260]
	char mappath[MAX_OSPATH];
	CSG_PathWork_Chdir_Linuxize_Slashses_No_Unicode_Support (csg_argnum, mappath, sizeof(mappath) );
	zpr ("CSG: Processing map %s", mappath);
	RELATED_ (csgc_csg_run)
	double time_start = Sys_DirtyTime ();
	int num_csg_used = 0;
	char preprocessor_runs[2048] = {0};
		
	char *sinza = FS_LoadFileString_Unsafe_ZAlloc (mappath);
	
	if (!sinza)
		CSG_Macro_Perform_Exit_1_Error("Unable to load file %s", mappath); // Couldn't open map

	char *sinza_before = Z_StrDup (sinza);
	
	Con_PrintLinef ("Loaded from file ok.");

commands:

#define CSG_CMDLiNE_REWRITE_RELOAD_ \
	{ /* RE-WRITE */ \
		/*Con_PrintLinef ("Rewriting / reparsing .map ...");*/ \
		c_strlcatf (preprocessor_runs, " %s", stringkey); \
		map_set (&list_map1, WORLD_0, "_zircon_preprocess", &preprocessor_runs[1]); \
		map_set (&list_map1, WORLD_0, "_zircon_version", engineversion); \
		baker_string_s *bsout = map_save_to_string_bsalloc (&list_map1); if (!bsout) CSG_Macro_Perform_Exit_1_Error("generated string using map_save_to_string_bsalloc failed"); \
		int isok2 = FS_SaveFileString_Is_Ok_Unsafe (mappath, bsout->string); \
		BakerString_Destroy_And_Null_It (&bsout); \
		if (isok2 == false) CSG_Macro_Perform_Exit_1_Error("Save file string failed."); \
	} \
	{ /* REFRESH SOURCE Free the map string -- then reload it */ \
		Mem_FreeNull_ (sinza); sinza = FS_LoadFileString_Unsafe_ZAlloc (mappath); if (!sinza) CSG_Macro_Perform_Exit_1_Error("Reload FS_LoadFileString_Unsafe_ZAlloc sinza failed"); \
	} // Ender


	// COMMANDS - The process and then exit immediately.	
	#define DEF_(csg_console_name, func, description, long_explain) // Nada

	#define Q3MAP_PRE(MYSTRING, MYFUNC, EXAMPLE, DESCRIBE, WORLD_OR_ENTITY_TYPE, APPLIES_TO) \
		while (String_Contains (sinza, MYSTRING)) { \
			ccs *stringkey = MYSTRING; \
			map_t list_map1 = {0}; /* pac.map */ \
			zpr ("Found " QUOTED_S " .. ", MYSTRING); \
			/* PARSE THE MAP */ \
			int isok = map_load_from_string (&list_map1, sinza); \
			if (isok == false) CSG_Macro_Perform_Exit_1_Error("map_load_from_string failed"); \
			int num_done = MYFUNC(&list_map1, MYSTRING); \
			zpr ("num_processed = %d", num_done); \
			/* RE-WRITE IF ANYTHING HAPPENED */ \
			if (num_done) { \
				num_csg_used ++; \
				CSG_CMDLiNE_REWRITE_RELOAD_ \
			} /* actions performed so rewrite*/ \
			map_freecontents	(&list_map1); \
			break; \
		} /* while */ // Ender

	RELATED_ (csgax_frost)
	#define Q3MAP_PRX(MYSTRING,MYFUNC,EXAMPLE,DESCRIBE,WORLD_OR_ENTITY_TYPE,APPLIES_TO) \
		while (String_Contains (sinza, MYSTRING)) { \
			ccs *stringkey = MYSTRING; \
			map_t list_map1 = {0}; /* pac.map */ \
			zpr ("Found " QUOTED_S " .. ", MYSTRING); \
			/* PARSE THE MAP */ \
			int isok = map_load_from_string (&list_map1, sinza); \
			if (isok == false) CSG_Macro_Perform_Exit_1_Error("map_load_from_string failed"); \
			int num_done = MYFUNC(&list_map1, MYSTRING, mappath); \
			zpr ("num_processed = %d", num_done); \
			/* RE-WRITE IF ANYTHING HAPPENED */ \
			if (num_done) { \
				num_csg_used ++; \
				CSG_CMDLiNE_REWRITE_RELOAD_ \
			} /* actions performed so rewrite*/ \
			map_freecontents	(&list_map1); \
			break; \
		} /* while */ // Ender

		#include "csg_table.h"
	

end_commands:

	if (num_csg_used) {
		// Construct the name
		//int isok2 = FS_SaveFileString_Is_Ok_Unsafe (mappath, bsout->string);
		va_copy_1024 (mappath_before, mappath);
		File_URL_Edit_Strip_Set_Extension (mappath_before, "_before.map", sizeof(mappath_before));
		int isok3 = FS_SaveFileString_Is_Ok_Unsafe (mappath_before, sinza_before);
		if (isok3)
			zpr ("Wrote %s", mappath_before);
		else
			zpr (CON_RED "Couldn't write %s", mappath_before);
	}

	Mem_FreeNull_ (sinza); // Free the map data -- then reload it
	Mem_FreeNull_ (sinza_before);
	double time_end = Sys_DirtyTime ();
	double elapsed = time_end - time_start;

	Con_PrintLinef ("CSG: Ending - seconds = %g CSG phases = %d = %s", elapsed, num_csg_used, &preprocessor_runs[1] );
	exit (0); // success
}







// menu_saveload2.c.h

// Save Map Progress Age
#define SAVELOAD_SORTBY_SAVENAME_0		0
#define SAVELOAD_SORTBY_MAP_1			1
#define SAVELOAD_SORTBY_PROGRESS_2		2
#define SAVELOAD_SORTBY_AGE_3			3

#define		local_count					g_saves_filename_noExt.numstrings
#define		local_cursor				m_load2_load2cursor
#define 	visiblerows 				m_load2_visiblerows
#define 	startrow 					m_load2_startrow
#define 	endrow 						m_load2_endrow
#define		local_click_time			m_load2_click_time
#define		local_scroll_is_blocked		m_load2_scroll_is_blocked
#define		local_sort_by				m_load2_sort_by
#define		local_sort_is_ascending		m_load2_is_sort_ascending
#define		local_sort_is_dirty			m_load2_sort_is_dirty

// Baker: July 19 2026 -- preserve these
int m_load2_sort_by = SAVELOAD_SORTBY_SAVENAME_0;
int m_load2_is_sort_ascending = true;
int m_load2_sort_is_dirty;

float m_load2_click_time = 0;
qbool m_load2_scroll_is_blocked;

int m_load2_oldload_cursor; // r_textures_shutdown sets to -1, M_Menu_Load2_f sets to -1. gamedir change does nothing.
int m_load2_load2cursor;
int m_load2_visiblerows;
int m_load2_startrow;
int m_load2_endrow;



// If we were to sort these, we need to sort the indexes.
// By a proxy.

// Baker: These are freed only on refresh before being repopulated.
stringlist_t	g_saves_filename_noExt;
stringlist_t	g_saves_filename_noExt_crop;
stringlist_t	g_saves_filename_timestr;
stringlist_t	g_saves_secondsold;
stringlist_t	g_saves_mapnames;
stringlist_t	g_saves_mapnames_crop;
stringlist_t	g_saves_jpeg_strings;
stringlist_t	g_saves_comment;
stringlist_t	g_saves_indexes_4byte;


// qsort(&g_saves_indexes[0], serverlist_list_count, sizeof(serverlist_list[0]), slist_sort_map);

int m_load2_sort_map = 0;

RELATED_ (slist_sort_map stringlistsort stringlistsort_cmp stringlistsort_start_length_cmp)
// qsort(&list->strings[0], list->numstrings, sizeof(list->strings[0]), myfunc);

//stringlist_t *plist;
//qsort(&plist->strings[0], plist->numstrings, sizeof(plist->strings[0]), myfunc);

// The idea is we sort g_saves_indexes

// Use stringlistsort_custom with uniq = false.
RELATED_ (stringlistsort_cmp)
int saveload_sort (const void *pa, const void *pb)
{
	int *sa = (int *)(*(const char **)pa);
	int *sb = (int *)(*(const char **)pb);
	int idx0 = *sa;
	int idx1 = *sb;
	const char *da = NULL;
	const char *db = NULL;
	int negator = m_load2_is_sort_ascending == false ? -1 : 1;

	switch (m_load2_sort_by) {
	default:
		break;

	case SAVELOAD_SORTBY_SAVENAME_0:
		da = g_saves_filename_noExt.strings[idx0];
		db = g_saves_filename_noExt.strings[idx1];
		break;

	case SAVELOAD_SORTBY_MAP_1:
		da = g_saves_mapnames.strings[idx0];
		db = g_saves_mapnames.strings[idx1];
		break;

	case SAVELOAD_SORTBY_PROGRESS_2:
		da = g_saves_comment.strings[idx0];
		db = g_saves_comment.strings[idx1];
		break;

	case SAVELOAD_SORTBY_AGE_3:
		da = g_saves_secondsold.strings[idx0];
		db = g_saves_secondsold.strings[idx1];
		break;
	} // sw

	int diff = strcasecmp(da, db);
	//Con_PrintLinef ("Comparing %d " QUOTED_S " vs. %d " QUOTED_S, idx0, da, idx1, db);
	if (diff == 0) {
		// Baker: Deterministic sorting. We cannot allow ties ever.
		// Reason: We want a predictable order every time otherwise the results could jumps around
		// with tie entries randomly filling out the list differently every sort.
		diff = (idx0 - idx1);
	}

	return negator * diff;
}



RELATED_ (GetFileList_Not_Path_Aware_Count)

// Saves - without the .sav
// MENU:		update stringlist_t	g_load2_list;
// NON-MENU:	PARTIAL EVAL

#include <time.h>

static const char *month_str (int m)
{
	if (m == 0) return "jan";
	if (m == 1) return "feb";
	if (m == 2) return "mar";
	if (m == 3) return "apr";
	if (m == 4) return "may";
	if (m == 5) return "jun";
	if (m == 6) return "jul";
	if (m == 7) return "aug";
	if (m == 8) return "sep";
	if (m == 9) return "oct";
	if (m == 10) return "nov";
	if (m == 11) return "dec";
	return "";

}

#if 0 // Baker: Not used, but keep in case we need it.
static char *ageof_this_vs_now_zalloc (double thistime, double nowtime)
{
	double delta = nowtime - thistime; // seconds

	double years = delta / (24 * 60 * 60 * 365);
	double months = delta / (24 * 60 * 60 * 30);
	double days = delta / (24 * 60 * 60);
	double hours = delta / (60 * 60);
	double minutes = delta / 60;

	char buf[256];
	if (years > 1) {
		c_dpsnprintf1 (buf, "%2d years", (int)years);
		return Z_StrDup (buf);
	}

	if (months > 1) {
		c_dpsnprintf1 (buf, "%2d months", (int)months);
		return Z_StrDup (buf);
	}
	if (days > 1) {
		c_dpsnprintf1 (buf, "%2d days", (int)days);
		return Z_StrDup (buf);
	}

	if (hours > 1) {
		c_dpsnprintf1 (buf, "%2d hours", (int)hours);
		return Z_StrDup (buf);
	}

	c_dpsnprintf1 (buf, "%2d minutes", (int)minutes);
	return Z_StrDup (buf);
}
#endif // Baker: Keep

static char *ageof_this_vs_now_months_zalloc (double thistime, double nowtime)
{
	double delta = nowtime - thistime; // seconds

	double years = delta / (24 * 60 * 60 * 365);
	double months = delta / (24 * 60 * 60 * 30);
	double days = delta / (24 * 60 * 60);
	double hours = delta / (60 * 60);
	double minutes = delta / 60;

	char buf[256];
	if (years > 1) {
		// Do months instead
		months = years * 12;
	}

	if (months > 1) {
		c_dpsnprintf1 (buf, "%2d months", (int)months);
		return Z_StrDup (buf);
	}
	if (days > 1) {
		c_dpsnprintf1 (buf, "%2d days", (int)days);
		return Z_StrDup (buf);
	}

	if (hours > 1) {
		c_dpsnprintf1 (buf, "%2d hours", (int)hours);
		return Z_StrDup (buf);
	}

	c_dpsnprintf1 (buf, "%2d minutes", (int)minutes);
	return Z_StrDup (buf);
}

RELATED_ (SV_Loadgame_from)
static int Add_SaveFile (char *sxy) // THIS IN MENU.C BRO
{
	int is_ok = true; // optimistic
	char *realpathname_zalloc = NULL;
	const char *text = NULL;
	char *s_jpeg_string_zalloc = NULL;
	char *age_zalloc = NULL;

	// Fill menu
	qfile_t	*f = FS_OpenRealFileReadBinary (sxy, &realpathname_zalloc);
	if (!f) {
		// Con_PrintLinef ("Could not FS_OpenRealFile %s", sxy);
		is_ok = false;
		goto errox;
	}

	FS_CloseNULL_ (f); // We don't need it open

	const char *t = text = (char *)FS_LoadFile (sxy, tempmempool, fs_quiet_FALSE, fs_size_ptr_null);
	if (!text) {
		// Con_PrintLinef ("Could not FS_LoadFile %s", sxy);
		is_ok = false;
		goto errox;
	}

	// version
	COM_Parse_Basic (&t);
	int version = atoi(com_token);
	if (version != SAVEGAME_VERSION_5) {
		is_ok = false;
		goto errox;
	}

	// description
	COM_Parse_Basic (&t);
	char	comment[SAVEGAME_COMMENT_LENGTH_39 + 1];
	c_strlcpy (comment, com_token);
	String_Edit_Replace_Char (comment, '_', CHAR_SPACE_32, /*count*/ NULL);

	for (int j = 0 ; j < NUM_SPAWN_PARMS_16; j ++) { // PARM_EXPAND IN MENU.C PARSING A BIT OF THE SAVE TO GET INFO
		COM_Parse_Basic (&t);
		//spawn_parms[i] = atof(com_token);
	}
	// skill
	COM_Parse_Basic (&t);

	// this silliness is so we can load 1.06 save files, which have float skill values
	current_skill = (int)(atof(com_token) + 0.5);

	// mapname
	COM_Parse_Basic (&t);

	char	mapname[MAX_QPATH_128];
	c_strlcpy (mapname, com_token);

	// Find screenshot

	const char *s_screenshot = strstr (t, "sv.screenshot ");
	s_jpeg_string_zalloc = NULL;

	while (s_screenshot) {
		t = s_screenshot;
		COM_Parse_Basic (&t); // sv.screenshot
		COM_Parse_Basic (&t); int jpeg_slen_int = atoi(com_token);

		int too_large = (SAVEGAME_PIC_WIDTH_512 * SAVEGAME_PIC_HEIGHT_320 * RGBA_4);
		if (jpeg_slen_int <= 2 || jpeg_slen_int > too_large) {
			//Sys_PrintToTerminal ("Rejecting invalid jpeg size" NEWLINE);
			break;
		}

		const char *s_jpeg = t; // Cursor is on space for base64 blob
		while (*s_jpeg && ISWHITESPACE(*s_jpeg))
			s_jpeg ++;

		size_t jpeg_slen = (size_t)jpeg_slen_int;
		s_jpeg_string_zalloc = (char *)Z_MemDup_Z (s_jpeg, jpeg_slen);
		break; // exit loop
	} // while


	double sav_filetime_since1970 = FS_File_DateTime_Unsafe(realpathname_zalloc);

	time_t		rawtime			= (time_t)(double)sav_filetime_since1970;
	struct tm	*tmx			= localtime(&rawtime);

	char		sfiledate[MAX_QPATH_128];

	age_zalloc = ageof_this_vs_now_months_zalloc (sav_filetime_since1970, /*now*/ (double)time(NULL));

	RELATED_ (M_ScanSaves)

	c_dpsnprintf6 (sfiledate,
		"%04d %3s %2d %2d:%02d %s",
		1900 + tmx->tm_year, // 	years since 1900
		month_str (tmx->tm_mon), // months since January (0 to 11)
		tmx->tm_mday,  // 1-31
		tmx->tm_hour > 12 ? tmx->tm_hour - 12 :
		tmx->tm_hour == 0 ? 12 :
			tmx->tm_hour
		, // 0-23
		tmx->tm_min,
		tmx->tm_hour >= 12 ? "PM" : "AM"
	);

	File_URL_Edit_Remove_Extension (sxy);

	const char *s_kills = strstr(comment, "kills:"); // Nov 14 2024 removed space
	if (s_kills) {
		s_kills += STRINGLEN("kills:"); // Nov 14 2024 removed space
	} else
		s_kills = "";

	int idx[2];
	idx[0] = g_saves_indexes_4byte.numstrings;
	stringlistappend_blob (&g_saves_indexes_4byte, (const byte *)&idx, sizeof(int) * 2 );

	stringlistappend	(&g_saves_filename_noExt, sxy);
	stringlistappendf	(&g_saves_filename_noExt_crop, "%-12.12s", sxy);
	stringlistappend	(&g_saves_jpeg_strings, s_jpeg_string_zalloc ? s_jpeg_string_zalloc : "");
	stringlistappend	(&g_saves_filename_timestr, age_zalloc );

	va_super (stmpseconds1970, 64, "%011.0f", sav_filetime_since1970);
	stringlistappend (&g_saves_secondsold, stmpseconds1970);

	stringlistappend	(&g_saves_mapnames, mapname);
	stringlistappendf	(&g_saves_mapnames_crop, "%-12.12s", mapname);
	stringlistappend	(&g_saves_comment, s_kills);

errox:

	Mem_FreeNull_	(s_jpeg_string_zalloc);
	Mem_FreeNull_	(age_zalloc);
	Mem_FreeNull_	(realpathname_zalloc);
	Mem_FreeNull_	(text);
	FS_CloseNULL_	(f);

	return is_ok;
}

CALLERS_ (M_Menu_Load2_f)
void Get_SavesListGameDirOnly (void)
{
	stringlistfreecontents (&g_saves_filename_noExt);
	stringlistfreecontents (&g_saves_filename_noExt_crop);
	stringlistfreecontents (&g_saves_filename_timestr);
	stringlistfreecontents (&g_saves_secondsold);
	stringlistfreecontents (&g_saves_mapnames);
	stringlistfreecontents (&g_saves_mapnames_crop);
	stringlistfreecontents (&g_saves_jpeg_strings);
	stringlistfreecontents (&g_saves_comment);
	stringlistfreecontents (&g_saves_indexes_4byte);
	stringlistfreecontents (&g_saves_secondsold);

	fssearch_t	*t = FS_Search ("*.sav", fs_caseless_true, fs_quiet_true, fs_pakfile_null, fs_gamedironly_true);

	if (!t)
		return;

	for (int idx = 0; idx < t->numfilenames; idx ++) {
		char *sxy = t->filenames[idx];
		// Con_PrintLinef ("%d: %s", idx, sxy);
		Add_SaveFile (sxy); // Get_SavesListGameDirOnly
	} // for

	FS_FreeSearch_Null_ (t);
}

void M_Menu_Load2_f (cmd_state_t *cmd)
{
	KeyDest_Set (key_menu); // key_dest = key_menu;
	menu_state_set_nova (m_load2);
	m_entersound = true;

	Get_SavesListGameDirOnly (); // Refresh list

	if (menu_state_reenter) {
		// Re-entrant
		// menu_state_reenter = 0;
	} else {
		local_sort_is_ascending = true; // Low high, this means alphabetical?
		m_load2_sort_by = SAVELOAD_SORTBY_SAVENAME_0;
	}

	m_load2_sort_is_dirty = true;
	if (m_load2_sort_is_dirty) {
		stringlistsort_custom	(&g_saves_indexes_4byte, fs_make_unique_false, saveload_sort);
		m_load2_sort_is_dirty = false;
	}

	startrow = not_found_neg1, endrow = not_found_neg1;

	if (local_cursor >= local_count)
		local_cursor = local_count - 1;
	if (local_cursor < 0)
		local_cursor = 0;

	menu_state_reenter = 0; m_load2_oldload_cursor = -1; m_load2_scroll_is_blocked = false;
}


RELATED_ (Get_SavesListGameDirOnly M_ScanSaves)
RELATED_ (LinkVideoTexture)

//#pragma message ("SAVE GAME TEXTURE MUST HANDLE VIDEO RESTART, GAMEDIR CHANGE")


RELATED_ (M_Menu_Load2_f)

#include "cl_screen.h"

typedef struct {
	bgra4		*savif_bgra_pels;	// Allocated on create, freed on destroy.  Memcpy new screenshots into it.
	char		supername[MAX_QPATH_128];
	int			superw;
	float		aspecthdivw; // 1200 / 600
	int			superh;
	int			aspectheight;
	int			is_fixed_size;
} saveo_t;

saveo_t saveo;

// DYNX: 0
CALLERS_ (SavegameTextureChange)
static void SavegameTextureChange_TextureCreate (void) // Called on change if no pixels
{
	if (saveo.savif_bgra_pels == NULL) {
		saveo.savif_bgra_pels = (bgra4 *)Z_Malloc (RGBA_4 * SAVEGAME_PIC_WIDTH_512 * SAVEGAME_PIC_HEIGHT_320);
	}

	dynamic_baker_texture_t bking = {0};
	Dynamic_Baker_Texture2D_Prep (&bking, q_is_dirty_true, SAVEGAME_PIC_NAME, saveo.savif_bgra_pels, SAVEGAME_PIC_WIDTH_512, SAVEGAME_PIC_HEIGHT_320);
}

// DYNX: Change - very specific
CALLERS_ (M_Load2_Draw)
static void SavegameTextureChange (const char *s_jpeg_string_base64)
{
	extern int image_width, image_height;

	if (saveo.savif_bgra_pels == NULL) {
		SavegameTextureChange_TextureCreate ();
	}

	// Baker: This either finds it in list (nothing is done) or creates a texture.
	//bgra4 *screenshotpels = SCR_Screenshot_Get_JPEG_BGRA4_From_Save_File_ZAlloc (s_save);
	bgra4 *screenshotpels = NULL;

	if (s_jpeg_string_base64 && s_jpeg_string_base64[0]) {
		screenshotpels = Jpeg_Base64_BGRA_Decode_ZAlloc (s_jpeg_string_base64);
	}

	if (screenshotpels == NULL) {
		// Save does not have it
		memset (saveo.savif_bgra_pels, /*black*/ 0, RGBA_4 * SAVEGAME_PIC_WIDTH_512 * SAVEGAME_PIC_HEIGHT_320);
	} else if (image_width != SAVEGAME_PIC_WIDTH_512 || image_height != SAVEGAME_PIC_HEIGHT_320) {
		// Save has some other size
		Con_DPrintLinef ("Save has unsupported image size %d x %d", image_width, image_height);
	} else {
		// We have a screenshot
		memcpy (saveo.savif_bgra_pels, screenshotpels, RGBA_4 * SAVEGAME_PIC_WIDTH_512 * SAVEGAME_PIC_HEIGHT_320);
	}

	Mem_FreeNull_ (screenshotpels);

	RELATED_ (DrawQ_SuperPic_Video)
	dynamic_baker_texture_t king = {0};
	Dynamic_Baker_Texture2D_Prep (&king, q_is_dirty_true, SAVEGAME_PIC_NAME, saveo.savif_bgra_pels, SAVEGAME_PIC_WIDTH_512, SAVEGAME_PIC_HEIGHT_320);
}

//Menu_Restart ();

RELATED_ (UnlinkVideoTexture)
CALLERS_ (M_Load2_Draw) // like SAVEGAME_PIC_NAME
static void DynamicTexturePurge (ccs *dynamic_pic_name, bgra4 *image_pels, int width, int height) // Only called when there are no save games.
{
	// free the texture (this does not destroy the cachepic_t, which is external)
	// CPIFX: Baker: I think this is called to clear the buff free old pic
	dynamic_baker_texture_t king = {0};

	// Baker: Note that Dynamic_Baker_Texture2D_Prep reloads the texture from the pels
	Dynamic_Baker_Texture2D_Prep (&king, q_is_dirty_false, dynamic_pic_name, image_pels, SAVEGAME_PIC_WIDTH_512, SAVEGAME_PIC_HEIGHT_320);
	Draw_FreePic	(dynamic_pic_name); // doesn't free the pic, runs R_SkinFrame_PurgeSkinFrame
	king.bpic->skinframe = NULL; // ? Q1SKY
}

static void M_Load2_Draw (void)
{
	//int oldstartrow = startrow; // We save this for what reason?
	if (m_load2_sort_is_dirty) {
		RELATED_ (saveload_sort)
		RELATED_ (m_load2_is_sort_ascending)
		RELATED_ (m_load2_sort_by)
		stringlistsort_custom	(&g_saves_indexes_4byte, fs_make_unique_false, saveload_sort);
		m_load2_sort_is_dirty = false;
	}

	drawidx = 0; drawsel_idx = not_found_neg1;  // PPX_Start - does not have frame cursor

	int menu_width_here = 512; // Baker: I added the columns  == 64
	M_Background (menu_width_here, vid_conheight.integer, q_darken_true);
	if (menu_x < 0)
		menu_x = 0; // Deal with it.

	// 400 is 800 x 600 mode
	// 533 is 1600 x 900 mode
	// 586.xx is 1760 x 990 mode
//	int screen_columns = (int)vid_conwidth.integer/8;
//	int numcolumns     = 64;
//	int screen_rows    = (int)vid_conheight.integer/8;
//	int offsetx        = screen_columns/64;


	// scroll the list as the cursor moves
	drawcur_y = 7 * 8; // 56

	int usable_height = menu_height /*640*/ - drawcur_y /*row_0_at_56*/;
	visiblerows = (usable_height / 8) - 3; // Baker: Room for bottom

	// Baker: Do it this way because a short list may have more visible rows than the list count
	// so using bound doesn't work.
	if (local_scroll_is_blocked == false) {
		startrow = local_cursor - (visiblerows / 2);

		if (startrow > local_count - visiblerows)
			startrow = local_count - visiblerows;
		if (startrow < 0)
			startrow = 0; // visiblerows can exceed local_count
	}

	endrow = Smallest (startrow + visiblerows, local_count);

	// The size of the screenshots are ...
	// Screenshot_To_Jpeg_String_Malloc_512_320
	// Aspect ratio = 512/320 1.6 or height is 0.625 or 62.5% of width

	int row_0_at_56				= drawcur_y;
	int title_row_0				= drawcur_y - (2 * 8);

	//                                         Width
	int column_1				= ( 1 * 8); // 12 + 1 = 13 .... 14  "here.sav"
	int column_2				= (14 * 8); // 10 + 1 = 11 .... 25  "start"
	int column_3				= (25 * 8); //  9 + 1 = 10 .... 35  "progress"
	int column_4				= (37 * 8); // 10 + 1 = 11 .... 46  "2 minutes"
	int column_5x				= (49 * 8); //  BEYOND
	//int column_6				= (65 * 8); //

	int line_thickness_2 = 0.5;
//	int line_margin = 2;

	// Make this use the conwidth
#if 0
	int picwidth_max			= vid_conwidth.integer - (menu_x + column_5 + 6 /*px*/ + 4) ;
	int mpicwidth = bound(50, 256 + 6, picwidth_max); // 512 x 320 is pic size 1.6 ... 0.625
	int mpicheight = mpicwidth * 0.625;
#endif
	int mpicwidth = 256 * 0.60;
	int mpicheight = mpicwidth * 0.625;
	int column_5 = vid_conwidth.integer - (mpicwidth + 6 + 4 + menu_x);

	if (column_5 > column_5x) {
		// We have extra space
		column_5 = column_5x;
	}

	int row_width_red_376		= (48 * 8);

	// If userdir, print it at the bottom of the screen.
	if (fs_userdir[0]) {
		va_super (tmp, 1024, "home dir: %s", fs_userdir);
		M_PrintBronzey	(6 * 8, vid_conheight.integer - 20 , tmp);
	}

	cachepic_t *p0 = Draw_CachePic ("gfx/p_load"); // size of this is 104 x 24

	// Baker: We may not get full canvas so improvise.
	int poshere = (vid_conwidth.integer - Draw_GetPicWidth(p0)) / 2;
	poshere -=  menu_x; // // It will add the menu_x so subtract it
	//M_DrawPic((menu_width_here - Draw_GetPicWidth(p0)) / 2, 4, "gfx/p_load", HOTSPOTS_NONE_0, NA0, NA0);
	M_DrawPic(poshere, 4, "gfx/p_load", HOTSPOTS_NONE_0, NA0, NA0);

	// drawcur_y = 7 * 8; is set above
	if (endrow > startrow) {
		for (int n = startrow; n < endrow; n++) {
			// local cursor index translated to sort index

			if (!in_range_beyond (0, n, local_count))
				continue;

			RELATED_ (M_Dump_f)
			int pidx = *((int *)&g_saves_indexes_4byte.strings[/*the index here*/ n][0]);

			if (n == local_cursor)
				drawsel_idx = (n - startrow) /*relative*/;

			// This adds a hotspot to the menu item, it draws nothing ...
			Hotspots_Add2	(menu_x + column_1 - 8/*(9 * 8)*/, menu_y + drawcur_y, row_width_red_376, 8, 1, hotspottype_button, n);

			// 12 12
			RELATED_ (M_Dump_f)
			M_ItemPrint		(column_1, drawcur_y, g_saves_filename_noExt_crop.strings[pidx], q_unghosted_true);
			M_ItemPrint		(column_2, drawcur_y, g_saves_mapnames_crop.strings[pidx], q_unghosted_true);
			M_ItemPrint		(column_3, drawcur_y, g_saves_comment.strings[pidx], q_unghosted_true);
			M_ItemPrint		(column_4, drawcur_y, g_saves_filename_timestr.strings[pidx], q_unghosted_true);

			drawcur_y += 8;
		} // for
	} // endrow > startrow
	else
	{
		M_Print(80, drawcur_y, "No Saves found");
	}

	// Determine underline color based on ascending
	float underline_color3[3] = {0.32, 0.32, 0.32}; // White
	if (local_sort_is_ascending) {
		underline_color3[1] = underline_color3[2] = 0; // RED
	}

	// DRAW COLUMN HEADERS
drawcolumnheaders:

	drawcur_y = title_row_0;
	int headidx		= 0;  // Hotspot idx, we are going -1 and lower

	#define DRAW_UNDERLINE_COLUMN \
		DrawQ_Fill(menu_x + drawcur_x, menu_y + drawcur_y + 9, draw_cur_w, 1 /*draw_cur_h*/, VECTOR3_SEND(underline_color3),  \
			alpha_1_0, DRAWFLAG_NORMAL_0) // Ender

	#define HOTSPOT_DRAW_ADD \
		Hotspots_Add2 (menu_x + drawcur_x, menu_y + drawcur_y, draw_cur_w, (8 * 1) + 1, /*count*/ 1,  hotspottype_listitem, --headidx); \
		drawcur_x += (draw_cur_w + /*space*/ 1 * 8) // Ender

	int drawcur_x	= column_1;

	int draw_cur_w;

	draw_cur_w = 12 /*chars*/ * 8;
	if (local_sort_by == SAVELOAD_SORTBY_SAVENAME_0) {DRAW_UNDERLINE_COLUMN;}
		M_PrintBronzey(drawcur_x, drawcur_y, "Save");	HOTSPOT_DRAW_ADD;

	draw_cur_w = 12 /*chars*/ * 8; // column width
	if (local_sort_by == SAVELOAD_SORTBY_MAP_1) {DRAW_UNDERLINE_COLUMN;}
		M_PrintBronzey(drawcur_x, drawcur_y, "Map");	HOTSPOT_DRAW_ADD;
		//drawcur_x += (/*1 spaces*/ 1 * 8);  // SPACES

	draw_cur_w = 9 /*chars*/ * 8; // column width
	if (local_sort_by == SAVELOAD_SORTBY_PROGRESS_2) {DRAW_UNDERLINE_COLUMN;}
		M_PrintBronzey(drawcur_x, drawcur_y, "Progress");	HOTSPOT_DRAW_ADD;
		//drawcur_x += (/*1 spaces*/ 1 * 8);

	draw_cur_w = 9 /*chars*/ * 8;
	if (local_sort_by == SAVELOAD_SORTBY_AGE_3) {DRAW_UNDERLINE_COLUMN;}
		M_PrintBronzey(drawcur_x, drawcur_y, "Age");	HOTSPOT_DRAW_ADD;
		//drawcur_x += (/*1 spaces*/ 1 * 8);

	PPX_DrawSel_End ();

	// Load new pic if applicable
	// This is not the draw phase
	// Baker: Column headers are mouse interactive only and do not affect local cursor
	int effective_cursor = local_cursor;

	while (effective_cursor != m_load2_oldload_cursor || menu_texture_stale) {
		if (local_count == 0) {
			// We have no picture
			DynamicTexturePurge (SAVEGAME_PIC_NAME, saveo.savif_bgra_pels, SAVEGAME_PIC_WIDTH_512, SAVEGAME_PIC_HEIGHT_320);
			break;
		}

		int pidx = *((int *)&g_saves_indexes_4byte.strings[effective_cursor][0]);
		SavegameTextureChange (g_saves_jpeg_strings.strings[pidx]);

		m_load2_oldload_cursor = effective_cursor;
		menu_texture_stale = false; // What of vid_restart?
		break;
	} // while 1

	// Draw Phase
draw_save_info_pane:
	while (local_count > 0) {
		// "" is no jpeg string
//		int lessor = (line_thickness_2 + line_margin) * 2;
		int pidx = *((int *)&g_saves_indexes_4byte.strings[effective_cursor][0]);

		// Baker: Non menu functions need to add the menu_x / menu_y :(
		// Black background behind
		DrawQ_Pic(menu_x + column_5, menu_y + row_0_at_56,
			Draw_CachePic("white"), mpicwidth + 6, mpicheight + 6, q_rgba_black, DRAWFLAG_NORMAL_0);
		DrawQ_Fill_Box (menu_x + column_5, menu_y + row_0_at_56, mpicwidth + 6, mpicheight + 6, q_rgba_solid_gray25_4_parms, DRAWFLAG_NORMAL_0, line_thickness_2);

		if (g_saves_jpeg_strings.strings[pidx][0] == 0) { // pIdx here
			// No picture - draw black rectangle for no savegame pic
			DrawQ_Fill (
					menu_x + column_5 + 3,
					menu_y + row_0_at_56 + 3,
					mpicwidth, mpicheight,
					q_rgba_black,
					DRAWFLAG_NORMAL_0
				);

			M_PrintBronzey (column_5 + mpicwidth/ 2 - (4.5 * 8) , row_0_at_56 + 24 + 24, "No preview");
		} else {
			p0 = Draw_CachePic_Flags (SAVEGAME_PIC_NAME, CACHEPICFLAG_NOTPERSISTENT);
			DrawQ_Pic (
				menu_x + column_5 + 3,
				menu_y + row_0_at_56 + 3,
				p0,
				mpicwidth, mpicheight,
				q_rgba_white,
				DRAWFLAG_NORMAL_0
			);
		}

		// Map name
		int text_x = column_5 + (1 * 8);
		int text_y = row_0_at_56 + (1 * 8);

		DrawQ_Fill (
				menu_x + text_x,
				menu_y + text_y - 4,
				mpicwidth - (4*2),
				4 + (1 * 8) + 4,
				q_rgba_alpha75_black_4_parms,
				DRAWFLAG_NORMAL_0
			);

		M_Print (text_x, text_y, g_saves_mapnames_crop.strings[pidx]); // pIdx here

		text_x = column_5 + 8;
		text_y = row_0_at_56 + (10 * 8);

		// Progress
		DrawQ_Fill (
			menu_x + text_x,
			menu_y + text_y - 4,
			mpicwidth - (4 * 2),
			4 + (2 * 8) + 4,
			q_rgba_alpha25_black_4_parms,
			DRAWFLAG_NORMAL_0
		);

		M_Print (text_x, text_y, g_saves_comment.strings[pidx]); // pIdx here

		text_x = column_5 + 8; // - menu_x + 0;
		text_y = row_0_at_56 + (11 * 8);

		// Age - Days ago
		M_Print (text_x, text_y, g_saves_filename_timestr.strings[pidx]); // pIdx here
		break;
	} // while 1 - show save game preview with screenshot

}

static void M_Load2_Key(cmd_state_t *cmd, int key, int ascii)
{
	int lcase_ascii;

	local_scroll_is_blocked = false;

	switch (key) {
	case K_MOUSE2: // fall thru to K_ESCAPE and then exit
	case K_ESCAPE:
		M_Menu_Main_f(cmd);
		break;

	case K_MOUSE1:
		if (hotspotx_hover == not_found_neg1)
			break;

#if 1 // DID WE CLICK A COLUMN HEADER THEN SET SORT AND GET OUT --- BLOCK
		{
			// This detects MOUSE collision with column headers
			// And sets the sort
			hotspotx_s *h = &hotspotxs[hotspotx_hover];
			if (h->hotspottype == hotspottype_listitem) {
				int sort_idx_for_hotspot = -(h->trueidx + 1);
				int did_click_the_current_sort_index = m_load2_sort_by == sort_idx_for_hotspot;

				m_load2_sort_by = sort_idx_for_hotspot;
				m_load2_sort_is_dirty = true;

				// IF WE CLICK THE CURRENT SORT INDEX, REVERSE IT
				if (did_click_the_current_sort_index) {
					// If clicked what is already the key, toggle it.
					m_load2_is_sort_ascending = !m_load2_is_sort_ascending;
				} else {
					int shall_invert = isin2 (m_load2_sort_by, SAVELOAD_SORTBY_AGE_3, SAVELOAD_SORTBY_PROGRESS_2);
					m_load2_is_sort_ascending = shall_invert ? false : true; // Low to high
				}

				break; // HIT A COLUMN HEADER, GET OUT.
			}
		}
#endif
#if 1 // DOUBLE CLICK LOGIC
		{
			int new_cursor = hotspotx_hover + startrow;
			int is_new_cursor = new_cursor != local_cursor;

			local_scroll_is_blocked = true; // PROTECT AGAINST AUTOSCROLL

			local_cursor = new_cursor;

			if (is_new_cursor) {
				// GET OUT!  SET FOCUS TO ITEM
				// Commit_To_Cname ();
				break;
			}

			// Same cursor -- double click in effect.
			// fall thru
			double new_click_time = Sys_DirtyTime();
			double click_delta_time = local_click_time ? (new_click_time - local_click_time) : 0;
			local_click_time = new_click_time;

			if (is_new_cursor == false && click_delta_time && click_delta_time < DOUBLE_CLICK_0_5) {
				// fall thru and load the game
			} else {
				// Entry changed or not fast enough
				break;
			}
		} // block
#endif

	case K_ENTER:
		S_LocalSound ("sound/misc/menu2.wav");
		if (local_count) {
			// local cursor index translated to sort index
			int pidx = *((int *)&g_saves_indexes_4byte.strings[local_cursor][0]);
			ccs *s_savename = g_saves_filename_noExt.strings[pidx];// pIdx here
			ccs *s_map = g_saves_mapnames.strings[pidx];// pIdx here

			va_super (tmp, 1024, "load %s // %s", s_savename, s_map);

			Cbuf_AddTextLine (cmd, tmp);

			Key_History_Push_String (tmp);

			// Load 2 is NOW re-entrant -- July 19 2026
			if (m_state == m_load2) {
				menu_state_reenter = m_load2;
			}
			KeyDest_Set (key_game); // key_dest = key_game;
			menu_state_set_nova (m_none);
		}

		break;

	case K_SPACE:
		break;

	case K_HOME:
		if (local_count)
			local_cursor = 0;
		break;

	case K_END:
		if (local_count)
			local_cursor = local_count - 1;
		break;

	case K_PGUP:
		local_cursor -= visiblerows / 2;
		if (local_cursor < 0) // PGUP does not wrap, stops at start
			local_cursor = 0;
		break;

	case K_MWHEELUP:
		local_cursor -= visiblerows / 4;
		if (local_cursor < 0) // K_MWHEELUP does not wrap, stops at start
			local_cursor = 0;
		break;

	case K_PGDN:
		local_cursor += visiblerows / 2;
		if (local_cursor >= local_count) // PGDN does not wrap, stops at end
			local_cursor = local_count - 1;
		break;

	case K_MWHEELDOWN:
		local_cursor += visiblerows / 4;
		if (local_cursor >= local_count)
			local_cursor = local_count - 1;
		break;

	case K_UPARROW:
		//S_LocalSound ("sound/misc/menu1.wav");
		local_cursor--;
		if (local_cursor < 0) // K_UPARROW wraps around to end EXCEPT ON MEGA LISTS
			local_cursor = 0; // MEGA EXCEPTION
		break;

	case K_DOWNARROW:
		//S_LocalSound ("sound/misc/menu1.wav");
		local_cursor ++;
		if (local_cursor >= local_count) // K_DOWNARROW wraps around to start EXCEPT ON MEGA LISTS
			local_cursor = local_count - 1 ; // MEGA EXCEPTION
		break;

	case K_LEFTARROW:
		break;

	case K_RIGHTARROW:
		break;

	default:
		lcase_ascii = tolower(ascii);
		if (in_range ('a', lcase_ascii, 'z')) {
			// Baker: This is a wraparound seek
			// Find the next item starting with 'a'
			// or whatever key someone pressed
			int startx = local_cursor;
			char sprefix[2] = { (char)lcase_ascii, 0 };

#if 0
			for (int iters = 0; iters < local_count; iters ++) {
				startx ++;
				if (startx >= local_count) {
					startx = 0;
				}

				char *sx = g_saves_filename_noExt_crop.strings[startx];

				if (String_Starts_With_Caseless (sx, sprefix)) {
					local_cursor = startx;
					break;
				} // if
			} // iters
#endif

			//int pidx = *((int *)&g_saves_indexes_4byte.strings[/*the index here*/ n][0]);

			//if (n == local_cursor)
			//	drawsel_idx = (n - startrow) /*relative*/;

			//// This adds a hotspot to the menu item, it draws nothing ...
			//Hotspots_Add2	(menu_x + column_1 - 8/*(9 * 8)*/, menu_y + drawcur_y, row_width_red_376, 8, 1, hotspottype_button, n);

			//// 12 12
			//RELATED_ (M_Dump_f)
			//M_ItemPrint		(column_1, drawcur_y, g_saves_filename_noExt_crop.strings[pidx], q_unghosted_true);

			for (int iters = 0; iters < local_count; iters ++) {
				startx ++;
				if (startx >= local_count) {
					startx = 0;
				}

				//char *sx = g_saves_filename_noExt_crop.strings[startx];
				int pidx = *((int *)&g_saves_indexes_4byte.strings[/*the index here*/ startx][0]);
				ccs *sx = g_saves_filename_noExt.strings[pidx];// pIdx here

				if (String_Starts_With_Caseless (sx, sprefix)) {
					local_cursor = startx;
					break;
				} // if
			} // iters

		} // if a-z
		break;
	} // sw

}

#undef	local_count
#undef	local_cursor
#undef 	visiblerows
#undef 	startrow
#undef 	endrow
#undef  local_click_time
#undef  local_sort_by
#undef  local_is_sort_ascending
#undef	local_sort_is_dirty
#undef  local_scroll_is_blocked


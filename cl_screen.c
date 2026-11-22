// cl_screen.c


#include "quakedef.h"
#include "cl_video.h"
#include "image.h"
#include "jpeg.h"
#include "image_png.h"
#include "cl_collision.h"
#include "libcurl.h"
#include "csprogs.h"
#include "r_stats.h"
#ifdef CONFIG_VIDEO_CAPTURE
#include "cap_avi.h"
#include "cap_ogg.h"
#endif

// we have to include snd_main.h here only to get access to snd_renderbuffer->format.speed when writing the AVI headers
#include "snd_main.h"

cvar_t scr_levelshot_auto = {CF_SERVER | CF_ARCHIVE, "scr_levelshot_auto", "1", "Automatic levelshot 2 seconds into maps. [Zircon]"};

cvar_t scr_viewsize = {CF_CLIENT | CF_ARCHIVE, "viewsize","100", "how large the view should be, 110 disables inventory bar, 120 disables status bar"};
cvar_t scr_fov = {CF_CLIENT | CF_ARCHIVE, "fov","90", "field of vision, 1-170 degrees, default 90, some players use 110-130"};
cvar_t scr_conalpha = {CF_CLIENT | CF_ARCHIVE, "scr_conalpha", "1", "opacity of console background gfx/conback"};
cvar_t scr_conalphafactor = {CF_CLIENT | CF_ARCHIVE, "scr_conalphafactor", "1", "opacity of console background gfx/conback relative to scr_conalpha; when 0, gfx/conback is not drawn"};
cvar_t scr_conalpha2factor = {CF_CLIENT | CF_ARCHIVE, "scr_conalpha2factor", "0", "opacity of console background gfx/conback2 relative to scr_conalpha; when 0, gfx/conback2 is not drawn"};
cvar_t scr_conalpha3factor = {CF_CLIENT | CF_ARCHIVE, "scr_conalpha3factor", "0", "opacity of console background gfx/conback3 relative to scr_conalpha; when 0, gfx/conback3 is not drawn"};
cvar_t scr_conbrightness = {CF_CLIENT | CF_ARCHIVE, "scr_conbrightness", "1", "brightness of console background (0 = black, 1 = image)"};
cvar_t scr_conforcewhiledisconnected = {CF_CLIENT, "scr_conforcewhiledisconnected", "1", "1 forces fullscreen console while disconnected, 2 also forces it when the listen server has started but the client is still loading"}; // SEPUS
cvar_t scr_conheight = {CF_CLIENT | CF_ARCHIVE, "scr_conheight", "0.5", "fraction of screen height occupied by console (reduced as necessary for visibility of loading progress and infobar)"};

cvar_t scr_xcutoutpos = {CF_CLIENT, "scr_xcutoutpos", "", "cut-out position [Zircon]"};
cvar_t scr_xcutoutsize = {CF_CLIENT, "scr_xcutoutsize", "", "cut-out size [Zircon]"};
cvar_t scr_xtranslate = {CF_CLIENT, "scr_xtranslate", "", "vector to translate form coordinates cut-out size in full canvas pixels [Zircon]"};

cvar_t scr_conscroll_x = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll_x", "0", "scroll speed of gfx/conback in x direction"};
cvar_t scr_conscroll_y = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll_y", "0", "scroll speed of gfx/conback in y direction"};
cvar_t scr_conscroll2_x = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll2_x", "0", "scroll speed of gfx/conback2 in x direction"};
cvar_t scr_conscroll2_y = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll2_y", "0", "scroll speed of gfx/conback2 in y direction"};
cvar_t scr_conscroll3_x = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll3_x", "0", "scroll speed of gfx/conback3 in x direction"};
cvar_t scr_conscroll3_y = {CF_CLIENT | CF_ARCHIVE, "scr_conscroll3_y", "0", "scroll speed of gfx/conback3 in y direction"};
cvar_t csqc_full_width_height = {CF_CLIENT, "csqc_full_width_height", "0", "csqc always gets a full client-sized canvas, vid_conwidth and vid_conheight are vid.width vid.height during csqc draws. 0 - Don't 1 - Yes [Zircon]"}; // Baker r7003 - option for csqc to get full vid.width/vid.height canvas
cvar_t csqc_full_width_height_available = {CF_CLIENT /*| CF_READONLY*/, "csqc_full_width_height_available", "1", "Indicates ability for csqc to gain access full width/height resolution, otherwise a mod would want to set vid_conwidth in csqc like Xonotic [Zircon]"}; // Baker r7004
cvar_t menuqc_full_width_height = {CF_CLIENT, "menuqc_full_width_height", "0", "Menu QC m_draw always gets a full client-sized canvas, vid_conwidth and vid_conheight are vid.width vid.height during Menu QC draws. 0 - Don't 1 - Yes [Zircon]"}; // Baker: Jan 27 2026
#ifdef CONFIG_MENU
cvar_t scr_menuforcewhiledisconnected = {CF_CLIENT, "scr_menuforcewhiledisconnected", "0", "forces menu while disconnected"};
#endif
cvar_t scr_centertime = {CF_CLIENT, "scr_centertime","2", "how long centerprint messages show"};
cvar_t scr_showram = {CF_CLIENT | CF_ARCHIVE, "showram","1", "show ram icon if low on surface cache memory (not used)"};
cvar_t scr_showturtle = {CF_CLIENT | CF_ARCHIVE, "showturtle","0", "show turtle icon when framerate is too low"};
cvar_t scr_showpause = {CF_CLIENT | CF_ARCHIVE, "showpause","1", "show pause icon when game is paused"};
cvar_t scr_showbrand = {CF_CLIENT, "showbrand","0", "shows gfx/brand.tga in a corner of the screen (different values select different positions, including centered)"};
cvar_t scr_printspeed = {CF_CLIENT, "scr_printspeed","0", "speed of intermission printing (episode end texts), a value of 0 disables the slow printing"};
cvar_t scr_loadingscreen_background = {CF_CLIENT, "scr_loadingscreen_background","0", "show the last visible background during loading screen (costs one screenful of video memory)"};
cvar_t scr_loadingscreen_scale = {CF_CLIENT, "scr_loadingscreen_scale","1", "scale factor of the background"};
cvar_t scr_loadingscreen_scale_base = {CF_CLIENT, "scr_loadingscreen_scale_base","0", "0 = console pixels, 1 = video pixels"};
cvar_t scr_loadingscreen_scale_limit = {CF_CLIENT, "scr_loadingscreen_scale_limit","0", "0 = no limit, 1 = until first edge hits screen edge, 2 = until last edge hits screen edge, 3 = until width hits screen width, 4 = until height hits screen height"};
cvar_t scr_loadingscreen_picture = {CF_CLIENT, "scr_loadingscreen_picture", "gfx/loading", "picture shown during loading"};
cvar_t scr_loadingscreen_count = {CF_CLIENT, "scr_loadingscreen_count","1", "number of loading screen files to use randomly (named loading.tga, loading2.tga, loading3.tga, ...)"};
cvar_t scr_loadingscreen_firstforstartup = {CF_CLIENT, "scr_loadingscreen_firstforstartup","0", "remove loading.tga from random scr_loadingscreen_count selection and only display it on client startup, 0 = normal, 1 = firstforstartup"};

cvar_t scr_loadingscreen_barcolor = {CF_CLIENT, "scr_loadingscreen_barcolor", "0.25 0.25 0.25", "rgb color of loadingscreen progress bar [Zircon default]"}; // Baker r8003 gray progress bar is the default color.

cvar_t scr_loadingscreen_barheight = {CF_CLIENT, "scr_loadingscreen_barheight", "0", "the height of the loadingscreen progress bar.  A setting of 0 will not show the bar, but if 1.5 seconds elapse the bar will show so you know the (maybe gigantic) map is actually loading [Zircon default]"}; // Baker r8082: Was going to set 0, but some of the slow loading giant Quake maps, this increases confidence it is actually loading and not frozen
cvar_t scr_loadingscreen_maxfps = {CF_CLIENT, "scr_loadingscreen_maxfps", "10", "maximum FPS for loading screen so it will not update very often (this reduces loading time with lots of models)"}; // Baker r1462: Faster load times.
cvar_t scr_infobar_height = {CF_CLIENT, "scr_infobar_height", "8", "the height of the infobar items"};
cvar_t vid_conwidth = {CF_CLIENT | CF_ARCHIVE, "vid_conwidth", "640", "virtual width of 2D graphics system (note: changes may be overwritten)"};
cvar_t vid_conheight = {CF_CLIENT | CF_ARCHIVE, "vid_conheight", "480", "virtual height of 2D graphics system"};

cvar_t vid_pixelheight = {CF_CLIENT | CF_ARCHIVE, "vid_pixelheight", "1", "adjusts vertical field of vision to account for non-square pixels (1280x1024 on a CRT monitor for example)"};
cvar_t scr_screenshot_jpeg = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_jpeg","1", "save jpeg instead of targa"};
cvar_t scr_screenshot_jpeg_quality = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_jpeg_quality","0.9", "image quality of saved jpeg"};
cvar_t scr_screenshot_png = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_png","0", "save png instead of targa"};
cvar_t scr_screenshot_gammaboost = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_gammaboost","1", "gamma correction on saved screenshots and videos, 1.0 saves unmodified images"};
cvar_t scr_screenshot_alpha = {CF_CLIENT, "scr_screenshot_alpha","0", "try to write an alpha channel to screenshots (debugging feature)"};
cvar_t scr_screenshot_timestamp = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_timestamp", "1", "use a timestamp based number of the type YYYYMMDDHHMMSSsss instead of sequential numbering"};
// scr_screenshot_name is defined in fs.c
#ifdef CONFIG_VIDEO_CAPTURE
cvar_t cl_capturevideo = {CF_CLIENT, "cl_capturevideo", "0", "enables saving of video to a .avi file using uncompressed I420 colorspace and PCM audio, note that scr_screenshot_gammaboost affects the brightness of the output)"};
cvar_t cl_capturevideo_demo_stop = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_demo_stop", "1", "automatically stops video recording when demo ends"};
cvar_t cl_capturevideo_printfps = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_printfps", "1", "prints the frames per second captured in capturevideo (is only written to the log file, not to the console, as that would be visible on the video)"};
cvar_t cl_capturevideo_width = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_width", "0", "scales all frames to this resolution before saving the video"};
cvar_t cl_capturevideo_height = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_height", "0", "scales all frames to this resolution before saving the video"};
cvar_t cl_capturevideo_realtime = {CF_CLIENT, "cl_capturevideo_realtime", "0", "causes video saving to operate in realtime (mostly useful while playing, not while capturing demos), this can produce a much lower quality video due to poor sound/video sync and will abort saving if your machine stalls for over a minute"};
cvar_t cl_capturevideo_fps = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_fps", "30", "how many frames per second to save (29.97 for NTSC, 30 for typical PC video, 15 can be useful)"};
cvar_t cl_capturevideo_nameformat = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_nameformat", "dpvideo", "prefix for saved videos (the date is encoded using strftime escapes)"};
cvar_t cl_capturevideo_number = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_number", "1", "number to append to video filename, incremented each time a capture begins"};
cvar_t cl_capturevideo_ogg = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_ogg", "1", "save captured video data as Ogg/Vorbis/Theora streams"};
cvar_t cl_capturevideo_framestep = {CF_CLIENT | CF_ARCHIVE, "cl_capturevideo_framestep", "1", "when set to n >= 1, render n frames to capture one (useful for motion blur like effects)"};
#endif
cvar_t r_letterbox = {CF_CLIENT, "r_letterbox", "0", "reduces vertical height of view to simulate a letterboxed movie effect (can be used by mods for cutscenes)"};
cvar_t r_stereo_separation = {CF_CLIENT, "r_stereo_separation", "4", "separation distance of eyes in the world (negative values are only useful for cross-eyed viewing)"};
cvar_t r_stereo_sidebyside = {CF_CLIENT, "r_stereo_sidebyside", "0", "side by side views for those who can't afford glasses but can afford eye strain (note: use a negative r_stereo_separation if you want cross-eyed viewing)"};
cvar_t r_stereo_horizontal = {CF_CLIENT, "r_stereo_horizontal", "0", "aspect skewed side by side view for special decoder/display hardware"};
cvar_t r_stereo_vertical = {CF_CLIENT, "r_stereo_vertical", "0", "aspect skewed top and bottom view for special decoder/display hardware"};
cvar_t r_stereo_redblue = {CF_CLIENT, "r_stereo_redblue", "0", "red/blue anaglyph stereo glasses (note: most of these glasses are actually red/cyan, try that one too)"};
cvar_t r_stereo_redcyan = {CF_CLIENT, "r_stereo_redcyan", "0", "red/cyan anaglyph stereo glasses, the kind given away at drive-in movies like Creature From The Black Lagoon In 3D"};
cvar_t r_stereo_redgreen = {CF_CLIENT, "r_stereo_redgreen", "0", "red/green anaglyph stereo glasses (for those who don't mind yellow)"};
cvar_t r_stereo_angle = {CF_CLIENT, "r_stereo_angle", "0", "separation angle of eyes (makes the views look different directions, as an example, 90 gives a 90 degree separation where the views are 45 degrees left and 45 degrees right)"};
cvar_t scr_stipple = {CF_CLIENT, "scr_stipple", "0", "interlacing-like stippling of the display"};
cvar_t scr_refresh = {CF_CLIENT, "scr_refresh", "1", "allows you to completely shut off rendering for benchmarking purposes"};
cvar_t scr_screenshot_name_in_mapdir = {CF_CLIENT | CF_ARCHIVE, "scr_screenshot_name_in_mapdir", "0", "if set to 1, screenshots are placed in a subdirectory named like the map they are from"};
cvar_t net_graph = {CF_CLIENT | CF_ARCHIVE, "net_graph", "0", "shows a graph of packet sizes and other information, 0 = off, 1 = show client netgraph, 2 = show client and server netgraphs (when hosting a server)"};
cvar_t cl_demo_mousegrab = {CF_CLIENT, "cl_demo_mousegrab", "0", "Allows reading the mouse input while playing demos. Useful for camera mods developed in csqc. (0: never, 1: always)"};
cvar_t timedemo_screenshotframelist = {CF_CLIENT, "timedemo_screenshotframelist", "", "when performing a timedemo, take screenshots of each frame in this space-separated list - example: 1 201 401"};
cvar_t vid_touchscreen_outlinealpha = {CF_CLIENT, "vid_touchscreen_outlinealpha", "0", "opacity of touchscreen area outlines"};
cvar_t vid_touchscreen_overlayalpha = {CF_CLIENT, "vid_touchscreen_overlayalpha", "0.25", "opacity of touchscreen area icons"};

cvar_t tool_inspector = {CF_CLIENT | CL_RESETNEWMAP_0, "tool_inspector", "0", "view visible entity QC information [Zircon]"}; // Baker r0106: tool inspector
//cvar_t tool_marker = {CF_CLIENT | CL_RESETNEWMAP_0, "tool_marker", "0", "set a position to display on-screen, this value must be quoted since it is a since string [Zircon]"}; // Baker r0109: tool marker

cvar_t eq_clipboard_set = {CF_CLIENT, "eq_clipboard_set", "1", "eq command will copy result to clipboard [Zircon]"}; // Baker: Oct 14 2024

extern cvar_t sbar_info_pos;
extern cvar_t r_fog_clear;

int jpeg_supported = false;

qbool	scr_initialized;		// ready to draw

int scr_loading_ex = false;  // we are in a loading screen

unsigned int        scr_con_current;
static unsigned int scr_con_margin_bottom;

extern int	con_vislines;

extern int cl_punchangle_applied;

static void SCR_ScreenShot_f(cmd_state_t *cmd);
static void R_Envmap_f(cmd_state_t *cmd);

// backend
void R_ClearScreen(qbool fogcolor);

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

char		scr_centerstring[MAX_INPUTLINE_16384];
float		scr_centertime_start;	// for slow victory printing
float		scr_centertime_off;
int			scr_center_lines;
int			scr_erase_lines;
int			scr_erase_center;
char        scr_infobarstring[MAX_INPUTLINE_16384];
float       scr_infobartime_off;

/*
==============
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void SCR_CenterPrint(const char *str)
{
	Con_LogCenterPrint (str); // Baker r1421: centerprint logging to console
	c_strlcpy (scr_centerstring, str);
	scr_centertime_off = scr_centertime.value;
	scr_centertime_start = cl.time;

// count the number of lines for centering
	scr_center_lines = 1;
	while (*str)
	{
		if (*str == '\n')
			scr_center_lines++;
		str++;
	}
}


static void SCR_DrawCenterString (void)
{
	char	*starttext;
	int		x, y;
	int		remaining;
	int		color;

	if (cl.intermission == 2) // in finale,
		if (sb_showscores) // make TAB hide the finale message (sb_showscores overrides finale in sbar.c)
			return;

	if (scr_centertime.value <= 0 && !cl.intermission)
		return;

// the finale prints the characters one at a time, except if printspeed is an absurdly high value
	if (cl.intermission && scr_printspeed.value > 0 && scr_printspeed.value < 1000000)
		remaining = (int)(scr_printspeed.value * (cl.time - scr_centertime_start));
	else
		remaining = 9999;

	scr_erase_center = 0;
	starttext = scr_centerstring;

	if (remaining < 1)
		return;

	if (scr_center_lines <= 4)
		y = (int)(vid_conheight.integer*0.35);
	else
		y = 48;

	color = -1;
	do
	{
		// scan the number of characters on the line, not counting color codes
		char *newline = strchr(starttext, '\n');
		int maxlendraw = newline ? (newline - starttext) : (int)strlen(starttext);

		float width = DrawQ_TextWidth(starttext, maxlendraw, 8, 8, ignorecolorcodes_false, FONT_CENTERPRINT);

		x = (int) (vid_conwidth.integer - width)/2;
		if (maxlendraw > 0)
		{
			if (remaining < maxlendraw)
				maxlendraw = remaining;

			DrawQ_String(x, y, starttext, maxlendraw, /*wh*/ 8, 8, q_rgba_white, DRAWFLAG_NORMAL_0, &color, ignorecolorcodes_false, FONT_CENTERPRINT);
			remaining -= maxlendraw;
			if (remaining <= 0)
				return;
		}
		y += 8;

		if (!newline)
			break;
		starttext = newline + 1; // skip the \n
	} while (1);
}

static void SCR_CheckDrawCenterString (void)
{
	if (scr_center_lines > scr_erase_lines)
		scr_erase_lines = scr_center_lines;

	if (cl.time > cl.oldtime)
		scr_centertime_off -= cl.time - cl.oldtime;

	// don't draw if this is a normal stats-screen intermission,
	// only if it is not an intermission, or a finale intermission
	if (cl.intermission == 1)
		return;
	if (scr_centertime_off <= 0 && !cl.intermission)
		return;
	if (key_dest != key_game)
		return;

	SCR_DrawCenterString ();
}

static void SCR_DrawNetGraph_DrawGraph (int graphx, int graphy, int graphwidth, int graphheight, float graphscale, int graphlimit, const char *label, float textsize, int packetcounter, netgraphitem_t *netgraph)
{
	netgraphitem_t *graph;
	int j, x, y;
	int totalbytes = 0;
	char bytesstring[128];
	float g[NETGRAPH_PACKETS_256][7];
	float *a;
	float *b;
	DrawQ_Fill(graphx, graphy, graphwidth, graphheight + textsize * 2, 0, 0, 0, 0.5, 0);
	// draw the bar graph itself
	memset(g, 0, sizeof(g));
	for (j = 0;j < NETGRAPH_PACKETS_256;j++)
	{
		graph = netgraph + j;
		g[j][0] = 1.0f - 0.25f * (host.realtime - graph->time);
		g[j][1] = 1.0f;
		g[j][2] = 1.0f;
		g[j][3] = 1.0f;
		g[j][4] = 1.0f;
		g[j][5] = 1.0f;
		g[j][6] = 1.0f;
		if (graph->unreliablebytes == NETGRAPH_LOSTPACKET_NEG1)
			g[j][1] = 0.00f;
		else if (graph->unreliablebytes == NETGRAPH_CHOKEDPACKET)
			g[j][2] = 0.90f;
		else
		{
			if (netgraph[j].time >= netgraph[(j+NETGRAPH_PACKETS_256-1)%NETGRAPH_PACKETS_256].time)
				if (graph->unreliablebytes + graph->reliablebytes + graph->ackbytes >= graphlimit * (netgraph[j].time - netgraph[(j+NETGRAPH_PACKETS_256-1)%NETGRAPH_PACKETS_256].time))
					g[j][2] = 0.98f;
			g[j][3] = 1.0f    - graph->unreliablebytes * graphscale;
			g[j][4] = g[j][3] - graph->reliablebytes   * graphscale;
			g[j][5] = g[j][4] - graph->ackbytes        * graphscale;
			// count bytes in the last second
			if (host.realtime - graph->time < 1.0f)
				totalbytes += graph->unreliablebytes + graph->reliablebytes + graph->ackbytes;
		}
		if (graph->cleartime >= 0)
			g[j][6] = 0.5f + 0.5f * (2.0 / M_PI) * atan((M_PI / 2.0) * (graph->cleartime - graph->time));
		g[j][1] = bound(0.0f, g[j][1], 1.0f);
		g[j][2] = bound(0.0f, g[j][2], 1.0f);
		g[j][3] = bound(0.0f, g[j][3], 1.0f);
		g[j][4] = bound(0.0f, g[j][4], 1.0f);
		g[j][5] = bound(0.0f, g[j][5], 1.0f);
		g[j][6] = bound(0.0f, g[j][6], 1.0f);
	}
	// render the lines for the graph
	for (j = 0;j < NETGRAPH_PACKETS_256;j++)
	{
		a = g[j];
		b = g[(j+1)%NETGRAPH_PACKETS_256];
		if (a[0] < 0.0f || b[0] > 1.0f || b[0] < a[0])
			continue;
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[2], graphx + graphwidth * b[0], graphy + graphheight * b[2], 1.0f, 1.0f, 1.0f, 1.0f, 0);
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[1], graphx + graphwidth * b[0], graphy + graphheight * b[1], 1.0f, 0.0f, 0.0f, 1.0f, 0);
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[5], graphx + graphwidth * b[0], graphy + graphheight * b[5], 0.0f, 1.0f, 0.0f, 1.0f, 0);
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[4], graphx + graphwidth * b[0], graphy + graphheight * b[4], 1.0f, 1.0f, 1.0f, 1.0f, 0);
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[3], graphx + graphwidth * b[0], graphy + graphheight * b[3], 1.0f, 0.5f, 0.0f, 1.0f, 0);
		DrawQ_Line(1, graphx + graphwidth * a[0], graphy + graphheight * a[6], graphx + graphwidth * b[0], graphy + graphheight * b[6], 0.0f, 0.0f, 1.0f, 1.0f, 0);
	}
	x = graphx;
	y = graphy + graphheight;
	dpsnprintf(bytesstring, sizeof(bytesstring), "%d", totalbytes);
	DrawQ_String(x, y, label      , 0, textsize, textsize, 1.0f, 1.0f, 1.0f, 1.0f, 0, NULL, false, FONT_DEFAULT);y += textsize;
	DrawQ_String(x, y, bytesstring, 0, textsize, textsize, 1.0f, 1.0f, 1.0f, 1.0f, 0, NULL, false, FONT_DEFAULT);y += textsize;
}

/*
==============
SCR_DrawNetGraph
==============
*/
static void SCR_DrawNetGraph (void)
{
	int i, separator1, separator2, graphwidth, graphheight, netgraph_x, netgraph_y, textsize, index, netgraphsperrow, graphlimit;
	float graphscale;
	netconn_t *c;
	char vabuf[1024];

	if (cls.state != ca_connected)
		return;
	if (!cls.netcon)
		return;
	if (!net_graph.integer)
		return;

	separator1 = 2;
	separator2 = 4;
	textsize = 8;
	graphwidth = 120;
	graphheight = 70;
	graphscale = 1.0f / 1500.0f;
	graphlimit = cl_rate.integer;

	netgraphsperrow = (vid_conwidth.integer + separator2) / (graphwidth * 2 + separator1 + separator2);
	netgraphsperrow = max(netgraphsperrow, 1);

	index = 0;
	netgraph_x = (vid_conwidth.integer + separator2) - (1 + (index % netgraphsperrow)) * (graphwidth * 2 + separator1 + separator2);
	netgraph_y = (vid_conheight.integer - 48 - sbar_info_pos.integer + separator2) - (1 + (index / netgraphsperrow)) * (graphheight + textsize + separator2);
	c = cls.netcon;
	SCR_DrawNetGraph_DrawGraph(netgraph_x                          , netgraph_y, graphwidth, graphheight, graphscale, graphlimit, "incoming", textsize, c->incoming_packetcounter, c->incoming_netgraph);
	SCR_DrawNetGraph_DrawGraph(netgraph_x + graphwidth + separator1, netgraph_y, graphwidth, graphheight, graphscale, graphlimit, "outgoing", textsize, c->outgoing_packetcounter, c->outgoing_netgraph);
	index++;

	if (sv.active && net_graph.integer >= 2)
	{
		for (i = 0;i < svs.maxclients;i++)
		{
			c = svs.clients[i].netconnection;
			if (!c)
				continue;
			netgraph_x = (vid_conwidth.integer + separator2) - (1 + (index % netgraphsperrow)) * (graphwidth * 2 + separator1 + separator2);
			netgraph_y = (vid_conheight.integer - 48 + separator2) - (1 + (index / netgraphsperrow)) * (graphheight + textsize + separator2);
			SCR_DrawNetGraph_DrawGraph(netgraph_x                          , netgraph_y, graphwidth, graphheight, graphscale, graphlimit, va(vabuf, sizeof(vabuf), "%s", svs.clients[i].name), textsize, c->outgoing_packetcounter, c->outgoing_netgraph);
			SCR_DrawNetGraph_DrawGraph(netgraph_x + graphwidth + separator1, netgraph_y, graphwidth, graphheight, graphscale, graphlimit, ""                           , textsize, c->incoming_packetcounter, c->incoming_netgraph);
			index++;
		}
	}
}

/*
==============
SCR_DrawTurtle
==============
*/
static void SCR_DrawTurtle (void)
{
	static int	count;

	if (cls.state != ca_connected)
		return;

	if (!scr_showturtle.integer)
		return;

	if (cl.realframetime < 0.1)
	{
		count = 0;
		return;
	}

	count++;
	if (count < 3)
		return;

	DrawQ_Pic (0, 0, Draw_CachePic ("gfx/turtle"), 0, 0, 1, 1, 1, 1, 0);
}

/*
==============
SCR_DrawNet
==============
*/
static void SCR_DrawNet (void)
{
	if (cls.state != ca_connected)
		return;
	if (host.realtime - cl.last_received_message < 0.3)
		return;
	if (cls.demoplayback)
		return;

	DrawQ_Pic (64, 0, Draw_CachePic ("gfx/net"), 0, 0, 1, 1, 1, 1, 0);
}

/*
==============
DrawPause
==============
*/
static void SCR_DrawPause (void)
{
	cachepic_t	*pic;

	if (cls.state != ca_connected)
		return;

	if (!scr_showpause.integer)		// turn off for screenshots
		return;

	if (!cl.paused)
		return;

	pic = Draw_CachePic ("gfx/pause");
	DrawQ_Pic ((vid_conwidth.integer - Draw_GetPicWidth(pic))/2, (vid_conheight.integer - Draw_GetPicHeight(pic))/2, pic, 0, 0, 1, 1, 1, 1, 0);
}

/*
==============
SCR_DrawBrand
==============
*/
static void SCR_DrawBrand (void)
{
	cachepic_t	*pic;
	float		x, y;

	if (!scr_showbrand.value /*d:0*/)
		return;

	pic = Draw_CachePic ("gfx/brand");

	switch ((int)scr_showbrand.value)
	{
	case 1:	// bottom left
		x = 0;
		y = vid_conheight.integer - Draw_GetPicHeight(pic);
		break;
	case 2:	// bottom centre
		x = (vid_conwidth.integer - Draw_GetPicWidth(pic)) / 2;
		y = vid_conheight.integer - Draw_GetPicHeight(pic);
		break;
	case 3:	// bottom right
		x = vid_conwidth.integer - Draw_GetPicWidth(pic);
		y = vid_conheight.integer - Draw_GetPicHeight(pic);
		break;
	case 4:	// centre right
		x = vid_conwidth.integer - Draw_GetPicWidth(pic);
		y = (vid_conheight.integer - Draw_GetPicHeight(pic)) / 2;
		break;
	case 5:	// top right
		x = vid_conwidth.integer - Draw_GetPicWidth(pic);
		y = 0;
		break;
	case 6:	// top centre
		x = (vid_conwidth.integer - Draw_GetPicWidth(pic)) / 2;
		y = 0;
		break;
	case 7:	// top left
		x = 0;
		y = 0;
		break;
	case 8:	// centre left
		x = 0;
		y = (vid_conheight.integer - Draw_GetPicHeight(pic)) / 2;
		break;
	default:
		return;
	}

	DrawQ_Pic (x, y, pic, 0, 0, 1, 1, 1, 1, 0);
}

/*
==============
SCR_DrawQWDownload
==============
*/
static int SCR_DrawQWDownload(int offset)
{
	// sync with SCR_InfobarHeight
	int len;
	float x, y;
	float size = scr_infobar_height.value;
	char temp[256];

	if (!cls.qw_downloadname[0]) {
		cls.qw_downloadspeedrate = 0;
		cls.qw_downloadspeedtime = host.realtime;
		cls.qw_downloadspeedcount = 0;
		return 0;
	}
	if (host.realtime >= cls.qw_downloadspeedtime + 1) {
		cls.qw_downloadspeedrate = cls.qw_downloadspeedcount;
		cls.qw_downloadspeedtime = host.realtime;
		cls.qw_downloadspeedcount = 0;
	}
	if (cls.qw_downloadmethod >= DL_QWCHUNKS_2) {
		extern int chunked_receivedbytes;
		double dur = Sys_DirtyTime () - cls.qw_downloadstarttime;
		cls.qw_downloadspeedrate = chunked_receivedbytes / dur;

		c_dpsnprintf4(temp, CON_CYAN "(FTE) Downloading " CON_WHITE "%s %3d%% (%d) at %d bytes/s", cls.qw_downloadname,
		cls.qw_downloadpercent,
		chunked_receivedbytes, cls.qw_downloadspeedrate);
	} else
	if (cls.protocol == PROTOCOL_QUAKEWORLD) {
		c_dpsnprintf4(temp, "Downloading %s %3d%% (%d) at %d bytes/s", cls.qw_downloadname,
			cls.qw_downloadpercent, cls.qw_downloadmemorycursize, cls.qw_downloadspeedrate);
	}
	else
		c_dpsnprintf5(temp, "Downloading %s %3d%% (%d/%d) at %d bytes/s", cls.qw_downloadname, cls.qw_downloadpercent,
		cls.qw_downloadmemorycursize, cls.qw_downloadmemorymaxsize, cls.qw_downloadspeedrate);

	len = (int)strlen(temp);
	x = (vid_conwidth.integer - DrawQ_TextWidth(temp, len, size, size, true, FONT_INFOBAR)) / 2;
	y = vid_conheight.integer - size - offset;
	DrawQ_Fill(0, y, vid_conwidth.integer, size, 0, 0, 0, cls.signon == SIGNONS_4 ? 0.5 : 1, 0);
	DrawQ_String(x, y, temp, len, size, size, 1, 1, 1, 1, 0, NULL, q_ignore_color_codes_false, FONT_INFOBAR);
	return size;
}
/*
==============
SCR_DrawInfobarString
==============
*/
static int SCR_DrawInfobarString(int offset)
{
	int len;
	float x, y;
	float size = scr_infobar_height.value;

	len = (int)strlen(scr_infobarstring);
	x = (vid_conwidth.integer - DrawQ_TextWidth(scr_infobarstring, len, size, size, q_ignore_color_codes_false, FONT_INFOBAR)) / 2;
	y = vid_conheight.integer - size - offset;
	DrawQ_Fill(0, y, vid_conwidth.integer, size, 0, 0, 0, cls.signon == SIGNONS_4 ? 0.5 : 1, 0);
	DrawQ_String(x, y, scr_infobarstring, len, size, size, 1, 1, 1, 1, 0, NULL, false, FONT_INFOBAR);
	return size;
}

/*
==============
SCR_DrawCurlDownload
==============
*/
static int SCR_DrawCurlDownload(int offset)
{
	// sync with SCR_InfobarHeight
	int len;
	int nDownloads;
	int i;
	float x, y;
	float size = scr_infobar_height.value;
	Curl_downloadinfo_t *downinfo;
	char temp[256];
	char addinfobuf[128];
	const char *addinfo;

	downinfo = Curl_GetDownloadInfo(&nDownloads, &addinfo, addinfobuf, sizeof(addinfobuf));
	if (!downinfo)
		return 0;

	y = vid_conheight.integer - size * nDownloads - offset;

	if (addinfo)
	{
		len = (int)strlen(addinfo);
		x = (vid_conwidth.integer - DrawQ_TextWidth(addinfo, len, size, size, true, FONT_INFOBAR)) / 2;
		DrawQ_Fill(0, y - size, vid_conwidth.integer, size, 1, 1, 1, cls.signon == SIGNONS_4 ? 0.8 : 1, 0);
		DrawQ_String(x, y - size, addinfo, len, size, size, 0, 0, 0, 1, 0, NULL, true, FONT_INFOBAR);
	}

	for(i = 0; i != nDownloads; ++i)
	{
		if (downinfo[i].queued)
			dpsnprintf(temp, sizeof(temp), "Still in queue: %s", downinfo[i].curfilename);
		else if (downinfo[i].progress <= 0)
			dpsnprintf(temp, sizeof(temp), "Downloading %s ...  ???.?%% @ %.1f KiB/s", downinfo[i].curfilename, downinfo[i].speed / 1024.0);
		else
			dpsnprintf(temp, sizeof(temp), "Downloading %s ...  %5.1f%% @ %.1f KiB/s", downinfo[i].curfilename, 100.0 * downinfo[i].progress, downinfo[i].speed / 1024.0);
		len = (int)strlen(temp);
		x = (vid_conwidth.integer - DrawQ_TextWidth(temp, len, size, size, true, FONT_INFOBAR)) / 2;
		DrawQ_Fill(0, y + i * size, vid_conwidth.integer, size, 0, 0, 0, cls.signon == SIGNONS_4 ? 0.5 : 1, 0);
		DrawQ_String(x, y + i * size, temp, len, size, size, 1, 1, 1, 1, 0, NULL, true, FONT_INFOBAR);
	}

	Z_Free(downinfo);

	return size * (nDownloads + (addinfo ? 1 : 0));
}

/*
==============
SCR_DrawInfobar
==============
*/
static void SCR_DrawInfobar(void)
{
	unsigned int offset = 0;
	offset += SCR_DrawQWDownload(offset);
	offset += SCR_DrawCurlDownload(offset);
	if (scr_infobartime_off > 0)
		offset += SCR_DrawInfobarString(offset);
	if (!offset && scr_loading_ex) {
		int effective_loading = scr_loadingscreen_barheight.integer;
		if (!effective_loading && scr_loading_ex && cl_signon_start_time
			&& Sys_DirtyTime() - cl_signon_start_time > 1.5)
			effective_loading = 8;

		offset = effective_loading;
	}
	if (offset != scr_con_margin_bottom)
		Con_DPrintLinef ("broken console margin calculation: %d != %d", offset, scr_con_margin_bottom);
}

static int SCR_InfobarHeight(void)
{
	int offset = 0;
	Curl_downloadinfo_t *downinfo;
	const char *addinfo;
	int nDownloads;
	char addinfobuf[128];
	RELATED_ (CL_Disconnect)

	if (cls.signon == SIGNON_ZERO) {
		scr_infobartime_off -= cl.realframetime; //clframetime;
	} else
	if (cl.time > cl.oldtime) {
		scr_infobartime_off -= cl.time - cl.oldtime;
	}
	if (scr_infobartime_off > 0)
		offset += 1;
	if (cls.qw_downloadname[0])
		offset += 1;

	downinfo = Curl_GetDownloadInfo(&nDownloads, &addinfo, addinfobuf, sizeof(addinfobuf));
	if (downinfo)
	{
		offset += (nDownloads + (addinfo ? 1 : 0));
		Z_Free(downinfo);
	}
	offset *= scr_infobar_height.value;

	return offset;
}

/*
==============
SCR_InfoBar_f
==============
*/
RELATED_ ()
static void SCR_InfoBar_f (cmd_state_t *cmd)
{
	//if (Cmd_Argc(cmd) == 4) {
	if (cmd_argc == 4 /* String_Match (cmd_argv_parm4, "silent")*/) {
		// Silent style
		scr_infobartime_off = atof(Cmd_Argv(cmd, 1));
		c_strlcpy (scr_infobarstring, Cmd_Argv(cmd, 2) );
		//Con_LogCenterPrint (scr_infobarstring); // Baker r1421: centerprint logging to console
	}
	//
	// 0        1            2
	// infobar <num seconds> <msg>
	//else if (Cmd_Argc(cmd) == 3) {
	else if (cmd_argc == 3) {
		scr_infobartime_off = atof(cmd_argv_parm1);// Cmd_Argv(cmd, 1));
		c_strlcpy (scr_infobarstring, cmd_argv_parm2); // Cmd_Argv(cmd, 2) );
		Con_LogCenterPrint (scr_infobarstring); // Baker r1421: centerprint logging to console
	} else {
		Con_PrintLinef ("usage:" NEWLINE "infobar expiretime " QUOTED_STR ("string") );
		//char full_console_line[1024];
		//Argv_Cumulate_All (cmd, full_console_line);
		//Con_PrintLinef ("Provided: %s", full_console_line);
	}
}
//=============================================================================

/*
==================
SCR_SetUpToDrawConsole
==================
*/
static void SCR_SetUpToDrawConsole (void)
{
#ifdef CONFIG_MENU
	static int framecounter = 0;
#endif

	Con_CheckResize ();

#ifdef CONFIG_MENU
	if (scr_menuforcewhiledisconnected.integer /*d:0*/ && key_dest == key_game && cls.state == ca_disconnected)
	{
		if (framecounter >= 2)
			MR_ToggleMenu(1); // conexit
		else
			framecounter++;
	}
	else
		framecounter = 0;
#endif

	//extern cvar_t cl_startdemos;
	if (scr_conforcewhiledisconnected.integer /*d:1*/ >= 2 && key_dest == key_game &&
		cls.signon != SIGNONS_4)
		Flag_Add_To(key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4);
	else if (scr_conforcewhiledisconnected.integer /*d:1*/ >= 1 && key_dest == key_game && cls.signon != SIGNONS_4 && !sv.active) {
// Baker: This is a failure
//		&& (cls.demonum == -1 || !cls.demoplayback*/) // Baker: December 13 2024 - Do not force console when start demos transition.
//		)
		// The goal here is not show console between startdemos.
#define are_start_demos_running (cls.demos[0][0] != 0)
		if (are_start_demos_running == false) {
			RELATED_ (CL_DisconnectEx)
			// Baker: This is the norm
			Flag_Add_To(key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4); // |= KEY_CONSOLEACTIVE_FORCED;
		} else {
			//int j = 5;
		}
	}
	// Baker: Whatever ...
	//else if (cls.signon != SIGNONS_4 && !sv.active && key_dest == key_game) {
	//	RELATED_ (CL_Disconnect)
	//	// Baker: We went to great lengths to get the DarkPlaces console to quit showing at weird times.
	//	// I think this is ok to not interfere with that.
	//	Flag_Add_To(key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4); // |= KEY_CONSOLEACTIVE_FORCED;
	//}
	else
		Flag_Remove_From(key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4); // &= ~KEY_CONSOLEACTIVE_FORCED_4;

	// decide on the height of the console
	if (Have_Flag(key_consoleactive, KEY_CONSOLEACTIVE_USER_1)) {
		// Baker r0004: Ctrl + up/down size console like JoeQuake
		scr_con_current = vid_conheight.integer *scr_conheight.value * console_user_pct; // Baker 1032.2
	}
	else
		scr_con_current = 0; // none visible
}

/*
==================
SCR_DrawConsole
==================
*/
int scr_skip_once;

RELATED_ (CL_UpdateScreen_SCR_DrawScreen)
void SCR_DrawConsole (void)
{
	g_consel.draww.console_draw_frame = 0;// was_consoledrawn = false; // Until we know otherwise .. what is superframe?

	if (cl_videoplaying == VIDEO_PLAYING_PLAYVIDEO_1)
		return; // Baker: Don't draw console during video playback

	static int mfirst;
	int effective_loading = scr_loadingscreen_barheight.integer;
	if (!effective_loading &&
		scr_loading_ex &&
		cl_signon_start_time &&
		((Sys_DirtyTime() - cl_signon_start_time) > 1.5) )
		effective_loading = 8;


	// infobar and loading progress are not drawn simultaneously
	scr_con_margin_bottom = scr_loading_ex ? effective_loading : SCR_InfobarHeight();

	if (mfirst == 0) {
		mfirst = 1;
		goto no_draw_0;
	}
	if (scr_skip_once) {
		scr_skip_once = false;
		goto no_draw_0;
	}

	if (Have_Flag (key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4)) {
		// full screen
		#if 1
			if (cls.state == ca_connected && in_range (2, cls.signon, 3) &&
				scr_loading_ex) {
					if (!cls.qw_downloadname[0])
						goto no_draw_0;
					// Baker: TODO curl ... hmmm
			}
		#endif
		Con_DrawConsole (vid_conheight.integer - scr_con_margin_bottom);
	}
	else if (scr_con_current) {
		Con_DrawConsole (Smallest(scr_con_current, vid_conheight.integer - scr_con_margin_bottom));
	} else {
no_draw_0:
		con_vislines = 0;
	}
}

//=============================================================================

/*
=================
SCR_SizeUp_f

Keybinding command
=================
*/
static void SCR_SizeUp_f (cmd_state_t *cmd)
{
	Cvar_SetValueQuick (&scr_viewsize, scr_viewsize.value + 10);
}


/*
=================
SCR_SizeDown_f

Keybinding command
=================
*/
static void SCR_SizeDown_f (cmd_state_t *cmd)
{
	Cvar_SetValueQuick(&scr_viewsize, scr_viewsize.value - 10);
}

#ifdef CONFIG_VIDEO_CAPTURE
void SCR_CaptureVideo_EndVideo(void);
#endif
void CL_Screen_Shutdown(void)
{
#ifdef CONFIG_VIDEO_CAPTURE
	SCR_CaptureVideo_EndVideo();
#endif
}


// Baker: A lot of utility-like commands I made
// and screenshots/video code was making the real action hard to
// find so I separated it out into the following file ...

#include "cl_screen_utils.c.h"


void R_ClearScreen(qbool fogcolor)
{
	float clearcolor[4];
	if (scr_screenshot_alpha.integer /*d:0*/)
		// clear to transparency (so png screenshots can contain alpha channel, useful for building model pictures)
		Vector4Set(clearcolor, 0.0f, 0.0f, 0.0f, 0.0f);
#if 1 // This was successful ... however ...
	else if (r_refdef.view.isbclearcolor &&
		r_refdef.scene.worldmodel && r_refdef.view.ismain == false) {
		Vector4CopyDestSrc(clearcolor, r_refdef.view.bclearcolor );
	}
#endif
	else
		// clear to opaque black (if we're being composited it might otherwise render as transparent)
		Vector4Set(clearcolor, 0.0f, 0.0f, 0.0f, 1.0f);

	if (fogcolor && r_fog_clear.integer /*d:1*/)
	{
		R_UpdateFog();
		VectorCopy(r_refdef.fogcolor, clearcolor); // Sets clearcolor to fog color.
	}
	// clear depth is 1.0
	// clear the screen
	// Baker: Feb 10 2025 - Trying to not clear color buffer clear here is a fail.
	// And would always be a fail for several reasons.

	GL_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | (vid.stencil ? GL_STENCIL_BUFFER_BIT : 0), clearcolor, 1.0f, 0);
}

int r_stereo_side;
extern cvar_t v_isometric;
extern cvar_t v_isometric_verticalfov;


// Baker r0005: Autoscale 360p
float   f_yfactors;
float    yfactor_mag_360;                    // output
float    scale_width_360;
float    scale_height_360;

int        old_vid_height;
int        old_vid_width;
qbool    old_vid_fullscreen;
int old_vid_kickme;

// Allow adjustment over automatic math
float    old_vid_fullscreen_conscale;
float    old_vid_window_conscale;

void scale_360_calc (void)
{
	if (old_vid_kickme) goto doitanyway;
    if (old_vid_height == vid.height &&
		old_vid_width == vid.width &&
        old_vid_fullscreen == vid.fullscreen &&
        old_vid_fullscreen_conscale == vid_fullscreen_conscale.value &&
        old_vid_window_conscale == vid_window_conscale.value)
        return;

doitanyway:
	if (old_vid_kickme) old_vid_kickme--;

    old_vid_width				  = vid.width;
	old_vid_height                = vid.height;
    old_vid_fullscreen            = vid.fullscreen;
    old_vid_fullscreen_conscale    = vid_fullscreen_conscale.value;
    old_vid_window_conscale        = vid_window_conscale.value;

    cvar_t *pcvar;
    pcvar  = vid.fullscreen ? &vid_fullscreen_conscale : &vid_window_conscale;

    f_yfactors = vid.height / 360.0f;

	// Baker: Dec 9 2025 - I don't know why this used Q_rint when I wrote it initially.
	// And I don't understand how the engine gets the status bar "right" whether or not it is 2 or 3.
	if (vid_magnification_factor_rint.integer /*d:1*/)
    	f_yfactors = Q_rint (f_yfactors); // 720 + is magnification of 2 or more
	else
		f_yfactors = (int)f_yfactors; // 720 + is magnification of 2 or more
    if (f_yfactors < 0)
        f_yfactors = 1;

    /*Con_PrintLinef ("conscale %d %d fs? %d conscale %f",
                    vid.width,
                    vid.height,
                    vid.fullscreen,
                    pcvar->value);
    Con_PrintLinef ("f_yfactors %f",
                    f_yfactors);
    */
    float fmulto = bound (0.5, pcvar->value, 2); // Baker: This is using vid_window_conscale magnification extra adjustment.

    if (f_yfactors)
        yfactor_mag_360 = (1 / f_yfactors) * fmulto; // Baker: f_yfactors is an integer but a float. See Q_Rint
    else 
		yfactor_mag_360 = 1;

	Con_DPrintLinef ("yfactor_mag_360 %f", yfactor_mag_360);

#if 0
	DebugPrintLinef ("vid.width %d vid.height %d iscale %f", vid.width, vid.height, f_yfactors);
	Con_PrintLinef ("vid.width %d vid.height %d iscale %f", vid.width, vid.height, f_yfactors);
#endif

	Cvar_SetValueQuick (&vid_magnification_factor, f_yfactors);

	Cvar_SetValueQuick(&vid_conheight, 399.5); // Evile!

    scale_width_360 =  vid.width * yfactor_mag_360;
    scale_height_360 = vid.height * yfactor_mag_360;

	extern cvar_t vid_minification_factor;
	Cvar_SetValueQuick (&vid_minification_factor, scale_height_360/(float)vid.height);
}

entity_t *find_e_render_CL_Tool_Inspector (entity_render_t *exy, int *enumy)
{
	int i;

	for (i = 1; i < cl.num_entities; i ++) {
		if (cl.entities_active[i]) {
			entity_t *e = cl.entities + i;
			entity_render_t *exy2 = &e->render;
			if (exy2 == exy) {
				(*enumy) = i;
				return e;
			} // if
		} // if
	} // i
	return NULL;
}

// We come here only if cls.signon == SIGNONS_4 (fully connected)
CALLERS_ (CL_UpdateScreen_SCR_DrawScreen)
void CL_Tool_Inspector (void)
{
	SV_LockThreadMutex (); //

	//int conwidth_2d_phase = scale_width_360;
	//int conheight_2d_phase = scale_height_360;

	RELATED_ (PRVM_ED_Write)

	vec3_t cl_player_origin, cl_player_angles;
	VectorCopyDestSrc (cl_player_origin, cl.entities[cl.playerentity].state_current.origin);
	VectorCopyDestSrc (cl_player_angles, cl.entities[cl.playerentity].state_current.angles);

	prvm_prog_t		*prog = SVVM_prog;
	stringlist_t	list_distance_space_entnum = {0}; // "20.0 24" // distance entnum

	// Baker: Find entities visible in scene list, put eligible entity numbers in stringlist
	for (int ent_num = 0; ent_num < r_refdef.scene.numentities; ent_num ++) {
		if (!r_refdef.viewcache.entityvisible[ent_num])
			continue;

		entity_render_t *e_this = r_refdef.scene.entities[ent_num];

		if (!e_this->model || e_this->model->Draw == NULL)
			continue; // Baker: No model or no draw for some reason

		int edict_num = 0;
		entity_t *e = find_e_render_CL_Tool_Inspector(e_this, &edict_num);

		if (!e)
			continue; // Baker: Can this happen?

		prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);
		if (ed->free)
			continue; // Baker: Rare, but will happen (any projectile remove, for instance)

		// Get distance
		{
			//prvm_vec_t	*p_org = PRVM_serveredictvector(ed, origin);
			vec3_t e_box_center, dist3;
			vec3_t v_screen2d = {0};

			// Find center of entity
			prvm_vec_t *vabsmin = PRVM_serveredictvector(ed, absmin), *vabsmax = PRVM_serveredictvector(ed, absmax);

			VectorCenter	(vabsmin, vabsmax, e_box_center);

			qbool is_infront = Math_infront(cl_player_origin, cl_player_angles, e_box_center);
			if (!is_infront)
				continue; // It's behind us.

			Math_Project	(e_box_center, v_screen2d);
			VectorSubtract	(e_box_center, r_refdef.view.origin, dist3);

			float dist = VectorLength(dist3);
			va_super (sortable_description, 64, "%08.1f %d", dist, edict_num);

			stringlistappend (&list_distance_space_entnum, sortable_description);
		} // edict not free
	} // for

	// SORT: Reverse so furthest away prints first, closest prints last.
	stringlistsortreverse (&list_distance_space_entnum, fs_make_unique_false);

	for (int sidx = 0; sidx < list_distance_space_entnum.numstrings; sidx ++) {
		ccs			*sxy			= list_distance_space_entnum.strings[sidx];
		ccs			*s_edict_num	= &sxy[9]; // First 8 characters are 0 filled distance, space, then edict num
		int			edict_num		= atoi(s_edict_num);

		prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);

		ccs			*s_classname	= PRVM_GetString(prog, PRVM_alledictstring(ed, classname));
		ccs			*s_model		= PRVM_GetString(prog, PRVM_alledictstring(ed, model));
		//prvm_vec_t*e_orig			= PRVM_serveredictvector(ed, origin);
		prvm_vec_t	*vabsmin		= PRVM_serveredictvector(ed, absmin);
		prvm_vec_t	*vabsmax		= PRVM_serveredictvector(ed, absmax);

		vec3_t e_box_center, dist3, v_screen2d = {0};

		VectorCenter	(vabsmin, vabsmax, e_box_center);
		Math_Project	(e_box_center, v_screen2d);
		VectorSubtract	(e_box_center, r_refdef.view.origin, dist3);

		// Set tool_inspector 2 for single frame printed information to console
		// it then reverts to tool_inspector 1
		if (tool_inspector.integer > 1)
			Con_PrintLinef ("%4d %-20.20s @ " VECTOR3_5d1F, edict_num, s_classname, VECTOR3_SEND(e_box_center));

		float fpelwidth = vid_conwidth.value/scale_width_360;
		float fpelheight = vid_conwidth.value/scale_height_360;

		int height_row	= 8 * 1 * fpelheight;

		if (!tool_inspector.integer) {
			// Draw less
			va_super (line1, 64, "%d", edict_num);
			int slen = (int)strlen(line1);
			int x			= v_screen2d[0] - slen / 2.0;
			int y_row1		= v_screen2d[1] - height_row / 2.0;
			int width_row	= 8 * (slen) * fpelwidth + 2;
			DrawQ_Fill		(x -1, y_row1 - 1, width_row + 2, height_row + 2, q_rgb3_black, alpha_0_5, DRAWFLAG_NORMAL_0);
			//DrawQ_String	(x, y_row1, line1, 0, /*scale x y*/ height_row, height_row, q_rgb3_bronze, alpha_1_0, DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);

			// LEGIBILITY
			DrawQ_String	(x, y_row1, line1, 0, /*scale x y*/ height_row, height_row, q_rgba_yellow, DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);
		} else {
			// Typical tool_inspector draw
			va_super (line1, 64, "edict %d", edict_num);
			va_super (line2, 64, "%s", s_classname);
			va_super (line3, 64, "%s", s_model);

			int larger		= (int)Largest(strlen(line1), Largest(strlen(line2), strlen(line3))) ;
			int width_row	= 8 * larger * fpelwidth;
			int height_tot	= height_row * 3;

			// Let's mix up the background slightly for better legibility
			int mody = edict_num % 8; // Has to be off edict_num otherwise the number may change (then the color changes)

			vec3_t color0;
			switch (mody) {
			default:        VectorSet (color0, 00000, 00000, 00000); // IMPOSSIBLE .. but gcc warning
			case_break 0:	VectorSet (color0, 00000, 00000, 00000); // RGB
			case_break 1:	VectorSet (color0, 00000, 00000, 0.125); // RGB
			case_break 2:	VectorSet (color0, 00000, 0.125, 00000); // RGB
			case_break 3:	VectorSet (color0, 00000, 0.125, 0.125); // RGB
			case_break 4:	VectorSet (color0, 0.125, 00000, 00000); // RGB
			case_break 5:	VectorSet (color0, 0.125, 00000, 0.125); // RGB
			case_break 6:	VectorSet (color0, 0.125, 0.125, 00000); // RGB
			case_break 7:	VectorSet (color0, 0.125, 0.125, 0.125); // RGB
			} // sw

			int x			= v_screen2d[0] - width_row / 2.0;
			int y_row1		= v_screen2d[1] - height_row / 2.0;
			int y_row2		= y_row1 + height_row;
			int y_row3		= y_row2 + height_row;


			DrawQ_Fill		(x, y_row1, width_row, height_tot, VECTOR3_SEND(color0), alpha_1_0, DRAWFLAG_NORMAL_0);
			DrawQ_String	(x, y_row1, line1, 0, /*scale x y*/ height_row, height_row, q_rgba_white, DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);
			DrawQ_String	(x, y_row2, line2, 0, /*scale x y*/ height_row, height_row, q_rgba_white, DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);
			DrawQ_String	(x, y_row3, line3, 0, /*scale x y*/ height_row, height_row, q_rgba_white, DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);
		}
	} // for

	stringlistfreecontents (&list_distance_space_entnum);

	if (tool_inspector.integer > 1)
		Cvar_SetValueQuick (&tool_inspector, 1);

	SV_UnlockThreadMutex ();
}

void CL_Tool_Marker (void)
{
	int conwidth_2d_phase = scale_width_360;
	int conheight_2d_phase = scale_height_360;

	vec3_t v_screen2d = {0};
	vec3_t mark_origin = {0};

	//Math_atov (tool_marker.string, mark_origin);
	VectorCopyDestSrc (mark_origin, cl.tool_marker_origin);

#if 0
	if (!mark_origin[1] && !mark_origin[2]) {
		Con_PrintLinef (CON_RED "Tool marker needs x y z where x and y are non-zero");
		Con_PrintLinef (CON_BRONZE "Make sure you put the origin in quotes");
		Con_PrintLinef (CON_BRONZE "Example: " CON_WHITE "tool_marker " QUOTED_STR("120 -3600 24") );
		Con_PrintLinef (CON_BRONZE "and ensure X and Y are not zero");
		Cvar_SetValueQuick (&tool_marker, 0);
	}
#endif

	Math_Project (mark_origin, v_screen2d);

	if (v_screen2d[2] <= 0)
		return; // It's behind us

	int width_row	= (8 * ONE_CHAR_1 * 3) * vid_conwidth.value/conwidth_2d_phase;
	int height_row	= (8 * ONE_CHAR_1 * 1) * vid_conheight.value/conheight_2d_phase;
	int height_tot	= height_row * 3;

	int x	= v_screen2d[0] - width_row / 2.0;
	int y0	= v_screen2d[1] - height_row / 2.0;
	int y1	= y0 + height_row;
	//int y2	= y1 + height_row;

	DrawQ_Fill		(x, y0, width_row, height_tot, /*rgba:*/ 0, 0, 1.0, 1.0, DRAWFLAG_NORMAL_0);
	DrawQ_String	(x + height_row, y1, "X", 0, /*scale x y*/ height_row, height_row,
		/*rgba:*/ 1, 1, 1, 1.0,
		DRAWFLAG_NORMAL_0, q_outcolor_null, q_ignore_color_codes_true, FONT_CENTERPRINT);

}

void Canvas_Finish (void)
{
	// Baker: Not seeing DrawQ_Start anywhere in existing code for, so not here either.
	// CL_UpdateScreen_SCR_DrawScreen
	DrawQ_Finish();
	R_Mesh_Finish();
}

void Canvas_Start_With_Size (int width, int height)
{
	Cvar_SetValueQuick (&vid_conwidth,  width);
	Cvar_SetValueQuick (&vid_conheight, height);

//baker_really_do_this:
//	SCR_SetUpToDrawConsole(); // conlines measurement and stuff

	// Start a new draw, this time without fullscreen
	//Draw_Frame();						// Looks like frees unused pics
	r_viewport_t viewport;

	R_Viewport_InitOrtho(&viewport, &identitymatrix, 0, 0, vid.width, vid.height, 0, 0, vid_conwidth.integer, vid_conheight.integer, -10, 100, NULL);
	R_Mesh_SetRenderTargets(0, NULL, NULL, NULL, NULL, NULL);
	R_SetViewport(&viewport);

	R_Mesh_Start();
}


CALLERS_ (sure)
// Baker: Also draws world if map is loaded
static void CL_UpdateScreen_SCR_DrawScreen(void)
{
cldraw2d0:
	Draw_Frame(); // Looks like frees unused pics

	// Baker: This does canvas stuff, we must have vid_conwidth / vid_conheight finalized BEFORE
drawstart:
	DrawQ_Start(); // Scissor, viewport, matrixes, etc.

	R_Mesh_Start();

	R_UpdateVariables(); // nearclip, gamma, etc.

	// Quake uses clockwise winding, so these are swapped
	r_refdef.view.cullface_front = GL_BACK;
	r_refdef.view.cullface_back = GL_FRONT;

	if (PLAYING_MAP_LOADED) {
		float size;

		size = scr_viewsize.value * (1.0 / 100.0);
		size = Smallest(size, 1);

		if (r_stereo_sidebyside.integer)
		{
			r_refdef.view.width = (int)(vid.width * size / 2.5);
			r_refdef.view.height = (int)(vid.height * size / 2.5 * (1 - bound(0, r_letterbox.value, 100) / 100));
			r_refdef.view.depth = 1;
			r_refdef.view.x = (int)((vid.width - r_refdef.view.width * 2.5) * 0.5);
			r_refdef.view.y = (int)((vid.height - r_refdef.view.height) / 2);
			r_refdef.view.z = 0;
			if (r_stereo_side)
				r_refdef.view.x += (int)(r_refdef.view.width * 1.5);
		}
		else if (r_stereo_horizontal.integer)
		{
			r_refdef.view.width = (int)(vid.width * size / 2);
			r_refdef.view.height = (int)(vid.height * size * (1 - bound(0, r_letterbox.value, 100) / 100));
			r_refdef.view.depth = 1;
			r_refdef.view.x = (int)((vid.width - r_refdef.view.width * 2.0) / 2);
			r_refdef.view.y = (int)((vid.height - r_refdef.view.height) / 2);
			r_refdef.view.z = 0;
			if (r_stereo_side)
				r_refdef.view.x += (int)(r_refdef.view.width);
		}
		else if (r_stereo_vertical.integer)
		{
			r_refdef.view.width = (int)(vid.width * size);
			r_refdef.view.height = (int)(vid.height * size * (1 - bound(0, r_letterbox.value, 100) / 100) / 2);
			r_refdef.view.depth = 1;
			r_refdef.view.x = (int)((vid.width - r_refdef.view.width) / 2);
			r_refdef.view.y = (int)((vid.height - r_refdef.view.height * 2.0) / 2);
			r_refdef.view.z = 0;
			if (r_stereo_side)
				r_refdef.view.y += (int)(r_refdef.view.height);
		}
		else
		{
			r_refdef.view.width = (int)(vid.width * size);
			r_refdef.view.height = (int)(vid.height * size * (1 - bound(0, r_letterbox.value, 100) / 100));
			r_refdef.view.depth = 1;
			r_refdef.view.x = (int)((vid.width - r_refdef.view.width) / 2);
			r_refdef.view.y = (int)((vid.height - r_refdef.view.height) / 2);
			r_refdef.view.z = 0;
		}

		// LadyHavoc: viewzoom (zoom in for sniper rifles, etc)
		// LadyHavoc: this is designed to produce widescreen fov values
		// when the screen is wider than 4/3 width/height aspect, to do
		// this it simply assumes the requested fov is the vertical fov
		// for a 4x3 display, if the ratio is not 4x3 this makes the fov
		// higher/lower according to the ratio
		r_refdef.view.useperspective = true;
		r_refdef.view.frustum_y = tan(scr_fov.value * M_PI / 360.0) * (3.0 / 4.0) * cl.viewzoom;
		r_refdef.view.frustum_x = r_refdef.view.frustum_y * (float)r_refdef.view.width / (float)r_refdef.view.height / vid_pixelheight.value;

		r_refdef.view.frustum_x *= r_refdef.frustumscale_x;
		r_refdef.view.frustum_y *= r_refdef.frustumscale_y;
		r_refdef.view.ortho_x = atan(r_refdef.view.frustum_x) * (360.0 / M_PI); // abused as angle by VM_CL_R_SetView
		r_refdef.view.ortho_y = atan(r_refdef.view.frustum_y) * (360.0 / M_PI); // abused as angle by VM_CL_R_SetView

		r_refdef.view.ismain = true;
	csqc:
		// if CSQC is loaded, it is required to provide the CSQC_UpdateView function,
		// and won't render a view if it does not call that.
		if (cl.csqc_loaded) {
			CL_VM_UpdateView(r_stereo_side ? 0.0 : max(0.0, cl.time - cl.oldtime));
			// Form draw would go here ...
			// CSQC *must* handle mouse cursorage?
			// Otherwise it just isn't helpful enough ...
			// So what happens with click events?  We intercept those too.
		}
		else
		{
			cl.csqc_vidvars.drawworld = r_drawworld.integer != 0; // Baker: r_drawworld sometimes ignored fix.

			// Prepare the scene mesh for rendering - this is lightning beams and other effects rendered as normal surfaces
			CL_MeshEntities_Scene_FinalizeRenderEntity(); // Baker: 3D mesh only

			CL_UpdateEntityShading();
			R_RenderView(0, NULL, NULL, r_refdef.view.x, r_refdef.view.y, r_refdef.view.width, r_refdef.view.height);
		}

		if (!cls.demoplayback && (tool_inspector.integer || g_auto_inspector) ) {
			CL_Tool_Inspector ();
		}
		if (cl.tool_marker_on) {
			if (cl.tool_marker_decay) {
				cl.tool_marker_decay -= (cl.time - cl.oldtime); // frametime
				if (cl.tool_marker_decay <=0)
					cl.tool_marker_on = false;
			}
			
			CL_Tool_Marker ();
		}
		// END CL_VM_UpdateView / CSQC
	} // cls.signon == SIGNONS_4

	// Don't apply debugging stuff like r_showsurfaces to the UI
	r_refdef.view.showdebug = false;

draw9part1submit:
draw9wrap3d:
	// If CSQC plus wants fullscreen
	// 1. Finish this draw
	// 2. Reset conwidth/conheight
	// 3. Start a new draw
baker_draw_phase_2:
	if (PLAYING_MAP_LOADED && CLVM_prog->loaded && csqc_full_width_height.integer) {
		// CSQC loaded and wants full screen

#if 0
		void XOR_CSQC_Form_Draw (void);
		XOR_CSQC_Form_Draw ();
#endif

#if 1
		// Finish the drawing performed above -- 3d, possibly CSQC 2D stuff too?
		Canvas_Finish (); // DrawQ_Finish R_Mesh_Finish
		Canvas_Start_With_Size (scale_width_360, scale_height_360);
#else
		DrawQ_Finish();
		R_Mesh_Finish();
		Cvar_SetValueQuick (&vid_conwidth,  scale_width_360);
		Cvar_SetValueQuick (&vid_conheight, scale_height_360);
#endif

baker_really_do_this:
		SCR_SetUpToDrawConsole(); // conlines measurement and stuff

		// Start a new draw, this time without fullscreen
		// Draw_Frame();			// Looks like frees unused pics
		r_viewport_t viewporta;

		R_Viewport_InitOrtho(&viewporta, &identitymatrix, 0, 0, vid.width, vid.height, 0, 0, vid_conwidth.integer, vid_conheight.integer, -10, 100, NULL);
		R_Mesh_SetRenderTargets(0, NULL, NULL, NULL, NULL, NULL);
		R_SetViewport(&viewporta);

		R_Mesh_Start();

	} // end csqc plus wants fullscreen


	if (!r_stereo_sidebyside.integer && !r_stereo_horizontal.integer && !r_stereo_vertical.integer)
	{
		r_refdef.view.width = vid.width;
		r_refdef.view.height = vid.height;
		r_refdef.view.depth = 1;
		r_refdef.view.x = 0;
		r_refdef.view.y = 0;
		r_refdef.view.z = 0;
		r_refdef.view.useperspective = false;
	}

	if (cls.timedemo && cls.td_frames > 0 && timedemo_screenshotframelist.string && timedemo_screenshotframelist.string[0]) {
		// TIMEDEMO
		const char *t;
		int framenum;
		t = timedemo_screenshotframelist.string;
		while (*t)
		{
			while (*t == ' ')
				t++;
			if (!*t)
				break;
			framenum = atof(t);
			if (framenum == cls.td_frames)
				break;
			while (*t && *t != ' ')
				t++;
		}
		if (*t)
		{
			// we need to take a screenshot of this frame...
			char filename[MAX_QPATH_128];
			unsigned char *buffer1;
			unsigned char *buffer2;
			dpsnprintf(filename, sizeof(filename), "timedemoscreenshots/%s%06d.tga", cls.demoname, cls.td_frames);
			buffer1 = (unsigned char *)Mem_Alloc(tempmempool, vid.width * vid.height * 4);
			buffer2 = (unsigned char *)Mem_Alloc(tempmempool, vid.width * vid.height * 3);
			SCR_ScreenShot(filename, buffer1, buffer2, 0, 0, vid.width, vid.height, false, false, false, false, false, true, false, q_clipboard_false);
			Mem_Free(buffer1);
			Mem_Free(buffer2);
		}
	}

d2go:
	// draw 2D stuff

	if (!scr_con_current && Have_Flag(key_consoleactive, KEY_CONSOLEACTIVE_FORCED_4) == false) {
		if (isin2(key_dest, key_game, key_message) &&
			!r_letterbox.value &&
			!scr_loading_ex)
			Con_DrawNotify();	// only draw notify in game
	} // if

	if (cl.islocalgame && (key_dest != key_game || key_consoleactive))
		host.paused = true;
	else
		host.paused = false;

	if (!scr_loading_ex && cls.signon == SIGNONS_4) { // MENUQC_OIC
		SCR_DrawNet();
		SCR_DrawTurtle();
		SCR_DrawPause();
		if (!r_letterbox.value)
			Sbar_Draw();
		SHOWLMP_drawall();
		SCR_CheckDrawCenterString();
	}
	SCR_DrawNetGraph();

	// Baker: Moved this before menu
	// Shouldn't draw on top of it.
	R_Shadow_EditLights_DrawSelectedLightProperties();

menu:
#ifdef CONFIG_MENU
	RELATED_(M_Draw; MP_Draw /*csqc*/)
		if (scr_loading_ex == false) {
			// Baker: We have a developer tools menu that sometimes wants a full canvas
			// If so we complete the UI draw -> set canvas to "fullscreen"
			// and then draw and then set the canvas back.
#ifdef DEVINFO
			extern cvar_t ui_full_canvas;
			int is_full_canvas = MVM_prog->loaded == false &&
				(isin2 (m_state, m_zdev_29, m_devinfo_30) || (m_state == m_csqc_form_32 && ui_full_canvas.integer)); // SIZEOIC
#else
			int is_full_canvas = false;
#endif // DEVINFO

		if (MVM_prog->loaded && menuqc_full_width_height.integer)
			is_full_canvas = true;

			switch (is_full_canvas) {
			case true:
				// Baker: CSQC added to mesh ui.
			// "devinfo" comes here
				Canvas_Finish ();
				Canvas_Start_With_Size (vid.width, vid.height);
				RELATED_ (M_Draw M_ZForm_Draw)

				MR_Draw (); // MENUOIC
				// "devinfo" FULLSCREEN SUBMIT
				Canvas_Finish ();
				Canvas_Start_With_Size (scale_width_360, scale_height_360);
				break;

			case false:
				MR_Draw (); // MENUOIC
				break;
			} // sw
		}

	
#endif // CONFIG_MENU

	if (scr_loading_ex) {
		Canvas_Finish ();
		Canvas_Start_With_Size (scale_width_360, scale_height_360);
		loadingscreenstack_t connect_status;
		qbool show_connect_status = !loadingscreenstack && (cls.connect_trying || cls.state == ca_connected);
		if (show_connect_status)
		{
			connect_status.absolute_loading_amount_min = 0;
			if (cls.signon > 0)
				dpsnprintf(connect_status.msg, sizeof(connect_status.msg), "Connect: Signon stage %d of %d", cls.signon, SIGNONS_4);
			else if (cls.connect_remainingtries > 0)
				dpsnprintf(connect_status.msg, sizeof(connect_status.msg), "Connect: Trying...  %d", cls.connect_remainingtries);
			else
				dpsnprintf(connect_status.msg, sizeof(connect_status.msg), "Connect: Waiting %d seconds for reply", 10 + cls.connect_remainingtries);
			loadingscreenstack = &connect_status;
		}

		SCR_DrawLoadingScreen();

		Canvas_Finish ();
		Canvas_Start_With_Size (scale_width_360, scale_height_360);
		if (show_connect_status)
			loadingscreenstack = NULL;
	}

	// Baker: Video draws on top of everything except console/infobar
	CL_DrawVideo_SCR_DrawScreen();

baker_draw_phase_console:
	SCR_DrawConsole();
	SCR_DrawInfobar();

	if (!scr_loading_ex) {
		SCR_DrawBrand();
		SCR_DrawTouchscreenOverlay();
	}

	if (r_timereport_active)
		R_TimeReport("2d");

	R_TimeReport_EndFrame();
	R_TimeReport_BeginFrame();
	if (!scr_loading_ex)
		Sbar_ShowFPS();


draw9finish:
	R_Mesh_Finish();
	DrawQ_Finish();
	R_RenderTarget_FreeUnused(false);
}

extern cvar_t cl_minfps;
extern cvar_t cl_minfps_fade;
extern cvar_t cl_minfps_qualitymax;
extern cvar_t cl_minfps_qualitymin;
extern cvar_t cl_minfps_qualitymultiply;
extern cvar_t cl_minfps_qualityhysteresis;
extern cvar_t cl_minfps_qualitystepmax;
extern cvar_t cl_minfps_force;

CALLERS_ (SCR_BeginLoadingPlaque SCR_PushLoadingScreen SCR_PopLoadingScreen)
CALLERS_ (CL_TimeRefresh_f CL_Frame Key_Console when a command is submitted by ENTER plus we are disconnected)
#include "cl_screen_update.c.h"
void CL_UpdateScreen(void)
{
	static double cl_updatescreen_quality = 1;

	vec3_t vieworigin;
	static double drawscreenstart = 0.0;
	double drawscreendelta;
	r_viewport_t viewportic;

	// TODO: Move to a better place.
	cl_punchangle_applied = 0;

	if (PLAYING_MAP_LOADED == false) {
			cl_updatescreen_quality = 1;
			r_refdef.lastdrawscreentime = 0;
	} else 
	if (drawscreenstart) {
		drawscreendelta = Sys_DirtyTime() - drawscreenstart;
#ifdef CONFIG_VIDEO_CAPTURE
		if (PLAYING_MAP_LOADED && cl_minfps.value /*d:40*/ > 0 && (cl_minfps_force.integer /*d:0*/ || !(cls.timedemo ||
			(cls.capturevideo.active && !cls.capturevideo.is_realtime))) && drawscreendelta >= 0 && drawscreendelta < 60)
#else
		if ((!scr_loading_ex && cls.signon == SIGNONS_4 && r_refdef.scene.worldmodel) && cl_minfps.value > 0 && (cl_minfps_force.integer || !cls.timedemo) && drawscreendelta >= 0 && drawscreendelta < 60)
#endif // CONFIG_VIDEO_CAPTURE
		{
			// quality adjustment according to render time
			double actualframetime;
			double targetframetime;
			double adjust;
			double f;
			double h;

			// fade lastdrawscreentime
			r_refdef.lastdrawscreentime += (drawscreendelta - r_refdef.lastdrawscreentime) * cl_minfps_fade.value /*d:1*/;

			// find actual and target frame times
			actualframetime = r_refdef.lastdrawscreentime;
			targetframetime = (1.0 / cl_minfps.value);

			// we scale hysteresis by quality
			h = cl_updatescreen_quality * cl_minfps_qualityhysteresis.value /*0.05*/;

			// calculate adjustment assuming linearity
			f = cl_updatescreen_quality / actualframetime * cl_minfps_qualitymultiply.value /*d:0.2*/;
			adjust = (targetframetime - actualframetime) * f;

			// one sided hysteresis
			if (adjust > 0)
				adjust = max(0, adjust - h);

			// don't adjust too much at once
			adjust = bound(-cl_minfps_qualitystepmax.value, adjust, cl_minfps_qualitystepmax.value /*d:0.1*/);

			// adjust!
			cl_updatescreen_quality += adjust;
			cl_updatescreen_quality = bound(max(0.01, cl_minfps_qualitymin.value /*d:0.25*/),
				cl_updatescreen_quality,
				cl_minfps_qualitymax.value /*d:1*/);
		}
		else
		{
			cl_updatescreen_quality = 1;
			r_refdef.lastdrawscreentime = 0;
		}
	}

	drawscreenstart = Sys_DirtyTime();

	Sbar_ShowFPS_Update(); // Baker: Just time calcs

	if (!scr_initialized || !con_initialized || !scr_refresh.integer)
		return;				// not initialized yet

	if (IS_NEXUIZ_DERIVED(gamemode)) {
		// play a bit with the palette (experimental)
		palette_rgb_pantscolormap[15][0] = (unsigned char) (128 + 127 * sin(cl.time / exp(1.0f) + 0.0f*M_PI/3.0f));
		palette_rgb_pantscolormap[15][1] = (unsigned char) (128 + 127 * sin(cl.time / exp(1.0f) + 2.0f*M_PI/3.0f));
		palette_rgb_pantscolormap[15][2] = (unsigned char) (128 + 127 * sin(cl.time / exp(1.0f) + 4.0f*M_PI/3.0f));
		palette_rgb_shirtcolormap[15][0] = (unsigned char) (128 + 127 * sin(cl.time /  M_PI  + 5.0f*M_PI/3.0f));
		palette_rgb_shirtcolormap[15][1] = (unsigned char) (128 + 127 * sin(cl.time /  M_PI  + 3.0f*M_PI/3.0f));
		palette_rgb_shirtcolormap[15][2] = (unsigned char) (128 + 127 * sin(cl.time /  M_PI  + 1.0f*M_PI/3.0f));
		memcpy(palette_rgb_pantsscoreboard[15], palette_rgb_pantscolormap[15], sizeof(*palette_rgb_pantscolormap));
		memcpy(palette_rgb_shirtscoreboard[15], palette_rgb_shirtcolormap[15], sizeof(*palette_rgb_shirtcolormap));
	}

#ifdef CONFIG_VIDEO_CAPTURE
	if (vid_hidden && !cls.capturevideo.active && !cl_capturevideo.integer)
#else
	if (vid_hidden)
#endif
	{
		VID_Finish();
		return;
	}

	if (scr_loading_ex)
	{
		Sys_PumpLoadingEvents();
		if (!loadingscreenstack && !cls.connect_trying && (cls.state != ca_connected || cls.signon == SIGNONS_4))
			SCR_EndLoadingPlaque(); // SCR_ClearLoadingScreenTexture
		else if (scr_loadingscreen_maxfps.value)
		{
			static float lastupdate;
			float now = Sys_DirtyTime();
			if (now - lastupdate < 1.0f / scr_loadingscreen_maxfps.value)
				return;
			lastupdate = now;
		}
	}

baker_csqc_draw_start_logic_here:
	CL_UpdateScreen_SCR_UpdateVars_First(); // Viewsize, sbar and stuff, scale_360_calc conwidth

	R_FrameData_NewFrame(); // Nothing with conwidth
	R_BufferData_NewFrame(); // Nothing with conwidth
cldraw2d2:

	Matrix4x4_OriginFromMatrix(&r_refdef.view.matrix, vieworigin); // View matrix is expected to be set
	R_HDR_UpdateIrisAdaptation(vieworigin);

	r_refdef.view.colormask[0] = 1;
	r_refdef.view.colormask[1] = 1;
	r_refdef.view.colormask[2] = 1;

baker_bypass_console_calcs_do_later: // No one calls goto
	if (PLAYING_MAP_LOADED && CLVM_prog->loaded && csqc_full_width_height.integer) {
		// Skip for later
	} else {
		SCR_SetUpToDrawConsole(); // Con_CheckResize, scr_con_current, key_consoleactive
	}

#ifndef USE_GLES2
	CHECKGLERROR
	qglDrawBuffer(GL_BACK);CHECKGLERROR
#endif

	// The idea here is to give csqc the entirety of the screen

bang:
	R_Viewport_InitOrtho(&viewportic, &identitymatrix, 0, 0, vid.width, vid.height, 0, 0, vid_conwidth.integer, vid_conheight.integer, -10, 100, NULL);
	R_Mesh_SetRenderTargets(0, NULL, NULL, NULL, NULL, NULL);
	R_SetViewport(&viewportic);
	GL_ScissorTest(false);
	GL_ColorMask(1,1,1,1);
	GL_DepthMask(true);

	R_ClearScreen(false);
	r_refdef.view.clear = false;
	r_refdef.view.isoverlay = false;

	// calculate r_refdef.view.quality
	r_refdef.view.quality = cl_updatescreen_quality;

	if (scr_stipple.integer)
	{
		Con_PrintLinef ("FIXME: scr_stipple not implemented");
		Cvar_SetValueQuick(&scr_stipple, 0);
	}

#ifndef USE_GLES2
	if (R_Stereo_Active())
	{
		r_stereo_side = 0;

		if (r_stereo_redblue.integer || r_stereo_redgreen.integer || r_stereo_redcyan.integer)
		{
			r_refdef.view.colormask[0] = 1;
			r_refdef.view.colormask[1] = 0;
			r_refdef.view.colormask[2] = 0;
		}

		if (vid.stereobuffer)
			qglDrawBuffer(GL_BACK_RIGHT);
cldraw2d4:
		CL_UpdateScreen_SCR_DrawScreen(); // This does not hit

		r_stereo_side = 1;
		r_refdef.view.clear = true;

		if (r_stereo_redblue.integer || r_stereo_redgreen.integer || r_stereo_redcyan.integer)
		{
			r_refdef.view.colormask[0] = 0;
			r_refdef.view.colormask[1] = r_stereo_redcyan.integer || r_stereo_redgreen.integer;
			r_refdef.view.colormask[2] = r_stereo_redcyan.integer || r_stereo_redblue.integer;
		}

		if (vid.stereobuffer)
			qglDrawBuffer(GL_BACK_LEFT);

		CL_UpdateScreen_SCR_DrawScreen(); // Stereo draw - This does not hit
		r_stereo_side = 0;
	}
	else
#endif  // End Not Android
	{
		r_stereo_side = 0;
		CL_UpdateScreen_SCR_DrawScreen(); // Stereo draw .. this hits
	}

#ifdef CONFIG_VIDEO_CAPTURE
	SCR_CaptureVideo();
#endif

	qglFlush(); // ensure that the commands are submitted to the GPU before we do other things

	// Baker: So what is relative?  Does this mean clip to window or what?

	if (!vid_activewindow || key_consoleactive) {
		// Not active window or console shown
		VID_SetMouse(q_mouse_relative_false, q_mouse_hidecursor_false);
	}
	else if (isin2(key_dest, key_menu, key_menu_grabbed) ||
			scr_loading_ex ||
			(key_dest == key_game && cls.demoplayback)) { // Baker 8081 mouse cursor shows during demo playback.
#ifdef CONFIG_MENU
		if (menu_is_quakec == false) {
				VID_SetMouse(q_mouse_relative_false, q_mouse_hidecursor_false);
		} else {
			// Baker: Make mouse cursor show in menu
			VID_SetMouse(vid_mouse.integer && !in_client_mouse && !vid_touchscreen.integer, 
				!vid_touchscreen.integer);
		}
#endif // CONFIG_MENU
	}
	else {
		// Norm for in-game
		// is_relative = vid_mouse.integer 1 && cl.csqc_wantsmousemove == 0 && cl_prydoncursor == 0
		// and (demoplay is 0 or cl_demo_mousegrab_0 > 0 && vid touchscreen is 0
		// is mousegrab since touchscreen is 0 will always be true typically
		int is_relative = vid_mouse.integer && !cl.csqc_wantsmousemove && cl_prydoncursor.integer <= 0
			&& (!cls.demoplayback || cl_demo_mousegrab.integer) && !vid_touchscreen.integer;
		// Tends to be true in game
		VID_SetMouse(is_relative, !vid_touchscreen.integer);
		// VID_SetMouse(is_relative true, mousegrab true); is typical key_game behavior here
	}

	VID_Finish();

	// Baker r9006: Kleskby ALT-TAB fix for certain international keyboards.
#ifdef _WIN32
	// Baker: VK_MENU is ALT key?
#if 0
	if (vid_fullscreen.integer && GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_TAB)) { //KleskBY Alt-Tab Fix
		ShowWindow(FindWindowA("SDL_app", NULL), SW_MINIMIZE);
	} // if
#endif
#endif
}

void CL_Screen_NewMap(void)
{
	// Baker: This is called, but empty.
	// Did DarkPlaces Beta have this, or did I add this function.
	// A: DarkPlaces Beta has this empty function
}



#include "cl_screen_gif.c.h"


void Dynamic_Baker_Texture2D_Prep (dynamic_baker_texture_t *fill, int is_dirty, const char *name,
								   bgra4 *vimagedata, int in_pic_width, int in_pic_height)
{
	fill->modelui	= CL_Mesh_UI ();
	fill->bpic		= Draw_CachePic_Flags(name, CACHEPICFLAG_QUIET); // CACHEPIC_BAKER

	DYNAMICTEX_Q3_START (vimagedata);
	fill->btex = Mod_Mesh_GetTexture (
		fill->modelui,
		fill->bpic->name,
		DRAWFLAG_NORMAL_0, // DRAWFLAG_MODULATE and such
		fill->bpic->texflags, // TEXF_CLAMP and such
		MATERIALFLAG_WALL | MATERIALFLAG_VERTEXCOLOR | MATERIALFLAG_ALPHAGEN_VERTEX | MATERIALFLAG_ALPHA | MATERIALFLAG_BLENDED | MATERIALFLAG_NOSHADOW
	);
	DYNAMICTEX_Q3_END ();

	if (is_dirty) {
		extern rtexturepool_t *r_main_texturepool;

		skinframe_t *skinframe		= fill->btex->materialshaderpass->skinframes[0];
		int			textureflags	= skinframe->textureflags;

		R_SkinFrame_PurgeSkinFrame(skinframe);
		textureflags &= ~TEXF_FORCE_RELOAD;

		skinframe->stain = NULL;	// I think this has been done.
		skinframe->merged = NULL;
		skinframe->base = NULL;
		skinframe->pants = NULL;
		skinframe->shirt = NULL;
		skinframe->nmap = NULL;
		skinframe->gloss = NULL;
		skinframe->glow = NULL;
		skinframe->fog = NULL;
		skinframe->rreflect = NULL;
		skinframe->hasalpha = false;

		// Baker: Similar to our Q1SKY fix upload fix
		skinframe->base = skinframe->merged =
			R_LoadTexture2D (
				r_main_texturepool,
				skinframe->basename,
				in_pic_width,
				in_pic_height,
				(byte *)vimagedata,
				TEXTYPE_BGRA,
				textureflags,
				/*miplevel*/ -1,
				/*palette*/ NULL
		);
	} // is dirty
}

// Baker r3102: "folder" command
// Baker: Why is this such a cluster mess?  What does "path" do?
RELATED_ (FS_Path_f FS_Pwd_f)
// folder realpath \"%s\"", realpath_unix <=== used by devinfo
void SCR_Folder_f(cmd_state_t *cmd) // "folder"
{
#ifdef __ANDROID__
	Con_PrintLinef ("folder opening not supported for this build");
	return;
#endif // __ANDROID__
#if defined(MACOSX)
    #if !defined(CORE_XCODE) // XCODE we use .m file, if "brew" using gcc assume objective is not avail
	Con_PrintLinef ("folder opening not supported for this build");
	return;
    #endif // !CORE_XCODE
#endif // MACOSX
		
	if (cmd_argc == (1 + 2) && String_Match (cmd_argv_parm1, "realpath") ) {
		// REALPATH FILE NAME - this allows opening "explorer" or equivalent
		// to highlight a file in a specific gamedir (what we use it for)
		// or outside the Quake folder entirely
		ccs *s_realpath_file = cmd_argv_parm2;
		va_super (s_realpath_file_unix, MAX_OSPATH_EX_1024, "%s", s_realpath_file);
		File_URL_Edit_SlashesForward_Like_Unix (s_realpath_file_unix);
		FS_File_Highlight_Is_Ok_Unsafe (s_realpath_file_unix);
		return;
	}

	ccs *underdir = cmd_argv_parm1;
	va_super (realpath_notrail_slash, MAX_OSPATH_EX_1024, "%s%s", fs_gamedir, underdir);

	// Convert the backslashes "\" to Unix "/"
	File_URL_Edit_SlashesForward_Like_Unix (realpath_notrail_slash);
	File_URL_Remove_Trailing_Unix_Slash(realpath_notrail_slash);

	FS_FILETYPE_e ftype = FS_SysFileOrDirectoryType (realpath_notrail_slash);

	if (ftype == FS_FILETYPE_FILE_1) {
		//Con_PrintLinef ("File " QUOTED_S " is not a folder", realpath_notrail_slash);
		FS_File_Highlight_Is_Ok_Unsafe (realpath_notrail_slash);
		return;
	}

	if (false == FS_Directory_Exists_Unsafe (realpath_notrail_slash)) {
		Con_PrintLinef ("Folder does not exist " QUOTED_S " so cannot open folder", realpath_notrail_slash);
		return;
	}

	Con_PrintLinef ("Opening folder %s", realpath_notrail_slash);
	
	if (!FS_Folder_Open_Is_Ok_Unsafe(realpath_notrail_slash)) {
		Con_PrintLinef ("Opening folder failed");
	}
}




cvar_t tosses_keep_fields = {CF_CLIENT, "tosses_keep_fields", "classname,origin,angles", "Comma separated list of QuakeC fields to ignore during export [Zircon]"};
cvar_t tosses_speed = {CF_CLIENT, "tosses_speed", "1000", "Speed to toss for ent_create_toss [Zircon]"};
void CL_Screen_Init(void)
{
	int i;
	Cvar_RegisterVariable (&scr_fov);
	Cvar_RegisterVariable (&scr_viewsize);
	Cvar_RegisterVariable (&scr_conalpha);
	Cvar_RegisterVariable (&scr_conalphafactor);
	Cvar_RegisterVariable (&scr_conalpha2factor);
	Cvar_RegisterVariable (&scr_conalpha3factor);

	Cvar_RegisterVariable (&scr_xcutoutpos);
	Cvar_RegisterVariable (&scr_xcutoutsize);
	Cvar_RegisterVariable (&scr_xtranslate);

	Cvar_RegisterVariable (&scr_conscroll_x);
	Cvar_RegisterVariable (&scr_conscroll_y);
	Cvar_RegisterVariable (&scr_conscroll2_x);
	Cvar_RegisterVariable (&scr_conscroll2_y);
	Cvar_RegisterVariable (&scr_conscroll3_x);
	Cvar_RegisterVariable (&scr_conscroll3_y);
	Cvar_RegisterVariable (&scr_conbrightness);
	Cvar_RegisterVariable (&scr_conforcewhiledisconnected);
	Cvar_RegisterVariable (&scr_conheight);

	// Baker r7003, r7004
	Cvar_RegisterVariable (&csqc_full_width_height);
	Cvar_RegisterVariable (&menuqc_full_width_height);
	Cvar_RegisterVariable (&csqc_full_width_height_available);

	Cvar_RegisterVariable (&scr_levelshot_auto);

#ifdef CONFIG_MENU
	Cvar_RegisterVariable (&scr_menuforcewhiledisconnected);
#endif
	Cvar_RegisterVariable (&scr_loadingscreen_background);
	Cvar_RegisterVariable (&scr_loadingscreen_scale);
	Cvar_RegisterVariable (&scr_loadingscreen_scale_base);
	Cvar_RegisterVariable (&scr_loadingscreen_scale_limit);
	Cvar_RegisterVariable (&scr_loadingscreen_picture);
	Cvar_RegisterVariable (&scr_loadingscreen_count);
	Cvar_RegisterVariable (&scr_loadingscreen_firstforstartup);
	Cvar_RegisterVariable (&scr_loadingscreen_barcolor);
	Cvar_RegisterVariable (&scr_loadingscreen_barheight);
	Cvar_RegisterVariable (&scr_loadingscreen_maxfps);
	Cvar_RegisterVariable (&scr_infobar_height);
	Cvar_RegisterVariable (&scr_showram);
	Cvar_RegisterVariable (&scr_showturtle);
	Cvar_RegisterVariable (&scr_showpause);
	Cvar_RegisterVariable (&scr_showbrand);
	Cvar_RegisterVariable (&scr_centertime);
	Cvar_RegisterVariable (&scr_printspeed);
	Cvar_RegisterVariable (&vid_conwidth);
	Cvar_RegisterVariable (&vid_conheight);

	Cvar_RegisterVariable (&vid_pixelheight);
	//Cvar_RegisterVariable (&vid_conwidthauto); // Baker r0005
	Cvar_RegisterVariable (&scr_screenshot_jpeg);
	Cvar_RegisterVariable (&scr_screenshot_jpeg_quality);
	Cvar_RegisterVariable (&scr_screenshot_png);
	Cvar_RegisterVariable (&scr_screenshot_gammaboost);
	Cvar_RegisterVariable (&scr_screenshot_name_in_mapdir);
	Cvar_RegisterVariable (&scr_screenshot_alpha);
	Cvar_RegisterVariable (&scr_screenshot_timestamp);

#ifdef CONFIG_VIDEO_CAPTURE
	Cvar_RegisterVariable (&cl_capturevideo);
	Cvar_RegisterVariable (&cl_capturevideo_demo_stop);
	Cvar_RegisterVariable (&cl_capturevideo_printfps);
	Cvar_RegisterVariable (&cl_capturevideo_width);
	Cvar_RegisterVariable (&cl_capturevideo_height);
	Cvar_RegisterVariable (&cl_capturevideo_realtime);
	Cvar_RegisterVariable (&cl_capturevideo_fps);
	Cvar_RegisterVariable (&cl_capturevideo_nameformat);
	Cvar_RegisterVariable (&cl_capturevideo_number);
	Cvar_RegisterVariable (&cl_capturevideo_ogg);
	Cvar_RegisterVariable (&cl_capturevideo_framestep);
#endif

	Cvar_RegisterVariable (&tool_inspector);

	Cvar_RegisterVariable (&r_letterbox);
	Cvar_RegisterVariable (&r_stereo_separation);
	Cvar_RegisterVariable (&r_stereo_sidebyside);
	Cvar_RegisterVariable (&r_stereo_horizontal);
	Cvar_RegisterVariable (&r_stereo_vertical);
	Cvar_RegisterVariable (&r_stereo_redblue);
	Cvar_RegisterVariable (&r_stereo_redcyan);
	Cvar_RegisterVariable (&r_stereo_redgreen);
	Cvar_RegisterVariable (&r_stereo_angle);
	Cvar_RegisterVariable (&scr_stipple);
	Cvar_RegisterVariable (&scr_refresh);
	Cvar_RegisterVariable (&net_graph);
	Cvar_RegisterVariableAlias( &net_graph, "shownetgraph");
	Cvar_RegisterVariable (&cl_demo_mousegrab);
	Cvar_RegisterVariable (&timedemo_screenshotframelist);
	Cvar_RegisterVariable (&vid_touchscreen_outlinealpha);
	Cvar_RegisterVariable (&vid_touchscreen_overlayalpha);
	Cvar_RegisterVariable (&r_speeds_graph);

	for (i = 0;i < (int)(sizeof(r_speeds_graph_filter)/sizeof(r_speeds_graph_filter[0]));i++)
		Cvar_RegisterVariable(&r_speeds_graph_filter[i]);

	Cvar_RegisterVariable (&r_speeds_graph_length);
	Cvar_RegisterVariable (&r_speeds_graph_seconds);
	Cvar_RegisterVariable (&r_speeds_graph_x);
	Cvar_RegisterVariable (&r_speeds_graph_y);
	Cvar_RegisterVariable (&r_speeds_graph_width);
	Cvar_RegisterVariable (&r_speeds_graph_height);
	Cvar_RegisterVariable (&r_speeds_graph_maxtimedelta);
	Cvar_RegisterVariable (&r_speeds_graph_maxdefault);

	// if we want no console, turn it off here too
	if (Sys_CheckParm ("-noconsole"))
		Cvar_SetQuick(&scr_conforcewhiledisconnected, "0");

	Cmd_AddCommand (CF_CLIENT, "sizeup", SCR_SizeUp_f, "increase view size (increases viewsize cvar)");
	Cmd_AddCommand (CF_CLIENT, "sizedown", SCR_SizeDown_f, "decrease view size (decreases viewsize cvar)");
	Cmd_AddCommand (CF_CLIENT, "screenshot", SCR_ScreenShot_f, "takes a screenshot of the next rendered frame");

	//Cvar_RegisterVariable (&tool_marker); // Baker r0109: tool marker
	Cmd_AddCommand (CF_CLIENT, "tool_marker", SCR_ToolMarker_f, "set an origin to display on-screen [Zircon]");

#ifdef CONFIG_MENU
#if 0
	void M_Dump_f (cmd_state_t *cmd);
	Cmd_AddCommand(CF_CLIENT, "mdump", M_Dump_f, "");
#endif
	
	Cmd_AddCommand (CF_CLIENT, "concatfilelistto", SCR_ConcatFiles_f, "Concatenates a list of files in a .txt file and outputs it to dest file [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "documentation_dump", SCR_documentation_dump_f, "Generates documentation on cvars/QC to documentation_*.txt [Zircon]");


	Cmd_AddCommand (CF_CLIENT, "jpegdecodeclipstring", SCR_jpegdecodeclipstring_f, "puts a screenshot base64 encoded jpeg string on the clipboard [Zircon]");

	Cmd_AddCommand (CF_CLIENT, "jpegshotclip", SCR_jpegshotclip_f, "puts a screenshot base64 encoded jpeg string on the clipboard [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "cliptraceoutline", SCR_cliptraceoutline_f, "puts a screenshot base64 encoded jpeg string on the clipboard [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "opendialog", SCR_opendialog_f, "opens a dialog [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "clipimagetobase64string", SCR_clipimagetobase64string_f, "take image from clipboard and put a base64 string on clipboard [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "jpegextractfromsave", SCR_jpegextract_from_savegame_f, "extract a jpeg from a save and copy to clipboard [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "gifclip", SCR_gifclip_f, "copy gif frame to clipboard [Zircon]");
	Cmd_AddCommand (CF_CLIENT, "tosses", SCR_Tosses_f, "list of created entities [Zircon]");
	Cvar_RegisterVariable (&tosses_keep_fields);
	Cvar_RegisterVariable (&tosses_speed);


	Cmd_AddCommand (CF_CLIENT, "cliplinesmakeunique", SCR_cliplinesmakeunique_f, "Make clipboard lines unique [Zircon]");

#if 1
	{
		void SCR_mazegen_f (cmd_state_t *cmd);
		void SCR_mazegen2_f (cmd_state_t *cmd);
		void SCR_maze_to_map_f (cmd_state_t *cmd);
		void SCR_maze_load_f (cmd_state_t *cmd);
		void SCR_maze_save_f (cmd_state_t *cmd);
		void SCR_maze_clear_f (cmd_state_t *cmd);
		
			
		Cmd_AddCommand (CF_CLIENT, "mazegen", SCR_mazegen_f, "Generates a random maze [Zircon]");
		Cmd_AddCommand (CF_CLIENT, "mazegen2", SCR_mazegen2_f, "Generates a random maze [Zircon]");

		void SCR_mazegen3_f (cmd_state_t *cmd);
		void SCR_mazegen4_f (cmd_state_t *cmd);
		Cmd_AddCommand (CF_CLIENT, "mazegen3", SCR_mazegen3_f, "Reads data/mymap.txt => maps/mymap2.map [Zircon]");
		Cmd_AddCommand (CF_CLIENT, "mazegen4", SCR_mazegen4_f, "[name] Halls_Make => maps/name.grid data/name.txt => maps/name.map[Zircon]");
		

		Cmd_AddCommand (CF_CLIENT, "maze_to_map", SCR_maze_to_map_f, "Write current maze to .map [Zircon]");
		Cmd_AddCommand (CF_CLIENT, "maze_save", SCR_maze_save_f, "Write current maze to .map [Zircon]");
		Cmd_AddCommand (CF_CLIENT, "maze_load", SCR_maze_load_f, "Save maze to .txt file [Zircon]");
		Cmd_AddCommand (CF_CLIENT, "maze_clear", SCR_maze_clear_f, "Clears current maze [Zircon]");
		//	Cmd_AddCommand (CF_CLIENT, "maze_path_find", SCR_maze_path_find_f, "Generates a random maze [Zircon]"); Feb 20 2026 - never completed
	}
#endif

	Cmd_AddCommand (CF_CLIENT, "folder", SCR_Folder_f, "open gamedir folder [Zircon]"); // Baker r3102: "folder" command

	void SCR_sigmoid_f (cmd_state_t *cmd);
	Cmd_AddCommand (CF_CLIENT, "sigmoid", SCR_sigmoid_f, "Sigmoid calculator [Zircon]");
	void SCR_brushmoverotate_f (cmd_state_t *cmd);

	Cmd_AddCommand (CF_CLIENT, "brushmoverotate", SCR_brushmoverotate_f, "Brush move rotate [Zircon]");
	void SCR_anglevectors_f (cmd_state_t *cmd);
	Cmd_AddCommand (CF_CLIENT, "anglevectors", SCR_anglevectors_f, "Sigmoid calculator [Zircon]");
	void SCR_vectoangles_f (cmd_state_t *cmd);
	Cmd_AddCommand (CF_CLIENT, "vectoangles", SCR_vectoangles_f, "Sigmoid calculator [Zircon]");
	
	void SCR_vectoangles2_f (cmd_state_t *cmd);
	Cmd_AddCommand (CF_CLIENT, "vectoangles2", SCR_vectoangles2_f, "Sigmoid calculator [Zircon]");
	
#ifdef _DEBUG
	 Cmd_AddCommand(CF_CLIENT, "reclip", SCR_reclip_f, "copy and paste text from clipboard (might scrub out oddball characters) [Zircon]");
#endif
	
	// Cmd_AddCommand(CF_CLIENT, "anglecalc", AngleCalc_f, " [Zircon]"); // Strip this.

	Cvar_RegisterVariable (&eq_clipboard_set);

	Cmd_AddCommand(CF_CLIENT, "atan2", SCR_atan2_f, "atan2 [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "eq", SCR_eq_f, "Equation calculator [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "hex", SCR_hex_f, "Decimal to hex [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "eq_tests", SCR_eqtests_f, "Equation tests [Zircon]");

	Cmd_AddCommand(CF_CLIENT, "vectornormal3", SCR_vectornormal3_f, "Calculate vector normal for 3 points [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "barmake", FS_BarMake_f, "<obj> <image> <replace_object_or_texture> makes bars of decreasing size [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "levelshot_here", R_LevelShot_Here_f, "take screenshot without HUD and write levelshots/mapname.jpg [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "levelshot_maps_all", R_LevelShot_Maps_All_f, "make levelshots of maps for the current gamedir only (optional prefix) [Zircon]");

	Cmd_AddCommand(CF_CLIENT, "envmap", R_Envmap_f, "render a cubemap (skybox) of the current scene [Zircon]");
	Cmd_AddCommand(CF_CLIENT, "infobar", SCR_InfoBar_f, "display a text in the infobar (usage: infobar expiretime string)");
#endif


#ifdef CONFIG_VIDEO_CAPTURE
	SCR_CaptureVideo_Ogg_Init();
#endif

	scr_initialized = true;
}


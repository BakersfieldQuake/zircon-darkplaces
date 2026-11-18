// cl_screen_update.c.h

// Baker: 1 caller, early in the process
CALLERS_ (CL_UpdateScreen)
static void CL_UpdateScreen_SCR_UpdateVars_First(void)
{
draw9:

	scale_360_calc ();

cldraw2d3:
	// Set the canvas here

	if (PLAYING_MAP_LOADED && CLVM_prog->loaded && csqc_full_width_height.integer) {
		// CSQC loaded and wants full screen
		// Regular Quake never hits here.  no csqc
		Cvar_SetValueQuick (&vid_conwidth,  vid.width);
		Cvar_SetValueQuick (&vid_conheight, vid.height);
	} else if (1 /*vid_conscale_auto.integer*/) {
		// So CSQC can receive scaleauto without skip a frame
		Cvar_SetValueQuick (&vid_conwidth,  scale_width_360);
		Cvar_SetValueQuick (&vid_conheight, scale_height_360);
	}

	// bound viewsize
	if (scr_viewsize.value < 30)
		Cvar_SetValueQuick(&scr_viewsize, 30);
	if (scr_viewsize.value > 120)
		Cvar_SetValueQuick(&scr_viewsize, 120);

	// bound field of view
	if (scr_fov.value < 1)
		Cvar_SetValueQuick(&scr_fov, 1);
	if (scr_fov.value > 170)
		Cvar_SetValueQuick(&scr_fov, 170);

	// intermission is always full screen
	if (cl.intermission)
		sb_lines = 0;
	else
	{
		if (scr_viewsize.value >= 120)
			sb_lines = 0;		// no status bar at all
		else if (scr_viewsize.value >= 110)
			sb_lines = 24;		// no inventory
		else
			sb_lines = 24 + 16 + 8;
	}
}

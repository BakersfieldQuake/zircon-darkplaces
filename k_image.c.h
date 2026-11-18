// k_image.c.h
// QC: Dec 13 2025 - Supports a border size, but no cutouts
// Engine: Supports cut-out and backalpha
// QC user of image is what? Load game.

void O_Image_Draw (ktrl k)
{
	if (KP(cm.kcutout)) {
		if (KP(image_name_a)) {
			cachepic_t *p = Draw_CachePic_Flags (KP(image_name_a), CACHEPICFLAG_NOTPERSISTENT);
			// Cutout ... child tells container about it on refresh.
			// For now, rounded is not supported.
			Draw_ImageCachePic_ST01 (KPA(r_cutout_top), p, KP(imagecolor), KP(imagealpha), KP(r_cutout_top_st0), KP(r_cutout_top_st1));
			Draw_ImageCachePic_ST01 (KPA(r_cutout_left), p, KP(imagecolor), KP(imagealpha), KP(r_cutout_left_st0), KP(r_cutout_left_st1));
			Draw_ImageCachePic_ST01 (KPA(r_cutout_right), p, KP(imagecolor), KP(imagealpha), KP(r_cutout_right_st0), KP(r_cutout_right_st1));
			Draw_ImageCachePic_ST01 (KPA(r_cutout_bottom), p, KP(imagecolor), KP(imagealpha), KP(r_cutout_bottom_st0), KP(r_cutout_bottom_st1));		
		}
		return;
	}
	if (KP(backalpha)) {
		Draw_Rect	(KPA(r_screen), k->backcolor, k->backalpha);
	}

#if 1 // ENGINE
	if (KP(is_quake1_bsp_texture) && KP(image_name_a) ) {
		ccs *q1texturename = KP(image_name_a);
		RELATED_ (SBar_DrawTexture)
		DrawQ_Pic_Texture_Q1BSP (q1texturename, k->r_screen.left, k->r_screen.top, k->r_screen.width, k->r_screen.height);
	} else 
#endif
	if (KP(image_name_a)) {
		cachepic_t *p = Draw_CachePic_Flags (KP(image_name_a), CACHEPICFLAG_NOTPERSISTENT | CACHEPICFLAG_QUIET); // Baker March 9 2026
		Draw_ImageCachePic (KPA(r_screen), p, KP(imagecolor), KP(imagealpha));
	}
	if (k->cursor_image_name_a && k->image_shigh) {
		// Calculate it.
		float x0 = k->r_screen.width * k->image_slow;
		float y0 = k->r_screen.height * k->image_tlow;
		float x1 = k->r_screen.width * k->image_shigh;
		float y1 = k->r_screen.height * k->image_thigh;
		rect_s r_highlight = { k->r_screen.left + x0, k->r_screen.top+ y0, x1 - x0, y1 - y0};
		cachepic_t *p = Draw_CachePic_Flags (KP(cursor_image_name_a), CACHEPICFLAG_NOTPERSISTENT | CACHEPICFLAG_QUIET); // Baker March 9 2026
		Draw_ImageCachePic (ADDRESSOF(r_highlight), p, KP(backcolorselected), alpha_1_0);
	}
}



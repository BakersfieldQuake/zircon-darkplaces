// k_rectangle.c.h

// Baker: Things
// 1. Finalize is defaulting properties (don't want that in the end)
// 2. Is not using default/inherited properties for the most part.
// 3. Engine does not have a Finalize for rect (we are abusing it)
// 4. Draw_RectRound is fair complex (but the algo is from Galaxy QC)
// 5. Does our rounded buttons use Draw_RectRound? (YES)
// 6. Is there live usage of this in QC? 
//    A: Yes. But only ListView size line so far.

RELATED_ (O_Button_Draw)

void drawThickLine(const vec3_t vcolor3, float x1, float y1, float x2, float y2, float thickness) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrt(dx * dx + dy * dy);
    
    if (len == 0) return; // Avoid division by zero

    // Calculate normal vector
    float nx = -dy / len;
    float ny = dx / len;

    // Scale normal by half thickness
    float offsetX = nx * (thickness / 2.0f);
    float offsetY = ny * (thickness / 2.0f);

	vec3_t p0 = {x1 + offsetX, y1 + offsetY, 0};
	vec3_t p1 = {x1 - offsetX, y1 - offsetY, 0};
	vec3_t p2 = {x2 + offsetX, y2 + offsetY, 0};
	vec3_t p3 = {x2 - offsetX, y2 - offsetY, 0};

	vec3_t texco000 = { 0, 0, 0}; // white
	vec3_t texco100 = { 1, 0, 0}; // white
	vec3_t texco110 = { 1, 1, 0}; // white
	vec3_t texco010 = { 0, 1, 0}; // white
	
	Draw_PolygonBegin ("engine/bicons/rect_aa" /*POLYGON_WHITE_TEXTURE_NULL*/);
		//ojpoly.drawflags = DRAWFLAG_ADDITIVE; //DRAWFLAG_NOGAMMA | DRAWFLAG_MIPMAP;
		Draw_PolygonVertex2D (p0, texco000, vcolor3, alpha_1_0);
		Draw_PolygonVertex2D (p1, texco100, vcolor3, alpha_1_0);
		
		Draw_PolygonVertex2D (p3, texco110, vcolor3, alpha_1_0);
		Draw_PolygonVertex2D (p2, texco010, vcolor3, alpha_1_0);
	Draw_PolygonEnd ();

}



RELATED_ (Object_Refresh_Recursive)
ktrl O_Rectangle_Refresh (ktrl k)
{
	// Tell parent they have a cutout ...
	// A form cannot have a cutout.  And really only a rect can.
	// Baker: Was added Jan 3 2026 I see no examples of outline in devinfo or imagegrid so far.
	// Outline might have been intended for GRID MAP or IMAGEGRID or DEVINFO
	// And instead a outline pic was used even in DEVINFO.
    // So whatever used this ... it did not look good enough
	// Or was too hard.  Or I just can't find it.
    // The timeline suggests imagegrid pre-map builder.
	if (k->is_outlined) {
		float thick = Q_rint(k->cellpaddingxpct * k->cm.relative_rect.width);

		rect_s r_inner = {thick, thick, k->cm.relative_rect.width - thick * 2, k->cm.relative_rect.height - thick * 2};
		rect_s r_top	= {0,0, k->cm.relative_rect.width, r_inner.top};
		int bottom_height = k->cm.relative_rect.height - RECT_BOTTOMOF (r_inner);
		rect_s r_bottom = {0, RECT_BOTTOMOF(r_inner), k->cm.relative_rect.width, bottom_height};
		rect_s r_left = {0, r_inner.top, 
			r_inner.left, r_inner.height};
		int right_width = k->cm.relative_rect.width - RECT_RIGHTOF(r_inner);
		rect_s r_right = {RECT_RIGHTOF(r_inner), r_inner.top, right_width, r_inner.height};

		RECT_MOVE (r_top, k->r_screen.left, k->r_screen.top);
		RECT_MOVE (r_bottom, k->r_screen.left, k->r_screen.top);
		RECT_MOVE (r_left, k->r_screen.left, k->r_screen.top);
		RECT_MOVE (r_right, k->r_screen.left, k->r_screen.top);
		k->r_cutout_top = r_top;
		k->r_cutout_left = r_left;
		k->r_cutout_bottom = r_bottom;
		k->r_cutout_right = r_right;
	} else
	if (KP(is_cutout)) {
		if (KP(container) != KP(fctrl)) {
			Form_Get (f,k);
			ktrl kpar = KP(container);
			if (KXP(kpar, r_screen.width) == 0 || KXP(kpar, r_screen.height) == 0) {
				zpr ("Cutoff with width or height == 0");
				return k;
			}
			k->container->cm.kcutout = k;
			FP(frm.has_cutout) = true;	
	
			// Assume parent is refreshed?  Apparently.
			rect_s r_top	= {0,0, kpar->cm.relative_rect.width, k->cm.relative_rect.top};
			int bottom_height = kpar->cm.relative_rect.height - RECT_BOTTOMOF (k->cm.relative_rect);
			rect_s r_bottom = {0, RECT_BOTTOMOF(k->cm.relative_rect), kpar->cm.relative_rect.width, bottom_height};
			rect_s r_left = {0, k->cm.relative_rect.top, 
				k->cm.relative_rect.left, k->cm.relative_rect.height};
			int right_width = kpar->cm.relative_rect.width - RECT_RIGHTOF( k->cm.relative_rect);
			rect_s r_right = {RECT_RIGHTOF(k->cm.relative_rect), k->cm.relative_rect.top, right_width, k->cm.relative_rect.height};
	
			if (KCLASS(kpar) == class_image) {
				kpar->r_cutout_top_st0[0] = 0;
				kpar->r_cutout_top_st0[1] = 0;
				kpar->r_cutout_top_st1[0] = RECT_RIGHTOF(r_top) / (double)kpar->r_screen.width; // left / width of image.
				kpar->r_cutout_top_st1[1] = RECT_BOTTOMOF(r_top)/ (double)kpar->r_screen.height;
	
				kpar->r_cutout_left_st0[0] = r_left.left / (double)kpar->r_screen.width;
				kpar->r_cutout_left_st0[1] = r_left.top / (double)kpar->r_screen.height;
				kpar->r_cutout_left_st1[0] = RECT_RIGHTOF(r_left) / (double)kpar->r_screen.width;
				kpar->r_cutout_left_st1[1] = RECT_BOTTOMOF(r_left) / (double)kpar->r_screen.height;
	
				kpar->r_cutout_right_st0[0] = r_right.left / (double)kpar->r_screen.width;
				kpar->r_cutout_right_st0[1] = r_right.top / (double)kpar->r_screen.height;
				kpar->r_cutout_right_st1[0] = RECT_RIGHTOF(r_right)/ (double)kpar->r_screen.width;
				kpar->r_cutout_right_st1[1] = RECT_BOTTOMOF(r_right)/ (double)kpar->r_screen.height;
	
				kpar->r_cutout_bottom_st0[0] = r_bottom.left / (double)kpar->r_screen.width;
				kpar->r_cutout_bottom_st0[1] = r_bottom.top / (double)kpar->r_screen.height;
				kpar->r_cutout_bottom_st1[0] = 1.0;
				kpar->r_cutout_bottom_st1[1] = 1.0;
			} //
		RECT_MOVE (r_top, kpar->r_screen.left, kpar->r_screen.top);
		RECT_MOVE (r_bottom, kpar->r_screen.left, kpar->r_screen.top);
		RECT_MOVE (r_left, kpar->r_screen.left, kpar->r_screen.top);
		RECT_MOVE (r_right, kpar->r_screen.left, kpar->r_screen.top);
		RECT_COPY (KXP(kpar, r_cutout_top), r_top);
		RECT_COPY (KXP(kpar, r_cutout_left), r_left);
		RECT_COPY (KXP(kpar, r_cutout_bottom), r_bottom);
		RECT_COPY (KXP(kpar, r_cutout_right), r_right);

		va_super (scutpos, 128, "%d %d", RECT_RIGHTOF(r_left) , RECT_BOTTOMOF(r_top)  );
		va_super (scutsize, 128, "%d %d", k->cm.relative_rect.width , k->cm.relative_rect.height  );

		extern cvar_t scr_xcutoutpos, scr_xcutoutsize;
		Cvar_SetQuick (&scr_xcutoutpos, scutpos);
		Cvar_SetQuick (&scr_xcutoutsize, scutsize);
		} // Contained cutout.
	} // CUTOUT
	
	return k;
}


// Draw enters with clip set to screen
// Q: Does this use the "draw list"?
RELATED_ (O_ListView_ColumnDragContinue Lister_Spawn Object_Draw_Recursive_Not_Form)
void O_Rectangle_Draw (ktrl k)
{
	if (KP(is_cutout))
		return;

	if (KP(linestyle)) {
		qbool is_lefttop = KP(linestyle) == LEFTOP_RIGHTBOT_1;
		float linewidthf = KP(linewidth);
		
		PCOLOR color3 = KP(backcolor);
		
		float x0 = k->r_screen.left;
		float x1 = RECT_RIGHTOF(k->r_screen);
		float y0 = k->r_screen.top;
		float y1 = RECT_BOTTOMOF(k->r_screen);
		if (is_lefttop == false)
			swapf (ADDRESSOF(y0), ADDRESSOF(y1));
		
		//Draw_Clip_Clear ();
		drawThickLine (color3, x0,y0,x1,y1, linewidthf);
		return;
	}

	if (k->is_outlined) {
		// We might be an outline servant or something.
		Draw_Rect		(&k->r_cutout_left, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_right, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_top, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_bottom, k->backcolor, k->backalpha);
	} else

	if (k->cm.kcutout) {
		// We have a hole in ourselves.
		// Cutout ... child tells container about it on refresh.
		// For now, rounded is not supported.
		Draw_Rect		(&k->r_cutout_left, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_right, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_top, k->backcolor, k->backalpha);
		Draw_Rect		(&k->r_cutout_bottom, k->backcolor, k->backalpha);
	} else {
		if (k->is_rounded)	
			Draw_RectRound	(&k->r_screen, k->backcolor, k->backalpha);
		else				
			Draw_Rect		(&k->r_screen, k->backcolor, k->backalpha);
	}
}


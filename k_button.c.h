// k_button.c

#include "a_zircon_ui_0_defines.h"

//// Use this for gradient draw ... but not sure we need it.
//Gradient_Rect ("engine/bicons/rounded_square", ADDRESSOF(r_button), backcolor0, alpha_1_0);
RELATED_ (drawThickLine)
extern cvar_t ui_gradient_percent;
void Gradient_Rect (rect_s *prect, const vec3_t lefttop3) 
{
	float x1 = prect->left;
	float y1 = prect->top;
	float x2 = RECT_RIGHTOF(*prect);
	float y2 = RECT_BOTTOMOF(*prect);

	vec3_t p0 = {x1, y1, 0};
	vec3_t p1 = {x2, y1, 0};
	vec3_t p2 = {x2, y2, 0};
	vec3_t p3 = {x1, y2, 0};

	vec3_t texco000 = { 0, 0, 0}; // white
	vec3_t texco100 = { 1, 0, 0}; // white
	vec3_t texco110 = { 1, 1, 0}; // white
	vec3_t texco010 = { 0, 1, 0}; // white

	vec3_t rightbottom3;
	
	VectorScale (lefttop3, ui_gradient_percent.value, rightbottom3);
	vec3_t mixed3;
	VectorLerp (lefttop3, 0.5, rightbottom3, mixed3);
	
	Draw_PolygonBegin ("engine/bicons/rounded_square");
		//ojpoly.drawflags = DRAWFLAG_ADDITIVE; //DRAWFLAG_NOGAMMA | DRAWFLAG_MIPMAP;
		Draw_PolygonVertex2D (p0, texco000, lefttop3, alpha_1_0);
		Draw_PolygonVertex2D (p1, texco100, mixed3, alpha_1_0);
		Draw_PolygonVertex2D (p2, texco110, rightbottom3, alpha_1_0);
		Draw_PolygonVertex2D (p3, texco010, mixed3, alpha_1_0);
		//Draw_PolygonVertex2D (p0, texco000, color3_gray_25, alpha_1_0);
	Draw_PolygonEnd ();

}

// Jan 28 2025 - r_button, r_text and friends need to be in screen coordinates.
// Using relative rect to put them in the right place.


void O_Button_Finalize (ktrl k)
{
#ifndef QUAKEC // ENGINE ONLY
	if (k->caption_a == NULL)				// If we are lacking text, spawn it.
		k->caption_a = Z_StrDup ("");
#endif // ! QUAKEC - ENGINE ONLY
}

RELATED_ (Object_Refresh_Recursive)



ktrl O_Button_Refresh (ktrl k)
{
	VarMyFont(myfont);
	KP(tb.stringwidthmaxpx)	= Draw_StringWidthInt (myfont, KP(caption_a), KP(fontsize)); // Ceil

	KP(tb.cellpaddingx_i)	= ceil(KP(cellpaddingxpct) * KFONTSIZE(k));
	KP(tb.cellpaddingy_i)	= ceil(KP(cellpaddingypct) * KFONTSIZE(k));

	// If image take a square and subtract

	KP(tb.r_button.left)	= KP(cm.relative_rect.left);
	KP(tb.r_button.top)		= KP(cm.relative_rect.top);
	KP(tb.r_button.width)	= KP(cm.relative_rect.width);// - 2;// - (k->tb.cellpaddingx_i * 2);
	KP(tb.r_button.height)	= KP(cm.relative_rect.height);// - 2;// - (k->tb.cellpaddingy_i * 2);
	// + ceil(k->fontsize) - descender_reduce_px;
	
	// The text area gets centered in the r_screen area - shouldn't it be the button area?
	rect_s r_text_canvas;
	RECT_COPY (r_text_canvas, KP(tb.r_button));

	// May 11 2026 - example == "devinfo" ButtonExit
	if (KP(imagesize)) {
		// Need r_image
		// tb.r_leftbar is r_button

		// FONTSIZE IS IMAGE SIZE
		RECT_SET (KP(tb.r_leftbar),
			KP(tb.r_button.left),
			KP(txta.r_textarea.top),
			KP(imagesize),
			KP(imagesize)
		);

		Label_Rect_Edit_Align (KPA(tb.r_leftbar), KPA(tb.r_button), (alignment_e) alignment_middle_center_4);
	} 
	else if (KP(image_name_a)) {
		r_text_canvas.left += KP(tb.cellpaddingx_i) + KFONTSIZECEIL(k) + KP(tb.cellpaddingx_i);
		r_text_canvas.width -= KP(tb.cellpaddingx_i) + KFONTSIZECEIL(k) + KP(tb.cellpaddingx_i);

		RECT_SET (KP(txta.r_textarea),
			0/*->cm.relative_rect.left + k->tb.cellpaddingx_i*/,
			0/*k->cm.relative_rect.top + k->tb.cellpaddingy_i*/,
			KP(tb.stringwidthmaxpx), //k->tb.r_button.width - (k->tb.cellpaddingx_i * 2),
			KFONTSIZECEIL(k)// + descender_reduce_px,
		);

		Label_Rect_Edit_Align (KPA(txta.r_textarea), ADDRESSOF(r_text_canvas), (alignment_e) alignment_middle_left_3 /*k->alignment*/);

		RECT_SET (KP(tb.r_leftbar),
			KP(tb.r_button.left) + KP(tb.cellpaddingx_i) /*->cm.relative_rect.left + k->tb.cellpaddingx_i*/,
			KP(txta.r_textarea.top), //  0/*k->cm.relative_rect.top + k->tb.cellpaddingy_i*/,
			KFONTSIZECEIL(k), //k->tb.r_button.width - (k->tb.cellpaddingx_i * 2),
			KFONTSIZECEIL(k)// + descender_reduce_px,
		);
	} else {
		RECT_SET (KP(txta.r_textarea),
			0/*->cm.relative_rect.left + k->tb.cellpaddingx_i*/,
			0/*k->cm.relative_rect.top + k->tb.cellpaddingy_i*/,
			KP(tb.stringwidthmaxpx), //k->tb.r_button.width - (k->tb.cellpaddingx_i * 2),
			KFONTSIZECEIL(k)// + descender_reduce_px,
		);

		Label_Rect_Edit_Align (KPA(txta.r_textarea), ADDRESSOF(r_text_canvas), (alignment_e) alignment_middle_center_4 /*k->alignment*/);
	}
	
	float	descender_pct			= myfont->ft_baker_descend_pct;
	float	descender_reduce_pct	= 1 - bound(0,KP(fontdescendpct),1);
	int		descender_reduce_px		= ceil(descender_pct * descender_reduce_pct * KFONTSIZE(k));

	k->txta.r_textarea.top += descender_reduce_px; // Move down a little.

	// Adds the container r_screen to these.
	AdjustLeftTopToScreenK (KP(tb.r_button));
	AdjustLeftTopToScreenK (KP(txta.r_textarea));
	AdjustLeftTopToScreenK (KP(tb.r_leftbar));

	return k;
}




ktrl O_Button_MouseButtonAction (ktrl k, int x, int y, int is_down)
{
	//K_GET_RELATIVE_XY (x,y);

	if (!is_down)
		return NULL;

	if (!k->ui.button_down_end_time)
		return NULL; // If pressed appearance, do not allow another.

	// How does click event fire?
	// ONLY for keypress ...k->ui.button_down_end_time = host.realtime + BUTTON_PRESS_TIME_0_3;

	return k;
}

ktrl O_Button_KeyDown (ktrl k, int key, int unicode)
{
	//Form_Get(f,k);
	switch (key) {
	default:		return NULL; // NO ACTION
	case_break K_ENTER:
					if (!k->ui.button_down_end_time) {
						k->ui.button_down_end_time = host.realtime + BUTTON_PRESS_TIME_0_3;
						// Enter fires click event with no mouse coords
						Form_CSQC_Event_With_Mouse_EZ (k, "Click", 0, NULL, -1, -1); // MOUSE1 (FORM)
					}
	} // sw
	return k;
}

void O_Button_Draw (ktrl k)
{
	VarMyFont(myfont);

	rect_s	r_button;
	RECT_COPY (r_button, KP(tb.r_button));
	rect_s	r_text;
	RECT_COPY (r_text, KP(txta.r_textarea));

	PSTRING s = KP(caption_a);

	qbool is_down = KFP(frm.kmousedown) == k;

	if (KP(ui.button_down_end_time) > Host_RealTime())
		is_down = true;
	else if (KP(ui.button_down_end_time))
		KP(ui.button_down_end_time) = 0; // End

	if (is_down) {
		r_text.left		+= PRESS_AMOUNT_2;
		r_text.top		+= PRESS_AMOUNT_2;
	}

	qbool is_focused = KIsFocused(k);

	PCOLOR	backcolor0		= is_focused ? KP(backcolorselected) : KP(backcolor);
	PCOLOR	forecolor0		= is_focused ? KP(forecolorselected) : KP(forecolor);

	if (KP(gradientstyle)) {
		Gradient_Rect (ADDRESSOF(r_button), backcolor0);
	} else
	if (KP(is_rounded))	
		Draw_RectRound	(ADDRESSOF(r_button), backcolor0, KP(backalpha));
	else				
		Draw_Rect		(ADDRESSOF(r_button), backcolor0, KP(backalpha));

	if (KP(imagesize)) {
		// NO TEXT USES R_IMAGE
		// image no text
		rect_s	r_image;
		RECT_COPY(r_image, KP(tb.r_leftbar));
		if (is_down)
			r_image.left += PRESS_AMOUNT_2, r_image.top += PRESS_AMOUNT_2;

		cachepic_t *p = Draw_CachePic_Flags (k->image_name_a, CACHEPICFLAG_NOTPERSISTENT);
		Draw_ImageCachePic (&r_image, p, k->imagecolor, k->imagealpha);

	} else if (KP(image_name_a)) {
		rect_s	r_image;
		RECT_COPY(r_image, KP(tb.r_leftbar));
		if (is_down)
			r_image.left += PRESS_AMOUNT_2, r_image.top += PRESS_AMOUNT_2;

		cachepic_t *p = Draw_CachePic_Flags (k->image_name_a, CACHEPICFLAG_NOTPERSISTENT);
		Draw_ImageCachePic (&r_image, p, k->imagecolor, k->imagealpha);
	}

	if (KP(caption_a) && KP(caption_a[0]))
		Draw_String	(ADDRESSOF(r_text), s, KP(fontsize), forecolor0, alpha_1_0);
}

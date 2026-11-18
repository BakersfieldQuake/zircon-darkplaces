// a_zircon_ui_1_macros.h

#define Form_Get(f,k)					ktrl f = k->fctrl
#define Form_QueueRefresh(f)			f->frm.refreshneeded ++

RELATED_ (Object_Metric_Recursive)
#define Form_QueueRefreshPlusMetric(f)	f->frm.refreshneeded ++; f->frm.has_metric_run = false
#define Form_QueueDotsRefresh(f)		f->frm.refreshdots ++

// Question of when to do this ...
#define AdjustLeftTopToScreenK(r) \
	r.left += k->container->r_interior.left; \
	r.top += k->container->r_interior.top



#define TXT_CURSOR_AT(k) \
	((k)->txta.cursor_pos + (k)->txta.cursor_length)

#define TXT_STRLEN(k) \
	((int)strlen((k)->text_a))

// This the highlight left.
#define TXT_SELLEFT(K) \
	((K)->txta.cursor_length >= 0 ? (K)->txta.cursor_pos : TXT_CURSOR_AT(K))


// This the highlight right.
#define TXT_SELRIGHT(K) \
	((K)->txta.cursor_length >= 0 ? TXT_CURSOR_AT(K) : (K)->txta.cursor_pos)

// This is number of chars selected.
#define TXT_SELCOUNTABS(K) \
	(abs((K)->txta.cursor_length))

#define TXT_CONTENTS_WIDTH_PX(k) \
	(k)->r_scrollport.width

#define TXT_CURSOR_LEFT_PX(k) \
	(k)->txta._sel_left_px

#define TXT_CURSOR_LENGTH_PX(k) \
	(k)->txta._sel_len_px

// These exist to standardize the loop
#define for_each_form_drawn_reverse(NN,F,KK) \
	for (int NN = F->frm.drawn_list_a->numitems - 1; NN >= 0; NN --) { \
		ktrl KK = F->frm.drawn_list_a->pointers_array[NN];

#define for_each_form_all_controls(NN,F,KK)\
	for (int NN = 0; NN < F->frm.controls_a->numitems; NN ++) { \
		ktrl KK = F->frm.controls_a->pointers_array[NN]; // Ender


CALLERS_ (Form_Draw Object_Refresh_Recursive, Object_Draw_Recursive_Not_Form, Object_Dump_Recursive)
#define for_each_form_control_with_container_as_no_servants(NN,F,KCAN,KK)\
	for (int NN = 0; NN < F->frm.controls_a->numitems; NN ++) { \
		ktrl KK = F->frm.controls_a->pointers_array[NN]; \
		if (KK->container != KCAN) continue; \
		if (KK->servant_owner) continue;  // Ender

#define for_each_form_servant_owner_is_k(NN,F,KSERVANT_OWNER,KK)\
	for (int NN = 0; NN < F->frm.controls_a->numitems; NN ++) { \
		ktrl KK = F->frm.controls_a->pointers_array[NN]; \
		if (KK->servant_owner != KSERVANT_OWNER) continue;  // Ender

CALLERS_ (Form_Hit_Drawn_Any Form_Draw)

#if 0 // Baker: Nov 2 2025 - no one uses this?  Not even draw?
// Baker: Nov 2 2025 -- strange since using this array should allow non-recursive draw.
#define for_each_form_drawn_forward(NN,F,KK) \
	for (int NN = 0; NN < F->frm.drawn_list_a->numitems; NN ++) { \
		ktrl KK = F->frm.drawn_list_a->pointers_array[NN]; // Ender
#endif

#define for_each_end }

#define KIsFocusDrawn(k)  	((k)->fctrl->focusstyle != focusstyle_nohighlight_1)
#define KIsFocused(k)		((k)->fctrl->frm.kfocused == (k)) // Ender
#define KIsDisabled(k)		((k)->is_disabled != 0)
#define KIsNonServant(k)	((k)->servant_owner == NULL)
#define KIsServant(k)		((k)->servant_owner != NULL)
#define KIsNotFocused(k)	((k)->fctrl->frm.kfocused != k)

//
// Font remnants
//

// Only VarMyFont uses
#define ___VarObjectFont(kk) \
	((kk->fontnameindex <= fontnameindex_default_0) ? zdev_dpfont : &dp_fonts.f[kk->fontnameindex] )

#define VarMyFont(myfont) \
	dp_font_t *myfont = k->fontnameindex != fontnameindex_default_0 ? ___VarObjectFont(k) : ___VarObjectFont(k->fctrl)



//
// Rect remnants
//


#define RECT_SEND_ISO_CONTRACT(rect, ISOAMT) \
	(rect).left + (ISOAMT), (rect).top + (ISOAMT), (rect).width - 2 * (ISOAMT), (rect).height - 2 * (ISOAMT) // Ender

#define RECT_SET_CONTRACT(rect,r, ISOAMT) \
	(rect).left		= (r).left + (ISOAMT), \
	(rect).top		= (r).top  + (ISOAMT), \
	(rect).width	= (r).width + (- 2 * (ISOAMT)), \
	(rect).height	= (r).height + (- 2 * (ISOAMT)) // Ender

#define RECT_SET_ADJUST(RDST,rect, L,T,W,H)  (RDST).left   = ((rect).left + (L)),		\
											 (RDST).top    = ((rect).top + (T)),		\
											 (RDST).width  = ((rect).width + (W)),		\
											 (RDST).height = ((rect).height + (H))

#define RECT_SEND_ADJUST(rect,L,T,W,H)	((rect).left + (L)), ((rect).top + (T)), ((rect).width + (W)), ((rect).height + (H))

// unused
#define RECT_MOVE_LEFTOP(rect, r2) \
	(rect).left += (r2).left, (rect).top += (r2).top

#define RECT_MOVE_XY(rect, xys) \
	(rect).left += (xys).x, (rect).top += (xys).y

//
// Draw remnants
//

// Draw_Clip_Clear will flushui draw everytime, does not check to see if clip area was set.
// But looks low cost and immediately exits.
#define Draw_Clip_Clear() \
  		DrawQ_ResetClipArea ()
#define Draw_Clip_Set_Rect(PR) \
 		Draw_Clip_Set ((PR)->left, (PR)->top, (PR)->width, (PR)->height)

// Engine: DrawQ_Fill (PRECT_SEND(PR), VECTOR3_SEND(BACKCOLOR), BACKALPHA, DRAWFLAG_NORMAL_0)
#define Draw_Rect(PR,BACKCOLOR,BACKALPHA) \
	DrawQ_Fill (PRECT_SEND(PR), VECTOR3_SEND(BACKCOLOR), BACKALPHA, DRAWFLAG_NORMAL_0)

#define Draw_Rect_XYWH(X,Y,W,H,BACKCOLOR,BACKALPHA) \
	DrawQ_Fill (X,Y,W,H, VECTOR3_SEND(BACKCOLOR), BACKALPHA, DRAWFLAG_NORMAL_0)

#define Draw_String(PR,S,FONTSIZE,FORECOLOR,FOREALPHA) \
		DrawQ_String_Scale (\
			(PR)->left,\
			(PR)->top,\
			S,\
			maxlen_0, \
			FONTSIZE,\
			FONTSIZE,\
			scale_1_0,\
			scale_1_0, \
			VECTOR3_SEND(FORECOLOR),\
			FOREALPHA, \
			DRAWFLAG_NORMAL_0, \
			OUTCOLOR_NULL, \
			ignorecolorcodes_false,		\
			myfont\
		) // Ender

#define Draw_StringXY(x,y,S,FONTSIZE,FORECOLOR,FOREALPHA) \
		DrawQ_String_Scale (\
			x,\
			y,\
			S,\
			maxlen_0, \
			FONTSIZE,\
			FONTSIZE,\
			scale_1_0,\
			scale_1_0, \
			VECTOR3_SEND(FORECOLOR),\
			FOREALPHA, \
			DRAWFLAG_NORMAL_0, \
			OUTCOLOR_NULL, \
			ignorecolorcodes_false,		\
			myfont\
		) // Ender





// cachepic_s *p = Draw_CachePic_Flags (k->image_name_a, CACHEPICFLAG_NOTPERSISTENT)
#define Draw_ImageCachePic(PR,CACHEPIC,BACKCOLOR,BACKALPHA) \
	DrawQ_Pic((PR)->left, (PR)->top, CACHEPIC, (PR)->width, (PR)->height, VECTOR3_SEND(BACKCOLOR), BACKALPHA, DRAWFLAG_NORMAL_0)

#define Draw_ImageCachePic_ST01(PR,CACHEPIC,BACKCOLOR,BACKALPHA,vst0,vst1) \
	DrawQ_Pic_ST0ST1((PR)->left, (PR)->top, CACHEPIC, (PR)->width, (PR)->height, VECTOR3_SEND(BACKCOLOR), BACKALPHA, DRAWFLAG_NORMAL_0, vst0[0], vst0[1], vst1[0], vst1[1]) // Ender

#define Draw_String_XY(x,y,S,FONTSIZE,FORECOLOR,FOREALPHA) \
		DrawQ_String_Scale (\
			x,\
			y,\
			S,\
			maxlen_0, \
			FONTSIZE,\
			FONTSIZE,\
			scale_1_0,\
			scale_1_0, \
			VECTOR3_SEND(FORECOLOR),\
			FOREALPHA, \
			DRAWFLAG_NORMAL_0, \
			OUTCOLOR_NULL, \
			ignorecolorcodes_false,		\
			myfont\
		) // Ender




// Lists of strings and such - we aren't doing doing huge lists like
// triangles or vertexes, so 4096 is a bit large.

// Baker: This is a struct declaration ... an array of object_s
DEF_COMPAT_ARRAY_STRUCT (oject_array_st, struct _oject_s);

//
// Parse remnants
//

#define Word_Check_Fail_Message(s) \
	if (!*wordray) { \
		Con_PrintLinef (CON_ERROR s);\
		break; \
	} // Ender


//
// Draw functions (not supposed to be here)
//

void Draw_Box (const rect_s *pr, int boxwidth, const vec3_t color, float alpha)
{
	//DrawQ_Fill (pr->left,pr->top,pr->width,boxwidth, (color)[0], (color)[1], (color)[2], alpha, DRAWFLAG_NORMAL_0);

	Draw_Rect_XYWH (pr->left, pr->top,                       pr->width, boxwidth, color, alpha); // TOP
	Draw_Rect_XYWH (pr->left, PRECT_BOTTOMOF(pr) - boxwidth, pr->width, boxwidth, color, alpha); // B

	Draw_Rect_XYWH (pr->left,
						pr->top + boxwidth, boxwidth, pr->height - boxwidth * 2, color, alpha); // L
	Draw_Rect_XYWH (PRECT_RIGHTOF(pr) - boxwidth,
						pr->top + boxwidth, boxwidth, pr->height - boxwidth * 2, color, alpha); // R
}

void Draw_Rect_Adjust4F (const frect *prf, const vec3_t color, float alpha)
{
	frect r = { RECT_SEND_EXPANDED(*prf, 4) };
	DrawQ_Fill (RECT_SEND(r), VECTOR3_SEND(color), alpha, DRAWFLAG_NORMAL_0);
}

void Draw_Rect_Adjust4 (const rect_s *pr, const vec3_t color, float alpha)
{
	rect_s r = { RECT_SEND_EXPANDED(*pr, 4) };
	DrawQ_Fill (RECT_SEND(r), VECTOR3_SEND(color), alpha, DRAWFLAG_NORMAL_0);
}



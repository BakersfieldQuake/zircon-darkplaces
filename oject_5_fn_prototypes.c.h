// oject_5_fn_prototypes.c.h

EXO___

RELATED_ (EditModeSetDotsObject)
typedef char *(*exofn_t)(ktrl f);
typedef int (*exokeyfn_t)(ktrl k, int scancode, int unicode, int is_down);
typedef int (*exointfn_t)(ktrl k);

// Baker: March 5 2026 - We do not do button bits because the mouse down is a specific button was clicked event.
typedef int (*exomouseactionfn_t)(ktrl k, mouse_index_15_e mouseindex, int x, int y, int is_down); // 1 = mouse1, 2 = mouse2, 4 = mouse3
typedef int (*exopopupendedfn_t)(ktrl f, ktrl k_initiator); // popuptype and popupreply will be filled in k_initiator

RELATED_ (Event_Find_Function)
EXO___ char *DevInit (ktrl f);
EXO___ char *DevGridParticlesOnChange (ktrl klistview);

EXO___ char *DevTabSelectOnChange (ktrl kTabSelect1);
EXO___ char *DevListViewSort (ktrl klistview);
EXO___ char *DevListViewOnChange (ktrl klistview);

EXO___ int   DevListViewOnKeyPreview (ktrl k, int scancode, int unicode, int is_down);
	EXO___ char *DevList2ViewOnChange (ktrl klistview);
	EXO___ int   DevFormOnKeyPreview (ktrl k, int scancode, int unicode, int is_down);
	
EXO___ int   DevMouseButtonAction (ktrl k, int button_bits, int x, int y, int is_down);
EXO___ int   DevMouseMoveAction (ktrl k, int button_bits, int x, int y, int is_down);
EXO___ int	 DevPopUpEnded (ktrl f, ktrl k);
EXO___ char *DevTxtFilterOnChange (ktrl k);
EXO___ int   DevTxtFilterBeforeClose (ktrl k);

exofn_t Event_Find_Function(ccs *s_eventname);

typedef struct {
	ccs		*name;
	exofn_t	fn;
	int		is_quakec;
} event_entry_s;

//char *DevListViewOnChange (ktrl k);

#define DOENTRY(x) { STRINGIFY(x), (exofn_t)x }
event_entry_s event_entries[] = {
	//{ "DevInit",	DevInit },
	//{ "DevOnIndex", DevOnIndex },
	DOENTRY(DevInit),
	DOENTRY(DevGridParticlesOnChange),
	DOENTRY(DevTabSelectOnChange),
	DOENTRY(DevTxtFilterOnChange),
	DOENTRY(DevListViewSort),
	DOENTRY(DevListViewOnChange),
	DOENTRY(DevListViewOnKeyPreview),
		DOENTRY(DevList2ViewOnChange),
		DOENTRY(DevFormOnKeyPreview),
		
	DOENTRY(DevMouseButtonAction), // ktrl k, int mouseindex, int x, int y, int is_down
	DOENTRY(DevMouseMoveAction), // ktrl k, int mouseindex, int x, int y, int is_down
	
	DOENTRY(DevPopUpEnded), // ktrl f DevPopUpEnded
	DOENTRY(DevTxtFilterBeforeClose), // int DevTxtFilterBeforeClose (ktrl k)
	
};
#undef DOENTRY

//




void Draw_PolygonBegin (const char *texname_or_null);
void Draw_PolygonVertex2D (const float *v, const float *tc, const float *color3, float alpha);
void Draw_PolygonEnd (void);
void Draw_Fill (const vec3_t pos, const vec3_t size, const vec3_t rgb, float alpha, float flags);

int Rect_Clip_Area_Result (const rect_s *pr_rect, const rect_s *pr_visible, rect_s *pr_clipped_out);

ktrl Object_Create_And_Assign (ktrl *pk, ktrl f, ktrl container, ktrl servant_to, const things_s *po);
ktrl Object_Create_And_Assign_By_ClassEnum (int class_id, ktrl *pk, ktrl f, ktrl container, ktrl servant_to);
ktrl Form_Find_Name (ktrl f, const char *s_value);
qbool Oject_Will_Mouse (ktrl k);
//ktrl Object_Spacing_Refresh (ktrl k);
ktrl Oject_Focus_Set (ktrl k);
void Object_Event_OnChange_Recursive (ktrl k, int shall_recurse);

ktrl Form_ContextMenu_Spawn (ktrl f);
void Form_PressHook_Set (ktrl f, ktrl k, mouse_hook_fn_t fn, timey_dbl_t hookwhen);
void Form_PressHook_Clear (ktrl f, ktrl k);
void Form_Finalize_Refresh (ktrl f);
ktrl Form_Create (void);
void Form_Focus_Next (ktrl f, int dir);
void Object_Method_ZOrder_Front (ktrl kzlast);
//int Form_Find_Idx (ktrl f, ktrl kfind);
void Form_Event_Onload (ktrl f);
void Form_Dump (ktrl f, stringlist_t *plist, dump_detail_e detail_level, mag_s *magni_opt);

ktrl O_Rectangle_Refresh (ktrl k);
ktrl O_ScrollBar_Refresh (ktrl k); // Since ListView and ListBox might call it.
void O_Scrollbar_SetPct (ktrl k, float pct);
ktrl O_SizeDot_Refresh (ktrl k);

void Magnify_Rectf (frect *prf, mag_s *mag, qbool is_contained);
void Magnify_Recti (rect_s *prf, mag_s *mag, qbool is_contained);

void Magnify_Reverse_Recti (rect_s *prf, mag_s *mag, qbool is_contained);
void Magnify_Reverse_Rectf (frect *prf, mag_s *mag, qbool is_contained);

qbool UI_Fly (ccs *s_controlname, ccs *s_property_name, char *valuebuf, size_t valuebuf_size);
qbool UI_Dump (ccs *s_controlname, ccs *s_property_name);

void *CSQC_Form_Destroy_VoidP (void *_f);

void Oject_Set_Infobar (ktrl k);

int Object_Property_Set(ktrl k, ccs *s_property_name, ccs *s_propertyvalue_unquoted);

qbool Form_CSQC_Event_EZ (ktrl k, ccs *s_event_name, float fval, ccs *s_value_string);

qbool Form_CSQC_Event_With_Mouse_EZ (ktrl k, ccs *s_event_name, float fval, ccs *s_value_string, float x, float y);

char *VarPack_String_Zalloc(qbool is_contained, varpack_u *vp, vartype_e property_vt, mag_s *magni_opt, mag_s *magni_reverse, qbool shall_force, qbool *p_did_base64);

void Object_Method_ZOrder_Back (ktrl kzlast);

ktrl Oject_Hidden_Set (ktrl k, int newvalue);

ktrl Form_MouseButtonAction(ktrl f, int key, int unicode, int is_down);

ktrl O_ContextMenu_Leaving (ktrl k, ccs *svalue);

void Form_Dump_Single_Line (ktrl f, stringlist_t *plist, dump_detail_e detail_level, mag_s *magni_opt);

// Lister calls these
ktrl O_ListBox_Refresh (ktrl k);
ktrl O_TabSelect_Refresh_Plus_Early (ktrl k);
ktrl O_ListView_Refresh (ktrl k);
ktrl O_ContextMenu_Refresh_Plus_Early(ktrl k);
ktrl O_ImageGrid_Refresh_Plus_Early (ktrl k);



ktrl Lister_FixedY_Refresh (ktrl k);

//viewcalc_s *ViewCalc_VariableColumns_Set (ktrl k, viewcalc_s *vc, const rect_s *pr_interior);

ktrl Form_Hit_Drawn_Any (ktrl f, int x, int y);

int colwidthx(ktrl k, int col);

ktrl O_TextBox_Refresh (ktrl k);
int EditModeSet (ktrl f, int newval);
void EditModeSetDotsObject (ktrl k);

void ToolTips_End_Any (ktrl f, ccs *s_reason);
void ToolTips_Maybe_Start_One (ktrl f, ktrl k_over_or_null, int x, int y, ccs *reason);

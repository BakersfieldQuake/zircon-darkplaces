// menu_zform.c.h



#include "oject.c.h"

#include "menu_zform_connect.c.h"

RELATED_ (DevInit Form_KeyDown)


static void M_ZDevinfoForm_Draw (void)
{
	GoogleRobotoFont_Check ();
	ktrl f = form1;

	if (!f)
		return;

	if (m_csqc_form_video_restarted) {
		RECT_SET (f->cm.relative_rect, 0, 0, vid.width, vid.height);
		//float newmag = floor((gm.video_fullscreen_res.y /*vid_height*/ / 360.0) * 4) / 4;
		float newmag = floor((vid.height / 360.0) * 4) / 4;
#if 0
		RELATED_ (Object_Metric_Recursive)
		// What is the "gap".
		// What is the effective width?
		float gapwidth = ceil(newmag 

		float fakewidth = 360 *  newmag;
		float fakeheight = 360 * newmag;
		Con_PrintVarFloat (fakewidth);
		Con_PrintVarFloat (fakeheight);
		float fakewidthsurplus = vid.width - fakewidth;
		float fakeheightsurplus = vid.height - fakeheight;
		Con_PrintVarFloat (fakewidthsurplus);
		Con_PrintVarFloat (fakeheightsurplus);
		float autocvar_offset_x = fakewidthsurplus / 2;
		float autocvar_offset_y = fakeheightsurplus / 2;

#endif


		autocvar_magnification = newmag;
		//Con_PrintVarFloat (autocvar_magnification);
		FP(frm.has_metric_run) = false;
		RELATED_ (Object_Refresh_Recursive)
		Form_QueueRefreshPlusMetric (f); //Form_QueueRefresh (f);
		
		Form_QueueDotsRefresh (f);
		m_csqc_form_video_restarted = false;
	}

	Form_Draw (f);
}

RELATED_ (Object_Draw Form_MouseButtonAction)
static void M_ZDevinfoForm_Key (cmd_state_t *cmd, int key, int unicode, int is_down)
{
	if (!form1)
		return;

	switch (key) {

	case K_ESCAPE:
		// Determine context
		if (is_down) {
			//if (form1 && form1->servo.kcontextmenu->is_hidden == false) { // CONTEXTOID (devinfo!)
			//	// Context menu is active.
			//	Form_KeyDown (form1, key, unicode, is_down);
			//	break; // GET OUT
			//}

			if (form1) {
				ktrl handled_k = Form_KeyDown (form1, key, unicode, is_down);
				if (handled_k)
					break;
				// Not handled ...
			}
			KeyDest_Set (key_game); menu_state_set_nova (m_none);  // simply leave menu
			//M_Menu_Main_f(cmd); up 1 level
			RELATED_ (M_Menu_Main_f)
		}
		break;

	case K_MOUSE1:
		Form_MouseButtonAction (form1, key, unicode, is_down);
		break;

	case K_MOUSE2:
		if (is_down) Form_Mouse2Down (form1);
		else {
			Form_Mouse2Up(form1);
		}
		break;

	case K_TAB:
		if (is_down)
		Form_Focus_Next (form1, KM_SHIFT ?  FOCUS_PREV_NEG1 : FOCUS_NEXT_1);
		break;

	default:
		Form_KeyDown (form1, key, unicode, is_down);
		break;
	} // sw
}

int old_in_windowmouse_x = 0; // Only used here to prevent junk mousemoves
int old_in_windowmouse_y = 0;

RELATED_ (Consel_MouseMove_Check M_ToggleMenu M_CSQC_Form_Key things )
void ZForm_MouseMove (int x, int y)
{
	if (MVM_prog->loaded == false && isin2 (m_state, m_devinfo_30, m_csqc_form_32)) {
		if (old_in_windowmouse_x != x || old_in_windowmouse_y != y) {
			old_in_windowmouse_x = x, old_in_windowmouse_y = y; // Prevent "no move" mouse moves
			if (m_state == m_csqc_form_32) {
				if (csqc_form) { // FIX THIS!
					if (ui_full_canvas.integer == 0 /*d:1*/) {
						// This is not the norm.
						x *= vid_conwidth.integer / (float)vid.width; // SIZEOIC
						y *= vid_conheight.integer / (float)vid.height;
					}
					Form_Mouse_Move (csqc_form, x, y);
				}
			} else {
				if (form1) {
					Form_Mouse_Move (form1, x, y);
				}
			}
		}
	}
}


void UI_Form_Load_f (cmd_state_t *cmd)
{
	if (cmd_argc < 2) {
		Con_PrintLinef ("usage:" NEWLINE "%s <form name>", cmd_argv_command_0);
		return;
	}
	ccs *sform = cmd_argv_parm1;
	extern cvar_t _scr_devinfo;
	Cvar_SetQuick (&_scr_devinfo, sform); 
	M_Menu_DevInfo_f (cmd_local);
}

// Baker: This doesn't have enough "junk"
RELATED_ (M_Menu_Keys_f)
void M_Menu_DevInfo_Free_f (cmd_state_t *cmd)
{
	KeyDest_Set (key_game); menu_state_set_nova (m_none);  // simply leave menu
	if (form1) {
		// Delete the form first
		Con_PrintLinef ("Freeing form1 ...");
		
		form1 = Form_Destroy (form1);
	}

}



RELATED_ (M_ZDevinfoForm_Key)

void M_Menu_DevInfo_f (cmd_state_t *cmd)
{
	KeyDest_Set (key_menu);
	menu_state_set_nova (m_devinfo_30);
	Con_CloseConsole_If_Client();
	m_entersound = true;

	g_s_devinfo_argnum = cmd_argv_parm1; // Feed devinfo the arg number.

	if (!devinfomempool) {
		devinfomempool = Mem_AllocPool("devinfo", /*flags*/ 0, /*parent*/ NULL);
	}

	// Dump it
	if (0 && form1) {
		stringlist_t lines = {0};
		Form_Dump (form1, &lines, DUMP_DETAIL_SAVE_FILE_0, magni_opt_null);
		stringlistprint_with_title (&lines, Con_PrintLinef, "Form Dump detail level (0 to 2) = %d", DUMP_DETAIL_SAVE_FILE_0 );
		stringlistfreecontents (&lines);

		Form_Print_Zones (form1);
		Form_Draw_Dump (form1, &lines);
		stringlistprint_with_title (&lines, Con_PrintLinef, "Drawn list = %d", form1->frm.drawn_list_a->numitems);
		stringlistfreecontents (&lines);
	}

#ifdef _DEBUG
	Things_Audit_Debug (); // Checks enums and stuff (Leaks nothing we see: June 2 2025)
#endif

	GoogleRobotoFont_Check ();

	//extern cvar_t _scr_devinfo; // _scr_devinfo engine/form1.txt
	va_super (sbuf, MAX_QPATH_128, "%s", _scr_devinfo.string /*d:"engine/form1.txt"*/); // Set form1.txt text

	File_URL_Edit_Default_Extension (sbuf, ".txt", sizeof(sbuf));

	if (form1) {
		// Delete the form first
		if (ezdev->kTabSelect1 && ezdev->kList1 && ezdev->kList2 && ezdev->kList3) {
			g_old_tab_idx = ezdev->kTabSelect1->selectedindex;
			g_old_listview1_idx = ezdev->kList1->selectedindex;
			g_old_listview2_idx = ezdev->kList2->selectedindex;
			g_old_listview3_idx = ezdev->kList3->selectedindex;
			//#pragma message ("This is where to try to capture values");
		}
			form1 = Form_Destroy (form1);
	}

	char *s_zalloc = FS_LoadFile_Quiet_Temp (sbuf);
	if (!s_zalloc) {
		Con_PrintLinef ("Error loading from file %s", sbuf);
		return;
	}

	float newmag = floor((vid.height / 360.0) * 4) / 4;
	autocvar_magnification = newmag;
	//Con_PrintVarFloat (autocvar_magnification);

#if 3926
	if (menu_state_reenter == m_devinfo_30) {
		ezdev->try_keep_indexes = true;
	}
#endif // 3926

	form1 = Form_Create_From_String (s_zalloc, magnify_false, /*handle plus 1*/ 0, /*callback func_t none which is zero*/ 0, /*close*/ 0, /*sMessage*/ NULL, /*modalBoss*/ NULL);

	Mem_FreeNull_ (s_zalloc);

#if 3926
	RELATED_ (DevInit)
	if (g_s_devinfo_argnum && g_s_devinfo_argnum[0] && ezdev->kTabSelect1) {
		ezdev->kTabSelect1->selectedindex = atoi(g_s_devinfo_argnum);
	} else if (ezdev->try_keep_indexes && ezdev->f && ezdev->f->wasinitialized && ezdev->kList1) {
		if (ezdev->f->frm.kfocused != ezdev->kList1) {
			Oject_Focus_Set (ezdev->kList1); // Set re-entrant focus to List1
		}
		ezdev->try_keep_indexes = false;
	}
#endif

	if (!form1) {
		Con_PrintLinef ("Error parse %s", sbuf);
		KeyDest_Set (key_game); menu_state_set_nova (m_none);  // simply leave menu
		return;
	}
	// Dump it
	if (0 && form1) {
		stringlist_t lines = {0};
		Form_Dump (form1, &lines, DUMP_DETAIL_PLUS_CLIPPING_2, magni_opt_null);
		stringlistprint_with_title (&lines, Con_PrintLinef, "Form Dump detail level = %d", DUMP_DETAIL_PLUS_CLIPPING_2);
		stringlistfreecontents (&lines);
	}

	RELATED_ (DevInit)
	if (_scr_devinfo_reenter.integer)
		menu_state_reenter = m_devinfo_30;
	else
		menu_state_reenter = 0;
}



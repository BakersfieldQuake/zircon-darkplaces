// oject.c.h

// Control Type Names should be insensitive (caseless)
// Property Names are case insensitive (caseless)
// Events are case insensitive (caseless)
// Methods are case insensitive (caseless)

// Object Names are case sensitive (case matters) for now (and might remain so)


#ifndef OJECT_C_H
#define OJECT_C_H

//typedef oject_s* ktrl;

RELATED_ (_oject_s oject_s)
//#define ktrl			struct _oject_s* 
//#define int			int
//#define qbool			qbool
//#define PSTRING			const char *
//#define HOSTREALTIME	host.realtime
//#define oject_s		struct _oject_s

extern cvar_t ui_export_magnification;
extern cvar_t ui_import_magnification;
extern cvar_t ui_full_canvas;

RELATED_ (VM_stringwidth Draw_StringWidthf _oject_s)
// SERVANT
// DOES NOT FOCUS, KFOCUS, ideally fires parent's event for it
// They are zebrasz

//  USES PARENT FOOTPRINT
//  USES PARENT CANVAS (INTERIOR)
//  DRAWS
//  IS NOT "CONTAINED" because that is coordinate system based off canvas interior.

	// INHERITED FROM FORM
	//size_t sizeofvec3_t = sizeof(vec3_t); // is this 12
	//typedef double dvec3_t[3];
	//size_t sizeofdvec3_t = sizeof(dvec3_t); // is this 12 or 24? ANSWER: 24 WORKS!
	// sizeofvec3_t does an expected!  DOES!


#include "oject_1_const_enums.c.h"
//#include "oject_2_macros.c.h"
#include "oject_3_structs.c.h"
#include "oject_3_structs_oject.c.h"
#include "oject_4_globals.c.h"
#include "oject_5_fn_prototypes.c.h"
#include "oject_6_table___aaaa.c.h"

extern mempool_t *devinfomempool;

// CODE
#include "oject_aaa_procs.c.h"
#include "oject_aab_procs_draw.c.h"
#include "oject_aac_exec.c.h"

#include "ka_lister_procs.c.h"
#include "ka_lister.c.h"

#include "k_button.c.h"
#include "k_checkbox.c.h"
#include "k_ext_closer.c.h" // Hides a control (EVENT?)
#include "k_ext_mover.c.h" // Moves a control
#include "k_contextmenu.c.h"
#include "k_image.c.h"
#include "k_imagegrid.c.h"
#include "k_label.c.h"
#include "k_listbox.c.h"
#include "k_listview_procs.c.h"
#include "k_listview.c.h"
#include "k_polygon.c.h"
#include "k_rectangle.c.h"
#include "k_scrollbar.c.h"
#include "k_sizedot.c.h"
#include "k_tabselect.c.h"
#include "k_textbox_procs.c.h"
#include "k_textbox.c.h"
#include "k_textbox_keys.c.h"
#include "k_textbox_mouse.c.h"

#include "oject_cca_properties.c.h"
#include "oject_ccb_object.c.h"
#include "oject_ccc_object_event.c.h"
#include "oject_ccd_object_method.c.h"

#include "kz_form_misc.c.h"
#include "kz_form_parse.c.h"
#include "kz_form_keymouse.c.h"
#include "kz_form.c.h"

#endif // !OJECT_C_H



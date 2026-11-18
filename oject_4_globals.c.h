// oject_4_globals.c.h

const vec3_t 	color3_gray_25		= { 0.25, 0.25, 0.25}; // 64ish
const vec3_t 	color3_gray_50		= { 0.50, 0.50, 0.50}; // 127ish
const vec3_t 	color3_gray_75		= { 0.75, 0.75, 0.75}; // 192ish
const vec3_t 	color3_gray_875		= { 0.875, 0.875, 0.875}; // 224?
const vec3_t 	color3_black		= { 0, 0, 0};
const vec3_t 	color3_white		= { 1, 1, 1};
const vec3_t 	color3_red			= { 1, 0, 0};
const vec3_t 	color3_green		= { 0, 1, 0};
const vec3_t 	color3_blue			= { 0, 0, 1};
const vec3_t 	color3_yellow		= { 1, 1, 0};
const vec3_t 	color3_purple		= { 1, 0, 1};
const vec3_t 	color3_form208		= { RGBFLOT(212,208,200) };

ojpoly_s ojpoly;
ktrl form1;
ktrl csqc_form;

RELATED_ (M_ZDevinfoForm_Draw) // Both engine sets of magnification occur there
// Q: May 30 2026 - where does QuakeC magnification occur.
// A: Zircon_UI_ConsizeThink
float autocvar_magnification = 2; // magnix - increments of 0.25
float autocvar_offset_x = 0;
float autocvar_offset_y = 0;




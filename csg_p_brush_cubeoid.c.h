// csg_p_brush_cubeoid.c.h

#define BRUSH_CUBEOID
RELATED_ (BRUSH_MAIN BRUSH_DIRECTION)

#pragma once

CALLERS_ (VM_map_entity_brush_add_cubeoid)
// Return brush number?
brushnum_int map_add_cubeoid (map_t *hm, int entnum, vec3_t floor_mins, vec3_t floor_maxs, ccs *texturename, float texture_scale_xy, TEXSIDE_INT_e texturesideflags/*TEXSIDE_CEIL_16*/)
{
	return brush_add_cubeoid (hm, ITOE(hm, entnum), floor_mins, floor_maxs, texturename, texture_scale_xy, texture_scale_xy, texturesideflags);
}

RELATED_ (TEXSIDE_NORTH_1)

RELATED_ (map_add_cubeoid)
int brush_add_cubeoid_lite (map_t *plist, entityx_s *d_ent, vec3_t brmins, vec3_t brmaxs, ccs *texturename)
{
	int brushnum = brush_add_cubeoid(plist, d_ent, brmins, brmaxs, texturename, /*texture scale x*/ 1.0, 1.0, TEXSIDE_ALL_63);
	return brushnum;
}

brushnum_int brush_add_cubeoid (map_t *hm, entityx_s *d_ent, vec3_t brmins, vec3_t brmaxs,
  ccs *texturename, float texture_scale_x, float texture_scale_y, TEXSIDE_INT_e wall_side_flags)
{
#define XH brmaxs[0]
#define YH brmaxs[1]
#define ZH brmaxs[2]
#define XL brmins[0]
#define YL brmins[1]
#define ZL brmins[2]

	vec3_t brdims;
	VectorSubtract (brmaxs, brmins, brdims);

	if (brdims[0] <= 0 || brdims[1] <= 0 || brdims[2] <= 0) {
		return not_found_neg1;
	}

	int brush_number = d_ent->brushlist.count;
	brush_s *d_brush = _LIFE_brush_add (&d_ent->brushlist); // CREATE BRUSH FOR DEST

	d_brush->is_cubeoid = true; // brush_add_cubeoid
	VectorCopyDestSrc (d_brush->bxmins, brmins);
	VectorCopyDestSrc (d_brush->bxmaxs, brmaxs);
	VectorCopyDestSrc (d_brush->bxdims, brdims);

	brushrow_s *br;
	qbool is_textured;
//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall	[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018			[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}
west_face: // XH is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_WEST_8);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	//( [XL] [YL] [ZH] ) ( [XL] [YL] [ZL] ) ( [XL] [YH] [ZH] ) common/caulk [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XL, YL, ZH); VectorSet (br->b, XL, YL, ZL); VectorSet (br->c, XL, YH, ZH);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	Vector4Set(br->tex_axis_s, 0, 1,  0, 0);
	Vector4Set(br->tex_axis_t, 0, 0, -1, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value
	br->trail_count = 3; // I guess?

	//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall	[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018			[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}
east: // XL is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_EAST_4);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	// ( [XH] [YH] [ZH] )                ( [XH] [YH] [ZL] )          ( [XH] [YL] [ZH] )
	VectorSet (br->a, XH, YH, ZH); VectorSet (br->b, XH, YH, ZL); VectorSet (br->c, XH, YL, ZH);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	Vector4Set(br->tex_axis_s, 0, 1,  0, 0);
	Vector4Set(br->tex_axis_t, 0, 0, -1, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value
	br->trail_count = 3; // I guess?

//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky			[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall		[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018				[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}

north: // YL is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_NORTH_1);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.

//	// ( [XL] [YH] [ZH] ) ( [XL] [YH] [ZL] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XL, YH, ZH); VectorSet (br->b, XL, YH, ZL); VectorSet (br->c, XH, YH, ZH);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	Vector4Set(br->tex_axis_s, 1, 0,  0, 0);
	Vector4Set(br->tex_axis_t, 0, 0, -1, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value
	br->trail_count = 3; // I guess?

//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky			[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall		[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018				[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}
south: // YH is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_SOUTH_2);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.

//	//( [XH] [YL] [ZH] ) ( [XH] [YL] [ZL] ) ( [XL] [YL] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XH, YL, ZH); VectorSet (br->b, XH, YL, ZL); VectorSet (br->c, XL, YL, ZH);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	Vector4Set(br->tex_axis_s, 1, 0,  0, 0);
	Vector4Set(br->tex_axis_t, 0, 0, -1, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value
	br->trail_count = 3; // I guess?

//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall	[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018			[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}
floor: // ZH is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_FLOOR_32);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	// ( [XL] [YH] [ZL] ) ( [XL] [YL] [ZL] ) ( [XH] [YH] [ZL] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet	(br->a, XL, YH, ZL); VectorSet (br->b, XL, YL, ZL); VectorSet (br->c, XH, YH, ZL);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	br->trail_count = 3; // I guess?
	Vector4Set(br->tex_axis_s, 1,  0, 0, 0);
	Vector4Set(br->tex_axis_t, 0, -1, 0, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value

//{
//N ( -288 256 64 ) ( -288 256 0 ) ( 256 256 64 ) azirc0/window_blinds_white_01		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//S ( 256 -256 64 ) ( 256 -256 0 ) ( -288 -256 64 ) skies/distant_sunset_sky		[ 1 0 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125 0 36 0
//E ( 256 256 64 ) ( 256 256 0 ) ( 256 -256 64 ) cc0-textures/japanese_stone_wall	[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//W ( -288 -256 64 ) ( -288 -256 0 ) ( -288 256 64 ) ambientcg/Bricks018			[ 0 1 0 0 ] [ 0 0 -1 0 ] 0 0.125 0.125
//C( 256 -256 64 ) ( -288 -256 64 ) ( 256 256 64 ) skies/mandelbrot_sky				[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125 0 36 100
//F( -288 256 0 ) ( -288 -256 0 ) ( 256 256 0 ) decals_sh/concrete_0001_dirtystain2	[ 1 0 0 0 ] [ 0 -1 0 0 ] 0 0.125 0.125
//}
//}
ceil: // ZL is missing
	is_textured = Have_Flag_Strict_Bool(wall_side_flags, TEXSIDE_CEIL_16);
	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	// ( [XH] [YL] [ZH] ) ( [XL] [YL] [ZH] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XH, YL, ZH); VectorSet (br->b, XL, YL, ZH); VectorSet (br->c, XH, YH, ZH);
	br->pbrtexture = Z_StrDup (is_textured ? texturename : "common/caulk");
	br->trail_count = 3; // I guess?
	Vector4Set(br->tex_axis_s, 1,  0, 0, 0);
	Vector4Set(br->tex_axis_t, 0, -1, 0, 0);
	Vector3Set(br->faceparms, 0, texture_scale_x, texture_scale_y); //rotation Uscale Vscale SurfaceFlag ContentsFlag Value

#undef XH
#undef YH
#undef ZH
#undef XL
#undef YL
#undef ZL
	return brush_number; // isok
}




RELATED_ (TEXSIDE_NORTH_1)
// samey --- if z == z, it inherits everything (Repeat for X and Y)
//           if z != z, it inherits everything but gets "common/caulk" as the texture
brushnum_int brush_add_as_cubeoid_texturing_like (map_t *plist, entityx_s *d_ent,
 const int r_brushnum, vec3_t brmins, vec3_t brmaxs, FACE_INT_e unused_cubeoid_nsewcf)
{
	vec3_t brdims;
	VectorSubtract (brmaxs, brmins, brdims);
	if (brdims[0] <= 0 || brdims[1] <= 0 || brdims[2] <= 0) {
		zpr (CON_RED "%s: brush dimension <= 0 in size", __func__);
		return false; // This is an error
	}

#ifdef _DEBUG
	static int iters = 0;
	vec3_t size;
	VectorSubtract(brmaxs, brmins, size);
	DebugPrintLinef ("Cubeoid: %d " VECTOR3_G " to " VECTOR3_G " size = " VECTOR3_G, iters, VECTOR3_SEND(brmins), VECTOR3_SEND(brmaxs), VECTOR3_SEND(size));
	iters ++;
	if (iters > 50)
		iters = iters;
#endif
	int brush_number = d_ent->brushlist.count;
	brush_s *d_brush = _LIFE_brush_add (&d_ent->brushlist); // CREATE BRUSH FOR DEST

#ifdef _DEBUG
	int entnum = ENT_GET_NUM(plist, d_ent);// ARRAY_INDEX (d_ent, plist->entity);
	//DebugPrintLinef ("Created Colliding carve %d:%d with world %d:%d", rentnum, rbrushnum, entnum, d_brushnum);
#endif


	VectorCopyDestSrc (d_brush->bxmins, brmins);
	VectorCopyDestSrc (d_brush->bxmaxs, brmaxs);
	VectorCopyDestSrc (d_brush->bxdims, brdims);
	d_brush->is_cubeoid = true;  // brush_add_cubeoid

	brush_add_faces_as_cubeoid_texturing_like (plist, d_ent, d_brush, r_brushnum, brmins, brmaxs, unused_cubeoid_nsewcf);
	return brush_number; // OK
}







//
//
//

CALLERS_ (csge_originmake _csge_heightmap_ent_num_added)
// Baker: September 3 2026 -- oddly popular
qbool brush_add_cubeoid_with_texture (entityx_s *d_ent, const vec3_t brmins, const vec3_t brmaxs, ccs *texturename)
{
	vec3_t brdims; VectorSubtract (brmaxs, brmins, brdims);
	brush_s *d_brush = _LIFE_brush_add (&d_ent->brushlist); // CREATE BRUSH FOR DEST
	d_brush->is_cubeoid = true;  // brush_add_cubeoid
	//vec3_t brdims2; VectorSubtract (brmaxs, brmins, brdims2);

	if (brdims[0] <= 0 || brdims[1] <= 0 || brdims[2] <= 0)
		return false;

#define XH brmaxs[0]
#define YH brmaxs[1]
#define ZH brmaxs[2]
#define XL brmins[0]
#define YL brmins[1]
#define ZL brmins[2]

	brushrow_s *br;

	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // 2 - all numbers identical to 1 except abc

	//( [XL] [YL] [ZH] ) ( [XL] [YL] [ZL] ) ( [XL] [YH] [ZH] ) common/caulk [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XL, YL, ZH); VectorSet (br->b, XL, YL, ZL); VectorSet (br->c, XL, YH, ZH); // WEST
	Vector4Set (br->tex_axis_s, 0, 1, 0, -20.4 ); Vector4Set (br->tex_axis_t, 0, 0, -1, 6.29999 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");

	br = _LIFE_brushface_alloc (&d_brush->brushrowlist);
	//          ( [XH] [YH] [ZH] )                ( [XH] [YH] [ZL] )          ( [XH] [YL] [ZH] )
	VectorSet (br->a, XH, YH, ZH); VectorSet (br->b, XH, YH, ZL); VectorSet (br->c, XH, YL, ZH); // EAST
	Vector4Set (br->tex_axis_s, 0, 1, 0, -20.4 ); Vector4Set (br->tex_axis_t, 0, 0, -1, 6.29999 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	//553648128 16512 0
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");


	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // 3
	//( [XH] [YL] [ZH] ) ( [XH] [YL] [ZL] ) ( [XL] [YL] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XH, YL, ZH); VectorSet (br->b, XH, YL, ZL); VectorSet (br->c, XL, YL, ZH); // SOUTH
	Vector4Set (br->tex_axis_s, 1, 0, 0, 20 ); Vector4Set (br->tex_axis_t, 0, 0, -1, 6.29999 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");

	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // 4 - all numbers identical to 3 except abc
	// ( [XL] [YH] [ZH] ) ( [XL] [YH] [ZL] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XL, YH, ZH); VectorSet (br->b, XL, YH, ZL); VectorSet (br->c, XH, YH, ZH); // NORTH
	Vector4Set (br->tex_axis_s, 1, 0, 0, 20 ); Vector4Set (br->tex_axis_t, 0, 0, -1, 6.29999 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");

	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // 5
	// ( [XL] [YH] [ZL] ) ( [XL] [YL] [ZL] ) ( [XH] [YH] [ZL] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XL, YH, ZL); VectorSet (br->b, XL, YL, ZL); VectorSet (br->c, XH, YH, ZL); // FLOOR
	Vector4Set (br->tex_axis_s, 1, 0, 0, 20 ); Vector4Set (br->tex_axis_t, 0, -1, 0, 20.4 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");

	br = _LIFE_brushface_alloc (&d_brush->brushrowlist); // 6 - all ident to 5 except a b c
	// ( [XH] [YL] [ZH] ) ( [XL] [YL] [ZH] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet (br->a, XH, YL, ZH); VectorSet (br->b, XL, YL, ZH); VectorSet (br->c, XH, YH, ZH); // CEIL
	Vector4Set (br->tex_axis_s, 1, 0, 0, 20 ); Vector4Set (br->tex_axis_t, 0, -1, 0, 20.4 );
	Vector6Set (br->faceparms, 0, 0.5, 0.5, 553648128, 16512, 0); br->trail_count = 6;
	br->pbrtexture = Z_StrDup (texturename); //"common/origin");

#undef XH
#undef YH
#undef ZH
#undef XL
#undef YL
#undef ZL

	return true; // isok
}

CALLERS_ (entitylist_perform_layout_from_boxes)
// Named map because it returns a brush number.
int map_add_brush_cubeoid_with_texture(entityx_s *d_ent, const vec3_t brmins, const vec3_t brmaxs,
									   ccs *texturename)
{
	vec3_t brdims; VectorSubtract (brmaxs, brmins, brdims);

	if (brdims[0] <= 0 || brdims[1] <= 0 || brdims[2] <= 0)
		return not_found_neg1;

	int newidx = d_ent->brushlist.count;
	brush_add_cubeoid_with_texture (d_ent, brmins, brmaxs, texturename);

	return newidx; // isok
}


// samey --- if z == z, it inherits everything (Repeat for X and Y)
//           if z != z, it inherits everything but gets "common/caulk" as the texture
brushnum_int brush_add_as_cubeoid_texturing_like_2_weld (map_t *hm, entityx_s *d_ent,
 const int r_brushnum1,
 const int r_brushnum2,
 vec3_t brmins, vec3_t brmaxs, FACE_INT_e unused_cubeoid_nsewcf)
{
	vec3_t brdims;
	VectorSubtract (brmaxs, brmins, brdims);
	if (brdims[0] <= 0 || brdims[1] <= 0 || brdims[2] <= 0) {
		zpr (CON_RED "%s: brush dimension <= 0 in size", __func__);
		return false; // This is an error
	}

	int brush_number = d_ent->brushlist.count;
	brush_s *d_brush = _LIFE_brush_add (&d_ent->brushlist); // CREATE BRUSH FOR DEST
//////// BRUSH CREATED
	int entnum = ENT_GET_NUM (hm, d_ent); //ARRAY_INDEX (d_ent, hm->entity);

	VectorCopyDestSrc (d_brush->bxmins, brmins);
	VectorCopyDestSrc (d_brush->bxmaxs, brmaxs);
	VectorCopyDestSrc (d_brush->bxdims, brdims);
	d_brush->is_cubeoid = true;  // brush_add_cubeoid

	brush_add_faces_as_cubeoid_texturing_like (hm, d_ent, d_brush, r_brushnum1, brmins, brmaxs, unused_cubeoid_nsewcf);

// SIMPLE WAY IS HERE DOING THE COPYING
	BRUSH_DEFINE_CONST (r_brush1, hm, entnum, r_brushnum1);
	BRUSH_DEFINE_CONST (r_brush2, hm, entnum, r_brushnum2);

	const brushrow_s *w = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_WEST_0]];
	const brushrow_s *e = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_EAST_1]];
	const brushrow_s *s = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_SOUTH_2]];
	const brushrow_s *n = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_NORTH_3]];

	// These 2 better be the same no matter what
	const brushrow_s *f = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_FLOOR_4]];
	const brushrow_s *c = &r_brush1->brushrowlist.brushrow[r_brush1->cubeoid_index[FACE_CEIL_5]];

	qbool ok = false;
	if (r_brush1->triangle_face_bits == TEXSIDE_NORTHWEST_57) {
		ok = r_brush1->triangle_face_bits == TEXSIDE_SOUTHEAST_54;
		s = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_SOUTH_2]];
		e = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_EAST_1]];
	}
	else if (r_brush1->triangle_face_bits == TEXSIDE_NORTHEAST_53) {
		ok = r_brush1->triangle_face_bits == TEXSIDE_SOUTHWEST_58;
		s = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_SOUTH_2]];
		w = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_WEST_0]];
	}
	else if (r_brush1->triangle_face_bits == TEXSIDE_SOUTHEAST_54) {
		ok = r_brush1->triangle_face_bits == TEXSIDE_NORTHWEST_57;
		n = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_NORTH_3]];
		w = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_WEST_0]];
	}

	else if (r_brush1->triangle_face_bits == TEXSIDE_SOUTHWEST_58) {
		ok = r_brush1->triangle_face_bits == TEXSIDE_NORTHEAST_53;
		n = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_NORTH_3]];
		e = &r_brush2->brushrowlist.brushrow[r_brush2->cubeoid_index[FACE_EAST_1]];
	} else {
		//int j = 5;
		// Error condition
	}

	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_WEST_0], w);
	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_EAST_1], e);
	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_SOUTH_2], s);
	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_NORTH_3], n);
	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_FLOOR_4], f);
	face_copy_texturedata (&d_brush->brushrowlist.brushrow[FACE_CEIL_5], c);

	return brush_number; // OK
}

qbool brush_add_faces_as_cubeoid_texturing_like (map_t *hm, entityx_s *d_ent, brush_s *d_brush,
	const int r_brushnum, vec3_t brmins, vec3_t brmaxs, FACE_INT_e unused_cubeoid_nsewcf)
{
#define XH brmaxs[0]
#define YH brmaxs[1]
#define ZH brmaxs[2]
#define XL brmins[0]
#define YL brmins[1]
#define ZL brmins[2]

	ccs *newtexturename;
	const brushrow_s *mirror_face;
	const int entnum = ENT_GET_NUM(hm, d_ent);// ARRAY_INDEX (d_ent, hm->entity);
	//const brush_s *b;
	brushrow_s *brush_face;
	qbool is_textured;
	int new_facenum;

	const brush_s *mirror_brush = BRUSH_GET (hm, entnum, r_brushnum);

west_face: // XH is missing
	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_WEST_0] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*WEST*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_WEST_0]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmins[0] == brmins[0]; // SAME WEST X == textured otherwise caulk
	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	//( [XL] [YL] [ZH] ) ( [XL] [YL] [ZL] ) ( [XL] [YH] [ZH] ) common/caulk [ 0 1 0 -20.4 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (brush_face->a, XL, YL, ZH); VectorSet (brush_face->b, XL, YL, ZL); VectorSet (brush_face->c, XL, YH, ZH);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);

east: // XL is missing

	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_EAST_1] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*EAST*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_EAST_1]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmaxs[0] == brmaxs[0]; // SAME EAST X == textured otherwise caulk

	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	// ( [XH] [YH] [ZH] )                ( [XH] [YH] [ZL] )          ( [XH] [YL] [ZH] )
	VectorSet (brush_face->a, XH, YH, ZH); VectorSet (brush_face->b, XH, YH, ZL); VectorSet (brush_face->c, XH, YL, ZH);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);

south: // YH IS MISSING
	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_SOUTH_2] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*SOUTH*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_SOUTH_2]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmins[1] == brmins[1]; // SAME SOUTH Y == textured otherwise caulk

	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	//( [XH] [YL] [ZH] ) ( [XH] [YL] [ZL] ) ( [XL] [YL] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (brush_face->a, XH, YL, ZH); VectorSet (brush_face->b, XH, YL, ZL); VectorSet (brush_face->c, XL, YL, ZH);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);

north: // YL IS MISSING
	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_NORTH_3] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*NORTH*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_NORTH_3]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmaxs[1] == brmaxs[1]; // SAME NORTH Y == textured otherwise caulk

	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	// ( [XL] [YH] [ZH] ) ( [XL] [YH] [ZL] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 0 -1 6.29999 ] 0 0.5 0.5 0 160 0
	VectorSet (brush_face->a, XL, YH, ZH); VectorSet (brush_face->b, XL, YH, ZL); VectorSet (brush_face->c, XH, YH, ZH);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);


floor: // ZH IS MISSING
	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_FLOOR_4] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*FLOOR*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_FLOOR_4]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmins[2] == brmins[2]; // SAME FLOOR Z == textured otherwise caulk

	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	// ( [XL] [YH] [ZL] ) ( [XL] [YL] [ZL] ) ( [XH] [YH] [ZL] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet	(brush_face->a, XL, YH, ZL); VectorSet (brush_face->b, XL, YL, ZL); VectorSet (brush_face->c, XH, YH, ZL);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);

ceil: // ZL IS MISSING
	new_facenum = d_brush->brushrowlist.count;
	d_brush->cubeoid_index[FACE_CEIL_5] = new_facenum;
	brush_face = _LIFE_brushface_alloc (&d_brush->brushrowlist); // NEW BRUSH ROW.
	/*CEIL*/ mirror_face = &mirror_brush->brushrowlist.brushrow[mirror_brush->cubeoid_index[FACE_CEIL_5]]; // AFTER ADD TO AVOID PAGE ARRAY STALE POINTER FOR BRUSH AND BRUSHROW

	is_textured = mirror_brush->bxmaxs[2] == brmaxs[2]; // SAME FLOOR Z == textured otherwise caulk

	newtexturename = is_textured ? mirror_face->pbrtexture : COMMON_CAULK;

	// ( [XH] [YL] [ZH] ) ( [XL] [YL] [ZH] ) ( [XH] [YH] [ZH] ) common/caulk [ 1 0 0 20 ] [ 0 -1 0 20.4 ] 0 0.5 0.5 0 160 0
	VectorSet (brush_face->a, XH, YL, ZH); VectorSet (brush_face->b, XL, YL, ZH); VectorSet (brush_face->c, XH, YH, ZH);

	brush_face->pbrtexture = Z_StrDup (newtexturename); // GENERIC
#ifdef _DEBUG
	if (brmaxs[2] == 0) {
		int j = 5;

	}
#endif // _DEBUG

	brush_face->trail_count = mirror_face->trail_count;
	Vector4CopyDestSrc(brush_face->tex_axis_s, mirror_face->tex_axis_s);
	Vector4CopyDestSrc(brush_face->tex_axis_t, mirror_face->tex_axis_t);
	Vector6CopyDestSrc(brush_face->faceparms,  mirror_face->faceparms);

#undef XH
#undef YH
#undef ZH
#undef XL
#undef YL
#undef ZL
	return true; // OK
} // FACE ADD ONLY



qbool brush_add_alpha_brush (map_t *hm, int entnum, const vec3_t point3d, float alpha)
{	
	alpha = bound (0, alpha, 1);
	if (alpha == 0) alpha = 0; // Get rid of stupid negative 0 problem.

	ENT_DEFINE (ent, hm, entnum);
	
	vec3_t brmins; VectorCopyDestSrc (brmins, point3d);
	vec3_t brmaxs; VectorCopyDestSrc (brmaxs, point3d);

	for (int d = 0; d < 3; d ++) {
		brmins[d] -= ALPHA_BRUSH_SIZE_HALF_48; 
		brmaxs[d] += ALPHA_BRUSH_SIZE_HALF_48;
	} // for

	va_super (texturename, 128, COMMON_ALPHA "_%1.2f", alpha);
	String_Edit_Replace (texturename, sizeof(texturename), ".", "_");
	
	int brushnum = brush_add_cubeoid_with_texture (ent, brmins, brmaxs, texturename);
	return brushnum;
}

static brushnum_int entity_add_brush_common_alpha_size_8 (entityx_s *d_ent, vec3_t point3d, float f_alpha)
{
	Vector3DeclareCopyEx (brmins, point3d);
	Vector3DeclareCopyEx (brmaxs, point3d);

	for (int dimension = 0; dimension < 3; dimension ++) {
		brmins[dimension] -= 4;
		brmaxs[dimension] += 4;
	} // for

	// Make all zeros -- EVEN NEGATIVE ZERO -- into real zero
	if (f_alpha == 0) {
		f_alpha = 0; // Negative zero fix.
	}

	va_super (texturename, 128, "common/alpha_%1.2f", f_alpha);
	String_Edit_Replace (texturename, sizeof(texturename), ".", "_");
	int brushnum = map_add_brush_cubeoid_with_texture (d_ent, brmins, brmaxs, texturename);
	return brushnum;
}

// pincommonterrain and adjacents use this - returns brushnum
static brushnum_int entity_add_brush_common_alpha_at_8_at_origin_string (entityx_s *d_ent, ccs *origin_string, float f_alpha)
{
	vec3_t origin; Math_atov3 (origin_string, origin);

	int brushnum = entity_add_brush_common_alpha_size_8 (d_ent, origin, f_alpha);
	return brushnum;
}


qbool brush_add_alpha_brush_string (map_t *hm, int entnum, ccs *point3d_string, float alpha)
{
	vec3_t point3d; Math_atov3 (point3d_string, point3d);
	return brush_add_alpha_brush (hm, entnum, point3d, alpha);
}

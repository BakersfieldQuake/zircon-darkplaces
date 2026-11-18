// csg_local_brush_face.h

RELATED_ (for_each_brush_2 for_each_brush_face)

// for_each_brush_face_2 (facenum, d_face, hm, entnum, brushnum)
// for_each_brush_face_2 (facenum, face, hm, entnum, brushnum)
// for_each_end // face
#define for_each_brush_face_2(YOUR_FACENUM_N, D_FACE, HM, ENTNUM, BRUSHNUM)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < FACE_COUNT(HM, ENTNUM, BRUSHNUM); YOUR_FACENUM_N ++) { \
		brushrow_s *D_FACE = FACE_GET(HM, ENTNUM, BRUSHNUM, YOUR_FACENUM_N); \
		if (D_FACE->is_considered_deleted) continue; // Ender

#define for_each_brush_face_even_deleted_2(YOUR_FACENUM_N, D_FACE, HM, ENTNUM, BRUSHNUM)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < FACE_COUNT(HM, ENTNUM, BRUSHNUM); YOUR_FACENUM_N ++) { \
		brushrow_s *D_FACE = FACE_GET(HM, ENTNUM, BRUSHNUM, YOUR_FACENUM_N);

// for_each_face_with_texture_2 (facenum, d_face, hm, entnum, brushnum, texturename)
// for_each_end // face
#define for_each_face_with_texture_2(YOUR_FACENUM_N, D_FACE, HM, ENTNUM, BRUSHNUM, TEXTURENAME)\
	for (int YOUR_FACENUM_N = 0; YOUR_FACENUM_N < FACE_COUNT(HM, ENTNUM, BRUSHNUM); YOUR_FACENUM_N ++) { \
		brushrow_s *D_FACE = FACE_GET(HM, ENTNUM, BRUSHNUM, YOUR_FACENUM_N); \
		if (D_FACE->is_considered_deleted) continue; \
		if (String_Match(D_FACE->pbrtexture, TEXTURENAME) == false) continue; // Ender

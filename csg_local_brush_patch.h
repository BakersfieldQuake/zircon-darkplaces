// csg_local_brush_patch.h



// for_each_brush_const_2 (brushnum, d_brush, hm, entnum)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_const_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		const brush_s *D_BRUSH = BRUSH_GET (HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue; /*PATCH, SKIP*/ \
		if (D_BRUSH->brushrowlist.count == 0) continue;


// for_each_brush_or_patch_2 (brushnum, brush, hm, entnum)
// for_each_brush_or_patch_2 (brushnum, d_brush, hm, entnum)
// for_each_end // for_each_brush_or_patch
//brush_s *D_BRUSH = ENT_GET(HM, ENTNUM)->brushlist.brush[YOUR_BRUSHNUM_N];
#define for_each_brush_or_patch_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue;


#define for_each_brush_or_patch_const_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		const brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue;


// for_each_patch_row (rownum, d_brush, d_patchrow)
// for_each_end // patch_row
#define for_each_patch_row_2(YOUR_PATCHROW, D_PATCHROW, HM, ENTNUM, D_BRUSH)\
	for (int YOUR_PATCHROW = 0; YOUR_PATCHROW < D_BRUSH->patchrowlist.count; YOUR_PATCHROW ++) { \
		patchrow_s *D_PATCHROW = PATCHROW_GET(HM, ENTNUM, BRUSHNUM, YOUR_PATCHROW); // Ender

// for_each_brush_2 (brushnum, brush, hm, entnum)
// for_each_brush_2 (brushnum, d_brush, hm, entnum)
// for_each_end // for_each_brush_2
#define for_each_brush_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue;

// for_each_brush_original_2 (brushnum, d_brush, hm, entnum)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_original_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_ORIGINAL_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue;

// for_each_brush_original_2 (brushnum, d_brush, hm, entnum)
// for_each_end // for_each_brush_or_patch
#define for_each_brush_added_2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = BRUSH_ORIGINAL_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch) continue;

// for_each_patch2 (brushnum, r_patch, hm, entnum)
// for_each_end // for_each_patch2
#define for_each_patch2(YOUR_BRUSHNUM_N, D_BRUSH, HM, ENTNUM)\
	for (int YOUR_BRUSHNUM_N = 0; YOUR_BRUSHNUM_N < BRUSH_COUNT(HM, ENTNUM); YOUR_BRUSHNUM_N ++) { \
		brush_s *D_BRUSH = BRUSH_GET(HM, ENTNUM, YOUR_BRUSHNUM_N); \
		if (D_BRUSH->is_considered_deleted) continue; \
		if (D_BRUSH->is_a_patch == false) continue;


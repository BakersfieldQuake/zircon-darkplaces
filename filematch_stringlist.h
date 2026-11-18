// filematch_stringlist.h

typedef struct stringlist_s {
#if 0
	mempool_t *list_mempool;
#endif
	/// maxstrings changes as needed, causing reallocation of strings[] array
	int maxstrings;
	int numstrings;
	char **strings;

} stringlist_t;

// Baker: floatlist_s
// To store floats with intention of list to be null terminated
// at all times IF count > 0
// So ..
// while (*floats++)
// .. would work

typedef struct _floatlist_s {
	float	*floats;
	int		maxsize;
	int		count;
} floatlist_s;

typedef struct _voidlist_s {
	const void	**vloats;
	int			maxsize;
	int			count;
} voidlist_s;

void voids_add1 (voidlist_s *vlist, const void *p);
void voids_freecontents (voidlist_s *vlist);


void floats_add1 (floatlist_s *flist, float p);
void floats_add2 (floatlist_s *flist, float px, float py);
void floats_add3 (floatlist_s *flist, float px, float py, float pz);

#define floats_add_vector(plist, v) floats_add3(plist,v[0],v[1],v[2])

void floats_print (floatlist_s *flist, void (*myprintf)(const char *, ...));
void floats_dump (floatlist_s *flist);

void floats_sort (floatlist_s *list, qbool uniq);
#define floats_sort_no_unique(plist) floats_sort (plist, fs_make_unique_false)
#define floats_sort_unique(plist) floats_sort (plist, fs_make_unique_true)


#define floats_vector_read_at(vsetme,flist,idx) \
	vsetme[0] = flist->floats[idx + 0]; \
	vsetme[1] = flist->floats[idx + 1]; \
	vsetme[2] = flist->floats[idx + 2] // Ender


char *floats_join_delim12_delim_zalloc (floatlist_s *flist, int number_per_vert, ccs *s_delimiter12, ccs *s_delimiter_elements, int shall_quote);


char *floats_join_delim12_delim_mag_zalloc (floatlist_s *flist, int number_per_vert, ccs *s_delimiter12, ccs *s_delimiter_elements, int shall_quote, mag_s *magni_opt);

// Returns 0 if the parse of all elements went as expected
int floats_append_parse_space_comma_num_elements_ignored (floatlist_s *flist, ccs *s_value, int number_per_vert, ccs *s_delimiter12, ccs *s_delimiter_elements);

void floats_freecontents (floatlist_s *flist);

typedef struct _int32list_s {
	int		*ints;
	int		maxsize;
	int		count;
} int32list_s;

void int32s_add1 (int32list_s *ilist, int p);
int int32s_find_index (int32list_s *ilist, int find_int);
void int32s_add2 (int32list_s *ilist, int px, int py);
void int32s_add3 (int32list_s *ilist, int px, int py, int pz);
qbool int32s_add1_unique_didadd (int32list_s *ilist, int p);

void int32s_sort (int32list_s *list, qbool uniq);
void int32s_print (int32list_s *ilist, void (*myprintf)(const char *, ...));
void int32s_dump (int32list_s *ilist);
void int32s_command_scale (int32list_s *ilist, int iscale);

#define int32s_resize int32s_count_set
RELATED_ (stringlist_resize)
void int32s_count_set (int32list_s *ilist, int newcount); // increases only.

int int32s_append_split_dequote (int32list_s *ilist, ccs *s_value, ccs *s_delimiter_elements);
char *int32s_join_delim_zalloc (int32list_s *ilist, ccs *s_delimiter_elements, int shall_quote);

char *int32s_join_delim_mag_zalloc (int magni_factor, int32list_s *ilist, ccs *s_delimiter_elements, int shall_quote); // SIZEOIC
char *int32s_join_delim_mag_reverse_zalloc (int magni_factor, int32list_s *ilist, ccs *s_delimiter_elements, int shall_quote);

void int32s_freecontents (int32list_s *ilist);

// Baker: stringlist is zonemempool
void stringlistinit(stringlist_t *list);
void stringlistfreecontents(stringlist_t *list);
#define stringlist_add stringlistappend // April 10 2026
#define stringlist_addf stringlistappendf // April 10 2026
void stringlistappend(stringlist_t *list, const char *text);
void stringlist_resize (stringlist_t *plist, int count); // Fills will "" to index
void stringlistreplaceindex(stringlist_t *plist, int idx, ccs *text);
void stringlistreplaceindexf(stringlist_t *plist, int idx, ccs *fmt, ...) DP_FUNC_PRINTF(3);

void stringlistappend_len (stringlist_t *list, const char *text, int text_len);
int stringlistappend_split_delimiter_linenumbers (stringlist_t *plist, ccs *s, ccs *delimiter);

#define stringlist_last(LISTX) LISTX.strings[LISTX.numstrings - 1]
#define stringlist_lastp(PLISTX) PLISTX->strings[PLISTX->numstrings - 1]


int stringlistappendfilelines_did_load (stringlist_t *plist, ccs *file, qbool is_base1, int is_number_column);

const char **stringlist_nullterm_add (stringlist_t *list);

qbool stringlistappend_unique_didAdd (stringlist_t *plist, ccs *s);
qbool stringlistappendf_unique_didAdd (stringlist_t *plist, ccs *fmt, ...) DP_FUNC_PRINTF(2);
qbool stringlistappend_unique_evens_didAdd (stringlist_t *plist, ccs *text); // Does evens only

void stringlistappendf (stringlist_t *list, ccs *fmt, ...)  DP_FUNC_PRINTF(2);
void stringlistappendf2 (stringlist_t *list, ccs *s, ccs *fmt, ...) DP_FUNC_PRINTF(3);

void stringlistappend2 (stringlist_t *list, ccs *s, ccs *s2);
void stringlistappend3 (stringlist_t *list, ccs *s, ccs *s2, ccs *s3);
void stringlistappend4 (stringlist_t *list, ccs *s, ccs *s2, ccs *s3, ccs *s4);
void stringlistappend5 (stringlist_t *list, ccs *s, ccs *s2, ccs *s3, ccs *s4, ccs *s5);


// Moved to darkplaces.h so fssearch_t is defined
void stringlistappendlist (stringlist_t *plist, const stringlist_t *add_these); // Baker: Add a list to a list
void stringlistappendlistunique (stringlist_t *plist, const stringlist_t *add_these);

//void stringlistprint (stringlist_t *list, const char *title_optional, void (*myprintf)(const char *, ...) );
void stringlistprint_with_title (stringlist_t *list, void (*myprintf)(ccs *, ...), ccs *fmt, ...) DP_FUNC_PRINTF(3);

#define stringlist_set_index stringlist_replace_at_index
void stringlist_replace_at_index (stringlist_t *list, int idx, const char *text);

RELATED_ (stringlistsortcolumns_alpha stringlistsortcolumns_nocarriages)
void stringlistsort(stringlist_t *list, qbool uniq);
#define stringlistsortascending stringlistsort_no_unique

void stringlistsortreverse(stringlist_t *list, qbool uniq);
#define stringlistsort_no_unique(plist) stringlistsort (plist, fs_make_unique_false)
#define stringlistsort_unique(plist) stringlistsort (plist, fs_make_unique_true)
void stringlist_condump_with_linenums (stringlist_t *plist);
void stringlist_condump_with_linenums_unquoted (stringlist_t *plist);
void stringlist_condump_with_linenums_band (stringlist_t *plist, int numcolumns);
void stringlist_condump_raw (stringlist_t *plist);
void stringlistsort_custom(stringlist_t *list, qbool uniq, int myfunc(const void *, const void *) );

int stringlist_quote_or_comma_decode_count (stringlist_t *plist);
int stringlist_quote_or_comma_encode_count (stringlist_t *plist);

// Returns NULL if no commas or quotes.
char *stringlist_to_base64_comma_delim_char12_encode_zalloc (stringlist_t *plist, qbool shall_force);
int stringlist_append_from_base64_comma_delim_char12_decode (stringlist_t *plist, ccs *sbase64);

// Returns first idx that does not match (-1 means no differences).  If lists different size returns 0.
int stringlist_compare (stringlist_t *plist1, stringlist_t *plist2);

void stringlist_bytedump (stringlist_t *plist, const byte *src, size_t src_sizeof);
void stringlist_stringdump (stringlist_t *plist, ccs *src);

void stringlistappend_blob (stringlist_t *list, const byte *blob, size_t blobsize); // Put binary data in a stringlist

void stringlistsort_substring(stringlist_t *list, qbool uniq, int startpos, int slength); // Substring sort

void stringlist_append_listdirectory(stringlist_t *list, ccs *basepath, ccs *filepattern);

/*JOIN*/
void stringlist_join_buf (stringlist_t *p_stringlist, ccs *s_delimiter, char *buf, size_t buflen); // JOIN

// Free with: baker_string_s *bs = NULL; if (bs) BakerString_Destroy_And_Null_It (&bs);
baker_string_s *stringlist_join_lines_bkalloc (stringlist_t *plist);
baker_string_s *stringlist_join_bkalloc (stringlist_t *p_stringlist, ccs *s_delimiter);

char *stringlist_join_lines_zalloc (stringlist_t *plist);
char *stringlist_join_spaced_zalloc (stringlist_t *plist);
char *stringlist_join_delim_zalloc (stringlist_t *plist, ccs *s_delimiter, int shall_quote);

/*SPLIT*/
void stringlistappend_split_len (stringlist_t *plist, ccs *text_in, int text_strlen, ccs *s_delimiter);
void stringlist_append_split (stringlist_t *plist, ccs *text_in, ccs *s_delimiter);

int stringlistappend_from_dir_pattern (stringlist_t *p_stringlist, const char *s_optional_dir_no_slash, const char *s_optional_dot_extension, int wants_strip_extension);
int stringlistappend_from_pattern (stringlist_t *p_stringlist, ccs *s_pattern);
int stringlistappend_dir_matches (stringlist_t *p_stringlist, ccs *s_prefix);
struct fssearch_s;
void stringlistappendfssearch (stringlist_t *plist, fssearch_t *t); // t == NULL is ok and handled
void stringlistappendfssearch_modulo (stringlist_t *plist, fssearch_t *t, int modulation); // 2 = 50%, 4 = 25%
void stringlistappend_search_pattern (stringlist_t *plist, const char *s_pattern);

void stringlistappend_dirs_recursive (stringlist_t *plist, ccs *mydir);
void stringlistappend_dirs_not_recursive (stringlist_t *plist, ccs *mydir);

typedef enum {
	sort_method_alpha_0			= 0,	// %s
	sort_method_float_09_2f_1	= 1,	// %09.2f	float format
	sort_method_int_09d_2		= 2,	// %9d		integer format
} sort_method_e;

// Returns new selected row for old selected row
int stringlistsortcolumns_nocarriages(stringlist_t *plist, int numcolumns, int sort_column, sort_method_e sort_method, int is_descending, int selected_row);

#define stringlistsortcolumns_alpha(plist, numcolumns, sortcol) \
	stringlistsortcolumns_nocarriages(plist, numcolumns, sortcol, sort_method_alpha_0, /*descend?*/ false, /*selrow*/ 0) // Ender

#define stringlistsortcolumns_integer(plist, numcolumns, sortcol) \
	stringlistsortcolumns_nocarriages(plist, numcolumns, sortcol, sort_method_int_09d_2, /*descend?*/ false, /*selrow*/ 0) // Ender



qbool stringlist_write_file (stringlist_t *plist, ccs *filename);

/*LINE WORD -> SPLIT*/

// Now requires something to help with line numbers.
void stringlistappend_tokenize_qcquotes (stringlist_t *plist, int32list_s *linelist, const char *text);

/*LINE SPLIT*/
void stringlistappend_textlines_cr_scrub (stringlist_t *plist, const char *text_in);
#define stringlistappend_split_lines_cr_scrub(plist, s) stringlistappend_textlines_cr_scrub(plist, s)

void stringlistappend_textlines_len_cr_scrub (stringlist_t *plist, const char *text_in, int text_strlen);


int stringlistappend_csv_nl_tab (stringlist_t *plist, const char *s);

void stringlist_from_delim (stringlist_t *p_stringlist, const char *s_space_delimited);
int stringlist_find_index (stringlist_t *p_stringlist, ccs *s_find_this);
#define stringlist_has_string(plist,s) (stringlist_find_index(plist,s) != not_found_neg1)

int stringlist_find_index_columns (stringlist_t *p_stringlist, ccs *s_find_this, int numcolumns);
int stringlist_find_row_with_column_string_match (stringlist_t *plist, ccs *s_find_this, int numcolumns, int columnwanted);

qbool stringlist_string_endswithany (ccs *s, stringlist_t *plist);

// Feb 18 2025
int stringlist_find_column_number (stringlist_t *p_stringlist, ccs *s_find_this, int numcolumns);
int stringlist_find_row_number (stringlist_t *p_stringlist, ccs *s_find_this, int numcolumns);


void stringlist_condump_ilist (stringlist_t *plist, int32list_s *ilist);
void stringlistclipboardset (stringlist_t *plist);
void stringlistclipboardset_columns_tabbed (stringlist_t *plist, int numcolumns);
void stringlistclipboardset_columns_firstlastrow (stringlist_t *plist, int numcolumns, int firstrow, int lastrow);
void stringlistclipboardset_row_columns_tabbed (stringlist_t *plist, int row, int numcolumns);
void int32s_countstrings (int32list_s *ilist, stringlist_t *plist, stringlist_t *plist_indexor);
void stringlist_condump_autocount_uniques (stringlist_t *plist);
ccs *stringlist_find_most_common_index_ignoring_this (stringlist_t *plist, ccs *ignoring_this);

RELATED_ (stringlist_append_functions_unique)
//void stringlist_append_entity_textures (stringlist_t *plist, entityx_s *r_ent);
void stringlistappend_face_vertex_unique (stringlist_t *plist, const brushrow_s *face);


void stringlistappend_list (stringlist_t *plist, stringlist_t *plist2);


// for_each_stringlist_string (n, list_textures, sxy)
// for_each_stringlist_string (sidx, list_track_ids, this_track_id)
// for_each_end // Each string
RELATED_ (for_each_stringlista_string)
#define for_each_stringlist_string(NN,LIST,SXY)\
	for (int NN = 0; NN < LIST.numstrings; NN ++) { \
		ccs	*SXY = LIST.strings[NN]; // Ender

// for_each_int32list (n, list_inums, myint)
// for_each_end // each int32
#define for_each_int32list(NN,LIST,SXY)\
	for (int NN = 0; NN < LIST.count; NN ++) { \
		int SXY = LIST.ints[NN]; // Ender

// for_each_int32list (n, list_inums, myint)
// for_each_end // each int32
#define for_each_int32listp(NN,PLIST,SXY)\
	for (int NN = 0; NN < PLIST->count; NN ++) { \
		int SXY = PLIST->ints[NN]; // Ender

// for_each_floatlist (n, list_inums, row)
// for_each_end // each float

#define for_each_floatlist(NN,LIST,SXY)\
	for (int NN = 0; NN < LIST.count; NN ++) { \
		float SXY = LIST.floats[NN]; // Ender

// for_each_floatlist_by_step (idx, sigmoid_float_list, 12)
// for_each_end
#define for_each_floatlist_by_step(NN, LIST, STEP)\
	for (int NN = 0; NN < LIST.count; NN += STEP) { // Ender

// for_each_floatlista_by_step (fidx, flist, BBOX_6)
// for_each_end
#define for_each_floatlista_by_step(NN, PLIST, STEP)\
	for (int NN = 0; NN < PLIST->count; NN += STEP) { // Ender


#define for_each_stringlist2_string(NN,LIST,SXY1,SXY2)\
	for (int NN = 0; NN < LIST.numstrings; NN += 2) { \
		ccs	*SXY1 = LIST.strings[NN + 0]; \
		ccs	*SXY2 = LIST.strings[NN + 1]; // Ender

#define for_each_stringlist3_string(NN, LIST, SXY1, SXY2, SXY3)\
	for (int NN = 0; NN < LIST.numstrings; NN += 3) { \
		ccs	*SXY1 = LIST.strings[NN + 0]; \
		ccs	*SXY2 = LIST.strings[NN + 1]; \
		ccs	*SXY3 = LIST.strings[NN + 2]; // Ender

#define for_each_stringlist4_string(NN, LIST, SXY1, SXY2, SXY3, SXY4)\
	for (int NN = 0; NN < LIST.numstrings; NN += 4) { \
		ccs	*SXY1 = LIST.strings[NN + 0]; \
		ccs	*SXY2 = LIST.strings[NN + 1]; \
		ccs	*SXY3 = LIST.strings[NN + 2]; \
		ccs	*SXY4 = LIST.strings[NN + 3]; // Ender

#define for_each_search(NN,TSEARCH,SXY)\
	for (int NN = 0; TSEARCH && NN < TSEARCH->numfilenames; NN ++) { \
		char *SXY = TSEARCH->filenames[NN]; // Ender

#define for_each_stringlista_string(NN,PLIST,SXY)\
	for (int NN = 0; NN < PLIST->numstrings; NN ++) { \
		ccs	*SXY = PLIST->strings[NN]; // Ender



#define for_each_end }
#define for_each_end_2 }}
#define for_each_end_3 }}}

#define WORDNUM(LIST, IDX) (LIST.numstrings > (IDX) ? LIST.strings[IDX] : NULL) // MAZEGEN

//
// FOR_EACH
//


#define for_each_edict_in_prog(edict_num,ed) \
	for (int edict_num = 0; edict_num < prog->num_edicts; edict_num ++) { \
		prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num); \
		if (ed->free) continue; // Ender




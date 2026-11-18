// filematch.h

RELATED_ (stringlistsortcolumns_alpha)

/*
Copyright (C) 2006-2021 DarkPlaces contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef FILEMATCH_H
#define FILEMATCH_H

#include "qtypes.h"

typedef struct {
	int magnifier;	// Q: Is vid_magnification an integer always: YES
	int offset_xx;
	int offset_yy;
} mag_s;

#include "filematch_stringlist.h"
#include "csg_global.h"


int matchpattern(const char *in, const char *pattern, int caseinsensitive);
int matchpattern_with_separator(const char *in, const char *pattern, int caseinsensitive, const char *separators, qbool wildcard_least_one);

// bakerarray_s

typedef struct {
	int		batchsize;					// Defaults to BAKER_ARRAY_BATCHSIZE_DEFAULT_128

	int		numitems;
	int		maxitems;

	void	**pointers_array;			// union?
	size_t	datasize;
} bakerarray_s;

#define DEF_COMPAT_ARRAY_STRUCT(MYARRAYDATATYPE_S,MYPTRTYPE) \
	typedef struct _ ## MYARRAYDATATYPE_S {											\
		int					batchsize;		/* If not specified, defaults 128 */	\
		int					numitems;												\
		int					maxitems;												\
		MYPTRTYPE			**pointers_array;			/* */						\
		size_t				datasize;												\
	} MYARRAYDATATYPE_S // ENDER

// Baker: This is a struct declaration ... an array of object_s
// Example: DEF_COMPAT_ARRAY_STRUCT (oject_array_st, struct _oject_s);
// Creates an array of _oject_s and that array type is oject_array_st
// See::#define oject_array_s	struct _oject_array_st

//int idx = BakerArray_NewIndex(k->fctrl->frm.drawn_list_a);
//NPA__ Baker_Array_Erase (f->frm.drawn_list_a);

#define BAKER_ARRAY_BATCHSIZE_DEFAULT_128	128 // Fool

int bakerarray_ptr_erase(bakerarray_s *a); // f->frm.drawn_list_a uses this to clear reference pointers
int bakerarray_ptr_new_item_index(bakerarray_s *a);

#define BakerArray_Erase(ba) \
	bakerarray_ptr_erase ((bakerarray_s *) ba)

// See: int idx = BakerArray_NewIndex(f->frm.controls_a);
#define BakerArray_NewIndex(ba) \
	bakerarray_ptr_new_item_index((bakerarray_s *)ba);

#define BakerArray_Item_At(ray, n) (ray).pointers_array[n]

// Opposite of Owned is reference array where we do not own the contexts (like frm.drawn_list_a)
#define BakerArray_Owned_Clear(bastruct) \
	/* mazebbox_array_s - BakerArray clear */ \
	{ \
		for (int idx = 0; idx < bastruct.numitems; idx ++) { \
			/*mazebbox_s*/ void *boxo = bastruct.pointers_array[idx]; \
			Mem_FreeNull_ (boxo); \
		} \
		Mem_FreeNull_ (bastruct.pointers_array); \
		size_t clearamt = sizeof(bastruct); \
		memset (&bastruct, 0, clearamt); /* sizeof (bastruct) */ \
	} // Ender

#define BakerArray_New_Empty_Item(PSTRUCTTYPE_STAR,VARNAME,BASTRUCT) \
	PSTRUCTTYPE_STAR VARNAME; /*mazebbox_s *newbox*/ \
	{ \
		int newidx9 = BakerArray_NewIndex (&(BASTRUCT)); \
		size_t szwanted2 = sizeof(**BASTRUCT.pointers_array); \
		VARNAME = (PSTRUCTTYPE_STAR)Mem_Alloc (zonemempool, szwanted2); \
		BASTRUCT.pointers_array[newidx9] = VARNAME; \
	} // Ender


#include "equat.h"

#define space_split1(splitme,s0)				_arch1_string_space_split_isok(splitme,s0,sizeof(s0))
#define space_split2(splitme,s0,s1)				_arch2_string_space_split_isok(splitme,s0,sizeof(s0),s1,sizeof(s1))

#define space_split5(splitme,s0,s1,s2,s3,s4)	_arch5_string_space_split_isok(splitme,s0,sizeof(s0),s1,sizeof(s1),s2,sizeof(s2),s3,sizeof(s3),s4,sizeof(s4))
#define space_split6(splitme,s0,s1,s2,s3,s4,s5)	_arch6_string_space_split_isok(splitme,s0,sizeof(s0),s1,sizeof(s1),s2,sizeof(s2),s3,sizeof(s3),s4,sizeof(s4),s5,sizeof(s5))

qbool _arch1_string_space_split_isok (ccs *split_this, char *dst0, size_t dst0_sizeof);
qbool _arch2_string_space_split_isok (ccs *split_this, char *dst0, size_t dst0_sizeof, char *dst1, size_t dst1_sizeof);
qbool _arch5_string_space_split_isok (ccs *split_this, char *dst0, size_t dst0_sizeof, char *dst1, size_t dst1_sizeof, char *dst2, size_t dst2_sizeof, char *dst3, size_t dst3_sizeof, char *dst4, size_t dst4_sizeof);
qbool _arch6_string_space_split_isok (ccs *split_this, char *dst0, size_t dst0_sizeof, char *dst1, size_t dst1_sizeof, char *dst2, size_t dst2_sizeof, char *dst3, size_t dst3_sizeof, char *dst4, size_t dst4_sizeof, char *dst5, size_t dst5_sizeof);

qbool _arch2_string_delimiter_split_isok (ccs *split_this, ccs *delimiter, char *dst0, size_t dst0_sizeof, char *dst1, size_t dst1_sizeof);
#define delimiter_split2(splitme,delim,s0,s1)	_arch2_string_delimiter_split_isok(splitme,delim,s0,sizeof(s0),s1,sizeof(s1))

// This one strictly splits on 1 space.  Everything before goes in #1 and everything after goes to #2
#define space_split2_only(splitme,s0,s1)		_arch2_string_space_split2(splitme,s0,sizeof(s0),s1,sizeof(s1))
int _arch2_string_space_split2 (ccs *split_this, char *dst0, size_t dst0_sizeof, char *dst1, size_t dst1_sizeof);


#endif // FILEMATCH_H

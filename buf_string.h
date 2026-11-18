// buf_string.h

RELATED_ (VM_bufstr_add)

RELATED_ (stringbuffer_getlist_by_index /*MAJOR*/)
char *stringbuffer_to_zippedbase64_newline_delim_ZAlloc(prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer)
{
	baker_string_s *bsa = BakerString_Create_Malloc (""); 
	for (int idx = 0; idx < stringbuffer->num_strings; idx ++) {
		ccs *sxy = stringbuffer->strings[idx];
		if (idx > 0)
			BakerString_CatC (bsa, NEWLINE);
		BakerString_CatC (bsa, sxy);
	}

	char *s_za = base64zip_zalloc(bsa->string);
	BakerString_Destroy_And_Null_It (&bsa);
	return s_za;
}


int stringbuffer_append (prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer, ccs *mystring)
{
	int strindex = stringbuffer->num_strings;

	BufStr_Expand(prog, stringbuffer, strindex);
	stringbuffer->num_strings = max(stringbuffer->num_strings, strindex + 1); // Baker: Larger of these, always will be 2nd one

	size_t alloclen = strlen(mystring) + 1; // Baker: This works even for UTF-8
	stringbuffer->strings[strindex] = (char *)Mem_Alloc(prog->progs_mempool, alloclen);
	memcpy(stringbuffer->strings[strindex], mystring, alloclen);

	return strindex;
}

int stringbuffer_append_not_zero_length (prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer, ccs *mystring)
{
	int strindex = stringbuffer->num_strings;

	BufStr_Expand(prog, stringbuffer, strindex);
	stringbuffer->num_strings = max(stringbuffer->num_strings, strindex + 1); // Baker: Larger of these, always will be 2nd one

	if (mystring && mystring[0]) {
		size_t alloclen = strlen(mystring) + 1; // Baker: This works even for UTF-8
		stringbuffer->strings[strindex] = (char *)Mem_Alloc(prog->progs_mempool, alloclen);
		memcpy(stringbuffer->strings[strindex], mystring, alloclen);
	}
	//ccs *str = stringbuffer->strings[strindex];
	return strindex;
}

void stringbuffer_appendf (prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	stringbuffer_append (prog, stringbuffer, text);
	VA_EXPAND_ALLOC_FREE (text);
}

void stringbuffer_append_stringlist_not_zero_length (stringlist_t *plist, prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer)
{
	for (int sidx = 0; sidx < plist->numstrings; sidx ++) {
		ccs *sxy = plist->strings[sidx];
		stringbuffer_append_not_zero_length (prog, stringbuffer, sxy);
	} // sidx
}

void stringbuffer_append_stringlist (stringlist_t *plist, prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer)
{
	for (int sidx = 0; sidx < plist->numstrings; sidx ++) {
		ccs *sxy = plist->strings[sidx];
		stringbuffer_append (prog, stringbuffer, sxy);
	} // sidx
}

void stringbuffernum_append_stringlist (stringlist_t *plist, prvm_prog_t *prog, int bufnum)
{
	prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, bufnum);
	stringbuffer_append_stringlist (plist, prog, stringbuffer);
}

RELATED_ (VM_buf_create)

// Baker: The buffernum reply is important
prvm_stringbuffer_t *stringbuffer_create_neverzero (prvm_prog_t *prog, int *pbufnum)
{
	// NEVER USE STRINGBUFFER 0.
	prvm_stringbuffer_t *stringbuffer = NULL;
	int bufnum;

	// Baker: Do not allow string buffer zero because we cannot distinguish 0 from an uninitialized variable.
hit_again_never_use_bufnum_zero:

	stringbuffer = (prvm_stringbuffer_t *) Mem_ExpandableArray_AllocRecord(&prog->stringbuffersarray);
	for (bufnum = 0;stringbuffer != Mem_ExpandableArray_RecordAtIndex(&prog->stringbuffersarray, bufnum);bufnum ++);
	stringbuffer->origin = PRVM_AllocationOrigin(prog);
	stringbuffer->flags |= STRINGBUFFER_SAVED_1; // April 14 2026

	if (bufnum == 0)
		goto hit_again_never_use_bufnum_zero;

	(*pbufnum) = bufnum;
	return stringbuffer;
}


RELATED_ (lw_csqc_stringlist_12 x= DevListViewOnChange)

void stringlist_append_from_stringbuffer(stringlist_t *plist, prvm_prog_t *prog, prvm_stringbuffer_t *stringbuffer)
{
	for (int strindex = 0; strindex < stringbuffer->num_strings; strindex ++) {
		ccs *sxy = stringbuffer->strings[strindex];

		// Baker: If NULL, append an empty string.
		stringlistappend (plist, sxy ? sxy : "");
	} // strindex
}

// Formerly: BufStringList_To_StringList (stringlist_t *plist, int bufnum)
void stringlist_append_from_stringbuffernum(stringlist_t *plist, prvm_prog_t *prog, int bufnum)
{
	prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, bufnum);
	stringlist_append_from_stringbuffer (plist, prog, stringbuffer);
}



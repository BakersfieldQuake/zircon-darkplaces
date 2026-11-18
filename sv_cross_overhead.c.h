// sv_cross_overhead.c.h



qbool Set_A_Global_Print_Errors (ccs *s_progname, ccs *s_fieldname, ccs *s_value);

RELATED_ (VM_bufstr_add)



// Erase everything
static void sCross_Clear (void)
{
	// Erase everything
	// 1. Erase all the string lists.
	// 2. Erase the array of strings
	stringlistfreecontents (&svs.server_cross_globals_list);

	// Work every single one.
	int stringlist_count = ARRAY_COUNT(svs.server_cross_globals_strlists);

	for (int stringlist_num = 0; stringlist_num < stringlist_count; stringlist_num ++) {
		//stringlist_t *plist = &svs.server_cross_globals_strlists[stringlist_num];
		stringlistfreecontents(&svs.server_cross_globals_strlists[stringlist_num]);
	} // each plist

	svs.num_server_cross_globals_strlists = 0;
}


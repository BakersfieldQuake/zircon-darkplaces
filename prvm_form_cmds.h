// prvm_form_cmds.h

// float map_load (string filename) = #150;

//#include "prvm_form_cmds.h"

// Returns PLUS1(index)
RELATED_ (VM_search_begin VM_callfunction VM_isfunction VM_getentityfieldstring)
RELATED_ (PRVM_Crash)

// ZIRCON_FORMS_V1
void *	CSQC_Form_Create_From_String_VoidP (ccs *s, int handle, func_t my_event_callback, func_t my_close_callback, ccs *sMessage, void *fModalBoss);
void	CSQC_Form_Show (void *_f);
int		CSQC_Form_Command (void *_f, ccs *s_method_name);
int		CSQC_Object_Method (void *_f, ccs *s_object_name, ccs *s_method_name, ccs *s_value);
qbool	CSQC_Object_Property_Get_Ok (void *_f, ccs *s_object_name, ccs *s_property_name, int listindex, ccs *s_key_name, char *valuebuf, size_t valuebuf_size);
qbool	CSQC_Object_Property_Set_Ok (void *_f, ccs *s_object_name, ccs *s_property_name, int listindex, ccs *s_key_name, ccs *s_value);
qbool	CSQC_Object_Property_Set_Float_Ok (void *_f, ccs *s_object_name, ccs *s_property_name, int listindex, ccs *s_key_name, float fval);
//void	CSQC_Form_Prepare_Close (void *_f, void **pfparent, func_t *pClose);
void *	CSQC_Form_Destroy_VoidP (void *_f);
void	CSQC_Form_Queue_Close (void *_f, ccs *sMessage); //

// Call Back:
// void Form1_Event (string controlname, string eventname, string text)

// float h = form_load ("engine/form2.txt"); // Returns > 0 on success, < 0 on failure.
RELATED_ (VM_map_close VM_map_load )
RELATED_ (oject_s M_Menu_DevInfo_f)

// float Form_Load (string filename, void(float h, string ControlName, string EventName, string Text) event_callback, void(string Message) close_callback, optional string message) = #180; // Returns handle.  Handle shall be greater than ZERO.

RELATED_ (VM_Inventory_Reload)
RELATED_ (VM_CL_frameforname VM_CL_modelframecount)
//float(float mdlidx) modelframecount = #0:modelframecount;
void VM_CL_modelframecount (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(1, 1, VM_modelframecount); // 0: model index
	int modelindex = (int)PRVM_G_FLOAT(OFS_PARM0);
	model_t *model = CL_GetModelByIndex(modelindex);
	//const char *name = PRVM_G_STRING(OFS_PARM1);
	//int i;
	PRVM_G_FLOAT(OFS_RETURN) = -1;
	if (!model)
		return;
	if (!model->animscenes) {
		PRVM_G_FLOAT(OFS_RETURN) = 1;
		return;
	}
	PRVM_G_FLOAT(OFS_RETURN) = (float)model->numframes;
}



#ifdef CONFIG_MENU
void VM_CL_Form_Load (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 5, VM_CL_Form_Load); // 0: filename  1: event handler 2 (Optional message)

	ccs *filename = PRVM_G_STRING(OFS_PARM0);  VM_CheckEmptyString(prog, filename); // Baker: Host error if first character is whitespace or NULL char 0
	func_t my_event_callback = prog->argc >= 2 ? PRVM_G_INT(OFS_PARM1) : 0;
	func_t my_close_callback = prog->argc >= 3 ? PRVM_G_INT(OFS_PARM2) : 0;
	ccs *sMessage = prog->argc >= 4 ? PRVM_G_STRING(OFS_PARM3) : "";
	int hModalBossPlus1 = prog->argc >= 5 ? (int)PRVM_G_FLOAT(OFS_PARM4) : 0;
	void *fModalBoss = NULL;

	if (hModalBossPlus1) {
		int hModalBoss = UNPLUS1(hModalBossPlus1);
		if (in_range_beyond (0, hModalBoss, PRVM_MAX_OPEN_FORMS_8) == false) {
			VM_WarningLinef (prog, "%s: invalid modal handle %d used in %s", __func__, hModalBoss, prog->name);
			return;
		}

		if (prog->forms[hModalBoss] == NULL) {
			VM_WarningLinef (prog, "%s: no such modal handle %d in %s", __func__, hModalBoss, prog->name);
			return;
		}

		fModalBoss = prog->forms[hModalBoss];
	}

	int handle;
	for (handle = 0; handle < PRVM_MAX_OPEN_FORMS_8; handle ++)
		if (prog->forms[handle] == 0)
			break;

	if (handle >= PRVM_MAX_OPEN_FORMS_8) {
		PRVM_G_FLOAT(OFS_RETURN) = -2; VM_WarningLinef (prog, "%s: ran out of form handles (%d) in %s", __func__, PRVM_MAX_OPEN_FORMS_8, prog->name);
		return;
	}

	// Load the file
	char *text_zalloc = FS_LoadFile_Quiet_Temp (filename);
	if (!text_zalloc) {
		PRVM_G_FLOAT(OFS_RETURN) = -3; VM_WarningLinef (prog, "%s: Couldn't load form text (%s) in %s", __func__, filename, prog->name);
		return;
	}

	// Baker: Form_Create_From_String will destroy itself on parse failure.
	void *form1 = CSQC_Form_Create_From_String_VoidP (text_zalloc, handle, my_event_callback, my_close_callback, sMessage, fModalBoss);

	if (!form1) {
		PRVM_G_FLOAT(OFS_RETURN) = -4; VM_WarningLinef (prog, "%s: form parse failed (%s) in %s", __func__, filename, prog->name);
		Mem_FreeNull_ (text_zalloc);
		return;
	}

	// Moved to form so events can fire because Form_Create ->
	//    --> QuakeC Events fire above and call functions!
	//prog->forms[handle] = form1;
	//prog->form_callbacks[handle] = 0;
	//prog->form_callbacks[handle] = mycallback;

	CSQC_Form_Show (form1);

	Mem_FreeNull_ (text_zalloc);

	PRVM_G_FLOAT(OFS_RETURN) = PLUS1(handle);

}

RELATED_ (VM_CL_GetEntity)

//string GetFormPropertyString (float h, string propertyname) = #182; // Returns string on success, NULL on failure.
//string GetPropertyString (float h, string objectname, string propertyname) = #182; // Returns string on success, NULL on failure.
//string GetListItem (float h, string objectname, string propertyname, float index) = #182;
//string GetFormListItem (float h, string objectname, float index) = #182;

void VM_CL_Form_Property_Get_String (prvm_prog_t *prog)
{
	char string16384[VM_STRINGTEMP_LENGTH_16384];

	// set the return value regardless of any possible errors
	PRVM_G_INT(OFS_RETURN) = OFS_NULL;

	VM_SAFEPARMCOUNTRANGE(2, 4, VM_CL_Form_Property_Get);

	HFORM_HANDLE_STRING_RETURN_BOILER;

	void *f = prog->forms[handle];

	// 0: handle  1:command (2) // form_property_get
	// 0: handle  1:objectname 2:command (3) // object_property_get
	// string s = GetListItem (hform, "Controls", n) = #182;
	ccs *sobject1 = NULL, *sproperty = NULL, *skeyname = NULL;
	int listindex = not_found_neg1;
	if (prog->argc == 2) {
		sproperty = PRVM_G_STRING(OFS_PARM1);		// GetFormPropertyString hform FocusStyle
	} else if (prog->argc == 3) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);		// GetPropertyString hform LabelBlocks Caption

		if (String_Match_Caseless(sobject1, "Key")) {
			// Special case ... GetFormListItem
			//Form_Command (h, "GetKey", "DotExt");
			sobject1 = NULL;
			sproperty = PRVM_G_STRING(OFS_PARM1);
			skeyname = PRVM_G_STRING(OFS_PARM2);
		}
		else if (String_Match_Caseless(sobject1, "Controls")) { // GetFormListItem (hform, "Controls", n) = #182;
			// Special case ... GetFormListItem
			sobject1 = NULL;
			sproperty = PRVM_G_STRING(OFS_PARM1);
			listindex = (int)PRVM_G_FLOAT(OFS_PARM2);
		} else {
			sproperty = PRVM_G_STRING(OFS_PARM2);
		}
	} else if (prog->argc == 4) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);		// GetListItem hform ListBlocks ImageList 5
		sproperty = PRVM_G_STRING(OFS_PARM2);
		listindex = (int)PRVM_G_FLOAT(OFS_PARM3);
	}

	qbool ret = CSQC_Object_Property_Get_Ok (f, sobject1, sproperty, listindex, skeyname, string16384, sizeof(string16384));

	if (!ret)
		return; // Failed.  Already printed a message, return NULL

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, string16384);
}


//string SetPropertyString (float h, string objectname, string propertyname, string value) = #181; // Returns string on success, NULL on failure.
//string SetListItem (float h, string objectname, string propertyname, float index, string value) = #181; // Returns string on success, NULL on failure.

// Form_QueueRefresh typically occurs -- will propogate on next draw
void VM_CL_Form_Property_Set_String (prvm_prog_t *prog)
{
	// set the return value regardless of any possible errors
	PRVM_G_INT(OFS_RETURN) = OFS_NULL;

	VM_SAFEPARMCOUNTRANGE(3, 5, VM_CL_Form_Property_Set);

	//HFORM_HANDLE_STRING_RETURN_BOILER;
	int _hform = (int)PRVM_G_FLOAT(OFS_PARM0);
	int handle = UNPLUS1(_hform);

	if (in_range_beyond (0, handle, PRVM_MAX_OPEN_FORMS_8) == false) { \
		VM_WarningLinef (prog, "%s: invalid handle %d used in %s", __func__, handle, prog->name);
		return;
	}

	if (prog->forms[handle] == NULL) {
		VM_WarningLinef (prog, "%s: no such handle %d in %s", __func__, handle, prog->name);
		return;
	}


	void *f = prog->forms[handle];

	// 0: handle  1:command (2) // form_property_set
	// 0: handle  1:objectname 2:command (3) // object_property_set
	ccs *sobject1 = NULL, *sproperty = NULL, *skeyname = NULL, *svalue = NULL;
	int listindex = not_found_neg1;
	if (prog->argc == 3) {
		sproperty = PRVM_G_STRING(OFS_PARM1);	// SetFormPropertyString hform ScrollBarColor #fc3
		svalue = PRVM_G_STRING(OFS_PARM2);
	} else if (prog->argc == 4) {
		// string Describe		= SetPropertyString (h, "Key", "Describe",	argv(2));
		sobject1 = PRVM_G_STRING(OFS_PARM1);	// SetPropertyString hform ListBlocks BackColor #fc3
		if (String_Match_Caseless(sobject1, "Key")) {
			// Special case ... GetFormListItem
			sobject1 = NULL; // Form
			sproperty = PRVM_G_STRING(OFS_PARM1); // "Key"
			skeyname = PRVM_G_STRING(OFS_PARM2); // "Describe" (keyname)
			svalue = PRVM_G_STRING(OFS_PARM3);
		} else {

			sproperty = PRVM_G_STRING(OFS_PARM2); // BackColor
			svalue = PRVM_G_STRING(OFS_PARM3); // #fc3
		}
	} else if (prog->argc == 5) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);	// SetListItem hform ListBlocks List 4 #fc3
		sproperty = PRVM_G_STRING(OFS_PARM2);
		listindex = (int)PRVM_G_FLOAT(OFS_PARM3);
		svalue = PRVM_G_STRING(OFS_PARM4);
	}

	qbool ret = CSQC_Object_Property_Set_Ok (f, sobject1, sproperty, listindex, skeyname, svalue);

	if (!ret) {
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return; // Failed.  Already printed a message, return NULL
	}

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, svalue);
}


//float Form_Command (float h, string command) = #184; // Returns > 0 on success, < 0 on failure.
//float Object_Command (float h, string objectname, string command, optional string svalue) = #184; // Returns > 0 on success, < 0 on failure.

void VM_CL_Form_Command (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(2, 4, VM_CL_Form_Command);

	HFORM_HANDLE_BOILER; // arg0
	void *f = prog->forms[handle];

	// 0: handle  1:command (2) // Form command
	// 0: handle  1:objectname 2:command (3) // Object command

	RELATED_ (VM_map_load VM_map_entity_add)

	int ret = not_found_neg1;
	switch (prog->argc) {
	case 2:
			{ // Form_Command (hform, "Show");
			ccs *scommand = PRVM_G_STRING(OFS_PARM1); // Baker: No string checking (yet?)
			ret = CSQC_Object_Method (f, NULL, scommand, NULL);
			}
			break;

	case 3:
			{ // Object_Command (hform, "ListBlocks", "SetFocus");
			ccs *sobject1 = PRVM_G_STRING(OFS_PARM1); //
			ccs *scommand = PRVM_G_STRING(OFS_PARM2); //
			ret = CSQC_Object_Method (f, sobject1, scommand, NULL);
			}
			break;

	case 4:
			{ // Object_Command (hform, "ListBlocks", "AddImage", spng );
			ccs *sobject1 = PRVM_G_STRING(OFS_PARM1); //
			ccs *scommand = PRVM_G_STRING(OFS_PARM2); //
			ccs *svalue = PRVM_G_STRING(OFS_PARM3); //
			ret = CSQC_Object_Method (f, sobject1, scommand, svalue);
			}
			break;
	}

	PRVM_G_FLOAT(OFS_RETURN) = ret;
}

RELATED_ (VM_map_close)

// float Form_Close (__inout float hform) = #183; // Return value is 1 on success, 0 on failure. Sets hmap to 0.

RELATED_ (CSQC_Form_Queue_Close)
void VM_CL_Form_Close (prvm_prog_t *prog)
{
	RELATED_ (VM_search_end)

	VM_SAFEPARMCOUNTRANGE (1, 2, VM_CL_Form_Close);

	// Baker: Deliberately no boiler macro
	int _hmap = (int)PRVM_G_FLOAT(OFS_PARM0);
	int handle = UNPLUS1(_hmap);

	ccs *sMessage = prog->argc >= 2 ? PRVM_G_STRING(OFS_PARM1) : NULL;

	if (in_range_beyond (0, handle, PRVM_MAX_OPEN_FORMS_8) == false) {
		VM_WarningLinef (prog, "%s: invalid handle %d used in %s", __func__, handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	if (prog->forms[handle] == NULL) {
		VM_WarningLinef (prog, "%s: no such handle %d in %s", __func__, handle, prog->name);
		PRVM_G_FLOAT(OFS_RETURN) = HMAP_HANDLE_INVALID_NEG_4;
		return;
	}

	void *f = prog->forms[handle];
	//func_t close_callback = 0;
	//void *fparent = NULL;
	//CSQC_Form_Prepare_Close (f, &fparent, &close_callback);
	//ktrl f = (ktrl )_f;
	//*pfparent	= f->ui.fModalBoss;
	//*pClose		= f->ui.form_close_qcfunc;
	CSQC_Form_Queue_Close (f, sMessage); // Wait a frame.
#if 0
	f = CSQC_Form_Destroy_VoidP(f);
	prog->forms[handle] = NULL;

	if (close_callback) {
		CL_VM_String_Callback (bad f->ui.fModalBoss, close_callback, sMessage);
	}
#endif

	PRVM_G_FLOAT(OFS_PARM0) = 0; // Sets the QuakeC float handle to 0
	PRVM_G_FLOAT(OFS_RETURN) = 1;
}


//float SetFormProperty (float h, string propertyname, float value) = #185; // Returns string on success, NULL on failure.
//float SetProperty (float h, string objectname, string propertyname, float value) = #185; // Returns string on success, NULL on failure.

// Form_QueueRefresh typically occurs -- will propogate on next draw
void VM_CL_Form_Property_Set_Float (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(3, 4, VM_CL_Form_Property_Set);

	HFORM_HANDLE_BOILER;

	void *f = prog->forms[handle];

	// 0: handle  1:command (2) // form_property_set
	// 0: handle  1:objectname 2:command (3) // object_property_set
	ccs *sobject1 = NULL, *sproperty = NULL, *skeyname = NULL;
	int listindex = not_found_neg1;

	float fvalue = -1;
	if (prog->argc == 3) {
		sproperty = PRVM_G_STRING(OFS_PARM1);	// SetFormProperty hform Autosize 1
		fvalue = PRVM_G_FLOAT(OFS_PARM2);
	} else if (prog->argc == 4) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);
		if (String_Match_Caseless(sobject1, "Key")) {
			// Special case ... GetFormListItem
			//float  IsFolders	= SetProperty		(h, "Key", "IsFolders", Options != "NoFolders");
			sobject1 = NULL; // Form
			sproperty = PRVM_G_STRING(OFS_PARM1); // "Key"
			skeyname = PRVM_G_STRING(OFS_PARM2); // IsFolders
			fvalue = PRVM_G_FLOAT(OFS_PARM3); // fval
		} else {
			// SetProperty hform ListBlocks Autosize 1
			sproperty = PRVM_G_STRING(OFS_PARM2);
			fvalue = PRVM_G_FLOAT(OFS_PARM3);
		}
	} else if (prog->argc == 5) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);	// SetListItem hform ListBlocks List 4 #fc3
		sproperty = PRVM_G_STRING(OFS_PARM2);
		listindex = (int)PRVM_G_FLOAT(OFS_PARM3);
		fvalue = PRVM_G_FLOAT(OFS_PARM4);
	}

	qbool ret = CSQC_Object_Property_Set_Float_Ok (f, sobject1, sproperty, listindex, skeyname, fvalue);

	if (!ret) {
		PRVM_G_FLOAT(OFS_RETURN) = -1;
		return; // Failed.  Already printed a message, return NULL
	}

	PRVM_G_FLOAT(OFS_RETURN) = fvalue;
}

//float GetFormProperty (float h, string propertyname) = #186; // Returns string on success, NULL on failure.
//float GetProperty (float h, string objectname, string propertyname) = #186; // Returns string on success, NULL on failure.
void VM_CL_Form_Property_Get_Float (prvm_prog_t *prog)
{
	char string16384[VM_STRINGTEMP_LENGTH_16384]; //

	// set the return value regardless of any possible errors
	PRVM_G_FLOAT(OFS_RETURN) = -1;

	VM_SAFEPARMCOUNTRANGE(2, 4, VM_CL_Form_Property_Get);

	HFORM_HANDLE_BOILER;

	void *f = prog->forms[handle];

	// 0: handle  1:command (2) // GetFormProperty hform FocusStyle
	// 0: handle  1:objectname 2:command (3) // GetProperty hform LabelBlocks Autosize

	ccs *sobject1 = NULL, *sproperty = NULL, *skeyname = NULL;;
	int listindex = not_found_neg1;
	if (prog->argc == 2) {
		sproperty = PRVM_G_STRING(OFS_PARM1);		// GetFormProperty hform FocusStyle
	} else if (prog->argc == 3) {
		sobject1 = PRVM_G_STRING(OFS_PARM1);		// GetProperty hform LabelBlocks Autosize

		if (String_Match_Caseless(sobject1, "Key")) {
			// Special case ... GetFormListItem
			//Form_Command (h, "GetKey", "DotExt");
			sobject1 = NULL;
			sproperty = PRVM_G_STRING(OFS_PARM1);
			skeyname = PRVM_G_STRING(OFS_PARM2);
		}
		else if (String_Match_Caseless(sobject1, "Controls")) { // GetFormListItem (hform, "Controls", n) = #182;
			// Special case ... GetFormListItem
			sobject1 = NULL;
			sproperty = PRVM_G_STRING(OFS_PARM1);
			listindex = (int)PRVM_G_FLOAT(OFS_PARM2);
		} else {
			sproperty = PRVM_G_STRING(OFS_PARM2);
		}
		sproperty = PRVM_G_STRING(OFS_PARM2);
	}

	qbool ret = CSQC_Object_Property_Get_Ok (f, sobject1, sproperty, listindex, skeyname, string16384, sizeof(string16384));

	if (!ret)
		return; // Failed.  Already printed a message, return NULL

	PRVM_G_FLOAT(OFS_RETURN) = atof(string16384);
}





#endif // CONFIG_MENU


// #190

// Returns >=0 for success, <0 for failure
// float fs_fileinfo(string filename, __inout float isDirectory, __inout float isExistingFile,
//__inout float isOutsideAPak, __inout string datestring, __inout float FileSizeRealPath, __inout float FileSize) = #190;
// Bytes returns -1 if not found or directory
// Datestring is NULL (world) if is not a real existing file (that is not in a pak)		if (!isRealFile)

void VM_FS_FileInfo (prvm_prog_t *prog)
{
	char datestring[VM_STRINGTEMP_LENGTH_16384];

	VM_SAFEPARMCOUNTRANGE(7, 7, VM_FS_FileInfo);

	// set the return value regardless of any possible errors
	PRVM_G_FLOAT(OFS_RETURN) = -1;
	PRVM_G_INT(OFS_PARM3) = OFS_NULL; // datestring

	ccs *filename = PRVM_G_STRING(OFS_PARM0);
	qbool isDirectory = false;
	qbool isExistingFile = false;
	qbool isOutsideAPak = false;
	double bytesrealpath = -1;
	double bytesanypath = -1;

	while (1) {
		isDirectory = FS_IsDirectory (filename);
		//if (isDirectory)
		//	break; // Directory ...
		isExistingFile = FS_FileExists (filename);
		//if (!isExistingFile)
		//	break; // Non-existent file
		isOutsideAPak = FS_IsRealFile (filename); // It is in a pak or something.
		bytesanypath = -1;
		qfile_t *file = FS_OpenVirtualFile(filename, fs_quiet_true);
		if (file) {
			bytesanypath = FS_FileSize (file);
			FS_CloseNULL_ (file);
		}

		bytesrealpath = FS_FileSizeEx_Safe (filename);
//		if (!isRealFile)
//			break;

		// This isn't the file time!
		qbool ok = File_TimeString_Fill_MM_DD_YY_HH_MM_AM (filename, datestring, sizeof(datestring));
		if (/*isDirectory && */!ok) {
			// Baker: Directories not in current game should report as realfile 0.
			// We aren't really doing "realfile" but rather
			// Real file in current gamedir.
			isOutsideAPak = false;
		}
		break;
	}

	PRVM_G_FLOAT(OFS_PARM1) = (int)isDirectory; // isDirectory
	PRVM_G_FLOAT(OFS_PARM2) = (int)isExistingFile; // isExisting
	PRVM_G_FLOAT(OFS_PARM3) = (int)isOutsideAPak; // isRealFile
	PRVM_G_INT(OFS_PARM4)	= PRVM_SetTempString(prog, datestring); // DateTime epoch time.
	PRVM_G_FLOAT(OFS_PARM5) = (float)bytesrealpath; // Bytes
	PRVM_G_FLOAT(OFS_PARM6) = (float)bytesanypath; // Bytes

	PRVM_G_FLOAT(OFS_RETURN) = 1;
}

// Returns "id1" or whatever.  Only returns where files would be written for a .sav or .dem.
// Purpose is to give the user some idea of what folder they are in for file dialogs.
// #191
void VM_FS_GetGameDir (prvm_prog_t *prog)
{
	//char datestring[VM_STRINGTEMP_LENGTH_16384];

	VM_SAFEPARMCOUNTRANGE(0, 0, VM_FS_GetGameDir);

	ccs *x_gamedir = fs_numgamedirs ? fs_gamedirs[fs_numgamedirs - 1] : gamedirname1;

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, x_gamedir); // DateTime epoch time.
}

// NULL on too long result (exceeds QuakeC string length of 16384)

// string stringlist_to_zipbase64 (float bufhandle, float flags) = #192; // Returns Base64 encoded string.
RELATED_ (VM_stringlist_from_zipbase64_is_ok)
void VM_stringlist_to_zipbase64 (prvm_prog_t *prog)
{
	PRVM_G_INT(OFS_RETURN) = OFS_NULL; // In case of failure

	int bufnum = (int)PRVM_G_FLOAT(OFS_PARM0);
	RELATED_ (VM_buf_getsize)
	
	prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index(prog, bufnum);
	if (!stringbuffer) {
		VM_WarningLinef (prog, "%s: invalid buffer %d used in %s", __func__, bufnum, prog->name);
		return;
	}
	char *s_za = stringbuffer_to_zippedbase64_newline_delim_ZAlloc(prog, stringbuffer);

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString16384(prog, s_za);
	Mem_FreeNull_ (s_za);
}

// string stobase64 (string s) = #192; // Returns Base64 encoded string.
RELATED_ (VM_base64tos)
void VM_stobase64 (prvm_prog_t *prog)
{
	// Base 64 max increase of 37%
	VM_SAFEPARMCOUNTRANGE(1, 2, VM_stobase64);

	if (prog->argc == 2) {
		// stringlist_to_zipbase64 if uses flags arg
		VM_stringlist_to_zipbase64 (prog);
		return;
	}

	PRVM_G_INT(OFS_RETURN) = OFS_NULL; // In case of failure
	ccs *s = PRVM_G_STRING(OFS_PARM0);

	size_t slen = strlen(s);
	char *s_base64_alloc = base64_encode_calloc ((byte *)s, slen, q_reply_len_NULL); // malloc

	size_t b64len = strlen(s_base64_alloc);
	if (b64len >= VM_STRINGTEMP_LENGTH_16384) {
		char sfirst20[20];
		c_strlcpy (sfirst20, s);
		VM_WarningLinef (prog, "%s: base64 encoded string length (%d) for " QUOTED_S
									" ... exceeds QuakeC limit of %d in %s",
			__func__,
			(int)b64len,
			sfirst20,
			(int)VM_STRINGTEMP_LENGTH_16384,
			prog->name
		);

		// CLEANUP!
		goto cleanup;
	}

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, s_base64_alloc);

cleanup:
	free (s_base64_alloc);
}

// string worldkey (string key, optional flags) = #194; // Returns a worldspawn key

//                          0                 1                             2
// Returns blocksize 0 0 0 means bad.

// "worldgrid_zip_base64"
// WORLDGRID_ZIP_BASE64
//worldgrid_get (plist_map, numcolumns, numrows)
RELATED_ (SCR_mazegen3_f VM_bufstr_add  write_map_from_blocks Parse_Blocks_List_From_Text)

// Read an embedded mymap.txt file and get the information
// In the future, if there is a center just set v_up or something.
// vector worldgrid_get_blocksize (float plistblocknames, float plistvisited, __inout float numcolumns, __inout float numrows) = #194; // Returns a worldspawn key
// IF plistblocknames is 0, it will not be filled.
// IF plistvisited is 0, it will not be filled.
// However, in vector form it would be columns, rows (X, Y) .. so standardize QuakeC around that.

CALLERS_ (VM_worldkey) //
void worldgrid_get_compressed_and_blocksize (prvm_prog_t *prog, baker_string_s *bsa, vec3_t blocksize_out)
{
	ccs *worldgrid_blocksize = NULL;
		
	qbool is_server = prog == SVVM_prog;
	va_super (entname, MAX_QPATH_128, "%s.ent", is_server ? sv.worldnamenoextension : cl.worldnamenoextension); // DOT ENT (11)

	char *entities_allocated_za	= FS_LoadFileQuick (entname);
	char *entities				= entities_allocated_za ? entities_allocated_za : (is_server ? sv.worldmodel->brush.entities : cl.worldmodel->brush.entities);
	
	worldgrid_blocksize = String_Worldspawn_Value_For_Key_Sbuf (entities, WORLDGRID_BLOCKSIZE); // SBUF IS SUPER SHORT TERM!
	// PROCESS HERE.  FREEING DATA MAKES IT DIE
	if (worldgrid_blocksize) {
		Math_atov3 (worldgrid_blocksize, blocksize_out);
	}

	if (worldgrid_blocksize) {
		int segnum = 0;
		while (1) {
			va_super (keyname, 512, WORLDGRID_ZIP_BASE64 "%d", segnum);
			ccs *worldgrid_zipbase64x = String_Worldspawn_Value_For_Key_Sbuf (entities, keyname);
			if (!worldgrid_zipbase64x)
				break;

			BakerString_CatC (bsa, worldgrid_zipbase64x);
			segnum ++;
		}
	}

	Mem_FreeNull_ (entities_allocated_za);
}

void Parse_Blocks_List_From_Text_To_Png (stringlist_t *plist_blockspng, ccs *s_mymap_txt_blob, int *pnumrows, int *pnumcolumns);
void worldgrid_get_blocksize (prvm_prog_t *prog)
{
	VM_SAFEPARMCOUNTRANGE(4, 4, worldgrid_get_blocksize);
	RELATED_ (write_map_from_blocks stringbuffer_append)

	// READ ARGS
	vec3_t vblocksize = {0,0,0};
	int bufnum_blocks	= (int)PRVM_G_FLOAT(OFS_PARM0);
	int bufnum_visited = (int)PRVM_G_FLOAT(OFS_PARM1);
	int numcolumns = 0;		// numcolumns is output
	int numrows = 0;		// numrows is output

	VectorCopyDestSrc(PRVM_G_VECTOR(OFS_RETURN), vblocksize);

	prvm_stringbuffer_t *stringbuffer_blocks = NULL;
	if (bufnum_blocks) {
		stringbuffer_blocks = stringbuffer_getlist_by_index (prog, bufnum_blocks);
		if (!stringbuffer_blocks) {
			VM_WarningLinef (prog, "%s: invalid buffer %d used in %s", __func__, bufnum_blocks, prog->name);
			return;
		}
	}

	prvm_stringbuffer_t *stringbuffer_visited = NULL;
	if (bufnum_visited) {
		stringbuffer_visited = stringbuffer_getlist_by_index (prog, bufnum_visited);
		if (!stringbuffer_visited) {
			VM_WarningLinef (prog, "%s: invalid buffer %d used in %s", __func__, bufnum_visited, prog->name);
			return;
		}
	}

	baker_string_s *bsa = BakerString_Create_Malloc ("");
	worldgrid_get_compressed_and_blocksize (prog, bsa, vblocksize);

	char *worldgrid_decompressed_za = bsa->length ? unbase64unzip_zalloc(bsa->string) : NULL;

	BakerString_Destroy_And_Null_It (&bsa);

	// Append the string
	if (Vector3_IsNotZeros (vblocksize) && worldgrid_decompressed_za) {
		stringlist_t list_blocks = {0};
#if 0 //def _DEBUG
		int slen = strlen(worldgrid_decompressed_za);
		Clipboard_Set_Text (worldgrid_decompressed_za);
#endif // _DEBUG
		Parse_Blocks_List_From_Text_To_Png (&list_blocks, worldgrid_decompressed_za, &numrows, &numcolumns);
		if ( stringbuffer_blocks) {
			stringbuffer_append_stringlist_not_zero_length (&list_blocks, prog, stringbuffer_blocks);
		}
		stringlistfreecontents (&list_blocks);

		int numcells = numrows * numcolumns;
		// Set the size

		//BufStr_Expand(prog, stringbuffer_visited, numcells - 1);
		if (stringbuffer_visited) {
			BufStr_Set (prog, stringbuffer_visited, numcells, "BEYOND");
		}
	}

	PRVM_G_FLOAT(OFS_PARM2) = numcolumns;
	PRVM_G_FLOAT(OFS_PARM3) = numrows;

	Mem_FreeNull_ (worldgrid_decompressed_za);

	VectorCopyDestSrc(PRVM_G_VECTOR(OFS_RETURN), vblocksize);
}


// string worldkeyget (string key, optional float flags) = #194; // Returns a worldspawn key
// vector worldgrid_get_compressed (__inout string sout, float flags) = #194; // Returns a worldspawn key
// vector worldgrid_get_blocksize (float buf_blocknames, float buf_visited, __inout float numcolumns, __inout float numrows) = #194;

RELATED_ (worldgrid_get_blocksize SCR_mazegen3_f SCR_mazegen2_f Halls_Make)
void VM_worldkey (prvm_prog_t *prog)
{
	// Get a worldspawn key
	VM_SAFEPARMCOUNTRANGE(1, 4, VM_worldkey);

	PRVM_G_INT(OFS_RETURN) = OFS_NULL; // In case of failure

	if (prog->argc == 4) {
		// worldgrid_get_blocksize
		worldgrid_get_blocksize (prog);
		return;
	}

	int flags	= prog->argc >= 2 ? (int)PRVM_G_FLOAT(OFS_PARM1) : 0;
	if (prog->argc == 2 && flags == WORLDGRID_COMPRESSED_1) {
		// worldgrid_get_compressed
		vec3_t vblocksize={0,0,0};
		baker_string_s *bsa = BakerString_Create_Malloc ("");
		worldgrid_get_compressed_and_blocksize (prog, bsa, vblocksize);
		ccs *worldgrid_compressed = bsa->length ? bsa->string : NULL;

		char string16384[VM_STRINGTEMP_LENGTH_16384] = {0}; // Baker: Size enforce
		if (worldgrid_compressed)
			c_strlcpy (string16384, worldgrid_compressed);

		PRVM_G_INT(OFS_PARM0) = PRVM_SetTempString(prog, string16384);
		VectorCopyDestSrc(PRVM_G_VECTOR(OFS_RETURN), vblocksize);

		// Cleanup
		BakerString_Destroy_And_Null_It (&bsa);
		return;
	}

	RELATED_ (String_Worldspawn_Value_For_Key_Sbuf)

	
	ccs *key	= PRVM_G_STRING(OFS_PARM0);
//#if 0 // PLANNED --- March 29 2026 -- but not actually implemented right now.
//	int flags	= prog->argc >= 2 ? (int)PRVM_G_FLOAT(OFS_PARM1) : 0;
//#endif //

	if (prog->argc == 2 && flags == WORLDGRID_COMPRESSED_1) {
		return;
	}

	va_super (entname, MAX_QPATH_128, "%s.ent", cl.worldnamenoextension); // DOT ENT (11)

	char *entities_allocated	= FS_LoadFileQuick (entname);
	char *entities				= entities_allocated ? entities_allocated : cl.worldmodel->brush.entities;

	ccs *current_value = String_Worldspawn_Value_For_Key_Sbuf (entities, key);
	if (current_value != NULL)
		PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, current_value);

	Mem_FreeNull_ (entities_allocated);
}


// NULL on too long result (exceeds QuakeC string length of 16384)

//float  stringlist_from_zipbase64_is_ok (float bufnum, string zipbase64string) = #193; // Returns isok >0
RELATED_ (VM_stringlist_to_zipbase64)
void VM_stringlist_from_zipbase64_is_ok(prvm_prog_t *prog)
{
	PRVM_G_FLOAT(OFS_RETURN) = 0; // In case of failure, set isok 0
	
	int bufnum = (int)PRVM_G_FLOAT(OFS_PARM0);
	ccs *s = PRVM_G_STRING(OFS_PARM1);
	prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index(prog, bufnum);
	if (!stringbuffer) {
		VM_WarningLinef (prog, "%s: invalid buffer %d used in %s", __func__, bufnum, prog->name);
		return;
	}

	char *s_za = unbase64unzip_zalloc(s);
	stringlist_t list = {0};
	stringlistappend_textlines_cr_scrub (&list, s_za);
	Mem_FreeNull_ (s_za);
	stringbuffer_append_stringlist (&list, prog, stringbuffer);
	stringlistfreecontents (&list);
	 
	PRVM_G_FLOAT(OFS_RETURN) = 1; // In case of failure, set isok 0
}

RELATED_ (VM_stobase64)
//float  stringlist_from_zipbase64_is_ok (float bufnum, string zipbase64string) = #193; // Returns isok >0

// string base64tos (string sbase64encoded) = #193; // Returns decoded string from Base64 string.
void VM_base64tos (prvm_prog_t *prog)
{
	// Base 64 max increase of 37%
	VM_SAFEPARMCOUNTRANGE(1, 2, VM_stobase64);

	if (prog->argc == 2) {
		// stringlist_to_zipbase64 if uses flags arg
		VM_stringlist_from_zipbase64_is_ok (prog);
		return;
	}

	PRVM_G_INT(OFS_RETURN) = OFS_NULL; // In case of failure
	ccs *s = PRVM_G_STRING(OFS_PARM0);

	size_t unbase_datasize = 0;
	char *data_unbase64_calloc = (char *)base64_decode_calloc (s, &unbase_datasize); // malloc

	// This ensures null termination of string.
	char *s_a = Z_StrDup_Len_Z (data_unbase64_calloc, unbase_datasize);

	// Baker: This shouldn't be possible.  Base64 encoded strings should decode to 33% - 37% smaller.
	// Although it is possible for some "too long" input string somehow?
	if (unbase_datasize >= VM_STRINGTEMP_LENGTH_16384) {
		char *sfirst20 = Z_StrDup_Len_Z (data_unbase64_calloc, unbase_datasize > 20 ? 20 : unbase_datasize);

		VM_WarningLinef (prog, "%s: base64 decoded string length (%d) for " QUOTED_S
									" ... exceeds QuakeC limit of %d in %s",
			__func__,
			(int)unbase_datasize,
			sfirst20,
			(int)VM_STRINGTEMP_LENGTH_16384,
			prog->name
		);
		// CLEANUP
		Mem_FreeNull_ (sfirst20);
		goto cleanup;
	}

	PRVM_G_INT(OFS_RETURN) = PRVM_SetTempString(prog, s_a);

cleanup:
	Mem_FreeNull_ (s_a);
	free (data_unbase64_calloc);
}







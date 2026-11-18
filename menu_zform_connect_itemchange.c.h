// menu_zform_connect_itemchange.c.h



// SETUP LISTVIEW2 usually.  Sometimes LISTVIEW3
RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)

EXO___ char *DevList2ViewOnChange (ktrl k) // returns an error string or NULL
{
	stringlist_t *plist2 = &k->list_strings_a;
	int row = k->selectedindex;
	int numcolumns = k->columncount;
	int row_idx = row * numcolumns;
	int row_idx_colmax = row_idx + numcolumns - 1;
	qbool is_empty_data = (false == in_range_beyond(0,row_idx_colmax,plist2->numstrings));
	if (is_empty_data) {
		Object_Property_Set (k, "ToolTip", "");
	}
	else {
		ccs *sxy = plist2->strings[row_idx_colmax];
		Object_Property_Set (k, "ToolTip", sxy);
	}
	return NULL;
}

EXO___ char *DevListViewOnChange (ktrl k) // returns an error string or NULL
{
	// 1. We create a long string version with the sort key leading. (TAB delimited?  Or some weird character?)
	// 2. All the columns cat "as-is"
	// 3. We sort
	// 4. Then we decode
	if (k != ezdev->kList1) {
		return NULL; // Mar 2 2026 - Happens?
	}
	stringlist_t *plist2 = &ezdev->kList2->list_strings_a;
	stringlist_t *plist3 = &ezdev->kList3->list_strings_a;

	qbool bcan_be_text_filtered_have_data = (int)ezdev->f->userdata; // Different var name for search
	if (bcan_be_text_filtered_have_data == false) {
		// This should not happen -- but it does TabSelectOnchange => TxtFilterOnChange => here
		return NULL;
	}

	// This can happen if the filter is set very tight
	int row = ezdev->kList1->selectedindex;
	int numcolumns = ezdev->kList1->columncount;
	int row_idx = row * numcolumns;
	int row_idx_col0 = row_idx + /*col*/ 0;
	qbool is_empty_data = (false == in_range_beyond(0,row_idx_col0,ezdev->kList1->list_strings_a.numstrings));

	int tab_idx = ezdev->kTabSelect1->selectedindex;
	Form_QueueRefresh(k->fctrl);

	//
	if (isin1(tab_idx, lw_textures_3)) {
		if (is_empty_data == false) {
			RELATED_ (R_WorldTextures_Query)

			ccs			*s_textureindex = ezdev->kList1->list_strings_a.strings[row_idx_col0];
			int			textureindex	= atoi(s_textureindex);

			model_t		*m				= r_refdef.scene.worldmodel;
			texture_t	*t				= &m->data_textures[textureindex];
			float		aspect			= 1.0;
			ccs			*texturename	= t->name;

			qbool is_taller = t->height > t->width;

			if (t->height) {
				if (is_taller)
					aspect = t->width / (float)t->height;
				else
					aspect = t->height / (float)t->width;
			}

			float basesize = 150.0;
			if (is_taller) {
				ezdev->kImageTexture->cm._relative_rect_orig.width = basesize * aspect; // connect_metric
				ezdev->kImageTexture->cm._relative_rect_orig.height = basesize; // connect_metric
			} else {
				ezdev->kImageTexture->cm._relative_rect_orig.width = basesize; // connect_metric
				ezdev->kImageTexture->cm._relative_rect_orig.height = basesize * aspect; // connect_metric
			}
			Object_Property_Set (ezdev->kImageTexture, "ImageName", texturename);
		}
		Form_QueueRefreshPlusMetric(k->fctrl);
		return NULL;
	}

	if (tab_idx == lw_models_1) { // KLIST2
		stringlistfreecontents (&ezdev->kList2->list_strings_a); //Object_Method_Clear
		ezdev->kList2->selectedindex = 0;
		ezdev->kList2->selectedlength = 0;
		ezdev->kList2->sortcolumnindex = 0;
		ezdev->kList2->sortcolumndescending = false;
		ezdev->kList2->r_scrollport.left = ezdev->kList2->r_scrollport.top = 0;
		if (is_empty_data || row == not_found_neg1)
			return NULL;

		ccs *s_modelindex = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		int modelindex = atoi(s_modelindex);
		model_t *mod = cl.model_precache[modelindex];
		// FILL LIST2

		RELATED_ (Mod_Info_f)
		// WHAT KIND OF MODEL STUFF?
		stringlistappendf2	(plist2, "name",		"%s", mod->model_name);
		stringlistappendf2	(plist2, "poses",		"%d", mod->num_poses);
		stringlistappendf2	(plist2, "frames",		"%d", mod->numframes);
		stringlistappendf2	(plist2, "triangles",	"%d", mod->surfmesh.num_triangles);

		stringlistappendf2	(plist2, "bones",		"%d", mod->num_bones);
		stringlistappendf2	(plist2, "tags",		"%d", mod->num_tags);
		stringlistappendf2	(plist2, "skins",		"%d", mod->numskins);
		stringlistappendf2	(plist2, "textures",	"%d", mod->num_textures);
		stringlistappendf2	(plist2, "mins",		VECTOR3_5d1F, VECTOR3_SEND(mod->normalmins));
		stringlistappendf2	(plist2, "maxs",		VECTOR3_5d1F, VECTOR3_SEND(mod->normalmaxs));

		vec3_t sizez;
		VectorSubtract (mod->normalmaxs, mod->normalmins, sizez);
		stringlistappendf2	(plist2, "size",			VECTOR3_5d1F, VECTOR3_SEND(sizez));

		RELATED_ (Mod_Alias_GetTagIndexForName)
		for (int n = 0; n < mod->numskins; n ++) {
			stringlistappendf	(plist2, "Skin %4d", n);
			stringlistappend	(plist2, (mod->data_textures + n)->name);
		}

		for (int n = 0; n < mod->num_bones; n ++) {
			stringlistappendf	(plist2, "Bone %4d", n);
			stringlistappend	(plist2, mod->data_bones[n].name);
		}
		for (int n = 0; n < mod->num_tags; n ++) {
			stringlistappendf	(plist2, "Tag %4d", n);
			stringlistappend	(plist2, mod->data_tags[n].name);
		}

		return NULL;
	} // modelinfo

	if (tab_idx == lw_shaders_4) { // LIST3
		stringlistfreecontents (plist3); //Object_Method_Clear
		ezdev->kList3->selectedindex = 0;
		ezdev->kList3->selectedlength = 0;
		ezdev->kList3->sortcolumnindex = 0;
		ezdev->kList3->sortcolumndescending = false;
		ezdev->kList3->r_scrollport.left = ezdev->kList3->r_scrollport.top = 0;

		if (is_empty_data || row == not_found_neg1)
			return NULL;

		int shader_col1_idx = row_idx + /*col*/ 1;
		ccs *s_shadername = ezdev->kList1->list_strings_a.strings[shader_col1_idx];
		int shader64row = stringlist_find_row_with_column_string_match (&ezdev->list_shaders64, s_shadername, LW_SHADER64_COLUMNS_3, /*columnwanted*/ 0);

		if (shader64row == not_found_neg1) {
			return NULL;
		}

		//ccs *shader64name = ezdev->list_shaders64.strings[shader64row * LW_SHADER64_COLUMNS_3 + 0];
		//ccs *shader64file = ezdev->list_shaders64.strings[shader64row * LW_SHADER64_COLUMNS_3 + 1];
		ccs *shader64blob = ezdev->list_shaders64.strings[shader64row * LW_SHADER64_COLUMNS_3 + 2];

        // Now split it
        stringlist_t list_lines = {0};
        stringlistappend_textlines_cr_scrub  (&list_lines, shader64blob);

        for (int sidx = 0; sidx < list_lines.numstrings; sidx ++) {
            ccs *sxy = list_lines.strings[sidx];
            stringlistappendf	(plist3, "%d", sidx);
            stringlistappend	(plist3, sxy);
        }
        stringlistfreecontents (&list_lines);

	} // lw_shaders

	if (tab_idx == lw_effectinfo_list_22) { // LIST3
		stringlistfreecontents (plist3); //Object_Method_Clear <================
		ezdev->kList3->selectedindex = 0;
		ezdev->kList3->selectedlength = 0;
		ezdev->kList3->sortcolumnindex = 0;
		ezdev->kList3->sortcolumndescending = false;
		ezdev->kList3->r_scrollport.left = ezdev->kList3->r_scrollport.top = 0;

		if (is_empty_data || row == not_found_neg1)
			return NULL;

		// "#,Text"
		RELATED_ (CL_EffectInfo_Dump_f)
		{
			int shader_col1_idx = row_idx + /*col*/ 1;
			ccs *effectnamewanted = ezdev->kList1->list_strings_a.strings[shader_col1_idx];

			int effectinfo_dump_name_columns_2 (stringlist_t *plist, ccs *s_effectname_wanted);
			effectinfo_dump_name_columns_2 (plist3, effectnamewanted);
		}

	} // lw_effectinfo_list_22

	if (tab_idx == lw_effectinfo_txt_23) {
		if (is_empty_data || row == not_found_neg1)
			return NULL;

		int text_line_idx = row_idx + /*col*/ 1;
		ccs *textline = ezdev->kList1->list_strings_a.strings[text_line_idx];

		qbool parsed_ok = false;
		int texnum0 = not_found_neg1;
		int texnum1 = not_found_neg1;
		if (strstr(textline, "tex")) {
			//va_super (s_line, 1024, "%s", textline);
			ccs *text = textline; //s_line;
			if (COM_ParseToken_Simple(&text, /*newline?*/ true, /*parse backslash?*/ false, /*parse comments?*/ true)) {
				if (String_Match (com_token, "tex")) {
					if (COM_ParseToken_Simple(&text, /*newline?*/ true, /*parse backslash?*/ false, /*parse comments?*/ true)) {
						texnum0 = atoi(com_token);
						if (COM_ParseToken_Simple(&text, /*newline?*/ true, /*parse backslash?*/ false, /*parse comments?*/ true)) {
							texnum1 = atoi(com_token);
							parsed_ok = true;
						}
					}
				}
			}
		} // tex
		ezdev->kImageParticleFont->is_hidden = (parsed_ok == false);
		if (parsed_ok) {
			float s0 = -999, t0= -999, s1 = -999, t1 = -999;
			qbool ok = ParticleFont_Get_Index_Was_Ok(texnum0, &s0, &t0, &s1, &t1);
			RELATED_ (O_Image_Draw)
			if (ok) {
				ezdev->kImageParticleFont->image_slow = s0;
				ezdev->kImageParticleFont->image_tlow = t0;
				ezdev->kImageParticleFont->image_shigh = s1;
				ezdev->kImageParticleFont->image_thigh = t1;
			} else {
				ezdev->kImageParticleFont->is_hidden = true; // Invalid
				ezdev->kImageParticleFont->image_slow = 0;
				ezdev->kImageParticleFont->image_tlow = 0;
				ezdev->kImageParticleFont->image_shigh = 0;
				ezdev->kImageParticleFont->image_thigh = 0;
			}
		}
		//ezdev->kImageTexture
		//ImageParticleFont
		// tex 0 8
	}
#if 0
		for (int effectnum = 0; effectnum < MAX_PARTICLETEXTURES_256; effectnum ++) {
			float s0 = -999, t0= -999, s1 = -999, t1 = -999;
			qbool ok = ParticleFont_Get_Index_Was_Ok(effectnum, &s0, &t0, &s1, &t1);
			if (!ok) {
				// Happens? Yes with 128 size particle font (Intermap)
				stringlistappendf	(&ezdev->listaccum, "%d", effectnum);
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
				stringlistappend	(&ezdev->listaccum, "");
			} else {
				stringlistappendf	(&ezdev->listaccum, "%d", effectnum);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", s0);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", t0);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", s1);
				stringlistappendf	(&ezdev->listaccum, "%1.4f", t1);
			}
		} // for
#endif

	// lw_csqc_ents_9 and 3 others
	if (isin2(tab_idx, lw_entities_0, lw_csqc_ents_9)) {
		prvm_prog_t *prog = tab_idx == lw_csqc_ents_9 ? CLVM_prog : SVVM_prog;
		stringlistfreecontents (plist2); //Object_Method_Clear
		ezdev->kList2->selectedindex = 0;
		ezdev->kList2->selectedlength = 0;
		ezdev->kList2->sortcolumnindex = 0;
		ezdev->kList2->sortcolumndescending = false;
		ezdev->kList2->r_scrollport.left = ezdev->kList2->r_scrollport.top = 0;
		if (is_empty_data || row == not_found_neg1)
			return NULL;

		if (!prog->loaded) {
			// This should never happen
			return NULL;
		}

		ccs *s_edict_num = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		int edict_num = atoi(s_edict_num);

		{
			prvm_edict_t *ed = PRVM_EDICT_NUM(edict_num);

			stringlistappend2 (plist2, "#dict", s_edict_num);
			qbool is_classname = false;

			for (int fieldnum = 1; fieldnum < prog->numfielddefs; fieldnum ++) {
				//PRVM_ED_EdictGet_Fly_Lite_Sizeof (prog, edict_num, "classname", vabuf);
				mdef_t	*d = &prog->fielddefs[fieldnum];
				ccs	*s_key = PRVM_GetString(prog, d->s_name);
				prvm_eval_t	*val = (prvm_eval_t *)(ed->fields.fp + d->ofs);

				// if the value is still all 0, skip the field
				int	vartype = Flag_Remove(d->type, DEF_SAVEGLOBAL);

				// Any of the following suffix we ignore, covered by vector.
				if (vartype == ev_float_2 && String_Ends_With_Caseless_3 (s_key, "_x", "_y", "_z"))
					continue;

				qbool was_found = false;

				// This is trying to determine if it has a non-zero value.
				for (int vector_idx = 0; vector_idx < prvm_type_size[vartype]; vector_idx ++) {
					if (val->ivector[vector_idx]) {
						was_found = true;
						break;
					}
				} // for

				if (!was_found)
					continue; // Baker: July 30 2025 - Does this happen?

				char valuebuf[MAX_INPUTLINE_16384];
				char *s_value = PRVM_UglyValueString(prog, (etype_t)d->type, val, valuebuf, sizeof(valuebuf));

				if (String_Match (s_key, "classname")) {
					s_key = "#lassname";
					is_classname = true;
				}

				// FIX UP ENUMS
				va_super (valuebuf2, MAX_INPUTLINE_16384, "%s", s_value);
				qbool did_change /*keep*/ = Edict_Value_Fixup_Did_Change (s_key, valuebuf2, sizeof(valuebuf2));
				stringlistappend2 (plist2, s_key, valuebuf2); // s_value);
			} // each fieldnum
		} // block

		// We want alpha sort by default
		if (ezdev->kList2->list_strings_a.numstrings) {
			DevListViewSort (ezdev->kList2);
		}

		// Now fix them up
		for (int idx = 0; idx < ezdev->kList2->list_strings_a.numstrings; idx += 2) {
			char *sxy = ezdev->kList2->list_strings_a.strings[idx];
			if (String_Match (sxy, "#lassname")) {
				sxy[0] = 'c';
			}
			if (String_Match (sxy, "#dict")) {
				sxy[0] = 'E';
			}
		} // for

		return NULL;
	} // entities

	if (isin2(tab_idx, lw_csqc_stringlist_12, lw_ssqc_stringlist_13)) {
		// CSQC STRINGLIST AND SSQC STRINGLIST
		prvm_prog_t *prog = tab_idx == lw_csqc_stringlist_12 ? CLVM_prog : SVVM_prog;
		stringlistfreecontents (plist2); //Object_Method_Clear
		ezdev->kList2->selectedindex = 0;
		ezdev->kList2->selectedlength = 0;
		ezdev->kList2->sortcolumnindex = 0;
		ezdev->kList2->sortcolumndescending = false;
		ezdev->kList2->r_scrollport.left = ezdev->kList2->r_scrollport.top = 0;
		if (is_empty_data || row == not_found_neg1)
			return NULL;

		if (!prog->loaded) {
			// This should never happen
			return NULL;
		}

		ccs *s_array_idx = ezdev->kList1->list_strings_a.strings[row_idx_col0];
		int array_idx = atoi(s_array_idx);

		// THIS IS WHERE WE FILL IN LIST2 WITH STRING BUFFER LIST
		prvm_stringbuffer_t *stringbuffer = stringbuffer_getlist_by_index (prog, array_idx);
		if (stringbuffer) {
			for (int idx = 0; idx < stringbuffer->num_strings; idx ++) {
				stringlistappendf (plist2, "%d", idx);
				ccs *sval = stringbuffer_get_index(stringbuffer, idx);
				if (sval)
					stringlistappendf (plist2, QUOTED_S, sval);
				else
					stringlistappend (plist2, "(empty)" );
			} // idx
		} // each idx
		return NULL; // ?
	} // lw_csqc_stringlist_12

	// THIS CAN HAPPEN WITH ANY TabSelectOnChange ==> DevTxtFilterOnChange => Here
	return NULL; // ?
}

RELATED_ (DevInit DevTabSelectOnChange DevListViewSort DevListViewOnChange DevTxtFilterOnChange)

// csg_perform_csgew.c.h

#pragma message ("What can we do to automatically make a misc_model_cool_torch have the light in the right place depending on angle")

static void CloneClassname_CalcLightPosition(const vec3_t model_origin,
                       const vec3_t model_angles,
                       const vec3_t local_offset,
                       vec3_t out_light_pos)
{
    /* Convert yaw angle to radians (model_angles[1] is yaw) */
    double yaw = model_angles[1] * (M_PI / 180.0);
    double cy = cos(yaw);
    double sy = sin(yaw);

    /* Rotate local offset vector around Z-axis by yaw */
    double rx = (local_offset[0] * cy) - (local_offset[1] * sy);
    double ry = (local_offset[0] * sy) + (local_offset[1] * cy);
    double rz = local_offset[2]; /* Z remains unchanged for pure yaw */

    /* Add rotated offset to model origin to get world position */
    out_light_pos[0] = model_origin[0] + (vec_t)rx;
    out_light_pos[1] = model_origin[1] + (vec_t)ry;
    out_light_pos[2] = model_origin[2] + (vec_t)rz;
}

// Baker: August 10 2026 - special handling of func_group_terrain is skipped?
// "maps_sh/fang_grass_only" is used when value is 1 or greater.  Hardcoded name?

qbool Floorage_WriteBox_Clip_To_DidWrite (floatlist_s *flist, ccs *dir, vec3_t minz, vec3_t maxz, vec3_t clipminz, vec3_t clipmaxz)
{
	int numclips = 0;
	Con_PrintLinef ("Considering RECT dir = %s received " VECTOR3BBOX_G, dir, VECTOR3BBOX_SEND (minz, maxz) );
	Con_PrintLinef ("Clipping against parent " VECTOR3BBOX_G, VECTOR3BBOX_SEND (clipminz, clipmaxz));
	if (minz[0] < clipminz[0]) {
		Con_PrintLinef ("Mins X clipped from %g to %g",minz[0], clipminz[0]);
		minz[0] = clipminz[0];
		numclips ++;
	}
	if (maxz[0] > clipmaxz[0]) {
		Con_PrintLinef ("Maxs X clipped from %g to %g",maxz[0], clipmaxz[0]);
		maxz[0] = clipmaxz[0];
		numclips ++;
	}
	if (minz[1] < clipminz[1]) {
		Con_PrintLinef ("Mins Y clipped from %g to %g",minz[1], clipminz[1]);
		minz[1] = clipminz[1];
		numclips ++;
	}
	if (maxz[1] > clipmaxz[1]) {
		Con_PrintLinef ("Maxs X clipped from %g to %g",maxz[1], clipmaxz[1]);
		maxz[1] = clipmaxz[1];
		numclips ++;
	}
	vec3_t dims;
	VectorSubtract (maxz, minz, dims);
	Con_PrintLinef ("Final dimensions: " VECTOR3_G, VECTOR3_SEND(dims) );
	if (dims[0] <= 0) {
		Con_PrintLinef ("Rejected on X width %g", dims[0]);
		return false;
	}
	if (dims[1] <= 0) {
		Con_PrintLinef ("Rejected on Y height %g", dims[1]);
		return false;
	}

	Con_PrintLinef ("Rect survived with %d clips============================================================", numclips);
	maxz[2] = RECTUS_ACTIVE_1;
	floats_add_vector (flist, minz);
	floats_add_vector (flist, maxz);

	return true;
}

#define emins e->bemins
#define emaxs e->bemaxs

static int _floorage_sub_collide_many (floatlist_s *flist, entityx_s *e, int entnum)
{
	int starting_count = flist->count;
	Con_PrintLinef ("Ent %d: " VECTOR3BBOX_G " testing versus floorage", entnum, VECTOR3BBOX_SEND(e->bemins,e->bemaxs) );
	for (int fidx = 0; fidx < starting_count; fidx += BBOX_6) {
		vec3_t bmins, bmaxs;
		floats_vector_read_at (bmins, flist, fidx + 0);
		floats_vector_read_at (bmaxs, flist, fidx + 3);

		//Con_PrintVarTitleBBOX ("Floor initial size is ", imins, imaxs);
		if (bmaxs[2] == RECTUS_DELETED_0)
			continue; // Do not want ...


		//if (bmaxs[0] - bmins[0] == 0) continue; // Was deleted.
		Con_PrintLinef ("Floorage %d: (" VECTOR3BBOX_G ")",
			fidx / BBOX_6,
			VECTOR3BBOX_SEND( bmins, bmaxs)
		);
		if (!BOX_XY_Collide (bmins, bmaxs, emins, emaxs)) {
			zpr ("ENT# %d: does not collide with floorage %d", entnum, fidx / BBOX_6);
			continue;
		}
		zpr (CON_RED "ENT# %d: collision with floorage %d", entnum, fidx / BBOX_6);

		flist->floats[fidx + BBOX_6 - 1] = RECTUS_DELETED_0; // MARK AS DELETED
		zpr ("Floorage %d marked as deleted", fidx / BBOX_6);

		// Everyone is a niner
		// NW N NE
		// W  X  E
		// SW S SE
		// --------------------------
		// |       |       |        |
		// --------------------------
		// |       |       |        |
		// --------------------------
		// |       |       |        |
		// --------------------------


		vec3_t ne0 = { bmins[0], bmins[1], -1 }; // BBOXS NW
		vec3_t ne1 = { emins[0], emins[1], -1 }; // EMINS NW

		Floorage_WriteBox_Clip_To_DidWrite (flist, "ne", ne0, ne1, bmins, bmaxs);

		vec3_t nc0 = { emins[0], bmins[1], -1 }; // BBOX IS THE NORTH ONLY
		vec3_t nc1 = { emaxs[0], emins[1], -1 }; // Figure out Z at write time.

		Floorage_WriteBox_Clip_To_DidWrite (flist, "n", nc0, nc1, bmins, bmaxs);

		vec3_t nw0 = { emaxs[0], bmins[1], -1 }; // BBOX TOP AND RIGHT
		vec3_t nw1 = { bmaxs[0], emins[1], -1 }; //

		Floorage_WriteBox_Clip_To_DidWrite (flist, "nw", nw0, nw1, bmins, bmaxs);


		// --------------------------
		// |       |       |        |
		// --------------------------
		// |       |       |        |
		// --------------------------
		// |       |       |        |
		// --------------------------
//center:
  //  ;
		vec3_t cw0 = { bmins[0], emins[1], -1 }; // BBOX RIGHT ONLY
		vec3_t cw1 = { emins[0], emaxs[1], -1 }; //

		Floorage_WriteBox_Clip_To_DidWrite (flist, "w", cw0, cw1, bmins, bmaxs);

#if 0 // CENTER GOES AWAY
		vec3_t cc0 = { emins[0], emins[1], -1 }; // E FULL
		vec3_t cc1 = { emaxs[0], emaxs[1], -1 }; //

		WriteBox (flist, xx0, xx1);
#endif
		vec3_t ce0 = { emaxs[0], emins[1], -1 }; // BBOX RIGHT ONLY
		vec3_t ce1 = { bmaxs[0], emaxs[1], -1 }; //

		Floorage_WriteBox_Clip_To_DidWrite (flist, "e", ce0, ce1, bmins, bmaxs);
//south:
		vec3_t sw0 = { bmins[0], emaxs[1], -1 }; // BBOXS NW
		vec3_t sw1 = { emins[0], bmaxs[1], -1 }; // EMINS NW

		Floorage_WriteBox_Clip_To_DidWrite (flist, "sw", sw0, sw1, bmins, bmaxs);

		vec3_t sc0 = { emins[0], emaxs[1], -1 }; // BBOX IS THE NORTH ONLY
		vec3_t sc1 = { emaxs[0], bmaxs[1], -1 }; // Figure out Z at write time.

		Floorage_WriteBox_Clip_To_DidWrite (flist, "s", sc0, sc1, bmins, bmaxs);

		vec3_t se0 = { emaxs[0], emaxs[1], -1 }; // BBOX TOP AND RIGHT
		vec3_t se1 = { bmaxs[0], bmaxs[1], -1 }; //

		Floorage_WriteBox_Clip_To_DidWrite (flist, "se", se0, se1, bmins, bmaxs);

		//myprintf ("%4d: " FLOAT_LOSSLESS_FORMAT, idx, f);
	} // for idx

	Con_PrintLinef (" ");
	return 1;
}
#undef emins
#undef emaxs

/////

// Baker: July 7 2026 - 5:44 PM
int csge_angle_jitter (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _angle_jitter, MYKEY)
		vec3_t vec_jitter;
		map_get_vec3 (hm, entnum, "_origin_jitter", vec_jitter);

		vec3_t vec_angles = {0,0,0};
		map_get_vec3 (hm, entnum, "angles", vec_angles);

		float roll_0 = -1 + lhrandom(0, 1) * 2;
		float roll_1 = -1 + lhrandom(0, 1) * 2;
		float roll_2 = -1 + lhrandom(0, 1) * 2;

		vec_angles[0] += roll_0 * vec_jitter[0];
		vec_angles[1] += roll_1 * vec_jitter[1];
		vec_angles[2] += roll_2 * vec_jitter[2];

		map_set_vec3_lossless (hm, entnum, "angles", vec_angles);
		printvarv (vec_angles);

// REMOVE
		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_angle_jitter);
	for_each_end // entities in .map

	CSGM_END_
}

int csge_atomize (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	// WE ADD ENTITIES SO USE ORIGINAL COUNT - WE DELETE THE SOURCE ENTITY
	// CSG_CME__ATOMIZE
	for_each_entity_with_key_afterworld_original_2 (src_entnum, src_ent, hm, _atomize, MYKEY)
		int final_entnum_this = not_found_neg1;
		src_ent->is_considered_deleted = true;

		for_each_brush_2 (src_brushnum, r_brush, hm, src_entnum)
			// TURN THIS BRUSH INTO A NEW ENTITY
			int new_entnum = final_entnum_this = map_add_entity_empty(hm);

			map_copy_keys (hm, new_entnum, hm, src_entnum);

			map_add_brush_or_patch_copy (hm, new_entnum, hm, src_entnum, src_brushnum);

			CSGM_KEY_DELETE_ENT_ (new_entnum, csge_atomize);

			CSGM_NUM_DONE_PLUS_
		for_each_end // for_each_brush_or_patch

		// THIS ENTITY HAD ATOMIZATION IF -- final_entnum_this != not_found_neg1
		if (final_entnum_this != not_found_neg1) {
			// Mark final entity -- no real reason, but maybe there was a plan.
			map_set (hm, final_entnum_this, "final_entity", "1");
		} // final_entnum_this >= 0

	for_each_end // for each entity
	CSGM_END_
}

int csgw_brush_delete_if_texture (map_t *hm, ccs *MYKEY)
{
	CSGM_START_
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0 (_brush_delete_if_texture, MYKEY); // JACK fgd

	char texturename_wanted[Q3PATHLENGTH_64];

	// "_brush_delete_if_texture"  "cc0-textures/concrete_rock_path_global"
	space_split1 (_brush_delete_if_texture, texturename_wanted);

	for_each_entity_2(entnum, d_ent, hm)
		for_each_brush_2 (brushnum, d_brush, hm, entnum)
			if (false == brush_has_texturename(d_brush, texturename_wanted))
				continue; // Doesn't have this texture

			// HAS TEXTURE -- DELETE THE BRUSH
			d_brush->is_considered_deleted = true;
			CSGM_NUM_DONE_PLUS_
		for_each_end // next brush
	for_each_end // next entity

	CSGM_KEY_DELETE_WORLD_ (csgw_brush_delete_if_texture)
	CSGM_END_ // return
}



int csgw_csg_test (map_t *hm, ccs *MYKEY)
{
	CSGM_START_
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0(_rewrite, MYKEY);	// "_sky_encase" "8192 skies/desert2_sky"

	CSGM_KEY_DELETE_WORLD_(csgw_csg_test)

	CSGM_NUM_DONE_PLUS_
	CSGM_END_
}

RELATED_ (csgc_detail_auto _NO_DETAIL_AUTO)
int csgw_detail_auto (map_t *hm, ccs *MYKEY)
{
	CSGM_START_
	return csgc_detail_auto (hm);
	CSGM_END_
}


// magically create floor 
int csgw_floorage (map_t *hm, ccs *MYKEY)
{
	CSGM_START_
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0(_floorage, MYKEY);	// "_floorage" "8192 skies/desert2_sky"

	ENT_DEFINE (d_world, hm, WORLD_0);
	floatlist_s list_bboxes_6 = {0};

	Vector3DeclareCopyEx (imins, hm->bbentsmins);
	Vector3DeclareCopyEx (imaxs, hm->bbentsmaxs);
	imaxs[2] = RECTUS_ACTIVE_1;

	Con_PrintVarTitleBBOX ("Floor initial size is ", imins, imaxs); // LEAVE IT --- MOVE ALONG
	floats_add_vector (&list_bboxes_6, imins);
	floats_add_vector (&list_bboxes_6, imaxs);

	// NUMERIC ==> MAPS_SH_FANG_GRASS otherwise use supplied name.
	ccs *texturename = atoi (_floorage) > 0 ? MAPS_SH_FANG_GRASS : _floorage;

	CSGM_KEY_DELETE_WORLD_ (csgw_floorage) // Kill the _floorage

	// FIND ALL THE FUNC_GROUP_TERRAIN starting classnames
	for_each_entity_classname_startswith_afterworld_original_2 (entnum, r_ent, hm, FUNC_GROUP_TERRAIN)
		if (Vector3_IsZeros(r_ent->bedims)) {
			zpr ("Entity %d: " FUNC_GROUP_TERRAIN " with no size?  Ignored", entnum);
			continue; // No size == move along.
		}
		if (r_ent->bemins[2] >= 0) {
			zpr ("Entity %d: mins z is %g >= ground level z of 0, ignoring ...", entnum, r_ent->bemins[2]);
			continue; // Above ground entity.
		}

		zpr ("Entity %d is a %s .. checking ...", entnum, this_classname);

		_floorage_sub_collide_many (&list_bboxes_6, r_ent, entnum);
	for_each_end // FUNC_GROUP_TERRAIN // entities that are ... func_wall?  Baker: August 10 2026 that was the comment.


	// Add brushes of the size
	floatlist_s *flist = &list_bboxes_6;
	for_each_floatlista_by_step (fidx, flist, BBOX_6)
		vec3_t bmins, bmaxs;
		floats_vector_read_at (bmins, flist, fidx + 0); // MINS
		floats_vector_read_at (bmaxs, flist, fidx + 3); // MAXS
		if (bmaxs[2] == RECTUS_DELETED_0)
			continue; // Deleted if Z marked 0
		if (bmaxs[0] - bmins[0] <= 0)
			continue; // Was deleted.  Better not happen.
		bmins[2] = imins[2];
		bmaxs[2] = 0;
		entity_brushs_add_cubeoid_quadrant (
			d_world, FACE_CEIL_5, texturename,
			bmins, bmaxs, TESSEL_BIG_10000, COMMON_CAULK
		);

		CSGM_NUM_DONE_PLUS_
	for_each_end // floats by 6


	floats_freecontents (&list_bboxes_6);

	CSGM_END_
}

// "_clone_classname" "light_csqc_emitter,origin_z 81,spawnflags 1"
// Create an exact copy of the entity
// DEEPLAIR USES!  ALLOWS TORCH TO ALSO BECOME A LIGHT

RELATED_ (CSG_CME__CLONE_CLASSNAME)
// In Deeplair the torches have this.
// baker_temple5 uses light_csqc_emitter but they are manually placed.

int csge_clone_classname (map_t *hm, ccs *MYKEY) // CSG_CME__CLONE_CLASSNAME
{
	CSGM_START_

	for_each_entity_with_key_afterworld_2 (entnum_old, source_entity, hm, _clone_classname, MYKEY) // CSG_CME__CLONE_CLASSNAME

		//
		// DO IT "_clone_classname" "light_csqc_emitter,origin_z 81,spawnflags 1"

		// CLONE THE KEYS ========================================================================
		int entnum_new = map_add_entity_empty (hm); // Can't fail.
		int entnum_new2 = map_add_entity_empty (hm); // Can't fail.

		//#define source_entity UNSAFE FOOL
		ENT_DEFINE (new_entity, hm, entnum_new);
		ENT_DEFINE (new_entity2, hm, entnum_new2);
		new_entity2->is_considered_deleted = true; // Unless used below.

		CSGM_KEY_DELETE_ENT_ (entnum_old, csge_clone_classname);

		entity_copy_keys_except (new_entity, ENT_GET2(hm, entnum_old) /*source_entity*/, MYKEY); // Exclude "_clone_classname"
		entity_copy_keys_except (new_entity2, ENT_GET2(hm, entnum_old) /*source_entity*/, MYKEY); // Exclude "_clone_classname"

		// SET NEW ORIGIN KEY
		// THIS MIGHT TAKE A BRAZIER AND SPAWN A LIGHT ABOVE IT.
		//
		vec3_t current_origin; // We may move it later.
		qbool has_origin = map_get_vec3 (hm, entnum_old, "origin", current_origin);
		if (has_origin == false) {
			// Brush model like func_group -- get the center.
			//ENT_DEFINE (safe_entnum_old, hm, entnum_old);

			//VectorCenter (source_entity->bemaxs, source_entity->bemins, current_origin);
			VectorCenter (ENT_GET2(hm, entnum_old)->bemaxs, ENT_GET2(hm, entnum_old)->bemins, current_origin);
		}
		entity_set_vec3_g	(new_entity, "origin", current_origin); // WHY?

		// END GET ORIGIN ==============

		// PARSE ADJUSTMENTS: Split on commas
		stringlist_t list_comma_split = {0};

		stringlist_append_split	(&list_comma_split, _clone_classname, ",");

		if (String_Contains (_clone_classname, ";")) {
			Con_PrintLinef ("Major error ... you cannot have semi-colons in _clone_classname");
		}
		// First one is classname
		// 1. Clone it
		// 2. Copy all epairs (except _clone_classname)
		// 3. Adjust values
		// "_clone_classname" "light_csqc_emitter,origin_z 81,spawnflags 1"
		RELATED_ (VM_map_load map_load_from_string)

		// Baker: We start at 1 because 0 is classname and we just read it
		// light csqc_emitter,light_offset 0 50 30,emitter_offset 32  0  29,spawnflags 1
		for_each_stringlist_string(sidx, list_comma_split, sxy)
			char action_string[64], action_value[64];
			qbool ok = space_split2_only (sxy, action_string, action_value);
			if (sidx == 0) {
				// OK  - we will accept this
				// light csqc_emitter
				ccs *new_classname = sxy;
				if (strchr(new_classname, CHAR_SPACE_32)) {
					char s1[64], s2[64];
					// A Double
					qbool ok = space_split2_only (new_classname, s1, s2);
					entity_set (new_entity, "classname", s1);
					entity_set (new_entity2, "classname", s2);
					new_entity2->is_considered_deleted = false;
					continue;
				}

				entity_set (new_entity, "classname", new_classname);
				continue;
			}

			//if (String_Starts_With (sxy, "emitter_")) {
				//int j = 5 ;
			//}
			int numspaces = String_Find_Count_Whole_String(sxy, " ");
			if (numspaces == 3) {
				char actionus_string[64], actionus_vector_string[64];
				//qbool ok =
				space_split2_only (sxy, actionus_string, actionus_vector_string);
				vec3_t local_offset = {0};
				Math_atov3 (actionus_vector_string, local_offset);

				// 4 units -- so vector
				// light csqc_emitter,light_offset 0 50 30,emitter_offset 32  0  29,spawnflags 1
				// light,light_offset 0 50 30,emitter_offset 0 50 30,spawnflags 1

				// light_offset 0 50 30
				if (String_Match (actionus_string, "light_offset")) {
					//float adjust_y = atof(action_value);
					// origin_y += y
					vec3_t new_origin = {0}; entity_get_vec3 (new_entity, "origin", new_origin);

					entity_set_vec3_g (new_entity, "model_origin", new_origin);

					vec3_t angles = {0}; entity_get_vec3 (new_entity, "angles", angles);

					entity_set_vec3_g (new_entity, "model_angles", angles);
					entity_set_vec3_g (new_entity, "light_offset", local_offset);

					CloneClassname_CalcLightPosition (new_origin, angles, local_offset, new_origin);
					//VectorAdd (new_origin, vadd, new_origin);
					entity_set_vec3_g (new_entity, "origin", new_origin);
					zpr ("%s pivot " V3G, actionus_string, VS(local_offset));
					continue;
				}
				if (String_Match (actionus_string, "emitter_offset")) {
					vec3_t new_origin = {0}; entity_get_vec3 (new_entity2, "origin", new_origin);

					entity_set_vec3_g (new_entity2, "model_origin", new_origin);

					vec3_t angles = {0}; entity_get_vec3 (new_entity2, "angles", angles);

					entity_set_vec3_g (new_entity2, "model_angles", angles);
					entity_set_vec3_g (new_entity2, "emitter_offset", local_offset);

					CloneClassname_CalcLightPosition (new_origin, angles, local_offset, new_origin);

					//VectorAdd (new_origin, vadd, new_origin);
					entity_set_vec3_g (new_entity2, "origin", new_origin);
					zpr ("%s pivot " V3G, actionus_string, VS(local_offset));

					new_entity2->is_considered_deleted = false;
					continue;
				}

				zpr ("Unknown 4 parter " QUOTED_S " ignored ..", sxy);

				continue;
			}
			if (!ok) {
				zpr ("%s: Didn't have 2 strings for " QUOTED_S " ignoring ..", __func__, sxy);
				continue;
			}

			// EVAL
			// THIS IS SHITTY - NO -- Sep 26 2026
			// TUNNELS DEMO Only .map that used this -- KILL
			if (String_Match (action_string, "origin_y")) {
				float adjust_y = atof(action_value);
				// origin_y += y
				vec3_t new_origin = {0};
				entity_get_vec3 (new_entity, "origin", new_origin);
				new_origin[1] += adjust_y;
				entity_set_vec3_g (new_entity, "origin", new_origin);
				zpr ("New adjusted y entity origin set to ... " V3G, VS(new_origin));
			}

			// DEEP_LAIR.JMF
			// misc_model_torch
			// origin_adjust		0 0 -40
			// _clone_classname		light_csqc_emitter,origin_z 81,spawnflags 1
			// Baker: Deep lair moves the light up 81 z
			// But the QuakeC then drops light emitter 40 below that.
			else if (String_Match (action_string, "origin_z")) {
				float adjust_z = atof(action_value);
				// origin_z += z
				vec3_t new_origin = {0};
				entity_get_vec3 (new_entity, "origin", new_origin);
				new_origin[2] += adjust_z;
				entity_set_vec3_g (new_entity, "origin", new_origin);
				zpr ("New adjusted z entity origin set to ... " V3G, VS(new_origin));
			}
			else if (String_Match (action_string, "spawnflags") ) {
				ccs *s_spawnflags_value = action_value;

				entity_set (new_entity, "spawnflags", s_spawnflags_value);
			} else {
				zpr ("We have a command we don't know named " QUOTED_S " ignoring ..", action_string);
			}
		for_each_end // string list
		stringlistfreecontents (&list_comma_split);

		CSGM_NUM_DONE_PLUS_
	for_each_end // after_world

#undef source_entity
	CSGM_END_
}

RELATED_ (entitylist_perform_floorage_world_only)

// Find appropriate _decals and drop them to floor.  <==== how?
// Take center point and compare against all other brushes and entities.
// Highest below collision is where to plant the _decal
// keyword = "_automatic_droptofloor"
int csge_decal_automatic_droptofloor (map_t *hm, /*_automatic_droptofloor*/ ccs *MYKEY)
{
	CSGM_START_

	// We add entities
	for_each_entity_classname_afterworld_original_2 (decal_entnum, d_ent_decal, hm, "_decal")
		// "_automatic_droptofloor"
		qbool have_auto = map_get_keyval_does_match(hm, decal_entnum, "target", CSG_CME__AUTOMATIC_DROPTOFLOOR);

		// DO NOT PROCESS IF NOT REQUESTED
        if (have_auto == false)
            continue;
		RELATED_ (brush_or_patch_tranlate_with_texturelock_was_processed)
		Vector3DeclareCopyEx (org, d_ent_decal->becenter);

		if (Vector3_IsZeros(org)) {
			zpr ("#ent %d _decal couldn't find center, ignoring.", decal_entnum);
			continue;
		}

		zpr ("#ent %d _decal center is " VECTOR3_G, decal_entnum, VECTOR3_SEND(org));

		// COLLIDE WITH TRACELINE
		vec3_t collide_spot;
		qbool did_hit = map_traceline_drop_ok (hm, org, /*ignore ent*/ decal_entnum, collide_spot);

		if (!did_hit) {
			zpr ("#ent %d _decal at " V3G " did not hit anything below it", decal_entnum, VS(org));
			continue;
		}

		collide_spot[2] -= 0.5; // Move it down just a little.

		zpr ("#ent %d _decal at " V3G " droptofloor result = " V3G,
			decal_entnum,
			VS(org),
			VS(collide_spot)
		);

		// Create a point entity at collide
		va_super (targetname, 128, "_decal_target_%d", _num_done);

		int target_entnum = map_add (hm, "info_null");
			map_set_vec3_g	(hm, target_entnum, "origin", collide_spot);
			map_set			(hm, target_entnum, "targetname", targetname);

		// NOW SET DECAL TO TARGET THAT
		map_set (hm, decal_entnum, "target", targetname);

		CSGM_NUM_DONE_PLUS_
	for_each_end // each entity after world original count

	CSGM_END_
}

// keyword = "_drop_vector"
int csge_decal_drop_distance (map_t *hm,/*_drop_distance*/  ccs *MYKEY)
{
	// Find appropriate _decals and drop them to floor.  <==== how?
	// Take center point and compare against all other brushes and entities.
	// Highest below collision is where to plant the _decal
	CSGM_START_
	for_each_entity_classname_afterworld_original_2 (decal_entnum, d_ent_decal, hm, "_decal")

		vec3_t vorigin_adjust;
		qbool has_this = map_get_vec3 (hm, decal_entnum, "_drop_vector", vorigin_adjust);
		if (!has_this)
			continue;  // No drop vector

		RELATED_ (brush_or_patch_tranlate_with_texturelock_was_processed)
		Vector3DeclareCopyEx (vcenterof, d_ent_decal->becenter);

		zpr ("#ent %d _decal vcenterof is " VECTOR3_G, decal_entnum, VECTOR3_SEND(vcenterof));

		// Now what?
		vec3_t vtargetorigin;
		VectorAdd (vcenterof, vorigin_adjust, vtargetorigin); // center + adjust ==> vtargetspot

		zpr ("#ent %d _decal at " VECTOR3_G " adjusted to = " VECTOR3_G,
			decal_entnum,
			VECTOR3_SEND(vcenterof),
			VECTOR3_SEND(vtargetorigin)
		);

		// Create a point entity at collide
		va_super (targetname, 128, "_decal_target_%d", _num_done);
		int d_infonull_entnum = map_add (hm, "info_null");
			map_set_vec3_g	(hm, d_infonull_entnum, "origin", vtargetorigin);
			map_set			(hm, d_infonull_entnum, "targetname", targetname);

		// NOW WIRE UP THE TARGET
		map_set (hm, decal_entnum, "target", targetname);

		CSGM_NUM_DONE_PLUS_
	for_each_end // entities in .map
	CSGM_END_
}

RELATED_ (TEXSIDE_CEIL_16)
RELATED_ (map_carve_cubeoid)

map_t *map_carver_temp = NULL;
const brush_s *static_only_get_brush_id (int entnum, int brushnum)
{
	// What unit?
	BRUSH_DEFINE_CONST (brush, map_carver_temp, entnum, brushnum);
	return brush;
}

//static int stringlistsort_brush_volume_cmp(const void *a, const void *b)
//{
//    // Cast the generic void pointers directly to const char pointers
//    const char *s_a = *(const char **)a;
//    const char *s_b = *(const char **)b;
//
//	// Not so fast my friend.  Unit, entnum, brushnum
//	vec3_t id_a; Math_atov3_ex (s_a, id_a);
//	vec3_t id_b; Math_atov3_ex (s_b, id_b);
//
//	const brush_s *br_a = static_only_get_brush_id (/*entnum*/ id_a[0], /*brushnum*/ id_a[1]);
//	const brush_s *br_b = static_only_get_brush_id (/*entnum*/ id_b[0], /*brushnum*/ id_b[1]);
//
//	int64_t cubicunits_a = brush_volume_cubic_units (br_a);
//	int64_t cubicunits_b = brush_volume_cubic_units (br_b);
//
//	// Biggest to front.
//	// Descending sort: Large volumes move to the front
//    if (cubicunits_a < cubicunits_b) return 1;
//    if (cubicunits_a > cubicunits_b) return -1;
//    return 0;
//}

// DETERMINISTIC SORTING
static int stringlistsort_brush_volume_cmp(const void *a, const void *b)
{
    const char *s_a = *(const char **)a;
    const char *s_b = *(const char **)b;

    // Unit, entnum, brushnum
    vec3_t id_a; Math_atov3_ex(s_a, id_a);
    vec3_t id_b; Math_atov3_ex(s_b, id_b);

    const brush_s *br_a = static_only_get_brush_id(id_a[0], id_a[1]);
    const brush_s *br_b = static_only_get_brush_id(id_b[0], id_b[1]);

    int64_t cubicunits_a = brush_volume_cubic_units(br_a);
    int64_t cubicunits_b = brush_volume_cubic_units(br_b);

    // Biggest to front.
    if (cubicunits_a < cubicunits_b) return 1;
    if (cubicunits_a > cubicunits_b) return -1;

    // Tiebreaker: entity number, then brush number.
    if (id_a[0] < id_b[0]) return -1;
    if (id_a[0] > id_b[0]) return 1;

    if (id_a[1] < id_b[1]) return -1;
    if (id_a[1] > id_b[1]) return 1;

    return 0;
}

RELATED_ (csgax_includes)
void stringlist_brushes_by_volume(stringlist_t *list)
{
	RELATED_ (stringlistsort_brush_volume_cmp)
	stringlistsort_custom (list, fs_make_unique_false, stringlistsort_brush_volume_cmp);
}


int csgax_includes (map_t *hm, /*"#includes/"*/ ccs *MYKEY, ccs *mappath_realpath_full)
{
	CSGM_START_

	qbool has_one = map_find_did_find_brush_texture_starts_with (hm, CSG_CET_INCLUDES); // "#includes/"
	if (has_one == false)
		return 0;

	int32list_s		list_entnums_to_include = {0};
	stringlist_t	list2_includes_texturename_sinza = {0}; // texturename, sinza

	//stringlistsortcolumns_integer

	// Any entity > WORLD_0 that has an "#includes/" texture
	// We are denying world brushes
	// Fills d_ent->texturename_work

	// 1. FIND CUBES.  ADD TO LIST.  STORE SINZA TEXT. AND MARK DELETE THE CUBES.
	int num_includes = csgmsub_find_delete_includes (hm, MYKEY, mappath_realpath_full,
		&list_entnums_to_include,
		&list2_includes_texturename_sinza // texturename, sinza
	);

	//map_to_clipboard_debug_only (hm);
	stringlistsortcolumns_alpha (&list2_includes_texturename_sinza, /*numcolumns*/ 2, /*sortcolumn*/ 0);

	if (num_includes == 0)
		return 0;

	int num_carves = 0;

	stringlist_t list_carves_done = {0};
	// CARVE PHASE - TAKE EACH CUBE AND LOAD CONTENTS AND CARVE
	for_each_int32list (cutter_unit, list_entnums_to_include, my_armundo_cube_entnum)
		// NOW PERFORM THE CARVE
		ENT_DEFINE_CONST (r_ent_cubeoid_armundo_box, hm, my_armundo_cube_entnum);
		//ccs *texturename = r_ent->texturename_work;
		int idx = stringlist_find_index (&list2_includes_texturename_sinza, r_ent_cubeoid_armundo_box->texturename_work);
		ccs *sxy_mapstring = list2_includes_texturename_sinza.strings[idx + 1];
		map_t listmap_include = {0};
		// CENTERED XY?
		map_load_from_string		(&listmap_include, sxy_mapstring);

		vec3_t center;
		VectorCenter (listmap_include.bbentsmins, listmap_include.bbentsmaxs, center);

		// CENTER THE MAP
		if (center[0] || center[1]) {
			Vector3DeclareSet (center_move, -center[0], -center[1], 0);

			map_move (&listmap_include, center_move);
		}

		// ROTATE IF NEEDED
		float current_cube_rotation = -1;		
		qbool ok = map_brush_get_rotation_is_ok (hm, my_armundo_cube_entnum, not_found_neg1, &current_cube_rotation); // accepts brushnum -1 and converts to FACE_CEIL_5
		if (isin3 (current_cube_rotation, 90, 180, 270)) {
			vec3_t move = {0,0,0};
			Vector3DeclareSet (rotation, 0, /*yaw*/ current_cube_rotation, 0);
			map_rotate_map (&listmap_include, move, rotation);
		}

		// MOVE IT INTO POSITION - MOVE DOWN THE NEGATIVE DESCENT AS EXTRA
		// r_ent->bemins IS CUBE BOTTOM WEST SOUTH
		Vector3DeclareCopyEx (moveamt, r_ent_cubeoid_armundo_box->bemins);

		map_move_prefix_targetnames	(&listmap_include, moveamt, /*prefixnum*/ cutter_unit, NULL);

		map_metric_again (&listmap_include);

		// Carve the world from every brush in the include
		// Need entities that are brushes
// CARVE PREP
		stringlist_t list_carvers = {0};
		for_each_entity_2 (cutter_entnum, cutter_ent, (&listmap_include))
			for_each_brush_2 (cutter_brushnum, cutter_brush, (&listmap_include), cutter_entnum)

#if 0 // GERONIMO
				if (false == brush_has_texturename (cutter_brush, COMMON_CARVE)) continue;
#endif // GERONIMO
				if (cutter_brush->is_cubeoid == false)
					continue; // Don't carve these.

				qbool got_carve=brush_has_texturename (cutter_brush, COMMON_CARVE);
				if (got_carve) {
					// We good!
					//continue;
				} else {
					//map_clipboard_set_brush (&listmap_include, cutter_entnum, cutter_brushnum);
					//qbool has_carve = brush_has_texturename (cutter_brush, COMMON_CARVE);
					ccs *first = brush_texture_first_non_common (cutter_brush, NULL);
					if (!first) {
						
						
						continue; // All caulk or something.
					}
				}

				stringlistappendf (&list_carvers, "%d %d", cutter_entnum, cutter_brushnum);

			for_each_end // for_each_brush_or_patch
		for_each_end // for_each_brush_or_patch

		extern map_t *map_carver_temp;
		map_carver_temp = &listmap_include;

		void stringlist_brushes_by_volume(stringlist_t *list);
		stringlist_brushes_by_volume (&list_carvers);


		for_each_stringlist_string (n, list_carvers, sxy)
			vec3_t id_a; Math_atov3_ex (sxy, id_a);
			int cutter_entnum = id_a[0];
			int cutter_brushnum = id_a[1];
			BRUSH_DEFINE_CONST (cutter_brush, &listmap_include, cutter_entnum, cutter_brushnum);

#if 0 //def _DEBUG
			map_clipboard_set_brush (&listmap_include, cutter_entnum, cutter_brushnum);
#endif
			//int map_carve_cubeoid_against_any_bulk_test (map_t *hm, const brush_s *carve_brush_in, int cutter_instance, int cutter_entnum, int cutter_brushnum)

			//int num_made_here = map_carve_cubeoid_against_any_bulk_test (hm, cutter_brush, targetprefixnum, cutter_entnum, cutter_brushnum);
			int num_made_here = map_carve_cubeoid_against_any_bulk (hm, cutter_brush, cutter_unit, cutter_entnum, cutter_brushnum);
			if (num_made_here > 0) {
				stringlistappendf (&list_carves_done, "cutter_unit %d ent %d brush %d size " V3G " num carve collisions = %d",
					cutter_unit,
					cutter_entnum,
					cutter_brushnum,
					VS(cutter_brush->bxdims),
					num_made_here
					);

				CSGM_NUM_DONE_PLUS_
				zpr ("Cutter unit %d ent %d brush %d created %d brushes (some may be removed)",
					cutter_unit,
					cutter_entnum,
					cutter_brushnum,
					num_made_here);
				DebugPrintLinef ("Targ %d ent %d brush %d Cutter created %d",
					cutter_unit,
					cutter_entnum,
					cutter_brushnum,
					num_made_here);
				num_carves ++;
				//goto bail_fail_bad_carve;
			}


		for_each_end // stringlist

		stringlistfreecontents (&list_carvers); map_carver_temp = NULL;
		// Free the map
		map_freecontents		(&listmap_include);
	for_each_end // Each int32
bail_fail_bad_carve:
	zpr ("Num carve operations that carved: %d (brushes are sorted by size, so this can be quite low because big brushes clear the path)", num_carves);
	for_each_stringlist_string (n, list_carves_done, sxy)
		zpr ("%4d: %s", n, sxy);
	for_each_end
	stringlistfreecontents (&list_carves_done);
	//printvari (num_carves);
	debugprintvari (num_carves);

//goto carve_only_skip;

	// INCLUDE PHASE
	for_each_int32list (targetprefixnum, list_entnums_to_include, my_armundo_cube_entnum)
		// NOW PERFORM THE INCLUDE
		ENT_DEFINE_CONST (r_ent_cubeoid_armundo_box, hm, my_armundo_cube_entnum);
		//ccs *texturename = r_ent->texturename_work;
		int idx = stringlist_find_index (&list2_includes_texturename_sinza, r_ent_cubeoid_armundo_box->texturename_work);
		ccs *sxy_mapstring = list2_includes_texturename_sinza.strings[idx + 1];
		map_t listmap_include = {0}, *plist2 = &listmap_include;
		// CENTERED XY?
		map_load_from_string		(&listmap_include, sxy_mapstring);

		// DELETE THE CARVE BRUSHES IN CARVING MAP
		for_each_entity_2 (entnum, d_ent, plist2)
			for_each_brush_2 (brushnum, d_brush, plist2, entnum)
				if (brush_has_texturename (d_brush, COMMON_CARVE)) {
					d_brush->is_considered_deleted = true;
				}
			for_each_end // br
		for_each_end // ent

		vec3_t center;
		VectorCenter (listmap_include.bbentsmins, listmap_include.bbentsmaxs, center);

		// CENTER THE MAP
		if (center[0] || center[1]) {
			Vector3DeclareSet (center_move, -center[0], -center[1], 0);

			map_move (&listmap_include, center_move);
		}

		// ROTATE IF NEEDED
		float current_cube_rotation = -1;
		RELATED_ (Carve_Create_Result_Brush)
		qbool ok = map_brush_get_rotation_is_ok (hm, my_armundo_cube_entnum, not_found_neg1, &current_cube_rotation); // accepts brushnum -1 and converts to FACE_CEIL_5
		if (isin3 (current_cube_rotation, 90, 180, 270)) {
			vec3_t move = {0,0,0};
			Vector3DeclareSet (rotation, 0, /*yaw*/ current_cube_rotation, 0);
			map_rotate_map (&listmap_include, move, rotation);
		}

		// MOVE IT INTO POSITION - MOVE DOWN THE NEGATIVE DESCENT AS EXTRA
		// r_ent->bemins IS CUBE BOTTOM WEST SOUTH
		Vector3DeclareCopyEx (moveamt, r_ent_cubeoid_armundo_box->bemins);

		map_move_prefix_targetnames	(&listmap_include, moveamt, targetprefixnum, NULL);

		// Append the map
		map_append_map (hm, &listmap_include);

		// Free the map
		map_freecontents		(&listmap_include);
	for_each_end // Each int32

carve_only_skip:

	int32s_freecontents (&list_entnums_to_include);
	stringlistfreecontents (&list2_includes_texturename_sinza);
	CSGM_END_
}

int csge_keep_every_nth (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _nth, MYKEY)
		int our_modnum = atoi(_nth);
		printvari (our_modnum);

		int our_nthness = map_get_nthness_classnametargetname(hm, entnum);

		int result = our_nthness % /*mod*/ our_modnum; // 3 mod 4
		//printvari (result);

		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_keep_every_nth);

		if (result == 0) {
			zpr ("Keep! %d", entnum); // KEEP
			continue;
		}

		// REMOVE!  ROLL FAILED!
		zpr ("Removed! %d", entnum);
		d_ent->is_considered_deleted = true;
	for_each_end // entities in .map
	CSGM_END_
}

//
// Baker: July 6 2026 - originally named "odds_of_keeping"
//
RELATED_ (entitylist_perform_originmake_num_made)
int csge_keep_percent (map_t *hm, ccs *MYKEY)
{
	// For any entity with "odds_of_keeping" do a lhrandom roll
	// If fails, remove
	//
	CSGM_START_

	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _keep_percent, MYKEY)
		float roll_above_this_eliminates = atof (_keep_percent);
		zpr ("Requires a roll less than %f", roll_above_this_eliminates);

		// Now what?  Check for
		float myroll = lhrandom(0, 1);

// REMOVE
		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_keep_percent);

		if (myroll <= roll_above_this_eliminates)
			continue; // KEEP because it rolled under the cutoff number.

		// REMOVE!  ROLL FAILED!
		entitylist_remove_odds_entity (hm, d_ent);
	for_each_end // entities in .map
	CSGM_END_
} // NEW

int csge_light_range (map_t *hm, ccs *MYKEY) // _light_range
{
	CSGM_START_
	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _light_range, MYKEY)
		vec3_t light3 = {0};
		qbool is_alternating = String_Starts_With  (_light_range, "alt");
		int my_lightf;
		if (is_alternating) {
			// "_light_range "alt 0 200"
			ccs *safter = &_light_range[3];
			Math_atov3 (safter, light3);
			float myrollf = lhrandom(0, 1);
			my_lightf = (myrollf >= 0.5) ? light3[1] : light3[0];
		} else {
			// "_light_range 0 200"
			Math_atov2 (_light_range, light3);
			float myrollf = lhrandom(0, 1);
			int low_light = light3[0];
			int hi_light = light3[1];
			int my_range = hi_light - low_light;
			my_lightf = low_light + my_range * myrollf;
		}

		map_set_float (hm, entnum, "light", my_lightf);
		printvari (my_lightf);
// REMOVE
		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_light_range);
	for_each_end // entities in .map

	CSGM_END_
}


// Baker: July 7 2026 - 5:31 PM
RELATED_ (CSG_CME__MODELSCALE_RANGE) // Entity key
int csge_modelscale_range (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _modelscale_range, MYKEY)
		vec3_t low_high_vec = {0};
		Math_atov2 (_modelscale_range, low_high_vec);
		float myrollf = lhrandom(0, 1);
		int lo = low_high_vec[0];
		int hi = low_high_vec[1];
		int my_range = hi - lo;
		float my_modelscale = lo + my_range * myrollf;

		map_set_float (hm, entnum, "modelscale", my_modelscale);
		printvarf (my_modelscale);
// REMOVE
		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_modelscale_range);
	for_each_end // entities in .map

	CSGM_END_
}


// Baker: July 7 2026 - 5:31 PM
int csge_origin_jitter (map_t *hm, ccs *MYKEY)
{
#pragma message ("TODO: This can't work on health box I don't think")
	CSGM_START_

	for_each_entity_with_key_original_2 (entnum, d_ent, hm, _origin_jitter, MYKEY)
		vec3_t vec_jitter;
		map_get_vec3 (hm, entnum, "_origin_jitter", vec_jitter);

		vec3_t new_origin = {0,0,0};
		map_get_vec3 (hm, entnum, "origin", new_origin);

		float roll_0 = -1 + lhrandom(0, 1) * 2;
		float roll_1 = -1 + lhrandom(0, 1) * 2;
		float roll_2 = -1 + lhrandom(0, 1) * 2;

		new_origin[0] += roll_0 * vec_jitter[0];
		new_origin[1] += roll_1 * vec_jitter[1];
		new_origin[2] += roll_2 * vec_jitter[2];

		map_set_vec3_lossless (hm, entnum, "origin", new_origin);
		printvarv (new_origin);
// REMOVE
		CSGM_NUM_DONE_PLUS_
		CSGM_KEY_DELETE_ENT_ (entnum, csge_origin_jitter);
	for_each_end // entities in .map

	CSGM_END_
}





int csge_originmake (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	for_each_entity_with_key_afterworld_2 (entnum, d_ent, hm, _originmake, MYKEY) // CSG_CME__ORIGINMAKE
		// Key found - add an origin brush the size of the entity -- then remove the key
		brush_add_cubeoid_with_texture (d_ent, d_ent->bemins, d_ent->bemaxs, COMMON_ORIGIN);
		map_key_delete (hm, entnum, CSG_CME__ORIGINMAKE);
		CSGM_NUM_DONE_PLUS_
	for_each_end // foreach

	CSGM_END_
}


// "_rename misc_model_torch=misc_model,misc_mdl_entity_kickable=misc_mdl_entity"
int csgw_rename_GARUX (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	// Get the "_rename" in entity #0 - find all of them and replace them.
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0 (_rename, MYKEY);

	stringlist_t list_rename_eq_pairs = {0}; // "misc_model_torch=misc_model,misc_mdl_entity_kickable=misc_mdl_entity"
	stringlist_append_split	(&list_rename_eq_pairs, _rename, ",");

	// Work each comma element, splitting it again by "=" and work that entity.
	for_each_stringlist_string (n, list_rename_eq_pairs, old_eq_new_entry)
		char oldclassname[64], newclassname[64];

		qbool ok = delimiter_split2 (old_eq_new_entry, "=", oldclassname, newclassname);
		if (!ok) {
			zpr ("Unexpected number of strings for split of %s, expected >= 2", old_eq_new_entry);
			break; // Expected 2 or more
		}
		int num_done_here = map_afterworld_classname_rename_all (hm, oldclassname, newclassname);

		CSGM_NUM_DONE_PLUS_EQ(num_done_here);
	for_each_end // Each string

	stringlistfreecontents (&list_rename_eq_pairs);
	CSGM_KEY_DELETE_WORLD_ (csgw_rename_GARUX) // Delete the _rename key
	CSGM_END_
}

RELATED_ (csgw_sky_encase)
// wood_bridge_titl_snow
//CSG_CMW__SKY_DECAL
//               dens z drop   z fall  texturename                    w   h
// "_sky_decal" "3    200       -32    textures/decals_sh/dirtystain1 512 512"
// [Density per 512x512] [drop z] [drop amount negative] [textures/texturename] [width] [height]
// Density is how many drops per 512x512 square.
// Must occur after sky encase?
int csgw_sky_decal (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	// "_sky_decal" "3 200 -32 textures/decals_sh/dirtystain1 512 512"
	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0(_sky_decal, MYKEY);

	// 1. Get size of the map
	Vector3DeclareCopyEx (worldmins, hm->bbentsmins);
	Vector3DeclareCopyEx (worldmaxs, hm->bbentsmaxs);
	Vector3DeclareCopyEx (worldsize, hm->bbentsdims);

	double square_area = hm->bbentsdims[0] * hm->bbentsdims[1];
	float num_sectors = square_area / (512 * 512); // Num places

	// FOR STARTERS:  We will
	// "_sky_decal 8192 anything" [density] [drop location] [drop depth negative number]
	// density is number of units per 512 x 512 square on average.
	// _sky_decal 3 200 40
	char s0_density[1024], s1_ztop[Q3PATHLENGTH_64], s2_zbelow[Q3PATHLENGTH_64], s3_texturesname[Q3PATHLENGTH_64], s4_sizex[Q3PATHLENGTH_64], s5_sizey[Q3PATHLENGTH_64];
	qbool isok = space_split6 (_sky_decal, s0_density, s1_ztop, s2_zbelow, s3_texturesname, s4_sizex, s5_sizey);

	float density = atof(s0_density); // 3
	float ztop = atof(s1_ztop); // 200
	float below = atof(s2_zbelow); // -32
	// s3_texturesname

	int num_decal_drops = num_sectors * density;

	// [Density per 512x512] [drop z] [drop amount negative] [textures/texturename] [width] [height]
	// 3 200 -32 textures/decals_sh/dirtystain1 512 512
	RELATED_ (_parsemap_readfaces_patchblock_isok)

	for (int n = 0; n < num_decal_drops; n ++) {
		float x = worldmins[0] + lhrandom(0, 1) * worldsize[0];
		float y = worldmins[1] + lhrandom(0, 1) * worldsize[1];
		float z = ztop;

		// "target" "_automatic_droptofloor"

		zpr ("%d: _decal drop at %g %g %g", n, x, y, z);
		int entnum = _csgw_sky_decal_add_decal_512_256_with_texturename (hm, s3_texturesname);

		vec3_t move_amount = {x, y, 0};
		map_entity_translate_with_metric (hm, entnum, move_amount);

		RELATED_ (map_traceline_drop_ok)
		map_set (hm, entnum, "target", CSG_CME__AUTOMATIC_DROPTOFLOOR); // "_automatic_droptofloor"

		CSGM_NUM_DONE_PLUS_
	}

	CSGM_KEY_DELETE_WORLD_ (csgw_sky_decal)
	CSGM_END_
}


int csgw_sky_encase (map_t *hm, ccs *MYKEY)
{
	CSGM_START_

	CSGM_KEY_DEFINE_WORLD_OR_ABORT_0(_sky_encase, MYKEY);	// "_sky_encase" "8192 skies/desert2_sky"

	char s0_wallheight[1024], s1_texturename[Q3PATHLENGTH_64];
	qbool isok = space_split2 (_sky_encase, s0_wallheight, s1_texturename);

	if (!isok) {
		zpr ("%s: Parse 2 strings failed on %s .. defaulting desert2 8192..", __func__, _sky_encase);
		c_strlcpy (s0_wallheight, STRINGIFY(SKY_HEIGHT_8192) );
		c_strlcpy (s1_texturename, "skies/desert2_sky");
	}

	int num_done_here = csgc_sky_encase (hm, s0_wallheight, s1_texturename);
	CSGM_NUM_DONE_PLUS_EQ (num_done_here);
	CSGM_KEY_DELETE_WORLD_ (csgw_sky_encase)
	CSGM_END_
}


// sv_phys_push.c.h

/*
============
SV_PushEntity_Did_Avoid_Teleport

Does not change the entities velocity at all
The trace struct is filled with the trace that has been done.
Returns true if the push did not result in the entity being teleported by QC code.
============
*/
// Baker: Returns true if move failed
RELATED_ (SV_WalkMove)
CALLERS_ (SV_FlyMove)
// July 18 2026 - Failure is here or deeper.
static qbool SV_PushEntity_Did_Avoid_Teleport (trace_t *trace, prvm_edict_t *ent, vec3_t pushvector, qbool dolink, qbool checkstuck)
{
	prvm_prog_t *prog = SVVM_prog;
	int solid;
	int movetype;
	int type;
	vec3_t minsvector, maxsvector;
	vec3_t startvector;
	vec3_t endvector;

#ifdef _DEBUG // July 18 2026
	int entnum = PRVM_NUM_FOR_EDICT(ent);
	//Con_PrintLinef ("%s: %d", __func__, entnum);
#endif

	solid = (int)PRVM_serveredictfloat(ent, solid);
	movetype = (int)PRVM_serveredictfloat(ent, movetype);
	VectorCopy(PRVM_serveredictvector(ent, mins), minsvector);
	VectorCopy(PRVM_serveredictvector(ent, maxs), maxsvector);

	// move start position out of solids
	if (sv_gameplayfix_nudgeoutofsolid.integer /*d:0*/ && sv_gameplayfix_nudgeoutofsolid_separation.value >= 0) {
		// Baker: This is not the norm
		SV_NudgeOutOfSolid(ent);
	}

	VectorCopy(PRVM_serveredictvector(ent, origin), startvector);

	// Baker: So what is pushvector?  Is it player origin + speed?  Or is ground entity transfer of speed.
	VectorAdd(startvector, pushvector, endvector);

	if (movetype == MOVETYPE_FLYMISSILE_9)
		type = MOVE_MISSILE_2;
	else if (movetype == MOVETYPE_FLY_WORLDONLY_33)
		type = MOVE_WORLDONLY_3;
	else if (solid == SOLID_TRIGGER_1 || solid == SOLID_NOT_0)
		type = MOVE_NOMONSTERS_1; // only clip against bmodels
	else
		type = MOVE_NORMAL_0;

	//if (ent is player and ground entity and it movetype push) {
	//	collision_extendmovelength +
	//}
	// MAJORUS2
	*trace = SV_TraceBox(startvector, minsvector, maxsvector, endvector, type, ent,
		SV_GenericHitSuperContentsMask(ent), 0, 0, collision_extendmovelength.value /*d:16*/);


	// fail the move if stuck in world
	if (trace->worldstartsolid)
		return true;

#if 0 //def _DEBUG // July 18 2026
	printvarf (trace->fraction); // do link 0 is player phyiscs otherwise entity.
	if (trace->fraction == 0) {
		int j = 5; // CLIENT ENTITY SV_Physics->SV_WalkMove->SV_FlyMove->SV_PushEntity entnum 1
	} else {
		int j = 5; // SV_Physics->SV_Physics_Entity->SV_Pusher->SV_PushMove->SV_PushEntity physent 62 entnum 1
	}
#endif

	VectorCopy(trace->endpos, PRVM_serveredictvector(ent, origin));

	ent->priv.required->mark = PRVM_EDICT_MARK_WAIT_FOR_SETORIGIN; // -2: setorigin running

	SV_LinkEdict(ent);

	if (sv_gameplayfix_fiendjumpfix.integer == 0) {
		// NORMAL
		if (dolink)
			SV_LinkEdict_TouchAreaGrid(ent);

		if ((PRVM_serveredictfloat(ent, solid) >= SOLID_TRIGGER_1 && trace->ent && (!((int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512) || PRVM_serveredictedict(ent, groundentity) != PRVM_EDICT_TO_PROG(trace->ent))))
			SV_Impact (ent, trace);
	} else {
		// THIS IS *******NOT*** THE NORM
		if (dolink)
		{
			SV_LinkEdict_TouchAreaGrid(ent);

			if ((PRVM_serveredictfloat(ent, solid) >= SOLID_TRIGGER_1 && trace->ent && (!((int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512) || PRVM_serveredictedict(ent, groundentity) != PRVM_EDICT_TO_PROG(trace->ent))))
				SV_Impact (ent, trace);
		}
	}

	if (ent->priv.required->mark == PRVM_EDICT_MARK_SETORIGIN_CAUGHT)
	{
		ent->priv.required->mark = 0;
		return false;
	}
	else if (ent->priv.required->mark == PRVM_EDICT_MARK_WAIT_FOR_SETORIGIN)
	{
		ent->priv.required->mark = 0;
		return true;
	}
	else
	{
		Con_PrintLinef ("The edict mark had been overwritten! Please debug this.");
		return true;
	}
}


/*
============
SV_PushMove

============
*/
RELATED_ (SV_NudgeOutOfSolid_PivotIsKnownGood)
static void SV_PushMove (prvm_edict_t *pusher, float movetime)
{
	prvm_prog_t *prog = SVVM_prog;
	int i, e;
	int pusherowner, pusherprog;
	int checkcontents;
	float savesolid, movetime2, pushltime;
	vec3_t mins, maxs, move, move1, moveangle, pushorig, pushang, a, forward, left, up, org, pushermins, pushermaxs, checkorigin, checkmins, checkmaxs;
	int num_moved;
	int numcheckentities;
	static prvm_edict_t *checkentities[MAX_EDICTS_32768];

	trace_t trace, trace2;
	matrix4x4_t pusherfinalmatrix, pusherfinalimatrix;
	static unsigned short moved_edicts[MAX_EDICTS_32768];

#ifdef _DEBUG // July 18 2026
	int entnum = PRVM_NUM_FOR_EDICT(pusher);  //Con_PrintLinef ("%s: %d", __func__, entnum);
#endif
	vec3_t pivot;

	if (!PRVM_serveredictvector(pusher, velocity)[0] && !PRVM_serveredictvector(pusher, velocity)[1] && !PRVM_serveredictvector(pusher, velocity)[2] && !PRVM_serveredictvector(pusher, avelocity)[0] && !PRVM_serveredictvector(pusher, avelocity)[1] && !PRVM_serveredictvector(pusher, avelocity)[2]) {
		// Baker: No movement ... get out.
		PRVM_serveredictfloat(pusher, ltime) += movetime;
		return;
	}

	switch ((int) PRVM_serveredictfloat(pusher, solid))
	{
	// LadyHavoc: valid pusher types
	case SOLID_BSP_4:
	case SOLID_BBOX_2:
	case SOLID_SLIDEBOX_3:
	case SOLID_CORPSE_5: // LadyHavoc: this would be weird...
		break;

	// LadyHavoc: no collisions
	case SOLID_NOT_0:
	case SOLID_TRIGGER_1:
		VectorMA (PRVM_serveredictvector(pusher, origin), movetime, PRVM_serveredictvector(pusher, velocity), PRVM_serveredictvector(pusher, origin));
		VectorMA (PRVM_serveredictvector(pusher, angles), movetime, PRVM_serveredictvector(pusher, avelocity), PRVM_serveredictvector(pusher, angles));
		PRVM_serveredictvector(pusher, angles)[0] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[0] * (1.0 / 360.0));
		PRVM_serveredictvector(pusher, angles)[1] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[1] * (1.0 / 360.0));
		PRVM_serveredictvector(pusher, angles)[2] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[2] * (1.0 / 360.0));
		PRVM_serveredictfloat(pusher, ltime) += movetime;
		SV_LinkEdict(pusher);
		return;
	default:
		Con_PrintLinef ("SV_PushMove: entity #%d, unrecognized solid type %f", PRVM_NUM_FOR_EDICT(pusher), PRVM_serveredictfloat(pusher, solid));
		return;
	}
	int index = (int) PRVM_serveredictfloat(pusher, modelindex);
	if (index < 1 || index >= MAX_MODELS_8192) {
		Con_PrintLinef ("SV_PushMove: entity #%d has an invalid modelindex %f", PRVM_NUM_FOR_EDICT(pusher), PRVM_serveredictfloat(pusher, modelindex));
		return;
	}
	model_t *pushermodel = SV_GetModelByIndex(index);
	pusherowner = PRVM_serveredictedict(pusher, owner);
	pusherprog = PRVM_EDICT_TO_PROG(pusher); // Baker: It's getting ent number

	qbool is_rotated = VectorLength2(PRVM_serveredictvector(pusher, angles)) + VectorLength2(PRVM_serveredictvector(pusher, avelocity)) > 0;

	movetime2 = movetime;
	VectorScale(PRVM_serveredictvector(pusher, velocity), movetime2, move1); // TRAINOIC
	VectorScale(PRVM_serveredictvector(pusher, avelocity), movetime2, moveangle);
	if (moveangle[0] || moveangle[2]) {
		// Baker: Tilted by pitch or roll.
		for (i = 0; i < 3; i ++) {
			if (move1[i] > 0)
			{
				mins[i] = pushermodel->rotatedmins[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->rotatedmaxs[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			}
			else
			{
				mins[i] = pushermodel->rotatedmins[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->rotatedmaxs[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			}
		}
	}
	else if (moveangle[1]) {
		// Baker: Only rotated by yaw
		for (i = 0;i < 3;i++) {
			if (move1[i] > 0) {
				mins[i] = pushermodel->yawmins[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->yawmaxs[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			}
			else
			{
				mins[i] = pushermodel->yawmins[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->yawmaxs[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			}
		} // dimension
	}
	else {
		// Baker: No angles at all.
		for (i = 0;i < 3; i ++) {
			if (move1[i] > 0) {
				// Baker: Positive move for this dimension
				mins[i] = pushermodel->normalmins[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->normalmaxs[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			} else {
				// Baker: Negative move or zero for this dimension
				mins[i] = pushermodel->normalmins[i] + move1[i] + PRVM_serveredictvector(pusher, origin)[i] - 1;
				maxs[i] = pushermodel->normalmaxs[i] + PRVM_serveredictvector(pusher, origin)[i] + 1;
			}
		} // dimension
	}

	VectorNegate (moveangle, a);
	AngleVectorsFLU (a, forward, left, up);

	VectorCopy (PRVM_serveredictvector(pusher, origin), pushorig);
	VectorCopy (PRVM_serveredictvector(pusher, angles), pushang);
	pushltime = PRVM_serveredictfloat(pusher, ltime);

// move the pusher to its final position
	VectorMA (PRVM_serveredictvector(pusher, origin), movetime, PRVM_serveredictvector(pusher, velocity), PRVM_serveredictvector(pusher, origin));
	VectorMA (PRVM_serveredictvector(pusher, angles), movetime, PRVM_serveredictvector(pusher, avelocity), PRVM_serveredictvector(pusher, angles));
	PRVM_serveredictfloat(pusher, ltime) += movetime;
	SV_LinkEdict(pusher);

	// Baker: Why are we getting the model again?  Could it change?
	model_t *pushermodel2 = SV_GetModelFromEdict(pusher);
	if (pushermodel2 != pushermodel)
		pushermodel = pushermodel2;
	Matrix4x4_CreateFromQuakeEntity(&pusherfinalmatrix, PRVM_serveredictvector(pusher, origin)[0], PRVM_serveredictvector(pusher, origin)[1], PRVM_serveredictvector(pusher, origin)[2], PRVM_serveredictvector(pusher, angles)[0], PRVM_serveredictvector(pusher, angles)[1], PRVM_serveredictvector(pusher, angles)[2], 1);
	Matrix4x4_Invert_Simple(&pusherfinalimatrix, &pusherfinalmatrix);

	savesolid = PRVM_serveredictfloat(pusher, solid); // Baker: was SOLID_BSP_4 but where does it change?  Line 362 to solid not.

// see if any solid entities are inside the final position
	num_moved = 0;

	if (PRVM_serveredictfloat(pusher, movetype) == MOVETYPE_FAKEPUSH_13) // Tenebrae's MOVETYPE_PUSH_7 variant that doesn't push...
		numcheckentities = 0;
	else { // MOVETYPE_PUSH_7
		if (collision_train_extend_box_amount.value != 0) {
			float amt = collision_train_extend_box_amount.value;
			vec3_t paddedmins, paddedmaxs;
			VectorSet(paddedmins, mins[0] - amt, mins[1] - amt, mins[2] - amt);
			VectorSet(paddedmaxs, maxs[0] + amt, maxs[1] + amt, maxs[2] + amt);
			//prvm_edict_t *player1 = PRVM_EDICT_NUM(1);//PRVM_EDICT_NUM(1);

			numcheckentities = SV_EntitiesInBox(paddedmins, paddedmaxs, MAX_EDICTS_32768, /*results to this*/ checkentities);
		} else {
			numcheckentities = SV_EntitiesInBox(mins, maxs, MAX_EDICTS_32768, /*results to this*/ checkentities);
		}
	}

#if 0
	//Con_PrintVarInt (numcheckentities);

#ifdef _DEBUG
	if (1) {
		qbool is_a_player_in_list = false;
		for (e = 0;e < numcheckentities;e++) {
			prvm_edict_t *check = checkentities[e];

			int check_entnum = PRVM_NUM_FOR_EDICT(check);  //Con_PrintLinef ("%s: %d", __func__, entnum);

			if (check_entnum == 1)
				is_a_player_in_list = true;
		}

		prvm_edict_t *player1 = PRVM_EDICT_NUM(1);//PRVM_EDICT_NUM(1);
		vec3_t playerabsmins, playerabsmaxs;
			//PRVM_serveredictvector(ed, absmin), PRVM_serveredictvector(ed, absmax)
		VectorCopy (PRVM_serveredictvector(player1, absmin), playerabsmins);
		VectorCopy (PRVM_serveredictvector(player1, absmax), playerabsmaxs);
			Con_PrintVarVector3 (mins);
			Con_PrintVarVector3 (maxs);
			Con_PrintVarVector3 (playerabsmins);
			Con_PrintVarVector3 (playerabsmaxs);

			if (is_a_player_in_list == false) {
				int j = 5;
			}
			// In our failure, the player is too far north.
			if (scratch1.value == 1) {
				checkentities[numcheckentities] = player1; //PRVM_EDICT_NUM(1);
				prvm_edict_t *groundentity4 = PRVM_PROG_TO_EDICT(PRVM_serveredictedict(player1, ENT_groundentity));
				int groundentity_entnum = PRVM_NUM_FOR_EDICT(groundentity4);

				Con_PrintVarInt (groundentity_entnum);
				if (is_a_player_in_list == false && groundentity4 && groundentity4 == pusher) {
					checkentities[numcheckentities] = player1; //PRVM_EDICT_NUM(1);
					numcheckentities ++;
				} else {

				}
			}

		Con_PrintVarInt (is_a_player_in_list);
	} // scratch1
#endif
#endif
	//

	for (e = 0;e < numcheckentities;e++) {
		prvm_edict_t *check = checkentities[e];
#ifdef _DEBUG // July 18 2026
		int check_entnum = PRVM_NUM_FOR_EDICT(check);  //Con_PrintLinef ("%s: %d", __func__, entnum);
#endif

		int movetype = (int)PRVM_serveredictfloat(check, movetype);
		switch(movetype) {
		case MOVETYPE_NONE_0:
		case MOVETYPE_PUSH_7:
		case MOVETYPE_FOLLOW_12:
		case MOVETYPE_NOCLIP_8:
		case MOVETYPE_FLY_WORLDONLY_33:
			continue;
		default:
			break;
		}

		if (PRVM_serveredictedict(check, owner) == pusherprog)
			continue;

		if (pusherowner == PRVM_EDICT_TO_PROG(check))
			continue;

		//Con_PrintLinef ("%d %s ", PRVM_NUM_FOR_EDICT(check), PRVM_GetString(PRVM_serveredictstring(check, classname)));

		// tell any MOVETYPE_STEP_4 entity that it may need to check for water transitions
		check->priv.server->waterposition_forceupdate = true;

		checkcontents = SV_GenericHitSuperContentsMask(check);

		// if the entity is standing on the pusher, it will definitely be moved
		// if the entity is not standing on the pusher, but is in the pusher's // ZMOVE_WARP
		// final position, move it.
		prvm_edict_t *groundentityx = PRVM_PROG_TO_EDICT(PRVM_serveredictedict(check, ENT_groundentity));
		//int groundentity_entnum = PRVM_NUM_FOR_EDICT(groundentity4);
		//printvari (groundentity_entnum);

		if (!((int)PRVM_serveredictfloat(check, flags) & FL_ONGROUND_512) ||
			groundentityx != pusher) {
			VectorCopy(PRVM_serveredictvector(pusher, mins), pushermins);
			VectorCopy(PRVM_serveredictvector(pusher, maxs), pushermaxs);
			VectorCopy(PRVM_serveredictvector(check, origin), checkorigin);
			VectorCopy(PRVM_serveredictvector(check, mins), checkmins);
			VectorCopy(PRVM_serveredictvector(check, maxs), checkmaxs);
			Collision_ClipToGenericEntity(&trace, pushermodel, pusher->priv.server->frameblend,
				&pusher->priv.server->skeleton,
				pushermins, pushermaxs, SUPERCONTENTS_BODY,
				&pusherfinalmatrix, &pusherfinalimatrix,
				checkorigin, checkmins, checkmaxs,
				checkorigin, checkcontents, SUPERCONTENTS_SKIP_NONE_0, MATERIALS_SKIP_NONE_0, collision_extendmovelength.value);

// SECONDUS
secondus:
			//trace = SV_TraceBox(PRVM_serveredictvector(check, origin), PRVM_serveredictvector(check, mins), PRVM_serveredictvector(check, maxs), PRVM_serveredictvector(check, origin), MOVE_NOMONSTERS_1, check, checkcontents);
			if (!trace.startsolid) {
				// Baker: The Half-Life train fall out hits here
				//Con_PrintLinef ("check entity %d- not in solid", check_entnum);
				continue;
			}
		}

		VectorLerp(PRVM_serveredictvector(check, mins), 0.5f, PRVM_serveredictvector(check, maxs), pivot);
		//VectorClear(pivot);

		if (is_rotated) { // Baker: Any pusher angles and any avelocity will do this.
			vec3_t org2;
			VectorSubtract (PRVM_serveredictvector(check, origin), PRVM_serveredictvector(pusher, origin), org);
			VectorAdd (org, pivot, org);
			org2[0] = DotProduct (org, forward);
			org2[1] = DotProduct (org, left);
			org2[2] = DotProduct (org, up);
			VectorSubtract (org2, org, move);
			VectorAdd (move, move1, move);
		}
		else
			VectorCopy (move1, move);

		//Con_Printf ("- pushing %f %f %f\n", move[0], move[1], move[2]);

		VectorCopy (PRVM_serveredictvector(check, origin), check->priv.server->moved_from); // Baker
		VectorCopy (PRVM_serveredictvector(check, angles), check->priv.server->moved_fromangles);
		moved_edicts[num_moved] = PRVM_NUM_FOR_EDICT(check);

		// Baker: edict 1 is player 1 or first player
		if (in_range (1, moved_edicts[num_moved], svs.maxclients)) {
			int client_idnum = moved_edicts[num_moved] - 1;
			client_t *hcl = &svs.clients[client_idnum];
			SV_PhysicsX_Zircon_Warp_Start (hcl, "pushed");
			//Con_PrintLinef ("Edict pushed player");
		} else {
			//Con_PrintLinef ("Edict pushed that isn't a player");
		}

		num_moved ++; // Baker: Moved

		// physics objects need better collisions than this code can do
		if (movetype == MOVETYPE_PHYSICS_32) {
			// Baker: ODE who cares.
			VectorAdd(PRVM_serveredictvector(check, origin), move, PRVM_serveredictvector(check, origin));
			SV_LinkEdict(check);
			SV_LinkEdict_TouchAreaGrid(check);
			continue;
		}

		// try moving the contacted entity
		PRVM_serveredictfloat(pusher, solid) = SOLID_NOT_0; // THIS IS WHERE IT CHANGES (LINE)

		if (!SV_PushEntity_Did_Avoid_Teleport(&trace, check, move, true, true)) { // TRAINOIC FRACTION 1 DO LINK TRAIN ENTITY
			// entity "check" got teleported
			PRVM_serveredictvector(check, angles)[1] += trace.fraction * moveangle[1];
			PRVM_serveredictfloat(pusher, solid) = savesolid; // was SOLID_BSP_4
			continue; // pushed enough
		}
		// FIXME: turn players specially
		PRVM_serveredictvector(check, angles)[1] += trace.fraction * moveangle[1]; RELATED_(PLAYER_xturn PLAYER_xflags)
		PRVM_serveredictfloat(pusher, solid) = savesolid; // was SOLID_BSP_4 .. it gets changed to SOLID_NOT on line 362
		//Con_Printf ("%s:%d frac %f startsolid %d bmodelstartsolid %d allsolid %d\n", __FILE__, __LINE__, trace.fraction, trace.startsolid, trace.bmodelstartsolid, trace.allsolid);

		// this trace.fraction < 1 check causes items to fall off of pushers
		// if they pass under or through a wall
		// the groundentity check causes items to fall off of ledges
		if (PRVM_serveredictfloat(check, movetype) != MOVETYPE_WALK_3 &&
						(trace.fraction < 1 ||
							PRVM_PROG_TO_EDICT(PRVM_serveredictedict(check, groundentity)) != pusher)) {
			// Baker: Remove ground flag - Baker: never hits during train problems or train non-problem only hits when wall pushes u off train.
								// RARELY HITS DURING TRAIN-NONPROBLEM
			PRVM_serveredictfloat(check, flags) = (int)PRVM_serveredictfloat(check, flags) &
			~FL_ONGROUND_512;
		}

		// if it is still inside the pusher, block
		VectorCopy(PRVM_serveredictvector(pusher, mins), pushermins);
		VectorCopy(PRVM_serveredictvector(pusher, maxs), pushermaxs);
		VectorCopy(PRVM_serveredictvector(check, origin), checkorigin);
		VectorCopy(PRVM_serveredictvector(check, mins), checkmins);
		VectorCopy(PRVM_serveredictvector(check, maxs), checkmaxs);

		// Baker: trace.startsolid == 0 when it shouldn't be -- July 24 2026
tertian:
		Collision_ClipToGenericEntity(&trace, pushermodel, pusher->priv.server->frameblend,
			&pusher->priv.server->skeleton, pushermins, pushermaxs, SUPERCONTENTS_BODY,
			&pusherfinalmatrix, &pusherfinalimatrix, checkorigin, checkmins, checkmaxs, checkorigin,
			checkcontents, SUPERCONTENTS_SKIP_NONE_0, MATERIALS_SKIP_NONE_0, collision_extendmovelength.value);
		//Con_PrintVarInt (trace.startsolid);
		if (trace.startsolid) {
			// Baker: We started the move in a a solid hopefully for the entity "check"
			vec3_t move2;
			// Baker: If we end up here for bad collision it is probably too late.
			if (SV_NudgeOutOfSolid_PivotIsKnownGood(check, pivot)) {
				// hack to invoke all necessary movement triggers
				VectorClear(move2);
				if (!SV_PushEntity_Did_Avoid_Teleport(&trace2, check, move2, /*dolink*/ true, /*checkstuck?*/ true)) {
					continue; // Baker: Player or something got teleported entity "check" got teleported
				}
				// we were able to fix it -- get could
				continue;
			}

			// Baker: July 24 2026 -- cannot fix it.

			// still inside pusher, so it's really blocked

			// fail the move
			if (PRVM_serveredictvector(check, mins)[0] == PRVM_serveredictvector(check, maxs)[0])
				continue;
			if (isin2(PRVM_serveredictfloat(check, solid), SOLID_NOT_0, SOLID_TRIGGER_1)) {
				// corpse
				if (sv_gameplayfix_nosquashentities.integer == 0) {
					// is crushed by a mover, preventing it from being interacted with again
					PRVM_serveredictvector(check, mins)[0] = PRVM_serveredictvector(check, mins)[1] = 0;
					VectorCopy (PRVM_serveredictvector(check, mins), PRVM_serveredictvector(check, maxs));
				}

				continue;
			}

			VectorCopy (pushorig, PRVM_serveredictvector(pusher, origin));
			VectorCopy (pushang, PRVM_serveredictvector(pusher, angles));
			PRVM_serveredictfloat(pusher, ltime) = pushltime;
			SV_LinkEdict(pusher);

			// move back any entities we already moved -- Baker: WHY?
			for (i = 0;i < num_moved; i ++) {
				prvm_edict_t *ed = PRVM_EDICT_NUM(moved_edicts[i]);
				VectorCopy (ed->priv.server->moved_from, PRVM_serveredictvector(ed, origin));
				VectorCopy (ed->priv.server->moved_fromangles, PRVM_serveredictvector(ed, angles));
				SV_LinkEdict(ed);
			}

			// if the pusher has a "blocked" function, call it, otherwise just stay in place until the obstacle is gone
			if (PRVM_serveredictfunction(pusher, blocked)) {
				PRVM_serverglobalfloat(time) = sv.time;
				PRVM_serverglobaledict(self) = PRVM_EDICT_TO_PROG(pusher);
				PRVM_serverglobaledict(other) = PRVM_EDICT_TO_PROG(check);
				prog->ExecuteProgram(prog, PRVM_serveredictfunction(pusher, blocked), "QC function self.blocked is missing");
			}
			break;
		}
	} // each check entity.

	//Con_PrintVarInt (num_moved);
	PRVM_serveredictvector(pusher, angles)[0] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[0] * (1.0 / 360.0));
	PRVM_serveredictvector(pusher, angles)[1] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[1] * (1.0 / 360.0));
	PRVM_serveredictvector(pusher, angles)[2] -= 360.0 * floor(PRVM_serveredictvector(pusher, angles)[2] * (1.0 / 360.0));
}

/*
================
SV_Physics_Pusher

================
*/
CALLERS_ (SV_Physics_Entity SV_Physics_ClientEntity)
static void SV_Physics_Pusher (prvm_edict_t *ent)
{
	prvm_prog_t *prog = SVVM_prog;
	prvm_vec_t thinktime, oldltime, movetime;

	oldltime = PRVM_serveredictfloat(ent, ltime);

	thinktime = PRVM_serveredictfloat(ent, nextthink);
	if (thinktime < PRVM_serveredictfloat(ent, ltime) + sv.frametime) {
		movetime = thinktime - PRVM_serveredictfloat(ent, ltime);
		if (movetime < 0)
			movetime = 0;
	}
	else
		movetime = sv.frametime;

	if (movetime) { // TRAINOIC
		// advances PRVM_serveredictfloat(ent, ltime) if not blocked
		SV_PushMove (ent, movetime);
	}

	if (thinktime > oldltime && thinktime <= PRVM_serveredictfloat(ent, ltime))
	{
		PRVM_serveredictfloat(ent, nextthink) = 0;
		PRVM_serverglobalfloat(time) = sv.time;
		PRVM_serverglobaledict(self) = PRVM_EDICT_TO_PROG(ent);
		PRVM_serverglobaledict(other) = PRVM_EDICT_TO_PROG(prog->edicts);
		prog->ExecuteProgram(prog, PRVM_serveredictfunction(ent, think), "QC function self.think is missing");
	}
}

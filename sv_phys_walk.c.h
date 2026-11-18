// sv_phys_walk.c.h

/*
=====================
SV_WalkMove

Only used by players
======================
*/
static void SV_WalkMove (prvm_edict_t *ent)
{
	prvm_prog_t *prog = SVVM_prog;
	int clip;
	int oldonground;
	//int originalmove_clip;
	int originalmove_flags;
	int originalmove_groundentity;
	int hitsupercontentsmask = SV_GenericHitSuperContentsMask(ent);
	int skipsupercontentsmask = 0;
	int skipmaterialflagsmask = 0;
	int type;
	vec3_t upmove, downmove, start_origin, start_velocity, stepnormal, originalmove_origin, originalmove_velocity, entmins, entmaxs;
	trace_t downtrace, trace;
	qbool applygravity;

	// if frametime is 0 (due to client sending the same timestamp twice),
	// don't move
	if (sv.frametime <= 0)
		return;

	if (sv_gameplayfix_unstickplayers.integer /*d:1*/)
		SV_CheckStuck (ent);

	applygravity = !SV_CheckWater (ent) && PRVM_serveredictfloat(ent, movetype) == MOVETYPE_WALK_3 && ! ((int)PRVM_serveredictfloat(ent, flags) & FL_WATERJUMP);

	SV_CheckVelocity_Limit_It(ent);

	// do a regular slide move unless it looks like you ran into a step
	oldonground = Have_Flag ((int)PRVM_serveredictfloat(ent, flags), FL_ONGROUND_512);

	VectorCopy (PRVM_serveredictvector(ent, origin), start_origin);
	VectorCopy (PRVM_serveredictvector(ent, velocity), start_velocity);

	clip = SV_FlyMove (ent, sv.frametime, applygravity, /*stepnormal*/ NULL, 
		hitsupercontentsmask, skipsupercontentsmask, skipmaterialflagsmask, 
		sv_gameplayfix_stepmultipletimes.integer /*d:0*/ ? sv_stepheight.value : 0); // TRAINOIC GROUND ZERO

#if 0 //def _DEBUG
	int entnum = PRVM_NUM_FOR_EDICT(ent);
	if (entnum == 1) {
		printvari (clip);
	}
#endif // DEBUG 0

	if (sv_gameplayfix_downtracesupportsongroundflag.integer /*d:0*/) {
		// BAKER: Not the norm
		if (!(clip & BLOCKED_FLOOR_1)) {
			// only try this if there was no floor in the way in the trace (no,
			// this check seems to be not REALLY necessary, because if clip & BLOCKED_FLOOR_1,
			// our trace will hit that thing too)
			VectorSet(upmove, PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2] + 1);
			VectorSet(downmove, PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2] - 1);
			if (PRVM_serveredictfloat(ent, movetype) == MOVETYPE_FLYMISSILE_9)
				type = MOVE_MISSILE_2;
			else if (PRVM_serveredictfloat(ent, movetype) == MOVETYPE_FLY_WORLDONLY_33)
				type = MOVE_WORLDONLY_3;
			else if (PRVM_serveredictfloat(ent, solid) == SOLID_TRIGGER_1 || PRVM_serveredictfloat(ent, solid) == SOLID_NOT_0)
				type = MOVE_NOMONSTERS_1; // only clip against bmodels
			else
				type = MOVE_NORMAL_0;
			VectorCopy(PRVM_serveredictvector(ent, mins), entmins);
			VectorCopy(PRVM_serveredictvector(ent, maxs), entmaxs);
			trace = SV_TraceBox(upmove, entmins, entmaxs, downmove, type, ent, SV_GenericHitSuperContentsMask(ent), skipsupercontentsmask, skipmaterialflagsmask, collision_extendmovelength.value);
			if (trace.fraction < 1 && trace.plane.normal[2] > 0.7) {
				// Baker: SLOPE == FLOOR
				clip |= BLOCKED_FLOOR_1; // but we HAVE found a floor
				// set groundentity so we get carried when walking onto a mover with sv_gameplayfix_nogravityonground
				PRVM_serveredictedict(ent, groundentity) = PRVM_EDICT_TO_PROG(trace.ent);
			}
		}
	} // sv_gameplayfix_downtracesupportsongroundflag.integer

	// if the move did not hit the ground at any point, we're not on ground
	if (Have_Flag(clip, BLOCKED_FLOOR_1) == false)
		PRVM_serveredictfloat(ent, flags) = (int)PRVM_serveredictfloat(ent, flags) & ~FL_ONGROUND_512;

#if 0 //def _DEBUG
	printvari (clip);
#endif

	SV_CheckVelocity_Limit_It(ent);
	SV_LinkEdict(ent);
	SV_LinkEdict_TouchAreaGrid(ent);

	if (Have_Flag(clip, BLOCKED_TELEPORT_8)) // teleport
		return;

	if ((int)PRVM_serveredictfloat(ent, flags) & FL_WATERJUMP)
		return;

	if (sv_nostep.integer)
		return;

	VectorCopy(PRVM_serveredictvector(ent, origin), originalmove_origin);
	VectorCopy(PRVM_serveredictvector(ent, velocity), originalmove_velocity);
	//originalmove_clip = clip;
	originalmove_flags = (int)PRVM_serveredictfloat(ent, flags);
	originalmove_groundentity = PRVM_serveredictedict(ent, groundentity);

	// if move didn't block on a step, return
	if (Have_Flag (clip, BLOCKED_STEP_2)) {
		// if move was not trying to move into the step, return
		if (fabs(start_velocity[0]) < DIST_EPSILON_0_03125 && fabs(start_velocity[1]) < DIST_EPSILON_0_03125)
			return;

		if (PRVM_serveredictfloat(ent, movetype) != MOVETYPE_FLY_5)
		{
			// return if gibbed by a trigger
			if (PRVM_serveredictfloat(ent, movetype) != MOVETYPE_WALK_3)
				return;

			// return if attempting to jump while airborn (unless sv_jumpstep)
			if (!sv_jumpstep.integer)
				if (!oldonground && PRVM_serveredictfloat(ent, waterlevel) == WATERLEVEL_NONE_0 /*0*/)
					return;
		}

		// try moving up and forward to go up a step
		// back to start pos
		VectorCopy (start_origin, PRVM_serveredictvector(ent, origin));
		VectorCopy (start_velocity, PRVM_serveredictvector(ent, velocity));

		// move up
		VectorClear (upmove);
		upmove[2] = sv_stepheight.value;
		if (!SV_PushEntity_Did_Avoid_Teleport(&trace, ent, upmove, /*dolink?*/ true, /*checkstuck*/ true)) {
			// we got teleported when upstepping... must abort the move
			return; // Baker: Does not happen with train issue July 18 2026
		}

		// Baker: This is the 2ND flymove in the function.
		// IN AN ATTEMPT TO STEP UP.
		// move forward
		PRVM_serveredictvector(ent, velocity)[2] = 0;
		clip = SV_FlyMove (ent, sv.frametime, applygravity, stepnormal, hitsupercontentsmask, skipsupercontentsmask, skipmaterialflagsmask, 0);
		PRVM_serveredictvector(ent, velocity)[2] += start_velocity[2];
		if (Have_Flag (clip, BLOCKED_TELEPORT_8)) {
			// we got teleported when upstepping... must abort the move
			// note that z velocity handling may not be what QC expects here, but we cannot help it
			return;
		}

		// Baker: We only come here when we are close to a wall.  Even phantom?
		// BUT NOT WHEN WE GO THRU IT.
		// THEREFORE CLIP DID NOT GET SET.
#ifdef _DEBUG
		
		if (Have_Flag (clip, BLOCKED_STEP_2)) {
			int j = 5;
		}
#endif

		SV_CheckVelocity_Limit_It(ent);
		SV_LinkEdict(ent);
		SV_LinkEdict_TouchAreaGrid(ent);

		// check for stuckness, possibly due to the limited precision of floats
		// in the clipping hulls
		if (clip
		 && fabs(originalmove_origin[1] - PRVM_serveredictvector(ent, origin)[1]) < DIST_EPSILON_0_03125
		 && fabs(originalmove_origin[0] - PRVM_serveredictvector(ent, origin)[0]) < DIST_EPSILON_0_03125) {
			//Con_Printf ("wall\n");
			// stepping up didn't make any progress, revert to original move
			VectorCopy(originalmove_origin, PRVM_serveredictvector(ent, origin));
			VectorCopy(originalmove_velocity, PRVM_serveredictvector(ent, velocity));
			//clip = originalmove_clip;
			PRVM_serveredictfloat(ent, flags) = originalmove_flags;
			PRVM_serveredictedict(ent, groundentity) = originalmove_groundentity;
			// now try to unstick if needed
			//clip = SV_TryUnstick (ent, oldvel);
			return;
		} // wall

		//Con_Printf ("step - ");

		// extra friction based on view angle
		if (clip & BLOCKED_STEP_2 && sv_wallfriction.integer /*d:1*/)
			SV_WallFriction (ent, stepnormal);
	}
	// don't do the down move if stepdown is disabled, moving upward, not in water,
	// or the move started offground or ended onground
	else if (!sv_gameplayfix_stepdown.integer /*d:0*/ ||
		PRVM_serveredictfloat(ent, waterlevel) >= WATERLEVEL_SUBMERGED_3 || start_velocity[2] >= (1.0 / 32.0)
		|| !oldonground || ((int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512))
		return;

	// move down
	VectorClear (downmove);
	downmove[2] = -sv_stepheight.value + start_velocity[2]*sv.frametime;
	if (!SV_PushEntity_Did_Avoid_Teleport(&downtrace, ent, downmove, true, true)) {
		// we got teleported when downstepping... must abort the move
		return;
	}

	if (downtrace.fraction < 1 && downtrace.plane.normal[2] > 0.7) {
		// this has been disabled so that you can't jump when you are stepping
		// up while already jumping (also known as the Quake2 double jump bug)
#if 0
		// LadyHavoc: disabled this check so you can walk on monsters/players
		//if (PRVM_serveredictfloat(ent, solid) == SOLID_BSP_4)
		{
			//Con_Printf ("onground\n");
			PRVM_serveredictfloat(ent, flags) =	(int)PRVM_serveredictfloat(ent, flags) | FL_ONGROUND_512;
			PRVM_serveredictedict(ent, groundentity) = PRVM_EDICT_TO_PROG(downtrace.ent);
		}
#endif
	}
	else
	{
		//Con_Printf ("slope\n");
		// if the push down didn't end up on good ground, use the move without
		// the step up.  This happens near wall / slope combinations, and can
		// cause the player to hop up higher on a slope too steep to climb
		VectorCopy(originalmove_origin, PRVM_serveredictvector(ent, origin));
		VectorCopy(originalmove_velocity, PRVM_serveredictvector(ent, velocity));
		//clip = originalmove_clip;
		PRVM_serveredictfloat(ent, flags) = originalmove_flags;
		PRVM_serveredictedict(ent, groundentity) = originalmove_groundentity;
	}

	SV_CheckVelocity_Limit_It(ent);
	SV_LinkEdict(ent);
	SV_LinkEdict_TouchAreaGrid(ent);
}



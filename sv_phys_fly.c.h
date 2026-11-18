// sv_phys_fly.c.h

/*
============
SV_FlyMove

The basic solid body movement clip that slides along multiple planes
Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
8 = teleported by touch method
If stepnormal is not NULL, the plane normal of any vertical wall hit will be stored
============
*/
static float SV_Gravity (prvm_edict_t *ent);
static qbool SV_PushEntity_Did_Avoid_Teleport (trace_t *trace, prvm_edict_t *ent, vec3_t push, qbool dolink, qbool checkstuck);
#define MAX_CLIP_PLANES_5 5
CALLERS_ (SV_WalkMove twice, SV_Physics_Step)

RELATED_ (SV_PushEntity_Did_Avoid_Teleport) // OR DEEPER IS WHERE
static int SV_FlyMove (prvm_edict_t *ent, float time, qbool applygravity, float *stepnormal, int hitsupercontentsmask, int skipsupercontentsmask, int skipmaterialflagsmask, float stepheight_multiplesteps)
{
	prvm_prog_t *prog = SVVM_prog;
	int blocked, bumpcount;
	int i, j, numplanes;
	float d, time_left, gravity;
	vec3_t dir, push_dir, planes[MAX_CLIP_PLANES_5];
	prvm_vec3_t primal_velocity, original_velocity, new_velocity, restore_velocity;
#if 0
	vec3_t end;
#endif
	trace_t trace;
returned_1: // TIME <= 0
	if (time <= 0)
		return 0;
	gravity = 0;

	VectorCopy(PRVM_serveredictvector(ent, velocity), restore_velocity); // SV_FlyMove

#ifdef _DEBUG // July 18 2026
	int entnum = PRVM_NUM_FOR_EDICT(ent);
	//Con_PrintLinef ("%s: %d", __func__, entnum);
#endif


	int is_onground = (int)PRVM_serveredictfloat(ent, flags);
	is_onground = Have_Flag (is_onground, FL_ONGROUND_512); // 512

	if (applygravity) {
		gravity = SV_Gravity(ent);
		// Baker: If not on-ground or we don't have nogravground set, apply gravity
		// sv_gameplayfix_nogravityonground defaults 0, although objectn uses 1
		if (is_onground == false || sv_gameplayfix_nogravityonground.integer == 0) {
			if (sv_gameplayfix_gravityunaffectedbyticrate.integer /*d:0*/)
				PRVM_serveredictvector(ent, velocity)[2] -= gravity * 0.5f;
			else
				PRVM_serveredictvector(ent, velocity)[2] -= gravity;
		}
	}

	blocked = 0;
	VectorCopy(PRVM_serveredictvector(ent, velocity), original_velocity);
	VectorCopy(PRVM_serveredictvector(ent, velocity), primal_velocity);
	numplanes = 0;
	time_left = time;
	for (bumpcount = 0; bumpcount < MAX_CLIP_PLANES_5; bumpcount++) {
		if (!PRVM_serveredictvector(ent, velocity)[0] &&
			!PRVM_serveredictvector(ent, velocity)[1] &&
			!PRVM_serveredictvector(ent, velocity)[2])
			break;

		VectorScale(PRVM_serveredictvector(ent, velocity), time_left, push_dir);

		// MAJORUS
		int did_avoid_teleport = SV_PushEntity_Did_Avoid_Teleport(&trace, ent, push_dir /*direction*/,
			/*dolink?*/ false,
/*checkstuck*/ true); // TRAINOIC FRACTION 0 CLIENT NO LINKY FALSE
#ifdef _DEBUG
		//Con_PrintVarInt (did_avoid_teleport);
#endif
		if (did_avoid_teleport == false) {
			// we got teleported by a touch function
			// let's abort the move
			blocked |= BLOCKED_TELEPORT_8;
			break;
		}

		// this code is used by MOVETYPE_WALK_3 and MOVETYPE_STEP_4 and SV_UnstickEntity
		// abort move if we're stuck in the world (and didn't make it out)

		if (trace.worldstartsolid && trace.allsolid) {
			// Baker: July 18 2026 - doesn't happen with the train at all.
			VectorCopy(restore_velocity, PRVM_serveredictvector(ent, velocity));
returned_2: // ALL SOLID
			return (BLOCKED_FLOOR_1 | BLOCKED_STEP_2) /*3*/;
		}

#ifdef _DEBUG
		// Already in the brush
		if (entnum == 1 && trace.fraction > 0 && trace.fraction < 1) {
			// Baker: This happens?
			int j = 5; // July 18 2026 - Happens with train issue.  Does not happen when not a train issue.
		}

#endif // _DEBUG

#ifdef _DEBUG
		if (trace.startsolid) {
			// Baker: This happens.  But when it does, is it too late?
			int j = 5; // July 18 2026 - Happens with train issue.  Does not happen when not a train issue.
		}
#endif

		if (trace.fraction == 1) {
#ifdef _DEBUG
			if (trace.startsolid) {
				int j = 5; // July 18 2026 - Happens with train issue.  Does not happen when not a train issue.
			}
#endif
			break;
		}

		time_left *= 1 - trace.fraction;

		if (trace.plane.normal[2]) {
			if (trace.plane.normal[2] > 0.7) {
				// floor
				blocked |= BLOCKED_FLOOR_1;

				if (!trace.ent) {
					Con_Printf ("SV_FlyMove: !trace.ent");
					trace.ent = prog->edicts;
				}

				PRVM_serveredictfloat(ent, PLAYER_xflags) = (int)PRVM_serveredictfloat(ent, PLAYER_xflags) | FL_ONGROUND_512; // ADD
				PRVM_serveredictedict(ent, ENT_groundentity) = PRVM_EDICT_TO_PROG(trace.ent);
			}
		}
		else if (stepheight_multiplesteps)
		{
			// step - handle it immediately
			vec3_t org;
			vec3_t steppush;
			trace_t steptrace;
			trace_t steptrace2;
			trace_t steptrace3;
			//Con_Printf ("step %f %f %f : ", PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2]);
			VectorSet(steppush, 0, 0, stepheight_multiplesteps);
			VectorScale(PRVM_serveredictvector(ent, velocity), time_left, push_dir);
			VectorCopy(PRVM_serveredictvector(ent, origin), org);
			if (!SV_PushEntity_Did_Avoid_Teleport(&steptrace, ent, steppush, /*dolink?*/ false, /*checkstuck*/ true)) {
				blocked |= BLOCKED_TELEPORT_8 /*8*/;
				break;
			}
			//Con_Printf ("%f %f %f : ", PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2]);
			if (!SV_PushEntity_Did_Avoid_Teleport(&steptrace2, ent, push_dir, /*dolink?*/ false, /*checkstuck*/ true)) {
				blocked |= BLOCKED_TELEPORT_8 /*8*/;
				break;
			}
			//Con_Printf ("%f %f %f : ", PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2]);
			VectorSet(steppush, 0, 0, org[2] - PRVM_serveredictvector(ent, origin)[2]);
			if (!SV_PushEntity_Did_Avoid_Teleport(&steptrace3, ent, steppush, /*dolink?*/ false, /*checkstuck*/ true)) {
				blocked |= BLOCKED_TELEPORT_8 /*8*/;
				break;
			}
			//Con_Printf ("%f %f %f : ", PRVM_serveredictvector(ent, origin)[0], PRVM_serveredictvector(ent, origin)[1], PRVM_serveredictvector(ent, origin)[2]);
			// accept the new position if it made some progress...
			// previously this checked if absolute distance >= 0.03125 which made stepping up unreliable
			if (PRVM_serveredictvector(ent, origin)[0] - org[0] ||
				PRVM_serveredictvector(ent, origin)[1] - org[1]) {
				//Con_Printf ("accepted (delta %f %f %f)\n", PRVM_serveredictvector(ent, origin)[0] - org[0], PRVM_serveredictvector(ent, origin)[1] - org[1], PRVM_serveredictvector(ent, origin)[2] - org[2]);
				trace = steptrace2;
				VectorCopy(PRVM_serveredictvector(ent, origin), trace.endpos);
				time_left *= 1 - trace.fraction;
				numplanes = 0;
				continue;
			}
			else
			{
				//Con_Printf ("REJECTED (delta %f %f %f)\n", PRVM_serveredictvector(ent, origin)[0] - org[0], PRVM_serveredictvector(ent, origin)[1] - org[1], PRVM_serveredictvector(ent, origin)[2] - org[2]);
				VectorCopy(org, PRVM_serveredictvector(ent, origin));
			}
		}
		else
		{
			// step - return it to caller
			blocked |= BLOCKED_STEP_2 /*2*/; // TRAINZ TRAINOIC -- THIS DOES NOT HIT WHEN IT SHOULD
			// save the trace for player extrafriction
			if (stepnormal)
				VectorCopy(trace.plane.normal, stepnormal);
		}
//#if 111 // Baker r0061: Classic DarkPlaces physics
//#else
		if (sv_gameplayfix_fiendjumpfix.integer) {
			// NEW WAY

			// Unlike some other movetypes Quake's SV_FlyMove calls SV_Impact only after setting ONGROUND which id1 fiends rely on.
			// If we stepped up (sv_gameplayfix_stepmultipletimes) this will impact the steptrace2 plane instead of the original.
			if (PRVM_serveredictfloat(ent, solid) >= SOLID_TRIGGER_1 && trace.ent)
				SV_Impact(ent, &trace);
returned_3: // FREE
			if (ent->free)
				return blocked; // removed by the impact function
		}
//#endif
		if (trace.fraction >= 0.001) {
			// actually covered some distance
			VectorCopy(PRVM_serveredictvector(ent, velocity), original_velocity);
			numplanes = 0;
		}

		// clipped to another plane
		if (numplanes >= MAX_CLIP_PLANES_5) {
			// this shouldn't really happen
			VectorClear(PRVM_serveredictvector(ent, velocity));
			blocked = BLOCKED_FLOOR_1 | BLOCKED_STEP_2; // 3;
			break;
		}


		VectorCopy(trace.plane.normal, planes[numplanes]);
		numplanes++;

		// modify original_velocity so it parallels all of the clip planes
		for (i = 0;i < numplanes;i++) {
			ClipVelocity(original_velocity, planes[i], new_velocity, 1);
			for (j = 0;j < numplanes;j++) {
				if (j != i) {
					// not ok
					if (DotProduct(new_velocity, planes[j]) < 0)
						break;
				}
			}
			if (j == numplanes)
				break;
		}

		if (i != numplanes) {
			// go along this plane
			VectorCopy(new_velocity, PRVM_serveredictvector(ent, velocity));
		}
		else
		{
			// go along the crease
			if (numplanes != 2)
			{
				VectorClear(PRVM_serveredictvector(ent, velocity));
				blocked = BLOCKED_FLOOR_1 | BLOCKED_STEP_2 | BLOCKED_OTHER_4 /*7*/;
				break;
			}
			CrossProduct(planes[0], planes[1], dir);
			// LadyHavoc: thanks to taniwha of QuakeForge for pointing out this fix for slowed falling in corners
			VectorNormalize(dir);
			d = DotProduct(dir, PRVM_serveredictvector(ent, velocity));
			VectorScale(dir, d, PRVM_serveredictvector(ent, velocity));
		}

		// if current velocity is against the original velocity,
		// stop dead to avoid tiny occilations in sloping corners
		if (DotProduct(PRVM_serveredictvector(ent, velocity), primal_velocity) <= 0)
		{
			VectorClear(PRVM_serveredictvector(ent, velocity));
			break;
		}
	}

	// LadyHavoc: this came from QW and allows you to get out of water more easily
	if (sv_gameplayfix_easierwaterjump.integer && ((int)PRVM_serveredictfloat(ent, flags) & FL_WATERJUMP) &&
		!(blocked & BLOCKED_TELEPORT_8))
		VectorCopy(primal_velocity, PRVM_serveredictvector(ent, velocity));

	if (applygravity) {
		if (!sv_gameplayfix_nogravityonground.integer || !((int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512)) {
			if (sv_gameplayfix_gravityunaffectedbyticrate.integer)
				PRVM_serveredictvector(ent, velocity)[2] -= gravity * 0.5f;
		}
	}

	return blocked;
}

// sv_phys_toss.c.h

/*
=============
SV_Physics_Toss

Toss, bounce, and fly movement.  When onground, do nothing.
=============
*/

void SV_Physics_Toss (prvm_edict_t *ent)
{
	prvm_prog_t *prog = SVVM_prog;
	trace_t trace;
	vec3_t move;
	vec_t movetime;
	int bump;
	prvm_edict_t *groundentityx;
	float d, ent_gravity;
	float bouncefactor;
	float bouncestop;

// if onground, return without moving
	if ((int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512) {
		groundentityx = PRVM_PROG_TO_EDICT(PRVM_serveredictedict(ent, ENT_groundentity)); // TOSS SV_Physics_Toss
		if (PRVM_serveredictvector(ent, velocity)[2] >= (1.0 / 32.0) && sv_gameplayfix_upwardvelocityclearsongroundflag.integer /*d:0*/) {
			// don't stick to ground if onground and moving upward
			PRVM_serveredictfloat(ent, flags) -= FL_ONGROUND_512;
		}
		else if (!PRVM_serveredictedict(ent, ENT_groundentity) || !sv_gameplayfix_noairborncorpse.integer /*d:0*/) { // TOSS SV_Physics_Toss
			// we can trust FL_ONGROUND_512 if groundentity is world because it never moves
			return;
		}
		else if (ent->priv.server->suspendedinairflag && groundentityx->free)
		{
			// if ent was supported by a brush model on previous frame,
			// and groundentity is now freed, set groundentity to 0 (world)
			// which leaves it suspended in the air
			PRVM_serveredictedict(ent, groundentity) = 0; // TOSS - suspend in air
			if (sv_gameplayfix_noairborncorpse_allowsuspendeditems.integer /*d:0*/)
				return;
		}
		else if (BoxesOverlap(ent->priv.server->cullmins, ent->priv.server->cullmaxs, groundentityx->priv.server->cullmins, groundentityx->priv.server->cullmaxs))
		{
			// don't slide if still touching the groundentity
			return;
		}
	}
	ent->priv.server->suspendedinairflag = false;

	SV_CheckVelocity_Limit_It (ent);

// add gravity
	float fmovetype = PRVM_serveredictfloat(ent, movetype);
	if (sv.is_qex && fmovetype == MOVETYPE_GIB_FIGHTS_BOUNCEMISSILE_11) // AURA 11.0
		fmovetype = MOVETYPE_BOUNCE_10; // MOVETYPE_GIB_FIGHTS_BOUNCEMISSILE_11

	if (isin2 (fmovetype, MOVETYPE_TOSS_6, MOVETYPE_BOUNCE_10))
		PRVM_serveredictvector(ent, velocity)[2] -= SV_Gravity(ent);

// move angles
	VectorMA (PRVM_serveredictvector(ent, angles), sv.frametime, PRVM_serveredictvector(ent, avelocity), PRVM_serveredictvector(ent, angles));

	movetime = sv.frametime;
	for (bump = 0;bump < MAX_CLIP_PLANES_5 && movetime > 0;bump++)
	{
	// move origin
		VectorScale(PRVM_serveredictvector(ent, velocity), movetime, move);
#if 111 // Baker r0061: Classic DarkPlaces physics
		if (!SV_PushEntity_Did_Avoid_Teleport(&trace, ent, move, true, true)) // TOSS SV_Physics_Toss
			return; // teleported
		if (ent->free)
			return;
		if (trace.bmodelstartsolid && sv_gameplayfix_unstickentities.integer)
		{
			// try to unstick the entity
			SV_UnstickEntity(ent);
			if (!SV_PushEntity_Did_Avoid_Teleport(&trace, ent, move, true, true))
				return; // teleported
			if (ent->free)
				return;
		}
#else
		// The buzzsaw traps in r2m6 and r2m7 use MOVETYPE_FLY_5 and rely on moving while stuck in the world.
		// Quake movetypes checked allsolid only in SV_FlyMove().
		if (!SV_PushEntity_Did_Avoid_Teleport(&trace, ent, move, true, PRVM_serveredictfloat(ent, movetype) != MOVETYPE_FLY_5))
			return; // teleported

		if (ent->free)
			return;
#endif
		if (trace.fraction == 1)
			break;
		movetime *= 1 - min(1, trace.fraction);
		switch((int)PRVM_serveredictfloat(ent, movetype))
		{
		case MOVETYPE_BOUNCEMISSILE_11:
			if (sv.is_qex) { // AURA 11.1
				goto gib_movetype; // MOVETYPE_GIB_FIGHTS_BOUNCEMISSILE_11 is BOUNCE
			}
			bouncefactor = PRVM_serveredictfloat(ent, bouncefactor);
			if (!bouncefactor)
				bouncefactor = 1.0f;

			ClipVelocity(PRVM_serveredictvector(ent, velocity), trace.plane.normal, PRVM_serveredictvector(ent, velocity), 1 + bouncefactor);
			PRVM_serveredictfloat(ent, flags) = (int)PRVM_serveredictfloat(ent, flags) & ~FL_ONGROUND_512;
			if (!sv_gameplayfix_slidemoveprojectiles.integer)
				movetime = 0;
			break;

		case MOVETYPE_BOUNCE_10:
gib_movetype: // MOVETYPE_GIB_FIGHTS_BOUNCEMISSILE_11 is BOUNCE // AURA 11.2
			bouncefactor = PRVM_serveredictfloat(ent, bouncefactor);
			if (!bouncefactor)
				bouncefactor = 0.5f;

			bouncestop = PRVM_serveredictfloat(ent, bouncestop);
			if (!bouncestop)
				bouncestop = 60.0f / 800.0f;

			ClipVelocity(PRVM_serveredictvector(ent, velocity), trace.plane.normal, PRVM_serveredictvector(ent, velocity), 1 + bouncefactor);
			ent_gravity = PRVM_serveredictfloat(ent, gravity);
			if (!ent_gravity)
				ent_gravity = 1.0f;
			// LadyHavoc: fixed grenades not bouncing when fired down a slope
			if (sv_gameplayfix_grenadebouncedownslopes.integer)
				d = fabs(DotProduct(trace.plane.normal, PRVM_serveredictvector(ent, velocity)));
			else
				d = PRVM_serveredictvector(ent, velocity)[2];
			if (trace.plane.normal[2] > 0.7 && d < sv_gravity.value * bouncestop * ent_gravity)
			{
				PRVM_serveredictfloat(ent, flags) = (int)PRVM_serveredictfloat(ent, flags) | FL_ONGROUND_512;
				PRVM_serveredictedict(ent, groundentity) = PRVM_EDICT_TO_PROG(trace.ent);
				VectorClear(PRVM_serveredictvector(ent, velocity));
				VectorClear(PRVM_serveredictvector(ent, avelocity));
				movetime = 0;
			}
			else
			{
				// Baker: July 24 2026 - remove onground TOSS - SV_Physics Toss
				PRVM_serveredictfloat(ent, PLAYER_xflags) = (int)PRVM_serveredictfloat(ent, PLAYER_xflags) & ~FL_ONGROUND_512; // Baker: Removes onground
				if (!sv_gameplayfix_slidemoveprojectiles.integer /*d:0*/)
					movetime = 0;
			}
			break;
		default:
			ClipVelocity (PRVM_serveredictvector(ent, velocity), trace.plane.normal, PRVM_serveredictvector(ent, velocity), 1.0);
			if (trace.plane.normal[2] > 0.7)
			{
				PRVM_serveredictfloat(ent, flags) = (int)PRVM_serveredictfloat(ent, flags) | FL_ONGROUND_512;
				PRVM_serveredictedict(ent, groundentity) = PRVM_EDICT_TO_PROG(trace.ent);
				if (PRVM_serveredictfloat(((prvm_edict_t *)trace.ent), solid) == SOLID_BSP_4)
					ent->priv.server->suspendedinairflag = true;
				VectorClear (PRVM_serveredictvector(ent, velocity));
				VectorClear (PRVM_serveredictvector(ent, avelocity));
				movetime = 0;
			}
			else
			{
				PRVM_serveredictfloat(ent, flags) = (int)PRVM_serveredictfloat(ent, flags) & ~FL_ONGROUND_512;
				if (!sv_gameplayfix_slidemoveprojectiles.integer)
					movetime = 0;
			}
			break;
		}
	}

// check for in water
	SV_CheckWaterTransition (ent);
}

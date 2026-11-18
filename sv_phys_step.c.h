// sv_phys_step.c.h


/*
===============================================================================

STEPPING MOVEMENT

===============================================================================
*/

/*
=============
SV_Physics_Step

Monsters freefall when they don't have a ground entity, otherwise
all movement is done with discrete steps.

This is also used for objects that have become still on the ground, but
will fall if the floor is pulled out from under them.
=============
*/
static void SV_Physics_Step (prvm_edict_t *ent)
{
	prvm_prog_t *prog = SVVM_prog;
	int flags = (int)PRVM_serveredictfloat(ent, flags);

	// DRESK
	// Backup Velocity in the event that movetypesteplandevent is called,
	// to provide a parameter with the entity's velocity at impact.
	vec3_t backupVelocity;
	VectorCopy(PRVM_serveredictvector(ent, velocity), backupVelocity);
	// don't fall at all if fly/swim
	if (!(flags & (FL_FLY_1 | FL_SWIM_2)))
	{
		if (flags & FL_ONGROUND_512)
		{
			// freefall if onground and moving upward
			// freefall if not standing on a world surface (it may be a lift or trap door)
			if (PRVM_serveredictvector(ent, velocity)[2] >= (1.0 / 32.0) && sv_gameplayfix_upwardvelocityclearsongroundflag.integer)
			{
				PRVM_serveredictfloat(ent, flags) -= FL_ONGROUND_512;
				SV_CheckVelocity_Limit_It(ent);
				SV_FlyMove(ent, sv.frametime, true, NULL, SV_GenericHitSuperContentsMask(ent), 0, 0, 0);
				SV_LinkEdict(ent);
				SV_LinkEdict_TouchAreaGrid(ent);
				ent->priv.server->waterposition_forceupdate = true;
			}
		}
		else
		{
			// freefall if not onground
			int hitsound = PRVM_serveredictvector(ent, velocity)[2] < sv_gravity.value * -0.1;

			SV_CheckVelocity_Limit_It(ent);
			SV_FlyMove(ent, sv.frametime, true, NULL, SV_GenericHitSuperContentsMask(ent), 0, 0, 0);
			SV_LinkEdict(ent);
			SV_LinkEdict_TouchAreaGrid(ent);

			// just hit ground
			if (hitsound && (int)PRVM_serveredictfloat(ent, flags) & FL_ONGROUND_512)
			{
				// DRESK - Check for Entity Land Event Function
				if (PRVM_serveredictfunction(ent, movetypesteplandevent))
				{ // Valid Function; Execute
					// Prepare Parameters
					// Assign Velocity at Impact
					PRVM_G_VECTOR(OFS_PARM0)[0] = backupVelocity[0];
					PRVM_G_VECTOR(OFS_PARM0)[1] = backupVelocity[1];
					PRVM_G_VECTOR(OFS_PARM0)[2] = backupVelocity[2];
					// Assign Self
					PRVM_serverglobaledict(self) = PRVM_EDICT_TO_PROG(ent);
					// Set Time
					PRVM_serverglobalfloat(time) = sv.time;
					// Execute VM Function
					prog->ExecuteProgram(prog, PRVM_serveredictfunction(ent, movetypesteplandevent), "movetypesteplandevent: NULL function");
				}
				else
				// Check for Engine Landing Sound
				if (sv_sound_land.string)
					SV_StartSound(ent, 0, sv_sound_land.string, 255, 1, false, 1.0f);
			}
			ent->priv.server->waterposition_forceupdate = true;
		}
	}
}

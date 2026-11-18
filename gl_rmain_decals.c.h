// gl_rmain_decals.c.h

void R_DecalSystem_Reset(decalsystem_t *decalsystem)
{
	if (decalsystem->decals)
		Mem_Free(decalsystem->decals);
	memset(decalsystem, 0, sizeof(*decalsystem));
}

static void R_DecalSystem_SpawnTriangle(decalsystem_t *decalsystem, const float *v0, const float *v1, const float *v2, const float *t0, const float *t1, const float *t2, const float *c0, const float *c1, const float *c2, int triangleindex, int surfaceindex, unsigned int decalsequence)
{
	tridecal_t *decal;
	int i;

	// expand or initialize the system
	if (decalsystem->maxdecals <= decalsystem->numdecals)
	{
		decalsystem_t old = *decalsystem;
		qbool useshortelements;
		decalsystem->maxdecals = max(16, decalsystem->maxdecals * 2);
		useshortelements = decalsystem->maxdecals * 3 <= 65536;
		decalsystem->decals = (tridecal_t *)Mem_Alloc(cls.levelmempool, decalsystem->maxdecals * (sizeof(tridecal_t) + sizeof(float[3][3]) + sizeof(float[3][2]) + sizeof(float[3][4]) + sizeof(int[3]) + (useshortelements ? sizeof(unsigned short[3]) : 0)));
		decalsystem->color4f = (float *)(decalsystem->decals + decalsystem->maxdecals);
		decalsystem->texcoord2f = (float *)(decalsystem->color4f + decalsystem->maxdecals*12);
		decalsystem->vertex3f = (float *)(decalsystem->texcoord2f + decalsystem->maxdecals*6);
		decalsystem->element3i = (int *)(decalsystem->vertex3f + decalsystem->maxdecals*9);
		decalsystem->element3s = (useshortelements ? ((unsigned short *)(decalsystem->element3i + decalsystem->maxdecals*3)) : NULL);
		if (decalsystem->numdecals)
			memcpy(decalsystem->decals, old.decals, decalsystem->numdecals * sizeof(tridecal_t));
		if (old.decals)
			Mem_Free(old.decals);
		for (i = 0;i < decalsystem->maxdecals*3;i++)
			decalsystem->element3i[i] = i;
		if (useshortelements)
			for (i = 0;i < decalsystem->maxdecals*3;i++)
				decalsystem->element3s[i] = i;
	}

	// grab a decal and search for another free slot for the next one
	decal = &decalsystem->decals[decalsystem->numdecals++];

	// initialize the decal
	decal->lived = 0;
	decal->triangleindex = triangleindex;
	decal->surfaceindex = surfaceindex;
	decal->decalsequence = decalsequence;
	decal->color4f[0][0] = c0[0];
	decal->color4f[0][1] = c0[1];
	decal->color4f[0][2] = c0[2];
	decal->color4f[0][3] = 1;
	decal->color4f[1][0] = c1[0];
	decal->color4f[1][1] = c1[1];
	decal->color4f[1][2] = c1[2];
	decal->color4f[1][3] = 1;
	decal->color4f[2][0] = c2[0];
	decal->color4f[2][1] = c2[1];
	decal->color4f[2][2] = c2[2];
	decal->color4f[2][3] = 1;
	decal->vertex3f[0][0] = v0[0];
	decal->vertex3f[0][1] = v0[1];
	decal->vertex3f[0][2] = v0[2];
	decal->vertex3f[1][0] = v1[0];
	decal->vertex3f[1][1] = v1[1];
	decal->vertex3f[1][2] = v1[2];
	decal->vertex3f[2][0] = v2[0];
	decal->vertex3f[2][1] = v2[1];
	decal->vertex3f[2][2] = v2[2];
	decal->texcoord2f[0][0] = t0[0];
	decal->texcoord2f[0][1] = t0[1];
	decal->texcoord2f[1][0] = t1[0];
	decal->texcoord2f[1][1] = t1[1];
	decal->texcoord2f[2][0] = t2[0];
	decal->texcoord2f[2][1] = t2[1];
	TriangleNormal(v0, v1, v2, decal->plane);
	VectorNormalize(decal->plane);
	decal->plane[3] = DotProduct(v0, decal->plane);
}

extern cvar_t cl_decals_bias;
extern cvar_t cl_decals_models;
extern cvar_t cl_decals_newsystem_intensitymultiplier;
// baseparms, parms, temps

// Baker: Here is the main action of creating the decal.
static void R_DecalSystem_SplatTriangle(decalsystem_t *decalsystem, float r, float g, float b, float a, float s1, float t1, float s2, float t2, unsigned int decalsequence, qbool dynamic, float (*planes)[4], matrix4x4_t *projection, int triangleindex, int surfaceindex)
{
	int cornerindex;
	int index;
	float v[9][3];
	const float *vertex3f;
	const float *normal3f;
	int numpoints;
	float points[2][9][3];
	float temp[3];
	float tc[9][2];
	float f;
	float c[9][4];
	const int *e;

	e = rsurface.modelelement3i + 3*triangleindex;

	vertex3f = rsurface.modelvertex3f;
	normal3f = rsurface.modelnormal3f;

	if (normal3f)
	{
		for (cornerindex = 0;cornerindex < 3;cornerindex++)
		{
			index = 3*e[cornerindex];
			VectorMA(vertex3f + index, cl_decals_bias.value, normal3f + index, v[cornerindex]);
		}
	}
	else
	{
		for (cornerindex = 0;cornerindex < 3;cornerindex++)
		{
			index = 3*e[cornerindex];
			VectorCopy(vertex3f + index, v[cornerindex]);
		}
	}

	// cull backfaces
	//TriangleNormal(v[0], v[1], v[2], normal);
	//if (DotProduct(normal, localnormal) < 0.0f)
	//	continue;
	// clip by each of the box planes formed from the projection matrix
	// if anything survives, we emit the decal
	numpoints = PolygonF_Clip(3        , v[0]        , planes[0][0], planes[0][1], planes[0][2], planes[0][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), points[1][0]);
	if (numpoints < 3)
		return;
	numpoints = PolygonF_Clip(numpoints, points[1][0], planes[1][0], planes[1][1], planes[1][2], planes[1][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), points[0][0]);
	if (numpoints < 3)
		return;
	numpoints = PolygonF_Clip(numpoints, points[0][0], planes[2][0], planes[2][1], planes[2][2], planes[2][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), points[1][0]);
	if (numpoints < 3)
		return;
	numpoints = PolygonF_Clip(numpoints, points[1][0], planes[3][0], planes[3][1], planes[3][2], planes[3][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), points[0][0]);
	if (numpoints < 3)
		return;
	numpoints = PolygonF_Clip(numpoints, points[0][0], planes[4][0], planes[4][1], planes[4][2], planes[4][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), points[1][0]);
	if (numpoints < 3)
		return;
	numpoints = PolygonF_Clip(numpoints, points[1][0], planes[5][0], planes[5][1], planes[5][2], planes[5][3], 1.0f/64.0f, sizeof(points[0])/sizeof(points[0][0]), v[0]);
	if (numpoints < 3)
		return;
	// some part of the triangle survived, so we have to accept it...
	if (dynamic)
	{
		// dynamic always uses the original triangle
		numpoints = 3;
		for (cornerindex = 0;cornerindex < 3;cornerindex++)
		{
			index = 3*e[cornerindex];
			VectorCopy(vertex3f + index, v[cornerindex]);
		}
	}
	for (cornerindex = 0;cornerindex < numpoints;cornerindex++)
	{
		// convert vertex positions to texcoords
		Matrix4x4_Transform(projection, v[cornerindex], temp);
		tc[cornerindex][0] = (temp[1]+1.0f)*0.5f * (s2-s1) + s1;
		tc[cornerindex][1] = (temp[2]+1.0f)*0.5f * (t2-t1) + t1;
		// calculate distance fade from the projection origin
		f = a * (1.0f-fabs(temp[0])) * cl_decals_newsystem_intensitymultiplier.value;
		f = bound(0.0f, f, 1.0f);
		c[cornerindex][0] = r * f;
		c[cornerindex][1] = g * f;
		c[cornerindex][2] = b * f;
		c[cornerindex][3] = 1.0f;
		//VectorMA(v[cornerindex], cl_decals_bias.value, localnormal, v[cornerindex]);
	}
	if (dynamic)
		R_DecalSystem_SpawnTriangle(decalsystem, v[0], v[1], v[2], tc[0], tc[1], tc[2], c[0], c[1], c[2], triangleindex, surfaceindex, decalsequence);
	else
		for (cornerindex = 0;cornerindex < numpoints-2;cornerindex++)
			R_DecalSystem_SpawnTriangle(decalsystem, v[0], v[cornerindex+1], v[cornerindex+2], tc[0], tc[cornerindex+1], tc[cornerindex+2], c[0], c[cornerindex+1], c[cornerindex+2], -1, surfaceindex, decalsequence);
}
static void R_DecalSystem_SplatEntity(entity_render_t *ent, const vec3_t worldorigin, const vec3_t worldnormal, float r, float g, float b, float a, float s1, float t1, float s2, float t2, float worldsize, unsigned int decalsequence)
{
	matrix4x4_t projection;
	decalsystem_t *decalsystem;
	qbool dynamic;
	model_t *model;
	const msurface_t *surface;
	const msurface_t *surfaces;
	const texture_t *texture;
	int numtriangles;
	int surfaceindex;
	int triangleindex;
	float localorigin[3];
	float localnormal[3];
	float localmins[3];
	float localmaxs[3];
	float localsize;
	//float normal[3];
	float planes[6][4];
	float angles[3];
	bih_t *bih;
	int bih_triangles_count;
	int bih_triangles[256];
	int bih_surfaces[256];

	decalsystem = &ent->decalsystem;
	model = ent->model;
	if (!model || !ent->allowdecals || ent->ralpha < 1 || (ent->crflags & (RENDER_ADDITIVE | RENDER_NODEPTHTEST)))
	{
		R_DecalSystem_Reset(&ent->decalsystem);
		return;
	}

	if (!model->brush.data_leafs && !cl_decals_models.integer)
	{
		if (decalsystem->model)
			R_DecalSystem_Reset(decalsystem);
		return;
	}

	if (decalsystem->model != model)
		R_DecalSystem_Reset(decalsystem);
	decalsystem->model = model;

	RSurf_ActiveModelEntity(ent, q_wants_normals_true, q_wants_tangents_false, q_prepass_false);

	Matrix4x4_Transform(&rsurface.inversematrix, worldorigin, localorigin);
	Matrix4x4_Transform3x3(&rsurface.inversematrix, worldnormal, localnormal);
	VectorNormalize(localnormal);
	localsize = worldsize*rsurface.inversematrixscale;
	localmins[0] = localorigin[0] - localsize;
	localmins[1] = localorigin[1] - localsize;
	localmins[2] = localorigin[2] - localsize;
	localmaxs[0] = localorigin[0] + localsize;
	localmaxs[1] = localorigin[1] + localsize;
	localmaxs[2] = localorigin[2] + localsize;

	//VectorCopy(localnormal, planes[4]);
	//VectorVectors(planes[4], planes[2], planes[0]);
	AnglesFromVectors(angles, localnormal, NULL, q_flip_pitch_false);
	AngleVectors(angles, planes[0], planes[2], planes[4]);
	VectorNegate(planes[0], planes[1]);
	VectorNegate(planes[2], planes[3]);
	VectorNegate(planes[4], planes[5]);
	planes[0][3] = DotProduct(planes[0], localorigin) - localsize;
	planes[1][3] = DotProduct(planes[1], localorigin) - localsize;
	planes[2][3] = DotProduct(planes[2], localorigin) - localsize;
	planes[3][3] = DotProduct(planes[3], localorigin) - localsize;
	planes[4][3] = DotProduct(planes[4], localorigin) - localsize;
	planes[5][3] = DotProduct(planes[5], localorigin) - localsize;

#if 1
// works
{
	matrix4x4_t forwardprojection;
	Matrix4x4_CreateFromQuakeEntity(&forwardprojection, localorigin[0], localorigin[1], localorigin[2], angles[0], angles[1], angles[2], localsize);
	Matrix4x4_Invert_Simple(&projection, &forwardprojection);
}
#else
// broken
{
	float projectionvector[4][3];
	VectorScale(planes[0], ilocalsize, projectionvector[0]);
	VectorScale(planes[2], ilocalsize, projectionvector[1]);
	VectorScale(planes[4], ilocalsize, projectionvector[2]);
	projectionvector[0][0] = planes[0][0] * ilocalsize;
	projectionvector[0][1] = planes[1][0] * ilocalsize;
	projectionvector[0][2] = planes[2][0] * ilocalsize;
	projectionvector[1][0] = planes[0][1] * ilocalsize;
	projectionvector[1][1] = planes[1][1] * ilocalsize;
	projectionvector[1][2] = planes[2][1] * ilocalsize;
	projectionvector[2][0] = planes[0][2] * ilocalsize;
	projectionvector[2][1] = planes[1][2] * ilocalsize;
	projectionvector[2][2] = planes[2][2] * ilocalsize;
	projectionvector[3][0] = -(localorigin[0]*projectionvector[0][0]+localorigin[1]*projectionvector[1][0]+localorigin[2]*projectionvector[2][0]);
	projectionvector[3][1] = -(localorigin[0]*projectionvector[0][1]+localorigin[1]*projectionvector[1][1]+localorigin[2]*projectionvector[2][1]);
	projectionvector[3][2] = -(localorigin[0]*projectionvector[0][2]+localorigin[1]*projectionvector[1][2]+localorigin[2]*projectionvector[2][2]);
	Matrix4x4_FromVectors(&projection, projectionvector[0], projectionvector[1], projectionvector[2], projectionvector[3]);
}
#endif // 0

	dynamic = model->surfmesh.isanimated;
	surfaces = model->data_surfaces;

	bih = NULL;
	bih_triangles_count = -1;
	if (!dynamic)
	{
		if (model->render_bih.numleafs)
			bih = &model->render_bih;
		else if (model->collision_bih.numleafs)
			bih = &model->collision_bih;
	}
	if (bih)
		bih_triangles_count = BIH_GetTriangleListForBox(bih, sizeof(bih_triangles) / sizeof(*bih_triangles), bih_triangles, bih_surfaces, localmins, localmaxs);
	if (bih_triangles_count == 0)
		return;
	if (bih_triangles_count > (int) (sizeof(bih_triangles) / sizeof(*bih_triangles))) // hit too many, likely bad anyway
		return;
	if (bih_triangles_count > 0)
	{
		for (triangleindex = 0; triangleindex < bih_triangles_count; ++triangleindex)
		{
			surfaceindex = bih_surfaces[triangleindex];
			surface = surfaces + surfaceindex;
			texture = surface->texture;
			if (!texture)
				continue;
			if (texture->currentmaterialflags & (MATERIALFLAG_BLENDED | MATERIALFLAG_NODEPTHTEST | MATERIALFLAG_SKY | MATERIALFLAG_SHORTDEPTHRANGE | MATERIALFLAG_WATERSHADER | MATERIALFLAG_REFRACTION))
				continue;
			if (texture->surfaceflags & Q3SURFACEFLAG_NOMARKS)
				continue;
			R_DecalSystem_SplatTriangle(decalsystem, r, g, b, a, s1, t1, s2, t2, decalsequence, dynamic, planes, &projection, bih_triangles[triangleindex], surfaceindex);
		}
	}
	else
	{
		for (surfaceindex = model->submodelsurfaces_start;surfaceindex < model->submodelsurfaces_end;surfaceindex++)
		{
			surface = surfaces + surfaceindex;
			// check cull box first because it rejects more than any other check
			if (!dynamic && !BoxesOverlap(surface->mins, surface->maxs, localmins, localmaxs))
				continue;
			// skip transparent surfaces
			texture = surface->texture;
			if (!texture)
				continue;
			if (texture->currentmaterialflags & (MATERIALFLAG_BLENDED | MATERIALFLAG_NODEPTHTEST | MATERIALFLAG_SKY | MATERIALFLAG_SHORTDEPTHRANGE | MATERIALFLAG_WATERSHADER | MATERIALFLAG_REFRACTION))
				continue;
			if (texture->surfaceflags & Q3SURFACEFLAG_NOMARKS)
				continue;
			numtriangles = surface->num_triangles;
			for (triangleindex = 0; triangleindex < numtriangles; triangleindex++)
				R_DecalSystem_SplatTriangle(decalsystem, r, g, b, a, s1, t1, s2, t2, decalsequence, dynamic, planes, &projection, triangleindex + surface->num_firsttriangle, surfaceindex);
		}
	}
}

// do not call this outside of rendering code - use R_DecalSystem_SplatEntities instead
static void R_DecalSystem_ApplySplatEntities(const vec3_t worldorigin, const vec3_t worldnormal, float r, float g, float b, float a, float s1, float t1, float s2, float t2, float worldsize, unsigned int decalsequence)
{
	RELATED_ (R_DecalSystem_SplatEntities)
	// Baker: RENDER only.  Particle collisions use R_DecalSystem_SplatEntities
	float worldmins[3];
	float worldmaxs[3];

	worldmins[0] = worldorigin[0] - worldsize;
	worldmins[1] = worldorigin[1] - worldsize;
	worldmins[2] = worldorigin[2] - worldsize;
	worldmaxs[0] = worldorigin[0] + worldsize;
	worldmaxs[1] = worldorigin[1] + worldsize;
	worldmaxs[2] = worldorigin[2] + worldsize;

	R_DecalSystem_SplatEntity(r_refdef.scene.worldentity, worldorigin, worldnormal, r, g, b, a, s1, t1, s2, t2, worldsize, decalsequence);

	for (int renderentityindex = 0; renderentityindex < r_refdef.scene.numentities; renderentityindex++) {
		entity_render_t *ent = r_refdef.scene.entities[renderentityindex];
		if (!BoxesOverlap(ent->mins, ent->maxs, worldmins, worldmaxs))
			continue;

		R_DecalSystem_SplatEntity(ent, worldorigin, worldnormal, r, g, b, a, s1, t1, s2, t2, worldsize, decalsequence);
	} // r_refdef.scene.numentities
}

typedef struct r_decalsystem_splatqueue_s
{
	vec3_t worldorigin;
	vec3_t worldnormal;
	float color[4];
	float tcrange[4];
	float worldsize;
	unsigned int decalsequence;
}
r_decalsystem_splatqueue_t;

int r_decalsystem_numqueued = 0;
r_decalsystem_splatqueue_t r_decalsystem_queue[MAX_DECALSYSTEM_QUEUE_1024];

// Baker: particle system queues decals here.
void R_DecalSystem_SplatEntities(const vec3_t worldorigin, const vec3_t worldnormal, float r, float g, float b, float a, float s1, float t1, float s2, float t2, float worldsize)
{
	r_decalsystem_splatqueue_t *queue;

	if (r_decalsystem_numqueued == MAX_DECALSYSTEM_QUEUE_1024)
		return;

	queue = &r_decalsystem_queue[r_decalsystem_numqueued++];
	VectorCopy(worldorigin, queue->worldorigin);
	VectorCopy(worldnormal, queue->worldnormal);
	Vector4Set(queue->color, r, g, b, a);
	Vector4Set(queue->tcrange, s1, t1, s2, t2);
	queue->worldsize = worldsize;
	queue->decalsequence = cl.decalsequence++;
}

CALLERS_ (R_RenderScene ==> R_RenderScene_R_DrawModelDecals => here)
static void R_DecalSystem_ApplySplatEntitiesQueue(void)
{
	int i;
	r_decalsystem_splatqueue_t *queue;

	for (i = 0, queue = r_decalsystem_queue;i < r_decalsystem_numqueued;i++, queue++)
		R_DecalSystem_ApplySplatEntities(queue->worldorigin, queue->worldnormal, queue->color[0], queue->color[1], queue->color[2], queue->color[3], queue->tcrange[0], queue->tcrange[1], queue->tcrange[2], queue->tcrange[3], queue->worldsize, queue->decalsequence);
	r_decalsystem_numqueued = 0;
}

extern cvar_t cl_decals_max;
static void R_DrawModelDecals_FadeEntity(entity_render_t *ent)
{
	int i;
	decalsystem_t *decalsystem = &ent->decalsystem;
	unsigned int killsequence;
	tridecal_t *decal;
	float frametime;
	float lifetime;

	if (!decalsystem->numdecals)
		return;

	if (r_showsurfaces.integer)
		return;

	if (ent->model != decalsystem->model || ent->ralpha < 1 || Have_Flag (ent->crflags, RENDER_ADDITIVE)) {
		R_DecalSystem_Reset(decalsystem);
		return;
	}

	killsequence = cl.decalsequence - bound(1, (unsigned int) cl_decals_max.integer, cl.decalsequence);
	lifetime = cl_decals_time.value + cl_decals_fadetime.value;

	if (decalsystem->lastupdatetime)
		frametime = (r_refdef.scene.time - decalsystem->lastupdatetime);
	else
		frametime = 0;
	decalsystem->lastupdatetime = r_refdef.scene.time;

	for (i = 0, decal = decalsystem->decals;i < decalsystem->numdecals;i++, decal++) {
			decal->lived += frametime;
			if (killsequence > decal->decalsequence || decal->lived >= lifetime)
			{
			*decal = decalsystem->decals[--decalsystem->numdecals];
			--i, --decal;  // Consider the just moved decal next.
		}
	}

	if (decalsystem->numdecals <= 0)
	{
		// if there are no decals left, reset decalsystem
		R_DecalSystem_Reset(decalsystem);
	}
}

extern skinframe_t *decalskinframe;
static void R_DrawModelDecals_Entity(entity_render_t *ent)
{
	int i;
	decalsystem_t *decalsystem = &ent->decalsystem;
	int numdecals;
	tridecal_t *decal;
	float faderate;
	float alpha;
	float *v3f;
	float *c4f;
	float *t2f;
	const int *e;
	const unsigned char *surfacevisible = ent == r_refdef.scene.worldentity ? r_refdef.viewcache.world_surfacevisible : NULL;
	int numtris = 0;

	numdecals = decalsystem->numdecals;
	if (!numdecals)
		return;

	if (r_showsurfaces.integer)
		return;

	if (ent->model != decalsystem->model || ent->ralpha < 1 || Have_Flag(ent->crflags, RENDER_ADDITIVE)) {
		R_DecalSystem_Reset(decalsystem);
		return;
	}

	// if the model is static it doesn't matter what value we give for
	// wantnormals and wanttangents, so this logic uses only rules applicable
	// to a model, knowing that they are meaningless otherwise
	RSurf_ActiveModelEntity(ent, q_wants_normals_false, q_wants_tangents_false, q_prepass_false);

	decalsystem->lastupdatetime = r_refdef.scene.time;

	faderate = 1.0f / max(0.001f, cl_decals_fadetime.value);

	// update vertex positions for animated models
	v3f = decalsystem->vertex3f;
	c4f = decalsystem->color4f;
	t2f = decalsystem->texcoord2f;
	for (i = 0, decal = decalsystem->decals;i < numdecals;i++, decal++) {
		if (!decal->color4f[0][3])
			continue;

		if (surfacevisible && !surfacevisible[decal->surfaceindex])
			continue;

		// skip backfaces
		if (decal->triangleindex < 0 && DotProduct(r_refdef.view.origin, decal->plane) < decal->plane[3])
			continue;

		// update color values for fading decals
		if (decal->lived >= cl_decals_time.value)
			alpha = 1 - faderate * (decal->lived - cl_decals_time.value);
		else
			alpha = 1.0f;

		c4f[ 0] = decal->color4f[0][0] * alpha;
		c4f[ 1] = decal->color4f[0][1] * alpha;
		c4f[ 2] = decal->color4f[0][2] * alpha;
		c4f[ 3] = 1;
		c4f[ 4] = decal->color4f[1][0] * alpha;
		c4f[ 5] = decal->color4f[1][1] * alpha;
		c4f[ 6] = decal->color4f[1][2] * alpha;
		c4f[ 7] = 1;
		c4f[ 8] = decal->color4f[2][0] * alpha;
		c4f[ 9] = decal->color4f[2][1] * alpha;
		c4f[10] = decal->color4f[2][2] * alpha;
		c4f[11] = 1;

		t2f[0] = decal->texcoord2f[0][0];
		t2f[1] = decal->texcoord2f[0][1];
		t2f[2] = decal->texcoord2f[1][0];
		t2f[3] = decal->texcoord2f[1][1];
		t2f[4] = decal->texcoord2f[2][0];
		t2f[5] = decal->texcoord2f[2][1];

		// update vertex positions for animated models
		if (decal->triangleindex >= 0 && decal->triangleindex < rsurface.modelnumtriangles)
		{
			e = rsurface.modelelement3i + 3*decal->triangleindex;
			VectorCopy(rsurface.modelvertex3f + 3*e[0], v3f);
			VectorCopy(rsurface.modelvertex3f + 3*e[1], v3f + 3);
			VectorCopy(rsurface.modelvertex3f + 3*e[2], v3f + 6);
		}
		else
		{
			VectorCopy(decal->vertex3f[0], v3f);
			VectorCopy(decal->vertex3f[1], v3f + 3);
			VectorCopy(decal->vertex3f[2], v3f + 6);
		}

		if (r_refdef.fogenabled)
		{
			alpha = RSurf_FogVertex(v3f);
			VectorScale(c4f, alpha, c4f);
			alpha = RSurf_FogVertex(v3f + 3);
			VectorScale(c4f + 4, alpha, c4f + 4);
			alpha = RSurf_FogVertex(v3f + 6);
			VectorScale(c4f + 8, alpha, c4f + 8);
		}

		v3f += 9;
		c4f += 12;
		t2f += 6;
		numtris++;
	}

	if (numtris > 0)
	{
		r_refdef.stats[r_stat_drawndecals] += numtris;

		// now render the decals all at once
		// (this assumes they all use one particle font texture!)
		RSurf_ActiveCustomEntity(&rsurface.matrix, &rsurface.inversematrix, rsurface.ent_flags, ent->shadertime, 1, 1, 1, 1, numdecals*3, decalsystem->vertex3f, decalsystem->texcoord2f, NULL, NULL, NULL, decalsystem->color4f, numtris, decalsystem->element3i, decalsystem->element3s, false, false);
//		R_Mesh_ResetTextureState();
		R_Mesh_PrepareVertices_Generic_Arrays(numtris * 3, decalsystem->vertex3f, decalsystem->color4f, decalsystem->texcoord2f);
		GL_DepthMask(false);
		GL_DepthRange(0, 1);
		GL_PolygonOffset(rsurface.basepolygonfactor + r_polygonoffset_decals_factor.value, rsurface.basepolygonoffset + r_polygonoffset_decals_offset.value);
		GL_DepthTest(true);
		GL_CullFace(GL_NONE);
		GL_BlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR); // filter
		R_SetupShader_Generic(decalskinframe->base, false, false, false);
		R_Mesh_Draw(0, numtris * 3, 0, numtris, decalsystem->element3i, NULL, 0, decalsystem->element3s, NULL, 0);
	}
}


static void R_DrawDebugModel(void)
{
	entity_render_t *ent = rsurface.entity;
	int j, flagsmask;
	const msurface_t *surface;
	model_t *model = ent->model;

	if (!sv.active  && !cls.demoplayback && ent != r_refdef.scene.worldentity)
		return;

	if (r_showoverdraw.value > 0)
	{
		float c = r_refdef.view.colorscale * r_showoverdraw.value * 0.125f;
		flagsmask = MATERIALFLAG_SKY | MATERIALFLAG_WALL;
		R_SetupShader_Generic_NoTexture(false, false);
		GL_DepthTest(false);
		GL_DepthMask(false);
		GL_DepthRange(0, 1);
		GL_BlendFunc(GL_ONE, GL_ONE);
		for (j = model->submodelsurfaces_start;j < model->submodelsurfaces_end;j++)
		{
			if (ent == r_refdef.scene.worldentity && !r_refdef.viewcache.world_surfacevisible[j])
				continue;
			surface = model->data_surfaces + j;
			rsurface.texture = R_GetCurrentTexture(surface->texture);
			if ((rsurface.texture->currentmaterialflags & flagsmask) && surface->num_triangles)
			{
				RSurf_PrepareVerticesForBatch(BATCHNEED_ARRAY_VERTEX | BATCHNEED_NOGAPS, 1, &surface);
				GL_CullFace((rsurface.texture->currentmaterialflags & MATERIALFLAG_NOCULLFACE) ? GL_NONE : r_refdef.view.cullface_back);
				if ((rsurface.texture->currentmaterialflags & MATERIALFLAG_BLENDED))
					GL_Color(c, 0, 0, 1.0f);
				else if (ent == r_refdef.scene.worldentity)
					GL_Color(c, c, c, 1.0f);
				else
					GL_Color(0, c, 0, 1.0f);
				R_Mesh_PrepareVertices_Generic_Arrays(rsurface.batchnumvertices, rsurface.batchvertex3f, NULL, NULL);
				RSurf_DrawBatch();
			}
		}
		rsurface.texture = NULL;
	}

	flagsmask = MATERIALFLAG_SKY | MATERIALFLAG_WALL;

//	R_Mesh_ResetTextureState();
	R_SetupShader_Generic_NoTexture(false, false);
	GL_DepthRange(0, 1);
	GL_DepthTest(!r_showdisabledepthtest.integer);
	GL_DepthMask(false);
	GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (r_showcollisionbrushes.value /*d:0*/ > 0 && model->collision_bih.numleafs) {
		int triangleindex;
		int bihleafindex;
		qbool cullbox = false;
		const q3mbrush_t *brush;
		const bih_t *bih = &model->collision_bih;
		const bih_leaf_t *bihleaf;
		float vertex3f[3][3];
		GL_PolygonOffset(r_refdef.polygonfactor + r_showcollisionbrushes_polygonfactor.value, r_refdef.polygonoffset + r_showcollisionbrushes_polygonoffset.value);
		for (bihleafindex = 0, bihleaf = bih->leafs;bihleafindex < bih->numleafs;bihleafindex++, bihleaf++)
		{
			if (cullbox && R_CullFrustum(bihleaf->mins, bihleaf->maxs))
				continue;
			switch (bihleaf->type)
			{
			case BIH_BRUSH:
				brush = model->brush.data_brushes + bihleaf->itemindex;
				if (brush->colbrushf && brush->colbrushf->numtriangles)
				{
					GL_Color((bihleafindex & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 5) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 10) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, r_showcollisionbrushes.value);
					R_Mesh_PrepareVertices_Generic_Arrays(brush->colbrushf->numpoints, brush->colbrushf->points->v, NULL, NULL);
					R_Mesh_Draw(0, brush->colbrushf->numpoints, 0, brush->colbrushf->numtriangles, brush->colbrushf->elements, NULL, 0, NULL, NULL, 0);
				}
				break;
			case BIH_COLLISIONTRIANGLE:
				triangleindex = bihleaf->itemindex;
				VectorCopy(model->brush.data_collisionvertex3f + 3*model->brush.data_collisionelement3i[triangleindex*3+0], vertex3f[0]);
				VectorCopy(model->brush.data_collisionvertex3f + 3*model->brush.data_collisionelement3i[triangleindex*3+1], vertex3f[1]);
				VectorCopy(model->brush.data_collisionvertex3f + 3*model->brush.data_collisionelement3i[triangleindex*3+2], vertex3f[2]);
				GL_Color((bihleafindex & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 5) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 10) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, r_showcollisionbrushes.value);
				R_Mesh_PrepareVertices_Generic_Arrays(3, vertex3f[0], NULL, NULL);
				R_Mesh_Draw(0, 3, 0, 1, polygonelement3i, NULL, 0, polygonelement3s, NULL, 0);
				break;
			case BIH_RENDERTRIANGLE:
				triangleindex = bihleaf->itemindex;
				VectorCopy(model->surfmesh.data_vertex3f + 3*model->surfmesh.data_element3i[triangleindex*3+0], vertex3f[0]);
				VectorCopy(model->surfmesh.data_vertex3f + 3*model->surfmesh.data_element3i[triangleindex*3+1], vertex3f[1]);
				VectorCopy(model->surfmesh.data_vertex3f + 3*model->surfmesh.data_element3i[triangleindex*3+2], vertex3f[2]);
				GL_Color((bihleafindex & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 5) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, ((bihleafindex >> 10) & 31) * (1.0f / 32.0f) * r_refdef.view.colorscale, r_showcollisionbrushes.value);
				R_Mesh_PrepareVertices_Generic_Arrays(3, vertex3f[0], NULL, NULL);
				R_Mesh_Draw(0, 3, 0, 1, polygonelement3i, NULL, 0, polygonelement3s, NULL, 0);
				break;
			}
		}
	}

	GL_PolygonOffset(r_refdef.polygonfactor, r_refdef.polygonoffset);

#ifndef USE_GLES2
	if ((r_trisentity.integer > 0 || r_showtris.value > 0) && qglPolygonMode)
	{
		if (r_showdisabledepthtest.integer)
		{
			GL_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			GL_DepthMask(false);
		}
		else
		{
			GL_BlendFunc(GL_ONE, GL_ZERO);
			GL_DepthMask(true);
		}
		qglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);CHECKGLERROR
		for (j = model->submodelsurfaces_start; j < model->submodelsurfaces_end; j++)
		{
			if (ent == r_refdef.scene.worldentity && !r_refdef.viewcache.world_surfacevisible[j])
				continue;
			surface = model->data_surfaces + j;
			rsurface.texture = R_GetCurrentTexture(surface->texture);
			if ((rsurface.texture->currentmaterialflags & flagsmask) && surface->num_triangles)
			{
				RSurf_PrepareVerticesForBatch(BATCHNEED_ARRAY_VERTEX | BATCHNEED_ARRAY_NORMAL | BATCHNEED_ARRAY_VECTOR | BATCHNEED_NOGAPS, 1, &surface);
				if ((rsurface.texture->currentmaterialflags & MATERIALFLAG_BLENDED))
					GL_Color(r_refdef.view.colorscale, 0, 0, r_showtris.value);
				else if (ent == r_refdef.scene.worldentity)
					GL_Color(r_refdef.view.colorscale, r_refdef.view.colorscale, r_refdef.view.colorscale, r_showtris.value);
				else
					GL_Color(0, r_refdef.view.colorscale, 0, r_showtris.value);
				R_Mesh_PrepareVertices_Generic_Arrays(rsurface.batchnumvertices, rsurface.batchvertex3f, NULL, NULL);
				RSurf_DrawBatch();
			}
		}
		qglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);CHECKGLERROR
		rsurface.texture = NULL;
	}

	// Baker: See DarkPlaces Beta gl_rmain.c for #if 0 that starts // FIXME!  implement r_shownormals with just triangles
	// Since it was #if 0 I removed it because it ate up to much space obscuring the live code.


#endif // #ifndef USE_GLES2
} // R_DrawDebugModel


static void R_RenderScene_R_DrawModelDecals(void)
{
	int i, numdecals;

	// fade faster when there are too many decals
	numdecals = r_refdef.scene.worldentity->decalsystem.numdecals;
	for (i = 0;i < r_refdef.scene.numentities;i++)
		numdecals += r_refdef.scene.entities[i]->decalsystem.numdecals;

	R_DrawModelDecals_FadeEntity(r_refdef.scene.worldentity);
	for (i = 0;i < r_refdef.scene.numentities;i++)
		if (r_refdef.scene.entities[i]->decalsystem.numdecals)
			R_DrawModelDecals_FadeEntity(r_refdef.scene.entities[i]);

	R_DecalSystem_ApplySplatEntitiesQueue();

	numdecals = r_refdef.scene.worldentity->decalsystem.numdecals;
	for (i = 0;i < r_refdef.scene.numentities;i++)
		numdecals += r_refdef.scene.entities[i]->decalsystem.numdecals;

	r_refdef.stats[r_stat_totaldecals] += numdecals;

	if (r_showsurfaces.integer || !r_drawdecals.integer)
		return;

	R_DrawModelDecals_Entity(r_refdef.scene.worldentity);

	for (i = 0; i < r_refdef.scene.numentities; i ++) {
		if (!r_refdef.viewcache.entityvisible[i])
			continue;
		if (r_refdef.scene.entities[i]->decalsystem.numdecals)
			R_DrawModelDecals_Entity(r_refdef.scene.entities[i]);
	} // r_refdef.scene.numentities
} // R_RenderScene_R_DrawModelDecals



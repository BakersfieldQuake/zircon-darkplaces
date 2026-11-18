// filematch_csg_procs.c.h

void bounds_expand (const vec3_t v, vec3_t loz, vec3_t hiz)
{
	if (v[0] > hiz[0])  hiz[0] = v[0];
	if (v[1] > hiz[1])  hiz[1] = v[1];
	if (v[2] > hiz[2])  hiz[2] = v[2];

	if (v[0] < loz[0])  loz[0] = v[0];
	if (v[1] < loz[1])  loz[1] = v[1];
	if (v[2] < loz[2])  loz[2] = v[2];
}

CALLERS_ (VM_map_entity_get_mins_maxs)

void bounds_init_9 (vec3_t your_mins, vec3_t your_maxs)
{
	Vector3Set (your_maxs, -999999, -999999, -999999);
	Vector3Set (your_mins,  999999,  999999,  999999);
}

void PlaneFromPoints(face_plane_t *plane, const vec3_t p0, const vec3_t p1, const vec3_t p2) 
{
    vec3_t d1, d2, cross;
    VectorSubtract(p1, p0, d1);
    VectorSubtract(p2, p0, d2);
    CrossProduct(d1, d2, cross);
    VectorNormalize(cross);
    plane->normal[0] = cross[0]; plane->normal[1] = cross[1]; plane->normal[2] = cross[2];
    plane->dist = DotProduct(plane->normal, p0);
}

// Helper to generate 3 points on a plane for Quake 3 .map serialization
void PointsFromPlane(const face_plane_t *plane, vec3_t p0, vec3_t p1, vec3_t p2) {
    for (int i = 0; i < 3; i++) {
        p0[i] = plane->normal[i] * plane->dist;
    }
    vec3_t up = {0.0f, 0.0f, 1.0f};
    if (fabsf(plane->normal[2]) > 0.9f) {
        up[0] = 1.0f; up[2] = 0.0f;
    }
    vec3_t tangent, bitangent;
    CrossProduct(up, plane->normal, tangent);
    VectorNormalize(tangent);
    CrossProduct(plane->normal, tangent, bitangent);

    for (int i = 0; i < 3; i++) {
        p1[i] = p0[i] + tangent[i] * 64.0f;
        p2[i] = p0[i] + bitangent[i] * 64.0f;
    }
}


void PlaneReverse (face_plane_t *plane)
{
	plane->normal[0] = -plane->normal[0];	// -cutter_plane.normal[0];
	plane->normal[1] = -plane->normal[1];	// -cutter_plane.normal[1];
	plane->normal[2] = -plane->normal[2];	// -cutter_plane.normal[1];
	plane->dist = -plane->dist;				// -cutter_plane.dist;
}

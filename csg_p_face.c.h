// csg_p_face.c.h



//1.0  — flat floor
//0.8  — gentle hill
//0.5  — steep hill
//0.1  — nearly vertical, but slightly upward-facing
//-1.0 - face down

qbool face_is_up (brushrow_s *face) {
	//qbool is_up = face->face_plane.normal[2] > 0; // Z
	qbool is_up = face->face_plane.normal[2] < 0; // It's inverted and could affect carve
	return is_up;
}

qbool face_is_down (brushrow_s *face) {
	//qbool is_down = face->face_plane.normal[2] < 0; // Z
	qbool is_down = face->face_plane.normal[2] > 0; // It's inverted and could affect carve
	return is_down;
}

ccs *face_direction(const brush_s *brush, int facenum, const brushrow_s *face)
{
	if (facenum >= 6)
		return "X";
	//int x = brush->cubeoid_index[facenum];
	int bits = brush->triangle_face_bits;

	if (brush->brushrowlist.count == 5 && brush->sloped_face_index == facenum)
		return "Sloped";

	if (brush->brushrowlist.count == 5 && brush->top_slanted_index == facenum)
		return "SlantTop";

	if (Have_Flag (bits, TEXSIDE_WEST_8) && facenum == brush->cubeoid_index[FACE_WEST_0])
		return "West";
	if (Have_Flag (bits, TEXSIDE_EAST_4) && facenum == brush->cubeoid_index[FACE_EAST_1])
		return "East";
	if (Have_Flag (bits, TEXSIDE_SOUTH_2) && facenum == brush->cubeoid_index[FACE_SOUTH_2])
		return "South";
	if (Have_Flag (bits, TEXSIDE_NORTH_1) && facenum == brush->cubeoid_index[FACE_NORTH_3])
		return "North";
	if (Have_Flag (bits, TEXSIDE_FLOOR_32) && facenum == brush->cubeoid_index[FACE_FLOOR_4])
		return "Floor";
	if (Have_Flag (bits, TEXSIDE_CEIL_16) && facenum == brush->cubeoid_index[FACE_CEIL_5])
		return "Ceil";

	return "Non";
}


// faceup is 1.0 and facedown is -1.0
qbool face_is_up_or_down (brushrow_s *face) {
	qbool is_up_or_down = face->face_plane.normal[2] != 0; // 0.0 is vertical wall.  C 0 == -0
	return is_up_or_down;
}

qbool face_is_flat (brushrow_s *r_face)
{
	// This can be upside down too.
	qbool is_flat = (r_face->a[2] == r_face->b[2] && r_face->b[2] == r_face->c[2]);
	return is_flat;
}

void face_copy_data (brushrow_s *d_face, const brushrow_s *r_face_src)
{
	memcpy(d_face, r_face_src, sizeof(*d_face));

	// Baker: This one must be separately allocated
    d_face->pbrtexture = Z_StrDup(r_face_src->pbrtexture);
}

void face_copy_texturedata (brushrow_s *dest, const brushrow_s *src)
{
    /* Copy vec4_t texture axes using Vector4CopyDestSrc */
    Vector4CopyDestSrc(dest->tex_axis_s, src->tex_axis_s);
    Vector4CopyDestSrc(dest->tex_axis_t, src->tex_axis_t);

    /* Copy trail count */
    dest->trail_count = src->trail_count;

    /* Copy faceparms array using memcpy */
    memcpy(dest->faceparms, src->faceparms, sizeof(src->faceparms));

	Mem_FreeNull_ (dest->pbrtexture);
	dest->pbrtexture = Z_StrDup(src->pbrtexture);
}


void face_texture_set (brushrow_s *d_face, ccs *s)
{
	Z_StrDup_Realloc (&d_face->pbrtexture, s);
}

void face_texture_append (brushrow_s *d_face, ccs *extension)
{
	va_copy_path_64_Q3 (newname, d_face->pbrtexture);
	dp_strlcat (newname, extension, sizeof(newname));
	Z_StrDup_Realloc (&d_face->pbrtexture, newname);
}

void face_texture_set_cat (brushrow_s *d_face, ccs *basename, ccs *extension)
{
	va_super (newname, Q3PATHLENGTH_64, "%s%s", basename, extension);
	Z_StrDup_Realloc (&d_face->pbrtexture, newname);
}

void face_texture_set_fmt (brushrow_s *d_face, ccs *fmt, ...)
{
	VA_EXPAND_ALLOC (text, text_slen, bufsiz, fmt);
	face_texture_set (d_face, text);
	VA_EXPAND_ALLOC_FREE (text);
}

// Nov 4 2026 - old adjacents used this, nothing uses it now
static qbool face_bbox_touch_point(brushrow_s *f1, vec3_t p)
{
	qbool does_touch = BOX_XY_Touch(f1->bfmins, f1->bfmaxs, p, p);
	return does_touch;
}

// Nov 4 2026 - old adjacents used this, nothing uses it now
// Nov 11 2026 - Used again
qbool face_bbox_touch_face(brushrow_s *f1, brushrow_s *f2)
{
	qbool does_touch = BOX_XY_Touch(f1->bfmins, f1->bfmaxs, f2->bfmins, f2->bfmaxs);
	return does_touch;
}

#define FACE_ADJACENCY_EPSILON 0.01
#define FACE_ADJACENCY_ANGLE_EPSILON 0.001

static qbool Face_Bounds_Touch (brushrow_s *a, brushrow_s *b)
{
    int i;

    for (i = 0; i < 3; i++) {
        if (a->bfmaxs[i] < b->bfmins[i] - FACE_ADJACENCY_EPSILON)
            return false;

        if (a->bfmins[i] > b->bfmaxs[i] + FACE_ADJACENCY_EPSILON)
            return false;
    }

    return true;
}


static qbool Face_Edges_Overlap (vec3_t a1, vec3_t a2,
                                  vec3_t b1, vec3_t b2)
{
    vec3_t u, v, cross, delta;
    float len_u, len_v;
    float len_u2, len_v2;
    float dot1, dot2;
    float overlap;
    float eps = FACE_ADJACENCY_EPSILON;

    VectorSubtract(a2, a1, u);
    VectorSubtract(b2, b1, v);

    len_u2 = DotProduct(u, u);
    len_v2 = DotProduct(v, v);

    if (len_u2 <= eps * eps || len_v2 <= eps * eps)
        return false;

    len_u = sqrtf(len_u2);
    len_v = sqrtf(len_v2);

    // Edges must be parallel.
    CrossProduct(u, v, cross);

    if (DotProduct(cross, cross) >
        FACE_ADJACENCY_ANGLE_EPSILON *
        FACE_ADJACENCY_ANGLE_EPSILON * len_u2 * len_v2)
        return false;

    // Both B endpoints must lie on A's edge line.
    VectorSubtract(b1, a1, delta);

    if (DotProduct(delta, delta) -
        DotProduct(delta, u) * DotProduct(delta, u) / len_u2 > eps * eps)
        return false;

    VectorSubtract(b2, a1, delta);

    if (DotProduct(delta, delta) -
        DotProduct(delta, u) * DotProduct(delta, u) / len_u2 > eps * eps)
        return false;

    // Project B's endpoints onto A's edge.
    VectorSubtract(b1, a1, delta);
    dot1 = DotProduct(delta, u) / len_u;

    VectorSubtract(b2, a1, delta);
    dot2 = DotProduct(delta, u) / len_u;

    // Calculate the amount of shared edge.
    //overlap = fminf(len_u, fmaxf(dot1, dot2))
    //        - fmaxf(0.0f, fminf(dot1, dot2));

    overlap = min(len_u, max(dot1, dot2))
            - max(0.0f, min(dot1, dot2));

    return overlap > eps;
}


qbool Faces_Are_Adjacent (brushrow_s *a, brushrow_s *b)
{
    int i, j;

    if (!a || !b || a == b)
        return false;

    if (a->num_points < 3 || b->num_points < 3)
        return false;

    // Quick bounding-box rejection.
    if (!Face_Bounds_Touch(a, b))
        return false;

    // Compare every edge against every edge.
    for (i = 0; i < a->num_points; i++) {
        vec3_t a1, a2;

        VectorCopy(a->points[i], a1);
        VectorCopy(a->points[(i + 1) % a->num_points], a2);

        for (j = 0; j < b->num_points; j++) {
            vec3_t b1, b2;

            VectorCopy(b->points[j], b1);
            VectorCopy(b->points[(j + 1) % b->num_points], b2);

            if (Face_Edges_Overlap(a1, a2, b1, b2))
                return true;
        }
    }

    return false;
}

qbool faces_ptr_bbox_touch (const brushrow_s *face1, const brushrow_s *face2)
{
    return Boxes_Touch_3D(
        face1->bfmins,
        face1->bfmaxs,
        face2->bfmins,
        face2->bfmaxs);
}

qbool faces_bbox_touch (map_t *hm, int entnum1, int brushnum1, int facenum1, int entnum2, int brushnum2, int facenum2)
{
	FACE_DEFINE_CONST (face1, hm, entnum1, brushnum1, facenum1);
	FACE_DEFINE_CONST (face2, hm, entnum2, brushnum2, facenum2);
	return faces_ptr_bbox_touch (face1, face2);
}




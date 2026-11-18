// filematch_sigmoid.c.h

#if 1



RELATED_ (CrossProduct)

// Vector helper function: Normalize vector to unit length
static float m_VectorNormalize(vec3_t v) {
    float length = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length > 0.00001f) {
        float ilength = 1.0f / length;
        v[0] *= ilength;
        v[1] *= ilength;
        v[2] *= ilength;
    } else {
        v[0] = 0.0f; v[1] = 0.0f; v[2] = 1.0f;
    }
    return length;
}

// Generates 3D Roller Coaster Loop geometry, direction vectors, and calculates total arc length
double Evaluate_Quake3_RollerCoaster_Loop_Old(
    const vec3_t start,
    const vec3_t end,
    float loop_height,
    float y_offset,
    float t,
    vec3_t out_origin,
    vec3_t out_forward,
    vec3_t out_up
) {
    // 1. Clamp input parametric t
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // 2. Compute path angle theta [0, 2*PI] and out-of-plane twist angle phi [0, PI]
    double theta = (double)t * 2.0 * M_PI;
    double phi = (double)t * M_PI;

    // Base position components along unit loop [0, 1]
    double x_rel = (1.0 - cos(theta)) * 0.5;                             // Smooth progress X
    double y_rel = (1.0 - cos(phi)) * 0.5 * (double)y_offset;            // Lateral corkscrew drift
    double z_rel = pow(sin(theta * 0.5), 2.0) * (double)loop_height;     // Vertical height envelope

    // 3. World Frame Transformations (Linear Interpolation baseline + Loop Offset)
    out_origin[0] = start[0] + (end[0] - start[0]) * (float)x_rel;
    out_origin[1] = start[1] + (end[1] - start[1]) * (float)x_rel + (float)y_rel;
    out_origin[2] = start[2] + (end[2] - start[2]) * (float)x_rel + (float)z_rel;

    // 4. Calculate Derivatives w.r.t 't' to derive local Forward Tangent Vector
    double dx_dt = sin(theta) * M_PI * (double)(end[0] - start[0]);
    double dy_dt = sin(theta) * M_PI * (double)(end[1] - start[1]) + sin(phi) * 0.5 * M_PI * (double)y_offset;
    double dz_dt = sin(theta) * M_PI * (double)(end[2] - start[2]) + sin(theta) * M_PI * (double)loop_height;

    out_forward[0] = (float)dx_dt;
    out_forward[1] = (float)dy_dt;
    out_forward[2] = (float)dz_dt;
    m_VectorNormalize(out_forward);

    // 5. Calculate Local Up Vector (Rotates 360 degrees inside the loop)
    // Primary track normal in local pitch space:
    vec3_t local_up;
    local_up[0] = (float)(-sin(theta));
    local_up[1] = (float)(sin(phi) * 0.2); // Subtle lateral lean
    local_up[2] = (float)(cos(theta));

    // Construct orthonormal basis (Forward x Up -> Right, Right x Forward -> Clean Up)
    vec3_t right;
    VectorCross(out_forward, local_up, right);
    VectorNormalize(right);
    VectorCross(right, out_forward, out_up);
    VectorNormalize(out_up);

    // 6. Integrate Total Arc Length via Gauss-Legendre Quadrature (5 Points)
    static const double x_nodes[5] = { 0.046910077, 0.230765345, 0.500000000, 0.769234655, 0.953089923 };
    static const double weights[5] = { 0.118463443, 0.239314335, 0.284444444, 0.239314335, 0.118463443 };

    double total_length = 0.0;
    for (int i = 0; i < 5; ++i) {
        double node_t = x_nodes[i];
        double node_theta = node_t * 2.0 * M_PI;
        double node_phi = node_t * M_PI;

        double node_dx = sin(node_theta) * M_PI * (double)(end[0] - start[0]);
        double node_dy = sin(node_theta) * M_PI * (double)(end[1] - start[1]) + sin(node_phi) * 0.5 * M_PI * (double)y_offset;
        double node_dz = sin(node_theta) * M_PI * (double)(end[2] - start[2]) + sin(node_theta) * M_PI * (double)loop_height;

        double ds_dt = sqrt(node_dx * node_dx + node_dy * node_dy + node_dz * node_dz);
        total_length += weights[i] * ds_dt;
    }

    return total_length;
}


#endif // 1

// Evaluates the logistic sigmoid position at 'fraction' and returns the total curve surface length
double Lerp_Quake3_Logistic_Sigmoid_Return_Sigmoid_Length(
    const vec3_t start,
    const vec3_t end,
    float fraction,
    float sigmoid_k,
    vec3_t out
) {
    // 1. Clamp fraction between 0.0 and 1.0
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    // Convert sigmoid_k to double for calculation stability
    double k = (double)sigmoid_k;

    // 2. Compute Normalization factors for Logistic Sigmoid: S(t) = 1 / (1 + exp(-k * (t - 0.5)))
    double s0 = 1.0 / (1.0 + exp(k * 0.5));        // Value at t = 0
    double s1 = 1.0 / (1.0 + exp(-k * 0.5));       // Value at t = 1
    double range = s1 - s0;

    // 3. Calculate normalized Sigmoid factor at given fraction 't'
    double raw_s = 1.0 / (1.0 + exp(-k * ((double)fraction - 0.5)));
    double smooth_z = (raw_s - s0) / range;

    // 4. Calculate 3D output point
    out[0] = start[0] + (end[0] - start[0]) * fraction; // Linear X
    out[1] = start[1] + (end[1] - start[1]) * fraction; // Linear Y
    out[2] = (float)((double)start[2] + ((double)end[2] - (double)start[2]) * smooth_z); // Sigmoid Z

    // 5. Compute horizontal plane distance (L_xy) and height difference (dz)
    double dx = (double)end[0] - (double)start[0];
    double dy = (double)end[1] - (double)start[1];
    double L_xy = sqrt(dx * dx + dy * dy);
    double dz = (double)end[2] - (double)start[2];

    // 6. Gauss-Legendre Quadrature (5 points) to integrate arc length along [0, 1]
    static const double x_nodes[5] = {
        0.046910077030668,
        0.230765344947158,
        0.500000000000000,
        0.769234655052842,
        0.953089922969332
    };
    static const double weights[5] = {
        0.118463442528095,
        0.239314335249683,
        0.284444444444444,
        0.239314335249683,
        0.118463442528095
    };

    double total_length = 0.0;

    for (int i = 0; i < 5; ++i) {
        double t = x_nodes[i];

        // Compute normalized derivative dS/dt at integration node t
        double exp_term = exp(-k * (t - 0.5));
        double denom = 1.0 + exp_term;
        double dS_dt = (k * exp_term / (denom * denom)) / range;

        // Instantaneous speed ds/dt = sqrt( (dL_xy/dt)^2 + (dz * dS/dt)^2 )
        double dz_dt = dz * dS_dt;
        double ds_dt = sqrt(L_xy * L_xy + dz_dt * dz_dt);

        total_length += weights[i] * ds_dt;
    }

    return total_length;
}

#include <math.h>

typedef float vec3_t[3];

// Evaluates point position, curve surface length, and local upward surface normal vector
double Lerp_Quake3_Logistic_Sigmoid_Return_Sigmoid_Length2(
    const vec3_t start,
    const vec3_t end,
    float fraction,
    float sigmoid_k,
    vec3_t out,
    vec3_t out_vector3up
) {
    // 1. Clamp fraction between 0.0 and 1.0
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    double k = (double)sigmoid_k;

    // 2. Normalization factors for Logistic Sigmoid: S(t) = 1 / (1 + exp(-k * (t - 0.5)))
    double s0 = 1.0 / (1.0 + exp(k * 0.5));        // Value at t = 0
    double s1 = 1.0 / (1.0 + exp(-k * 0.5));       // Value at t = 1
    double range = s1 - s0;

    // 3. Calculate normalized Sigmoid factor at given fraction 't'
    double raw_s = 1.0 / (1.0 + exp(-k * ((double)fraction - 0.5)));
    double smooth_z = (raw_s - s0) / range;

    // 4. Calculate 3D output point
    out[0] = start[0] + (end[0] - start[0]) * fraction; // Linear X
    out[1] = start[1] + (end[1] - start[1]) * fraction; // Linear Y
    out[2] = (float)((double)start[2] + ((double)end[2] - (double)start[2]) * smooth_z); // Sigmoid Z

    // 5. Delta components
    double dx = (double)end[0] - (double)start[0];
    double dy = (double)end[1] - (double)start[1];
    double dz = (double)end[2] - (double)start[2];
    double L_xy = sqrt(dx * dx + dy * dy);

    // 6. Calculate local tangent & normal (up vector) at 'fraction'
    double exp_term_f = exp(-k * ((double)fraction - 0.5));
    double denom_f = 1.0 + exp_term_f;
    double dS_dt_f = (k * exp_term_f / (denom_f * denom_f)) / range;

    if (L_xy > 0.00001) {
        // Horizontal unit direction vector
        double dir_x = dx / L_xy;
        double dir_y = dy / L_xy;
        double dz_dt = dz * dS_dt_f;

        // Up vector perpendicular to the slope: (-dz_dt * dir_x, -dz_dt * dir_y, L_xy)
        double up_x = -dz_dt * dir_x;
        double up_y = -dz_dt * dir_y;
        double up_z = L_xy;

        // Normalize to unit vector
        double up_len = sqrt(up_x * up_x + up_y * up_y + up_z * up_z);
        out_vector3up[0] = (float)(up_x / up_len);
        out_vector3up[1] = (float)(up_y / up_len);
        out_vector3up[2] = (float)(up_z / up_len);
    } else {
        // Fallback for purely vertical motion (Z axis only)
        out_vector3up[0] = 0.0f;
        out_vector3up[1] = 0.0f;
        out_vector3up[2] = 1.0f;
    }

    // 7. Gauss-Legendre Quadrature (5 points) to integrate total surface arc length
    static const double x_nodes[5] = {
        0.046910077030668,
        0.230765344947158,
        0.500000000000000,
        0.769234655052842,
        0.953089922969332
    };
    static const double weights[5] = {
        0.118463442528095,
        0.239314335249683,
        0.284444444444444,
        0.239314335249683,
        0.118463442528095
    };

    double total_length = 0.0;

    for (int i = 0; i < 5; ++i) {
        double t = x_nodes[i];

        double exp_term = exp(-k * (t - 0.5));
        double denom = 1.0 + exp_term;
        double dS_dt = (k * exp_term / (denom * denom)) / range;

        double dz_dt = dz * dS_dt;
        double ds_dt = sqrt(L_xy * L_xy + dz_dt * dz_dt);

        total_length += weights[i] * ds_dt;
    }

    return total_length;
}


// Evaluates point position, curve surface length, local forward vector, and local upward normal vector
double Lerp_Quake3_Logistic_Sigmoid_Return_Sigmoid_Length_V2(
    const vec3_t start,
    const vec3_t end,
    float fraction,
    float sigmoid_k,
    vec3_t out,
    vec3_t out_vector3up,
    vec3_t out_forward
) {
    // 1. Clamp fraction between 0.0 and 1.0
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    double k = (double)sigmoid_k;

    // 2. Normalization factors for Logistic Sigmoid
    double s0 = 1.0 / (1.0 + exp(k * 0.5));
    double s1 = 1.0 / (1.0 + exp(-k * 0.5));
    double range = s1 - s0;

    // 3. Calculate normalized Sigmoid factor at given fraction 't'
    double raw_s = 1.0 / (1.0 + exp(-k * ((double)fraction - 0.5)));
    double smooth_z = (raw_s - s0) / range;

    // 4. Calculate 3D output point
    out[0] = start[0] + (end[0] - start[0]) * fraction;
    out[1] = start[1] + (end[1] - start[1]) * fraction;
    out[2] = (float)((double)start[2] + ((double)end[2] - (double)start[2]) * smooth_z);

    // 5. Delta components
    double dx = (double)end[0] - (double)start[0];
    double dy = (double)end[1] - (double)start[1];
    double dz = (double)end[2] - (double)start[2];
    double L_xy = sqrt(dx * dx + dy * dy);

    // 6. Calculate local tangent (forward vector) & normal (up vector) at 'fraction'
    double exp_term_f = exp(-k * ((double)fraction - 0.5));
    double denom_f = 1.0 + exp_term_f;
    double dS_dt_f = (k * exp_term_f / (denom_f * denom_f)) / range;

    double dz_dt = dz * dS_dt_f;

    // Forward vector (dx, dy, dz_dt) normalized
    double fwd_len = sqrt(dx * dx + dy * dy + dz_dt * dz_dt);
    if (fwd_len > 0.00001) {
        out_forward[0] = (float)(dx / fwd_len);
        out_forward[1] = (float)(dy / fwd_len);
        out_forward[2] = (float)(dz_dt / fwd_len);
    } else {
        out_forward[0] = 1.0f;
        out_forward[1] = 0.0f;
        out_forward[2] = 0.0f;
    }

    // Up vector perpendicular to forward vector
    if (L_xy > 0.00001) {
        double dir_x = dx / L_xy;
        double dir_y = dy / L_xy;

        double up_x = -dz_dt * dir_x;
        double up_y = -dz_dt * dir_y;
        double up_z = L_xy;

        double up_len = sqrt(up_x * up_x + up_y * up_y + up_z * up_z);
        out_vector3up[0] = (float)(up_x / up_len);
        out_vector3up[1] = (float)(up_y / up_len);
        out_vector3up[2] = (float)(up_z / up_len);
    } else {
        out_vector3up[0] = 0.0f;
        out_vector3up[1] = 0.0f;
        out_vector3up[2] = 1.0f;
    }

    // 7. Gauss-Legendre Quadrature (5 points) to integrate total surface arc length
    static const double x_nodes[5] = {
        0.046910077030668, 0.230765344947158, 0.500000000000000, 0.769234655052842, 0.953089922969332
    };
    static const double weights[5] = {
        0.118463442528095, 0.239314335249683, 0.284444444444444, 0.239314335249683, 0.118463442528095
    };

    double total_length = 0.0;

    for (int i = 0; i < 5; ++i) {
        double t = x_nodes[i];

        double exp_term = exp(-k * (t - 0.5));
        double denom = 1.0 + exp_term;
        double dS_dt = (k * exp_term / (denom * denom)) / range;

        double node_dz_dt = dz * dS_dt;
        double ds_dt = sqrt(L_xy * L_xy + node_dz_dt * node_dz_dt);

        total_length += weights[i] * ds_dt;
    }

    return total_length;
}

RELATED_ (SCR_ToolMarker_f)


#undef VectorNormalize
float VectorNormalize(vec3_t v) {
    float length, ilength;

    length = (float)sqrt((double)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));

    if (length > 0.00001f) {
        ilength = 1.0f / length;
        v[0] *= ilength;
        v[1] *= ilength;
        v[2] *= ilength;
    } else {
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 1.0f;
    }

    return length;
}



void QVectorsToAngles( const vec3_t forward, const vec3_t up, vec3_t angles ) {
    vec3_t right;
    float  xy_dist;

    // 1. Right vector = Forward x Up
    CrossProduct( forward, up, right );

    // 2. Yaw (Horizontal heading)
    if ( forward[0] == 0.0f && forward[1] == 0.0f ) {
        angles[1] = 0.0f;
    } else {
        angles[1] = (float)( atan2( forward[1], forward[0] ) * 180.0 / M_PI );
        if ( angles[1] < 0.0f ) {
            angles[1] += 360.0f;
        }
    }

    // 3. Pitch (Nose up / nose down)
    xy_dist = (float)sqrt( forward[0] * forward[0] + forward[1] * forward[1] );
    angles[0] = (float)( atan2( -forward[2], xy_dist ) * 180.0 / M_PI );
    if ( angles[0] < 0.0f ) {
        angles[0] += 360.0f;
    }

    // 4. Roll (Bank angle derived from the Right and Up vectors)
    // Project the Up vector onto the local coordinate frame
    angles[2] = (float)( atan2( right[2], up[2] ) * 180.0 / M_PI );
    if ( angles[2] < 0.0f ) {
        angles[2] += 360.0f;
    }
}


/*
===============================================================================

Evaluate_Quake3_RollerCoaster_Loop

Evaluates position, forward vector, up vector, and total arc length for a
vertical loop in a Quake 3 style coordinate system (+X East, +Y North, +Z Up).

Parameters:
  start, end    - World space start and end coordinates of the loop segment.
  loop_height   - Total height of the vertical loop in world units.
  t             - Parametric progress along the track [0.0 to 1.0].
  out_origin    - Evaluated 3D position at time t.
  out_up        - Surface normal / roof vector of the cart at time t.
  out_forward   - Velocity / travel direction vector of the cart at time t.

Returns:
  Total integrated 3D arc length of the loop track.

===============================================================================
*/
#include <math.h>

typedef float vec3_t[3];

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
===============================================================================

Evaluate_Quake3_RollerCoaster_Loop

Evaluates position, forward vector, up vector, and total arc length for a
vertical loop in a Quake 3 style coordinate system (+X East, +Y North, +Z Up).

Parameters:
  start, end    - World space start and end coordinates of the loop segment.
  loop_height   - Total height of the vertical loop in world units.
  t             - Parametric progress along the track [0.0 to 1.0].
  out_origin    - Evaluated 3D position at time t.
  out_up        - Surface normal / roof vector of the cart at time t.
  out_forward   - Velocity / travel direction vector of the cart at time t.

Returns:
  Total integrated 3D arc length of the loop track.

===============================================================================
*/
double Evaluate_Quake3_RollerCoaster_Loop(
    const vec3_t start,
    const vec3_t end,
    float loop_height,
    float t,
    vec3_t out_origin,
    vec3_t out_up,
    vec3_t out_forward
) {
    float horiz_len_sq;
    vec3_t horiz_fwd;

    // 1. Clamp parametric t to [0, 1]
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    // 2. Calculate overall track deltas
    double dx = (double)end[0] - (double)start[0];
    double dy = (double)end[1] - (double)start[1];
    double dz = (double)end[2] - (double)start[2];

    double R = (double)loop_height * 0.5;
    double theta = (double)t * 2.0 * M_PI;

    // 3. Local Circle Geometry
    double local_x = R * sin(theta);
    double local_z = R * (1.0 - cos(theta));

    // 4. Position Evaluation
    out_origin[0] = (float)((double)start[0] + dx * (double)t + local_x);
    out_origin[1] = (float)((double)start[1] + dy * (double)t);
    out_origin[2] = (float)((double)start[2] + dz * (double)t + local_z);

    // 5. Horizontal Track Direction
    horiz_fwd[0] = (float)dx;
    horiz_fwd[1] = (float)dy;
    horiz_fwd[2] = 0.0f;

    horiz_len_sq = horiz_fwd[0] * horiz_fwd[0] + horiz_fwd[1] * horiz_fwd[1];
    if (horiz_len_sq > 0.000001f) {
        float inv_len = 1.0f / sqrtf(horiz_len_sq);
        horiz_fwd[0] *= inv_len;
        horiz_fwd[1] *= inv_len;
    } else {
        horiz_fwd[0] = 1.0f;
        horiz_fwd[1] = 0.0f;
        horiz_fwd[2] = 0.0f;
    }

    // 6. Tangent (vel) and Normal (norm) Circle Derivatives
    // At t=0: vel_x = 1.0 (forward),  vel_z = 0.0 (no climb)
    // At t=0: norm_x = 0.0 (no tilt),  norm_z = 1.0 (straight up)
    double vel_x = cos(theta);
    double vel_z = sin(theta);

    double norm_x = -sin(theta);
    double norm_z = cos(theta);

    // 7. Forward Vector Construction (Velocity / Direction of Travel)
    out_forward[0] = horiz_fwd[0] * (float)vel_x;
    out_forward[1] = horiz_fwd[1] * (float)vel_x;
    out_forward[2] = (float)vel_z;

    // Normalize forward vector
    float fwd_len_sq = out_forward[0] * out_forward[0] +
                       out_forward[1] * out_forward[1] +
                       out_forward[2] * out_forward[2];
    if (fwd_len_sq > 0.000001f) {
        float inv_len = 1.0f / sqrtf(fwd_len_sq);
        out_forward[0] *= inv_len;
        out_forward[1] *= inv_len;
        out_forward[2] *= inv_len;
    }

    // 8. Up Vector Construction (Surface Normal / Roof Direction)
    out_up[0] = horiz_fwd[0] * (float)norm_x;
    out_up[1] = horiz_fwd[1] * (float)norm_x;
    out_up[2] = (float)norm_z;

    // Normalize up vector
    float up_len_sq = out_up[0] * out_up[0] +
                     out_up[1] * out_up[1] +
                     out_up[2] * out_up[2];
    if (up_len_sq > 0.000001f) {
        float inv_len = 1.0f / sqrtf(up_len_sq);
        out_up[0] *= inv_len;
        out_up[1] *= inv_len;
        out_up[2] *= inv_len;
    }

    // 9. Arc Length Integration (5-point Gauss-Legendre Quadrature)
    static const double x_nodes[5] = { 0.046910077, 0.230765345, 0.500000000, 0.769234655, 0.953089923 };
    static const double weights[5] = { 0.118463443, 0.239314335, 0.284444444, 0.239314335, 0.118463443 };

    double total_length = 0.0;
    int i;
    for (i = 0; i < 5; ++i) {
        double node_t = x_nodes[i];
        double node_theta = node_t * 2.0 * M_PI;

        double node_dx = dx + R * 2.0 * M_PI * cos(node_theta);
        double node_dy = dy;
        double node_dz = dz + R * 2.0 * M_PI * sin(node_theta);

        double ds_dt = sqrt(node_dx * node_dx + node_dy * node_dy + node_dz * node_dz);
        total_length += weights[i] * ds_dt;
    }

    return total_length;
}
RELATED_ (SV_Spawn_Model_At)

void SCR_anglevectors_f (cmd_state_t *cmd)
{
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (3, " <x angle> <y> <z>",
		"Prints v_forward v_right v_up ");

	cmd_argvv_def_(angles, 1);

	printvarv (angles);
	vec3_t forward, right, up;
	AngleVectors (angles, forward, right, up);
	printvarv (forward);
	printvarv (right);
	printvarv (up);
}

void SCR_vectoangles_f (cmd_state_t *cmd)
{
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (3, " <v_forward x> <forward y> <forward z>",
		"Calculates angles from v_forward");

	cmd_argvv_def_(forward, 1);

	vec3_t angles;
	AnglesFromVectors(angles, forward, /*up*/ NULL, q_flip_pitch_true);

	printvarv (forward);
	printvarv (angles);
}

void SCR_vectoangles2_f (cmd_state_t *cmd)
{
	CMD_USAGE_NUM_ARGS_DESCRIBE_ (3, " <forward x> <forward y> <forward z> <up x> <up y> <up z>",
		"Calculates angles from v_forward v_up");

	cmd_argvv_def_(forward, 1);
	cmd_argvv_def_(up, 4);

	vec3_t angles;
	AnglesFromVectors(angles, forward, up, q_flip_pitch_true);

	printvarv (forward);
	printvarv (up);
	printvarv (angles);
}

/*
===============================================================================

Quake3_VectorsToAngles

Converts orthonormal Forward and Up vectors to Quake 3 Euler angles (Pitch, Yaw, Roll).

Parameters:
  forward     - Normalized direction-of-travel unit vector.
  up          - Normalized roof/surface-normal unit vector.
  out_angles  - Output array where angles are stored:
                  out_angles[0] = Pitch (negated for Quake convention)
                  out_angles[1] = Yaw
                  out_angles[2] = Roll

===============================================================================
*/
#include <math.h>

typedef float vec3_t[3];

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
===============================================================================

Quake3_VectorsToAngles

Converts orthonormal Forward and Up vectors to Quake 3 Euler angles (Pitch, Yaw, Roll).
Keeps Roll smooth and continuous through 360-degree vertical loop inversions.

===============================================================================
*/
void Quake3_VectorsToAngles(const vec3_t forward, const vec3_t up, vec3_t out_angles) {
    vec3_t right;
    float horiz_dist;

    // 1. Right vector = Forward x Up
    right[0] = forward[1] * up[2] - forward[2] * up[1];
    right[1] = forward[2] * up[0] - forward[0] * up[2];
    right[2] = forward[0] * up[1] - forward[1] * up[0];

    // 2. Horizontal distance on the X/Y ground plane
    horiz_dist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

    // 3. Singularity Guard at pure vertical apex / entry (horiz_dist near 0)
    if (horiz_dist < 0.001f) {
        // When moving straight vertical, lock Yaw to Up vector direction
        if (up[0] != 0.0f || up[1] != 0.0f) {
            out_angles[1] = atan2f(up[1], up[0]) * 180.0f / (float)M_PI;
        } else {
            out_angles[1] = 0.0f;
        }

        // Pitch is either -90 (climbing straight up) or +90 (falling straight down)
        out_angles[0] = (forward[2] > 0.0f) ? -90.0f : 90.0f;
        out_angles[2] = 0.0f;
    } else {
        // --- STANDARD MATRIX CONVERSION ---

        // Yaw: Horizontal heading angle atan2(Y, X)
        out_angles[1] = atan2f(forward[1], forward[0]) * 180.0f / (float)M_PI;

        // Pitch: Negated for Quake 3 pitch convention (-atan2(Z, horiz_dist))
        out_angles[0] = -atan2f(forward[2], horiz_dist) * 180.0f / (float)M_PI;

        // Roll: Projected against local right vector to eliminate world-Z inversion flip
        // For a pure 2D vertical loop on rails, local roll remains 0.
        out_angles[2] = 0.0f;
    }

    // 4. Normalize all angles into standard [0, 360) range
    for (int i = 0; i < 3; i++) {
        while (out_angles[i] < 0.0f)   out_angles[i] += 360.0f;
        while (out_angles[i] >= 360.0f) out_angles[i] -= 360.0f;
    }
}


floatlist_s sigmoid_float_list; // origin 3, angles 3, forward 3, up 3 -- a 12 pack

RELATED_ (SCR_brushmoverotate_f)
void SCR_sigmoid_f (cmd_state_t *cmd)
{
	qbool Edict_SetNum_String_Print_Errors (prvm_prog_t *prog, int edict_num, ccs *s_fieldname, ccs *valuebuf);
	void SV_Models_Clear (cmd_state_t *cmd);
	int SV_Spawn_Model_Apple_At (ccs *origin, ccs *angles);

	floats_freecontents (&sigmoid_float_list); // Make pos add to list?
	SV_Models_Clear (cmd);

	CMD_USAGE_NUM_ARGS_DESCRIBE_ (8, " <percent01> <sigmoid_k 0 to 20> <p0> <p1> <steps> <maps/model.map>",
		"Calculates origin at percent -- sigmoid_k 0 is flat and beyond 6 starts getting steep ");

//	float percent01 = cmd_argvf_parm1;
//	float sigmoidk = cmd_argvf_parm2;
	vec3_t p0 = { cmd_argvf_parm3, cmd_argvf_parm4, cmd_argvf_parm5 };
	vec3_t p1 = { cmd_argvf_parm6, cmd_argvf_parm7, cmd_argvf_parm8 };
	vec3_t vdelta;
	VectorSubtract (p1, p0, vdelta);
	int num_steps = cmd_argvf_parm9;
//	ccs *model = cmd_argv_parm9;
	vec3_t org = {0};
	vec3_t up = {0};
	vec3_t forward = {0};
//	float y_offset = -vdelta[1];
	float z_height = vdelta[0] * 2;

	if (num_steps <= 0) {
		zpr ("Can't: Num steps is <= 0");
		return;
	}

	baker_string_s *bsa = BakerString_Create_Malloc ("");

	for (int n = 0; n <= num_steps; n ++) {
		float this_percent = n / (float)num_steps;
		this_percent = bound(0, this_percent, 1);
#if 0
		float length = Lerp_Quake3_Logistic_Sigmoid_Return_Sigmoid_Length_V2(
			p0, p1, this_percent, sigmoidk, org, up, forward);
#endif
//		float length = Evaluate_Quake3_RollerCoaster_Loop(
//			p0, p1, z_height, y_offset, this_percent, org, up, forward);

		//float length = Evaluate_Quake3_RollerCoaster_Loop(
		//	p0, p1, z_height, this_percent, org, up, forward);
		float length = Evaluate_Quake3_RollerCoaster_Loop(
			p0, p1, z_height, this_percent, org, up, forward);

		if (n == 0)
			printvarf (length);

		vec3_t forward_quake_angles = {0};
		//vec3_t
		//VectorToQuakeAngles (forward, forward_quake_angles);
		//AnglesFromVectors(angles, forward, /*up*/ NULL, q_flip_pitch_true);
		RELATED_ (SCR_vectoangles_f)
		AnglesFromVectors(forward_quake_angles, forward, up, q_flip_pitch_true);
		//AnglesFromVectors(forward_quake_angles, forward, up, q_flip_pitch_false);
		//QVectorsToAngles (forward, up, forward_quake_angles);
		//Quake3_VectorsToAngles (forward, up, forward_quake_angles);

		va_super (origin, 1024, VECTOR3_G, VECTOR3_SEND(org));
		va_super (angles, 1024, VECTOR3_G, VECTOR3_SEND(forward_quake_angles));

		BakerString_CatLinef (bsa, "{");
		BakerString_CatLinef (bsa, QUOTED_STR("classname") " " QUOTED_STR("path_corner"));
		BakerString_CatLinef (bsa, QUOTED_STR("spawnflags") " " QUOTED_STR("3"));
		BakerString_CatLinef (bsa, QUOTED_STR("origin") " " QUOTED_S, origin);
		BakerString_CatLinef (bsa, QUOTED_STR("angles") " " QUOTED_S, angles);
		BakerString_CatLinef (bsa, QUOTED_STR("forward") " "  "\"" VECTOR3_G "\"", VECTOR3_SEND(forward) );
		BakerString_CatLinef (bsa, QUOTED_STR("up") " "  "\"" VECTOR3_G "\"", VECTOR3_SEND(up) );
		BakerString_CatLinef (bsa, QUOTED_STR("targetname") " "  "\"" "loop%d" "\"", n);
		BakerString_CatLinef (bsa, QUOTED_STR("target") " "  "\"" "loop%d" "\"", n + 1);
		BakerString_CatLinef (bsa, "}");


		floats_add_vector(&sigmoid_float_list, org);
		floats_add_vector(&sigmoid_float_list, forward_quake_angles);
		floats_add_vector(&sigmoid_float_list, forward);
		floats_add_vector(&sigmoid_float_list, up);

		RELATED_ (SV_Ent_Create_Is_Toss)
		int edict_num = SV_Spawn_Model_Apple_At (origin, angles);
		if (edict_num != not_found_neg1)
		Edict_SetNum_String_Print_Errors (SVVM_prog, edict_num, "colormod", "2 2 2");
	} // n

	Clipboard_Set_Text (bsa->string);
	BakerString_Destroy_And_Null_It (&bsa);
	zpr ("Copied to clipboard");
}




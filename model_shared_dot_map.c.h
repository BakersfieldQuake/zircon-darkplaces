// model_shared_dot_map.c.h

#define LogLinef Con_Printf

RELATED_ (Mod_Decompile_OBJ)

#if 0 // Baker: This sucks
static void Mod_Decompile_MAP (model_t *model, const char *rawname, const char *filename,
                               int submodelnum, ccs *printf_pattern)
{
    size_t outbufferpos = 0;
    size_t outbuffermax = 0x100000; // 1 MB starting buffer
    char *outbuffer = (char *) Z_Malloc(outbuffermax), *oldbuffer;
    
    int brush_counter = 0;
    size_t jj;

    // Header for Quake map format
    jj = dpsnprintf (outbuffer + outbufferpos, outbuffermax - outbufferpos,
        "// Decompiled map by darkplaces engine\n{\n\"classname\" \"worldspawn\"\n");
    if (jj > 0) outbufferpos += jj;

    int start_index  = submodelnum ? submodelnum + 0 : 0;
    int beyond_index = submodelnum ? submodelnum + 1 : max(1, model->brush.numsubmodels);

    for (int submodelindex = start_index; submodelindex < beyond_index; submodelindex++) {
        model_t *submodel = model->brush.numsubmodels ? model->brush.submodels[submodelindex] : model;
        
        for (int surfaceindex = submodel->submodelsurfaces_start; surfaceindex < submodel->submodelsurfaces_end; surfaceindex++) {
            const msurface_t *surface = model->data_surfaces + surfaceindex;
            if (!surface || surface->num_triangles <= 0) continue;

            const char *texname = (surface->texture && surface->texture->name[0]) ? surface->texture->name : COMMON_CAULK;

            const int *e = model->surfmesh.data_element3i + surface->num_firsttriangle * 3;
            
            for (int triangleindex = 0; triangleindex < surface->num_triangles; triangleindex++, e += 3) {
                int i1 = e[0];
                int i2 = e[1];
                int i3 = e[2];

                float *v1 = model->surfmesh.data_vertex3f + i1 * 3;
                float *v2 = model->surfmesh.data_vertex3f + i2 * 3;
                float *v3 = model->surfmesh.data_vertex3f + i3 * 3;

                // 1. Calculate normal using cross product
                float edge1[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
                float edge2[3] = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2] };
                
                float normal[3];
                normal[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
                normal[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
                normal[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];

                float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
                if (len < 0.0001f) continue;
                normal[0] /= len; normal[1] /= len; normal[2] /= len;

                // 2. Extrude slightly backward to create a 3D slab (4 units deep)
                float thickness = 4.0f;
                float ev1[3] = { v1[0] - normal[0] * thickness, v1[1] - normal[1] * thickness, v1[2] - normal[2] * thickness };
                float ev2[3] = { v2[0] - normal[0] * thickness, v2[1] - normal[1] * thickness, v2[2] - normal[2] * thickness };
                float ev3[3] = { v3[0] - normal[0] * thickness, v3[1] - normal[1] * thickness, v3[2] - normal[2] * thickness };

                // Ensure buffer safety check
                if (outbufferpos >= outbuffermax >> 1) {
                    outbuffermax *= 2;
                    oldbuffer = outbuffer;
                    outbuffer = (char *) Z_Malloc(outbuffermax);
                    memcpy(outbuffer, oldbuffer, outbufferpos);
                    Z_Free(oldbuffer);
                }

                // 3. Print brush block structure
                jj = dpsnprintf(outbuffer + outbufferpos, outbuffermax - outbufferpos,
                    "// brush %d\n"
                    "{\n"
                    " ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) %s 0 0 0 1 1\n"
                    " ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) %s 0 0 0 1 1\n"
                    " ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) %s 0 0 0 1 1\n"
                    " ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) %s 0 0 0 1 1\n"
                    " ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) ( %.4f %.4f %.4f ) %s 0 0 0 1 1\n"
                    "}\n",
                    ++brush_counter,
                    v1[0], v1[1], v1[2],  v2[0], v2[1], v2[2],  v3[0], v3[1], v3[2],  texname,
                    ev3[0], ev3[1], ev3[2],  ev2[0], ev2[1], ev2[2],  ev1[0], ev1[1], ev1[2],  texname,
                    v1[0], v1[1], v1[2],  ev1[0], ev1[1], ev1[2],  ev2[0], ev2[1], ev2[2],  COMMON_CAULK,
                    v2[0], v2[1], v2[2],  ev2[0], ev2[1], ev2[2],  v3[0], v3[1], v3[2],  COMMON_CAULK,
                    v3[0], v3[1], v3[2],  ev3[0], ev3[1], ev3[2],  v1[0], v1[1], v1[2],  COMMON_CAULK
                );

                if (jj > 0) outbufferpos += jj;
            }
        }
    }

    // Close worldspawn entity block
    jj = dpsnprintf (outbuffer + outbufferpos, outbuffermax - outbufferpos, "}\n");
    if (jj > 0) outbufferpos += jj;

    // Write file and cleanup
    FS_WriteFile(filename, outbuffer, outbufferpos);
    Z_Free(outbuffer);

    Con_PrintLinef ("Wrote %s (%d bytes, %d brushes generated)", filename, (int)outbufferpos, brush_counter);
}

#endif

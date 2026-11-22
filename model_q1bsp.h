#ifndef MODEL_Q1BSP_H
#define MODEL_Q1BSP_H

#include "qtypes.h"
#include "model_brush.h"

struct mod_pvscache_s;

typedef struct model_brushq1_s
{
	mmodel_t		*submodels;

	int				numvertexes;
	mvertex_t		*vertexes;

	int				numedges;
	medge_t			*edges;

	int				numtexinfo;
	mtexinfo_t		*texinfo;

	int				numsurfedges;
	int				*surfedges;

	int				numclipnodes;
	mclipnode_t		*clipnodes;

	hull_t			hulls[MAX_MAP_HULLS];

	int				num_compressedpvs;
	unsigned char			*data_compressedpvs;
	int				*data_cluster_visofs;
	struct mod_pvscache_s	*pvscache;

	int				num_lightdata;
	unsigned char			*lightdata;
	unsigned char			*nmaplightdata; // deluxemap file

	// lightmap update chains for light styles
	int				num_lightstyles;
	model_brush_lightstyleinfo_t *data_lightstyleinfo;

	// this contains bytes that are 1 if a surface needs its lightmap rebuilt
	unsigned char *lightmapupdateflags;
	qbool firstrender; // causes all surface lightmaps to be loaded in first frame
	qbool render_bih_tried; // avoids repeated lazy BIH attempts on 0-triangle submodels
}
model_brushq1_t;

#endif // !MODEL_Q1BSP_H

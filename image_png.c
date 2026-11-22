/*
	Copyright (C) 2006  Serge "(515)" Ziryukin, Ashley Rose Hale (LadyHavoc)

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/

//LadyHavoc: rewrote most of this.

#if 0 && defined(_MSC_VER) && _MSC_VER < 1900 // Baker: cutoff?

	#include "quakedef.h"

	unsigned char *PNG_LoadImage_BGRA (const unsigned char *pbytes, int data_size, int *miplevel_unused)
	{
		return NULL;
	}

#else
	#include "darkplaces.h"
	#include "image.h"
	# undef sprintf
	# undef snprintf
	# undef vsnprintf
	#include "image_png.h"




	/*
	=================================================================

	  DLL load & unload

	=================================================================
	*/

	#undef strcat
	#undef strncat
	#undef strcpy
	#undef strncpy


	#include "lodepng.h"

	unsigned char *PNG_LoadImage_BGRA (const unsigned char *pbytes, int data_size, int *miplevel)
	{
		byte *lodepng_data = NULL;
		unsigned uwidth, uheight, lodepng_error, numpels;

		lodepng_error = lodepng_decode32 (&lodepng_data, &uwidth, &uheight, pbytes, (size_t) data_size);

		if (lodepng_error) {
			if (lodepng_data)
				free (lodepng_data);
			Con_PrintLinef (CON_WARN "PNG_LoadImage_BGRA: decode failed (error %u: %s)", lodepng_error, lodepng_error_text(lodepng_error));
			return NULL;
		}

		if (uwidth == 0 || uheight == 0 || uwidth > 32768 || uheight > 32768) {
			free (lodepng_data);
			Con_PrintLinef (CON_WARN "PNG_LoadImage_BGRA: invalid image dimensions %ux%u", uwidth, uheight);
			return NULL;
		}

		// Baker: we have some globals here
		image_width = (int)uwidth;
		image_height = (int)uheight;

		// Early in-place picmip downscale before allocating tempmempool buffer (matches JPEG_LoadImage_BGRA)
		// Cuts peak RAM on 2048x2048 PNGs from 16+16 MB down to 16+4 MB (picmip 1) or 16+1 MB (picmip 2)
		if (miplevel && *miplevel > 0) {
			int depth = 1;
			while (*miplevel > 0 && (image_width > 1 || image_height > 1)
				&& ((image_width & 1) == 0 || image_width == 1)
				&& ((image_height & 1) == 0 || image_height == 1))
			{
				int destw = image_width > 1 ? (image_width >> 1) : 1;
				int desth = image_height > 1 ? (image_height >> 1) : 1;
				Image_MipReduce32(lodepng_data, lodepng_data, &image_width, &image_height, &depth, destw, desth, 1);
				(*miplevel)--;
			}
		}

		numpels = (unsigned)(image_width * image_height);

		unsigned char *image_data = (unsigned char *)Mem_Alloc(tempmempool, (size_t)numpels * 4);

		if (!image_data) {
			free (lodepng_data);
			Con_PrintLinef ("PNG_LoadImage : not enough memory");
			return NULL;
		}

		// Copy and swizzle RGBA -> BGRA in one pass, then release lodepng's C-heap buffer immediately
		{
			size_t y, nbytes = (size_t)numpels * 4;
			for (y = 0; y < nbytes; y += 4) {
				image_data[y+0] = lodepng_data[y+2];
				image_data[y+1] = lodepng_data[y+1];
				image_data[y+2] = lodepng_data[y+0];
				image_data[y+3] = lodepng_data[y+3];
			}
		}

		free (lodepng_data);

		return image_data;
	}

	void *Image_Save_PNG_Memory_Alloc (const unsigned *rgba, int width, int height, /*required*/ size_t *png_length, const char *description)
	{		
		byte *png_out = NULL;

		unsigned error = lodepng_encode32 (&png_out, png_length, (byte *)rgba, width, height);

		if (error) {
			Con_PrintLinef ("Image_Save_PNG_Memory_Alloc: Error saving image to memory '%s'", description);
			return NULL;
		}

		return png_out;
	}

#endif


/*
====================
PNG_SaveImage_preflipped

Save a preflipped PNG image to a file
====================
*/
//void *Image_Save_PNG_Memory_Alloc (const unsigned *rgba, int width, int height, reply size_t *png_length, const char *description)
//{
//	const char *fn_name = "Image_Save_PNG_Memory_Alloc";
//	byte *png_out = NULL;
//	size_t png_length_tmp = 0;
//	unsigned error = lodepng_encode32 (&png_out, &png_length_tmp, (byte *)rgba, width, height);
//
//	if (error)
//		return log_debug ("%s: Error saving image to memory '%s'", fn_name, description), NULL; // Tricksie null return
//
//	NOT_MISSING_ASSIGN (png_length, png_length_tmp);
//	return png_out;
//}

RELATED_ (Image_WriteTGABGRA)
qbool PNG_SaveImage_preflipped (const char *filename, int width, int height, 
								qbool has_alpha, byte *data_rgba)
{

	byte *png_data_a = NULL;
	size_t png_length = 0;
	unsigned error = lodepng_encode32 (&png_data_a, &png_length, (byte *)data_rgba, width, height);

	if (error)
		return false;

	qbool ret = FS_WriteFile (filename, png_data_a, /*length*/ png_length);

	freenull_ (png_data_a); // Discard png data.

	return ret;
}


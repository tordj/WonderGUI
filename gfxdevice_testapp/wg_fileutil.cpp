/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifdef WIN32
#    include <SDL.h>
#    include <SDL_image.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#    include <SDL2_image/SDL_image.h>
#else
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_image.h>
#endif

#include "wg_fileutil.h"

namespace wg
{
	//____ convertSDLFormat() ______________________________________________________

	void convertSDLFormat(PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat)
	{
		pWGFormat->format = PixelFormat::Custom;
		pWGFormat->bits = pSDLFormat->BitsPerPixel;
		pWGFormat->bIndexed = (pSDLFormat->palette != nullptr);

		pWGFormat->R_mask = pSDLFormat->Rmask;
		pWGFormat->G_mask = pSDLFormat->Gmask;
		pWGFormat->B_mask = pSDLFormat->Bmask;
		pWGFormat->A_mask = pSDLFormat->Amask;

		pWGFormat->R_shift = pSDLFormat->Rshift;
		pWGFormat->G_shift = pSDLFormat->Gshift;
		pWGFormat->B_shift = pSDLFormat->Bshift;
		pWGFormat->A_shift = pSDLFormat->Ashift;

		pWGFormat->R_loss = pSDLFormat->Rloss;
		pWGFormat->G_loss = pSDLFormat->Gloss;
		pWGFormat->B_loss = pSDLFormat->Bloss;
		pWGFormat->A_loss = pSDLFormat->Aloss;

		pWGFormat->R_bits = 8 - pSDLFormat->Rloss;
		pWGFormat->G_bits = 8 - pSDLFormat->Gloss;
		pWGFormat->B_bits = 8 - pSDLFormat->Bloss;
		pWGFormat->A_bits = 8 - pSDLFormat->Aloss;

	}

	//____ loadBlob() _________________________________________________________

	Blob_p		FileUtil::loadBlob(const char * pPath)
	{
		FILE * fp;

//		errno_t err = fopen_s(&fp, pPath, "rb");
        fp = fopen(pPath, "rb");
		if (!fp)
			return 0;

		fseek(fp, 0, SEEK_END);
		int size = (int) ftell(fp);
		fseek(fp, 0, SEEK_SET);

		Blob_p pBlob = Blob::create(size);

		int nRead = (int) fread(pBlob->data(), 1, size, fp);
		fclose(fp);

		if (nRead < size)
			return 0;

		return pBlob;
	}

	//____ loadSurface() ______________________________________________________

	Surface_p	FileUtil::loadSurface(const char * pPath, SurfaceFactory * pFactory, int flags )
	{
		PixelDescription format;

		auto pSDLSurf = IMG_Load(pPath);
		convertSDLFormat(&format, pSDLSurf->format);

		PixelFormat px;
		Color * pClut = nullptr;

		Color clut[256];



		if (format.bIndexed)
		{
			px = PixelFormat::I8;

			for (int i = 0; i < 256; i++)
			{
				clut[i].r = pSDLSurf->format->palette->colors[i].r;
				clut[i].g = pSDLSurf->format->palette->colors[i].g;
				clut[i].b = pSDLSurf->format->palette->colors[i].b;
				clut[i].a = pSDLSurf->format->palette->colors[i].a;
			}
			pClut = clut;
		}


		else if (format.A_bits > 0)
			px = PixelFormat::BGRA_8;
		else
			px = PixelFormat::BGR_8;


		auto pSurface = pFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), px, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &format, flags, pClut);
		SDL_FreeSurface(pSDLSurf);
		return pSurface;
	}



}


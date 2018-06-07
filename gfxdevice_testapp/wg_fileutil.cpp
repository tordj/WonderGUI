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

#include <wg_fileutil.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>
#endif



namespace wg
{
	//____ convertSDLFormat() ______________________________________________________

	void convertSDLFormat(PixelFormat * pWGFormat, const SDL_PixelFormat * pSDLFormat)
	{
		pWGFormat->type = PixelType::Custom;
		pWGFormat->bits = pSDLFormat->BitsPerPixel;

		pWGFormat->R_mask = pSDLFormat->Rmask;
		pWGFormat->G_mask = pSDLFormat->Gmask;
		pWGFormat->B_mask = pSDLFormat->Bmask;
		pWGFormat->A_mask = pSDLFormat->Amask;

		pWGFormat->R_shift = pSDLFormat->Rshift;
		pWGFormat->G_shift = pSDLFormat->Gshift;
		pWGFormat->B_shift = pSDLFormat->Bshift;
		pWGFormat->A_shift = pSDLFormat->Ashift;

		pWGFormat->R_bits = 8 - pSDLFormat->Rloss;
		pWGFormat->G_bits = 8 - pSDLFormat->Gloss;
		pWGFormat->B_bits = 8 - pSDLFormat->Bloss;
		pWGFormat->A_bits = 8 - pSDLFormat->Aloss;

	}

	//____ loadBlob() _________________________________________________________

	Blob_p		FileUtil::loadBlob(const char * pPath)
	{
		FILE * fp;

		errno_t err = fopen_s(&fp, pPath, "rb");
		if (!fp)
			return 0;

		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		Blob_p pBlob = Blob::create(size);

		int nRead = fread(pBlob->data(), 1, size, fp);
		fclose(fp);

		if (nRead < size)
			return 0;

		return pBlob;
	}

	//____ loadSurface() ______________________________________________________

	Surface_p	FileUtil::loadSurface(const char * pPath, SurfaceFactory * pFactory )
	{
		PixelFormat format;

		auto pSDLSurf = IMG_Load(pPath);
		convertSDLFormat(&format, pSDLSurf->format);

		PixelType px = format.A_bits > 0 ? PixelType::BGRA_8 : PixelType::BGR_8;

		auto pSurface = pFactory->createSurface(Size(pSDLSurf->w, pSDLSurf->h), px, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &format);
		SDL_FreeSurface(pSDLSurf);
		return pSurface;
	}



}


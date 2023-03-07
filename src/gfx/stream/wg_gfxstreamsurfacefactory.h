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

#ifndef	WG_STREAMSURFACEFACTORY_DOT_H
#define	WG_STREAMSURFACEFACTORY_DOT_H
#pragma once

#include <wg_surfacefactory.h>
#include <wg_gfxstreamencoder.h>

#include <vector>

namespace wg
{

	class GfxStreamSurfaceFactory;
	typedef	StrongPtr<GfxStreamSurfaceFactory>		GfxStreamSurfaceFactory_p;
	typedef	WeakPtr<GfxStreamSurfaceFactory>	GfxStreamSurfaceFactory_wp;

	//____ GfxStreamSurfaceFactory _____________________________________________________

	class GfxStreamSurfaceFactory : public SurfaceFactory
	{
	public:
		//.____ Creation __________________________________________

		static GfxStreamSurfaceFactory_p	create( GfxStreamEncoder * pEncoder ) { return GfxStreamSurfaceFactory_p(new GfxStreamSurfaceFactory(pEncoder)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		SizeI		maxSize() const override;

		//.____ Misc _______________________________________________________

		Surface_p	createSurface(const Surface::Blueprint& blueprint) override;
		Surface_p	createSurface(const Surface::Blueprint& blueprint, Blob* pBlob, int pitch) override;
		Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels,
								  const PixelDescription& pixelDescription, int pitch = 0, const Color8 * pPalette = nullptr) override;
		Surface_p	createSurface(const Surface::Blueprint& blueprint, const uint8_t* pPixels, PixelFormat format = PixelFormat::Undefined, int pitch = 0,
										  const Color8 * pPalette = nullptr) override;

		void		setRememberSurfacesCreated(bool bRemember);

		std::vector<Surface_p> surfacesCreated();

	protected:
		GfxStreamSurfaceFactory( GfxStreamEncoder * pEncoder);
		~GfxStreamSurfaceFactory(){}

		void	_addReference(Surface* pSurface);

		bool					m_bKeepReferences = false;
		std::vector<Surface_wp>	m_surfaceReferences;

		GfxStreamEncoder_p	m_pEncoder;
	};
}


#endif //WG_STREAMSURFACEFACTORY_DOT_H


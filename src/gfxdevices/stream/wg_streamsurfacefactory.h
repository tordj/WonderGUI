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

	class StreamSurfaceFactory;
	typedef	StrongPtr<StreamSurfaceFactory>		StreamSurfaceFactory_p;
	typedef	WeakPtr<StreamSurfaceFactory>	StreamSurfaceFactory_wp;

	//____ StreamSurfaceFactory _____________________________________________________

	class StreamSurfaceFactory : public SurfaceFactory
	{
	public:
		//.____ Creation __________________________________________

		static StreamSurfaceFactory_p	create( GfxStreamEncoder * pEncoder ) { return StreamSurfaceFactory_p(new StreamSurfaceFactory(pEncoder)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		SizeI		maxSize() const override;

		//.____ Misc _______________________________________________________

		Surface_p	createSurface( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr ) override;
		Surface_p	createSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr ) override;
		Surface_p	createSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = SurfaceFlag::Static, const Color8 * pClut = nullptr ) override;
		Surface_p	createSurface( Surface * pOther, int flags = SurfaceFlag::Static ) override;

		void		setRememberSurfacesCreated(bool bRemember);

		inline std::vector<Surface_p> surfacesCreated();

	protected:
		StreamSurfaceFactory( GfxStreamEncoder * pEncoder);

		void	_addReference(Surface* pSurface);

		bool					m_bKeepReferences = false;
		std::vector<Surface_wp>	m_surfaceReferences;

		GfxStreamEncoder_p	m_pEncoder;
	};
}


#endif //WG_STREAMSURFACEFACTORY_DOT_H


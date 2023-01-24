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

#include <memory.h>

#include <wg_gfxstreamsurface.h>
#include <wg_gfxstreamsurfacefactory.h>
#include <wg_gfxutil.h>



namespace wg
{

	const TypeInfo GfxStreamSurfaceFactory::TYPEINFO = { "GfxStreamSurfaceFactory", &SurfaceFactory::TYPEINFO };

	//____ constructor ________________________________________________________

	GfxStreamSurfaceFactory::GfxStreamSurfaceFactory(GfxStreamEncoder * pEncoder) : m_pEncoder(pEncoder)
	{

	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamSurfaceFactory::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ maxSize() ______________________________________________________________

	SizeI GfxStreamSurfaceFactory::maxSize() const
	{
		return GfxStreamSurface::maxSize();
	}

	//____ createSurface() ________________________________________________________

	Surface_p GfxStreamSurfaceFactory::createSurface(const Surface::Blueprint& blueprint)
	{
		auto p = GfxStreamSurface::create(m_pEncoder,blueprint);
		_addReference( p );
		return p;
	}

	Surface_p GfxStreamSurfaceFactory::createSurface(const Surface::Blueprint& blueprint, Blob * pBlob, int pitch )
	{
		auto p = GfxStreamSurface::create(m_pEncoder, blueprint, pBlob ,pitch );
		_addReference( p );
		return p;
	}

	Surface_p GfxStreamSurfaceFactory::createSurface( const Surface::Blueprint& blueprint, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription )
	{
		auto p = GfxStreamSurface::create(m_pEncoder, blueprint, pPixels, pitch, pPixelDescription);
		_addReference(p);
		return p;
	}

	Surface_p GfxStreamSurfaceFactory::createSurface( const Surface::Blueprint& blueprint, Surface* pOther )
	{
		auto p = GfxStreamSurface::create(m_pEncoder, blueprint,pOther );
		_addReference(p);
		return p;
	}

	//____ setRememberSurfacesCreated() _________________________________________

	void GfxStreamSurfaceFactory::setRememberSurfacesCreated(bool bKeep )
	{
		m_bKeepReferences = bKeep;
		if (!m_bKeepReferences)
			m_surfaceReferences.clear();
	}

	//____ surfacesCreated() __________________________________________________

	std::vector<Surface_p> GfxStreamSurfaceFactory::surfacesCreated()
	{
		std::vector<Surface_p> vec;
		vec.reserve(m_surfaceReferences.size());

		for (auto& wp : m_surfaceReferences)
		{
			if (wp)
				vec.push_back(wp.rawPtr());
		}
		return vec;
	}

	//____ _addReference() ____________________________________________________

	void GfxStreamSurfaceFactory::_addReference(Surface* pSurface)
	{
		if (m_bKeepReferences)
		{
			for (auto& p : m_surfaceReferences)
			{
				if (!p)
				{
					p = pSurface;
					return;
				}
			}

			m_surfaceReferences.push_back(pSurface);
		}
	}


} // namespace wg

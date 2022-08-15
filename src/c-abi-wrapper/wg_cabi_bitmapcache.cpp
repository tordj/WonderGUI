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

#include <wg_base.h>

#include <wg_cabi_bitmapcache.h>
#include <wg_cabi.h>

#include <cassert>

namespace wg
{

	const TypeInfo	CABIBitmapCache::TYPEINFO = { "CABIBitmapCache", &Object::TYPEINFO };


	//____ constructor ____________________________________________________________

	CABIBitmapCache::CABIBitmapCache( wg_obj bitmapCache )
	{
		m_cCache = bitmapCache;
		CABI::object->retain(bitmapCache);

		// Retrieve and wrap existing cache surfaces

		int nbSurfaces = CABI::bitmapCache->getNbCacheSurfaces(bitmapCache);

		if (nbSurfaces > 0)
		{
			// Reserve buffer, get surface objects

			int mem = nbSurfaces * sizeof(wg_obj);

			wg_obj * pSurfaces = (wg_obj*) Base::memStackAlloc(mem);
			CABI::bitmapCache->getCacheSurfaces(bitmapCache, nbSurfaces, pSurfaces);


			int highestNumber = 0;

			if (CABI::surface->getSurfaceIdentity(pSurfaces[0]) == 0)
			{
				// We are first CABIBitmapCache, we start numbering existing ones

				for (int i = 0; i < nbSurfaces; i++)
					CABI::surface->setSurfaceIdentity(pSurfaces[i], i + 1);

				highestNumber = nbSurfaces;
			}
			else
			{
				// Already numbered, we need to find highest number for stack size.

				for (int i = 0; i < nbSurfaces; i++)
					highestNumber = std::max( CABI::surface->getSurfaceIdentity(pSurfaces[i]), highestNumber);
			}

			// Resize vector, create and insert wrappers

			m_surfaces.resize(highestNumber);

			for (int i = 0; i < nbSurfaces; i++)
			{
				wg_obj obj = pSurfaces[i];

				int ofs = CABI::surface->getSurfaceIdentity(obj) - 1;
				m_surfaces[ofs] = CABISurface::create(obj);
			}

			// Release buffer

			Base::memStackRelease(mem);
		}

		// Set up listener.

		wg_cacheListenerBP	bp;


		bp.addFunc = [](wg_obj surface, void* pData, int data) { ((CABIBitmapCache*)pData)->_surfaceAdded(surface); };
		bp.addPtr = this;

		bp.removeFunc = [](int nSurfaces, wg_obj* pSurfaces, void* pData, int data) { ((CABIBitmapCache*)pData)->_surfacesRemoved(nSurfaces, pSurfaces); };
		bp.removePtr = this;

		bp.clearFunc = [](void* pData, int data) { ((CABIBitmapCache*)pData)->_cleared(); };
		bp.clearPtr = this;

		m_listenerId = CABI::bitmapCache->addCacheListener(bitmapCache, bp);
	}

	//____ Destructor _____________________________________________________________

	CABIBitmapCache::~CABIBitmapCache()
	{
		CABI::bitmapCache->removeCacheListener(m_cCache,m_listenerId);

		CABI::object->release(m_cCache);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABIBitmapCache::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setLimit() ____________________________________________________________

	void CABIBitmapCache::setLimit(int maxBytes)
	{
		CABI::bitmapCache->setCacheLimit(m_cCache, maxBytes);
	}

	//____ limit() ____________________________________________________________

	int CABIBitmapCache::limit() const
	{
		return CABI::bitmapCache->cacheLimit(m_cCache);
	}

	//____ size() _____________________________________________________________

	int CABIBitmapCache::size() const
	{
		return CABI::bitmapCache->cacheSize(m_cCache);
	}

	//____ clear() _______________________________________________________________

	void CABIBitmapCache::clear()
	{
		CABI::bitmapCache->clearCache(m_cCache);
	}

	//____ addListener() ______________________________________________________

	void CABIBitmapCache::addListener(BitmapCacheListener* pListener)
	{
		m_listeners.push_back(pListener);
	}

	//____ removeListener() ___________________________________________________

	void CABIBitmapCache::removeListener(BitmapCacheListener* pListener)
	{
		if (pListener)
		{
			m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), pListener));
			if (m_listeners.empty())
				clear();
		}
	}

	//____ getCacheSlot() _____________________________________________________

	std::tuple<Surface_p, CoordI> CABIBitmapCache::getCacheSlot(SizeI size)
	{
		auto slot = CABI::bitmapCache->getCacheSlot(m_cCache, { size.w,size.h });

		int ofs = CABI::surface->getSurfaceIdentity(slot.surface) - 1;

		return std::make_tuple(m_surfaces[ofs], CoordI( slot.coord.x, slot.coord.y ) );
	}

	//____ getSurfaces() ______________________________________________________

	std::vector<Surface_p> CABIBitmapCache::getSurfaces() const
	{
		std::vector<Surface_p> vec;

		for ( auto& surf : m_surfaces )
		{
			if (surf)
				vec.push_back(surf);
		}

		return vec;
	}

	//____ wrapperSurface() ___________________________________________________

	CABISurface* CABIBitmapCache::wrapperSurface(wg_obj surfaceObject)
	{
		int offset = CABI::surface->getSurfaceIdentity(surfaceObject) -1;

		assert(offset > 0 && offset < m_surfaces.size() && m_surfaces[offset] != nullptr );

		return m_surfaces[offset];
	}

	//____ _surfaceAdded() ____________________________________________________

	void CABIBitmapCache::_surfaceAdded(wg_obj surface)
	{
		int ofs = CABI::surface->getSurfaceIdentity(surface) - 1;
		if (ofs < 0)
		{
			// We are the first CABIBitmapCache receiving the callback. Let's decide and offset.

			int ofs = 0;
			for ( auto& surf : m_surfaces )
			{
				if (!surf)
					break;
				ofs++;
			}

			CABI::surface->setSurfaceIdentity(surface, ofs + 1);
		}

		if (ofs >= m_surfaces.size())
			m_surfaces.resize(ofs + 1);

		m_surfaces[ofs] = CABISurface::create(surface);
	}

	//____ _surfacesRemoved() _________________________________________________

	void CABIBitmapCache::_surfacesRemoved(int nSurfaces, wg_obj* pSurfaces)
	{
		for (int i = 0; i < nSurfaces; i++)
		{
			int ofs = CABI::surface->getSurfaceIdentity(pSurfaces[i]) - 1;
			m_surfaces[ofs] = nullptr;
		}
	}

	//_____ _cleared() ________________________________________________________

	void CABIBitmapCache::_cleared()
	{
		m_surfaces.clear();
	}




} // namespace wg

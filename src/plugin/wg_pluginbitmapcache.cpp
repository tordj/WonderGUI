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

#include <wg_pluginbitmapcache.h>
#include <wg_pluginbase.h>

#include <cassert>
#include <algorithm>


namespace wg
{

	const TypeInfo	PluginBitmapCache::TYPEINFO = { "PluginBitmapCache", &Object::TYPEINFO };


	//____ constructor ____________________________________________________________

	PluginBitmapCache::PluginBitmapCache( wg_obj bitmapCache )
	{
		m_cCache = bitmapCache;
		PluginBase::object->retain(bitmapCache);

		// Retrieve and wrap existing cache surfaces

		int nbSurfaces = PluginBase::bitmapCache->getNbCacheSurfaces(bitmapCache);

		if (nbSurfaces > 0)
		{
			// Reserve buffer, get surface objects

			int mem = nbSurfaces * sizeof(wg_obj);

			wg_obj * pSurfaces = (wg_obj*) Base::memStackAlloc(mem);
			PluginBase::bitmapCache->getCacheSurfaces(bitmapCache, nbSurfaces, pSurfaces);


			int highestNumber = 0;

			if (PluginBase::surface->getSurfaceIdentity(pSurfaces[0]) == 0)
			{
				// We are first PluginBitmapCache, we start numbering existing ones

				for (int i = 0; i < nbSurfaces; i++)
					PluginBase::surface->setSurfaceIdentity(pSurfaces[i], i + 1);

				highestNumber = nbSurfaces;
			}
			else
			{
				// Already numbered, we need to find highest number for stack size.

				for (int i = 0; i < nbSurfaces; i++)
					highestNumber = std::max( PluginBase::surface->getSurfaceIdentity(pSurfaces[i]), highestNumber);
			}

			// Resize vector, create and insert wrappers

			m_surfaces.resize(highestNumber);

			for (int i = 0; i < nbSurfaces; i++)
			{
				wg_obj obj = pSurfaces[i];

				int ofs = PluginBase::surface->getSurfaceIdentity(obj) - 1;
				m_surfaces[ofs] = PluginSurface::create(obj);
			}

			// Release buffer

			Base::memStackRelease(mem);
		}

		// Set up listener.

		wg_cacheListenerBP	bp;


		bp.addFunc = [](wg_obj surface, void* pData, int data) { ((PluginBitmapCache*)pData)->_surfaceAdded(surface); };
		bp.addPtr = this;

		bp.removeFunc = [](int nSurfaces, wg_obj* pSurfaces, void* pData, int data) { ((PluginBitmapCache*)pData)->_surfacesRemoved(nSurfaces, pSurfaces); };
		bp.removePtr = this;

		bp.clearFunc = [](void* pData, int data) { ((PluginBitmapCache*)pData)->_cleared(); };
		bp.clearPtr = this;

		m_listenerId = PluginBase::bitmapCache->addCacheListener(bitmapCache, bp);
	}

	//____ Destructor _____________________________________________________________

	PluginBitmapCache::~PluginBitmapCache()
	{
		PluginBase::bitmapCache->removeCacheListener(m_cCache,m_listenerId);

		PluginBase::object->release(m_cCache);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& PluginBitmapCache::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setLimit() ____________________________________________________________

	void PluginBitmapCache::setLimit(int maxBytes)
	{
		PluginBase::bitmapCache->setCacheLimit(m_cCache, maxBytes);
	}

	//____ limit() ____________________________________________________________

	int PluginBitmapCache::limit() const
	{
		return PluginBase::bitmapCache->cacheLimit(m_cCache);
	}

	//____ size() _____________________________________________________________

	int PluginBitmapCache::size() const
	{
		return PluginBase::bitmapCache->cacheSize(m_cCache);
	}

	//____ clear() _______________________________________________________________

	void PluginBitmapCache::clear()
	{
		PluginBase::bitmapCache->clearCache(m_cCache);
	}

	//____ addListener() ______________________________________________________

	void PluginBitmapCache::addListener(BitmapCacheListener* pListener)
	{
		m_listeners.push_back(pListener);
	}

	//____ removeListener() ___________________________________________________

	void PluginBitmapCache::removeListener(BitmapCacheListener* pListener)
	{
		if (pListener)
		{
			m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), pListener));
			if (m_listeners.empty())
				clear();
		}
	}

	//____ getCacheSlot() _____________________________________________________

	std::tuple<Surface_p, CoordI> PluginBitmapCache::getCacheSlot(SizeI size)
	{
		auto slot = PluginBase::bitmapCache->getCacheSlot(m_cCache, { size.w,size.h });

		int ofs = PluginBase::surface->getSurfaceIdentity(slot.surface) - 1;

		return std::make_tuple(m_surfaces[ofs], CoordI( slot.coord.x, slot.coord.y ) );
	}

	//____ getSurfaces() ______________________________________________________

	std::vector<Surface_p> PluginBitmapCache::getSurfaces() const
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

	PluginSurface* PluginBitmapCache::wrapperSurface(wg_obj surfaceObject)
	{
		int offset = PluginBase::surface->getSurfaceIdentity(surfaceObject) -1;

		assert(offset > 0 && offset < m_surfaces.size() && m_surfaces[offset] != nullptr );

		return m_surfaces[offset];
	}

	//____ _surfaceAdded() ____________________________________________________

	void PluginBitmapCache::_surfaceAdded(wg_obj surface)
	{
		int ofs = PluginBase::surface->getSurfaceIdentity(surface) - 1;
		if (ofs < 0)
		{
			// We are the first PluginBitmapCache receiving the callback. Let's decide and offset.

			int ofs = 0;
			for ( auto& surf : m_surfaces )
			{
				if (!surf)
					break;
				ofs++;
			}

			PluginBase::surface->setSurfaceIdentity(surface, ofs + 1);
		}

		if (ofs >= m_surfaces.size())
			m_surfaces.resize(ofs + 1);

		m_surfaces[ofs] = PluginSurface::create(surface);
	}

	//____ _surfacesRemoved() _________________________________________________

	void PluginBitmapCache::_surfacesRemoved(int nSurfaces, wg_obj* pSurfaces)
	{
		for (int i = 0; i < nSurfaces; i++)
		{
			int ofs = PluginBase::surface->getSurfaceIdentity(pSurfaces[i]) - 1;
			m_surfaces[ofs] = nullptr;
		}
	}

	//_____ _cleared() ________________________________________________________

	void PluginBitmapCache::_cleared()
	{
		m_surfaces.clear();
	}




} // namespace wg

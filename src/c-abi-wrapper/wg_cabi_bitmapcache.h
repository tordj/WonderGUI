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

#ifndef	WG_CABI_BITMAPCACHE_DOT_H
#define	WG_CABI_BITMAPCACHE_DOT_H
#pragma once

#include <wg_bitmapcache.h>
#include <wg_cabi.h>
#include <wg_cabi_surface.h>
#include <wg_c_bitmapcache.h>

namespace wg
{

	class CABIBitmapCache;
	typedef	StrongPtr<CABIBitmapCache>		CABIBitmapCache_p;
	typedef	WeakPtr<CABIBitmapCache>		CABIBitmapCache_wp;



	class CABIBitmapCache : public Object
	{
	public:
		//.____ Creation __________________________________________

		static CABIBitmapCache_p	create(wg_obj bitmapCache);

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content ___________________________________________________

		void 	setLimit(int maxBytes);
		int		limit() const;

		int		size() const;

		void	clear();

		//.____ Misc ___________________________________________________________

		void		addListener(BitmapCacheListener* pListener);
		void		removeListener(BitmapCacheListener* pListener);

		std::tuple<Surface_p, CoordI> 	getCacheSlot(SizeI size);
		std::vector<Surface_p>			getSurfaces() const;

		CABISurface *  wrapperSurface(wg_obj surfaceObject);


	protected:
		CABIBitmapCache(wg_obj bitmapCache);
		virtual ~CABIBitmapCache();

		void _surfaceAdded(wg_obj surface);
		void _surfacesRemoved(int nSurfaces, wg_obj * pSurfaces);
		void _cleared();


		wg_obj				m_cCache;
		wg_cacheListener	m_listenerId;


		std::vector<BitmapCacheListener*>	m_listeners;	// List of alive listeners.

		std::vector<CABISurface_p>			m_surfaces;

	};

} // namespace wg
#endif //WG_CABI_BITMAPCACHE_DOT_H

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

#ifndef	WG_IICONDISPLAY_DOT_H
#define	WG_IICONDISPLAY_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_geo.h>
#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_skin.h>
#include <wg_cicondisplay.h>

namespace wg
{

	class IIconDisplay;
	typedef	StrongInterfacePtr<IIconDisplay>	IIconDisplay_p;
	typedef	WeakInterfacePtr<IIconDisplay>		IIconDisplay_wp;

	class IIconDisplay : public Interface
	{
	public:
		/** @private */

		IIconDisplay(CIconDisplay* pComponent) : m_pComponent(pComponent) {}

		//.____ Content _____________________________________________

		inline bool			set( Skin * pIconGfx, Origo origo = Origo::West, BorderI padding = BorderI(0),
								 float scale = 0.f, bool bOverlap = false ) { return m_pComponent->set(pIconGfx,origo,padding,scale,bOverlap); }
		inline void			clear() { m_pComponent->clear(); }

		//.____ Appearance _____________________________________________

		inline bool			setScale( float scaleFactor ) { return m_pComponent->setScale(scaleFactor); }
		inline void			setOrigo( Origo origo ) { m_pComponent->setOrigo(origo); }
		inline void			setPadding( BorderI padding ) { m_pComponent->setPadding(padding); }
		inline void			setOverlap( bool bOverlap ) { m_pComponent->setOverlap(bOverlap); }
		inline void			setSkin( Skin * pSkin ) { m_pComponent->setSkin(pSkin); }

		inline float		scale() const { return m_pComponent->scale(); }
		inline Origo		origo() const { return m_pComponent->origo(); }
		inline BorderI		padding() const { return m_pComponent->padding(); }
		inline bool			overlap() const { return m_pComponent->overlap(); }
		inline Skin_p		skin() const { return m_pComponent->skin(); }

		//.____ Misc __________________________________________________

		inline IIconDisplay_p		ptr() { return IIconDisplay_p(this); }


	protected:
		Object * 			_object() const { return m_pComponent->_object(); };

		CIconDisplay *			m_pComponent;
	};


} // namespace wg
#endif	// WG_IICONDISPLAY_DOT_H

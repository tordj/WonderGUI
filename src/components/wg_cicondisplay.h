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

#ifndef	WG_CICONDISPLAY_DOT_H
#define	WG_CICONDISPLAY_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_geocomponent.h>
#include <wg_geo.h>
#include <wg_skin.h>

namespace wg
{


	//____ CIconDisplay _____________________________________________________________

	class CIconDisplay : public GeoComponent	/** @private */
	{
	public:
		CIconDisplay( GeoComponent::Holder * pHolder );
		virtual ~CIconDisplay() {};


		bool			set( Skin * pIconGfx, Origo origo = Origo::West, BorderI padding = BorderI(0),
									float _scale = 0.f, bool _bOverlap = false );
		void			clear();

		inline bool		isEmpty() const { return !m_pSkin; }

		bool			setScale( float scaleFactor );
		void			setOrigo( Origo origo );
		void			setPadding( BorderI borders );
		void			setOverlap( bool bOverlap );
		void			setSkin( Skin * pSkin );

		float			scale() const { return m_scale; }
		Origo			origo() const { return m_origo; }
		BorderI			padding() const { return m_padding; }
		bool			overlap() const { return m_bOverlap; }
		Skin_p			skin() const { return  m_pSkin; }


		RectI			getIconRect( const RectI& contentRect ) const;
		RectI			getIconRect( const RectI& contentRect, const SizeI& iconSize ) const;
		RectI			getTextRect( const RectI& contentRect, const RectI& iconRect ) const;
		void			onCloneContent( const CIconDisplay * _pOrg );
		SizeI			preferredSize() const;

	protected:

		Origo			m_origo;
		float			m_scale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
		bool			m_bOverlap;
		BorderI		m_padding;
		Skin_p		m_pSkin;

	};


} // namespace wg
#endif	// WG_CICONDISPLAY_DOT_H

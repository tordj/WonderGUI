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
#include <wg_util.h>

namespace wg
{

	class CIconDisplay;
	typedef	StrongComponentPtr<CIconDisplay>	CIconDisplay_p;
	typedef	WeakComponentPtr<CIconDisplay>		CIconDisplay_wp;

	//____ CIconDisplay _____________________________________________________________

	class CIconDisplay : public GeoComponent	/** @private */
	{
	public:
		CIconDisplay( GeoComponent::Holder * pHolder );
		virtual ~CIconDisplay() {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _____________________________________________

		bool			set(Skin * pIconGfx, Origo origo = Origo::West, BorderI padding = BorderI(0),
							float _scale = 0.f, bool _bOverlap = false);
		void			clear();

		inline bool		isEmpty() const { return !m_pSkin; }

		//.____ Appearance _____________________________________________

		inline bool			setScale(float scaleFactor) { return _setScale(scaleFactor); }
		inline void			setOrigo(Origo origo) { _setOrigo(origo); }
		inline void			setPadding(Border padding) { _setPadding(Util::qpixToRaw(padding)); }
		inline void			setOverlap(bool bOverlap) { _setOverlap(bOverlap); }
		inline void			setSkin(Skin * pSkin) { _setSkin(pSkin); }

		inline float		scale() const { return _scale(); }
		inline Origo		origo() const { return _origo(); }
		inline Border		padding() const { return Util::rawToQpix(_padding()); }
		inline bool			overlap() const { return _overlap(); }
		inline Skin_p		skin() const { return _skin(); }

		//.____ Misc __________________________________________________

		inline CIconDisplay_p		ptr() { return CIconDisplay_p(this); }


	protected:


		bool			_setScale( float scaleFactor );
		void			_setOrigo( Origo origo );
		void			_setPadding( BorderI borders );
		void			_setOverlap( bool bOverlap );
		void			_setSkin( Skin * pSkin );

		float			_scale() const { return m_scale; }
		Origo			_origo() const { return m_origo; }
		BorderI			_padding() const { return m_padding; }
		bool			_overlap() const { return m_bOverlap; }
		Skin_p			_skin() const { return  m_pSkin; }


		RectI			_getIconRect( const RectI& contentRect ) const;
		RectI			_getIconRect( const RectI& contentRect, const SizeI& iconSize ) const;
		RectI			_getTextRect( const RectI& contentRect, const RectI& iconRect ) const;
		void			_onCloneContent( const CIconDisplay * _pOrg );
		SizeI			_preferredSize() const;

	private:
		Origo			m_origo;
		float			m_scale;					// Range: 0.f -> 1.f. 0.f = Fixed size.
		bool			m_bOverlap;
		BorderI			m_padding;
		Skin_p			m_pSkin;

	};


} // namespace wg
#endif	// WG_CICONDISPLAY_DOT_H

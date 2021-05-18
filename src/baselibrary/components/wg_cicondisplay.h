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

	class CIconDisplay : public GeoComponent
	{
	public:
		CIconDisplay( GeoComponent::Holder * pHolder );
		virtual ~CIconDisplay() {};

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _____________________________________________

		bool			set(Skin * pIconGfx, Placement placement = Placement::West, Border padding = Border(0),
							float _scaleFactor = 0.f, bool _bOverlap = false);
		void			clear();

		inline bool		isEmpty() const { return !m_pSkin; }

		//.____ Appearance _____________________________________________

		inline bool			setScale(float scaleFactor) { return _setScaleFactor(scaleFactor); }
		inline void			setPlacement(Placement placement) { _setPlacement(placement); }
		inline void			setPadding(Border padding) { _setPadding(padding); }
		inline void			setOverlap(bool bOverlap) { _setOverlap(bOverlap); }
		inline void			setSkin(Skin * pSkin) { _setSkin(pSkin); }
		inline float		scaleFactor() const { return _scaleFactor(); }
		inline Placement	placement() const { return _placement(); }
		inline Border		padding() const { return m_padding; }
		inline bool			overlap() const { return _overlap(); }
		inline Skin_p		skin() const { return _skin(); }

		//.____ Misc __________________________________________________

		inline CIconDisplay_p		ptr() { return CIconDisplay_p(this); }


	protected:


		bool			_setScaleFactor( float scaleFactor );
		void			_setPlacement( Placement placement );
		void			_setPadding( Border borders );
		void			_setOverlap( bool bOverlap );
		void			_setSkin( Skin * pSkin );

		float			_scaleFactor() const { return m_scaleFactor; }
		Placement		_placement() const { return m_placement; }
		BorderSPX		_padding(int scale) const { return Util::align(Util::ptsToSpx(m_padding,scale)); }
		bool			_overlap() const { return m_bOverlap; }
		Skin_p			_skin() const { return  m_pSkin; }


		RectSPX			_getIconRect( const RectSPX& contentRect, int scale ) const;
		RectSPX			_getIconRect( const RectSPX& contentRect, const SizeSPX& iconSize, int scale ) const;
		RectSPX			_getTextRect( const RectSPX& contentRect, const RectSPX& iconRect, int scale ) const;
		void			_onCloneContent( const CIconDisplay * _pOrg );
		SizeSPX			_preferredSize(int scale) const;

	private:
		Placement		m_placement;
		float			m_scaleFactor;					// Range: 0.f -> 1.f. 0.f = Fixed size.
		bool			m_bOverlap;
		Border			m_padding;
		Skin_p			m_pSkin;
	};


} // namespace wg
#endif	// WG_CICONDISPLAY_DOT_H

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
#include <wg_widgetcomponent.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_util.h>

namespace wg
{

	//____ CIconDisplay _____________________________________________________________

	class CIconDisplay : public WidgetComponent
	{
	public:
		CIconDisplay( Widget * pWidget );
		virtual ~CIconDisplay() {};


		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Skin_p			skin;
			Placement		placement = Placement::West;
			Border			padding;
			bool			overlap = false;
		};

		//.____ Content _____________________________________________

		bool			set(Skin * pIconGfx, Placement placement = Placement::West, Border padding = Border(0),
							bool _bOverlap = false);
		void			clear();

		inline bool		isEmpty() const { return !m_pSkin; }

		//.____ Appearance _____________________________________________

		inline void			setPlacement(Placement placement) { _setPlacement(placement); }
		inline void			setPadding(Border padding) { _setPadding(padding); }
		inline void			setOverlap(bool bOverlap) { _setOverlap(bOverlap); }
		inline void			setSkin(Skin * pSkin) { _setSkin(pSkin); }
		inline Placement	placement() const { return _placement(); }
		inline Border		padding() const { return m_padding; }
		inline bool			overlap() const { return _overlap(); }
		inline Skin_p		skin() const { return _skin(); }

		//.____ Internal ______________________________________________________

		void			_initFromBlueprint(const Blueprint& blueprint);

	protected:

		void			_setPlacement( Placement placement );
		void			_setPadding( Border borders );
		void			_setOverlap( bool bOverlap );
		void			_setSkin( Skin * pSkin );

		Placement		_placement() const { return m_placement; }
		BorderSPX		_padding(int scale) const { return Util::align(Util::ptsToSpx(m_padding,scale)); }
		bool			_overlap() const { return m_bOverlap; }
		Skin_p			_skin() const { return  m_pSkin; }


		RectSPX			_getIconRect( const RectSPX& contentRect, int scale ) const;
		RectSPX			_getIconRect( const RectSPX& contentRect, const SizeSPX& iconSize, int scale ) const;
		RectSPX			_getTextRect( const RectSPX& contentRect, const RectSPX& iconRect, int scale ) const;
		SizeSPX			_defaultSize(int scale) const;
		SizeSPX			_defaultSize(int scale, SizeSPX& textSize) const;

	private:
		Placement		m_placement;
		bool			m_bOverlap;
		Border			m_padding;
		Skin_p			m_pSkin;
	};


} // namespace wg
#endif	// WG_CICONDISPLAY_DOT_H

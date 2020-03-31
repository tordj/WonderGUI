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

#ifndef WG_VOLUMEMETER_DOT_H
#define WG_VOLUMEMETER_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_color.h>
#include <wg_geo.h>

#include <vector>

namespace wg
{
	class VolumeMeter;
	typedef	StrongPtr<VolumeMeter>		VolumeMeter_p;
	typedef	WeakPtr<VolumeMeter>		VolumeMeter_wp;


	//____ VolumeMeter ____________________________________________________________

	class VolumeMeter : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static VolumeMeter_p	create() { return VolumeMeter_p(new VolumeMeter()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance __________________________________________

		void			setLEDColors( Color bottomOn, Color middleOn, Color topOn,
								   Color bottomOff, Color middleOff, Color topOff );
		void			setNbLEDs( int bottomSection, int middleSection, int topSection );
		void			setLEDSpacing( float spacing );

		void            enableFades(bool b) { m_bUseFades = b; }

		void			setDirection( Direction direction );
		void            setZeroInMiddle(bool mb) { m_bZeroInMiddle = mb; }

		//.____ Geometry _________________________________________________

		Size			preferredSize() const override;

		//.____ Control ____________________________________________

		void			setValue( float peak, float hold );

		//.____ Geometry ____________________________________________

		void            setSidePadding( float padding ) { m_fSidePadding = padding; }


	protected:
		VolumeMeter();
		virtual ~VolumeMeter();
		virtual Widget* _newOfMyType() const override { return new VolumeMeter(); };

		void			_receive( Msg * pMsg ) override;
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		bool			_alphaTest( const Coord& ofs ) override;
		void			_resize( const Size& size ) override;

		RouteId			m_tickRouteId;

		Direction		m_direction;
		Color			m_LEDColors[3][2];
		int				m_nSectionLEDs[3];
		int				m_nLEDs;
		float			m_LEDSpacing;

		int				m_iPeak;

		int				m_iHold;
		float           m_fPeak;

		float           m_fSidePadding;
		MU				m_iSidePadding;

		bool            m_bZeroInMiddle;

		std::vector<float> m_LEDStates;

		bool            m_bUseFades;
	};

} // namespage wg
#endif //WG_VOLUMEMETER_DOT_H

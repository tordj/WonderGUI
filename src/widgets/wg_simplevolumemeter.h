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
#ifndef WG_SIMPLEVOLUMEMETER_DOT_H
#define WG_SIMPLEVOLUMEMETER_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_color.h>
#include <wg_geo.h>

namespace wg
{

	//____ SimpleVolumeMeter ____________________________________________________________

	class SimpleVolumeMeter;
	typedef	StrongPtr<SimpleVolumeMeter>		SimpleVolumeMeter_p;
	typedef	WeakPtr<SimpleVolumeMeter>		SimpleVolumeMeter_wp;


	class SimpleVolumeMeter : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static SimpleVolumeMeter_p	create() { return SimpleVolumeMeter_p(new SimpleVolumeMeter()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ______________________________________________________

		Size			preferredSize() const override;

		//.____ Appearance _________________________________________________

		void			setColors( HiColor bottom, HiColor middle, HiColor top );
		void			setSections( float bottomFraction, float topFraction );
		void			setHoldHeight( float fraction );

		void            setGaps(float fSides, float fGap) {m_fSidePadding = fSides; m_fGap = fGap; }

		void				setDirection(Direction direction);
		inline Direction	direction() const { return m_direction; }

		//.____ Control ____________________________________________

		void			setValue( float peak, float hold );
		void			setValue( float leftPeak, float leftHold, float rightPeak, float rightHold );


	protected:
		SimpleVolumeMeter();
		virtual ~SimpleVolumeMeter();
		virtual Widget* _newOfMyType() const override { return new SimpleVolumeMeter(); };

		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		bool			_alphaTest( const Coord& ofs ) override;
		void			_resize( const Size& size ) override;

		void			_renderBar( GfxDevice * pDevice, int nb, const Rect& _rect );
		void 			_requestRenderPartial( const Rect& canvas, MU newLeftPeak, MU newLeftHold, MU newRightPeak, MU newRightHold );
		void			_updateValueDisplays( Size sz );
		MU 				_calcIHold( float holdValue, Size canvas );


		Direction		m_direction;
		HiColor			m_sectionColors[3];
		float			m_fSectionHeight[3];
		float			m_fHoldHeight;
		bool			m_bStereo;
		float 			m_fGap;
		float 			m_fSidePadding;
		float			m_fPeak[2];
		float			m_fHold[2];

		// Point values calculated from relative values above

		MU 				m_iGap;
		MU 				m_iSidePadding;
		MU				m_iHoldHeight;
		MU				m_iPeak[2];
		MU 				m_iHold[2];
		MU				m_iSectionHeight[3];
	};


} // namespace wg
#endif //WG_SIMPLEVOLUMEMETER_DOT_H

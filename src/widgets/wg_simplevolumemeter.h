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

		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static SimpleVolumeMeter_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		Size			preferredSize() const;

		//.____ Appearance _________________________________________________

		void			setColors( Color bottom, Color middle, Color top );
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
		virtual Widget* _newOfMyType() const { return new SimpleVolumeMeter(); };

		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_cloneContent( const Widget * _pOrg );
		bool			_alphaTest( const Coord& ofs );
		void			_setSize( const Size& size );
		void			_setSkin( Skin * pSkin );
		
		void			_renderBar( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip );
		void 			_requestRenderPartial( const Rect& canvas, int newLeftPeak, int newLeftHold, int newRightPeak, int newRightHold );
		void			_updateIValues( Size sz );
		int 			_calcIHold( float holdValue, Size canvas );

		
		Direction		m_direction;
		Color			m_sectionColors[3];
		float			m_fSectionHeight[3];
		float			m_fHoldHeight;
		bool			m_bStereo;
		float 			m_fGap;
		float 			m_fSidePadding;
		float			m_fPeak[2];
		float			m_fHold[2];
			
		// Pixel values calculated from relative values above
			
		int 			m_iGap;
		int 			m_iSidePadding;
		int				m_iHoldHeight;
		int				m_iPeak[2];
		int 			m_iHold[2];
		int				m_iSectionHeight[3];
	};


} // namespace wg
#endif //WG_SIMPLEVOLUMEMETER_DOT_H

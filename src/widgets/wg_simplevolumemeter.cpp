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

#include <wg_simplevolumemeter.h>
#include <wg_gfxdevice.h>


namespace wg
{
	const char SimpleVolumeMeter::CLASSNAME[] = {"SimpleVolumeMeter"};

	//____ Constructor ____________________________________________________________

	SimpleVolumeMeter::SimpleVolumeMeter()
	{
		m_sectionColors[0] = Color::Green;
		m_sectionColors[1] = Color::Yellow;
		m_sectionColors[2] = Color::Red;
		
		m_fSectionHeight[0] = 0.75f;
		m_fSectionHeight[1] = 0.18f;
		m_fSectionHeight[2] = 0.07f;
		
		m_fHoldHeight = 0.10f;
		m_bStereo = false;
		m_fPeak[0] = 0.f;
		m_fPeak[1] = 0.f;
		m_fHold[0] = 0.f;
		m_fHold[1] = 0.f;

		m_fGap = 0.1f;
		m_fSidePadding = 0.1f;
		
		_updateIValues( Size(0,0) );
	}

	//____ Destructor _____________________________________________________________

	SimpleVolumeMeter::~SimpleVolumeMeter()
	{
	}

	//____ isInstanceOf() _________________________________________________________
	
	bool SimpleVolumeMeter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SimpleVolumeMeter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SimpleVolumeMeter_p SimpleVolumeMeter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SimpleVolumeMeter_p( static_cast<SimpleVolumeMeter*>(pObject.rawPtr()) );
	
		return 0;
	}

	//____ setColors() ___________________________________________________________

	void SimpleVolumeMeter::setColors( Color bottom, Color middle, Color top )
	{
		if( bottom != m_sectionColors[0] || middle != m_sectionColors[1] || top != m_sectionColors[2] )
		{
			m_sectionColors[0] = bottom;
			m_sectionColors[1] = middle;
			m_sectionColors[2] = top;
			_requestRender();
		}
	}

	//____ setSections() _________________________________________________________

	void SimpleVolumeMeter::setSections( float bottomFraction, float topFraction )
	{
		limit( bottomFraction, 0.f, 1.f );
		limit( topFraction, 0.f, 1.f - bottomFraction );
			
		float middleFraction = 1.f - bottomFraction - topFraction;
		
		if( bottomFraction != m_fSectionHeight[0] || topFraction != m_fSectionHeight[2] )
		{
			m_fSectionHeight[0] = bottomFraction;
			m_fSectionHeight[1] = middleFraction;
			m_fSectionHeight[2] = topFraction;

			Size canvasSize = m_pSkin ? Size() - m_pSkin->contentPadding() : Size();

			_updateIValues( canvasSize );
			_requestRender();
		}
	}

	//____ setHoldHeight() ________________________________________________________

	void SimpleVolumeMeter::setHoldHeight( float fraction )
	{
		limit( fraction, 0.f, 0.25f );
		
		if( m_fHoldHeight != fraction )
		{
			m_fHoldHeight = fraction;
			_requestRender();
		}
	}

	//____ setValue() ______________________________________________________________

	void SimpleVolumeMeter::setValue( float peak, float hold )
	{
		limit( peak, 0.f, 1.f );
		limit( hold, 0.f, 1.f );

		m_fPeak[0] = peak;
		m_fHold[0] = hold;

		Rect canvas = m_pSkin ? m_pSkin->contentRect( size(), m_state ) : Rect(size());

		int	iPeak = peak * canvas.h;
		int iHold = _calcIHold(hold, canvas.h);
		
		if( m_bStereo )
		{
			m_bStereo = false;
			_requestRender();
		}
		else if( m_iPeak[0] != iPeak || m_iHold[0] != iHold )
		{
			_requestRenderPartial( canvas, iPeak, iHold, 0, 0 );
		}

		m_iPeak[0] = iPeak;
		m_iHold[0] = iHold;
	}

	void SimpleVolumeMeter::setValue( float leftPeak, float leftHold, float rightPeak, float rightHold )
	{
		limit( leftPeak, 0.f, 1.f );
		limit( leftHold, 0.f, 1.f );
		limit( rightPeak, 0.f, 1.f );
		limit( rightHold, 0.f, 1.f );

		m_fPeak[0] = leftPeak;
		m_fPeak[1] = rightPeak;
		m_fHold[0] = leftHold;
		m_fHold[1] = rightHold;

		Rect canvas = m_pSkin ? m_pSkin->contentRect( size(), m_state ) : Rect(size());

		int	iPeakL = leftPeak * canvas.h;
		int	iPeakR = rightPeak * canvas.h;
		int iHoldL = _calcIHold(leftHold, canvas.h);
		int iHoldR = _calcIHold(rightHold, canvas.h);

		if( !m_bStereo )
		{
			m_bStereo = true;
			_requestRender();		
		}
		else if( m_iPeak[0] != iPeakL || m_iHold[0] != iHoldL || m_iPeak[1] != iPeakR || m_iHold[1] != iHoldR )
		{
			_requestRenderPartial( canvas, iPeakL, iHoldL, iPeakR, iHoldR );
		}

		m_iPeak[0] = iPeakL;
		m_iPeak[1] = iPeakR;
		m_iHold[0] = iHoldL;
		m_iHold[1] = iHoldR;

	}

	//____ _requestRenderPartial() _________________________________________________

	void SimpleVolumeMeter::_requestRenderPartial( const Rect& canvas, int newLeftPeak, int newLeftHold, int newRightPeak, int newRightHold )
	{
		int	beg = INT_MAX, end = INT_MIN;
		
		if( newLeftPeak != m_iPeak[0] )
		{
			beg = min(newLeftPeak,m_iPeak[0]);
			end = max(newLeftPeak,m_iPeak[0]);
		}

		if( m_bStereo && newRightPeak != m_iPeak[1] )
		{
			int b = min(newRightPeak,m_iPeak[1]);
			if(beg>b)
				beg = b;

			int e = max(newRightPeak,m_iPeak[1]);
			if(end<e)
				end = e;
		}

		if( m_iHoldHeight != 0 )
		{
			if( newLeftHold != m_iHold[0] )
			{
				int b = min(newLeftHold,m_iHold[0]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				int e = max(newLeftHold,m_iHold[0]);
				if(end<e)
					end = e;
			}
			
			if( m_bStereo && newRightHold != m_iHold[1] )
			{
				int b = min(newRightHold,m_iHold[1]) - m_iHoldHeight;
				if(beg>b)
					beg = b;

				int e = max(newRightHold,m_iHold[1]);
				if(end<e)
					end = e;
			}
		}
		
		_requestRender( Rect( canvas.x, canvas.y + canvas.h - end, canvas.w, end-beg) );		
	}


	//____ _calcIHold() ____________________________________________________________

	int SimpleVolumeMeter::_calcIHold( float holdValue, int canvasHeight )
	{
		if( m_iHoldHeight == 0 )
			return 0;					// Should not be visible.

		int height = m_iHoldHeight;
		
		int ofs = (int) (holdValue * canvasHeight);

		if( ofs > m_iSectionHeight[0] )
		{
			if( ofs - height < m_iSectionHeight[0] )
				ofs = m_iSectionHeight[0] + height;
			else if( ofs > m_iSectionHeight[0] + m_iSectionHeight[1] )
			{
				if( ofs - height < m_iSectionHeight[0] + m_iSectionHeight[1] )
					ofs = m_iSectionHeight[0] + m_iSectionHeight[1] + height;
			}
		}

		return ofs;
	}

	//____ preferredSize() ________________________________________________________________

	Size SimpleVolumeMeter::preferredSize() const
	{
		return Size(9,20);
	}

	//____ _onNewSize() ____________________________________________________________________

	void SimpleVolumeMeter::_onNewSize( const Size& size )
	{
		Size canvasSize = m_pSkin ? size - m_pSkin->contentPadding() : size;
		
		_updateIValues( canvasSize );
		_requestRender();
	}

	//____ _onSkinChanged() ________________________________________________________________

	void SimpleVolumeMeter::_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin )
	{
		Size sz = Size();
		
		Rect oldCanvas = pOldSkin ? pOldSkin->contentRect(sz,m_state) : Rect(sz);
		Rect newCanvas = pNewSkin ? pNewSkin->contentRect(sz,m_state) : Rect(sz);
		
		if( oldCanvas != newCanvas )
			_updateIValues( newCanvas );
		
		Widget::_onSkinChanged( pOldSkin, pNewSkin );
	}

	//____ _onRender() _____________________________________________________________________

	void SimpleVolumeMeter::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice, _canvas, _window, _clip);
		
		if( !m_state.isEnabled() )
			return;

		Rect canvas;
		if( m_pSkin )
			canvas = m_pSkin->contentRect(_canvas, m_state);
		else
			canvas = _canvas;
		
		if( m_bStereo )
		{
			Rect r = canvas;
			r.w = (r.w - m_iGap) / 2 - m_iSidePadding;
			r.x += m_iSidePadding;
			_renderBar( pDevice, 0, r, _clip );

			r.x += r.w + m_iGap;
			_renderBar( pDevice, 1, r, _clip );
			
		}
		else 
		{
			Rect r = canvas;
			r.w = r.w - 2 * m_iSidePadding;
			r.x += m_iSidePadding;
			_renderBar( pDevice, 0, r, _clip );
		}
	}

	//____ _renderBar()_____________________________________________________________

	void SimpleVolumeMeter::_renderBar( GfxDevice * pDevice, int nb, const Rect& _rect, const Rect& _clip )
	{
		int peakHeight 	= m_iPeak[nb];
		int holdOfs 	= m_iHold[nb];
		
		// Possibly render Hold
		
		if( m_iHoldHeight > 0 )
		{
			if( holdOfs - m_iHoldHeight > peakHeight )				// Render Hold separately if separated from Peak
			{
				Color c;
				
				if( holdOfs <= m_iSectionHeight[0] )
					c = m_sectionColors[0];
				else if( holdOfs > m_iSectionHeight[0] + m_iSectionHeight[1] )
					c = m_sectionColors[2];
				else 
					c = m_sectionColors[1];

				Rect r( _rect.x, _rect.y + _rect.h - holdOfs, _rect.w, m_iHoldHeight );
				pDevice->fill( Rect( r, _clip ), c );
			}
			else if( holdOfs > peakHeight )
				peakHeight = m_iHold[nb];							// Hold and Peak are connected, so we let Hold extend the peakHeight.
		}
		
		// Render Peak
			
		int ofs = 0;
		
		for( int i = 0 ; i < 3 ; i++ )
		{
			if( peakHeight <= 0 )
				break;
			
			int sectionHeight = m_iSectionHeight[i];
			if( sectionHeight > peakHeight )
				sectionHeight = peakHeight;
			
			Rect r( _rect.x, _rect.y + _rect.h - ofs - sectionHeight, _rect.w, sectionHeight );
			pDevice->fill( Rect( r, _clip ), m_sectionColors[i] );
			
			ofs += sectionHeight;
			peakHeight -= sectionHeight;
		}
			
	}

	//____ _updateIValues() ______________________________________________________

	void SimpleVolumeMeter::_updateIValues( Size sz )
	{
		m_iGap = (int) (sz.w * m_fGap);
		if( m_iGap == 0 && m_fGap > 0.f )
			m_iGap = 1;

		m_iSidePadding = (int) (sz.w * m_fSidePadding);
		if( m_iSidePadding == 0 && m_fSidePadding > 0.f )
			m_iSidePadding = 1;
					
		m_iHoldHeight = m_fHoldHeight * sz.h;
		if( m_iHoldHeight == 0 && m_fHoldHeight > 0.f )
			m_iHoldHeight = 1;

		m_iSectionHeight[0] = (int) (m_fSectionHeight[0] * sz.h + 0.5f);
		m_iSectionHeight[1] =  ((int)((m_fSectionHeight[0] + m_fSectionHeight[1]) * sz.h + 0.5f)) - m_iSectionHeight[0];
		m_iSectionHeight[2] = sz.h - m_iSectionHeight[1] - m_iSectionHeight[0];

		m_iPeak[0] = m_fPeak[0] * sz.h;
		m_iPeak[1] = m_fPeak[1] * sz.h;

		m_iHold[0] = _calcIHold( m_fHold[0], sz.h );
		m_iHold[1] = _calcIHold( m_fHold[1], sz.h );
	}

	//____ _onCloneContent() _________________________________________________________________ 

	void SimpleVolumeMeter::_onCloneContent( const Widget * _pOrg )
	{
		const SimpleVolumeMeter * pOrg = static_cast<const SimpleVolumeMeter*>(_pOrg);

		for( int i = 0 ; i < 3 ; i++ )
		{
			m_sectionColors[i] 	= pOrg->m_sectionColors[i];
			m_fSectionHeight[i] = pOrg->m_fSectionHeight[i];
		}
		
		m_fHoldHeight = pOrg->m_fHoldHeight;
		m_fGap = pOrg->m_fGap;
		m_fSidePadding = pOrg->m_fSidePadding;
		m_bStereo = pOrg->m_bStereo;
		m_fPeak[0] = pOrg->m_fPeak[0];
		m_fPeak[1] = pOrg->m_fPeak[1];
		m_fHold[0] = pOrg->m_fHold[0];
		m_fHold[1] = pOrg->m_fHold[1];
		
		Size canvasSize = m_pSkin ? Size() - m_pSkin->contentPadding() : Size();
		_updateIValues( canvasSize );
	}

	//____ _onAlphaTest() ____________________________________________________________________

	bool SimpleVolumeMeter::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		//TODO: Should we detect on the bars?
		
		return Widget::_onAlphaTest(ofs, sz);
	}
} // namespace wg
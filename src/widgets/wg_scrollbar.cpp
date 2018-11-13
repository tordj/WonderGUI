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

#include <memory.h>
#include <wg_scrollbar.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_scrollbartarget.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	using namespace Util;
	
	const char Scrollbar::CLASSNAME[] = {"Scrollbar"};
	
	
	//____ Scrollbar() ____________________________________________________
	
	Scrollbar::Scrollbar()
	{
		m_pScrollbarTargetInterface = 0;
	
		m_handleSize 		= 1.0;
		m_handlePos 		= 0.0;
	
		m_bHorizontal		= false;
		m_bgPressMode		= JUMP_PAGE;
		m_bPressOnHandle	= false;
		m_handlePressOfs	= 0;
	
		m_btnLayout			= DEFAULT;
		m_headerLen			= 0;
		m_footerLen			= 0;
		
		for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS; i++ )
			m_states[i] = StateEnum::Normal;
	}
	
	//____ ~Scrollbar() _________________________________________________________
	
	Scrollbar::~Scrollbar( void )
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Scrollbar::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Scrollbar::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Scrollbar_p Scrollbar::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Scrollbar_p( static_cast<Scrollbar*>(pObject) );
	
		return 0;
	}
	
	//____ setOrientation() _______________________________________________________
	/**
	 * Set if scrollbar should be horizontal or vertical.
	 *
	 * @param orientation	Set to Orientation::Horizontal to make widget horizontal or
	 * 						Orientation::Vertical to make widget vertical.
	 *
	 * By default a scrollbar is vertical.
	 **/
	
	void Scrollbar::setOrientation( Orientation orientation )
	{
		bool bHorizontal = (orientation == Orientation::Horizontal);
	
		if( m_bHorizontal != bHorizontal )
		{
			m_bHorizontal = bHorizontal;
			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}
	
	
	//____ setBackgroundPressMode() _______________________________________________________
	/**
	 * Set scrollbar behavior for pressing left mouse button on scrollbar background.
	 *
	 * @param	mode	JUMP_PAGE or GOTO_POS depending on behavior wanted.
	 *
	 * The scrollbar can be set to behave in two different ways when left mouse button
	 * is clicked on the scrollbar background.
	 *
	 * The default behavior (JUMP_PAGE) is that it jumps a page forward or backward depending on what
	 * side of the handle the press occurred.
	 *
	 * The alternative behavior (GOTO_POS) is that the scrollbar handle goes directly to the position that
	 * was pressed.
	 *
	 **/
	
	void Scrollbar::setBackgroundPressMode( BgPressMode mode )
	{
		m_bgPressMode = mode;
	}
	
	
	
	//____ setHandle() ____________________________________________________________
	/**
	 * Set relative size and position of the scrollbar handle.
	 *
	 * @param pos	Position of the handle in the range 0.0 -> 1.0.
	 * @param size	Size of the handle in the range 0.0001 -> 1.0.
	 *
	 * Both parameters will be limited to their respective range.
	 * The size of the handle will not get smaller than the skin of the
	 * handle allows, but the value set is still the value later
	 * returned by handlePos(). A size of 1.0 means that the handle will cover
	 * the whole background and make position void.
	 *
	 * The positon of the handle is independent of the size of the handle. A
	 * position of 0.5 always positions the handle straight in the middle of its area.
	 *
	 **/
	
	void Scrollbar::setHandle( float _pos, float _size )
	{
		limit( _size, 0.0001f, 1.f );
		limit( _pos, 0.f, 1.f );
	
		if( m_handlePos == _pos && m_handleSize == _size )
			return;
	
		m_handlePos		= _pos;
		m_handleSize 	= _size;
	
		_requestRender();
	}
	
	//____ setHandlePos() _________________________________________________________
	/**
	 * Set relative position of scrollbar handle.
	 *
	 * @param	pos		Position of the handle in the range 0.0 -> 1.0.
	 *
	 * Position is limited to its range before applied.
	 * The positon of the handle is independent of the size of the handle. A
	 * position of 0.5 always positions the handle straight in the middle of its area.
	 *
	 **/
	
	void Scrollbar::setHandlePos( float pos )
	{
		limit( pos, 0.f, 1.f );
	
		if( pos > m_handlePos-0.000001 && pos < m_handlePos+0.000001 )
			return;
	
		m_handlePos = pos;
		_requestRender();
	}
	
	//____ setHandlePixelPos() ___________________________________________________
	/**
	 * Set position of scrollbar handle in pixels.
	 *
	 * @param	pos		Position of the handle, as a pixel offset from the start
	 *					of the slider area.
	 *
	 * Position is limited to its valid range before applied.
	 * The scrollbar handle is moved so that it starts specified number of pixels
	 * from the start of the slider area. The generated position is stored as as a
	 * relative value, making it move to keep its relative position when the
	 * scrollbar widget or handle is resized.
	 *
	 **/
	
	void Scrollbar::setHandlePixelPos( int pos )
	{
		int		handlePos, handleLen;
		_viewToPosLen( &handlePos, &handleLen );
	
		int		length;
		if( m_bHorizontal )
		{
			length = size().w;
			if( m_pSkin )
				length -= m_pSkin->contentPadding().w;
		}
		else
		{
			length = size().h;
			if( m_pSkin )
				length -= m_pSkin->contentPadding().h;
		}
	
		length -= m_headerLen + m_footerLen;
	
		float scrollhandlePos = 0.f;
		if( m_handleSize < 1.f)
			scrollhandlePos = ((float)(pos - (handleLen >> 1))) / (length - handleLen);
	
		setHandlePos(scrollhandlePos);
	}
	
	//____ setHandleSize() ________________________________________________________
	/**
	 * Set relative size of scrollbar handle.
	 *
	 * @param	size		Size of the handle in the range 0.0001 -> 1.0.
	 *
	 * Size is limited to its range before applied.
	 *
	 * The size of the handle will not get smaller than the skin of the
	 * handle allows, but the value set is still the value later
	 * returned by handlePos(). A size of 1.0 means that the handle will cover
	 * the whole background and make position void.
	 *
	 **/
	
	void Scrollbar::setHandleSize( float _size )
	{
		limit( _size, 0.0001f, 1.f );
	
		if( _size == m_handleSize )
			return;
	
		m_handleSize = _size;
		_requestRender();
	}
	
	
	//____ setSkins() ____________________________________________________________
	/**
	 * Set all skins in one go.
	 *
	 * @param pBaseSkin			The normal widget skin, enclosing all components.
	 * @param pBackgroundSkin	Background skin for the scrollbar handle area.
	 * @param pHandleSkin		Skin for the scrollbar handle.
	 * @param pBwdButtonSkin	Skin for the backward (up or left) button.
	 * @param pFwdButtonSkin	Skin for the forward (down or right) button.
	 *
	 * Skinning a scrollbar is a little more complex than a normal widget since it
	 * is built up by different components that needs to be skinned individually.
	 *
	 * These components are the forward and backward buttons, the area for the scrollbar
	 * handle to move over and the scrollbar handle itself. Additionally, the normal widget
	 * skin, which encloses all components is still available. This gives a total of five
	 * individual skins.
	 *
	 **/
	
	
	void Scrollbar::setSkins( Skin * pBaseSkin, Skin * pBackgroundSkin,
								Skin * pHandleSkin,
								Skin * pBwdButtonSkin, Skin * pFwdButtonSkin )
	{
		m_pSkin 		= pBaseSkin;
		m_pBgSkin		= pBackgroundSkin;
		m_pHandleSkin	= pHandleSkin;
		m_pBtnFwdSkin	= pFwdButtonSkin;
		m_pBtnBwdSkin	= pBwdButtonSkin;
	
		_headerFooterChanged();
		_updateMinSize();
		_requestRender();
	}
	
	//____ setBackgroundSkin() ____________________________________________________________

	void Scrollbar::setBackgroundSkin( Skin * pSkin )
	{
		if( pSkin != m_pBgSkin )
		{
			m_pBgSkin = pSkin;

			_updateMinSize();
			_requestRender();
		}
	}

	//____ setHandleSkin() ____________________________________________________________

	void Scrollbar::setHandleSkin( Skin * pSkin )
	{
		if( pSkin != m_pHandleSkin )
		{
			m_pHandleSkin = pSkin;

			_updateMinSize();
			_requestRender();
		}
	}

	//____ setBwdButtonSkin() ____________________________________________________________

	void Scrollbar::setBwdButtonSkin( Skin * pSkin )
	{
		if( pSkin != m_pBtnBwdSkin )
		{
			m_pBtnBwdSkin = pSkin;

			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}

	//____ setFwdButtonSkin() ____________________________________________________________

	void Scrollbar::setFwdButtonSkin( Skin * pSkin )
	{
		if( pSkin != m_pBtnFwdSkin )
		{
			m_pBtnFwdSkin = pSkin;

			_headerFooterChanged();
			_updateMinSize();
			_requestRender();
		}
	}


	//____ setButtonLayout() ______________________________________________________
	/**
	 * Set the layout of the scrollbar buttons
	 *
	 * @param 	layout		Enum or bitmask specifying what buttons to show.
	 *
	 * The scrollbar can have a total of four buttons, a forward
	 * and backward button in the header and the same in the footer.
	 *
	 * ButtonLayout is essentially a bitmask specifying which of the
	 * buttons are displayed, so any combination can be achieved by
	 * combining different values.
	 *
	 * The default is one
	 *
	 **/
	
	void Scrollbar::setButtonLayout(  BtnLayout layout )
	{
		m_btnLayout = layout;
	
		_headerFooterChanged();
	}
	
	//____ setScrollbarTarget() _______________________________________________________
	
	bool Scrollbar::setScrollbarTarget( ScrollbarTarget * pTarget )
	{
		// Release previous target (if any)
	
		if( m_pScrollbarTargetWidget )
			m_pScrollbarTargetInterface->m_pScrollbar = 0;
	
		// Set new target
	
		if( pTarget == 0 )
		{
			m_pScrollbarTargetInterface	= 0;
			m_pScrollbarTargetWidget	= 0;
		}
		else
		{
			if( pTarget->m_pScrollbar )
				return false;									// Target is already controlled by another scrollbar.
	
			m_pScrollbarTargetInterface 	= pTarget;
			m_pScrollbarTargetWidget		= pTarget->_getWidget();
			m_pScrollbarTargetInterface->m_pScrollbar = this;
			_setHandle( pTarget->_getHandlePosition(), pTarget->_getHandleSize() );
		}
		return true;
	}
	
	//____ _headerFooterChanged() _______________________________________________
	
	void Scrollbar::_headerFooterChanged()
	{
		int	fwdLen = 0, bwdLen = 0;
	
		if( m_bHorizontal )
		{
			if( m_pBtnFwdSkin )
				fwdLen = m_pBtnFwdSkin->preferredSize().w;
			if( m_pBtnBwdSkin )
				bwdLen = m_pBtnBwdSkin->preferredSize().w;
		}
		else
		{
			if( m_pBtnFwdSkin )
				fwdLen = m_pBtnFwdSkin->preferredSize().h;
			if( m_pBtnBwdSkin )
				bwdLen = m_pBtnBwdSkin->preferredSize().h;
		}
	
		int	headerLen = 0;
		int footerLen = 0;
	
		if( (m_btnLayout & HEADER_BWD) )
			headerLen += bwdLen;
	
		if( (m_btnLayout & HEADER_FWD) )
			headerLen += fwdLen;
	
		if( (m_btnLayout & FOOTER_BWD) )
			footerLen += bwdLen;
	
		if( (m_btnLayout & FOOTER_FWD) )
			footerLen += fwdLen;
	
		if( headerLen != m_headerLen || footerLen != m_footerLen )
		{
			m_headerLen = headerLen;
			m_footerLen = footerLen;
	
			_updateMinSize();
		}
	
		_requestRender();
	}
	
	
	
	//____ _cloneContent() _______________________________________________________
	
	void Scrollbar::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Scrollbar * pOrg = (Scrollbar *) _pOrg;
	
		m_pBgSkin			= pOrg->m_pBgSkin;
		m_pHandleSkin		= pOrg->m_pHandleSkin;
		m_pBtnFwdSkin		= pOrg->m_pBtnFwdSkin;
		m_pBtnBwdSkin		= pOrg->m_pBtnBwdSkin;
	
	  	m_handlePos			= pOrg->m_handlePos;
		m_handleSize		= pOrg->m_handleSize;
	
		m_bHorizontal		= pOrg->m_bHorizontal;
		m_bgPressMode		= pOrg->m_bgPressMode;
	
		m_bPressOnHandle	= 0;
		m_handlePressOfs	= 0;
	
		m_btnLayout			= pOrg->m_btnLayout;
		m_headerLen			= pOrg->m_headerLen;
		m_footerLen			= pOrg->m_footerLen;
	
		m_minSize			= pOrg->m_minSize;
	}
	
	//____ _viewToPosLen() _________________________________________________________
	
	void	Scrollbar::_viewToPosLen( int * _wpPos, int * _wpLen )
	{
		// changes by Viktor.
	
		// using floats here results in errors.
		//float pos, len;
		int pos, len;
	
		int maxLen;
		if( m_bHorizontal )
		{
			maxLen = size().w;
			if( m_pSkin )
				maxLen -= m_pSkin->contentPadding().w;
		}
		else
		{
			maxLen = size().h;
			if( m_pSkin )
				maxLen -= m_pSkin->contentPadding().h;
		}
		maxLen -= m_headerLen + m_footerLen;
	
		//len = m_handleSize * maxLen;
		len = (int)(m_handleSize * (float)maxLen);
	
		int		minLen;
	
		if( m_bHorizontal )
			minLen = m_pHandleSkin->minSize().w;
		else
			minLen = m_pHandleSkin->minSize().h;
	
		if( minLen < 4 )
			minLen = 4;
	
	
		if( len < minLen )
		{
			//len = (float) minLen;
			len = minLen;
		}
	
		//pos = m_handlePos * (maxLen-len);
		pos = (int)(m_handlePos * (float)(maxLen-len));
	
		if( pos + len > maxLen )
			pos = maxLen - len;
	
	//	pos += m_headerLen;
	
		//* _wpPos = (int) pos;
		//* _wpLen = (int) len;
		* _wpPos = pos;
		* _wpLen = len;
	//	* _wpLen = ((int)(pos + len)) - (int) pos;
	}
	
	
	//____ _setState() ______________________________________________________
	
	void Scrollbar::_setState( State state )
	{
		if( state.isEnabled() != m_state.isEnabled() )
		{
			for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
				m_states[i].setEnabled(state.isEnabled());
		}
		_requestRender();
	}
	
	
	//____ _refresh() _______________________________________________________
	
	void Scrollbar::_refresh( void )
	{
		Widget::_refresh();
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size Scrollbar::preferredSize() const
	{
		Size sz = m_minSize;
	
		// Add 50 pixels in the scrollbars direction for best size.
	
		if( m_bHorizontal )
			sz.w += 50;
		else
			sz.h += 50;
	
		return sz;
	}
	
	
	//____ _updateMinSize() ________________________________________________________
	
	void Scrollbar::_updateMinSize()
	{
		int	minW = 4;
		int	minH = 4;
	
		// Check min w/h for BgGfx.
	
		if( m_pBgSkin )
		{
			Size sz = m_pBgSkin->minSize();
	
			minW = max( minW, sz.w );
			minH = max( minH, sz.h );
		}
	
		// Check min w/h for BarGfx.
	
		if( m_pHandleSkin )
		{
			Size sz = m_pHandleSkin->minSize();
	
			minW = max( minW, sz.w );
			minH = max( minH, sz.h );
		}
	
	
		// Add header and footer to minW/minH from scrollbar part.
	
		if( m_bHorizontal )
			minW += m_footerLen + m_headerLen;
		else
			minH += m_footerLen + m_headerLen;
	
	
		// Check min w/h for forward button.
	
		if( m_pBtnFwdSkin && (m_btnLayout & (HEADER_FWD | FOOTER_FWD)) )
		{
			Size sz = m_pBtnFwdSkin->preferredSize();
	
			minW = max( minW, sz.w );
			minH = max( minH, sz.h );
		}
	
		// Check min w/h for backward button.
	
		if( m_pBtnBwdSkin && (m_btnLayout & (HEADER_BWD | FOOTER_BWD)) )
		{
			Size sz = m_pBtnBwdSkin->preferredSize();
	
			minW = max( minW, sz.w );
			minH = max( minH, sz.h );
		}
	
		// Add padding for base skin
	
		if( m_pSkin )
		{
			minW += m_pSkin->contentPadding().w;
			minH += m_pSkin->contentPadding().h;
		}
	
		// Set if changed.
	
		if( minW != m_minSize.w || minH != m_minSize.h )
		{
			m_minSize.w = minW;
			m_minSize.h = minH;
	
			_requestResize();
		}
	}
	
	//____ _renderButton() _________________________________________________________
	
	void Scrollbar::_renderButton( GfxDevice * pDevice, const Rect& _clip, Rect& _dest, Skin * pSkin, State state )
	{
			if( m_bHorizontal )
				_dest.w = pSkin->preferredSize().w;
			else
				_dest.h = pSkin->preferredSize().h;
	
			pSkin->render( pDevice, _dest, state, _clip );
	
			if( m_bHorizontal )
				_dest.x += _dest.w;
			else
				_dest.y += _dest.h;
	}
	
	//____ _render() ________________________________________________________
	
	void Scrollbar::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_render(pDevice,_canvas,_window,_clip);
	
		Rect	dest = _canvas;
		if( m_pSkin )
			dest = m_pSkin->contentRect(_canvas,m_state);
	
		// Render header buttons
	
		if( m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD) )
			_renderButton( pDevice, _clip, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD] );
	
		if( m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD) )
			_renderButton( pDevice, _clip, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD] );
	
		// Render background (if any).
	
		if( m_bHorizontal )
			dest.w = size().w - m_headerLen - m_footerLen;
		else
			dest.h = size().h - m_headerLen - m_footerLen;
	
		if( m_pBgSkin )
			m_pBgSkin->render( pDevice, dest, m_states[C_BG], _clip );
	
		// Render the handle
	
		if( m_pHandleSkin )
		{
			int handlePos;
			int handleLen;
			_viewToPosLen( &handlePos, &handleLen );
	
			Rect	handleDest;
			if( m_bHorizontal )
				handleDest = Rect( dest.x + handlePos, dest.y, handleLen, dest.h );
			else
				handleDest = Rect( dest.x, dest.y + handlePos, dest.w, handleLen );
	
			m_pHandleSkin->render( pDevice, handleDest, m_states[C_HANDLE], _clip );
		}
	
		// Render footer buttons
	
		if( m_bHorizontal )
			dest.x += dest.w;
		else
			dest.y += dest.h;
	
		if( m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD) )
			_renderButton( pDevice, _clip, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD] );
	
		if( m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD) )
			_renderButton( pDevice, _clip, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD] );
	}
	
	//____ _alphaTest() ______________________________________________________
	
	bool Scrollbar::_alphaTest( const Coord& ofs )
	{
		if( _findMarkedComponent( ofs ) != C_NONE )
			return true;
	
		return Widget::_alphaTest(ofs);
	}
	
	//____ _markTestButton() _______________________________________________________
	
	bool Scrollbar::_markTestButton( Coord ofs, Rect& _dest, Skin * pSkin, State state )
	{
			if( m_bHorizontal )
				_dest.w = pSkin->preferredSize().w;
			else
				_dest.h = pSkin->preferredSize().h;
	
			bool retVal = pSkin->markTest( ofs, _dest, state, m_markOpacity );
	
			if( m_bHorizontal )
				_dest.x += _dest.w;
			else
				_dest.y += _dest.h;
	
			return retVal;
	}
	
	//____ _findMarkedComponent() __________________________________________________
	
	Scrollbar::Component Scrollbar::_findMarkedComponent( Coord ofs )
	{
		Rect canvas;
	
		if( m_pSkin )
			canvas = m_pSkin->contentRect( Rect(0,0,size()),m_state);
		else 
			canvas = Rect(0,0,size());
	
		Rect dest = canvas;
	
		// First of all, do a mark test against the header buttons...
	
		if( m_pBtnBwdSkin && (m_btnLayout & HEADER_BWD) )
		{
			if( _markTestButton( ofs, dest, m_pBtnBwdSkin, m_states[C_HEADER_BWD]) )
				return C_HEADER_BWD;
		}
	
		if( m_pBtnFwdSkin && (m_btnLayout & HEADER_FWD) )
		{
			if( _markTestButton( ofs, dest, m_pBtnFwdSkin, m_states[C_HEADER_FWD]) )
				return C_HEADER_FWD;
		}
	
		// Then do a mark test against the footer buttons...
	
		if( m_bHorizontal )
			dest.x = canvas.x + canvas.w - m_footerLen;
		else
			dest.y = canvas.y + canvas.h - m_footerLen;
	
		if( m_pBtnBwdSkin && (m_btnLayout & FOOTER_BWD) )
		{
			if( _markTestButton( ofs, dest, m_pBtnBwdSkin, m_states[C_FOOTER_BWD]) )
				return C_FOOTER_BWD;
		}
	
		if( m_pBtnFwdSkin && (m_btnLayout & FOOTER_FWD) )
		{
			if( _markTestButton( ofs, dest, m_pBtnFwdSkin, m_states[C_FOOTER_FWD]) )
				return C_FOOTER_FWD;
		}
	
		// Then, do a mark test against the dragbar...
	
		if( _markTestHandle( ofs ) == true )
			return C_HANDLE;
	
		// Do a mark test against the dragbar background.
	
		Rect	r = canvas;
	
		if( m_bHorizontal )
		{
			r.x += m_headerLen;
			r.w -= m_headerLen + m_footerLen;
		}
		else
		{
			r.y += m_headerLen;
			r.h -= m_headerLen + m_footerLen;
		}
	
		if( m_pBgSkin && m_pBgSkin->markTest( ofs, r, m_states[C_BG], m_markOpacity ) )
			return C_BG;
	
		return C_NONE;
	}
	
	//____ _unhoverReqRender() ______________________________________________________
	
	void Scrollbar::_unhoverReqRender()
	{
		for( int i = 0 ; i < C_NUMBER_OF_COMPONENTS ; i++ )
			m_states[i].setHovered(false);
	
		_requestRender();
	}
	
	//____ _receive() ______________________________________________________________
	
	void Scrollbar::_receive( Msg * pMsg )
	{
		int		handlePos, handleLen;
		_viewToPosLen( &handlePos, &handleLen );
	
		MsgRouter_p	pHandler = Base::msgRouter();
		Coord pos = InputMsg::cast(pMsg)->pointerPos() - globalPos();
	
		int		pointerOfs;
		int		length;
	
		Rect contentRect;
		if( m_pSkin )
			contentRect = m_pSkin->contentRect(Rect(0,0,size()),m_state);
		else
			contentRect = Rect(0,0,size());
	
		if( m_bHorizontal )
		{
			pointerOfs = pos.x - contentRect.x;
			length = contentRect.w;
		}
		else
		{
			pointerOfs = pos.y - contentRect.y;
			length = contentRect.h;
		}
	
		length -= m_headerLen + m_footerLen;
		pointerOfs -= m_headerLen;
	
		switch( pMsg->type() )
		{
			case MsgType::MouseRelease:
			{
				if( MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left )
					return;
	
				// Just put them all to NORMAL and request render.
				// Release is followed by over before render anyway so right one will be highlighted.
	
				_unhoverReqRender();
				break;
			}
	
			case MsgType::MouseLeave:
			{
				// Turn any MARKED/SELECTED button/bg back to NORMAL.
				// Turn handle back to NORMAL only if MARKED (selected handle should remain selected).
				// Request render only if something changed (which it has unless bar was SELECTED...).
	
				if( m_states[C_HANDLE].isPressed() )
					return;
	
				_unhoverReqRender();
				break;
			}
	
			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				if( m_states[C_HANDLE].isPressed() )
					return;
	
				Component c = _findMarkedComponent(pos);
	
				if( (c != C_NONE && !m_states[c].isHovered()) || (c == C_BG && m_states[C_HANDLE].isHovered()) )
				{
					_unhoverReqRender();
					m_states[c].setHovered(true);
					if( c == C_HANDLE )
						m_states[C_BG].setHovered(true);			// Always also mark bg if bar is marked.
				}
				else if( c == C_BG && m_states[C_HANDLE].isHovered() )
				{
					m_states[C_HANDLE].setHovered(false);
					m_states[C_BG].setHovered(false);
					_requestRender();
				}
	
	
				break;
			}
	
			case MsgType::MousePress:
			{
				if( MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left )
					return;
	
				Component c = _findMarkedComponent(pos);
	
				_unhoverReqRender();
				m_states[c].setPressed(true);
	
				if( c == C_HANDLE )
				{
					m_handlePressOfs = pointerOfs - handlePos;
					m_states[C_BG].setHovered(true);			// Always mark bg if bar is pressed.
				}
				else if( c == C_BG )
				{
					switch( m_bgPressMode )
					{
					case JUMP_PAGE:
	
						if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						{
							if( pointerOfs - handlePos < handleLen/2 )
								setHandlePos( m_pScrollbarTargetInterface->_jumpBwd() );
							else
								setHandlePos( m_pScrollbarTargetInterface->_jumpFwd() );
						}
	
						int pxlPos, pxlLen;
						_viewToPosLen( &pxlPos, &pxlLen );
						pHandler->post( RangeUpdateMsg::create(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
	
						break;
					case GOTO_POS:
						m_states[C_HANDLE].setPressed(true);
						m_states[C_BG].setHovered(true);
						m_handlePressOfs = handleLen/2;
						setHandlePixelPos( pointerOfs );
						break;
					default:
	//					assert( false );
						break;
					}
	
				}
				else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
				{
					if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						setHandlePos( m_pScrollbarTargetInterface->_stepFwd() );
				}
				else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
				{
					if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						setHandlePos( m_pScrollbarTargetInterface->_stepBwd() );
				}
	
	
				int pxlPos, pxlLen;
				_viewToPosLen( &pxlPos, &pxlLen );
				pHandler->post( RangeUpdateMsg::create(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
				break;
			}
	
			case MsgType::MouseRepeat:
			{
				if( MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left )
					return;
	
				if( m_states[C_HANDLE].isPressed() )
					return;
	
				Component c = _findMarkedComponent(pos);
	
				if( c == C_BG )
				{
					if( pointerOfs - handlePos < handleLen/2 )
					{
						if( m_pScrollbarTargetWidget.rawPtr() != 0 )
							setHandlePos( m_pScrollbarTargetInterface->_jumpBwd() );
					}
					else
					{
						if( m_pScrollbarTargetWidget.rawPtr() != 0 )
							setHandlePos( m_pScrollbarTargetInterface->_jumpFwd() );
					}
				}
				else if( c == C_HEADER_FWD || c == C_FOOTER_FWD )
				{
					if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						setHandlePos( m_pScrollbarTargetInterface->_stepFwd() );
				}
				else if( c == C_HEADER_BWD || c == C_FOOTER_BWD )
				{
					if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						setHandlePos( m_pScrollbarTargetInterface->_stepBwd() );
				}
	
				int pxlPos, pxlLen;
				_viewToPosLen( &pxlPos, &pxlLen );
				pHandler->post( RangeUpdateMsg::create(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
				break;
			}
	
			case MsgType::MouseDrag:
			{
				if( MouseButtonMsg::cast(pMsg)->button() != MouseButton::Left )
					return;
	
				if( m_states[C_HANDLE].isPressed() )
				{
					float	scrollhandlePos = 0.f;
	
					if( m_handleSize < 1.f)
						scrollhandlePos = ((float)(pointerOfs - m_handlePressOfs)) / (length - handleLen);
	
					limit( scrollhandlePos, 0.f, 1.f );
	
					if( scrollhandlePos != m_handlePos )
					{
						m_handlePos = scrollhandlePos;
	
						if( m_pScrollbarTargetWidget.rawPtr() != 0 )
							m_handlePos = m_pScrollbarTargetInterface->_setPosition(m_handlePos);
	
						int pxlPos, pxlLen;
						_viewToPosLen( &pxlPos, &pxlLen );
						pHandler->post(RangeUpdateMsg::create(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
						_requestRender();
					}
				}
				break;
			}
			
			case MsgType::WheelRoll:
			{
				WheelRollMsg_p p = WheelRollMsg::cast(pMsg);
	
				if( p->wheel() == 1 )
				{
					int distance = p->distance().y;		//TODO: Should we support horizontal wheel roll for horizontal scrollbar?
					if( m_pScrollbarTargetWidget.rawPtr() != 0 )
						setHandlePos( m_pScrollbarTargetInterface->_wheelRolled(distance) );
					
					int pxlPos, pxlLen;
					_viewToPosLen( &pxlPos, &pxlLen );
					pHandler->post( RangeUpdateMsg::create(this,pxlPos,pxlLen,m_handlePos,m_handleSize,true) );
					pMsg->swallow();
				}
			}
			
	        default:
	            break;
	
		}
	
		// Swallow all button 1 messages.
	
		if( pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == MouseButton::Left )
				pMsg->swallow();
	
	}
	
	
	//____ _markTestHandle() _______________________________________________________
	
	bool Scrollbar::_markTestHandle( Coord ofs )
	{
		if( !m_pHandleSkin )
			return false;
	
		int   handlePos, handleLen;
		_viewToPosLen( &handlePos, &handleLen );
	
		Rect area(0,0,size());
	
		if( m_pSkin )
			area = m_pSkin->contentRect(area,m_state);
	
		if( m_bHorizontal )
		{
			area.x = handlePos + m_headerLen;
			area.w = handleLen;
		}
		else
		{
			area.y = handlePos + m_headerLen;
			area.h = handleLen;
		}
	
		return m_pHandleSkin->markTest( ofs, area, m_states[C_HANDLE], m_markOpacity );
	}
	
	//____ _setHandle() ____________________________________________________________
	
	bool Scrollbar::_setHandle( float _pos, float _size )
	{
		limit( _size, 0.0001f, 1.f );
		limit( _pos, 0.f, 1.f );
	
		if( m_handlePos == _pos && m_handleSize == _size )
			return true;
	
		m_handlePos		= _pos;
		m_handleSize 	= _size;
	
		_requestRender();
		return	true;
	}

} // namespace wg

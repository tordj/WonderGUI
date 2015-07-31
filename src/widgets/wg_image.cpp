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



#include <wg_image.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_surface.h>

namespace wg 
{
	
	
	const char WgImage::CLASSNAME[] = {"Image"};
	
	//____ Constructor ____________________________________________________________
	
	WgImage::WgImage() : m_pSurface(0)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	WgImage::~WgImage()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgImage::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgImage::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgImage_p WgImage::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgImage_p( static_cast<WgImage*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ SetImage() _____________________________________________________________
	
	void WgImage::SetImage( const WgSurface_p& pSurface, const WgRect& rect )
	{
		if( pSurface != m_pSurface || rect != m_rect )
		{
			bool bResize = false;
			if( rect.w != m_rect.w || rect.h != m_rect.h )
				bResize = true;
	
			m_pSurface = pSurface;
	
			if( pSurface )
				m_rect = WgRect( rect, WgRect(pSurface->size()) );
			else
				m_rect.clear();
	
			if( bResize )
				_requestResize();
			_requestRender();
		}
	}
	
	void WgImage::SetImage( const WgSurface_p& pSurface )
	{
		if( pSurface != m_pSurface )
		{
			bool bResize = false;
			if( pSurface->size() != m_pSurface->size() )
				bResize = true;
	
			m_pSurface = pSurface;
	
			if( pSurface )
				m_rect = pSurface->size();
			else
				m_rect.clear();
	
			if( bResize )
				_requestResize();
			_requestRender();
		}
	}
	
	
	
	//____ preferredSize() _____________________________________________________________
	
	WgSize WgImage::preferredSize() const
	{
		if( m_pSurface )
		{
			if( m_pSkin )
				return m_pSkin->sizeForContent( m_rect.size() );
			else
				return m_rect.size();
		}
	
		return WgWidget::preferredSize();
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void WgImage::_onCloneContent( const WgWidget * _pOrg )
	{
		WgImage * pOrg = (WgImage*) _pOrg;
	
		m_pSurface	= pOrg->m_pSurface;
		m_rect		= pOrg->m_rect;
	}
	
	//____ _onRender() _____________________________________________________________
	
	void WgImage::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
	{
		WgWidget::_onRender(pDevice,_canvas,_window,_clip);
	
		if( m_pSurface && !m_rect.isEmpty() )
		{
			WgRect dest;
			if( m_pSkin )
				dest = m_pSkin->contentRect( _canvas, state() );
			else
				dest = _canvas;
	
			pDevice->clipStretchBlit( _clip, m_pSurface, m_rect, dest );
		}
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool WgImage::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
	{
		if( m_pSurface && !m_rect.isEmpty() )
		{
			WgRect dest;
			if( m_pSkin )
				dest = m_pSkin->contentRect( sz, state() );
			else
				dest = size();
	
			if( WgUtil::markTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity ) )
				return true;
		}
	
		return WgWidget::_onAlphaTest(ofs,sz);
	}

} // namespace wg

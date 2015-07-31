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
	
	
	const char Image::CLASSNAME[] = {"Image"};
	
	//____ Constructor ____________________________________________________________
	
	Image::Image() : m_pSurface(0)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	Image::~Image()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Image::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Image::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Image_p Image::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Image_p( static_cast<Image*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setImage() _____________________________________________________________
	
	void Image::setImage( const Surface_p& pSurface, const Rect& rect )
	{
		if( pSurface != m_pSurface || rect != m_rect )
		{
			bool bResize = false;
			if( rect.w != m_rect.w || rect.h != m_rect.h )
				bResize = true;
	
			m_pSurface = pSurface;
	
			if( pSurface )
				m_rect = Rect( rect, Rect(pSurface->size()) );
			else
				m_rect.clear();
	
			if( bResize )
				_requestResize();
			_requestRender();
		}
	}
	
	void Image::setImage( const Surface_p& pSurface )
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
	
	Size Image::preferredSize() const
	{
		if( m_pSurface )
		{
			if( m_pSkin )
				return m_pSkin->sizeForContent( m_rect.size() );
			else
				return m_rect.size();
		}
	
		return Widget::preferredSize();
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void Image::_onCloneContent( const Widget * _pOrg )
	{
		Image * pOrg = (Image*) _pOrg;
	
		m_pSurface	= pOrg->m_pSurface;
		m_rect		= pOrg->m_rect;
	}
	
	//____ _onRender() _____________________________________________________________
	
	void Image::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		if( m_pSurface && !m_rect.isEmpty() )
		{
			Rect dest;
			if( m_pSkin )
				dest = m_pSkin->contentRect( _canvas, state() );
			else
				dest = _canvas;
	
			pDevice->clipStretchBlit( _clip, m_pSurface, m_rect, dest );
		}
	}
	
	//____ _onAlphaTest() ___________________________________________________________
	
	bool Image::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		if( m_pSurface && !m_rect.isEmpty() )
		{
			Rect dest;
			if( m_pSkin )
				dest = m_pSkin->contentRect( sz, state() );
			else
				dest = size();
	
			if( WgUtil::markTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity ) )
				return true;
		}
	
		return Widget::_onAlphaTest(ofs,sz);
	}

} // namespace wg

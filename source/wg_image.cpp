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


const char WgImage::CLASSNAME[] = {"Image"};

//____ Constructor ____________________________________________________________

WgImage::WgImage() : m_pSurface(0)
{
}

//____ Destructor _____________________________________________________________

WgImage::~WgImage()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgImage::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgImage::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgImagePtr WgImage::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgImagePtr( static_cast<WgImage*>(pObject.RawPtr()) );

	return 0;
}

//____ SetImage() _____________________________________________________________

void WgImage::SetImage( const WgSurfacePtr& pSurface, const WgRect& rect )
{
	if( pSurface != m_pSurface || rect != m_rect )
	{
		bool bResize = false;
		if( rect.w != m_rect.w || rect.h != m_rect.h )
			bResize = true;

		m_pSurface = pSurface;

		if( pSurface )
			m_rect = WgRect( rect, WgRect(pSurface->Size()) );
		else
			m_rect.Clear();

		if( bResize )
			_requestResize();
		_requestRender();
	}
}

void WgImage::SetImage( const WgSurfacePtr& pSurface )
{
	if( pSurface != m_pSurface )
	{
		bool bResize = false;
		if( pSurface->Size() != m_pSurface->Size() )
			bResize = true;

		m_pSurface = pSurface;

		if( pSurface )
			m_rect = pSurface->Size();
		else
			m_rect.Clear();

		if( bResize )
			_requestResize();
		_requestRender();
	}
}



//____ PreferredSize() _____________________________________________________________

WgSize WgImage::PreferredSize() const
{
	if( m_pSurface )
	{
		if( m_pSkin )
			return m_pSkin->SizeForContent( m_rect.Size() );
		else
			return m_rect.Size();
	}

	return WgWidget::PreferredSize();
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

	if( m_pSurface && !m_rect.IsEmpty() )
	{
		WgRect dest;
		if( m_pSkin )
			dest = m_pSkin->ContentRect( _canvas, State() );
		else
			dest = _canvas;

		pDevice->ClipStretchBlit( _clip, m_pSurface, m_rect, dest );
	}
}

//____ _onAlphaTest() ___________________________________________________________

bool WgImage::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
{
	if( m_pSurface && !m_rect.IsEmpty() )
	{
		WgRect dest;
		if( m_pSkin )
			dest = m_pSkin->ContentRect( sz, State() );
		else
			dest = Size();

		if( WgUtil::MarkTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity ) )
			return true;
	}

	return WgWidget::_onAlphaTest(ofs,sz);
}

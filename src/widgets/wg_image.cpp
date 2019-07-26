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

	Image_p Image::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Image_p( static_cast<Image*>(pObject) );

		return 0;
	}

	//____ setImage() _____________________________________________________________

	void Image::setImage( Surface * pSurface, const RectI& rect )
	{
		if( pSurface != m_pSurface || rect != m_rect )
		{
			bool bResize = false;
			if( rect.w != m_rect.w || rect.h != m_rect.h )
				bResize = true;

			m_pSurface = pSurface;

			if( pSurface )
				m_rect = RectI( rect, RectI(pSurface->size()) );
			else
				m_rect.clear();

			if( bResize )
				_requestResize();
			_requestRender();
		}
	}

	void Image::setImage( Surface * pSurface )
	{
		if( pSurface != m_pSurface )
		{
			bool bResize = false;
			if( !pSurface || !m_pSurface || pSurface->size() != m_pSurface->size() )
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



	//____ _preferredSize() _____________________________________________________________

	SizeI Image::_preferredSize() const
	{
		if( m_pSurface )
		{
			if( m_pSkin )
				return m_pSkin->_sizeForContent( m_rect.size() );
			else
				return m_rect.size();
		}

		return Widget::_preferredSize();
	}

	//____ _cloneContent() _______________________________________________________

	void Image::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Image * pOrg = (Image*) _pOrg;

		m_pSurface	= pOrg->m_pSurface;
		m_rect		= pOrg->m_rect;
	}

	//____ _render() _____________________________________________________________

	void Image::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		if( m_pSurface && !m_rect.isEmpty() )
		{
			RectI dest;
			if( m_pSkin )
				dest = m_pSkin->_contentRect( _canvas, state() );
			else
				dest = _canvas;

			pDevice->setBlitSource(m_pSurface);
			pDevice->stretchBlit( dest, m_rect );
		}
	}

	//____ _alphaTest() ___________________________________________________________

	bool Image::_alphaTest( const CoordI& ofs )
	{
		if( m_pSurface && !m_rect.isEmpty() )
		{
			RectI dest;
			if( m_pSkin )
				dest = m_pSkin->_contentRect( m_size, state() );
			else
				dest = m_size;

			if( Util::markTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity ) )
				return true;
		}

		return Widget::_alphaTest(ofs);
	}

} // namespace wg

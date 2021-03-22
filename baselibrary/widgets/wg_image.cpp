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
	using namespace Util;

	const TypeInfo Image::TYPEINFO = { "Image", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Image::Image()
	{
	}

	//____ Destructor _____________________________________________________________

	Image::~Image()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Image::typeInfo(void) const
	{
		return TYPEINFO;
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

	//____ preferredSize() _____________________________________________________________

	Size Image::preferredSize() const
	{
		if( m_pSurface )
			return OO(skin)._sizeForContent( m_rect.size() );
		else
			return Widget::preferredSize();
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

	void Image::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		Widget::_render(pDevice,_canvas,_window);

		if( m_pSurface && !m_rect.isEmpty() )
		{
			Rect dest = OO(skin)._contentRect( _canvas, state() );

			pDevice->setBlitSource(m_pSurface);
			pDevice->stretchBlit( dest.px(), m_rect );
		}
	}

	//____ _alphaTest() ___________________________________________________________

	bool Image::_alphaTest( const Coord& ofs )
	{
		if( m_pSurface && !m_rect.isEmpty() )
		{
			Rect dest = OO(skin)._contentRect( m_size, state() );

			if( Util::markTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity ) )
				return true;
		}

		return Widget::_alphaTest(ofs);
	}

} // namespace wg

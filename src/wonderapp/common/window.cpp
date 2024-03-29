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

#include "window.h"

using namespace wg;


const TypeInfo Window::TYPEINFO = { "Window", &Object::TYPEINFO };

//____ constructor _____________________________________________________________

Window::Window(wg::RootPanel * pRoot, const wg::Rect& geo ) :
	m_pRootPanel(pRoot),
	m_geo(geo)
{
}

//____ typeInfo() _________________________________________________________

const wg::TypeInfo& Window::typeInfo(void) const
{
	return TYPEINFO;
}

//____ setGeo() _____________________________________________________________

wg::Rect Window::setGeo(const wg::Rect& geo)
{
	m_geo = _updateWindowGeo(geo);
	return m_geo;
}

//____ setPos() _____________________________________________________________

wg::Coord Window::setPos(wg::Coord pos)
{
	m_geo = _updateWindowGeo({ pos, m_geo.size() } );
	return m_geo.pos();
}

//____ setSize() _____________________________________________________________

wg::Size Window::setSize(wg::Size size)
{
	m_geo = _updateWindowGeo({ m_geo.pos(), size });
	return m_geo.size();
}

//____ setCloseRequestHandler() _______________________________________________

void Window::setCloseRequestHandler( std::function<bool(void)> handler )
{
	m_closeRequestHandler = handler;
}

//____ _onCloseRequest() ______________________________________________________

bool Window::_onCloseRequest()
{
	if( m_closeRequestHandler )
		return m_closeRequestHandler();
	else
		return true;
}

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

#include <wg_gizmo_dummy.h>
#include <wg_gfxdevice.h>

static const char	c_gizmoType[] = {"TordJ/Dummy"};

//____ Constructor ____________________________________________________________

WgGizmoDummy::WgGizmoDummy()
{
	m_markPolicy = WG_MARKPOLICY_TRANSPARENT;
}

//____ Destructor _____________________________________________________________

WgGizmoDummy::~WgGizmoDummy()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoDummy::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoDummy::GetMyType()
{
	return c_gizmoType;
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoDummy::OnCloneContent( const WgGizmo * _pOrg )
{
}

//____ OnRender() _____________________________________________________________

void WgGizmoDummy::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

//____ OnAlphaTest() ___________________________________________________________

bool WgGizmoDummy::OnAlphaTest( const WgCord& ofs )
{
	if( m_markPolicy == WG_MARKPOLICY_OPAQUE )
		return true;

	return false;
}

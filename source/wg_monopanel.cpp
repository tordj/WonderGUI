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

#include <wg_monopanel.h>

static const char	c_hookType[] = {"MonoHook"};


//_____________________________________________________________________________
const char * WgMonoPanel::Hook::Type( void ) const
{
	return ClassType();
}

//_____________________________________________________________________________
const char * WgMonoPanel::Hook::ClassType()
{
	return c_hookType;
}


//____ SetChild() ______________________________________________________________

WgHook * WgMonoPanel::SetChild( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return false;

	m_hook._attachGizmo(pGizmo);
	pGizmo->_onNewSize(Size());

	_requestRender();
	_requestResize();
	return &m_hook;
}

//____ DeleteChild() ___________________________________________________________

bool WgMonoPanel::DeleteChild()
{
	WgGizmo * pGizmo = m_hook._releaseGizmo();
	if( pGizmo )
	{
		delete pGizmo;
		_requestRender();
		_requestResize();
		return true;
	}

	return false;
}

//____ ReleaseChild() __________________________________________________________

WgGizmo * WgMonoPanel::ReleaseChild()
{
	WgGizmo * pGizmo = m_hook._releaseGizmo();
	if( pGizmo )
	{
		_requestRender();
		_requestResize();
	}

	return pGizmo;
}

//____ DeleteChild() ___________________________________________________________

bool WgMonoPanel::DeleteChild( WgGizmo * pGizmo )
{
	if( pGizmo == m_hook.Gizmo() )
		return DeleteChild();

	return false;
}

//____ ReleaseChild() __________________________________________________________

WgGizmo * WgMonoPanel::ReleaseChild( WgGizmo * pGizmo )
{
	if(pGizmo == m_hook.Gizmo() )
		return ReleaseChild();

	return 0;
}

//____ DeleteAllChildren() _____________________________________________________

bool WgMonoPanel::DeleteAllChildren()
{
	return DeleteChild();
}

//____ ReleaseAllChildren() ____________________________________________________

bool WgMonoPanel::ReleaseAllChildren()
{
	ReleaseChild();
	return true;
}

//____ HeightForWidth() ________________________________________________________

int WgMonoPanel::HeightForWidth( int width ) const
{
	if( m_hook.Gizmo() )
		return m_hook.Gizmo()->HeightForWidth( width );
	else
		return WgGizmo::HeightForWidth(width);
}

//____ WidthForHeight() ________________________________________________________

int WgMonoPanel::WidthForHeight( int height ) const
{
	if( m_hook.Gizmo() )
		return m_hook.Gizmo()->WidthForHeight( height );
	else
		return WgGizmo::WidthForHeight(height);
}

//____ DefaultSize() ___________________________________________________________

WgSize WgMonoPanel::DefaultSize() const
{
	if( m_hook.Gizmo() )
		return m_hook.Gizmo()->DefaultSize();
	else
		return WgSize(1,1);
}

//____ _onCollectPatches() _____________________________________________________

void WgMonoPanel::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_hook.IsVisible() && m_hook.Gizmo() )
		m_hook.Gizmo()->_onCollectPatches( container, geo, clip );
}

//____ _onMaskPatches() ________________________________________________________

void WgMonoPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( m_hook.IsVisible() && m_hook.Gizmo() )
		m_hook.Gizmo()->_onMaskPatches( patches, geo, clip, blendMode );
}

//____ _onCloneContent() _______________________________________________________

void WgMonoPanel::_onCloneContent( const WgGizmo * _pOrg )
{
}

//____ _onNewSize() ____________________________________________________________

void WgMonoPanel::_onNewSize( const WgSize& size )
{
	if( m_hook.Gizmo() )
		m_hook.Gizmo()->_onNewSize(size);
}

//____ _firstHook() ____________________________________________________________

WgHook* WgMonoPanel::_firstHook() const
{
	if( m_hook.Gizmo() )
		return const_cast<Hook*>(&m_hook);
	else
		return 0;
}

//____ _lastHook() _____________________________________________________________

WgHook* WgMonoPanel::_lastHook() const
{
	if( m_hook.Gizmo() )
		return const_cast<Hook*>(&m_hook);
	else
		return 0;
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgMonoPanel::_firstHookWithGeo( WgRect& geo ) const
{
	if( m_hook.Gizmo() )
	{
		geo = WgRect(0,0,Size());
		return const_cast<Hook*>(&m_hook);
	}

	return 0;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgMonoPanel::_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}

//____ _lastHookWithGeo() ______________________________________________________

WgHook * WgMonoPanel::_lastHookWithGeo( WgRect& geo ) const
{
	if( m_hook.Gizmo() )
	{
		geo = WgRect(0,0,Size());
		return const_cast<Hook*>(&m_hook);
	}

	return 0;
}

//_____ _prevHookWithGeo() _____________________________________________________

WgHook * WgMonoPanel::_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const
{
	return 0;
}

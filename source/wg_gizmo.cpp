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

#include <wg_gizmo.h>
#include <wg_types.h>

static const char	s_type[] = {"Unspecified"};

//____ Constructor ____________________________________________________________

WgGizmo::WgGizmo()
{
	m_id			= 0;
	m_pHook			= 0;

	m_bEnabled		= true;
	m_bOpaque		= false;

	m_bRendersAll	= false;
	m_bRenderOne	= false;

	m_cursorStyle	= WG_CURSOR_DEFAULT;

}

//____ Destructor _____________________________________________________________

WgGizmo::~WgGizmo()
{
}


//____ Type() _________________________________________________________________

const char * WgGizmo::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmo::GetMyType()
{
	return s_type;
}


//____ Enable() _______________________________________________________________

void WgGizmo::Enable()
{
	if( !m_bEnabled )
	{
		m_bEnabled = true;
		OnEnable();
	}
}

//____ Disable() ______________________________________________________________

void WgGizmo::Disable()
{
	if( m_bEnabled )
	{
		m_bEnabled = false;
		OnDisable();
	}
}

//____ CloneContent() _________________________________________________________

void WgGizmo::CloneContent( const WgGizmo * _pOrg )
{
	m_id			= _pOrg->m_id;
	m_tooltip		= _pOrg->m_tooltip;

	m_bEnabled		= _pOrg->m_bEnabled;
	m_bOpaque		= _pOrg->m_bOpaque;

	m_bRendersAll	= _pOrg->m_bRendersAll;
	m_bRenderOne	= _pOrg->m_bRenderOne;

	if( _pOrg->Type() == Type() )
		OnCloneContent( _pOrg );
}

//____ SetHook() ___________________________________________________________

void WgGizmo::SetHook( WgGizmoHook * pHook )
{
	m_pHook = pHook;
	OnNewSize( Size() );
}



//____ Local2abs() ____________________________________________________________

WgCord WgGizmo::Local2abs( const WgCord& cord ) const
{
	WgCord c = ScreenPos();
	c.x += cord.x;
	c.y += cord.y;
	return c;
}

//____ Abs2local() ____________________________________________________________

WgCord WgGizmo::Abs2local( const WgCord& cord ) const
{
	WgCord c = ScreenPos();
	return WgCord( cord.x - c.x, cord.y - c.y );
}



//____ HeightForWidth() _______________________________________________________

Uint32 WgGizmo::HeightForWidth( Uint32 width ) const
{
	return 0;		// No recommendation.
}

//____ WidthForHeight() _______________________________________________________

Uint32 WgGizmo::WidthForHeight( Uint32 height ) const
{
	return 0;		// No recommendation.
}

//____ MinSize() ______________________________________________________________

WgSize WgGizmo::MinSize() const
{
	return WgSize(1,1);
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmo::BestSize() const
{
	return WgSize(0,0);		// No recommendation.
}

//____ MaxSize() ______________________________________________________________

WgSize WgGizmo::MaxSize() const
{
	return WgSize(65535,65535);
}


//____ Fillers _______________________________________________________________

void WgGizmo::OnNewSize( const WgSize& size )
{
	RequestRender();
}

void WgGizmo::OnRefresh()
{
	RequestRender();
}

void WgGizmo::OnUpdate( const WgUpdateInfo& _updateInfo )
{
}

void WgGizmo::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

bool WgGizmo::OnMarkTest( const WgCord& ofs )
{
	return true;
}

void WgGizmo::OnEnable()
{
	RequestRender();
}

void WgGizmo::OnDisable()
{
	RequestRender();
}

void WgGizmo::OnGotInputFocus()
{
}

void WgGizmo::OnLostInputFocus()
{
}


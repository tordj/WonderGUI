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
#include <wg_skinmanager.h>

static const char	s_type[] = {"Unspecified"};

//____ Constructor ____________________________________________________________

WgGizmo::WgGizmo():m_id(0), m_pHook(0), m_pSkinNode(0), m_bEnabled(true), m_bOpaque(false),
					m_bRendersAll(false), m_bRenderOne(false), m_cursorStyle(WG_CURSOR_DEFAULT),
					m_markPolicy( WG_MARKPOLICY_ALPHA )
{
}

//____ Destructor _____________________________________________________________

WgGizmo::~WgGizmo()
{
	delete m_pSkinNode;
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
	if( !m_bEnabled || IsContainer() )
	{
		m_bEnabled = true;
		OnEnable();
	}
}

//____ Disable() ______________________________________________________________

void WgGizmo::Disable()
{
	if( m_bEnabled || IsContainer() )
	{
		m_bEnabled = false;
		OnDisable();
	}
}

//____ SetSkinManager() _______________________________________________________

void WgGizmo::SetSkinManager( WgSkinManager * pManager )
{
	if( m_pSkinNode )
		delete m_pSkinNode;

	if( pManager )
		m_pSkinNode = pManager->NewNode( this );
	else
		m_pSkinNode = 0;
}

//____ GetSkinManager() _______________________________________________________

WgSkinManager *	WgGizmo::GetSkinManager() const 
{ 
	if( m_pSkinNode )
		return m_pSkinNode->GetManager();

	return 0;
}

//____ MarkTest() _____________________________________________________________

bool WgGizmo::MarkTest( const WgCord& ofs )
{
	switch( m_markPolicy )
	{
	case WG_MARKPOLICY_ALPHA:
		return OnAlphaTest(ofs);
	case WG_MARKPOLICY_TRANSPARENT:
		return false;
	default:						//WG_MARKPOLICY_OPAQUE:
		return true;
	}
}

//____ CloneContent() _________________________________________________________

void WgGizmo::CloneContent( const WgGizmo * _pOrg )
{
	m_id			= _pOrg->m_id;
	m_tooltip		= _pOrg->m_tooltip;

	m_bEnabled		= _pOrg->m_bEnabled;
	m_bOpaque		= _pOrg->m_bOpaque;
	m_markPolicy	= _pOrg->m_markPolicy;

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

//____ SetSkinNode() __________________________________________________________

void WgGizmo::SetSkinNode( WgSkinNode * pNode )
{
	m_pSkinNode = pNode;
	OnRefresh();
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

int WgGizmo::HeightForWidth( int width ) const
{
	return 0;		// No recommendation.
}

//____ WidthForHeight() _______________________________________________________

int WgGizmo::WidthForHeight( int height ) const
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

//____ SetMarked() ____________________________________________________________

bool WgGizmo::SetMarked()
{
	return false;
}

//____ SetSelected() __________________________________________________________

bool WgGizmo::SetSelected()
{
	return false;
}

//____ SetNormal() ____________________________________________________________

bool WgGizmo::SetNormal()
{
	if( m_bEnabled )
		return true;
	else
		return false;
}

//____ Mode() _________________________________________________________________

WgMode WgGizmo::Mode() const
{
	if( m_bEnabled )
		return WG_MODE_NORMAL;
	else
		return WG_MODE_DISABLED;
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

void WgGizmo::OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

bool WgGizmo::OnAlphaTest( const WgCord& ofs )
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

bool WgGizmo::TempIsInputField() const
{
	return false;
}

Wg_Interface_TextHolder* WgGizmo::TempGetText()
{
	return 0;
}

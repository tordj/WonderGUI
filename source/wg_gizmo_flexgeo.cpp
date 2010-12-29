/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_gizmo_flexgeo.h>

static const char	c_gizmoType[] = {"TordJ/FlexGeo"};


void WgFlexHook::SetAnchored()
{
}

bool WgFlexHook::SetAnchored( int anchorTopLeft, int anchorBottomRight )
{
}

void WgFlexHook::SetFloating()
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry )
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot )
{
}

bool WgFlexHook::SetFloating( const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

void WgFlexHook::Top()
{
}

void WgFlexHook::Bottom()
{
}

bool WgFlexHook::Up()
{
}

bool WgFlexHook::Down()
{
}

bool WgFlexHook::MoveOver( WgFlexHook * pSibling )
{
}

bool WgFlexHook::MoveUnder( WgFlexHook * pSibling )
{
}

void WgFlexHook::Hide()
{
	if( !m_bHidden )
	{
		RequestRender();
		m_bHidden = true;
	}
}

void WgFlexHook::Show()
{
	if( m_bHidden )
	{
		m_bHidden = false;
		RequestRender();
	}
}

bool WgFlexHook::SetHotspot( WgLocation hotspot )
{
}

bool WgFlexHook::SetSizePolicy( WgSizePolicy policy )
{
}

bool WgFlexHook::SetMinSize( const WgSize& size )
{
}

bool WgFlexHook::SetMaxSize( const WgSize& size )
{
}

bool WgFlexHook::SetGeo( const WgRect& geometry )
{
}

bool WgFlexHook::SetOfs( const WgCord& ofs )
{
}

bool WgFlexHook::SetOfsX( int x )
{
}

bool WgFlexHook::SetOfsY( int y )
{
}

bool WgFlexHook::SetSize( const WgSize& size )
{
}

bool WgFlexHook::SetWidth( int width )
{
}

bool WgFlexHook::SetHeight( int height )
{
}

bool WgFlexHook::Move( const WgCord& ofs )
{
}

bool WgFlexHook::MoveX( int x )
{
}

bool WgFlexHook::MoveY( int y )
{
}

WgCord WgFlexHook::Pos() const
{
	return m_realGeo.pos();
}

WgSize WgFlexHook::Size() const
{
	return m_realGeo.size();
}

WgRect WgFlexHook::Geo() const
{
	return m_realGeo;
}

WgCord WgFlexHook::ScreenPos() const
{
}

WgRect WgFlexHook::ScreenGeo() const
{
}

WgGizmoHook * WgFlexHook::PrevHook() const
{
}

WgGizmoHook * WgFlexHook::NextHook() const
{
}

WgGizmoContainer* WgFlexHook::Parent() const
{
}

WgWidget* WgFlexHook::GetRoot()
{
}

WgFlexHook::WgFlexHook( WgGizmo * pGizmo, WgGizmoFlexGeo * pParent )
{
}

void WgFlexHook::RefreshRealGeo()
{
}

void WgFlexHook::RequestRender()
{
}

void WgFlexHook::RequestRender( const WgRect& rect )
{
}

void WgFlexHook::RequestResize()
{
}

//____ () _________________________________________________

WgGizmoFlexGeo::WgGizmoFlexGeo() : m_bClipChildren(false), m_bConfineChildren(false)
{
}

//____ () _________________________________________________

WgGizmoFlexGeo::~WgGizmoFlexGeo()
{
	DeleteAllGizmos();
	DeleteAllAnchors();
}

//____ () _________________________________________________

const char * WgGizmoFlexGeo::Type( void ) const
{
	return GetMyType();
}

//____ () _________________________________________________

const char * WgGizmoFlexGeo::GetMyType()
{
	return c_gizmoType;
}

//____ () _________________________________________________

void WgGizmoFlexGeo::SetClipChildren( bool bClipChildren )
{
	if( bClipChildren != m_bClipChildren )
	{
		m_bClipChildren = bClipChildren;

		// Request render on portion of children (and grandchildren inside bounding box) outside our geometry.

		if( !m_bConfineChildren )
		{
			WgFlexHook * p = m_hooks.First();
			while( p )
			{
				Continue here!!!

				p = p->NextHook();
			}
		}

	}

}

//____ () _________________________________________________

void WgGizmoFlexGeo::SetConfineChildren( bool bConfineChildren )
{
	if( bConfineChildren != m_bConfineChildren )
	{
		m_bConfineChildren = bConfineChildren;

		BoundingBoxChanged();

		// Update geo on children and render if changed.
		// Clip rendering if m_bClipChildren!

		Continue here!!!
	}
}



//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::AddGizmo( WgGizmo * pGizmo, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, int anchorTopLeft, int anchorBottomRight, WgBorders borders = 0 )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot, int anchor )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling, const WgRect& geometry, WgLocation hotspot = WG_NORTHWEST )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::DeleteGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::ReleaseGizmo( WgGizmo * pGizmo )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::DeleteAllGizmos()
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::ReleaseAllGizmos()
{
}

//____ () _________________________________________________

int WgGizmoFlexGeo::AddAnchor( float relativeX, float relativeY, const WgCord& pixelOfs )
{
}

//____ () _________________________________________________

int WgGizmoFlexGeo::ReplaceAnchor( int index, float relativeX, float relativeY, const WgCord& pixelOfs )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::DeleteAnchor( int index )
{
}

//____ () _________________________________________________

inline int WgGizmoFlexGeo::NbAnchors() const { return m_anchors.size()+9; 
{
}

//____ () _________________________________________________

WgFlexAnchor * WgGizmoFlexGeo::Anchor( int index )
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::FirstHook() const
{
}

//____ () _________________________________________________

WgFlexHook * WgGizmoFlexGeo::LastHook() const
{
}

//____ () _________________________________________________

WgRect WgGizmoFlexGeo::BoundingBoxForSize( WgSize size ) const
{
	switch( m_childGeoPolicy )
	{
		case PARENT_CLIP:
		case PARENT_LIMIT:
			return WgRect( Pos(), size );

		case UNLIMITED:
		{

		}
	}
}

//____ () _________________________________________________

int WgGizmoFlexGeo::HeightForWidth( int width ) const
{
}

//____ () _________________________________________________

int WgGizmoFlexGeo::WidthForHeight( int height ) const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::MinSize() const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::BestSize() const
{
}

//____ () _________________________________________________

WgSize WgGizmoFlexGeo::MaxSize() const
{
}

//____ () _________________________________________________

WgGizmo * WgGizmoFlexGeo::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnCloneContent( const WgGizmo * _pOrg )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnNewSize( const WgSize& size )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}

//____ () _________________________________________________

bool WgGizmoFlexGeo::OnAlphaTest( const WgCord& ofs )
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnEnable()
{
}

//____ () _________________________________________________

void WgGizmoFlexGeo::OnDisable()
{
}


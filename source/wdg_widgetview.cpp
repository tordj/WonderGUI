/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@swipnet.se].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@swipnet.se] for details.

=========================================================================*/

#include	<wdg_widgetview.h>
#include	<assert.h>

static const char	Wdg_Type[] = {"TordJ/WidgetView"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_WidgetView::NewOfMyType() const
{
	return new Wdg_WidgetView;
}


//____ Init() _________________________________________________________________

void Wdg_WidgetView::Init( void )
{
	m_pViewContent = 0;
	m_pLastMarkedWidget = 0;

	for( int i = 0 ; i < 10 ; i++ )
		m_pPressWidget[i] = 0;


}

//____ Destructor _____________________________________________________________

Wdg_WidgetView::~Wdg_WidgetView( void )
{
}


//____ Type() _________________________________________________________________

const char * Wdg_WidgetView::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_WidgetView::GetMyType( void )
{
	return Wdg_Type;
}



//____ SetContent() ___________________________________________________________

void Wdg_WidgetView::SetContent( WgWidget * pContent )
{
	assert(pContent);

	SetContentSize( pContent->Width(), pContent->Height() );

	m_pViewContent		= pContent;
	m_pLastMarkedWidget	= 0;


	m_pViewContent->SetGeometry( WgOrigo::topLeft(), WgRect( 0,0, pContent->Width(), pContent->Height() ) );
	RequestRender();
}


//____ DoMyOwnUpdate() ________________________________________________________

void Wdg_WidgetView::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{

	if( m_pViewContent )
	{
		m_pViewContent->UpdateX( _updateInfo );

		TransferDirtRecursively( m_pViewContent, &m_dirtyRects, -(int)ViewPixelOfsX(), -(int)ViewPixelOfsY() );

//		m_pViewContent->CollectDirtyRectangles( &m_dirtyRects );

	}
}


void Wdg_WidgetView::TransferDirtRecursively( WgWidget * pWidget, WgDirtyRectObj * pDest, int ofsx, int ofsy )
{
	ofsx += pWidget->m_geo.x;
	ofsy += pWidget->m_geo.y;

	WgDirtyRect	* pDirt = pWidget->m_dirtyRects.pRectList;
	while( pDirt )
	{
		pDirt->x += ofsx;
		pDirt->y += ofsy;
		pDirt = pDirt->pNext;
	}

	pWidget->m_dirtyRects.Transfer( pDest );


	pWidget = pWidget->FirstChild();
	while( pWidget )
	{
		TransferDirtRecursively( pWidget, pDest, ofsx, ofsy );
		pWidget = pWidget->NextSibling();
	}
}



//____ DoMyOwnMarkTest() ______________________________________________________

bool Wdg_WidgetView::DoMyOwnMarkTest( int _x, int _y )
{
	return true;		// We are always opaque.
}

//____ DoMyOwnActionRespond() _________________________________________________

void Wdg_WidgetView::DoMyOwnActionRespond( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	int x = _info.x;
	int y = _info.y;

	Abs2local( &x, &y );

	x += ViewPixelOfsX();
	y += ViewPixelOfsY();

	WgActionDetails actionDetails(_info);
	actionDetails.x = x;
	actionDetails.y = y;

	if( _action == WgInput::BUTTON_DOWN && m_pPressWidget[_button_key] != 0 )
	{
		m_pPressWidget[_button_key]->ActionRespond( _action, _button_key, actionDetails, _inputObj );
		return;
	}


	if( _action == WgInput::POINTER_EXIT && m_pLastMarkedWidget != 0 )
	{
 		m_pLastMarkedWidget->ActionRespond( _action, _button_key, actionDetails, _inputObj );
		m_pLastMarkedWidget = 0;
		return;
	}

	assert(m_pViewContent);
	WgWidget * pWidget = m_pViewContent->FindOccupant( x, y, true );	// Return widget (me, child or 0) occupying position


	if( pWidget != m_pLastMarkedWidget && m_pLastMarkedWidget != 0 )
	{
		m_pLastMarkedWidget->ActionRespond( WgInput::POINTER_EXIT, _button_key, actionDetails, _inputObj );
		m_pLastMarkedWidget = 0;
	}

	if( pWidget != m_pViewContent && _action == WgInput::BUTTON_DOWN )
		y = y;

	if( m_pPressWidget[_button_key] != 0 && (_action == WgInput::BUTTON_RELEASE || _action == WgInput::BUTTON_RELEASE_OUTSIDE ) )
	{
		if( pWidget != m_pPressWidget[_button_key] )
			m_pPressWidget[_button_key]->ActionRespond( WgInput::BUTTON_RELEASE_OUTSIDE, _button_key, actionDetails, _inputObj );

		m_pPressWidget[_button_key] = 0;
	}

	if( pWidget )
	{
		// Make sure item gets a POINTER_ENTER.

		if( _action == WgInput::POINTER_OVER && !m_pLastMarkedWidget )
			pWidget->ActionRespond( WgInput::POINTER_ENTER, _button_key, actionDetails, _inputObj );

		//

		if( _action == WgInput::BUTTON_PRESS )
			m_pPressWidget[_button_key] = pWidget;


		pWidget->ActionRespond( _action, _button_key, actionDetails, _inputObj );
		m_pLastMarkedWidget = pWidget;
	}
}


//____ DoMyOwnRender() ________________________________________________________

void Wdg_WidgetView::DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgRect window;

	window.x = _window.x - ViewPixelOfsX();
	window.y = _window.y - ViewPixelOfsY();
	window.w = ContentWidth();
	window.h = ContentHeight();

	RenderRecursively( m_pViewContent, window, _clip, _layer );
}

//____ RenderRecursively() ____________________________________________________

void Wdg_WidgetView::RenderRecursively( WgWidget * pWidget, const WgRect& parentGeo, const WgRect& clip, Uint8 _layer )
{
	WgRect window = pWidget->Geometry();

	window.x += parentGeo.x;
	window.y += parentGeo.y;

	WgRect myClip;
	if( !myClip.Intersection( window, clip ) )
		return;

	pWidget->DoMyOwnRender( window, myClip, _layer );
	pWidget->m_bRenderedHere = true;

	WgWidget * pChild = pWidget->FirstChild();
	while( pChild )
	{
		RenderRecursively( pChild, window, myClip, _layer );
		pChild = pChild->NextSibling();
	}
}


//____ DoMyOwnRefresh() _______________________________________________________

void Wdg_WidgetView::DoMyOwnRefresh( void )
{
	// Not implemented yet.
}

//____ DoMyOwnCloning() _______________________________________________________

void Wdg_WidgetView::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
	// Not implemented yet.
}

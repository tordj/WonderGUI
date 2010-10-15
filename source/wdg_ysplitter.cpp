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

#include	<wdg_ysplitter.h>

static const char	Wdg_Type[] = {"TordJ/YSplitter"};


//_____________________________________________________________________________
WgWidget * Wdg_YSplitter::NewOfMyType() const
{
	return new Wdg_YSplitter;
}



//_____________________________________________________________________________
void Wdg_YSplitter::Init( void )
{
	m_pTopPane		= 0;
	m_pBottomPane	= 0;
	m_pHandle		= 0;
}


//_____________________________________________________________________________
Wdg_YSplitter::~Wdg_YSplitter( void )
{
	Stop();
}


//_____________________________________________________________________________
const char * Wdg_YSplitter::Type( void ) const
{
	return GetMyType();
}

//_____________________________________________________________________________
const char * Wdg_YSplitter::GetMyType( void )
{
	return Wdg_Type;
}


//_____________________________________________________________________________
void Wdg_YSplitter::Start( WgWidget * pTopPane, WgWidget * pSplitHandle, WgWidget * pBottomPane, float topFraction )
{
	m_pTopPane = pTopPane;
	m_pBottomPane = pBottomPane;
	m_pHandle = pSplitHandle;
	m_topFraction = topFraction;

	m_pHandle->AddCallbackW( WgSignal::ButtonPress(1), cbHandlePressed, this );
	m_pHandle->AddCallbackW( WgSignal::ButtonDragTotalY(1), cbHandleDragged, this );


/*	m_pHandle->SetGeometry( WgOrigo::topLeft(), 0, 0, WgOrigo::topRight(), 0, m_pHandle->Height() );
	m_pHandle->SetParent(this);

	m_pTopPane->SetGeometry( WgOrigo::topLeft(), 0,0, WgOrigo::topRight(), 0, m_pTopPane->MinHeight() );
	m_pTopPane->SetParent(this);

	m_pBottomPane->SetGeometry( WgOrigo::bottomLeft(), 0,0, WgOrigo::bottomRight(), 0, -m_pTopPane->MinHeight() );
	m_pBottomPane->SetParent(this);
*/
	UpdatePanes();
}


//_____________________________________________________________________________
void Wdg_YSplitter::Stop()
{
	if( m_pHandle )
		m_pHandle->RemoveCallbacksW( this );

	m_pTopPane		= 0;
	m_pBottomPane	= 0;
	m_pHandle		= 0;
}

//_____________________________________________________________________________
void Wdg_YSplitter::UpdatePanes()
{
	if( m_topFraction == 0.f )
		BottomPaneOnly();
	else if( m_topFraction == 1.f )
		TopPaneOnly();
	else if( m_pHandle->Height() + m_pTopPane->MinHeight() + m_pBottomPane->MinHeight() > Height() )
	{
		if( m_topFraction > 0.5f )
			TopPaneOnly();
		else
			BottomPaneOnly();
	}
	else
	{
		int topSize = (int) ((Height() - m_pHandle->Height())*m_topFraction);
		int botSize = Height() - topSize - m_pHandle->Height();

		int corr = 0;
		if( topSize < m_pTopPane->MinHeight() )
			corr = m_pTopPane->MinHeight() - topSize;
		else if( botSize < m_pBottomPane->MinHeight() )
			corr = -(m_pBottomPane->MinHeight() - botSize);

		topSize += corr;
		botSize -= corr;

		m_pTopPane->SetGeometry(WgOrigo::topLeft(), 0,0, WgOrigo::topRight(), 0, topSize );
		m_pHandle->SetGeometry( WgOrigo::topLeft(), 0, topSize, WgOrigo::topRight(), 0, topSize + m_pHandle->Height() );
		m_pBottomPane->SetGeometry( WgOrigo::bottomLeft(), 0, -botSize, WgOrigo::bottomRight(), 0, 0 );

		m_pTopPane->SetParent(this);
		m_pBottomPane->SetParent(this);
		m_pHandle->SetParent(this);
	}
}

//_____________________________________________________________________________
void Wdg_YSplitter::TopPaneOnly()
{
	m_pTopPane->SetGeometry( WgOrigo::topLeft(), 0,0, WgOrigo::bottomRight(), 0, -m_pHandle->Height() );
	m_pHandle->SetGeometry( WgOrigo::bottomLeft(), 0, -m_pHandle->Height(), WgOrigo::bottomRight(), 0, 0 );
	m_pBottomPane->SetParent(0);
}

//_____________________________________________________________________________
void Wdg_YSplitter::BottomPaneOnly()
{
	m_pTopPane->SetParent(0);
	m_pHandle->SetGeometry( WgOrigo::topLeft(), 0, 0, WgOrigo::topRight(), 0, m_pHandle->Height() );
	m_pBottomPane->SetGeometry( WgOrigo::topLeft(), 0, m_pHandle->Height(), WgOrigo::bottomRight(), 0, 0 );
}


//_____________________________________________________________________________
void Wdg_YSplitter::OnHandlePressed()
{
	m_pressPos = m_pHandle->Geometry().y;
}

//_____________________________________________________________________________
void Wdg_YSplitter::OnHandleDragged( int ofs )
{
	int pos = m_pressPos+ofs;

	if( pos < m_pTopPane->MinHeight() / 2 )
		m_topFraction = 0.f;
	else if( pos > Height() - m_pBottomPane->MinHeight() / 2  - m_pHandle->Height() )
		m_topFraction = 1.f;
	else
		m_topFraction = pos / (float) (Height()-m_pHandle->Height());

	UpdatePanes();
}

//_____________________________________________________________________________
void Wdg_YSplitter::DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
{
	if( oldGeo.h != newGeo.h )
		UpdatePanes();
}

//_____________________________________________________________________________
bool Wdg_YSplitter::DoMyOwnMarkTest( int _x, int _y )
{
	return false;		// We never have opaque pixels.
}

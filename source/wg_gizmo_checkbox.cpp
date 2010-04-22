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

#include <wg_gizmo_checkbox.h>
#include <wg_surface.h>
#include <wg_gfxdevice.h>
#include <wg_font.h>
#include <wg_util.h>
#include <assert.h>

using namespace WgSignal;

static const char	Wdg_Type[] = {"TordJ/CheckBox2"};



//____ WgGizmoCheckbox() _________________________________________________________________

WgGizmoCheckbox::WgGizmoCheckbox()
{
	m_bChecked			= false;
	m_bOver				= false;
	m_bPressed			= false;

	m_pText				= &m_text;
	m_text.setHolder( this );

	m_textMouseOverOfsX = 0xFFFF;
	m_pTextArea			= 0;
	m_textAreaCount		= 0;

	m_bFixedSizeBox		= false;

	m_aDisplace[0].x	= m_aDisplace[0].y = 0;
	m_aDisplace[1]		= m_aDisplace[2] = m_aDisplace[0];
}

//____ Destructor _____________________________________________________________

WgGizmoCheckbox::~WgGizmoCheckbox()
{
	if( m_pTextArea )
		delete [] m_pTextArea;
}


//____ Type() _________________________________________________________________

const char * WgGizmoCheckbox::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoCheckbox::GetMyType( void )
{
	return Wdg_Type;
}


//____ SetSource() ____________________________________________________________

bool WgGizmoCheckbox::SetSource( const WgBlockSetPtr& _pUnchecked, const WgBlockSetPtr& _pChecked, bool bFixedSizeBox )
{
	m_pBlockUnchecked	= _pUnchecked;
	m_pBlockChecked		= _pChecked;
	m_bFixedSizeBox		= bFixedSizeBox;

	OnRefresh();
	return true;
}

//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgGizmoCheckbox::GetTextAreaWidth()
{ 
	int w = Size().w;

	WgBlockSetPtr p = m_bChecked ? m_pBlockChecked : m_pBlockUnchecked;

	if( p )
		w -= p->GetContentBorders().GetWidth(); 

	return w;
}

//____ SetFixedSize() _________________________________________________________

bool WgGizmoCheckbox::SetFixedSize(bool bFixedSizeBox)
{
	m_bFixedSizeBox = bFixedSizeBox;

	RequestRender();
	return true;
}

//____ GetLineWidth() _________________________________________________________

int WgGizmoCheckbox::GetLineWidth() const
{
	return m_text.getLineWidth();
}

//____ SetDisplacement() ______________________________________________________

bool WgGizmoCheckbox::SetDisplacement( Sint8 _xUnchecked, Sint8 _yUnchecked, Sint8 _xOver, Sint8 _yOver, Sint8 _xChecked, Sint8 _yChecked )
{
  m_aDisplace[0].x = _xUnchecked;
  m_aDisplace[0].y = _yUnchecked;

  m_aDisplace[1].x = _xOver;
  m_aDisplace[1].y = _yOver;

  m_aDisplace[2].x = _xChecked;
  m_aDisplace[2].y = _yChecked;

  RequestRender();
  return  true;
}

void WgGizmoCheckbox::GetDisplacement( Sint8& xUp, Sint8& yUp, Sint8& xOver, Sint8& yOver, Sint8& xDown, Sint8& yDown ) const
{
	xUp = m_aDisplace[0].x;
	yUp = m_aDisplace[0].y;

	xOver = m_aDisplace[1].x;
	yOver = m_aDisplace[1].y;

	xDown = m_aDisplace[2].x;
	yDown = m_aDisplace[2].y;
}

//____ SetState() _____________________________________________________________

bool WgGizmoCheckbox::SetState( bool _state )
{
	if( m_bChecked != _state )
	{
		m_bChecked = _state;

		if( _state )
		{
			if( m_pHook )
				m_pHook->GetEmitter()->Emit( Set() );
		}
		else
		{
			if( m_pHook )
				m_pHook->GetEmitter()->Emit( Unset() );
		}
		if( m_pHook )
			m_pHook->GetEmitter()->Emit( Flipped(), m_bChecked );
		RequestRender();
	}

	return true;
}


//____ SetTextMouseOverOfsX() _________________________________________________
void WgGizmoCheckbox::SetTextMouseOverOfsX( Uint16 ofs )
{
	m_textMouseOverOfsX = ofs;
	RefreshTextArea();
}

//____ OnEnable() _________________________________________________
void WgGizmoCheckbox::OnEnable()
{
	RequestRender();
}

//____ OnDisable() _________________________________________________
void WgGizmoCheckbox::OnDisable()
{
	m_bOver = false;
	m_bPressed = false;

	RequestRender();	
}


//____ OnAction() _________________________________________________

void WgGizmoCheckbox::OnAction( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	switch( _action )
	{
		case WgInput::POINTER_ENTER:
			if( !m_bOver )
			{
				m_bOver = true;
				RequestRender();
			}
			break;

		case WgInput::POINTER_EXIT:
			if( m_bOver )
			{
				m_bOver = false;
				RequestRender();
			}
			break;

		case WgInput::BUTTON_PRESS:
			if( _button_key == 1 && !m_bPressed )
			{
				m_bPressed = true;
				RequestRender();
			}
			break;

		case WgInput::BUTTON_RELEASE:
		case WgInput::BUTTON_RELEASE_OUTSIDE:
			if( _button_key == 1 && m_bPressed )
			{
				m_bPressed = false;
				RequestRender();
			}
			break;

		case WgInput::BUTTON_CLICK:
			if( _button_key == 1 )
				SetState( !m_bChecked );
			break;

        default:
            break;

	}
}

//____ OnRender() ________________________________________________________

void WgGizmoCheckbox::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	WgMode	mode = WG_MODE_NORMAL;
	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;
	else if( m_bOver )
	{
		mode = WG_MODE_MARKED;
		if( m_bPressed )
			mode = WG_MODE_SELECTED;
	}

	WgBlockSetPtr	pBlockSet = 0;
	if( m_bChecked )
		pBlockSet = m_pBlockChecked;
	else
		pBlockSet = m_pBlockUnchecked;

	if( pBlockSet )
	{
		if( m_bFixedSizeBox )
			pDevice->ClipBlitBlock( _clip, pBlockSet->GetBlock(mode), WgRect(_window.x, _window.y, pBlockSet->GetWidth(), pBlockSet->GetHeight()) );
		else
			pDevice->ClipBlitBlock( _clip, pBlockSet->GetBlock(mode), _window );
	}

 	if( m_text.nbLines()!= 0 )
	{
		int	iDisplacement = 0;
		if( (m_bPressed && m_bOver) || m_bChecked )
            iDisplacement = 2;
		else if( m_bOver )
			iDisplacement = 1;

		int   xOfs = m_aDisplace[iDisplacement].x;
		int   yOfs = m_aDisplace[iDisplacement].y;



		WgRect	printWindow = _window;

		if( pBlockSet )
			printWindow.Shrink( pBlockSet->GetContentBorders() );
		printWindow.x += xOfs;
		printWindow.y += yOfs;
		m_pText->setMode( mode );
		pDevice->PrintText( _clip, m_pText, printWindow );
	}
}

//____ OnRefresh() _______________________________________________________

void WgGizmoCheckbox::OnRefresh( void )
{
	if( m_pBlockChecked && m_pBlockUnchecked && m_pBlockChecked->IsOpaque() &&
		m_pBlockUnchecked->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	RequestRender();
	RefreshTextArea();
}

//____ OnNewSize() _______________________________________________________

void WgGizmoCheckbox::OnNewSize( const WgSize& size )
{
	Uint32 w = size.w;

	WgBlockSetPtr p = m_bChecked ? m_pBlockChecked : m_pBlockUnchecked;

	if( p )
		w -= p->GetContentBorders().GetWidth();

	m_text.setLineWidth(w);
	RefreshTextArea();
}


//____ OnCloneContent() _______________________________________________________

void WgGizmoCheckbox::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoCheckbox * pOrg = (WgGizmoCheckbox *) _pOrg;

	m_bChecked			= pOrg->m_bChecked;
	m_bOver				= false;
	m_bPressed			= false;

	m_pBlockUnchecked	= pOrg->m_pBlockUnchecked;
	m_pBlockChecked		= pOrg->m_pBlockChecked;
	m_bFixedSizeBox		= pOrg->m_bFixedSizeBox;

	m_text				= pOrg->m_text;

	m_textMouseOverOfsX = pOrg->m_textMouseOverOfsX;


	for( int i = 0 ; i < 3 ; i++ )
	{
		m_aDisplace[i]	= pOrg->m_aDisplace[i];
	}

	Wg_Interface_TextHolder::OnCloneContent( pOrg );
}

//____ TextModified() _________________________________________________________

void WgGizmoCheckbox::TextModified()
{
	RefreshTextArea();
	RequestRender();
}


//____ RefreshTextArea() ______________________________________________________

void WgGizmoCheckbox::RefreshTextArea()
{
	if( m_pTextArea )
	{
		delete [] m_pTextArea;
		m_pTextArea = 0;
	}
	m_textAreaCount = 0;

	// calculate mark testing area for the text

	WgTextPropPtr	pProp = m_text.getDefaultProperties();

	const WgFont * pFontSet = pProp->GetFont();
	if( !pFontSet )
		return;
	const WgOrigo& origo	= m_pText->alignment();

	int textheight = m_pText->height();

	int yPos = (int) (Size().h * origo.anchorY()
							- textheight * origo.hotspotY());

	Uint32				n1 = m_pText->nbLines();
	const WgTextLine *	p1 = m_pText->getLines();

	m_textAreaCount = n1;
	m_pTextArea = new WgRect[ m_textAreaCount ];

	int textOfs = 0;
	WgBlockSetPtr	p = m_bChecked ? m_pBlockChecked : m_pBlockUnchecked;
	if( p )
		textOfs = p->GetContentBorders().left;

	for( int i = 0 ; i < (int) n1 ; i++ )
	{
		// if text mouse over x offset is specified, text mark area starts there, otherwise at the regular text offset
		int xMin = textOfs;
		if( m_textMouseOverOfsX != 0xFFFF )
			xMin = m_textMouseOverOfsX;

		int linewidth = WgTextTool::lineWidth( pProp, WG_MODE_NORMAL, p1[i].pText );
		int lineheight = m_pText->softLineHeight(i);
		int textStartX = (int) (textOfs + Size().w * origo.anchorX()
								- linewidth * origo.hotspotX());

		int xMax = textStartX + linewidth;

		m_pTextArea[ i ] = WgRect( xMin, yPos, xMax - xMin, lineheight );

		yPos += lineheight;
	}
}

//____ MarkTestTextArea() ______________________________________________________

bool WgGizmoCheckbox::MarkTestTextArea( int _x, int _y )
{
	if( m_textAreaCount != m_pText->nbLines() )
		RefreshTextArea();
	for( int i = 0; i < m_textAreaCount; ++i )
	{
		const WgRect& kArea = m_pTextArea[ i ];
		if( _x >= kArea.x && _x < kArea.x + kArea.w
			&& _y >= kArea.y && _y < kArea.y + kArea.h )
			return true;
	}
	return false;
}

//____ OnMarkTest() ______________________________________________________

bool WgGizmoCheckbox::OnMarkTest( const WgCord& ofs )
{
	// mark test text area
	if( MarkTestTextArea( ofs.x, ofs.y ) )
		return true;


	WgMode mode = WG_MODE_NORMAL;

	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;
	else if( m_bOver )
	{
		mode = WG_MODE_MARKED;
		if( m_bPressed )
			mode = WG_MODE_SELECTED;
	}

	WgBlock block;
	if( m_bChecked && m_pBlockChecked )
		block = m_pBlockChecked->GetBlock(mode);
	else if( m_pBlockUnchecked )
		block = m_pBlockUnchecked->GetBlock(mode);
	else
		return false;

	WgRect a;

	if( m_bFixedSizeBox )
		a = WgRect(0,0,block.GetWidth(),block.GetHeight());
	else
	{
		WgSize sz = Size();
		a = WgRect(0,0, sz.w, sz.h);
	}

	return WgUtil::MarkTestBlock( ofs.x, ofs.y, block, a );
}

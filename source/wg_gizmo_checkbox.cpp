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

static const char	c_gizmoType[] = {"TordJ/CheckBox2"};



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


	m_iconAreaBorders	= WgBorders(0);
	m_iconScale			= 0.f;
	m_bIconPushText		= true;


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
	return c_gizmoType;
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

//____ SetIcon() ______________________________________________________________

void WgGizmoCheckbox::SetIcon( const WgBlockSetPtr& _pUnchecked, const WgBlockSetPtr& _pChecked,
								WgBorders _areaBorders, const WgOrigo& _origo, float _scale,
								bool _bPushText )
{
	m_pIconUnchecked	= _pUnchecked;
	m_pIconChecked		= _pChecked;
	m_iconOrigo			= _origo;
	m_iconScale			= _scale;
	m_bIconPushText		= _bPushText;
	m_iconAreaBorders	= _areaBorders;

	OnRefresh();
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

//____ GetTextAreaWidth() _____________________________________________________

Uint32 WgGizmoCheckbox::GetTextAreaWidth()
{
	WgSize gizmoSize = Size();

	return GetContentRect( gizmoSize, GetIconRect( gizmoSize ) ).w;
}


//____ GetIconRect() __________________________________________________________

/*
	Gets an icon-rect for the icon including borders, relative to upper left corner of widget.

*/

WgRect WgGizmoCheckbox::GetIconRect( const WgSize& gizmoSize )
{
	WgRect rect;

	WgBlockSetPtr	pIcon;

	if( m_bChecked )
		pIcon		= m_pIconChecked;
	else
		pIcon		= m_pIconUnchecked;

	if( pIcon )
	{
		int w = pIcon->GetWidth();
		int h = pIcon->GetHeight();

		int bgW = gizmoSize.w - m_iconAreaBorders.GetWidth();
		int bgH = gizmoSize.h - m_iconAreaBorders.GetHeight();

		if( m_iconScale != 0.f )
		{
			if( (w / (float) bgW) > (h / (float) bgH) )
			{
				h = (int) ((h * bgW * m_iconScale) / h);
				w = (int) (bgW * m_iconScale);
			}
			else
			{
				w = (int) ((w * bgH * m_iconScale) / h);
				h = (int) (bgH * m_iconScale);
			}
		}

		rect.x = m_iconOrigo.calcOfsX( bgW, w );
		rect.y = m_iconOrigo.calcOfsY( bgH, h );
		rect.w = w + m_iconAreaBorders.GetWidth();
		rect.h = h + m_iconAreaBorders.GetHeight();;
	}
	else
	{
		rect.x = 0;
		rect.y = 0;
		rect.w = 0;
		rect.h = 0;
	}

	return rect;
}


//____ GetContentRect() _____________________________________________________

WgRect WgGizmoCheckbox::GetContentRect( const WgSize& gizmoSize, const WgRect& iconRect )
{
	WgRect rect( 0,0, gizmoSize.w, gizmoSize.h );

	WgBlockSetPtr p = m_bChecked ? m_pBlockChecked : m_pBlockUnchecked;

	if( p )
		rect.Shrink( p->GetContentBorders() );

	if( m_bIconPushText )
	{
		int	iconX1 = iconRect.x;
		int iconX2 = iconRect.x + iconRect.w;

		int contentX1 = rect.x;
		int contentX2 = rect.x + rect.w;

		int spaceRightOfIcon = contentX2 - iconX2;
		int spaceLeftOfIcon = iconX1 - contentX1;

		if( spaceRightOfIcon <= 0 && spaceLeftOfIcon <= 0 )
			return WgRect(0,0,0,0);

		if( spaceLeftOfIcon > spaceRightOfIcon )
		{
			rect.w = contentX1 - iconX1;
		}
		else
		{
			rect.x = iconX2;
			rect.w = contentX2 - iconX2;
		}
	}

	return rect;
}



//____ OnRender() ________________________________________________________

void WgGizmoCheckbox::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Get correct mode

	WgMode	mode = WG_MODE_NORMAL;
	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;
	else if( m_bOver )
	{
		mode = WG_MODE_MARKED;
		if( m_bPressed )
			mode = WG_MODE_SELECTED;
	}

	// Get blockset pointers for background and icon

	WgBlockSetPtr	pBlockSet = 0;
	WgBlockSetPtr	pIcon = 0;
	if( m_bChecked )
	{
		pBlockSet	= m_pBlockChecked;
		pIcon		= m_pIconChecked;
	}
	else
	{
		pBlockSet	= m_pBlockUnchecked;
		pIcon		= m_pIconUnchecked;
	}

	// Blit background

	if( pBlockSet )
	{
		if( m_bFixedSizeBox )
			pDevice->ClipBlitBlock( _clip, pBlockSet->GetBlock(mode), WgRect(_canvas.x, _canvas.y, pBlockSet->GetWidth(), pBlockSet->GetHeight()) );
		else
			pDevice->ClipBlitBlock( _clip, pBlockSet->GetBlock(mode), _canvas );
	}

	// Blit icon

	WgRect iconRect = GetIconRect( _canvas );

	if( pIcon && iconRect.w > 0 && iconRect.h > 0 )
	{
		WgRect iconBlitRect = iconRect;
		iconBlitRect.Shrink( m_iconAreaBorders );
		iconBlitRect.x += _canvas.x;
		iconBlitRect.y += _canvas.y;
		pDevice->ClipBlitBlock( _clip, pIcon->GetBlock(mode), iconBlitRect );
	}

	// Print content

 	if( m_text.nbLines()!= 0 )
	{
		int	iDisplacement = 0;
		if( (m_bPressed && m_bOver) || m_bChecked )
            iDisplacement = 2;
		else if( m_bOver )
			iDisplacement = 1;

		int   xOfs = m_aDisplace[iDisplacement].x;
		int   yOfs = m_aDisplace[iDisplacement].y;


		WgRect	printWindow = GetContentRect( _canvas, iconRect );
		printWindow.x += _canvas.x;
		printWindow.y += _canvas.y;

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
	m_text.setLineWidth( GetTextAreaWidth() );
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

	m_pIconUnchecked	= pOrg->m_pIconUnchecked;
	m_pIconChecked		= pOrg->m_pIconChecked;
	m_iconOrigo			= pOrg->m_iconOrigo;
	m_iconScale			= pOrg->m_iconScale;
	m_iconAreaBorders	= pOrg->m_iconAreaBorders;
	m_bIconPushText		= pOrg->m_bIconPushText;

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

	WgRect	contentRect = GetContentRect( Size(), GetIconRect( Size() ) );



	// calculate mark testing area for the text

	WgTextPropPtr	pProp = m_text.getProperties();

	const WgFont * pFontSet = pProp->GetFont();
	if( !pFontSet )
		return;
	const WgOrigo& origo	= m_pText->alignment();

	int textheight = m_pText->height();

	int yPos = (int) (Size().h * origo.anchorY()
							- textheight * origo.hotspotY());

	Uint32				nLines = m_pText->nbLines();

	m_textAreaCount = nLines;
	m_pTextArea = new WgRect[ m_textAreaCount ];

	int	textOfs = contentRect.x;

	for( int i = 0 ; i < (int) nLines ; i++ )
	{
		// if text mouse over x offset is specified, text mark area starts there, otherwise at the regular text offset
		int xMin = textOfs;
		if( m_textMouseOverOfsX != 0xFFFF )
			xMin = m_textMouseOverOfsX;

		int linewidth = m_pText->getLineWidth(i);
		int lineheight = m_pText->softLineSpacing(i);
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
//	if( m_textAreaCount != m_pText->nbLines() )
//		RefreshTextArea();

	for( int i = 0; i < m_textAreaCount; ++i )
	{
		if( m_pTextArea[i].Contains(_x,_y) )
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

	WgBlock bgBlock;
	WgBlock iconBlock;

	if( m_bChecked )
	{
		if( m_pBlockChecked )
			bgBlock = m_pBlockChecked->GetBlock(mode);

		if( m_pIconChecked )
			iconBlock = m_pIconChecked->GetBlock(mode);
	}
	else
	{
		if( m_pBlockUnchecked )
			bgBlock = m_pBlockUnchecked->GetBlock(mode);

		if( m_pIconUnchecked )
			iconBlock = m_pIconUnchecked->GetBlock(mode);
	}

	WgRect bgRect;
	WgSize sz = Size();

	if( bgBlock.IsValid() )
	{
		if( m_bFixedSizeBox )
			bgRect = WgRect(0,0,bgBlock.GetWidth(), bgBlock.GetHeight() );
		else
			bgRect = WgRect(0,0, sz.w, sz.h);

		if( WgUtil::MarkTestBlock( ofs.x, ofs.y, bgBlock, bgRect ) )
			return true;
	}

	if( iconBlock.IsValid() )
	{
		WgRect iconRect = GetIconRect( sz );
		if( WgUtil::MarkTestBlock( ofs.x, ofs.y, iconBlock, iconRect ) )
			return true;
	}

	return false;
}

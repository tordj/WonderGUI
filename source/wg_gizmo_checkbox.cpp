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
#include <wg_root.h>
#include <wg_eventhandler.h>
#include <assert.h>

using namespace WgSignal;

static const char	c_gizmoType[] = {"CheckBox"};



//____ WgGizmoCheckbox() _________________________________________________________________

WgGizmoCheckbox::WgGizmoCheckbox()
{
	m_bChecked			= false;
	m_bOver				= false;
	m_bPressed			= false;

	m_pText				= &m_text;
	m_text.setHolder( this );

	m_iconAreaBorders	= WgBorders(0);
	m_iconScale			= 0.f;
	m_bIconPushText		= true;

	m_aDisplace[0].x	= m_aDisplace[0].y = 0;
	m_aDisplace[1]		= m_aDisplace[2] = m_aDisplace[0];

	m_clickArea			= DEFAULT;
}

//____ Destructor _____________________________________________________________

WgGizmoCheckbox::~WgGizmoCheckbox()
{
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

bool WgGizmoCheckbox::SetSource( const WgBlockSetPtr& _pUnchecked, const WgBlockSetPtr& _pChecked )
{
	m_pBlockUnchecked	= _pUnchecked;
	m_pBlockChecked		= _pChecked;
	
	_onRefresh();
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

	_onRefresh();
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
			Emit( Set() );
		else
			Emit( Unset() );

		Emit( Flipped(), m_bChecked );

		WgEventHandler * pHandler = EventHandler();
		if( pHandler )
		{				
			if( _state )
				pHandler->QueueEvent( new WgEvent::CheckboxCheck( this ) );
			else
				pHandler->QueueEvent( new WgEvent::CheckboxUncheck( this ) );
				
			pHandler->QueueEvent( new WgEvent::CheckboxToggle(this, _state ) );
		}

		RequestRender();
	}

	return true;
}

//____ BestSize() __________________________________________________

WgSize WgGizmoCheckbox::BestSize() const
{
	WgSize iconBestSize;
	WgSize bgBestSize;
	WgSize textBestSize;
	
	if( m_text.nbChars() > 0 )
		textBestSize = m_text.unwrappedSize();

	if( m_pBlockUnchecked )
	{
		bgBestSize = m_pBlockUnchecked->GetSize();
		textBestSize += m_pBlockUnchecked->GetContentBorders();
	}	

	if( m_pIconUnchecked )
	{
		iconBestSize = m_pIconUnchecked->GetSize() + m_iconAreaBorders.size();
		
		//TODO: Add magic for how icon influences textBestSize based on origo, iconAreaBorders, iconScale and bgBestSize
	}

	WgSize bestSize = WgSize::max( WgSize::max(iconBestSize,bgBestSize), textBestSize);
	
	return bestSize;
}


//____ _onEnable() _________________________________________________
void WgGizmoCheckbox::_onEnable()
{
	RequestRender();
}

//____ _onDisable() _________________________________________________
void WgGizmoCheckbox::_onDisable()
{
	m_bOver = false;
	m_bPressed = false;

	RequestRender();
}

//____ _onEvent() _____________________________________________________________

void WgGizmoCheckbox::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSE_ENTER:
			if( !m_bOver )
			{
				m_bOver = true;
				RequestRender();
			}
			break;

		case WG_EVENT_MOUSE_LEAVE:
			if( m_bOver )
			{
				m_bOver = false;
				RequestRender();
			}
			break;

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 && !m_bPressed )
			{
				m_bPressed = true;
				RequestRender();
			}
			break;
		}
		
		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 && m_bPressed )
			{
				m_bPressed = false;
				RequestRender();
			}
			break;
		}
		
		case WG_EVENT_MOUSEBUTTON_CLICK:
		{
			int button = static_cast<const WgEvent::MouseButtonPress*>(pEvent)->Button();
			if( button == 1 )
				SetState( !m_bChecked );
			break;
		}
		
        default:
            break;
	}
}

//____ _onAction() _________________________________________________

void WgGizmoCheckbox::_onAction( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
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

	return _getContentRect( gizmoSize, _getIconRect( gizmoSize ) ).w;
}


//____ _getIconRect() _________________________________________________________

/*
	Gets an icon-rect for the icon including borders, relative to upper left corner of widget.

*/

WgRect WgGizmoCheckbox::_getIconRect( const WgSize& gizmoSize )
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

		int bgW = gizmoSize.w - m_iconAreaBorders.width();
		int bgH = gizmoSize.h - m_iconAreaBorders.height();

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
		rect.w = w + m_iconAreaBorders.width();
		rect.h = h + m_iconAreaBorders.height();;
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


//____ _getContentRect() _____________________________________________________

WgRect WgGizmoCheckbox::_getContentRect( const WgSize& gizmoSize, const WgRect& iconRect )
{
	WgRect rect( 0,0, gizmoSize.w, gizmoSize.h );

	WgBlockSetPtr p = m_bChecked ? m_pBlockChecked : m_pBlockUnchecked;

	if( p )
		rect.shrink( p->GetContentBorders() );

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
			if( contentX2 > iconX1 )
			rect.w = contentX1 - iconX1;
		}
		else
		{
			if( contentX1 < iconX2 )
			{
				rect.x = iconX2;
				rect.w = contentX2 - iconX2;
			}
		}
	}

	return rect;
}



//____ _onRender() ________________________________________________________

void WgGizmoCheckbox::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
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
		pDevice->ClipBlitBlock( _clip, pBlockSet->GetBlock(mode,_canvas.size()), _canvas );

	// Blit icon

	WgRect iconRect = _getIconRect( _canvas );

	if( pIcon && iconRect.w > 0 && iconRect.h > 0 )
	{
		WgRect iconBlitRect = iconRect;
		iconBlitRect.shrink( m_iconAreaBorders );
		iconBlitRect.x += _canvas.x;
		iconBlitRect.y += _canvas.y;
		pDevice->ClipBlitBlock( _clip, pIcon->GetBlock(mode,iconBlitRect.size()), iconBlitRect );
	}

	// Print content

 	if( m_text.nbLines()!= 0 )
	{
		if( pBlockSet )
			m_text.SetBgBlockColors(pBlockSet->GetTextColors());

		int	iDisplacement = 0;
		if( (m_bPressed && m_bOver) || m_bChecked )
            iDisplacement = 2;
		else if( m_bOver )
			iDisplacement = 1;

		int   xOfs = m_aDisplace[iDisplacement].x;
		int   yOfs = m_aDisplace[iDisplacement].y;


		WgRect	printWindow = _getContentRect( _canvas, iconRect );
		printWindow.x += _canvas.x;
		printWindow.y += _canvas.y;

		printWindow.x += xOfs;
		printWindow.y += yOfs;
		m_pText->setMode( mode );
		pDevice->PrintText( _clip, m_pText, printWindow );
	}
}

//____ _onRefresh() _______________________________________________________

void WgGizmoCheckbox::_onRefresh( void )
{
	if( m_pBlockChecked && m_pBlockUnchecked && m_pBlockChecked->IsOpaque() &&
		m_pBlockUnchecked->IsOpaque() )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	RequestRender();
}

//____ _onNewSize() _______________________________________________________

void WgGizmoCheckbox::_onNewSize( const WgSize& size )
{
	m_text.setLineWidth( GetTextAreaWidth() );
}


//____ _onCloneContent() _______________________________________________________

void WgGizmoCheckbox::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoCheckbox * pOrg = (WgGizmoCheckbox *) _pOrg;

	m_bChecked			= pOrg->m_bChecked;
	m_bOver				= false;
	m_bPressed			= false;

	m_pBlockUnchecked	= pOrg->m_pBlockUnchecked;
	m_pBlockChecked		= pOrg->m_pBlockChecked;

	m_pIconUnchecked	= pOrg->m_pIconUnchecked;
	m_pIconChecked		= pOrg->m_pIconChecked;
	m_iconOrigo			= pOrg->m_iconOrigo;
	m_iconScale			= pOrg->m_iconScale;
	m_iconAreaBorders	= pOrg->m_iconAreaBorders;
	m_bIconPushText		= pOrg->m_bIconPushText;

	m_text				= pOrg->m_text;
	m_clickArea			= pOrg->m_clickArea;

	for( int i = 0 ; i < 3 ; i++ )
	{
		m_aDisplace[i]	= pOrg->m_aDisplace[i];
	}

	Wg_Interface_TextHolder::_onCloneContent( pOrg );
}

//____ _textModified() _________________________________________________________

void WgGizmoCheckbox::_textModified()
{
	RequestRender();
}


//____ _markTestTextArea() ______________________________________________________

bool WgGizmoCheckbox::_markTestTextArea( int _x, int _y )
{
	WgRect	contentRect = _getContentRect( Size(), _getIconRect( Size() ) );

	if( m_text.CoordToOfs( WgCoord(_x,_y), contentRect ) != -1 )
		return true;

	return false;
}

//____ _onAlphaTest() ______________________________________________________

bool WgGizmoCheckbox::_onAlphaTest( const WgCoord& ofs )
{
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

	WgSize	bgSize		= Size();
	WgRect	iconRect	= _getIconRect( bgSize );

	iconRect.shrink( m_iconAreaBorders );


	if( m_bChecked )
	{
		if( m_pBlockChecked )
			bgBlock = m_pBlockChecked->GetBlock(mode,bgSize);

		if( m_pIconChecked )
			iconBlock = m_pIconChecked->GetBlock(mode,iconRect);
	}
	else
	{
		if( m_pBlockUnchecked )
			bgBlock = m_pBlockUnchecked->GetBlock(mode,bgSize);

		if( m_pIconUnchecked )
			iconBlock = m_pIconUnchecked->GetBlock(mode,iconRect);
	}


	switch( m_clickArea )
	{
		case DEFAULT:		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		{
			// Extend iconRect so it connects with textArea before we compare

			WgRect	contentRect = _getContentRect( bgSize, _getIconRect( bgSize ) );	// This call needs IconRect with borders...

			if( iconRect.x + iconRect.w < contentRect.x )
				iconRect.w = contentRect.x - iconRect.x;

			if( iconRect.x > contentRect.right() )
			{
				iconRect.w += iconRect.x - contentRect.right();
				iconRect.x = contentRect.right();
			}

			if( iconRect.y + iconRect.h < contentRect.y )
				iconRect.h = contentRect.y - iconRect.y;

			if( iconRect.y > contentRect.bottom() )
			{
				iconRect.h += iconRect.y - contentRect.bottom();
				iconRect.y = contentRect.bottom();
			}

			//

			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize) )) ||
				_markTestTextArea( ofs.x, ofs.y ) ||
				iconRect.contains( ofs.x, ofs.y ) )
				return true;

			return false;
		}
		case ALPHA:			// Alpha test on background and icon.
		{
			if( (bgBlock.IsValid() && WgUtil::MarkTestBlock( ofs, bgBlock, WgRect(0,0,bgSize) )) ||
				(iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect )) )
				return true;

			return false;
		}
		case GEO:			// Full geometry of Gizmo is clickable.
			return true;
		case ICON:			// Only the icon (alpha test) is clickable.
		{
			if( iconBlock.IsValid() && WgUtil::MarkTestBlock( ofs, iconBlock, iconRect ) )
				return true;

			return false;
		}
		case TEXT:			// Only the text is clickable.
		{
			if( _markTestTextArea( ofs.x, ofs.y ) )
				return true;

			return false;
		}

		default:
			assert(false);			// Garbage in m_clickArea
			return false;
	};
}

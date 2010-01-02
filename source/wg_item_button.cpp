#include <wg_item_button.h>
#include <wg_gfx.h>
#include <wg_font.h>
#include <wg_interface_itemholder.h>

static const char	Wdg_Type[] = {"TordJ/WgItemButton"};

const char * WgItemButton::Type( void ) const
{
	return GetMyType();
}

const char * WgItemButton::GetMyType()
{
	return Wdg_Type;
}


WgItemButton::WgItemButton( ) :
	m_pGfx(0)
{
	Init();
}

WgItemButton::WgItemButton( Uint32 id ) :
	WgItem(id),
	m_pGfx(0)
{
	Init();
}

WgItemButton::WgItemButton( Uint32 id, WgBlockSetPtr pGfx ) :
	WgItem(id),
	m_pGfx(pGfx)
{
	Init();
}

WgItemButton::WgItemButton( Uint32 id, WgBlockSetPtr pGfx, const char * pStr, const WgTextPropPtr& pProp ) :
	WgItem(id),
	m_pGfx(pGfx)
{
	m_text.setDefaultProperties( pProp );
	m_text.setText( pStr );

	Init();
}

WgItemButton::WgItemButton( Uint32 id, WgBlockSetPtr pGfx, const WgChar * pStr, const WgTextPropPtr& pProp ) :
	WgItem(id),
	m_pGfx(pGfx)
{
	m_text.setDefaultProperties( pProp );
	m_text.setText( pStr );

	Init();
}

void WgItemButton::Init()
{
	m_text.SetWrap(false);

	m_bMouseOver	= false;
	m_bEnabled		= true;
	m_pText			= &m_text;

	m_width = m_text.width();
	//m_height = m_text.height();
	m_height = 0;

	if(m_pGfx)
	{
		if( m_width < (Uint32)m_pGfx->GetWidth() )
			m_width = m_pGfx->GetWidth();
		m_height = m_pGfx->GetHeight();
	}
}

WgItemButton::~WgItemButton()
{
}

void WgItemButton::TextModified()
{
	Uint32	width;

	width = m_text.width();

	if(m_pGfx)
	{
		if( width < (Uint32)m_pGfx->GetWidth() )
			width = m_pGfx->GetWidth();
	}

	Sint32 widthModif = width - m_width;

	m_width = width;
	Modified(widthModif, 0);
}



void WgItemButton::ActionRespond( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	if( !m_bEnabled )
		return;

//	int		btn = _button_key-1;

	switch( _action )
	{

		case WgInput::POINTER_OVER:
		{
			if( !m_bMouseOver )
			{
				m_bMouseOver = true;
				Modified(0,0);
			}
			break;
		}

		case WgInput::POINTER_EXIT:
		{
			if( m_bMouseOver )
			{
				m_bMouseOver = false;
				Modified(0,0);
			}
			break;
		}

        default:
            break;
	}

	WgItem::ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );
}

void WgItemButton::SetText( const char * pText )
{
	m_text.setText(pText);
	TextModified();
}

void WgItemButton::Render( const WgRect& _window, const WgRect& _clip )
{
	WgRect r = _window;

	if( r.w < (Sint32) m_width )
		r.w = m_width;
	if( r.h < (Sint32) m_height )
		r.h = m_height;



	if( m_pGfx )
	{
		WgMode mode;
		if( IsSelected() )
			mode = WG_MODE_SELECTED;
		else if( m_bMouseOver )
			mode = WG_MODE_MARKED;
		else
			mode = WG_MODE_NORMAL;

		WgGfx::clipBlitBlock( _clip, m_pGfx->GetBlock(mode), _window );
		//WgGfx::clipBlitRow( _clip, m_pSurf, *pSrc, m_tileOfs, m_tileLen, _window.x, _window.y, _window.w );
	}

	WgGfx::clipPrintText( _clip, m_pText, r );

}

WgItem* WgItemButton::Clone( WgItem * _pClone )
{
	//TODO: This method doesn't work if we get a vaild object as a parameter,
	//      so consequently we can't use this as a super-class.

	WgItemButton * pClone = (WgItemButton*) _pClone;

	if( !pClone )
		pClone = new WgItemButton( m_id, m_pGfx, m_text.getText(), m_text.getDefaultProperties());

	Wg_Interface_TextHolder::CloneInterface( pClone );
	return WgItem::Clone(pClone);
}

void WgItemButton::Enable( bool bEnable )
{
	if( m_bEnabled != bEnable )
	{
		m_bEnabled = bEnable;
		if( m_bEnabled )
			m_text.setMode(WG_MODE_NORMAL);
		else
			m_text.setMode(WG_MODE_DISABLED);

		TextModified();					// Font might have changed...
	}
}



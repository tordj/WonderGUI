#include <wg_item_text.h>
#include <wg_gfx.h>
#include <wg_font.h>
#include <wg_interface_itemholder.h>

static const char	Wdg_Type[] = {"TordJ/WgItemText"};

const char * WgItemText::Type( void ) const
{
	return GetMyType();
}

const char * WgItemText::GetMyType()
{
	return Wdg_Type;
}

WgItemText::WgItemText( )
{
	Init();
}

WgItemText::WgItemText( Sint64 id, const WgCharSeq& seq, const WgTextPropPtr& pProp, Uint8 marginLeft, Uint8 marginRight, Uint8 marginTop, Uint8 marginBottom ) : WgItem(id)
{
	m_text.setProperties(pProp);
	m_text.setText( seq );

	m_margin.left	= marginLeft;
	m_margin.right	= marginRight;
	m_margin.top	= marginTop;
	m_margin.bottom	= marginBottom;

	Init();
}

WgItemText::WgItemText( Sint64 id, const WgText * pText, const WgTextPropPtr& pProp, Uint8 marginLeft, Uint8 marginRight, Uint8 marginTop, Uint8 marginBottom ) : WgItem(id)
{
	m_text.setProperties(pProp);
	m_text.setText( pText );

	m_margin.left	= marginLeft;
	m_margin.right	= marginRight;
	m_margin.top		= marginTop;
	m_margin.bottom	= marginBottom;

	Init();
}

void WgItemText::Init()
{
	m_bEnabled = true;
	m_bAllFitInArea = true;
	m_pText = &m_text;
	m_text.setHolder( this );
	m_text.SetWrap(false);
	m_text.SetAutoEllipsis(IsAutoEllipsisDefault());	

	m_width = m_text.width() + m_margin.left + m_margin.right;
	m_height = m_text.height() + m_margin.top + m_margin.bottom;

	// We might get zero width/height if we don't have any font and that is not acceptable...

	if( m_width < 1 )
		m_width = 1;

	if( m_height < 1 )
		m_height = 1;

}

WgItemText::~WgItemText()
{
}

void WgItemText::SetTextProp(const WgTextPropPtr& pProp)
{
	m_text.setProperties(pProp);
	_textModified();
}

void WgItemText::SetText(const WgCharSeq& seq)
{
	m_text.setText(seq);
	Modified( 0, 0 );
}

void WgItemText::SetText(const WgText * pText)
{
	m_text.setText(pText);
	_textModified();
}

void WgItemText::SetMargin(WgBorders margin)
{
	m_margin = margin;
	_textModified();
}

void WgItemText::_textModified()
{
	Uint32	width, height;

	width = m_text.width() + m_margin.left + m_margin.right;
	height = m_text.height() + m_margin.top + m_margin.bottom;

	// We might get zero width/height if we don't have any font and that is not acceptable...

	if( width < 1 )
		width = 1;

	if( height < 1 )
		height = 1;

	Sint32 widthModif = width - m_width;
	Sint32 heightModif = height - m_height;

	m_width = width;
	m_height = height;

	Modified(widthModif, heightModif);
}

void WgItemText::Render( const WgRect& _window, const WgRect& _clip )
{
	if( m_text.nbLines() == 0 || !m_text.getFont() )
		return;

	WgRect r = _window;
/*
	if( r.w < (Sint32) m_width )
		r.w = m_width;
	if( r.h < (Sint32) m_height )
		r.h = m_height;
*/
	r.x += m_margin.left;
	r.y += m_margin.top;
	r.w -= m_margin.left + m_margin.right;
	r.h -= m_margin.top + m_margin.bottom;

	if( GetMode() != m_pText->mode() )
		m_pText->setMode( GetMode() );

	m_bAllFitInArea = WgGfx::printText( _clip, m_pText, r );
}

WgItem* WgItemText::Clone( WgItem * _pClone )
{
	//TODO: This method doesn't work if we get a vaild object as a parameter,
	//      so consequently we can't use this as a super-class.

	WgItemText * pClone = (WgItemText*) _pClone;

	if( !pClone )
		pClone = new WgItemText( m_id, m_pText, m_pText->getProperties(), m_margin.left, m_margin.right, m_margin.top, m_margin.bottom );

	Wg_Interface_TextHolder::_cloneInterface( pClone );
	return WgItem::Clone(pClone);
}

void WgItemText::Enable( bool bEnable )
{
	if( m_bEnabled != bEnable )
	{
		m_bEnabled = bEnable;

		if( m_bEnabled )
			m_text.setMode(WG_MODE_NORMAL);
		else
			m_text.setMode(WG_MODE_DISABLED);

		_textModified();					// Font might have changed...
	}
}

WgString WgItemText::GetTooltipString() const
{
	if( m_text.IsAutoEllipsis() && !m_bAllFitInArea )
		return WgString( m_text.getBuffer() );

	return 0;
}

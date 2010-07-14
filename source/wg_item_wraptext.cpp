#include <wg_item_wraptext.h>
#include <wg_gfx.h>
#include <wg_font.h>
#include <wg_interface_itemholder.h>

static const char	Wdg_Type[] = {"TordJ/WgItemWrapText"};

const char * WgItemWrapText::Type( void ) const
{
	return GetMyType();
}

const char * WgItemWrapText::GetMyType()
{
	return Wdg_Type;
}
//____ Constructors ___________________________________________________________
WgItemWrapText::WgItemWrapText( ) :
	m_minWidth(0),
	m_minHeight(0),
	m_bgFill(0,0,0,0)
{
	Init();
}

WgItemWrapText::WgItemWrapText( Uint32 id, const char * pStr, WgTextPropPtr pProp, Uint32 startwidth, WgColor bgFill, Uint8 marginLeft, Uint8 marginRight, Uint8 marginTop, Uint8 marginBottom ) : WgItem(id)
{
	m_text.setProperties( pProp );
	Uint32 linewidth = startwidth - marginLeft - marginRight;
	if( linewidth > startwidth )
		linewidth = 1;
	m_text.setLineWidth(linewidth);
	m_text.setText( pStr );
	m_text.setHolder( this );

	m_margin.left	= marginLeft;
	m_margin.right	= marginRight;
	m_margin.top		= marginTop;
	m_margin.bottom	= marginBottom;

	m_minWidth		= 0;
	m_minHeight		= 0;

	m_bgFill		= bgFill;
	Init();
}

WgItemWrapText::WgItemWrapText( Uint32 id, const Uint16 * pStr, WgTextPropPtr pProp, Uint32 startwidth, WgColor bgFill, Uint8 marginLeft, Uint8 marginRight, Uint8 marginTop, Uint8 marginBottom ) : WgItem(id)
{
	m_text.setProperties( pProp );
	Uint32 linewidth = startwidth - marginLeft - marginRight;
	if( linewidth > startwidth )
		linewidth = 1;
	m_text.setLineWidth(linewidth);
	m_text.setText( pStr );

	m_margin.left	= marginLeft;
	m_margin.right	= marginRight;
	m_margin.top		= marginTop;
	m_margin.bottom	= marginBottom;

	m_minWidth		= 0;
	m_minHeight		= 0;

	m_bgFill		= bgFill;
	Init();
}

WgItemWrapText::WgItemWrapText( Uint32 id, const WgText * pText, Uint32 startwidth, WgColor bgFill, Uint8 marginLeft, Uint8 marginRight, Uint8 marginTop, Uint8 marginBottom ) : WgItem(id)
{
	m_text.setProperties( pText->getProperties() );
	Uint32 linewidth = startwidth - marginLeft - marginRight;
	if( linewidth > startwidth )
		linewidth = 1;
	m_text.setLineWidth(linewidth);
	m_text.setText( pText );

	m_margin.left	= marginLeft;
	m_margin.right	= marginRight;
	m_margin.top		= marginTop;
	m_margin.bottom	= marginBottom;

	m_minWidth		= 0;
	m_minHeight		= 0;

	m_bgFill		= bgFill;
	Init();
}

//____ Init() _________________________________________________________________

void WgItemWrapText::Init()
{
	m_bEnabled = true;
	m_pText = &m_text;

	m_width = m_text.width() + m_margin.left + m_margin.right;
	m_height = m_text.height() + m_margin.top + m_margin.bottom;

	if( m_width < 1 )
		m_width = 1;

	if( m_height < 1 )
		m_height = 1;
}

//____ Destructor _____________________________________________________________

WgItemWrapText::~WgItemWrapText()
{
}

//____ MinSize() _________________________________________________________

void WgItemWrapText::MinSize( Uint32 width, Uint32 height )
{
	m_minWidth = width;
	m_minHeight = height;
	UpdateSize();
}

//____ SetBgFill() _________________________________________________________
void WgItemWrapText::SetBgFill( const WgColor& bgFill )
{
	m_bgFill = bgFill;
	Modified( 0, 0 );
}

void WgItemWrapText::SetText(const char * pStr)
{
	m_text.setText(pStr);
	Modified( 0, 0 );
}

void WgItemWrapText::SetText(const Uint16 * pStr)
{
	m_text.setText(pStr);
	Modified( 0, 0 );
}

void WgItemWrapText::SetText(const WgText * pStr)
{
	m_text.setText(pStr);
	Modified( 0, 0 );
}

void WgItemWrapText::SetTextProp(WgTextPropPtr pProp)
{
	m_text.setProperties(pProp);
	Modified( 0, 0 );
}

void WgItemWrapText::SetMargin(WgBorders margin)
{
	m_margin = margin;
	Modified( 0, 0 );
}

//____ TextModified() _________________________________________________________

void WgItemWrapText::TextModified()
{
	UpdateSize();
}

//____ UpdateSize() _________________________________________________________

void WgItemWrapText::UpdateSize()
{
	Uint32	width, height;

	width = m_text.width() + m_margin.left + m_margin.right;
	height = m_text.height() + m_margin.top + m_margin.bottom;

	if( width < m_minWidth )
		width = m_minWidth + m_margin.left + m_margin.right;
	if( height < m_minHeight )
		height = m_minHeight + m_margin.top + m_margin.bottom;

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

//____ Render() _______________________________________________________________

void WgItemWrapText::Render( const WgRect& _window, const WgRect& _clip )
{
	WgRect r = _window;

	if( r.w < (Sint32) m_width )
		r.w = m_width;
	if( r.h < (Sint32) m_height )
		r.h = m_height;

	r.x += m_margin.left;
	r.y += m_margin.top;
	r.w -= ( m_margin.left + m_margin.right );
	r.h -= ( m_margin.top + m_margin.bottom );


	if( m_bgFill.a != 0 )
		WgGfx::fillRect( WgRect(r, _clip), m_bgFill );

	if( GetMode() != m_pText->mode() )
		m_pText->setMode( GetMode() );

	WgGfx::printText( _clip, m_pText, r );

}

//____ Clone() ________________________________________________________________

WgItem* WgItemWrapText::Clone( WgItem * _pClone )
{
	//TODO: This method doesn't work if we get a vaild object as a parameter,
	//      so consequently we can't use this as a super-class.

	WgItemWrapText * pClone = (WgItemWrapText*) _pClone;

	if( !pClone )
	{
		pClone = new WgItemWrapText( m_id, m_pText, m_width, m_bgFill, m_margin.left, m_margin.right, m_margin.top, m_margin.bottom );
		pClone->m_minWidth = m_minWidth;
		pClone->m_minHeight = m_minHeight;
	}

	Wg_Interface_TextHolder::CloneInterface( pClone );
	return WgItem::Clone(pClone);
}

//____ Enable() _______________________________________________________________

void WgItemWrapText::Enable( bool bEnable )
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

//____ CompareTo() ____________________________________________________________

Sint32 WgItemWrapText::CompareTo( WgItem * _pOther )
{
	WgItemWrapText * pOther = (WgItemWrapText *) _pOther; //dynamic_cast<WgItemWrapText*>(_pOther);

	if( pOther == 0 )
		return 0;

	return m_text.getBuffer()->CompareTo( pOther->m_text.getBuffer() );
}

//____ AdaptToWidth() _________________________________________________________

void WgItemWrapText::AdaptToWidth( Uint32 displayed_width )
{
	int width = displayed_width - m_margin.left - m_margin.right;
	if( width < 1 )
		width = 1;

	m_text.setLineWidth( width );
	TextModified();
}


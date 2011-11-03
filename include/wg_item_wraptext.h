#ifndef WG_ITEM_WRAPTEXT_DOT_H
#define WG_ITEM_WRAPTEXT_DOT_H

#include <wg_item.h>
#include <wg_text.h>
#include <wg_interface_textholder.h>
#include <wg_color.h>
#include <wg_blockset.h>
#include <wg_tooltip.h>

class WgItemWrapText : public WgItem, public Wg_Interface_TextHolder
{
public:
	WgItemWrapText(  );
	WgItemWrapText( Sint64 id, const WgCharSeq& seq, WgTextPropPtr pProp, Uint32 startwidth, WgColor bgFill = WgColor(0,0,0,0), Uint8 marginLeft=0, Uint8 marginRight=0, Uint8 marginTop=0, Uint8 marginBottom=0 );
	WgItemWrapText( Sint64 id, const WgText * pText, Uint32 startwidth, WgColor bgFill = WgColor(0,0,0,0), Uint8 marginLeft=0, Uint8 marginRight=0, Uint8 marginTop=0, Uint8 marginBottom=0 );
	~WgItemWrapText();

	void			MinSize( Uint32 width, Uint32 height );

	void			SetBgFill( const WgColor& bgFill );
	void			SetText( const WgCharSeq& seq );
	void			SetText(const WgText * pStr);
	void			SetTextProp(WgTextPropPtr pProp);
	void			SetMargin(WgBorders margin);

	WgPointerStyle	GetPointerStyle() const;


	DECLARE_TOOLTIP_SUPPORT();

	virtual const char *Type( void ) const;
	static const char * GetMyType();
protected:
	void	Init();
	void	_textModified();		// Callback for Wg_Interface_TextHolder
	void	UpdateSize();

	virtual void 	AdaptToWidth( Uint32 displayed_width );

	virtual void	ActionRespond( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
 	virtual void	Render( const WgRect& _window, const WgRect& _clip );
	virtual WgItem* Clone( WgItem * _pClone = 0 );
	virtual void	Enable( bool bEnable ); 
	virtual Sint32	CompareTo( WgItem * pOther );

private:

	WgText			m_text;
	WgBorders		m_margin;
	Uint32			m_minWidth;
	Uint32			m_minHeight;
	bool			m_bEnabled;
	WgColor			m_bgFill;
};


#endif // WG_ITEM_WRAPTEXT_DOT_H


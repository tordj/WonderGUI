#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#include <wg3_chain.h>

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#include <wg3_textstyle.h>

//____ WgRulerLabels ____________________________________________________________

class WgRulerLabels : public WgWidget, protected WgTextHolder
{
public:
	WgRulerLabels();
	virtual ~WgRulerLabels();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgRulerLabels(); };

	//____ Methods __________________________________________

	void			AddLabel( const wg::CharSeq& text, wg::TextStyle * pStyle, float offset, WgOrigo origo );
	WgSize			PreferredPixelSize() const;
	void			SetDirection( WgDirection direction );

protected:

	void			_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window );
	void			_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
	void 			_setScale( int scale );

	void            _textModified( WgText * pText );

	class Label : public wg::Link
	{
	public:
		LINK_METHODS(Label);

		WgText		text;
		float		offset;
	};

	wg::Chain<Label>	m_labels;
	WgDirection		m_direction;
};



#endif //WG_VOLUMEMETER_DOT_H

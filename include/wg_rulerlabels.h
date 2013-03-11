#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

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
	
	void			AddLabel( const WgCharSeq& text, const WgTextpropPtr& pProp, float offset, WgOrigo origo );
    void            SetTextManager( WgTextManager * pTextManager );
	WgSize			PreferredSize() const;
	void			SetDirection( WgDirection direction );
	
protected:
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
    void            _textModified( WgText * pText );
	
	class Label : public WgLink
	{
	public:
		LINK_METHODS(Label);
		
		WgText		text;
		float		offset;
	};
	
	WgChain<Label>	m_labels;
	WgDirection		m_direction;
    WgTextManager * m_pTextManager;
};



#endif //WG_VOLUMEMETER_DOT_H
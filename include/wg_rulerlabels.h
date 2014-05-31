#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

class WgRulerLabels;
typedef	WgSmartPtr<WgRulerLabels,WgWidgetPtr>		WgRulerLabelsPtr;
typedef	WgWeakPtr<WgRulerLabels,WgWidgetWeakPtr>	WgRulerLabelsWeakPtr;

//____ WgRulerLabels ____________________________________________________________

class WgRulerLabels : public WgWidget, protected WgTextHolder
{
public:
	static WgRulerLabelsPtr	Create() { return WgRulerLabelsPtr(new WgRulerLabels()); }
	
	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgRulerLabelsPtr	Cast( const WgObjectPtr& pObject );
	
	//____ Methods __________________________________________
	
	void			AddLabel( const WgCharSeq& text, const WgTextpropPtr& pProp, float offset, WgOrigo origo );
    void            SetTextManager( WgTextManager * pTextManager );
	WgSize			PreferredSize() const;
	void			SetDirection( WgDirection direction );
	WgIModifTextPtr	GetLabel(int index);


protected:
	WgRulerLabels();
	virtual ~WgRulerLabels();
	virtual WgWidget* _newOfMyType() const { return new WgRulerLabels(); };
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onStateChanged( WgState oldState );
	void			_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	bool			_onAlphaTest( const WgCoord& ofs );
    void            _fieldModified( WgTextField * pField );
	
	class Label : public WgLink
	{
	public:
		LINK_METHODS(Label);
		
		WgTextField	text;
		float		offset;
	};
	
	WgChain<Label>	m_labels;
	WgDirection		m_direction;
    WgTextManager * m_pTextManager;
};



#endif //WG_VOLUMEMETER_DOT_H
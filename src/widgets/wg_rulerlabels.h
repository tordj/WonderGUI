#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

class WgRulerLabels;
typedef	WgStrongPtr<WgRulerLabels,WgWidgetPtr>		WgRulerLabelsPtr;
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
	
	void			AddLabel( const WgCharSeq& text, const WgTextStylePtr& pStyle, float offset );
 	WgSize			PreferredSize() const;
	void			SetDirection( WgDirection direction );
	WgModTextPtr	GetLabel(int index);


protected:
	WgRulerLabels();
	virtual ~WgRulerLabels();
	virtual WgWidget* _newOfMyType() const { return new WgRulerLabels(); };
	
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onCloneContent( const WgWidget * _pOrg );
	void			_onStateChanged( WgState oldState );
	void			_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	bool			_onAlphaTest( const WgCoord& ofs, const WgSize& sz );

	WgObject * 		_object() { return this; };
	void			_onFieldDirty( WgField * pField );
	void 			_onFieldResize( WgField * pField );

	class Label : public WgLink
	{
	public:
		Label(WgTextHolder*pHolder) : textField(pHolder), textInterface(&textField) {}
		LINK_METHODS(Label);
		WgModText	textInterface;
		WgTextField	textField;
		float		offset;
	};
	
	WgChain<Label>	m_labels;
	WgDirection		m_direction;
};



#endif //WG_VOLUMEMETER_DOT_H

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

namespace wg 
{
	
	class WgRulerLabels;
	typedef	WgStrongPtr<WgRulerLabels,WgWidget_p>		WgRulerLabels_p;
	typedef	WgWeakPtr<WgRulerLabels,WgWidget_wp>	WgRulerLabels_wp;
	
	//____ WgRulerLabels ____________________________________________________________
	
	class WgRulerLabels : public WgWidget, protected WgTextHolder
	{
	public:
		static WgRulerLabels_p	create() { return WgRulerLabels_p(new WgRulerLabels()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgRulerLabels_p	cast( const WgObject_p& pObject );
		
		//____ Methods __________________________________________
		
		void			AddLabel( const WgCharSeq& text, const WgTextStyle_p& pStyle, float offset );
	 	WgSize			preferredSize() const;
		void			SetDirection( WgDirection direction );
		WgModText_p	GetLabel(int index);
	
	
	protected:
		WgRulerLabels();
		virtual ~WgRulerLabels();
		virtual WgWidget* _newOfMyType() const { return new WgRulerLabels(); };
		
		void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void			_onCloneContent( const WgWidget * _pOrg );
		void			_onStateChanged( WgState oldState );
		void			_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
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
	
	
	

} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H

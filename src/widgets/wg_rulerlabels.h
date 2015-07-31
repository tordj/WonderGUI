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
	
	class RulerLabels;
	typedef	WgStrongPtr<RulerLabels,Widget_p>		RulerLabels_p;
	typedef	WgWeakPtr<RulerLabels,Widget_wp>	RulerLabels_wp;
	
	//____ RulerLabels ____________________________________________________________
	
	class RulerLabels : public Widget, protected TextHolder
	{
	public:
		static RulerLabels_p	create() { return RulerLabels_p(new RulerLabels()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static RulerLabels_p	cast( const Object_p& pObject );
		
		//____ Methods __________________________________________
		
		void			addLabel( const CharSeq& text, const TextStyle_p& pStyle, float offset );
	 	Size			preferredSize() const;
		void			setDirection( WgDirection direction );
		ModText_p	getLabel(int index);
	
	
	protected:
		RulerLabels();
		virtual ~RulerLabels();
		virtual Widget* _newOfMyType() const { return new RulerLabels(); };
		
		void			_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onCloneContent( const Widget * _pOrg );
		void			_onStateChanged( State oldState );
		void			_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
		bool			_onAlphaTest( const Coord& ofs, const Size& sz );
	
		Object * 		_object() { return this; };
		void			_onFieldDirty( Field * pField );
		void 			_onFieldResize( Field * pField );
	
		class Label : public Link
		{
		public:
			Label(TextHolder*pHolder) : textField(pHolder), textInterface(&textField) {}
			LINK_METHODS(Label);
			ModText	textInterface;
			TextField	textField;
			float		offset;
		};
		
		Chain<Label>	m_labels;
		WgDirection		m_direction;
	};
	
	
	

} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H

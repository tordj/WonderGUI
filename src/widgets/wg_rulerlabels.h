#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_chain.h>
#include <wg_modtext.h>

namespace wg 
{
	
	class RulerLabels;
	typedef	StrongPtr<RulerLabels,Widget_p>		RulerLabels_p;
	typedef	WeakPtr<RulerLabels,Widget_wp>	RulerLabels_wp;
	
	//____ RulerLabels ____________________________________________________________
	
	class RulerLabels : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static RulerLabels_p	create() { return RulerLabels_p(new RulerLabels()); }
		
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static RulerLabels_p cast( const Object_p& pObject );
		
		//.____ Geometry ____________________________________________
		
	 	Size			preferredSize() const;

		void			setDirection( Direction direction );
		inline Direction direction() const { return m_direction; }
		
		//.____ Control ____________________________________________
		
		void		addLabel( const CharSeq& text, const TextStyle_p& pStyle, float offset );
		ModText_p	getLabel(int index);

		
	protected:
		RulerLabels();
		virtual ~RulerLabels();
		virtual Widget* _newOfMyType() const { return new RulerLabels(); };
		
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_cloneContent( const Widget * _pOrg );
		void			_setState( State state );
		bool			_alphaTest( const Coord& ofs );
	
		Coord			_itemPos( const Item * pItem ) const;
		Size			_itemSize( const Item * pItem ) const;
		Rect			_itemGeo( const Item * pItem ) const;

		void			_resizeRequested( const Item * pItem );

	
		class Label : public Link
		{
		public:
			Label(Widget * pWidget) : textItem(pWidget), textInterface(&textItem) {}
			LINK_METHODS(Label);
			ModText	textInterface;
			TextItem	textItem;
			float		offset;
		};
		
		Chain<Label>	m_labels;
		Direction		m_direction;
	};
	
	
	

} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H

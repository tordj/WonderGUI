#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_chain.h>
#include <wg_itextdisplay.h>

namespace wg
{

	class RulerLabels;
	typedef	StrongPtr<RulerLabels>		RulerLabels_p;
	typedef	WeakPtr<RulerLabels>	RulerLabels_wp;

	//____ RulerLabels ____________________________________________________________

	class RulerLabels : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static RulerLabels_p	create() { return RulerLabels_p(new RulerLabels()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static RulerLabels_p cast( Object * pObject );

		//.____ Geometry ____________________________________________

	 	SizeI			preferredSize() const override;

		void			setDirection( Direction direction );
		inline Direction direction() const { return m_direction; }

		//.____ Control ____________________________________________

		void		addLabel( const CharSeq& text, TextStyle * pStyle, float offset );
		ITextDisplay_p	getLabel(int index);


	protected:
		RulerLabels();
		virtual ~RulerLabels();
		virtual Widget* _newOfMyType() const override { return new RulerLabels(); };

		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_setState( State state ) override;
		bool			_alphaTest( const CoordI& ofs ) override;

		CoordI			_componentPos( const Component * pComponent ) const override;
		SizeI			_componentSize( const Component * pComponent ) const override;
		RectI			_componentGeo( const Component * pComponent ) const override;

		void			_componentRequestResize( const Component * pComponent ) override;


		class Label : public Link
		{
		public:
			Label(Widget * pWidget) : text(pWidget), interface(&text) {}
			LINK_METHODS(Label);
			CTextDisplay	text;
			ITextDisplay	interface;
			float			offset;
		};

		Chain<Label>	m_labels;
		Direction		m_direction;
	};




} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H

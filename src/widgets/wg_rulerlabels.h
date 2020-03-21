#ifndef WG_RULERLABELS_DOT_H
#define WG_RULERLABELS_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_chain.h>
#include <wg_ctextdisplay.h>

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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setDirection( Direction direction );
		inline Direction direction() const { return m_direction; }

		//.____ Control ____________________________________________

		void		addLabel( const CharSeq& text, TextStyle * pStyle, float offset );
		CTextDisplay_p	getLabel(int index);


	protected:
		RulerLabels();
		virtual ~RulerLabels();
		virtual Widget* _newOfMyType() const override { return new RulerLabels(); };

		SizeI			_preferredSize() const override;

		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_setState( State state ) override;
		bool			_alphaTest( const CoordI& ofs ) override;

		CoordI			_componentPos( const GeoComponent * pComponent ) const override;
		SizeI			_componentSize( const GeoComponent * pComponent ) const override;
		RectI			_componentGeo( const GeoComponent * pComponent ) const override;

		void			_componentRequestResize( const GeoComponent * pComponent ) override;

		class Label : public Link
		{
		public:
			Label(Widget * pWidget) : text(pWidget) {}
			LINK_METHODS(Label);
			CTextDisplay	text;
			float			offset;

			class TextAccess : public CTextDisplay { friend class RulerLabels; };
			const TextAccess& _text() const { return static_cast<const TextAccess&>(text); }
			TextAccess& _text() { return static_cast<TextAccess&>(text); }
		};

		Chain<Label>	m_labels;
		Direction		m_direction;
	};




} // namespace wg
#endif //WG_VOLUMEMETER_DOT_H

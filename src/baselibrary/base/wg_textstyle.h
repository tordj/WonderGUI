/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/
#ifndef WG_TEXTSTYLE_DOT_H
#define WG_TEXTSTYLE_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_color.h>
#include <wg_util.h>
#include <wg_font.h>
#include <wg_textlink.h>

namespace wg
{

	class TextStyle;
	typedef	StrongPtr<TextStyle>		TextStyle_p;
	typedef	WeakPtr<TextStyle>		TextStyle_wp;

	struct TextAttr
	{
		public:

			//.____ Properties ___________________________________________________________

			Font_p			pFont;
			spx				size;
			HiColor			color;
			BlendMode		renderMode;
			HiColor			bgColor;
			BlendMode		bgRenderMode;
			TextDecoration	decoration;
			TextLink_p		pLink;


			// TODO: Add flags for showing linespace, tab, CR, NBSP and both BREAK PERMITTED CHARACTERS.
	};


	//TODO: Add support for subscript and superscript font-modifiers.
	//TODO: Add support for extra spacing between characters.

	class TextStyle : public Object
	{
		friend class Char;
		friend class CharBuffer;
		friend class TextTool;


	public:
		//.____ Creation __________________________________________

        static TextStyle_p	create();

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________

		void			setFont( Font * pFont );
		void			setLink( TextLink * pLink );
		void			setRenderMode(BlendMode mode);
		void			setBgRenderMode(BlendMode mode);

		void			setColor( HiColor color );
		void			setColor(HiColor color, State state);
		void			setBgColor( HiColor color );
		void			setBgColor(HiColor color, State state);
		void			setSize( pts size );
		void			setSize(pts size, State state);
		void			setDecoration( TextDecoration decoration );
		void			setDecoration( TextDecoration decoration, State state );

		inline void		clearFont() { setFont(nullptr); }
		inline void		clearLink() { setLink(nullptr); }
		inline void		clearRenderMode() { setRenderMode(BlendMode::Undefined); }
		inline void		clearBgRenderMode() { setBgRenderMode(BlendMode::Undefined); }

		void		    clearColor();
		void		    clearColor(State state);
		void		    clearBgColor();
		void		    clearBgColor(State state);
		inline void		clearSize() { setSize(0); }
		inline void		clearSize(State state) { setSize(0, state); }
		inline void		clearDecoration() { setDecoration(TextDecoration::Underline); }
		inline void		clearDecoration( State state ) { setDecoration(TextDecoration::Underline, state); }

		inline Font_p			font() const;
		inline TextLink_p		link() const;
		inline BlendMode		renderMode() const;
		inline BlendMode		bgRenderMode() const;
		inline HiColor			color( State state ) const;
		inline HiColor			bgColor( State state ) const;
		inline pts				size( State state ) const;
		inline TextDecoration 	decoration( State state ) const;

		inline bool             isColorStatic() const { return m_bStaticColor; }
		inline bool             isBgColorStatic() const { return m_bStaticBgColor; }
		inline bool             isSizeStatic() const { return m_bStaticSize; }
		inline bool             isDecorationStatic() const { return m_bStaticDecoration; }

		inline bool             isColorDefined(State state) const;
		inline bool             isBgColorDefined(State state) const;



		//.____ Misc __________________________________________________________

		inline TextStyle_h	handle() { return m_handle; }

		void			exportAttr( State state, TextAttr * pDest, int scale ) const;
		void			addToAttr( State state, TextAttr * pDest, int scale ) const;

        bool            isStateIdentical( State state1, State state2 ) const;
		bool			isIdentical( TextStyle * pOther );
		bool			isIdenticalForState( TextStyle * pOther, State state );

		TextStyle_p     clone() const;

	protected:
		TextStyle();
		virtual ~TextStyle();

		void		_refreshSize();
		void		_refreshColor();
		void		_refreshBgColor();
		void		_refreshDecoration();


		Font_p				m_pFont;
		TextLink_p			m_pLink;
		BlendMode			m_renderMode = BlendMode::Blend;
		BlendMode			m_bgRenderMode = BlendMode::Blend;

		pts					m_size[StateEnum_Nb];
		HiColor				m_color[StateEnum_Nb];
		HiColor				m_bgColor[StateEnum_Nb];
		TextDecoration		m_decoration[StateEnum_Nb];

		Bitmask<uint32_t>	m_sizeSetMask = 0;
		Bitmask<uint32_t>	m_colorSetMask = 0;
		Bitmask<uint32_t>	m_bgColorSetMask = 0;
		Bitmask<uint32_t>	m_decorationSetMask = 0;

		Bitmask<uint32_t>	m_colorDefinedMask = 0;
		Bitmask<uint32_t>	m_bgColorDefinedMask = 0;


		bool				m_bStaticColor = true;         // Combined color is identical in all states.
		bool				m_bStaticBgColor = true;       // Combined background color is identical in all states.
		bool				m_bStaticSize = true;          // Combined size is identical for in states.
		bool				m_bStaticDecoration = true;    // Combined decoration is identical in all states.


		TextStyle_h	m_handle;
	};


	//______________________________________________________________________________
	inline Font_p TextStyle::font() const
	{
		return m_pFont;
	}

	//______________________________________________________________________________
	inline TextLink_p TextStyle::link() const
	{
		return m_pLink;
	}

	//______________________________________________________________________________
	inline HiColor TextStyle::color( State state ) const
	{
		return m_color[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline 	HiColor TextStyle::bgColor( State state ) const
	{
		return m_bgColor[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline pts TextStyle::size( State state ) const
	{
		return m_size[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline TextDecoration TextStyle::decoration( State state ) const
	{
		return m_decoration[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::renderMode() const
	{
		return m_renderMode;
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::bgRenderMode() const
	{
		return m_bgRenderMode;
	}

	//______________________________________________________________________________
	inline bool TextStyle::isColorDefined(State state) const
	{
		return m_colorDefinedMask.bit(Util::_stateToIndex(state));
	}

	//______________________________________________________________________________
	inline bool TextStyle::isBgColorDefined(State state) const
	{
		return m_bgColorDefinedMask.bit(Util::_stateToIndex(state));
	}


} // namespace wg
#endif //WG_TEXTSTYLE_DOT_H

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
#ifndef WG3_TEXTSTYLE_DOT_H
#define WG3_TEXTSTYLE_DOT_H
#pragma once

#include <vector>

#include <wg3_pointers.h>
#include <wg3_color.h>
#include <wg3_util.h>
#include <wg3_font.h>
#include <wg3_textlink.h>

namespace wg
{

	class TextStyle;
	typedef	StrongPtr<TextStyle>	TextStyle_p;
	typedef	WeakPtr<TextStyle>		TextStyle_wp;

	struct TextAttr
	{
		public:

			//.____ Properties ___________________________________________________________

			Font_p			pFont;
			spx				size;
			HiColor			color;
			BlendMode		blendMode;
			HiColor			backColor;
			BlendMode		backBlendMode;
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

		//____ Blueprint ______________________________________________________

		struct StateData
		{
			HiColor			backColor = HiColor::Undefined;
			HiColor			color = HiColor::Undefined;
			TextDecoration	decoration = TextDecoration::Undefined;
			pts				size = -1;
		};

		struct StateBP
		{
			State			state = StateEnum::Normal;
			StateData		data;
		};

		struct Blueprint
		{
			HiColor			backColor = HiColor::Transparent;
			BlendMode		backBlendMode = BlendMode::Undefined;
			BlendMode		blendMode = BlendMode::Undefined;
			HiColor			color = HiColor::Undefined;
			TextDecoration	decoration = TextDecoration::Undefined;
			Font_p			font = nullptr;
			TextLink_p		link = nullptr;
			pts				size = -1;

			std::vector<StateBP>	states;
			
			void 			modifyState( State state, const StateData& data );
		};

		//.____ Creation __________________________________________

        static TextStyle_p	create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________

		inline Font_p			font() const;
		inline TextLink_p		link() const;
		inline BlendMode		blendMode() const;
		inline BlendMode		backBlendMode() const;
		inline HiColor			color( State state ) const;
		inline HiColor			backColor( State state ) const;
		inline pts				size( State state ) const;
		inline TextDecoration 	decoration( State state ) const;

		inline bool             isColorStatic() const { return m_bStaticColor; }
		inline bool             isBackColorStatic() const { return m_bStaticBgColor; }
		inline bool             isSizeStatic() const { return m_bStaticSize; }
		inline bool             isDecorationStatic() const { return m_bStaticDecoration; }


		//.____ Misc __________________________________________________________

		inline TextStyle_h	handle() { return m_handle; }

		void			exportAttr( State state, TextAttr * pDest, int scale ) const;
		void			addToAttr( State state, TextAttr * pDest, int scale ) const;

        bool            isStateIdentical( State state1, State state2 ) const;
		bool			isIdentical( TextStyle * pOther );
		bool			isIdenticalForState( TextStyle * pOther, State state );

		Blueprint		blueprint() const;

	protected:
		TextStyle( const Blueprint& blueprint );
		virtual ~TextStyle();

		void		_refreshSize();
		void		_refreshColor();
		void		_refreshBgColor();
		void		_refreshDecoration();

		Font_p				m_pFont;
		TextLink_p			m_pLink;
		BlendMode			m_blendMode = BlendMode::Blend;
		BlendMode			m_backBlendMode = BlendMode::Blend;

		pts					m_size[StateEnum_Nb];
		HiColor				m_color[StateEnum_Nb];
		HiColor				m_backColor[StateEnum_Nb];
		TextDecoration		m_decoration[StateEnum_Nb];

		Bitmask<uint32_t>	m_sizeSetMask = 0;
		Bitmask<uint32_t>	m_colorSetMask = 0;
		Bitmask<uint32_t>	m_backColorSetMask = 0;
		Bitmask<uint32_t>	m_decorationSetMask = 0;

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
	inline 	HiColor TextStyle::backColor( State state ) const
	{
		return m_backColor[Util::_stateToIndex(state)];
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
	inline BlendMode TextStyle::blendMode() const
	{
		return m_blendMode;
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::backBlendMode() const
	{
		return m_backBlendMode;
	}


} // namespace wg
#endif //WG3_TEXTSTYLE_DOT_H
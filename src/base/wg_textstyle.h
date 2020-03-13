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
			int				size;
			Color			color;
			BlendMode		renderMode;
			Color			bgColor;
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

		static TextStyle_p	create() { return TextStyle_p(new TextStyle()); }

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];


		//.____ Hierarchy _____________________________________________

		bool			setParent( TextStyle * pParent );
		void			cascade();

		//.____ Content _______________________________________________

		void			setFont( Font * pFont );
		void			setLink( TextLink * pLink );

		void			setColor( Color color, BlendMode operation = BlendMode::Replace );
		void			setBgColor( Color color, BlendMode operation = BlendMode::Replace );
		void			setSize( int size );
		void			setDecoration( TextDecoration decoration );
		void			setRenderMode( BlendMode mode );
		void			setBgRenderMode( BlendMode mode );

		void			setColor( Color color, State state, BlendMode operation = BlendMode::Replace );
		void			setBgColor( Color color, State state, BlendMode operation = BlendMode::Replace );
		void			setSize( int size, State state );
		void			setDecoration( TextDecoration decoration, State state );
		void			setRenderMode( BlendMode mode, State state );
		void			setBgRenderMode( BlendMode mode, State state );

		void			clearFont();
		void			clearLink();

		void			clearColor();
		void			clearBgColor();
		void			clearSize();
		void			clearDecoration();
		void			clearRenderMode();
		void			clearBgRenderMode();

		void			clearColor( State state );
		void			clearBgColor( State state );
		void			clearSize( State state );
		void			clearDecoration( State state );
		void			clearRenderMode( State state );
		void			clearBgRenderMode( State state );

		inline Font_p			font() const;
		inline TextLink_p		link() const;
		inline Color			color( State state ) const;
		inline Color			bgColor( State state ) const;
		inline int				size( State state ) const;
		inline TextDecoration 	decoration( State state ) const;
		inline BlendMode		colorBlendMode( State state ) const;
		inline BlendMode		bgColorBlendMode( State state ) const;
		inline BlendMode		renderMode( State state ) const;
		inline BlendMode		bgRenderMode( State state ) const;

		inline Font_p			combFont() const;
		inline TextLink_p		combLink() const;
		inline Color			combColor( State state ) const;
		inline Color			combBgColor( State state ) const;
		inline int				combSize( State state ) const;
		inline TextDecoration 	combDecoration( State state ) const;
		inline BlendMode		combColorBlendMode( State state ) const;
		inline BlendMode		combBgColorBlendMode( State state ) const;
		inline BlendMode		combRenderMode( State state ) const;
		inline BlendMode		combBgRenderMode( State state ) const;

        inline bool             isColorStatic() const { return m_bStaticColor; }
        inline bool             isBgColorStatic() const { return m_bStaticBgColor; }
        inline bool             isSizeStatic() const { return m_bStaticSize; }
        inline bool             isDecorationStatic() const { return m_bStaticDecoration; }
        inline bool             isRenderModeStatic() const { return m_bStaticRenderMode; }
        inline bool             isBgRenderModeStatic() const { return m_bStaticBgRenderMode; }

        
		//.____ Misc __________________________________________________________

		inline TextStyle_h	handle() { return m_handle; }

		void			exportAttr( State state, TextAttr * pDest ) const;
		void			addToAttr( State state, TextAttr * pDest ) const;

		bool			isIdentical( TextStyle * pOther );
		bool			isIdenticalForState( TextStyle * pOther, State state );
        
        TextStyle_p     clone() const;
        
	protected:
		TextStyle();
		virtual ~TextStyle();

		struct AttrSet
		{
			Font_p			pFont;
			int				size[StateEnum_Nb];
			Color			color[StateEnum_Nb];
			Color			bgColor[StateEnum_Nb];
			TextDecoration	decoration[StateEnum_Nb];
			BlendMode		renderMode[StateEnum_Nb];
			BlendMode		bgRenderMode[StateEnum_Nb];
			TextLink_p		pLink;

			BlendMode		colorBlendMode[StateEnum_Nb];		// Internal use only, operation used to blend color with color from upstream (parent or already in attr)
			BlendMode		bgColorBlendMode[StateEnum_Nb];		// Internal use only, operation used to blend bgColor with bgColor from upstream (parent or already in attr)


		};

		bool		_compareSets( AttrSet * pSet1, AttrSet * pSet2 );
		bool		_compareSetsForState( AttrSet * pSet1, AttrSet * pSet2, State state );
		bool		_refreshComb();
		void		_clearSet( AttrSet * pSet );

        bool        _isColorStatic() const;
        bool        _isBgColorStatic() const;
        bool        _isSizeStatic() const;
        bool        _isDecorationStatic() const;
        bool        _isRenderModeStatic() const;
        bool        _isBgRenderModeStatic() const;
        
		TextStyle_p	m_pParent;
		TextStyle *	m_pFirstChild;
		TextStyle *	m_pNextSibling;
		TextStyle *	m_pPrevSibling;


		AttrSet		m_specAttr;
		AttrSet		m_combAttr;

        bool        m_bStaticColor = true;         // Combined color is identical in all states.
        bool        m_bStaticBgColor = true;       // Combined background color is identical in all states.
        bool        m_bStaticSize = true;          // Combined size is identical for in states.
        bool        m_bStaticDecoration = true;    // Combined decoration is identical in all states.
        bool        m_bStaticRenderMode = true;
        bool        m_bStaticBgRenderMode = true;
        

		TextStyle_h	m_handle;
	};


	//______________________________________________________________________________
	inline Font_p TextStyle::font() const
	{
		return m_specAttr.pFont;
	}

	//______________________________________________________________________________
	inline TextLink_p TextStyle::link() const
	{
		return m_specAttr.pLink;
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::colorBlendMode( State state ) const
	{
		return m_specAttr.colorBlendMode[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline Color TextStyle::color( State state ) const
	{
		return m_specAttr.color[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::bgColorBlendMode( State state ) const
	{
		return m_specAttr.bgColorBlendMode[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline 	Color TextStyle::bgColor( State state ) const
	{
		return m_specAttr.bgColor[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline int TextStyle::size( State state ) const
	{
		return m_specAttr.size[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline TextDecoration TextStyle::decoration( State state ) const
	{
		return m_specAttr.decoration[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::renderMode( State state ) const
	{
		return m_specAttr.renderMode[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::bgRenderMode( State state ) const
	{
		return m_specAttr.bgRenderMode[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________

	inline Font_p TextStyle::combFont() const
	{
		return m_combAttr.pFont;
	}

	//______________________________________________________________________________
	inline TextLink_p TextStyle::combLink() const
	{
		return m_combAttr.pLink;
	}

	//______________________________________________________________________________
	inline Color TextStyle::combColor( State state ) const
	{
		return m_combAttr.color[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline Color TextStyle::combBgColor( State state ) const
	{
		return m_combAttr.bgColor[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline int TextStyle::combSize( State state ) const
	{
		return m_combAttr.size[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline TextDecoration TextStyle::combDecoration( State state ) const
	{
		return m_combAttr.decoration[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::combRenderMode( State state ) const
	{
		return m_combAttr.renderMode[Util::_stateToIndex(state)];
	}

	//______________________________________________________________________________
	inline BlendMode TextStyle::combBgRenderMode( State state ) const
	{
		return m_combAttr.bgRenderMode[Util::_stateToIndex(state)];
	}



} // namespace wg
#endif //WG_TEXTSTYLE_DOT_H

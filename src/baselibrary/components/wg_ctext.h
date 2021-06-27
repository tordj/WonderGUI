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

#ifndef	WG_CTEXT_DOT_H
#define WG_CTEXT_DOT_H
#pragma once

#include <wg_geocomponent.h>
#include <wg_textstyle.h>
#include <wg_textmapper.h>
#include <wg_base.h>

namespace wg
{
	class EditState;


	class CText;
	typedef	StrongComponentPtr<CText>	CText_p;
	typedef	WeakComponentPtr<CText>		CText_wp;


	//____ CText __________________________________________________________________

	class CText : public GeoComponent, protected Text
	{
		friend class TextMapper;
		friend class StdTextMapper;
	public:

		CText(Holder * pHolder );
		virtual ~CText();


		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _____________________________________________

		virtual void		setStyle(TextStyle * pStyle);
		virtual void		clearStyle();
		TextStyle_p			style() const { return m_pStyle; }

		virtual void		setTextMapper(TextMapper * pTextMapper);
		virtual void		clearTextMapper();
		TextMapper_p		textMapper() const { return m_pTextMapper; }

		//.____ State _______________________________________________

		inline State		state() const { return m_state; }

		//.____ Misc __________________________________________________

		inline CText_p		ptr() { return CText_p(this); }

	protected:

		virtual void		_clear();

		virtual void		_set(const CharSeq& seq);
		virtual void		_set(const CharBuffer * buffer);
		virtual void		_set(const String& str);

		virtual int			_append(const CharSeq& seq);
		virtual int			_insert(int ofs, const CharSeq& seq);
		virtual int			_replace(int ofs, int nDelete, const CharSeq& seq);
		virtual int			_erase(int ofs, int len);

		virtual void		_setCharStyle(TextStyle * pStyle);
		virtual void		_setCharStyle(TextStyle * pStyle, int ofs, int len);

		virtual void		_clearCharStyle();
		virtual void		_clearCharStyle(int ofs, int len);


		virtual String		_getString() const;

		inline State		_state() const { return m_state; }
		virtual void		_setState( State state );

		virtual void		_setSize( SizeSPX size, int scale );
		inline SizeSPX		_size() const { return m_size; }

		virtual SizeSPX		_preferredSize(int scale) const;
		virtual spx			_matchingWidth( spx height, int scale ) const;
		virtual spx			_matchingHeight( spx width, int scale ) const;

		virtual int			_charAtPos( CoordSPX pos ) const;
		virtual RectSPX		_charRect( int charOfs ) const;
		virtual int			_charLine( int charOfs ) const;

		virtual void		_refresh();

		virtual String		_tooltip() const;

		virtual void		_render( GfxDevice * pDevice, const RectSPX& _canvas );

		virtual RectSPX		_rectForRange( int ofs, int length ) const;

		TextMapper *		_textMapper() const { return m_pTextMapper ? m_pTextMapper.rawPtr() : Base::defaultTextMapper().rawPtr(); }
		TextStyle *			_style() const { if( m_pStyle ) return m_pStyle.rawPtr(); return Base::defaultStyle().rawPtr(); }

		// Needed by Text

		inline void			_mapperRequestRender() override { _requestRender(); }
		void				_mapperRequestRender(const RectSPX& rect) override;
		inline void			_mapperRequestResize() override { _requestResize(); }

		virtual SizeSPX		_textSize() const override { return m_size; }
		virtual State		_textState() const override { return m_state; }
		virtual TextStyle * _textStyle() const override { if( m_pStyle ) return m_pStyle.rawPtr(); return Base::defaultStyle().rawPtr(); }
		
		virtual const Char *_textBegin() const override { return m_charBuffer.chars(); }
		virtual int 		_textLength() const override { return m_charBuffer.nbChars(); }

		virtual bool		_caretVisible() const override { return false; }
		virtual int			_caretOffset() const override { return 0; }
		virtual std::tuple<int,int>	_selectedText() const override { return std::make_tuple(0,0); };		// Begin/end of selection

		int					_scale() const override { return GeoComponent::_scale(); }


	protected:

		SizeSPX				m_size;
		int					m_scale;

		State				m_state;
		TextStyle_p			m_pStyle;
		CharBuffer			m_charBuffer;
		TextMapper_p		m_pTextMapper;

		// Here so it can be shared by CTextEditor and CValueEditor

		class EditState
		{
		public:
			EditState() : bCaret(false), bShiftDown(false), bButtonDown(false), selectOfs(0), caretOfs(0), wantedOfs(-1) {}

			bool 			bCaret;			// Set if caret should be displayed.
			bool			bShiftDown;		// Set if caret is in "selection mode" modifying the selection when moving.
			bool			bButtonDown;	// Set when mouse button was pressed inside component and still is down.
			int 			selectOfs;		// Selection is between selectOfs and caretOfs.
			int				caretOfs;		// End of selection and caret offset (if displaying)
			spx				wantedOfs;		// Carets wanted offset when skipping between lines. -1 = none set.
			TextStyle_p		pCharStyle;		// Style of characters to be inserted by caret.
		};
	};


} // namespace wg
#endif //WG_CTEXT_DOT_H

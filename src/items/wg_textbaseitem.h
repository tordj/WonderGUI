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

#ifndef	WG_TEXTBASEITEM_DOT_H
#define WG_TEXTBASEITEM_DOT_H

#ifndef WG_ITEM_DOT_H
#	include <wg_item.h>
#endif

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_TEXTMAPPER_DOT_H
#	include <wg_textmapper.h>
#endif

#ifndef WG_BASE_DOT_H
#	include <wg_base.h>
#endif

namespace wg 
{
	struct EditState;
	
	//____ TextBaseItem __________________________________________________________________
	
	class TextBaseItem : public Item
	{
		friend class TextMapper;
	public:
		TextBaseItem( ItemHolder * pHolder );
		virtual ~TextBaseItem();
	
		virtual void		setStyle( const TextStyle_p& pStyle );
		virtual void		clearStyle();
		TextStyle *			style() const { return m_pStyle.rawPtr(); }
	
		virtual void		setTextMapper( const TextMapper_p& pTextMapper );
		virtual void		clearTextMapper();
		TextMapper *		textMapper() const { return m_pTextMapper.rawPtr(); }
	
		virtual void		setState( State state );
		inline State		state() const { return m_state; }

		virtual void		setSize( Size size );	
		inline Size			size() const { return m_size; }

		virtual Size		preferredSize() const;	
		virtual int			matchingWidth( int height ) const;
		virtual int			matchingHeight( int width ) const;
	
		virtual int			charAtPos( Coord pos ) const;
		virtual Rect		charRect( int charOfs ) const;
		virtual int			charLine( int charOfs ) const;
	
		virtual void		refresh();
	
		virtual String		tooltip() const;
	
		virtual void		render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip );
	
		virtual Rect		rectForRange( int ofs, int length ) const;
		
		virtual String		getString() const;
		
		virtual int			selectionBegin() const;
		virtual int			selectionEnd() const;

		// Alternative calls, guaranteed to return valid values.

		TextMapper *		_textMapper() const { return m_pTextMapper ? m_pTextMapper.rawPtr() : Base::defaultTextMapper().rawPtr(); }
		TextStyle *			_style() const { if( m_pStyle ) return m_pStyle.rawPtr(); return Base::defaultStyle().rawPtr(); }
		virtual const EditState * _editState() const;
		
	protected:
	
		Size				m_size;
	
		union 
		{
			void *			m_pTextMapperData;
			int				m_textMapperData;
		};
	
		State				m_state;
		TextStyle_p			m_pStyle;
		CharBuffer			m_charBuffer;
		TextMapper_p		m_pTextMapper;
	
	};
	
	struct EditState
	{
		bool 			bCaret;			// Set if caret should be displayed.
		bool			bShiftDown;		// Set if caret is in "selection mode" modifying the selection when moving.
		bool			bButtonDown;	// Set when mouse button was pressed inside item and still is down.
		int 			selectOfs;		// Selection is between selectOfs and caretOfs.
		int				caretOfs;		// End of selection and caret offset (if displaying)
		int				wantedOfs;		// Carets wanted offset in pixels when skipping between lines. -1 = none set.		
		TextStyle_p		pCharStyle;		// Style of characters to be inserted by caret.
	};
	
	

} // namespace wg
#endif //WG_TEXTBASEITEM_DOT_H

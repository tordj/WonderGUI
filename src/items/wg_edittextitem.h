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

#ifndef	WG_EDITTEXTITEM_DOT_H
#define WG_EDITTEXTITEM_DOT_H


#ifndef WG_TEXTITEM_DOT_H
#	include <wg_textitem.h>
#endif

namespace wg 
{
		
	//____ EditTextItem __________________________________________________________________
	
	class EditTextItem : public TextItem
	{
	public:
		EditTextItem( Widget * pWidget );

		void			receive( const Msg_p& pMsg );
	
		// Overloaded so we can update caret and selection
	
		void			clear();
	
		void			set( const CharSeq& seq );
		void			set( const CharBuffer * buffer );
		void			set( const String& str );
	
		int				append( const CharSeq& seq );
		int				insert( int ofs, const CharSeq& seq );
		int				replace( int ofs, int nDelete, const CharSeq& seq );
		int				erase( int ofs, int len );
	
		void			setState( State state );
		void			onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip );
	
	
		void			setEditMode( TextEditMode mode );
		TextEditMode	editMode() const { return m_editMode; };
		

		// These methods will fail if editMode is Static

		bool			select( int begin, int end );		// Cursor (if enabled) gets end position. End can be smaller than begin.
		bool			selectAll();
		bool			unselect();
		int				eraseSelected();
		
		int				selectionBegin() const;
		int				selectionEnd() const;

		inline bool		hasSelection() const { return m_editState.caretOfs != m_editState.selectOfs; }

		// These methods will fail unless caret is present

		bool			setCaretOfs( int ofs );			// Move cursor to offset. Any selection will be unselected.
		int				caretOfs() const;

		int				caretPut( const CharSeq& seq );	// Will insert or overwrite depending on caret mode
		bool			caretPut( uint16_t c );			// " -

		bool			caretUp();
		bool			caretDown();
		bool			caretLeft();
		bool			caretRight();
		
		bool			caretNextWord();
		bool			caretPrevWord();
		
		bool			caretEraseNextChar();
		bool			caretErasePrevChar();
		bool			caretEraseNextWord();
		bool			caretErasePrevWord();

		bool			caretLineBegin();
		bool			caretLineEnd();
		bool			caretTextBegin();
		bool			caretTextEnd();

		bool			caretToPos( Coord pos);			// Move caret as close as possible to the given position.
		bool			caretSelectWord();
		bool			caretSelectLine();				// Full line with hard line ending.


	
	
	protected:
	
		enum class MoveMethod
		{
			Keyboard,
			Mouse,
			ApiCall
		};
	
	
		bool				_moveCaret( int caretOfs, MoveMethod method );
		void				_updateDisplayArea();
		const EditState * 	_editState() const;


		bool			_caretToBegin( MoveMethod method );
		bool			_caretToEnd( MoveMethod method );
	
		TextEditMode	m_editMode;
		EditState		m_editState;
	};
	
} // namespace wg
#endif //WG_EDITTEXTITEM_DOT_H

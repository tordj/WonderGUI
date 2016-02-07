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

#ifndef	WG_EDITTEXTFIELD_DOT_H
#define WG_EDITTEXTFIELD_DOT_H


#ifndef WG_TEXTFIELD_DOT_H
#	include <wg_textfield.h>
#endif

namespace wg 
{
	
	
	//____ EditTextHolder ___________________________________________________________
	
	struct EditTextHolder : public TextHolder
	{
	};
	
	//____ EditTextField __________________________________________________________________
	
	class EditTextField : public TextField
	{
	public:
		EditTextField( EditTextHolder * pHolder );
	
		// Overloaded so we can update cursor and selection
	
		void				clear();
	
		void				set( const CharSeq& seq );
		void				set( const CharBuffer * buffer );
		void				set( const String& str );
	
		int					append( const CharSeq& seq );
		int					insert( int ofs, const CharSeq& seq );
		int					replace( int ofs, int nDelete, const CharSeq& seq );
		int					remove( int ofs, int len );
	
		void				setState( State state );
		void				onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip );
	
		//
	
		void				setEditMode( TextEditMode mode );
		TextEditMode		editMode() const;
		
		void				setCaret( const Caret2_p& pCaret );
		Caret2_p 		caret() { return m_pCaret; }
		
		int					selectionBegin() const;
		int					selectionEnd() const;
	
	
	protected:
	
		void			_cursorToEnd();
	
		TextEditMode	m_editMode;
		EditState		m_editState;
		Caret2_p		m_pCaret;
	};
	

} // namespace wg
#endif //WG_EDITTEXTFIELD_DOT_H

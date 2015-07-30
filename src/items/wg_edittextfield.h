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


//____ WgEditTextHolder ___________________________________________________________

struct WgEditTextHolder : public WgTextHolder
{
};

//____ WgEditTextField __________________________________________________________________

class WgEditTextField : public WgTextField
{
public:
	WgEditTextField( WgEditTextHolder * pHolder );

	// Overloaded so we can update cursor and selection

	void				clear();

	void				set( const WgCharSeq& seq );
	void				set( const WgCharBuffer * buffer );
	void				set( const WgString& str );

	int					append( const WgCharSeq& seq );
	int					insert( int ofs, const WgCharSeq& seq );
	int					replace( int ofs, int nDelete, const WgCharSeq& seq );
	int					remove( int ofs, int len );

	void				setState( WgState state );
	void				onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	//

	void				setEditMode( WgTextEditMode mode );
	WgTextEditMode		editMode() const;
	
	void				setCaret( const WgCaret2_p& pCaret );
	WgCaret2_p 		caret() { return m_pCaret; }
	
	int					selectionBegin() const;
	int					selectionEnd() const;


protected:

	void			_cursorToEnd();

	WgTextEditMode	m_editMode;
	WgEditState		m_editState;
	WgCaret2_p		m_pCaret;
};

#endif //WG_EDITTEXTFIELD_DOT_H

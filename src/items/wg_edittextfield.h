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

	void				Clear();

	void				Set( const WgCharSeq& seq );
	void				Set( const WgCharBuffer * buffer );
	void				Set( const WgString& str );

	int					Append( const WgCharSeq& seq );
	int					Insert( int ofs, const WgCharSeq& seq );
	int					Replace( int ofs, int nDelete, const WgCharSeq& seq );
	int					Delete( int ofs, int len );

	void				SetState( WgState state );
	void				OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );

	//

	void				SetEditMode( WgTextEditMode mode );
	WgTextEditMode		EditMode() const;
	
	void				SetCaret( const WgCaret2Ptr& pCaret );
	WgCaret2Ptr 		Caret() { return m_pCaret; }
	
	int					SelectionBegin() const;
	int					SelectionEnd() const;


protected:

	void			_cursorToEnd();

	WgTextEditMode	m_editMode;
	WgEditState		m_editState;
	WgCaret2Ptr		m_pCaret;
};

#endif //WG_EDITTEXTFIELD_DOT_H

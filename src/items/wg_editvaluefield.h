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
#ifndef	WG_EDITVALUEFIELD_DOT_H
#define WG_EDITVALUEFIELD_DOT_H

#ifndef WG_MODVALUEFIELD_DOT_H
#	include <wg_modvaluefield.h>
#endif


class WgEditValueField;

//____ WgEditValueHolder ___________________________________________________________

class WgEditValueHolder : public WgModValueHolder
{
public:
	virtual void		_onValueEdited( WgEditValueField * pField ) = 0;
	virtual WgObject*	_object() = 0;
};


//____ WgEditValueField ____________________________________________________________

class WgEditValueField : public WgModValueField
{
public:
	WgEditValueField( WgEditValueHolder * pHolder );
	~WgEditValueField();

	void				setEditMode(WgTextEditMode mode);
	inline WgTextEditMode editMode() const { return m_editMode; }

	inline bool			isEditable() const { return m_editMode == WG_TEXT_EDITABLE; }
	inline bool			isSelectable() const { return m_editMode != WG_TEXT_STATIC; }

	void				setCursorSkin( const WgCaretPtr& pCursor );
	inline WgCaretPtr	cursorSkin() const { return m_pCursorSkin; }

	int					insertAtCursor( const WgCharSeq& str );
	bool				insertAtCursor( Uint16 c );

	int					append( const WgCharSeq& seq );
	int					insert( int ofs, const WgCharSeq& seq );
	int					replace( int ofs, int nDelete, const WgCharSeq& seq );
	int					remove( int ofs, int len );
	void				deleteSelected();

	void				select( int ofs, int len );
	void				selectAll();
	inline int			selectionStart() const { return m_selBeg; }
	inline int			selectionLength() const { return m_selEnd - m_selBeg; }
	void				clearSelection();

	void				goBol();
	void				goEol();
	void				goBof();
	void				goEof();

protected:
	void				_onValueEdited() { static_cast<WgEditValueHolder*>(m_pHolder)->_onValueEdited(this); }

	WgTextEditMode		m_editMode;
	WgCaretPtr			m_pCursorSkin;
	int					m_cursorOfs;		// -1 = No cursor.
	int					m_selBeg;
	int					m_selEnd;
};


#endif //WG_EDITVALUEFIELD_DOT_H
